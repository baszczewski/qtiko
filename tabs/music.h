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

#ifndef MUSIC_H
#define MUSIC_H

#include "core/tab.h"
#include <QPointer>
#include <QHash>

namespace Ui
{
    class Music;
}

class Playlist;
class QAbstractItemModel;
class QModelIndex;

class Music : public Tab
{
    Q_OBJECT
    
public:
    explicit Music(QWidget *parent = 0);
    ~Music();

    Playlist *defaultPlaylist();

protected:
    void prepare();
    QWidget *tab(const int &index);

protected slots:
    void tabChanged(const int &tab);

private:
    Ui::Music *m_ui;
    QHash<int, QWidget*> m_widgets;
};

#endif // MUSIC_H
