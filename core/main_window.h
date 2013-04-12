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

#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>
#include <QPointer>
#include <QHash>

#include "base/player.h"
#include "base/library.h"

class QItemSelection;
class QStandardItemModel;
class Filter;

namespace Ui
{
    class MainWindow;
}
class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void writeSettings();
    void readSettings();
    bool userReallyWantsToQuit() const;
    void closeEvent(QCloseEvent *event);
    void prepare();
    void toolbar();
    void menuActions();
    QWidget *tab(const int &index);
    void widgets();

protected slots:
    void changeTab(const QItemSelection &current, const QItemSelection &previous);
    void changeTab(const int &row, const int &previousRow);
    void menuAction();
    void lateActions();
    void volumeMute();
    void volumeMuteChanged(const bool &mute);
    void stateChanged(const PlayerState &state);
    void playerPlay();
    void playerNext();
    void playerPrevious();
    void playerShuffle();
    void playerRepeat();
    void playerDefaultPlaylist();
    void libraryScaned();
    void rescan();

signals:
    void filterChanged(const QString &text);
    void filterEnter();
    
private:
    Ui::MainWindow *m_ui;
    QHash<QString, QAction*> m_actions;
    QStandardItemModel *m_tabsModel;
    QHash<int, QWidget*> m_tabs;
    Filter *m_filter;
};

#endif // MAIN_WINDOW_H
