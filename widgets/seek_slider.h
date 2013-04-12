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

#ifndef SEEK_SLIDER_H
#define SEEK_SLIDER_H

#include <QSlider>
#include "base/player.h"

class QTimer;
class Player;

class SeekSlider : public QSlider
{
    Q_OBJECT
public:
    explicit SeekSlider(QWidget *parent = 0);

    void setPlayer(Player *player);

protected slots:
    void timer();
    void seek(const int &pos);
    void setTick(const qint64 &time);
    void setTotalTime(const qint64 &newTotalTime);
    void stateChanged(const PlayerState &state);
    void startDrag();
    void stopDrag();

protected:
    void mousePressEvent(QMouseEvent * event);
    void mouseMoveEvent(QMouseEvent* event);

signals:
    void positionChanged(const qint64 &time);

private:
    QTimer *m_timer;
    Player *m_player;
    bool m_drag;
};

#endif // SEEK_SLIDER_H
