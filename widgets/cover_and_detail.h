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

#ifndef COVER_AND_DETAIL_H
#define COVER_AND_DETAIL_H

#include <QWidget>

#include "models/cover_model.h"

class QModelIndex;

class CoverModel;
class CoverView;
class DetailModel;
class DetailView;
class Breadcrumb;

class CoverAndDetail : public QWidget
{
    Q_OBJECT
public:
    explicit CoverAndDetail(QWidget *parent = 0);
    explicit CoverAndDetail(const CoverType &type, QWidget *parent = 0);

    DetailModel *model();

protected:
    void prepare();
    void keyPressEvent(QKeyEvent * event);
    void resizeEvent(QResizeEvent *event);

protected slots:
    void coverSelected(const QModelIndex &index);
    void backToCovers();

signals:
    void resized();

private:
    CoverType m_type;
    CoverModel *m_coverModel;
    CoverView *m_coverView;
    DetailModel *m_detailModel;
    DetailView *m_detailView;
    Breadcrumb *m_breadcrumb;
};

#endif // COVER_AND_DETAIL_H
