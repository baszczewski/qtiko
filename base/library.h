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

#ifndef LIBRARY_H
#define LIBRARY_H

#include <QObject>
#include <QVariant>

#include "types/track.h"
#include "types/cover.h"

class LibraryWorker;
class LibraryCollection;

class Library: public QObject
{
    Q_OBJECT
public:
    explicit Library(QObject *parent = 0);
    ~Library();

    void rescan();
    void clear();

    void getTracks(const QString &filter = QString(), const int &limit = -1);
    void getTracks(const QString &filter = QString(), const CoverType &type = CoverAlbum, const int parent = -1, const int &limit = -1);
    void getCovers(const QString &filter = QString(), const CoverType &type = CoverAlbum, const int &limit = -1);

    void more(const int &limit = -1);

    void setDirectory(const QString &directory);

    bool isEmpty();

protected slots:
    void busy(const bool &value);

signals:
    void tracksReceived(const Tracks &tracks);
    void coversReceived(const Covers &covers);

    void itemsCountReceived(const int &count);

    void error(const QString &message);

    void collectionHashed(const int &filesCount);
    void collectionProgress(const qreal &value);
    void collectionDone();

    void getTracksFWD(const QString &filter, const CoverType &type, const int parent, const int &limit);
    void getCoversFWD(const QString &filter, const CoverType &type, const int &limit);
    void moreFWD(const int &limit);
    void clearFWD();
    void rescanFWD(const QString &directory);

private:
    LibraryWorker *m_worker;
    LibraryCollection *m_collection;

    QThread *m_workerThread;
    QThread *m_collectionThread;

    bool m_workerBusy;

    QString m_directory;
};

#endif // LIBRARY_H
