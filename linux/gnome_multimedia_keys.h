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

#ifndef GNOME_MULTIMEDIA_KEYS_H
#define GNOME_MULTIMEDIA_KEYS_H

#include <QObject>

#include "interfaces/interface_control.h"

class QDBusInterface;

class GnomeMultimediaKeys: public QObject, public InterfaceControl
{
    Q_OBJECT
    Q_INTERFACES(InterfaceControl)
public:
    explicit GnomeMultimediaKeys(QObject *parent = 0);
    ~GnomeMultimediaKeys();

    void setPlayer(QObject *player);

protected slots:
    void mediaKeyPressed(const QString& application, const QString& key);

signals:
    void play();
    void stop();
    void previous();
    void next();

private:
    QObject *m_player;
    QDBusInterface *m_interface;
};

#endif // GNOME_MULTIMEDIA_KEYS_H
