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

#ifndef FILTER_H
#define FILTER_H

#include <QLineEdit>

class QToolButton;
class QTimer;

class Filter : public QLineEdit
{
    Q_OBJECT
public:
    explicit Filter(QWidget *parent = 0);
    
protected:
    void prepare();
    void resizeEvent(QResizeEvent *event);

protected slots:
    void filter(const QString &text);
    void timer();

signals:
    void filterChanged(const QString &filter);

private:
    QToolButton *m_button;
    QTimer *m_timer;
};

#endif // FILTER_H
