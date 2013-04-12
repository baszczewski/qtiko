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

#include <QtDBus>
#include <QDebug>

#include "linux/mpris.h"

#include "core/singleton.h"
#include "base/player.h"

MPRISPlayer::MPRISPlayer(QObject *parent)
    : QDBusAbstractAdaptor(parent)
{
}

void MPRISPlayer::changed()
{
//    QList<QByteArray> changedProps;

//        changedProps << "playbackStatus";

//    QVariantMap map;
//    foreach(QByteArray name, changedProps)
//        map.insert(name, m_props.value(name));

//    QDBusMessage msg = QDBusMessage::createSignal("/org/mpris/MediaPlayer2",
//                                                "org.freedesktop.DBus.Properties", "PropertiesChanged");
//    QVariantList args = QVariantList()
//        << "org.mpris.MediaPlayer2.Player"
//        << map
//        << QStringList();
//    msg.setArguments(args);
//    QDBusConnection::sessionBus().send(msg);
}

bool MPRISPlayer::canControl() const
{
    return true;
}

bool MPRISPlayer::canGoNext() const
{
    return true;
}

bool MPRISPlayer::canGoPrevious() const
{
    return true;
}

bool MPRISPlayer::canPause() const
{
    return true;
}

bool MPRISPlayer::canPlay() const
{
    return true;
}

bool MPRISPlayer::canSeek() const
{
    return true;
}

QString MPRISPlayer::playbackStatus() const
{
    if(Singleton::instance()->player()->state()==PlayerPlayingState)
        return "Playing";
    else if (Singleton::instance()->player()->state()==PlayerPausedState)
        return "Paused";
    return "Stopped";
}

qlonglong MPRISPlayer::position() const
{
    return 0;
}

void MPRISPlayer::Next()
{
    Singleton::instance()->player()->next();
}

void MPRISPlayer::Pause()
{
    Singleton::instance()->player()->play();
}

void MPRISPlayer::Play()
{
    Singleton::instance()->player()->play();
}

void MPRISPlayer::PlayPause()
{
    Singleton::instance()->player()->play();
}

void MPRISPlayer::Previous()
{
    Singleton::instance()->player()->previous();
}

void MPRISPlayer::Stop()
{
    Singleton::instance()->player()->stop();
}


MPRISRoot::MPRISRoot(QObject *parent)
    : QDBusAbstractAdaptor(parent)
{
}

MPRISRoot::~MPRISRoot()
{}

bool MPRISRoot::canQuit() const
{
    return true;
}

bool MPRISRoot::canRaise() const
{
    return false;
}

QString MPRISRoot::desktopEntry() const
{
    return "qtiko";
}

bool MPRISRoot::hasTrackList() const
{
    return false;
}
QString MPRISRoot::identity() const
{
    return "qtiko";
}

QStringList MPRISRoot::supportedMimeTypes() const
{
    return QStringList();
}

QStringList MPRISRoot::supportedUriSchemes() const
{
    return QStringList();
}

void MPRISRoot::Quit()
{
    qDebug() << "quit";
//    UiHelper::instance()->exit();
}

void MPRISRoot::Raise()
{
    emit raise();
}

MPRIS::MPRIS(QObject *parent)
    : QObject(parent)
{
    QDBusConnection connection = QDBusConnection::sessionBus();

    m_root = new MPRISRoot(this);
    m_player = new MPRISPlayer(this);

    connect(this, SIGNAL(changed()), m_player, SLOT(changed()));

    connection.registerObject("/org/mpris/MediaPlayer2", this);
    connection.registerService("org.mpris.qtiko");
    connection.registerService("org.mpris.MediaPlayer2.qtiko");

    connect(m_root, SIGNAL(raise()), SIGNAL(raise()));
    connect(m_root, SIGNAL(quit()), SIGNAL(quit()));
}

MPRIS::~MPRIS()
{
    qDebug() << "close";
    QDBusConnection::sessionBus().unregisterService("org.mpris.MediaPlayer2.qtiko");
}
