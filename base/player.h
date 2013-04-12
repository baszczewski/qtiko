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

#ifndef PLAYER_H
#define PLAYER_H

#include <QObject>
#include <phonon>

#include "types/track.h"
#include "types/playlist.h"

enum PlayerState
{
    PlayerUnkownState,
    PlayerLoadingState,
    PlayerStoppedState,
    PlayerPlayingState,
    PlayerBufferingState,
    PlayerPausedState,
    PlayerErrorState
};

class Player: public QObject
{
    Q_OBJECT
public:
    explicit Player(QObject *parent = 0);
    ~Player();

public slots:
    void play();
    void play(const QString &fileName);
    void play(const QVariant &item);
    void stop();
    void next();
    void previous();
    void current();

    void seek(const qint64 &time);

    PlayerState state() const;
    qreal volume() const;
    bool muted() const;

    void setMuted(const bool &mute);
    void setVolume(const qreal &newVolume);

    Playlist *playlist();
    void setPlaylist(Playlist *playlist);

    Track lastPlayedTrack() const;

protected:
    bool playlistAvailable();
    PlayerState state(const Phonon::State &value) const;

protected slots:
    void updateState(const Phonon::State &newState, const Phonon::State &oldState);
    void playFinished();

signals:
    void tick(const qint64 &time);
    void volumeChanged(const qreal &newVolume);
    void bufferStatus(const int &percentFilled);
    void stateChanged(const PlayerState &state);
    void totalTimeChanged(const qint64 &newTotalTime);
    void mutedChanged(const bool &muted);
    void finished();
    void needPlaylist();

private:
    Playlist *m_playlist;

    Phonon::MediaObject *m_mediaObject;
    Phonon::AudioOutput *m_audioOutput;

    qreal m_defaultVolume;
    bool m_defaultMuted;
    Track m_track;
};

#endif // PLAYER_H
