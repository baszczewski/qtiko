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
#include <QApplication>
#include <QLinearGradient>
#include <QDebug>

#include "widgets/detail_item.h"
#include "types/track.h"

DetailItem::DetailItem(QWidget *parent)
    : QStyledItemDelegate(parent)
{
    QIcon iconPlay = QIcon::fromTheme("media-playback-start");
    m_play = iconPlay.pixmap(qApp->fontMetrics().height(), qApp->fontMetrics().height());

    QIcon iconPause = QIcon::fromTheme("media-playback-pause");
    m_pause = iconPause.pixmap(qApp->fontMetrics().height(), qApp->fontMetrics().height());
}


QSize DetailItem::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(index);

    const int height = option.fontMetrics.height()*7;

    return QSize(height, height);
}

void DetailItem::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyleOptionViewItemV4 opt(option);
    initStyleOption(&opt, index);

    Track::TrackState state = (Track::TrackState)index.data(Qt::UserRole+1).toInt();

    const bool selected = option.state & QStyle::State_Selected;
    const bool current = state!=Track::TrackStateNone;
    const int textMargin = opt.rect.height()/4;

    painter->save();

    QPalette::ColorGroup cg = option.state & QStyle::State_Enabled ? QPalette::Normal : QPalette::Disabled;
    if (cg == QPalette::Normal && !(option.state & QStyle::State_Active))
        cg = QPalette::Inactive;

    if (current && !selected)
    {
        const QColor highlightColor = option.palette.color(QPalette::Highlight);

        const QColor colorA = highlightColor.lighter(170);
        const QColor colorB = highlightColor.lighter(160);

        QLinearGradient linearGradient(option.rect.left(), option.rect.top(), option.rect.left(), option.rect.top()+option.rect.height());
        linearGradient.setColorAt(0.0, colorA);
        linearGradient.setColorAt(1.0, colorB);
        painter->fillRect(option.rect, linearGradient);
    }

    if (selected)
        painter->setPen(option.palette.color(cg, QPalette::HighlightedText));
    else
        painter->setPen(option.palette.color(cg, QPalette::Text));

    if (current)
    {
        QFont tempFont = painter->font();
        tempFont.setBold(true);
        painter->setFont(tempFont);

        if (index.column()==0)
        {
            switch(state)
            {
            case (Track::TrackStatePlay):
                painter->drawPixmap(option.rect.left()+textMargin, option.rect.top()+option.rect.height()/2-m_play.height()/2, m_play.width(), m_play.height(), m_play);
                break;
            case (Track::TrackStatePause):
                painter->drawPixmap(option.rect.left()+textMargin, option.rect.top()+option.rect.height()/2-m_pause.height()/2, m_pause.width(), m_pause.height(), m_pause);
                break;
            default:
                break;
            }
        }
    }

    const int extraSpace = ((index.column()==0 && current)?(m_play.width()*1.3):0);

    const QString title = painter->fontMetrics().elidedText(index.data().toString(), Qt::ElideRight, opt.rect.width()-textMargin-extraSpace);

    switch(index.column())
    {
        case 0:
        case 1:
        case 2:
        {
            painter->drawText(QRect(opt.rect.left()+textMargin+extraSpace, opt.rect.top(), opt.rect.width()-textMargin-extraSpace, opt.rect.height()), option.displayAlignment, title);
            break;
        }
        case 3:
        {
            painter->drawText(QRect(opt.rect.left(), opt.rect.top(), opt.rect.width()-textMargin, opt.rect.height()), Qt::AlignRight | Qt::AlignVCenter, title);
            break;
        }
    }

    painter->setPen(option.palette.color(QPalette::Midlight));
    painter->drawLine(opt.rect.left(), opt.rect.bottom(), opt.rect.right(), opt.rect.bottom());

    painter->restore();
}
