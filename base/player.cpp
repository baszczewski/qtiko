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

#include "player.h"

#include "core/singleton.h"

Player::Player(QObject *parent)
    : QObject(parent)
    , m_playlist(0)
    , m_mediaObject(0)
    , m_audioOutput(0)
{
    m_defaultMuted = false;
    m_defaultVolume = 1;

    connect(this, SIGNAL(finished()), SLOT(playFinished()));
}

Player::~Player()
{
}

void Player::stop()
{
    if (m_playlist)
    {
        m_playlist->setCurrentItemState(Track::TrackStateNone);
        m_playlist = 0;
    }

    if (m_mediaObject)
    {
        m_mediaObject->stop();
        delete m_mediaObject;
        m_mediaObject = 0;
    }

    if (m_audioOutput)
    {
        delete m_audioOutput;
        m_audioOutput = 0;
    }
}

void Player::play()
{
    if (!m_mediaObject)
    {
        current();
        return;
    }

    if (m_mediaObject->state()==Phonon::PlayingState)
        m_mediaObject->pause();
    else if (m_mediaObject->state()==Phonon::PausedState)
        m_mediaObject->play();
    else
        current();
}

void Player::play(const QString &fileName)
{
    if (!m_mediaObject)
    {
        m_mediaObject = new Phonon::MediaObject(this);
        connect(m_mediaObject, SIGNAL(stateChanged(Phonon::State, Phonon::State)), SLOT(updateState(Phonon::State,Phonon::State)));
        connect(m_mediaObject, SIGNAL(tick(qint64)), SIGNAL(tick(qint64)));
        connect(m_mediaObject, SIGNAL(totalTimeChanged(qint64)), SIGNAL(totalTimeChanged(qint64)));
        connect(m_mediaObject, SIGNAL(bufferStatus(int)), SIGNAL(bufferStatus(int)));
        connect(m_mediaObject, SIGNAL(finished()), SIGNAL(finished()));

        m_audioOutput = new Phonon::AudioOutput(Phonon::MusicCategory, this);
        connect(m_audioOutput, SIGNAL(volumeChanged(qreal)), SIGNAL(volumeChanged(qreal)));
        connect(m_audioOutput, SIGNAL(mutedChanged(bool)), SIGNAL(mutedChanged(bool)));

        m_audioOutput->setVolume(m_defaultVolume);
        m_audioOutput->setMuted(m_defaultMuted);

        m_mediaObject->setTickInterval(1000);

        Phonon::createPath(m_mediaObject, m_audioOutput);
    }
    m_mediaObject->setCurrentSource(Phonon::MediaSource(fileName));
//    m_mediaObject->setCurrentSource(Phonon::MediaSource("http://91.121.164.186:9250"));

    m_mediaObject->play();

    qDebug() << fileName;
}

void Player::play(const QVariant &item)
{
    if (item.canConvert<Track>())
    {
        Track track = item.value<Track>();

        if (track.isValid())
        {
            Singleton::instance()->show(track.artist, track.title, QImage(":/images/album.png"));
            m_track = track;
            play(track.url);
        }
        else
            stop();
    }
    else if (item.canConvert<QString>())
    {
        QString url = item.value<QString>();
        if (!url.isEmpty())
            play(url);
        else
            stop();
    }
}

PlayerState Player::state() const
{
    if (!m_mediaObject)
        return PlayerUnkownState;

    return state(m_mediaObject->state());
}

qreal Player::volume() const
{
    if (m_audioOutput)
        return m_audioOutput->volume();
    else
        return m_defaultVolume;
}

bool Player::muted() const
{
    if (m_audioOutput)
        return m_audioOutput->isMuted();
    else
        return m_defaultMuted;
}

void Player::seek(const qint64 &time)
{
    if (m_mediaObject)
        m_mediaObject->seek(time);
}

void Player::setMuted(const bool &mute)
{
    if (m_audioOutput)
        m_audioOutput->setMuted(mute);
    else
    {
        m_defaultMuted = mute;
        emit mutedChanged(mute);
    }
}

void Player::setVolume(const qreal &newVolume)
{
    if (m_audioOutput)
        m_audioOutput->setVolume(newVolume);
    else
    {
        m_defaultVolume = newVolume;
        emit volumeChanged(newVolume);
    }
}

void Player::updateState(const Phonon::State &newState, const Phonon::State &oldState)
{
    Q_UNUSED(oldState);

    PlayerState state = this->state(newState);

    if (m_playlist)
    {
        switch(state)
        {
        case(PlayerBufferingState):
        case(PlayerLoadingState):
        case(PlayerPlayingState):
            m_playlist->setCurrentItemState(Track::TrackStatePlay);
            break;
        case(PlayerPausedState):
            m_playlist->setCurrentItemState(Track::TrackStatePause);
            break;
        default:
            m_playlist->setCurrentItemState(Track::TrackStateNone);
            break;
        }
    }

    emit stateChanged(state);
}

Playlist *Player::playlist()
{
    return m_playlist;
}

void Player::setPlaylist(Playlist *playlist)
{
    if (playlist==m_playlist)
        return;

    stop();

    if (m_playlist)
    {
        QObject *object = dynamic_cast<QObject*>(m_playlist);
        if (object)
            disconnect(object, 0, this, 0);
    }

    m_playlist = playlist;
}

void Player::current()
{
    if (playlistAvailable())
    {
        m_playlist->setCurrentItemState(Track::TrackStateNone);

        play(m_playlist->currentItem());
    }
}

void Player::next()
{
    if (playlistAvailable())
    {
        m_playlist->setCurrentItemState(Track::TrackStateNone);
        play(m_playlist->nextItem());
    }
}

void Player::previous()
{
    if (playlistAvailable())
    {
        m_playlist->setCurrentItemState(Track::TrackStateNone);
        play(m_playlist->previousItem());
    }
}

void Player::playFinished()
{
    next();
}

bool Player::playlistAvailable()
{
    if (m_playlist)
        return true;

    emit needPlaylist();

    return m_playlist!=0;
}

Track Player::lastPlayedTrack() const
{
    return m_track;
}

PlayerState Player::state(const Phonon::State &value) const
{
    switch(value)
    {
    case(Phonon::LoadingState):
        return PlayerLoadingState;
    case(Phonon::StoppedState):
        return PlayerStoppedState;
    case(Phonon::PlayingState):
        return PlayerPlayingState;
    case(Phonon::BufferingState):
        return PlayerBufferingState;
    case(Phonon::PausedState):
        return PlayerPausedState;
    case(Phonon::ErrorState):
        return PlayerErrorState;
    default:
        return PlayerUnkownState;
    }

    return PlayerUnkownState;
}
