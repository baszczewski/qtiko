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

#ifndef WELCOME_H
#define WELCOME_H

#include "core/tab.h"

namespace Ui
{
    class Welcome;
}

class Welcome : public Tab
{
    Q_OBJECT

public:
    explicit Welcome(QWidget *parent = 0);
    ~Welcome();

protected slots:
    void changeAndScan();
    void scan();
    void hashed(const int &value);
    void progress(const qreal &value);
    void done();

signals:
    void finished();

private:
    Ui::Welcome *m_ui;
    QString m_directory;
};

#endif // WELCOME_H
