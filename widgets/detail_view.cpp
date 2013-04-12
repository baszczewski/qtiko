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

#include <QHeaderView>
#include <QResizeEvent>
#include <QDebug>
#include <qmath.h>

#include "widgets/detail_view.h"
#include "widgets/detail_item.h"

#include "types/track.h"

DetailView::DetailView(QWidget *parent)
    : QTableView(parent)
{
    setAlternatingRowColors(true);
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setShowGrid(false);

    verticalHeader()->hide();

    setItemDelegate(new DetailItem(this));

    setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);

    horizontalHeader()->setStretchLastSection(true);
    horizontalHeader()->setResizeMode(QHeaderView::Interactive);

    verticalHeader()->setDefaultSectionSize(fontMetrics().height()*2); //setHeight


    connect(this, SIGNAL(doubleClicked(QModelIndex)), SLOT(selected(QModelIndex)));
    connect(parent, SIGNAL(resized()), SLOT(resizeColumns()));
}

void DetailView::resizeEvent(QResizeEvent *event)
{
    const int oldWidth = event->oldSize().width();

    qreal sum = 0;

    m_columns.clear();

    if (oldWidth==-1) // default columns widths (%)
    {
        m_columns << 0.4 << 0.3 << 0.3;
    }
    else
    {
        for (int i=0; i<3; ++i)
        {
            qreal temp = (qreal)horizontalHeader()->sectionSize(i)/(oldWidth-horizontalHeader()->sectionSize(3));
            m_columns << temp;

            sum += m_columns.last();
        }

        if (sum>1) // to much - we dont want horizontal scroolbar
        {
            for (int i=0; i<3; ++i)
            {
                m_columns[i] /= sum;
            }
        }
    }

    QTableView::resizeEvent(event);

    if (oldWidth==-1)
        resizeColumns();
}

void DetailView::resizeColumns()
{
    if (!m_columns.count())
        return;

    qreal temp = 0;


    const int left = qMax(200, width()-120);

    for (int i=0; i<3; ++i)
    {
        setColumnWidth(i, qMax(50.0,floor(m_columns[i]*left)-1));

        temp += m_columns[i];
    }

    setColumnWidth(3, 0);
}

void DetailView::contextMenuEvent(QContextMenuEvent * event)
{
    QModelIndex index = indexAt(event->pos());
    emit contexMenuRequested(index, QCursor::pos());
}

void DetailView::keyPressEvent(QKeyEvent * event)
{
    if (event->key()==Qt::Key_Return)
    {
        QModelIndexList indexes = selectionModel()->selection().indexes();
        if (indexes.count())
        {
            QModelIndex index = indexes.first();
            if (index.isValid())
            {
                emit trackSelected(index);
            }
        }
    }

    QTableView::keyPressEvent(event);
}

void DetailView::enter()
{
    if (!model())
        return;

    if (!model()->rowCount())
        return;

    setFocus();

    selectRow(0);
//    selectionModel()->select(model()->index(0,0), QItemSelectionModel::Select);


}

void DetailView::selected(const QModelIndex &index)
{
    if (!index.isValid())
        return;

    emit trackSelected(index);
}

void DetailView::setModel(QAbstractItemModel * model)
{
    connect(this, SIGNAL(trackSelected(QModelIndex)), model, SLOT(trackSelected(QModelIndex)));
    QTableView::setModel(model);
}
