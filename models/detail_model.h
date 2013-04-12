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

#ifndef DETAIL_MODEL_H
#define DETAIL_MODEL_H

#include <QAbstractTableModel>

#include "types/playlist.h"
#include "types/track.h"
#include "types/cover.h"

class Library;

class DetailModel : public QAbstractTableModel, public Playlist
{
    Q_OBJECT
public:
    explicit DetailModel(QObject *parent = 0);

    int rowCount(const QModelIndex & parent = QModelIndex()) const;
    int columnCount(const QModelIndex & parent = QModelIndex()) const;
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

    void play(const QModelIndex &index);
    void stop();

    void clear();
    void getTracks(const QString &filter = QString(), const int &limit = -1);
    void getTracks(const QString &filter = QString(), const CoverType &type = CoverAlbum, const int parent = -1, const int &limit = -1);

    void setCurrentItemState(const int &state);

    QVariant currentItem();
    QVariant nextItem();
    QVariant previousItem();
    QVariant randomItem();

public slots:
    void filter(const QString &text);
    void trackSelected(const QModelIndex &index);

protected:
    bool canFetchMore(const QModelIndex &parent) const;
    void fetchMore(const QModelIndex &parent);

protected slots:
    void insert(const Tracks &tracks);
    void itemsCountReceived(const int &count);

private:
    Library *m_library;
    Tracks m_tracks;
    int m_count;
    int m_play;
    bool m_clear;
    bool m_active;
};

#endif // DETAIL_MODEL_H
