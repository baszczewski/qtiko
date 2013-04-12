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

#include "core/singleton.h"
#include "core/main_window.h"

#include "models/cover_model.h"
#include "models/detail_model.h"

#include "widgets/cover_view.h"
#include "widgets/detail_view.h"
#include "widgets/cover_and_detail.h"

#include "ui_music.h"

#include "tabs/music.h"

Music::Music(QWidget *parent)
    : Tab(parent)
    , m_ui(new Ui::Music)
{
    m_ui->setupUi(this);

    prepare();
}

Music::~Music()
{
    delete m_ui;
}

void Music::prepare()
{
    tabChanged(0);

//    m_ui->categories->setMovable(true);

    m_ui->categories->setDrawBase(false);
    m_ui->categories->addTab(tr("Songs"));
    m_ui->categories->addTab(tr("Albums"));
    m_ui->categories->addTab(tr("Artists"));
   // m_ui->categories->addTab(tr("Folders"));

    connect(m_ui->categories, SIGNAL(currentChanged(int)), this, SLOT(tabChanged(int)));
}

void Music::tabChanged(const int &tab)
{
    qDebug() << tab;
    disconnect(this, SIGNAL(filterEnter()), 0, 0);
    disconnect(this, SIGNAL(filterChanged(QString)), 0, 0);

    foreach(QWidget *widget, m_widgets.values())
    {
        widget->hide();
        m_ui->main->layout()->removeWidget(widget);
    }

    if (m_widgets.contains(tab))
    {
        m_ui->main->layout()->addWidget(m_widgets.value(tab));
        m_widgets.value(tab)->show();
    }
    else
    {
        QWidget *widget = this->tab(tab);
        m_widgets.insert(tab, widget);
        m_ui->main->layout()->addWidget(widget);
    }

    QAbstractItemView *view = qobject_cast<QAbstractItemView*>(m_widgets.value(tab));
    if (view)
    {
        QAbstractItemModel *model = qobject_cast<QAbstractItemModel*>(view->model());
        connect(this, SIGNAL(filterChanged(QString)), model, SLOT(filter(QString)));
        connect(this, SIGNAL(filterEnter()), view, SLOT(enter()));
    }
}

QWidget *Music::tab(const int &index)
{
    switch(index)
    {
        case 0:
        {
            DetailModel *m_model = new DetailModel(this);
            DetailView *view = new DetailView(this);
            view->setModel(m_model);

            m_model->getTracks(QString(), 1024);
            return view;
        }
        case 1:
        {
            CoverAndDetail *view = new CoverAndDetail(this);
            return view;
        }
        case 2:
        {
            CoverAndDetail *view = new CoverAndDetail(CoverArtist, this);
            return view;
        }
        case 3:
        {
            CoverAndDetail *view = new CoverAndDetail(CoverFolder, this);
            return view;
        }
    }

    return 0;
}

Playlist *Music::defaultPlaylist()
{
    DetailView *view = qobject_cast<DetailView*>(m_widgets.value(m_ui->categories->currentIndex()));
    if (view)
    {
        Playlist *model = dynamic_cast<Playlist*>(view->model());

        if (model)
            return model;
        else
            return 0;
    }

    CoverAndDetail *coverAndDetail = qobject_cast<CoverAndDetail*>(m_widgets.value(m_ui->categories->currentIndex()));
    if (coverAndDetail)
    {
        Playlist *model = dynamic_cast<Playlist*>(coverAndDetail->model());

        if (model)
            return model;
        else
            return 0;
    }

    return 0;
}
