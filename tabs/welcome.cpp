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

#include <QFileDialog>
#include <QSettings>

#include "tabs/welcome.h"
#include "ui_welcome.h"
#include "core/helper.h"
#include "core/singleton.h"

#include "base/library.h"

Welcome::Welcome(QWidget *parent)
    : Tab(parent)
    , m_ui(new Ui::Welcome)
{
    m_ui->setupUi(this);

    m_ui->progress->hide();

    m_directory = Helper::musicDirectory();

    m_ui->actionDefault->setContent(tr("Use default"), tr("Default: %1.").arg(m_directory), QIcon::fromTheme("folder"));
    m_ui->actionChange->setContent(tr("Choose"), tr("Select the location of your music collection."), QIcon::fromTheme("folder"));

    connect(m_ui->actionDefault, SIGNAL(clicked()), this, SLOT(scan()));
    connect(m_ui->actionChange, SIGNAL(clicked()), this, SLOT(changeAndScan()));

    connect(Singleton::instance()->library(), SIGNAL(collectionDone()), SLOT(done()));
    connect(Singleton::instance()->library(), SIGNAL(collectionHashed(int)), SLOT(hashed(int)));
    connect(Singleton::instance()->library(), SIGNAL(collectionProgress(qreal)), SLOT(progress(qreal)));
}

Welcome::~Welcome()
{
    delete m_ui;
}

void Welcome::changeAndScan()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Select Music Directory"), QString(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    if (!dir.isEmpty())
    {
        m_directory = dir;
        scan();
    }
}

void Welcome::scan()
{
    m_ui->progress->setMinimumWidth(m_ui->actionChange->width());
    m_ui->progress->setMaximumWidth(m_ui->actionChange->width());
    m_ui->progress->show();
    m_ui->actionChange->hide();
    m_ui->actionDefault->hide();

    QSettings settings;
    settings.setValue("musicDirectory", m_directory);

    Singleton::instance()->library()->setDirectory(m_directory);
    Singleton::instance()->library()->rescan();
}

void Welcome::progress(const qreal &value)
{
    m_ui->progress->setValue(m_ui->progress->maximum()*value);
}

void Welcome::done()
{
    if (m_ui->progress->maximum()==0)
        m_ui->progress->setMaximum(100);

    m_ui->progress->setValue(m_ui->progress->maximum());

    emit finished();
}

void Welcome::hashed(const int &value)
{
    Q_UNUSED(value);
    m_ui->progress->setMaximum(100);
    m_ui->progress->setValue(0);
}
