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

#ifndef PLAYLIST_H
#define PLAYLIST_H

#include <QMap>
#include <QVariant>

class Track;

class Playlist
{
public:
    virtual QVariant currentItem() { return QVariant(); }
    virtual QVariant nextItem() { return QVariant(); }
    virtual QVariant previousItem() { return QVariant(); }

    virtual void setCurrentItemState(const int &state) = 0;

protected:
    virtual void trackSelected(const QModelIndex &index) = 0;
};

#endif // PLAYLIST_H
