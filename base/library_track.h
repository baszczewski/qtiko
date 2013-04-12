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

#ifndef LIBRARY_TRACK_H
#define LIBRARY_TRACK_H

#include <QString>
#include <QPair>

typedef QPair<QString, QString> Pair;
#define PairCreate(a, b) qMakePair<QString, QString>(a, b)

class LibraryTrack
{
public:
    LibraryTrack(const QString &fileName, const QString &libraryDirectory = "");

    QString title() const;
    QString album() const;
    QString artist() const;
    QString helper() const;
    int year() const;
    int number() const;
    int length() const;
    int quality() const;

    bool isValid() const;
    bool tagsComplete() const;

protected:
    void findMissingData(const QString &fileName);
    Pair pair(const QString &text, const QString &symbol = "/") const;
    QString clean(const QString &text);

private:
    QString m_title;
    QString m_helper;
    QString m_album;
    QString m_artist;
    QString m_libraryDirectory;

    int m_year;
    int m_number;
    int m_length;
    int m_quality;

    bool m_valid;
    bool m_tags;
};

#endif // LIBRARY_TRACK_H
