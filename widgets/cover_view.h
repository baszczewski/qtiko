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

#ifndef COVER_VIEW_H
#define COVER_VIEW_H

#include <QListView>

class QTimeLine;

class CoverView : public QListView
{
    Q_OBJECT
public:
    explicit CoverView(QWidget *parent = 0);

signals:
    void opacityChanged();

protected slots:
    void animationChanged(const int &frame);
    void animationDiscard();
    void itemEntered(const QModelIndex &index);
    void itemExited(const QModelIndex &index);
    void itemClear();
    void currentChanged(const QModelIndex & current, const QModelIndex & previous);

protected:
//    void resizeEvent(QResizeEvent * e);
    void leaveEvent(QEvent * event);
    void hideEvent(QHideEvent * event);

signals:
    void coverSelected(const QModelIndex &index);

private:
    qreal m_opacity;
    QHash<int, QTimeLine*> m_animations;
    QTimer *m_timer;
};

#endif // COVER_VIEW_H
