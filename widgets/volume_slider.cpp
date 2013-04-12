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
#include <QMouseEvent>
#include <QStyle>

#include "widgets/volume_slider.h"
#include "base/player.h"

VolumeSlider::VolumeSlider(QWidget *parent)
    : QSlider(parent)
{
    setOrientation(Qt::Horizontal);

    connect(this, SIGNAL(valueChanged(int)), this, SLOT(volume(int)));

    const int step = qMax(1.0,(qreal)100/50);
    setSingleStep(step);
    setPageStep(step);
}

void VolumeSlider::setVolume(const qreal &volume)
{
    if (m_player->muted() && volume>0)
    {
        m_player->setMuted(false);
    }

    if (!m_player->muted() && volume==0)
    {
        m_player->setMuted(true);
    }

    blockSignals(true);
    setValue(maximum()*volume);
    blockSignals(false);
}

void VolumeSlider::volume(const int &value)
{
    emit volumeChanged((qreal)value/maximum());
}

void VolumeSlider::setPlayer(Player *player)
{
    m_player = player;

    connect(m_player, SIGNAL(mutedChanged(bool)), SLOT(mutedChanged(bool)));
    connect(m_player, SIGNAL(volumeChanged(qreal)), SLOT(setVolume(qreal)));
    connect(this, SIGNAL(volumeChanged(qreal)), m_player, SLOT(setVolume(qreal)));
}

void VolumeSlider::mutedChanged(const bool &mute)
{
    if (mute)
    {
        setVolume(0);
    }
    else
    {
        if (m_player->volume()>0)
            setVolume(m_player->volume());
        else
        {
            setVolume(1);
            emit volumeChanged(1);
        }
    }
}

void VolumeSlider::mousePressEvent(QMouseEvent * event)
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
