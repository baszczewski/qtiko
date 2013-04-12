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
#include <QImage>
#include <QThread>

#include "asynchronous_image.h"
#include "asynchronous_image_worker.h"

AsynchronousImage::AsynchronousImage(QObject *parent)
    : QObject(parent)
    , m_worker(0)
{
    // worker
    m_workerThread = new QThread(this);

    m_worker = new AsynchronousImageWorker();
    m_worker->moveToThread(m_workerThread);

    connect(this, SIGNAL(loadImageFWD(QString)), m_worker, SLOT(loadImage(QString)));
    connect(m_worker, SIGNAL(imageLoaded(QString,QImage)), SLOT(image(QString,QImage)));
    connect(m_worker, SIGNAL(imageLoaded(QString,QImage)), SIGNAL(imageLoaded(QString,QImage)));
    connect(m_worker, SIGNAL(finished()), m_worker, SLOT(deleteLater()));
    connect(m_worker, SIGNAL(finished()), m_workerThread, SLOT(quit()));

    m_workerThread->start();
}

AsynchronousImage::~AsynchronousImage()
{
    m_worker->release();
    m_workerThread->wait();
}

void AsynchronousImage::loadImage(const QString &url)
{
    if (m_requested.contains(url))
        return;

    m_requested.append(url);
    emit loadImageFWD(url);
}

void AsynchronousImage::image(const QString &url, const QImage &image)
{
    Q_UNUSED(image);
    m_requested.removeOne(url);
}
