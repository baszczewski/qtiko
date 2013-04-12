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

#ifndef LIBRARY_COLLECTION_H
#define LIBRARY_COLLECTION_H

#include <QObject>
#include <QStringList>
#include <QHash>

class QDir;
class QSqlDatabase;
class LibraryLastFM;

class LibraryCollection : public QObject
{
    Q_OBJECT
public:
    explicit LibraryCollection(QObject *parent = 0);
    ~LibraryCollection();

    bool isEmpty() const;

public slots:
    void release();
    void rescan(const QString &directory);
    void clear();

protected slots:
    void downloadCover();

protected:
    QSqlDatabase database() const;
    QString hash(const QString &value) const;
    QString directoryHash(const QDir &directory, int &files, const QString &hash = QString()) const;
    QString scanDirectory(const QDir &directory, const QString &hash = QString());
    void processFile(const QString &path, const QString &hash = QString());
    void tracksTempTable();
    void removeMissing();
    void removeEmpty();
    bool execute(const QString &sql) const;
    int coversToDownload() const;
    void finish();

signals:
    void collectionHashed(const int &filesCount);
    void collectionProgress(const qreal &value);
    void collectionDone();
    void finished();

private:
    LibraryLastFM *m_lastFM;

    QString m_libraryDirectory;
    QString m_databaseFile;
    QString m_hash;
    bool m_release;
    QStringList m_blacklist;
    int m_files;
    int m_scaned;
    int m_toDownload;


    QHash<int, QString> m_artistsQueue; // for covers
    QHash<int, QString> m_albumsQueue; // for covers
};

#endif // LIBRARY_COLLECTION_H
