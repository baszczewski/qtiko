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

#include <QDebug>
#include "qmath.h"

#include "models/cover_model.h"
#include "base/library.h"
#include "base/asynchronous_image.h"

CoverModel::CoverModel(QObject *parent)
    : QAbstractListModel(parent)
{
    m_type = CoverAlbum;
    prepare();
}

CoverModel::CoverModel(const CoverType &type, QObject *parent)
    : QAbstractListModel(parent)
{
    m_type = type;
    prepare();
}

CoverModel::~CoverModel()
{
}

void CoverModel::prepare()
{
    qRegisterMetaType<Covers>();
    qRegisterMetaType<CoverType>();

    m_clear = false;

    m_loader = new AsynchronousImage(this);
    connect(m_loader, SIGNAL(imageLoaded(QString,QImage)), SLOT(imageLoaded(QString,QImage)));

    m_library = new Library(this);

    connect(m_library, SIGNAL(itemsCountReceived(int)), SLOT(itemsCountReceived(int)));
    connect(m_library, SIGNAL(coversReceived(Covers)), SLOT(insert(Covers)));

    m_library->getCovers(QString(), m_type, 1024);
}

void CoverModel::clear()
{
    beginRemoveRows(QModelIndex(), 0, m_covers.count()-1);
    m_covers.clear();
    m_clear = false;
    m_loaded.clear();
    m_pixmaps.clear();
    endRemoveRows();
}

bool CoverModel::canFetchMore(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    if (m_covers.count() < m_count)
        return true;
    else
        return false;
}

void CoverModel::fetchMore(const QModelIndex &parent)
{
    Q_UNUSED(parent);

    int remainder = m_count - m_covers.count();
    int itemsToFetch = qMin(qMax(1024, m_count/3), remainder);

    m_library->more(itemsToFetch);
}

int CoverModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_covers.count();
}

QVariant CoverModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (index.row() >= m_covers.count() || index.row() < 0)
        return QVariant();

    if (role == Qt::DisplayRole)
    {
        return m_covers.at(index.row()).title;
    }

    if (role == Qt::UserRole)
    {
        return m_covers.at(index.row());
    }

    if (role == (Qt::UserRole+1)) // pixmap (load asynchronous)
    {
        const QString image = m_covers.at(index.row()).image;

        if (!image.isEmpty())
        {
            if (!m_loaded.contains(image))
            {
                m_loader->loadImage(image);
            }
            else
            {
                if (m_pixmaps.contains(image))
                    return m_pixmaps.value(image);
            }
        }

        return QPixmap();
    }

    return QVariant();
}

bool CoverModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    Q_UNUSED(role);

    if (!index.isValid())
        return false;

    if (index.row() >= m_covers.count() || index.row() < 0)
        return false;

    m_covers[index.row()] = value.value<Cover>();

    emit dataChanged(index, index);

    return true;
}

void CoverModel::insert(const Covers &covers)
{
    if (m_clear) // got result - clear (prevent blinking)
        clear();

    beginInsertRows(QModelIndex(), m_covers.count(), m_covers.count()+covers.count()-1);
    m_covers.append(covers);
    endInsertRows();
}

void CoverModel::itemsCountReceived(const int &count)
{
    if (m_clear && count == 0) // got result - clear (prevent blinking)
        clear();

    m_count = count;
}

void CoverModel::imageLoaded(const QString &url, const QImage &image)
{
    m_loaded.append(url);

    if (image.isNull())
        return;

    m_pixmaps.insert(url, QPixmap::fromImage(image));

    // FIX ME: IT CAN BE TO SLOW FOR BIG COLLECTION
    for (int i=0; i<m_covers.count(); ++i)
    {
        if (m_covers[i].image==url)
        {
            QModelIndex index = this->index(i);
            emit dataChanged(index, index);
        }
    }
}

void CoverModel::clearPixmaps()
{
    beginResetModel();
    m_loaded.clear();
    m_pixmaps.clear();
    endResetModel();
}
