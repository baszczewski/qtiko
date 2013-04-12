/***********************************************************************************
* Qtiko.
* Copyright (C) 2012 Marcin Baszczewski <marcin@baszczewski.pl>
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 2
* of the License, or (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*
***********************************************************************************/

#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QFile>
#include <QDateTime>
#include <QCryptographicHash>
#include <QtSql>

#include "library_collection.h"
#include "library_track.h"
#include "library_last_fm.h"

LibraryCollection::LibraryCollection(QObject *parent)
    : QObject(parent)
    , m_lastFM(0)
{
    m_release = false;

    m_files = 0;
    m_blacklist << "jpg" << "png" << "gif" << "bmp" << "ini" << "txt" << "doc" << "rtf" << "pdf" << "db" << "log" << "zip" << "rar" << "dmg" << "iso" << "m3u" << "pls" << "cue";

    m_databaseFile = "database.db";

    if (database().isOpen())
        return;

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", m_databaseFile);
    db.setDatabaseName(m_databaseFile);

    if (!db.open() || !db.isValid())
    {
        qDebug() << "Database not available.";
    }
}

LibraryCollection::~LibraryCollection()
{
    qDebug() << "close collection";
}

void LibraryCollection::clear()
{
    QSqlDatabase db = database();

    db.transaction();
    QSqlQuery("UPDATE details SET hash='' WHERE name='hash'", db);
    QSqlQuery("DELETE FROM tracks", db);
    QSqlQuery("DELETE FROM albums", db);
    QSqlQuery("DELETE FROM artists", db);
    db.commit();
}

void LibraryCollection::rescan(const QString &directory)
{
    m_files = 0;
    m_scaned = 0;
    m_libraryDirectory = directory;

    QSqlQuery q1("SELECT value FROM details WHERE name='hash'", database());
    if (!q1.next())
    {
        qDebug() << "Hash not available";
        return;
    }

    const QString oldHash = q1.value(0).toString();
    const QString currentHash = directoryHash(QDir(directory), m_files);

    //sleep(1);

    m_hash = oldHash;

    emit collectionHashed(m_files);

    if (oldHash != currentHash)
    {
        m_lastFM = new LibraryLastFM(this);
        connect(m_lastFM, SIGNAL(ready()), SLOT(downloadCover()));

        tracksTempTable();
        m_hash = scanDirectory(QDir(directory));
        removeMissing();
        removeEmpty();

        m_toDownload = coversToDownload();

        downloadCover();

        return;
    }

    finish();
    emit collectionDone();
}

QString LibraryCollection::directoryHash(const QDir &directory, int &files, const QString &hash) const
{
    QDir tempDirectory = directory;
    QString tempHash = hash;

    tempDirectory.setFilter(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot | QDir::Readable);

    const QFileInfoList list = tempDirectory.entryInfoList();

    // catalogs first
    for (int i = 0; i < list.size(); ++i)
    {
        const QFileInfo info = list.at(i);
        if (info.isDir())
        {
            tempHash += directoryHash(QDir(info.absoluteFilePath()), files, tempHash);
        }
    }

    // tracks next
    for (int i = 0; i < list.size(); ++i)
    {
        const QFileInfo info = list.at(i);
        if (!info.isDir())
        {
            files++;
            const uint lastModified = info.lastModified().toTime_t();
            tempHash += QString::number(lastModified);
            tempHash += info.absoluteFilePath();
        }
    }

    return this->hash(tempHash);
}

QString LibraryCollection::scanDirectory(const QDir &directory, const QString &hash)
{
    if (m_release)
    {
        return "";
    }

    QDir tempDirectory = directory;
    QString tempHash = hash;
    QSqlDatabase db = database();

    tempDirectory.setFilter(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot | QDir::Readable);

    const QFileInfoList list = tempDirectory.entryInfoList();

    //dirs
    for (int i = 0; i < list.size(); ++i)
    {
        const QFileInfo info = list.at(i);
        if (info.isDir())
        {
            tempHash += scanDirectory(QDir(info.absoluteFilePath()), tempHash);
        }
    }

    //files
    db.transaction();
    for (int i = 0; i < list.size(); ++i)
    {
        const QFileInfo info = list.at(i);
        if (!info.isDir())
        {
            const uint lastModified = info.lastModified().toTime_t();
            const QString currentHash = QString::number(lastModified) + info.absoluteFilePath();

            tempHash += currentHash;

            QSqlQuery q0(db);
            q0.prepare("DELETE FROM t__tracks WHERE url=?");
            q0.addBindValue(info.absoluteFilePath());
            q0.exec();

            QSqlQuery q1(db);
            q1.prepare("SELECT hash, id FROM tracks WHERE url=?");
            q1.addBindValue(info.absoluteFilePath());
            q1.exec();

            if (q1.next()) // item found
            {
                if (q1.value(0).toString()!=this->hash(currentHash)) // update
                {
                    QSqlQuery q2(db);
                    q2.prepare("DELETE FROM tracks WHERE id=?");
                    q2.addBindValue(q1.value(1).toInt());
                    q2.exec();

                    if (!m_blacklist.contains(info.suffix().toLower()) || info.size()>(1024 * 1024 * 1024))
                        processFile(info.absoluteFilePath(), currentHash);
                }
            }
            else if (!m_blacklist.contains(info.suffix().toLower()) || info.size()>(1024 * 1024 * 1024))
                processFile(info.absoluteFilePath(), currentHash);

            m_scaned++;
            emit collectionProgress(((qreal)m_scaned/m_files)/2);
        }
    }
    db.commit();

    return this->hash(tempHash);
}

void LibraryCollection::processFile(const QString &path, const QString &hash)
{
    LibraryTrack about(path, m_libraryDirectory);

    if (!about.isValid())
        return;

    QSqlDatabase db = database();

    int albumID = 0;
    int artistID = 0;

    QSqlQuery q2(db);
    q2.prepare("SELECT id FROM albums WHERE helper=?");
    q2.addBindValue(about.album().toLower());
    q2.exec();

    if (q2.next())
        albumID = q2.value(0).toInt();
    else
    {
        QSqlQuery qi(db);
        qi.prepare("INSERT INTO albums(title, helper, year) VALUES(?, ?, ?)");
        qi.addBindValue(about.album());
        qi.addBindValue(about.album().toLower());
        qi.addBindValue(about.year());
        qi.exec();
        albumID = qi.lastInsertId().toInt();

        m_albumsQueue.insert(albumID, about.album());
    }

    QSqlQuery q4(db);
    q4.prepare("SELECT id FROM artists WHERE helper=?");
    q4.addBindValue(about.artist().toLower());
    q4.exec();

    if (q4.next())
        artistID = q4.value(0).toInt();
    else
    {
        QSqlQuery qi(db);
        qi.prepare("INSERT INTO artists(title, helper) VALUES(?, ?)");
        qi.addBindValue(about.artist());
        qi.addBindValue(about.artist().toLower());
        qi.exec();
        artistID = qi.lastInsertId().toInt();

        m_artistsQueue.insert(artistID, about.artist());
    }


    QSqlQuery q3(db);
    q3.prepare("INSERT INTO tracks(url, hash, title, helper, year, number, length, quality, album_id, artist_id) VALUES(:url, :hash, :title, :helper, :year, :number, :length, :quality, :album, :artist)");
    q3.bindValue(":url", path);
    q3.bindValue(":hash", this->hash(hash));
    q3.bindValue(":title", about.title());
    q3.bindValue(":helper", about.helper());
    q3.bindValue(":year", about.year());
    q3.bindValue(":number", about.number());
    q3.bindValue(":length", about.length());
    q3.bindValue(":quality", about.quality());

    if (albumID)
        q3.bindValue(":album", albumID);
    else
        q3.bindValue(":album", QVariant());

    if (artistID)
        q3.bindValue(":artist", artistID);
    else
        q3.bindValue(":artist", QVariant());

    q3.exec();
}

QString LibraryCollection::hash(const QString &value) const
{
    return QString::fromAscii(QCryptographicHash::hash(value.toUtf8(), QCryptographicHash::Md5).toHex());
}

QSqlDatabase LibraryCollection::database() const
{
    return QSqlDatabase::database(m_databaseFile);
}

void LibraryCollection::release()
{
    m_release = true;

    emit finished();
}

void LibraryCollection::tracksTempTable()
{
    execute(
        "CREATE TEMP TABLE IF NOT EXISTS t__tracks ("
        "'id' INTEGER,"
        "'url' TEXT"
        ")");

    execute("INSERT INTO t__tracks(id, url) SELECT id, url FROM tracks");
}

void LibraryCollection::removeMissing()
{
    QSqlDatabase db = database();

    db.transaction();

    QSqlQuery q1("SELECT id FROM t__tracks", db);
    while(q1.next())
    {
        QSqlQuery q2(db);
        q2.prepare("DELETE FROM tracks WHERE id=?");
        q2.addBindValue(q1.value(0).toInt());
        q2.exec();
    }
    execute("DELETE FROM t__tracks");

    db.commit();
}

void LibraryCollection::removeEmpty()
{
    // TO DO: remove empty artists, folders and albums
}

bool LibraryCollection::execute(const QString &sql) const
{
    QSqlQuery q1(sql, database());

    if (q1.lastError().isValid())
    {
        qDebug() << "execute error" << q1.lastError().text();
        return false;
    }
    return true;
}

bool LibraryCollection::isEmpty() const
{
    QSqlQuery q1("SELECT value FROM details WHERE name='hash'", database());
    if (!q1.next())
    {
        qDebug() << "Hash not available";
        return true;
    }

    const QString hash = q1.value(0).toString();

    return hash.isEmpty();
}

void LibraryCollection::downloadCover()
{
    if (m_lastFM->isBusy() && m_release)
        return;

    if (m_artistsQueue.count())
    {
        if (m_albumsQueue.count())
        {
            int id = m_albumsQueue.keys().first();
            QString name = m_albumsQueue.values().first();
            m_albumsQueue.take(id);
            m_lastFM->findAlbumCover(id, name);
        }
        else if (m_artistsQueue.count())
        {
            int id = m_artistsQueue.keys().first();
            QString name = m_artistsQueue.values().first();
            m_artistsQueue.take(id);
            m_lastFM->findArtistCover(id, name);
        }

        const qreal p = (qreal)(m_toDownload - coversToDownload())/m_toDownload;
        emit collectionProgress(0.5+p/2);
    }

    if (!coversToDownload())
    {
        finish();
        emit collectionDone();
    }
}

int LibraryCollection::coversToDownload() const
{
    return m_artistsQueue.count()+m_albumsQueue.count();
}

void LibraryCollection::finish()
{
    emit collectionProgress(1);

    if (!m_release)
    {
        QSqlQuery q2(database());
        q2.prepare("UPDATE details SET value=? WHERE name='hash'");
        q2.addBindValue(m_hash);
        q2.exec();
    }
    qDebug() << "------------- DONE SCAN: " << m_hash;
}
