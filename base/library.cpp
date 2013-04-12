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
#include <QApplication>
#include <QThread>

#include "library.h"
#include "library_worker.h"
#include "library_collection.h"
#include "library_last_fm.h"

Library::Library(QObject *parent)
    : QObject(parent)
{
    qDebug() << "fm";
    LibraryLastFM *fm = new LibraryLastFM(this);
    fm->findArtistCover(1, "Scooter");

    qRegisterMetaType<CoverType>();

    // worker
    m_workerThread = new QThread(this);

    m_worker = new LibraryWorker();
    m_worker->moveToThread(m_workerThread);

    connect(this, SIGNAL(getTracksFWD(QString,CoverType,int,int)), m_worker, SLOT(getTracks(QString,CoverType,int,int)));
    connect(this, SIGNAL(getCoversFWD(QString,CoverType,int)), m_worker, SLOT(getCovers(QString,CoverType,int)));
    connect(this, SIGNAL(moreFWD(int)), m_worker, SLOT(more(int)));
    connect(m_worker, SIGNAL(itemsCountReceived(int)), this, SIGNAL(itemsCountReceived(int)));
    connect(m_worker, SIGNAL(tracksReceived(Tracks)), this, SIGNAL(tracksReceived(Tracks)));
    connect(m_worker, SIGNAL(coversReceived(Covers)), this, SIGNAL(coversReceived(Covers)));
    connect(m_worker, SIGNAL(busy(bool)), this, SLOT(busy(bool)));
    connect(m_worker, SIGNAL(finished()), m_worker, SLOT(deleteLater()));
    connect(m_worker, SIGNAL(finished()), m_workerThread, SLOT(quit()));

    m_workerThread->start();

    m_workerBusy = false;

    // collection
    m_collectionThread = new QThread(this);

    m_collection = new LibraryCollection();
    m_collection->moveToThread(m_collectionThread);

    connect(this, SIGNAL(rescanFWD(QString)), m_collection, SLOT(rescan(QString)));
    connect(this, SIGNAL(clearFWD()), m_collection, SLOT(clear()));
    connect(m_collection, SIGNAL(collectionDone()), SIGNAL(collectionDone()));
    connect(m_collection, SIGNAL(collectionHashed(int)), SIGNAL(collectionHashed(int)));
    connect(m_collection, SIGNAL(collectionProgress(qreal)), SIGNAL(collectionProgress(qreal)));
    connect(m_collection, SIGNAL(finished()), m_collection, SLOT(deleteLater()));
    connect(m_collection, SIGNAL(finished()), m_collectionThread, SLOT(quit()));

    m_collectionThread->start();
}

Library::~Library()
{
    m_collection->release();
    m_worker->release();

    m_collectionThread->wait();
    m_workerThread->wait();
}

void Library::rescan()
{
    qDebug() << "rescan";
    emit rescanFWD(m_directory);
}

void Library::clear()
{
    emit clearFWD();
}

void Library::setDirectory(const QString &directory)
{
    m_directory = directory;
}

void Library::getTracks(const QString &filter, const int &limit)
{
    if (!m_workerBusy)
        emit getTracksFWD(filter, CoverAlbum, -1, limit);
}

void Library::getTracks(const QString &filter, const CoverType &type, const int parent, const int &limit)
{
    if (!m_workerBusy)
        emit getTracksFWD(filter, type, parent, limit);
}

void Library::getCovers(const QString &filter, const CoverType &type, const int &limit)
{
    if (!m_workerBusy)
        emit getCoversFWD(filter, type, limit);
}

void Library::more(const int &limit)
{
    if (!m_workerBusy)
        emit moreFWD(limit);
}

void Library::busy(const bool &value)
{
    m_workerBusy = value;
}

bool Library::isEmpty()
{
    return m_collection->isEmpty(); // direct!
}
