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

#ifndef SINGLETON_H
#define SINGLETON_H

#include <QMultiHash>

class QObject;
class Player;
class Library;
class InterfaceControl;
class QImage;
class Notifications;
class MainWindow;
class MPRIS;

enum PluginType
{
    PluginControl
};

class Singleton
{
public:
    static Singleton* instance();
    static void drop();

    Library *library();
    Player *player();

    QList<InterfaceControl*> remoteControls() const;

    bool shuffle() const;
    bool repeat() const;
    void setShuffle(const bool &shuffle);
    void setRepeat(const bool &repeat);

    void show(const QString &title, const QString &subtitle, const QImage &image);
    void raise();

    void setMainWindow(MainWindow *window);

private:
    Singleton();
    ~Singleton();
    Singleton(const Singleton &);
    Singleton& operator=(const Singleton &);
    static Singleton* m_instance;

    QList<QObject*> m_plugins;

    bool m_repeat;
    bool m_shuffle;

    Library *m_library;
    Player *m_player;
    Notifications *m_notifications;
    MPRIS *m_mpris;

    MainWindow *m_window;
};

#endif // SINGLETON_H
