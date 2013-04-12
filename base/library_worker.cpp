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

#include <QtSql>
#include <QDebug>

#include "library_worker.h"

LibraryWorker::LibraryWorker(QObject *parent)
    : QObject(parent)
    , m_query(0)
{
    m_databaseFile = "database.db";

    // TO DO: unique connection for all threads

    if (database().isOpen())
        return;

    if (!QFile(m_databaseFile).exists())
    {
        QFile(":/database.db").copy(m_databaseFile);
        QFile(m_databaseFile).setPermissions(QFile::WriteOwner | QFile::ReadOwner);
    }

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", m_databaseFile);
    db.setDatabaseName(m_databaseFile);

    if (!db.open() || !db.isValid())
    {
        qDebug() << "Database not available.";
    }
}

LibraryWorker::~LibraryWorker()
{
    if (m_query)
    {
        delete m_query;
    }
    qDebug() << "close worker";
}

QSqlDatabase LibraryWorker::database() const
{
    return QSqlDatabase::database(m_databaseFile);
}

void LibraryWorker::getTracks(const QString &filter, const CoverType &type, const int parent, const int &limit)
{
    emit busy(true);

    m_type = TypeTracks;
    int count = 0;

    if (type==CoverOther) // TO DO - filters and more - temporary
    {
        if (parent==-1)
        {
            QSqlQuery query("SELECT COUNT(*) FROM tracks", database());
            query.next();
            count = query.value(0).toInt();
            emit itemsCountReceived(query.value(0).toInt());
        }
        else
        {
            QSqlQuery query(QString("SELECT COUNT(*) FROM other_details WHERE otherCover_id=%1").arg(parent), database());
            query.next();
            count = query.value(0).toInt();
            emit itemsCountReceived(query.value(0).toInt());
        }

        if (m_query)
            delete m_query;

        m_pos = 0;

        if (parent==-1)
        {
            m_query = new QSqlQuery("SELECT t.*, '' AS album, '' as artist FROM other_details t ORDER BY t.title ASC", database());
        }
        else
        {
            m_query = new QSqlQuery(QString("SELECT t.*, '' AS album, '' as artist FROM other_details t WHERE otherCover_id=%1 ORDER BY t.title ASC").arg(parent), database());
        }

        emit busy(false);
        more(qMin(limit, count));
        return;
    }

    QString column;
    switch(type)
    {
    case CoverAlbum:
        column = "album_id";
        break;
    case CoverArtist:
        column = "artist_id";
        break;
    case CoverFolder:
        column = "folder_id";
        break;
    default:
        column = "album_id";
        break;
    }

    if (parent==-1)
    {
        if (filter.isEmpty())
        {
            QSqlQuery query("SELECT COUNT(*) FROM tracks", database());
            query.next();
            count = query.value(0).toInt();
            emit itemsCountReceived(query.value(0).toInt());
        }
        else
        {
            QSqlQuery query(QString("SELECT COUNT(*) FROM tracks WHERE helper LIKE '%%1%'").arg(filter), database());
            query.next();
            count = query.value(0).toInt();
            emit itemsCountReceived(query.value(0).toInt());
        }
    }
    else
    {
        if (filter.isEmpty())
        {
            QSqlQuery query(QString("SELECT COUNT(*) FROM tracks WHERE %1=%2").arg(column).arg(parent), database());
            query.next();
            count = query.value(0).toInt();
            emit itemsCountReceived(query.value(0).toInt());
        }
        else
        {
            QSqlQuery query(QString("SELECT COUNT(*) FROM tracks WHERE helper LIKE '%%1%' AND %2=%3").arg(filter).arg(column).arg(parent), database());
            query.next();
            count = query.value(0).toInt();
            emit itemsCountReceived(query.value(0).toInt());
        }
    }

    if (m_query)
        delete m_query;

    m_pos = 0;

    if (parent==-1)
    {
        if (filter.isEmpty())
            m_query = new QSqlQuery("SELECT t.*, a.title AS album, b.title as artist FROM tracks t LEFT JOIN albums a ON a.id=t.album_id LEFT JOIN artists b ON b.id=t.artist_id ORDER BY t.helper ASC", database());
        else
            m_query = new QSqlQuery(QString("SELECT t.*, a.title AS album, b.title as artist FROM tracks t LEFT JOIN albums a ON a.id=t.album_id LEFT JOIN artists b ON b.id=t.artist_id WHERE t.helper LIKE '%%1%' ORDER BY t.helper ASC").arg(filter), database());
    }
    else
    {
        if (filter.isEmpty())
            m_query = new QSqlQuery(QString("SELECT t.*, a.title AS album, b.title as artist FROM tracks t LEFT JOIN albums a ON a.id=t.album_id LEFT JOIN artists b ON b.id=t.artist_id WHERE %1=%2 ORDER BY t.helper ASC").arg(column).arg(parent), database());
        else
            m_query = new QSqlQuery(QString("SELECT t.*, a.title AS album, b.title as artist FROM tracks t LEFT JOIN albums a ON a.id=t.album_id LEFT JOIN artists b ON b.id=t.artist_id WHERE t.helper LIKE '%%1%' AND %2=%3 ORDER BY t.helper ASC").arg(filter).arg(column).arg(parent), database());
    }

    emit busy(false);

    more(qMin(limit, count));
}

void LibraryWorker::getCovers(const QString &filter, const CoverType &type, const int &limit)
{
    emit busy(true);

    m_type = TypeCovers;
    m_coverType = type;

    int count = 0;

    QString table;

    if (type==CoverOther)
    {
        if (filter.isEmpty())
        {
            QSqlQuery query("SELECT COUNT(*) FROM other_covers", database());
            query.next();
            count = query.value(0).toInt();
            emit itemsCountReceived(query.value(0).toInt());
        }

        if (m_query)
            delete m_query;

        m_pos = 0;

        m_query = new QSqlQuery("SELECT id, title FROM other_covers ORDER BY title ASC", database());
    }
    else
    {

        switch(type)
        {
        case CoverAlbum:
            table = "albums";
            break;
        case CoverArtist:
            table = "artists";
            break;
        case CoverFolder:
            table = "folders";
            break;
        default:
            table = "albums";
            break;
        }

        if (filter.isEmpty())
        {
            QSqlQuery query(QString("SELECT COUNT(*) FROM %1").arg(table), database());
            query.next();
            count = query.value(0).toInt();
            emit itemsCountReceived(query.value(0).toInt());
        }
        else
        {
            QSqlQuery query(QString("SELECT COUNT(*) FROM %1 WHERE title LIKE '%%2%'").arg(table).arg(filter), database());
            query.next();
            count = query.value(0).toInt();
            emit itemsCountReceived(query.value(0).toInt());
        }

        if (m_query)
            delete m_query;

        m_pos = 0;

        if (filter.isEmpty())
            m_query = new QSqlQuery(QString("SELECT id, title FROM %1 ORDER BY title ASC").arg(table), database());
        else
            m_query = new QSqlQuery(QString("SELECT id, title FROM %1 WHERE title LIKE '%%2%' ORDER BY title ASC").arg(table).arg(filter), database());
    }

    emit busy(false);

    more(qMin(limit, count));
}

void LibraryWorker::more(const int &limit)
{
    beginBusy();

    qDebug() << "more" << limit;

    if (!m_query->isActive())
        return;

    if (m_type==TypeTracks)
    {
        Tracks temp;

        int count = limit;
        while( (count || limit==-1) && m_query->isSelect() && m_query->isActive())
        {
            m_query->seek(m_pos++);
            QSqlRecord r = m_query->record();

            Track track;
            track.title = r.value("title").toString();
            track.artist = r.value("artist").toString();
            track.album = r.value("album").toString();
            track.url = r.value("url").toString();
            track.length = r.value("length").toInt();
            track.state = Track::TrackStateNone;

            temp.append(track);

            if (limit!=-1)
                count--;
        }

        if (temp.count())
            emit tracksReceived(temp);
    }

    if (m_type==TypeCovers)
    {
        Covers temp;

        int count = limit;
        while( (count || limit==-1) && m_query->isSelect() && m_query->isActive())
        {
            m_query->seek(m_pos++);
            QSqlRecord r = m_query->record();

            Cover cover;
            cover.id = r.value("id").toInt();
            cover.title = r.value("title").toString();
            cover.type = m_coverType;

            if (m_coverType==CoverArtist)
            {
                if (QFile::exists(QString("artist/%1.png").arg(r.value("id").toInt())))
                    cover.image = QString("artist/%1.png").arg(r.value("id").toInt());
            }
            if (m_coverType==CoverAlbum)
            {
                if (QFile::exists(QString("album/%1.png").arg(r.value("id").toInt())))
                    cover.image = QString("album/%1.png").arg(r.value("id").toInt());
            }

            temp.append(cover);

            if (limit!=-1)
                count--;
        }

        if (temp.count())
            emit coversReceived(temp);
    }

    endBusy();
}

void LibraryWorker::beginBusy()
{
    emit busy(true);
}

void LibraryWorker::endBusy()
{
    emit busy(false);
}

void LibraryWorker::release()
{
    emit finished();
}
