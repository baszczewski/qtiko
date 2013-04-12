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

#ifndef COVER_H
#define COVER_H

#include <QVariant>

enum CoverType
{
    CoverAlbum,
    CoverArtist,
    CoverFolder,
    CoverOther
};
Q_DECLARE_METATYPE(CoverType)

struct Cover
{
    int id;
    QString title;
    QString subtitle;
    QString image;
    qreal animation;
    CoverType type;

    Cover()
    {
        id = 0;
        title = subtitle = image = "";
        animation = 0;
        type = CoverAlbum;
    }

    operator QVariant() const
    {
        return QVariant::fromValue(*this);
    }
};
Q_DECLARE_METATYPE(Cover)
typedef QList<Cover> Covers;
Q_DECLARE_METATYPE(Covers)

#endif // COVER_H
