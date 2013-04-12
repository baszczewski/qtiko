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

#ifndef FANCY_BUTTON_H
#define FANCY_BUTTON_H

#include <QPushButton>

class QStyleOptionButton;

#define FANCY_MARGIN 3

class FancyButton : public QPushButton
{
    Q_OBJECT
public:
    explicit FancyButton(QWidget *parent = 0);

    void setContent(const QString &title, const QString &subtitle, const QIcon &icon = QIcon());

    QSize sizeHint() const;

protected:
    void paintEvent(QPaintEvent *event);
    QStyleOptionButton getStyleOption() const;

private:
    QString m_title;
    QString m_subtitle;
    QPixmap m_pixmap;
};

#endif // FANCY_BUTTON_H
