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

#include <QStyleOptionButton>
#include <QMenu>
#include <QPainter>
#include <QStylePainter>

#include "widgets/fancy_button.h"

FancyButton::FancyButton(QWidget *parent)
    : QPushButton(parent)
{
}

void FancyButton::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QStylePainter p(this);

    p.drawControl(QStyle::CE_PushButton, getStyleOption());

    int lineHeight = fontMetrics().height();

    int left = 0;

    if (!m_pixmap.isNull())
    {
        p.drawPixmap(QRect(FANCY_MARGIN,FANCY_MARGIN,m_pixmap.width()-FANCY_MARGIN*2, m_pixmap.height()-FANCY_MARGIN*2), m_pixmap);
        left += m_pixmap.width()+20;
    }

    QFont font = this->font();
    font.setBold(true);
    p.setFont(font);

    p.drawText(QRect(left, height()/2-lineHeight, width(), height()), m_title);

    font.setBold(false);
    p.setFont(font);
    p.drawText(QRect(left, height()/2, width(), height()), m_subtitle);
}

QStyleOptionButton FancyButton::getStyleOption() const
{
    QStyleOptionButton opt;
    opt.initFrom(this);

    opt.features = QStyleOptionButton::None;
    opt.features |= QStyleOptionButton::Flat;

    if (autoDefault() || isDefault())
        opt.features |= QStyleOptionButton::AutoDefaultButton;

    if (isDefault())
        opt.features |= QStyleOptionButton::DefaultButton;

    if (isDown() || (menu() && menu()->isVisible()))
        opt.state |= QStyle::State_Sunken;

    if (isChecked())
        opt.state |= QStyle::State_On;

    opt.text = "";
    opt.icon = QIcon();
    opt.iconSize = iconSize();

    return opt;
}

void FancyButton::setContent(const QString &title, const QString &subtitle, const QIcon &icon)
{
    m_title = title;
    m_subtitle = subtitle;

    if (icon.isNull())
        m_pixmap = QPixmap();
    else
        m_pixmap = icon.pixmap(64, 64);

    repaint();
}

QSize FancyButton::sizeHint() const
{
    const int textWidth = fontMetrics().width(m_subtitle);
    const int imageSize = m_pixmap.width();

    return QSize(textWidth+imageSize+20+FANCY_MARGIN*2, qMax(fontMetrics().height()*3,imageSize));
}
