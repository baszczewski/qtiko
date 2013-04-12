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

#include <QToolButton>
#include <QStyle>
#include <QMenu>
#include <QTimer>
#include <QDebug>

#include "widgets/filter.h"

Filter::Filter(QWidget *parent)
    : QLineEdit(parent)
    , m_button(0)
    , m_timer(0)
{
    prepare();
}

void Filter::prepare()
{
    QIcon icon = QIcon::fromTheme("edit-find");

    int frameWidth = style()->pixelMetric(QStyle::PM_DefaultFrameWidth);
    QSize minimumSize = minimumSizeHint();

    m_button = new QToolButton(this);
    m_button->setPopupMode(QToolButton::InstantPopup);
    m_button->setIcon(icon);
    m_button->setCursor(Qt::ArrowCursor);
    m_button->setStyleSheet("QToolButton { border: none; padding: 0px; background: transparent; } QToolButton::menu-indicator {image: none;}");
    m_button->setAutoRaise(true);

    setStyleSheet(QString("QLineEdit { padding-left: %1px; }").arg(m_button->sizeHint().width() + frameWidth + 1));

    setMinimumSize(qMax(minimumSize.width(), m_button->sizeHint().height() + frameWidth * 2 + 2), qMax(minimumSize.height(), m_button->sizeHint().height() ));

    connect(this, SIGNAL(textChanged(QString)), SLOT(filter(QString)));
}

void Filter::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);

    QSize sizeHint = m_button->sizeHint();
    int frameWidth = style()->pixelMetric(QStyle::PM_DefaultFrameWidth);

    m_button->move(frameWidth+1, (rect().bottom() + 1 - sizeHint.height())/2);
}

void Filter::filter(const QString &text)
{
    if (!m_timer)
    {
        m_timer = new QTimer(this);
        m_timer->setSingleShot(true);
        m_timer->setInterval(300);
        connect(m_timer, SIGNAL(timeout()), SLOT(timer()));
    }

    m_timer->setProperty("text", text);

    m_timer->stop();
    m_timer->start();
}

void Filter::timer()
{
    QString text = sender()->property("text").toString();
    emit filterChanged(text);
}
