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
#include <QtGui>

#include "widgets/breadcrumb.h"

#include "elide_label.h"

Breadcrumb::Breadcrumb(QWidget *parent)
    : QFrame(parent)
{
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setMargin(1);
    layout->setSpacing(10);

    QPushButton *button = new QPushButton(tr("Back"), this);
    button->setFlat(true);
    button->setIcon(QIcon::fromTheme("back"));
    button->setIconSize(QSize(32,32));

    m_label = new ElideLabel(this);
    m_label->setBold(true);

    m_cover = new QLabel(this);

    layout->addWidget(button);
    layout->addWidget(m_label);
    layout->addWidget(m_cover);

    connect(button, SIGNAL(clicked()), SIGNAL(back()));
}

void Breadcrumb::setText(const QString &text)
{
    m_label->setText(text);
}

void Breadcrumb::setCover(const QPixmap &pixmap)
{
    if (pixmap.isNull())
        m_cover->setPixmap(m_pixmap);
    else
        m_cover->setPixmap(pixmap.scaled(QSize(32,32), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
}

void Breadcrumb::setDefaultCover(const QPixmap &pixmap)
{
    m_pixmap = pixmap.scaled(QSize(32,32), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
}

void Breadcrumb::keyPressEvent(QKeyEvent * event)
{
    QFrame::keyPressEvent(event);
}
