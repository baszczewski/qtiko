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

#ifndef COVER_MODEL_H
#define COVER_MODEL_H

#include <QAbstractListModel>
#include <QPixmap>
#include <QVariant>
#include "types/cover.h"

class Library;
class AsynchronousImage;

class CoverModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit CoverModel(QObject *parent = 0);
    explicit CoverModel(const CoverType &type, QObject *parent = 0);
    ~CoverModel();
    
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

protected:
    void prepare();
    void clear();
    bool canFetchMore(const QModelIndex &parent) const;
    void fetchMore(const QModelIndex &parent);

public slots:
    void clearPixmaps();

protected slots:
    void insert(const Covers &covers);
    void itemsCountReceived(const int &count);
    void imageLoaded(const QString &url, const QImage &image);

private:
    QHash<QString, QPixmap> m_pixmaps; // pixmaps for items
    QList<QString> m_loaded;

    AsynchronousImage *m_loader;

    Library *m_library;
    Covers m_covers;
    int m_count;
    bool m_clear;

    CoverType m_type;

};

#endif // COVER_MODEL_H
