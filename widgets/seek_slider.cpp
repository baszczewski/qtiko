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
#include <QTimer>
#include <QMouseEvent>
#include <QStyle>
#include <QStyleOptionSlider>
#include <qmath.h>

#include "widgets/seek_slider.h"

#include "base/player.h"

SeekSlider::SeekSlider(QWidget *parent)
    : QSlider(parent)
    , m_timer(0)
    , m_player(0)
{
    m_drag = false;

    setOrientation(Qt::Horizontal);

    connect(this, SIGNAL(valueChanged(int)), this, SLOT(seek(int)));
    connect(this, SIGNAL(sliderPressed()), this, SLOT(startDrag()));
    connect(this, SIGNAL(sliderReleased()), this, SLOT(stopDrag()));
}

void SeekSlider::setTick(const qint64 &time)
{
    if (m_timer)
    {
        if (m_timer->isActive())
            return;
    }

    blockSignals(true);
    setValue(time);
    blockSignals(false);
}

void SeekSlider::setTotalTime(const qint64 &newTotalTime)
{
    setMaximum(newTotalTime);

    const int step = qMax(1.0,(qreal)newTotalTime/100);
    setSingleStep(step);
    setPageStep(step);
}

void SeekSlider::seek(const int &pos)
{
    if (!m_timer)
    {
        m_timer = new QTimer(this);
        m_timer->setInterval(100);
        m_timer->setSingleShot(true);

        connect(m_timer, SIGNAL(timeout()), this, SLOT(timer()));
    }

    m_timer->setProperty("position", pos);

    m_timer->stop();
    m_timer->start();
}

void SeekSlider::timer()
{
    const int pos = sender()->property("position").toInt();

    emit positionChanged(pos);
}

void SeekSlider::mousePressEvent(QMouseEvent * event)
{
    Qt::MouseButton button = event->button();

    if (event->button() == Qt::LeftButton)
    {
        int buttons = style()->styleHint(QStyle::SH_Slider_AbsoluteSetButtons);

        switch(buttons)
        {
        case (Qt::LeftButton):
            button = Qt::LeftButton;
            break;
        case (Qt::MidButton):
            button = Qt::MidButton;
            break;
        case (Qt::RightButton):
            button = Qt::RightButton;
            break;
        }
    }

    QMouseEvent newEvent(event->type(), event->pos(), button, button, event->modifiers());
    QSlider::mousePressEvent(&newEvent);

    if (newEvent.isAccepted())
        event->accept();
}

void SeekSlider::mouseMoveEvent(QMouseEvent* event)
{
    QSlider::mouseMoveEvent(event);

    /*
    QStyleOptionSlider option;
    initStyleOption(&option);

    QRect groove = style()->subControlRect(QStyle::CC_Slider, &option, QStyle::SC_SliderGroove, this);
    QRect handle = style()->subControlRect(QStyle::CC_Slider, &option, QStyle::SC_SliderHandle, this);

    int slider_min = groove.x();
    int slider_max = groove.right() - handle.width() + 1;

    int time = QStyle::sliderValueFromPosition(minimum(), maximum(), event->x() - handle.width()/2 - slider_min + 1, slider_max - slider_min);
    qDebug() << time;
    */

    // TO DO: tooltip
}

void SeekSlider::setPlayer(Player *player)
{
    m_player = player;

    connect(m_player, SIGNAL(tick(qint64)), SLOT(setTick(qint64)));
    connect(m_player, SIGNAL(totalTimeChanged(qint64)), SLOT(setTotalTime(qint64)));
    connect(m_player, SIGNAL(stateChanged(PlayerState)), SLOT(stateChanged(PlayerState)));

    connect(this, SIGNAL(positionChanged(qint64)), m_player, SLOT(seek(qint64)));

    if (m_player->state()==PlayerUnkownState)
        setEnabled(false);
}

void SeekSlider::stateChanged(const PlayerState &state)
{
    if (state==PlayerPlayingState || state==PlayerBufferingState || state==PlayerPausedState)
    {
        setEnabled(true);
    }
    else
    {
        setValue(0);
        setEnabled(false);
    }
}

void SeekSlider::startDrag()
{
    m_drag = true;
}

void SeekSlider::stopDrag()
{
    m_drag = false;
}
