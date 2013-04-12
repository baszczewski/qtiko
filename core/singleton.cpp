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

#include <QMutex>
#include <QDir>
#include <QPluginLoader>
#include <QApplication>

#include "base/library.h"
#include "base/player.h"

#include "core/singleton.h"
#include "core/helper.h"
#include "core/main_window.h"

#include "interfaces/interface_control.h"
#include "interfaces/interface_notifications.h"

#ifdef Q_OS_LINUX
    #include "linux/gnome_multimedia_keys.h"
    #include "linux/notifications.h"
    #include "linux/mpris.h"
#endif

Singleton* Singleton::m_instance = 0;

Singleton* Singleton::instance()
{
    static QMutex mutex;
    if (!m_instance)
    {
        mutex.lock();

        if (!m_instance)
            m_instance = new Singleton;

        mutex.unlock();
    }

    return m_instance;
}

void Singleton::drop()
{
    static QMutex mutex;
    mutex.lock();
    delete m_instance;
    m_instance = 0;
    mutex.unlock();
}

Singleton::Singleton()
{
    m_notifications = 0;

    m_repeat = m_shuffle = false;

    m_player = new Player();

    m_library = new Library();
    m_library->setDirectory(Helper::musicDirectory());

    #ifdef Q_OS_LINUX
        m_notifications = new Notifications;
        m_plugins << m_notifications;

        QObject *gnomeMultimediaKeys = new GnomeMultimediaKeys;
        QObject::connect(gnomeMultimediaKeys, SIGNAL(play()), m_player, SLOT(play()));
        QObject::connect(gnomeMultimediaKeys, SIGNAL(stop()), m_player, SLOT(stop()));
        QObject::connect(gnomeMultimediaKeys, SIGNAL(next()), m_player, SLOT(next()));
        QObject::connect(gnomeMultimediaKeys, SIGNAL(previous()), m_player, SLOT(previous()));
        m_plugins << gnomeMultimediaKeys;

        m_mpris = new MPRIS;
        QObject::connect(m_player, SIGNAL(stateChanged(PlayerState)), m_mpris, SIGNAL(changed()));
        m_plugins << m_mpris;
    #endif
}

Singleton::~Singleton()
{
    delete m_player;
    delete m_library;

    qDeleteAll(m_plugins);
}

Library *Singleton::library()
{
    return m_library;
}

Player *Singleton::player()
{
    return m_player;
}

bool Singleton::shuffle() const
{
    return m_shuffle;
}

bool Singleton::repeat() const
{
    return m_repeat;
}

void Singleton::setShuffle(const bool &shuffle)
{
    m_shuffle = shuffle;
}

void Singleton::setRepeat(const bool &repeat)
{
    m_repeat = repeat;
}

void Singleton::show(const QString &title, const QString &subtitle, const QImage &image)
{
    if (m_notifications)
        m_notifications->show((title.isEmpty())?"Unkown":title, subtitle.isEmpty()?"Unkown":subtitle, image);
}

void Singleton::setMainWindow(MainWindow *window)
{
    m_window = window;

    if (m_mpris)
    {
        QObject::connect(m_mpris, SIGNAL(raise()), m_window, SLOT(raise()));
        QObject::connect(m_mpris, SIGNAL(raise()), m_window, SLOT(raise()));
    }
}
