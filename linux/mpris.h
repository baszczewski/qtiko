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

#ifndef MPRIS_H
#define MPRIS_H

#include <QObject>
#include <QDBusAbstractAdaptor>
#include <QStringList>

class MPRISPlayer : public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.mpris.MediaPlayer2.Player")
    Q_PROPERTY(bool CanControl READ canControl)
    Q_PROPERTY(bool CanGoNext READ canGoNext)
    Q_PROPERTY(bool CanGoPrevious READ canGoPrevious)
    Q_PROPERTY(bool CanPause READ canPause)
    Q_PROPERTY(bool CanPlay READ canPlay)
    Q_PROPERTY(bool CanSeek READ canSeek)
    Q_PROPERTY(QString PlaybackStatus READ playbackStatus)
    Q_PROPERTY(qlonglong Position READ position)

public:
    MPRISPlayer(QObject *parent = 0);

    bool canControl() const;
    bool canGoNext() const;
    bool canGoPrevious() const;
    bool canPause() const;
    bool canPlay() const;
    bool canSeek() const;

    QString playbackStatus() const;
    qlonglong position() const;

public slots:
    void Next();
    void Pause();
    void Play();
    void PlayPause();
    void Previous();
    void Stop();
    void changed();

private:
    QMap<QString, QVariant> m_props;
    QString m_trackID;
    qint64 m_previous_pos;
};

class MPRISRoot : public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.mpris.MediaPlayer2")
    Q_PROPERTY(bool CanQuit READ canQuit)
    Q_PROPERTY(bool CanRaise READ canRaise)
    Q_PROPERTY(QString DesktopEntry READ desktopEntry)
    Q_PROPERTY(bool HasTrackList READ hasTrackList)
    Q_PROPERTY(QString Identity READ identity)
    Q_PROPERTY(QStringList SupportedMimeTypes READ supportedMimeTypes)
    Q_PROPERTY(QStringList SupportedUriSchemes READ supportedUriSchemes)
public:
    MPRISRoot(QObject *parent = 0);
    virtual ~MPRISRoot();

    bool canQuit() const;
    bool canRaise() const;
    QString desktopEntry() const;
    bool hasTrackList() const;
    QString identity() const;
    QStringList supportedMimeTypes() const;
    QStringList supportedUriSchemes() const;

public slots:
    void Quit();
    void Raise();

signals:
    void raise();
    void quit();
};

class MPRIS : public QObject
{
    Q_OBJECT
public:
    explicit MPRIS(QObject *parent = 0);
    ~MPRIS();

signals:
    void raise();
    void quit();
    void changed();

private:
    MPRISPlayer *m_player;
    MPRISRoot *m_root;
};

#endif // MPRIS_H
