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

#include <QPainter>
#include <QDebug>

#include "widgets/cover_item.h"

#include "types/cover.h"

CoverItem::CoverItem(QObject *parent)
    : QStyledItemDelegate(parent)
{
    prepare();
}

void CoverItem::prepare()
{
    m_margin = ITEM_SIZE*ITEM_MARGIN;
    m_size = ITEM_SIZE-m_margin*2;

    m_emptyMargin = ITEM_SIZE*ITEM_MARGIN/2;
    m_emptySize = ITEM_SIZE-m_emptyMargin*2;

    m_background = QPixmap(":/images/shadow.png");
    m_background = m_background.scaled(ITEM_SIZE, ITEM_SIZE, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

    m_album = QPixmap(":/images/album.png");
    m_album = m_album.scaled(m_emptySize, m_emptySize, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

    m_play = QPixmap(":/images/play.png");
    m_play = m_play.scaled(m_emptySize, m_emptySize, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
}

QSize CoverItem::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option);
    Q_UNUSED(index);

    return QSize(170, 170+option.fontMetrics.height());
}

void CoverItem::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Cover data = index.data(Qt::UserRole).value<Cover>();
    const QPixmap pixmap = index.data(Qt::UserRole+1).value<QPixmap>();

    painter->save();

    painter->translate(option.rect.topLeft());

    painter->drawPixmap(0, 0, m_background);

    QTextOption o;
    o.setAlignment(Qt::AlignHCenter | Qt::AlignBottom);

    const QString title = painter->fontMetrics().elidedText(data.title, Qt::ElideRight, ITEM_SIZE);


    painter->drawText(QRect(0, 0, ITEM_SIZE, ITEM_SIZE+painter->fontMetrics().height()), title, o);


    if (data.animation!=0)
    {
        if (pixmap.isNull())
            painter->drawPixmap(QRect(m_emptyMargin, m_emptyMargin, m_emptySize, m_emptySize), m_album);
        else
        {
            const qreal w = pixmap.height();
            const qreal t = w*0.1*data.animation;

            painter->drawPixmap(QRect(m_margin, m_margin, m_size, m_size), pixmap, QRect(t, t, w-t*2, w-t*2));
        }

        painter->setOpacity((qreal)data.animation*0.95);
        painter->drawPixmap(QRect(m_emptyMargin, m_emptyMargin, m_emptySize, m_emptySize), m_play);
    }
    else
    {
        if (pixmap.isNull())
            painter->drawPixmap(QRect(m_emptyMargin, m_emptyMargin, m_emptySize, m_emptySize), m_album);
        else
        {
//            painter->drawPixmap(QRect(m_margin, m_margin, m_size, m_size), pixmap.scaled(m_size, m_size, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
            painter->drawPixmap(QRect(m_margin, m_margin, m_size, m_size), pixmap);
        }
    }

    painter->restore();
}
