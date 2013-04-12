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

#include <QVBoxLayout>
#include <QModelIndex>
#include <QDebug>
#include <QKeyEvent>

#include "widgets/cover_and_detail.h"
#include "widgets/cover_view.h"
#include "widgets/detail_view.h"
#include "widgets/breadcrumb.h"
#include "models/detail_model.h"

CoverAndDetail::CoverAndDetail(QWidget *parent)
    : QWidget(parent)
{
    m_type = CoverAlbum;
    prepare();
}

CoverAndDetail::CoverAndDetail(const CoverType &type, QWidget *parent)
    : QWidget(parent)
{
    m_type = type;
    prepare();;
}

void CoverAndDetail::prepare()
{
    m_detailModel = 0;
    m_detailView = 0;
    m_breadcrumb = 0;

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setSpacing(0);
    layout->setMargin(0);

    m_coverModel = new CoverModel(m_type, this);
    m_coverView = new CoverView(this);
    m_coverView->setModel(m_coverModel);

    layout->addWidget(m_coverView);

    connect(m_coverView, SIGNAL(coverSelected(QModelIndex)), SLOT(coverSelected(QModelIndex)));
}

void CoverAndDetail::coverSelected(const QModelIndex &index)
{
    if (!m_detailView)
    {
        m_breadcrumb = new Breadcrumb(this);
        connect(m_breadcrumb, SIGNAL(back()), SLOT(backToCovers()));

        if (m_type == CoverFolder)
            m_breadcrumb->setDefaultCover(QPixmap(":/images/folder.png"));
        else
            m_breadcrumb->setDefaultCover(QPixmap(":/images/album.png"));

        m_detailModel = new DetailModel(this);
        m_detailView = new DetailView(this);
        m_detailView->setModel(m_detailModel);
    }

    Cover data = index.data(Qt::UserRole).value<Cover>();
    qDebug() << "get" << data.id;

    m_breadcrumb->setText(data.title);
    m_breadcrumb->setCover(index.data(Qt::UserRole+1).value<QPixmap>());

    m_detailModel->getTracks(QString(), m_type, data.id, 1024);

    m_coverView->hide();
    layout()->removeWidget(m_coverView);

    layout()->addWidget(m_breadcrumb);
    m_breadcrumb->show();
    layout()->addWidget(m_detailView);
    m_detailView->show();

    m_detailView->setFocus();
    m_detailView->enter();
}

void CoverAndDetail::backToCovers()
{
    layout()->addWidget(m_coverView);
    m_coverView->show();

    m_breadcrumb->hide();
    layout()->removeWidget(m_breadcrumb);
    m_detailView->hide();
    layout()->removeWidget(m_detailView);
}

void CoverAndDetail::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    emit resized();
}

DetailModel *CoverAndDetail::model()
{
    return m_detailModel;
}

void CoverAndDetail::keyPressEvent(QKeyEvent * event)
{
    if (event->key()==Qt::Key_Escape && !m_coverView->isVisible())
    {
        backToCovers();
    }
    QWidget::keyPressEvent(event);
}
