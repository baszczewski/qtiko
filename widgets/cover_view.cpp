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

#include <QScrollBar>
#include <QDebug>
#include <QPropertyAnimation>
#include <QTimeLine>
#include <QResizeEvent>
#include <qmath.h>

#include "models/cover_model.h"
#include "widgets/cover_view.h"
#include "widgets/cover_item.h"

CoverView::CoverView(QWidget *parent)
    : QListView(parent)
    , m_timer(0)
{
    setEditTriggers(NoEditTriggers);

    QPalette p = palette();
    p.setColor(QPalette::Background, Qt::transparent);
    p.setBrush(QPalette::Base, Qt::transparent);
    setPalette(p);

    setItemDelegate(new CoverItem(this));
    setSelectionMode(QAbstractItemView::ExtendedSelection);

    setWrapping(true);
    setFlow(QListView::LeftToRight);
//    setGridSize(QSize(ITEM_SIZE, 170));


    setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);

    setResizeMode(QListView::Adjust);
    setMovement(QListView::Static);
    setUniformItemSizes(true);

    setDragEnabled(true);
    setDragDropMode(QAbstractItemView::DragOnly);

    setAttribute(Qt::WA_MacShowFocusRect, false);
    setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    setFrameShape(QFrame::NoFrame);

    verticalScrollBar()->setPageStep(3);
    verticalScrollBar()->setSingleStep(1);

    setMouseTracking(true);
    connect(this, SIGNAL(entered(QModelIndex)), this, SLOT(itemEntered(QModelIndex)));
    connect(this, SIGNAL(viewportEntered()), this, SLOT(itemClear()));
    connect(this, SIGNAL(clicked(QModelIndex)), SIGNAL(coverSelected(QModelIndex)));
}

//void CoverView::resizeEvent(QResizeEvent * e)
//{
//    const QSize temp = e->size();

//    const int horizontalItemsCount = qFloor(temp.width()/ITEM_SIZE);
//    const int horizontalItemsWidth = horizontalItemsCount*ITEM_SIZE;
//    const int spacing = (temp.width()-horizontalItemsWidth)/(horizontalItemsCount);

//    setSpacing(spacing-3);

//    qDebug() << spacing;

//    QListView::resizeEvent(e);
//}

void CoverView::currentChanged(const QModelIndex &current, const QModelIndex &previous)
{
    QListView::currentChanged(current, previous);
}

void CoverView::itemClear()
{
    foreach (int key, m_animations.keys())
    {
         itemExited(model()->index(key, 0));
    }
}

void CoverView::itemEntered(const QModelIndex &index)
{
    foreach (int key, m_animations.keys())
    {
        if (key!=index.row())
        {
            itemExited(model()->index(key, 0));
        }
    }

    if (!m_animations.contains(index.row()))
    {
        QTimeLine *timeLine = new QTimeLine(300, this);
        timeLine->setCurveShape(QTimeLine::EaseInOutCurve);
        timeLine->setProperty("index", index.row());
        timeLine->setFrameRange(0, 100);

        connect(timeLine, SIGNAL(frameChanged(int)), this, SLOT(animationChanged(int)));
        connect(timeLine, SIGNAL(finished()), this, SLOT(animationDiscard()));

        m_animations.insert(index.row(), timeLine);
        timeLine->start();
    }
    else
    {
        QTimeLine *timeLine = m_animations[index.row()];
        timeLine->stop();
        timeLine->setFrameRange(timeLine->currentFrame(), 100);
        timeLine->start();
    }
}

void CoverView::itemExited(const QModelIndex &index)
{
    if (!m_animations.contains(index.row()))
    {
        QTimeLine *timeLine = new QTimeLine(300, this);
        timeLine->setCurveShape(QTimeLine::EaseInOutCurve);
        timeLine->setProperty("index", index.row());
        timeLine->setFrameRange(100, 0);

        connect(timeLine, SIGNAL(frameChanged(int)), this, SLOT(animationChanged(int)));
        connect(timeLine, SIGNAL(finished()), this, SLOT(animationDiscard()));

        m_animations.insert(index.row(), timeLine);
        timeLine->start();
    }
    else
    {
        QTimeLine *timeLine = m_animations[index.row()];
        if (timeLine->endFrame()!=0)
        {
            timeLine->stop();
            timeLine->setFrameRange(timeLine->currentFrame(), 0);
            timeLine->start();
        }
    }
}

void CoverView::animationChanged(const int &frame)
{
    int index = sender()->property("index").toInt();

    QVariant v = model()->data(model()->index(index, 0), Qt::UserRole);
    Cover d = v.value<Cover>();
    d.animation = (qreal)frame/100;
    model()->setData(model()->index(index,0), d, Qt::UserRole);
}

void CoverView::animationDiscard()
{
    QTimeLine *timeLine = qobject_cast<QTimeLine*>(sender());
    if (timeLine->currentFrame()==0)
    {
        m_animations.remove(timeLine->property("index").toInt());
        delete timeLine;
    }
}

void CoverView::leaveEvent(QEvent * event)
{
    Q_UNUSED(event);

    itemClear();
}

void CoverView::hideEvent(QHideEvent * event)
{
    CoverModel *coverModel = qobject_cast<CoverModel*>(model());
    if (coverModel)
    {
        coverModel->clearPixmaps();
    }
    QListView::hideEvent(event);
}
