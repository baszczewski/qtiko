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

#ifndef VOLUME_SLIDER_H
#define VOLUME_SLIDER_H

#include <QSlider>

class Player;

class VolumeSlider : public QSlider
{
    Q_OBJECT
public:
    explicit VolumeSlider(QWidget *parent = 0);

    void setPlayer(Player *player);

protected slots:
    void volume(const int &value);
    void setVolume(const qreal &volume);
    void mutedChanged(const bool &mute);

signals:
    void volumeChanged(const qreal &volume);

protected:
    void mousePressEvent(QMouseEvent * event);

private:
    Player *m_player;
};

#endif // VOLUME_SLIDER_H
