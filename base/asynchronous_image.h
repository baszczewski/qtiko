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

#ifndef ASYNCHRONOUS_IMAGE_H
#define ASYNCHRONOUS_IMAGE_H

#include <QObject>

class QImage;
class QThread;
class AsynchronousImageWorker;

class AsynchronousImage: public QObject
{
    Q_OBJECT
public:
    explicit AsynchronousImage(QObject *parent = 0);
    ~AsynchronousImage();

public slots:
    void loadImage(const QString &url);

protected slots:
    void image(const QString &url, const QImage &image);

signals:
    void imageLoaded(const QString &url, const QImage &image);
    void loadImageFWD(const QString &url);

private:
    AsynchronousImageWorker *m_worker;
    QThread *m_workerThread;
    QList<QString> m_requested;
};

#endif // ASYNCHRONOUS_IMAGE_H
