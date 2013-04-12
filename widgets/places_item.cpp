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

#include "widgets/places_item.h"

PlacesItem::PlacesItem(QObject *parent)
    : QStyledItemDelegate(parent)
{
}

bool PlacesItem::isCategory(const QModelIndex &index) const
{
    const QModelIndex previousIndex = index.sibling(index.row()-1, index.column());
    if (previousIndex.isValid())
    {
        return previousIndex.data(Qt::UserRole+1)!=index.data(Qt::UserRole+1);
    }
    else
        return true;
}

QSize PlacesItem::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QSize result = QStyledItemDelegate::sizeHint(option, index);

    if (isCategory(index))
        result.setHeight(result.height()*2.8);
    else
        result.setHeight(result.height()*1.4);

    return result;
}

void PlacesItem::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    const QRect rect = option.rect;
    const bool category = isCategory(index);

    if (category)
    {
        const QString categoryTitle = index.data(Qt::UserRole+1).toString();

        painter->save();

        QFont font;
        font.setBold(true);

        QTextOption opt;
        opt.setAlignment(Qt::AlignVCenter);

        painter->setFont(font);
        painter->drawText(QRect(rect.left()+15, rect.top(), rect.width(), rect.height()/2), categoryTitle, opt);

        painter->restore();
    }

    QStyleOptionViewItemV4 opt = option;
    initStyleOption(&opt, index);

    painter->save();

    const int iconSize = painter->fontMetrics().height()*1.3;
    QPixmap icon = opt.icon.pixmap(QSize(iconSize,iconSize));

    opt.text = "";
    opt.icon = QIcon();
    opt.rect = QRect(rect.left(), category?(rect.top()+rect.height()/2):rect.top(), rect.width(), category?rect.height()/2:rect.height());

    QStyle *style = opt.widget ? opt.widget->style() : QApplication::style();
    if (!opt.state.testFlag(QStyle::State_HasFocus))
    {
        opt.state = opt.state ^ QStyle::State_Active;
    }

    style->drawControl(QStyle::CE_ItemViewItem, &opt, painter, opt.widget);

    QPalette::ColorGroup cg = opt.state & QStyle::State_Enabled ? QPalette::Normal : QPalette::Disabled;
    if (cg == QPalette::Normal && !(opt.state & QStyle::State_Active))
        cg = QPalette::Inactive;

     if (opt.state & QStyle::State_Selected)
         painter->setPen(opt.palette.color(cg, QPalette::HighlightedText));
     else
         painter->setPen(opt.palette.color(cg, QPalette::Text));

    painter->drawPixmap(QRect(rect.left()+25, opt.rect.top()+opt.rect.height()/2-iconSize/2, iconSize, iconSize), icon);

    const QString title = painter->fontMetrics().elidedText(index.data().toString(), Qt::ElideRight, rect.width()-(rect.left()+25+iconSize*1.4));


    painter->drawText(QRect(rect.left()+25+iconSize*1.4, category?(rect.top()+rect.height()/2):rect.top(), rect.width(), category?rect.height()/2:rect.height()), opt.displayAlignment, title);

    painter->restore();

}
