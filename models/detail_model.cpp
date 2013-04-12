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
#include <QTime>
#include <qmath.h>

#include "core/singleton.h"

#include "models/detail_model.h"

#include "base/library.h"
#include "base/player.h"

DetailModel::DetailModel(QObject *parent)
    : QAbstractTableModel(parent)
    , m_library(0)
{
    qRegisterMetaType<Tracks>();

    m_library = new Library(this);

    m_count = 0;
    m_play = -1;
    m_clear = false;

    connect(m_library, SIGNAL(itemsCountReceived(int)), SLOT(itemsCountReceived(int)));
    connect(m_library, SIGNAL(tracksReceived(Tracks)), SLOT(insert(Tracks)));
}

void DetailModel::getTracks(const QString &filter, const int &limit)
{
    clear();
    m_library->getTracks(filter, limit);
}

void DetailModel::getTracks(const QString &filter, const CoverType &type, const int parent, const int &limit)
{
    clear();
    m_library->getTracks(filter, type, parent, limit);
}

QVariant DetailModel::currentItem()
{
    if (m_play==-1)
    {
        if (Singleton::instance()->shuffle())
            return randomItem();

        if (m_tracks.count())
        {
            m_play = 0;
            return m_tracks.first();
        }
    }
    else
    {
        QModelIndex index = this->index(m_play,0);
        if (index.isValid())
        {
            if (index.data(Qt::UserRole).canConvert<Track>())
            {
                Track track = index.data(Qt::UserRole).value<Track>();
                return track;
            }
        }
    }

    return Track();
}

QVariant DetailModel::nextItem()
{
    if (Singleton::instance()->shuffle())
        return randomItem();

    if (m_play==-1)
    {
        if (m_tracks.count())
        {
            m_play = 0;
            return m_tracks.first();
        }
    }
    else
    {
        if (m_play>=(m_tracks.count()-10) && m_count!=m_tracks.count())
            fetchMore(QModelIndex());

        QModelIndex index = this->index(m_play+1,0);
        if (index.isValid())
        {
            m_play++;
            if (index.data(Qt::UserRole).canConvert<Track>())
            {
                Track track = index.data(Qt::UserRole).value<Track>();
                return track;
            }
        }
        else
        {
            if (Singleton::instance()->repeat() && m_tracks.count())
            {
                m_play = 0;
                return m_tracks.first();
            }
        }
    }

    return Track();
}

QVariant DetailModel::previousItem()
{
    if (Singleton::instance()->shuffle())
        return randomItem();

    if (m_play!=-1)
    {
        QModelIndex index = this->index(m_play-1,0);
        if (index.isValid())
        {
            m_play--;
            if (index.data(Qt::UserRole).canConvert<Track>())
            {
                Track track = index.data(Qt::UserRole).value<Track>();
                return track;
            }
        }
        else
            m_play = -1;
    }
    return Track();
}

QVariant DetailModel::randomItem()
{
    return Track();
}

void DetailModel::setCurrentItemState(const int &state)
{
    if (m_play==-1)
        return;

    QModelIndex index = this->index(m_play,0);
    Track track = index.data(Qt::UserRole).value<Track>();
    track.state = (Track::TrackState)state;
    setData(index, track);
}

void DetailModel::itemsCountReceived(const int &count)
{
    if (m_clear && count == 0) // got result - clear (prevent blinking)
        clear();

    m_count = count;
}

int DetailModel::rowCount(const QModelIndex & parent) const
{
    Q_UNUSED(parent);
    return m_tracks.count();
}

int DetailModel::columnCount(const QModelIndex & parent) const
{
    Q_UNUSED(parent);
    return 4;
}

bool DetailModel::canFetchMore(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    if (m_tracks.count() < m_count)
        return true;
    else
        return false;
}

void DetailModel::fetchMore(const QModelIndex &parent)
{
    Q_UNUSED(parent);

    int remainder = m_count - m_tracks.count();
    int itemsToFetch = qMin(qMax(1024, m_count/3), remainder);

    m_library->more(itemsToFetch);
}

QVariant DetailModel::data(const QModelIndex & index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (index.row() >= m_tracks.count() || index.row() < 0)
        return QVariant();

    if (role == Qt::DisplayRole)
    {
        switch(index.column())
        {
            case 0:
            {
                const QString title = m_tracks.at(index.row()).title;
                return title.isEmpty()?tr("Unkown"):title;
            }
            case 1:
            {
                const QString artist = m_tracks.at(index.row()).artist;
                return artist.isEmpty()?tr("Unkown"):artist;
            }
            case 2:
            {
                const QString album = m_tracks.at(index.row()).album;
                return album.isEmpty()?tr("Unkown"):album;
            }
            case 3:
            {
                if (m_tracks.at(index.row()).length>3600)
                    return QTime().addSecs(m_tracks.at(index.row()).length).toString("h:mm:ss");
                else
                    return QTime().addSecs(m_tracks.at(index.row()).length).toString("m:ss");
            }
        }
    }

    switch(role)
    {
    case (Qt::UserRole):
        return m_tracks.at(index.row());
    case (Qt::UserRole+1):
        return m_tracks.at(index.row()).state;
    }

    return QVariant();
}

QVariant DetailModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
    {
        switch(section)
        {
        case 0:
            return tr("Title");
        case 1:
            return tr("Artist");
        case 2:
            return tr("Album");
        case 3:
            return tr("Length");
        }

        return QString("column %1").arg(section);
    }

    return QAbstractTableModel::headerData(section, orientation, role);
}

void DetailModel::insert(const Tracks &tracks)
{
    if (m_clear) // got result - clear (prevent blinking)
        clear();

    beginInsertRows(QModelIndex(), m_tracks.count(), m_tracks.count()+tracks.count()-1);
    m_tracks.append(tracks);
    endInsertRows();

    Player *player = Singleton::instance()->player();

    // update item state if player is use current playlist
    if (player->playlist()==this)
    {
        if (player->state()==PlayerPlayingState || player->state()==PlayerPausedState || player->state()==PlayerErrorState)
        {

            qDebug() << "find" << m_play;
            for (int i=0; i<m_tracks.count(); ++i)
            {
                if (m_tracks[i].url==player->lastPlayedTrack().url)
                {
                    m_play = i;

                    QModelIndex newIndex = this->index(m_play, 0);
                    Track newTrack = newIndex.data(Qt::UserRole).value<Track>();

                    if (player->state()==PlayerPlayingState)
                        newTrack.state = Track::TrackStatePlay;
                    if (player->state()==PlayerPausedState)
                        newTrack.state = Track::TrackStatePause;
                    if (player->state()==PlayerErrorState)
                        newTrack.state = Track::TrackStateError;

                    setData(newIndex, newTrack);

                    break;
                }
            }
        }
    }
}

bool DetailModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    Q_UNUSED(role);

    if (!index.isValid())
        return false;

    if (value.canConvert<Track>())
    {
        m_tracks.replace(index.row(), value.value<Track>());
        emit dataChanged(this->index(index.row(),0), this->index(index.row(),columnCount()-1));
        return true;
    }

    return false;
}

void DetailModel::play(const QModelIndex &index)
{
    stop();
    m_play = index.row();
}

void DetailModel::stop()
{
    if (m_play==-1)
        return;

    QModelIndex oldIndex = this->index(m_play, 0);
    Track oldTrack = oldIndex.data(Qt::UserRole).value<Track>();
    oldTrack.state = Track::TrackStateNone;
    setData(oldIndex, oldTrack);

    m_play = -1;
}

void DetailModel::filter(const QString &text)
{
    m_clear = true; // clear after get result (prevent blink)
    m_library->getTracks(text, 1024);
}

void DetailModel::clear()
{
    beginRemoveRows(QModelIndex(), 0, m_tracks.count()-1);
    m_play = -1;
    m_tracks.clear();
    m_clear = false;
    endRemoveRows();
}

void DetailModel::trackSelected(const QModelIndex &index)
{
    if (!index.data(Qt::UserRole).canConvert<Track>())
        return;

    Track track = index.data(Qt::UserRole).value<Track>();

    Singleton::instance()->player()->setPlaylist(this);
    Singleton::instance()->player()->play(track);

    play(index);
}
