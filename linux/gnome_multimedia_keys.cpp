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
#include <QAction>
#include <QtDBus>

#include "gnome_multimedia_keys.h"

GnomeMultimediaKeys::GnomeMultimediaKeys(QObject *parent)
    : QObject(parent)
    , m_player(0)
{
    if (QDBusConnection::sessionBus().interface()->isServiceRegistered("org.gnome.SettingsDaemon"))
    {
        m_interface = new QDBusInterface("org.gnome.SettingsDaemon", "/org/gnome/SettingsDaemon/MediaKeys", "org.gnome.SettingsDaemon.MediaKeys", QDBusConnection::sessionBus(), this);
        connect(m_interface, SIGNAL(MediaPlayerKeyPressed(QString,QString)), SLOT(mediaKeyPressed(QString,QString)));
    }
}

GnomeMultimediaKeys::~GnomeMultimediaKeys()
{
}

void GnomeMultimediaKeys::setPlayer(QObject *player)
{
    m_player = player;
}

void GnomeMultimediaKeys::mediaKeyPressed(const QString& application, const QString& key)
{
    Q_UNUSED(application);

    if (key == "Play")     emit play();
    if (key == "Stop")     emit stop();
    if (key == "Next")     emit next();
    if (key == "Previous") emit previous();
}
