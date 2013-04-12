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

#ifndef LIBRARY_WORKER_H
#define LIBRARY_WORKER_H

#include <QObject>

#include "types/cover.h"
#include "types/track.h"

class QSqlDatabase;
class QSqlQuery;

enum GetType
{
    TypeTracks,
    TypeCovers
};

class LibraryWorker : public QObject
{
    Q_OBJECT
public:
    explicit LibraryWorker(QObject *parent = 0);
    ~LibraryWorker();

public slots:
    void getTracks(const QString &filter, const CoverType &type, const int parent, const int &limit);
    void getCovers(const QString &filter, const CoverType &type, const int &limit);
    void more(const int &limit);
    void release();

signals:
    void tracksReceived(const Tracks &tracks);
    void coversReceived(const Covers &covers);

    void itemsCountReceived(const int &count);

    void finished();
    void busy(const bool &value);

protected:
    QSqlDatabase database() const;
    void beginBusy();
    void endBusy();

private:
    QString m_databaseFile;
    QSqlQuery *m_query;

    GetType m_type;
    CoverType m_coverType;

    int m_pos;
};

#endif // LIBRARY_WORKER_H
