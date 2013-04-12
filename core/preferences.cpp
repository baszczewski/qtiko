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

#include <QSettings>
#include <QFileDialog>

#include "core/preferences.h"
#include "core/helper.h"
#include "ui_preferences.h"

Preferences::Preferences(QWidget *parent)
    : QDialog(parent)
    , m_ui(new Ui::Preferences)
{
    m_ui->setupUi(this);

    load();

    connect(this, SIGNAL(accepted()), this, SLOT(save()));
    connect(m_ui->changeMusicDirectory, SIGNAL(clicked()), this, SLOT(changeMusicDirectory()));
}

Preferences::~Preferences()
{
    delete m_ui;
}

void Preferences::load()
{
    m_ui->musicDirectory->setText(Helper::musicDirectory());

}

void Preferences::save()
{
    QSettings settings;
    settings.setValue("musicDirectory", m_ui->musicDirectory->text());
}

void Preferences::changeMusicDirectory()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Select Music Directory"), QString(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    if (!dir.isEmpty())
    {
        m_ui->musicDirectory->setText(dir);
    }
}
