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
#include <QRegExp>

#include "library_track.h"

// TagLib
#include "fileref.h"
#include "tag.h"

LibraryTrack::LibraryTrack(const QString &fileName, const QString &libraryDirectory)
{
    TagLib::FileRef fileRef((TagLib::FileName)fileName.toUtf8());

    m_libraryDirectory = libraryDirectory+"/";

    m_tags = true;
    m_title = m_album = m_artist = m_helper = "";
    m_year = m_number = m_length = m_quality = 0;

    if (!fileRef.isNull())
    {
        TagLib::AudioProperties *audio = fileRef.audioProperties();
        TagLib::Tag *tag = fileRef.tag();

        if (tag && audio)
        {
            m_title = QString::fromUtf8(tag->title().toCString(true));
            m_album = QString::fromUtf8(tag->album().toCString(true));
            m_artist = QString::fromUtf8(tag->artist().toCString(true));
            m_year = tag->year();
            m_number = tag->track();
            m_length = audio->length();
            m_quality = audio->bitrate();

            if (m_title.isEmpty() || m_artist.isEmpty() || m_album.isEmpty())
                findMissingData(fileName);

            m_artist = clean(m_artist);
            m_album = clean(m_album);
            m_title = clean(m_title);

            m_helper = m_title+m_artist+m_album;
            m_helper = m_helper.toLower().trimmed();

            m_valid = true;
        }
    }
    else
    {
        m_valid = false;
    }
}

QString LibraryTrack::title() const
{
    return m_title;
}

QString LibraryTrack::helper() const
{
    return m_helper;
}

QString LibraryTrack::album() const
{
    return m_album;
}

QString LibraryTrack::artist() const
{
    return m_artist;
}

int LibraryTrack::year() const
{
    return m_year;
}

int LibraryTrack::number() const
{
    return m_number;
}

int LibraryTrack::length() const
{
    return m_length;
}

int LibraryTrack::quality() const
{
    return m_quality;
}

bool LibraryTrack::isValid() const
{
    return m_valid;
}

void LibraryTrack::findMissingData(const QString &fileName)
{
    m_tags = false;

    QString path = fileName;
    path.remove(0, m_libraryDirectory.length());

    if (path.contains("/"))
    {
        Pair base = pair(path, "/");

        if (base.first.contains("-")) // "artist - album/(files)"
        {
            Pair artistAndAlbum = pair(base.first, "-");

            if (m_artist.isEmpty())
                m_artist = artistAndAlbum.first;

            if (m_album.isEmpty())
                m_album = artistAndAlbum.second;

            QString title = base.second;

            QRegExp rx("([0-9]{1,}).{1,}");
            int pos = rx.indexIn(title);
            if (pos > -1)
            {
                QString number = rx.cap(1);
                m_number = number.toInt();

                title = title.remove(0, number.length()).trimmed();
                if (title.indexOf(".")==0 || title.indexOf("-")==0 || title.indexOf("_")==0)
                    title.remove(0,1);
            }

            if (title.lastIndexOf(".")!=-1)
            {
                title = title.left(title.lastIndexOf("."));
            }

            if (m_title.isEmpty())
                m_title = title;

        }
        else // "artist/(files)"
        {
            if (m_artist.isEmpty())
                m_artist = base.first;

            QString title = base.second;

            QRegExp rx("([0-9]{1,}).{1,}");
            int pos = rx.indexIn(title);
            if (pos > -1)
            {
                QString number = rx.cap(1);
                m_number = number.toInt();

                title = title.remove(0, number.length()).trimmed();
                if (title.indexOf(".")==0 || title.indexOf("-")==0 || title.indexOf("_")==0)
                    title.remove(0,1);
            }

            if (title.lastIndexOf(".")!=-1)
            {
                title = title.left(title.lastIndexOf("."));
            }

            if (m_title.isEmpty())
                m_title = title;
        }

    }
}

bool LibraryTrack::tagsComplete() const
{
    return m_tags;
}

Pair LibraryTrack::pair(const QString &text, const QString &symbol) const
{
    QString pathA = text.left(text.indexOf(symbol));
    QString pathB = text.right(text.length()-pathA.length()-symbol.length());

    return PairCreate(pathA, pathB);
}

QString LibraryTrack::clean(const QString &text)
{
    QString temp = text.trimmed();

    if (temp.endsWith("'") && temp.startsWith("'"))
    {
        temp.remove("'");
        temp = temp.trimmed();
    }
    else if (temp.startsWith("'"))
    {
        temp.remove(0,1);
        temp = temp.trimmed();
    }

    return temp;
}
