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
#include <QtGui>

#include "ui_main_window.h"

#include "core/main_window.h"
#include "core/preferences.h"
#include "core/helper.h"
#include "core/singleton.h"

#include "widgets/filter.h"
#include "widgets/spacer.h"
#include "widgets/seek_slider.h"
#include "widgets/volume_slider.h"

#include "tabs/welcome.h"
#include "tabs/music.h"
#include "tabs/radio.h"
#include "tabs/playlists.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_ui(new Ui::MainWindow)
{
    m_ui->setupUi(this);

    prepare();
}

MainWindow::~MainWindow()
{
    Singleton::drop();

    delete m_ui;
}

void MainWindow::writeSettings()
{
    QSettings settings;

    settings.setValue("maximized", isMaximized());

    settings.setValue("geometry", saveGeometry());
    settings.setValue("state", saveState());

    settings.setValue("volume", Singleton::instance()->player()->volume());
    settings.setValue("muted", Singleton::instance()->player()->muted());

    settings.setValue("shuffle", Singleton::instance()->shuffle());
    settings.setValue("repeat", Singleton::instance()->repeat());

    settings.setValue("splitter", m_ui->splitter->saveState());
}

void MainWindow::readSettings()
{
    QSettings settings;

    if (settings.value("maximized").toBool())
    {
       showMaximized();
    }

    restoreGeometry(settings.value("geometry").toByteArray());
    restoreState(settings.value("state").toByteArray());

    Singleton::instance()->player()->setVolume(settings.value("volume", 1).toReal());
    Singleton::instance()->player()->setMuted(settings.value("muted", false).toBool());

    Singleton::instance()->setShuffle(settings.value("shuffle", false).toBool());
    Singleton::instance()->setRepeat(settings.value("repeat", false).toBool());
    m_ui->actionShuffle->setChecked(Singleton::instance()->shuffle());
    m_ui->actionRepeat->setChecked(Singleton::instance()->repeat());

    m_ui->splitter->restoreState(settings.value("splitter").toByteArray());
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (userReallyWantsToQuit())
    {
        writeSettings();
        event->accept();
    }
    else
    {
        event->ignore();
    }
}

bool MainWindow::userReallyWantsToQuit() const
{
    return true;
}

void MainWindow::prepare()
{
    Singleton::instance();

    toolbar();
    menuActions();
    widgets();
    readSettings();

    QTimer::singleShot(100, this, SLOT(lateActions()));

    Singleton::instance()->setMainWindow(this);
}

void MainWindow::widgets()
{
    m_tabsModel = new QStandardItemModel(this);
    m_ui->places->setModel(m_tabsModel);

    QStandardItem *item1 = new QStandardItem("Music");
    item1->setData("Library");

    QStandardItem *item2 = new QStandardItem("Radio");
    item2->setData("Library");

   // QStandardItem *item3 = new QStandardItem("Ubuntu One");
   // item3->setData("Stores");

   // QStandardItem *item4 = new QStandardItem("Ubuntu One");
   // item4->setData("Playlists");

    item1->setIcon(QIcon::fromTheme("audio-x-generic"));
    item2->setIcon(QIcon::fromTheme("audio-x-generic"));
   // item3->setIcon(QIcon::fromTheme("ubuntuone"));
   // item4->setIcon(QIcon::fromTheme("ubuntuone"));

    m_tabsModel->invisibleRootItem()->appendRow(item1);
    m_tabsModel->invisibleRootItem()->appendRow(item2);
   // m_tabsModel->invisibleRootItem()->appendRow(item3);
   // m_tabsModel->invisibleRootItem()->appendRow(item4);

    m_ui->places->selectionModel()->select(m_ui->places->model()->index(0,0), QItemSelectionModel::Select);
    changeTab(m_ui->places->selectionModel()->selection(), m_ui->places->selectionModel()->selection());

    connect(m_ui->places->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), this, SLOT(changeTab(QItemSelection,QItemSelection)));

    connect(m_ui->actionShuffle, SIGNAL(triggered()), SLOT(playerShuffle()));
    connect(m_ui->actionRepeat, SIGNAL(triggered()), SLOT(playerRepeat()));


    connect(dynamic_cast<QObject*>(Singleton::instance()->player()), SIGNAL(needPlaylist()), this, SLOT(playerDefaultPlaylist()));
}

void MainWindow::toolbar()
{
    setUnifiedTitleAndToolBarOnMac(true);

    QAction *previousAction = new QAction(tr("P&revious"), this);
    previousAction->setStatusTip(tr("Go back to the previous track"));
    previousAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Left));
    previousAction->setPriority(QAction::LowPriority);
    m_actions.insert("previous", previousAction);
    connect(previousAction, SIGNAL(triggered()), SLOT(playerPrevious()));

    QAction *playAction = new QAction(tr("&Play"), this);
    playAction->setStatusTip(tr("Start playback"));
    playAction->setShortcuts(QList<QKeySequence>() << QKeySequence(Qt::Key_Space) << QKeySequence(Qt::Key_MediaPlay));
    playAction->setEnabled(true);
    playAction->setCheckable(true);
    m_actions.insert("play", playAction);
    connect(playAction, SIGNAL(triggered()), SLOT(playerPlay()));

    QAction *forwardAction = new QAction(tr("&Next"), this);
    forwardAction->setStatusTip(tr("Skip to the next track"));
    forwardAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Right));
    forwardAction->setPriority(QAction::LowPriority);
    m_actions.insert("forward", forwardAction);
    connect(forwardAction, SIGNAL(triggered()), SLOT(playerNext()));

    QAction *muteAction = new QAction(this);
    muteAction->setStatusTip(tr("Mute volume"));
    muteAction->setShortcuts(QList<QKeySequence>() << QKeySequence(Qt::CTRL + Qt::Key_E));
    addAction(muteAction);
    m_actions.insert("mute", muteAction);

    connect(muteAction, SIGNAL(triggered()), SLOT(volumeMute()));
    connect(dynamic_cast<QObject*>(Singleton::instance()->player()), SIGNAL(mutedChanged(bool)), this, SLOT(volumeMuteChanged(bool)));
    connect(dynamic_cast<QObject*>(Singleton::instance()->player()), SIGNAL(stateChanged(PlayerState)), this, SLOT(stateChanged(PlayerState)));

    m_filter = new Filter(this);
    m_filter->setPlaceholderText(tr("Type To Search..."));
    m_filter->setMaximumWidth(220);
    connect(m_filter, SIGNAL(filterChanged(QString)), SIGNAL(filterChanged(QString)));
    connect(m_filter, SIGNAL(returnPressed()), SIGNAL(filterEnter()));

    QShortcut * filter = new QShortcut(QKeySequence::Find, this);
    QObject::connect(filter, SIGNAL(activated()), m_filter, SLOT(setFocus()));
    QObject::connect(filter, SIGNAL(activated()), m_filter, SLOT(selectAll()));

    SeekSlider *seekSlider = new SeekSlider(this);
    seekSlider->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);
    seekSlider->setPlayer(Singleton::instance()->player());

    VolumeSlider *volumeSlider = new VolumeSlider(this);
    volumeSlider->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    volumeSlider->setPlayer(Singleton::instance()->player());

    playAction->setIcon(QIcon::fromTheme("media-playback-start", QIcon(":/icons/play.png")));
    forwardAction->setIcon(QIcon::fromTheme("media-skip-forward", QIcon(":/icons/forward.png")));
    previousAction->setIcon(QIcon::fromTheme("media-skip-backward", QIcon(":/icons/previous.png")));
    muteAction->setIcon(QIcon::fromTheme("audio-volume-high", QIcon(":/icons/mute.png")));


    m_ui->toolBar->addAction(previousAction);
    m_ui->toolBar->addAction(playAction);
    m_ui->toolBar->addAction(forwardAction);
    m_ui->toolBar->addWidget(new Spacer(this));
    m_ui->toolBar->addWidget(seekSlider);
    m_ui->toolBar->addWidget(new Spacer(this));
    m_ui->toolBar->addAction(muteAction);
    m_ui->toolBar->addWidget(volumeSlider);
    m_ui->toolBar->addWidget(new Spacer(this));
    m_ui->toolBar->addWidget(m_filter);

    m_ui->toolBar->setContextMenuPolicy(Qt::PreventContextMenu);

    foreach(QAction *action, m_actions.values())
    {
        action->setAutoRepeat(false);

        action->setShortcutContext(Qt::ApplicationShortcut);
    }
}

void MainWindow::playerDefaultPlaylist()
{
    const int row  = m_ui->places->currentIndex().row();

    if (row==-1)
        return;

    QWidget *widget = m_tabs.value(row);

    if (!widget)
        return;

    Music *music = qobject_cast<Music*>(widget);
    if (music)
    {
        Singleton::instance()->player()->setPlaylist(music->defaultPlaylist());
    }
}

void MainWindow::playerShuffle()
{
    QAction *action = qobject_cast<QAction*>(sender());
    Singleton::instance()->setShuffle(action->isChecked());
}

void MainWindow::playerRepeat()
{
    QAction *action = qobject_cast<QAction*>(sender());
    Singleton::instance()->setRepeat(action->isChecked());
}

void MainWindow::stateChanged(const PlayerState &state)
{
    if (state == PlayerPlayingState)
    {
        m_actions.value("play")->setChecked(true);
    }
    else if (state == PlayerStoppedState || state == PlayerPausedState)
    {
        m_actions.value("play")->setChecked(false);
    }
}

void MainWindow::volumeMute()
{
    Singleton::instance()->player()->setMuted(!Singleton::instance()->player()->muted());
}

void MainWindow::volumeMuteChanged(const bool &mute)
{
    QAction *action = m_actions.value("mute");

    if (mute)
    {
        action->setIcon(QIcon::fromTheme("audio-volume-muted"));
    }
    else
    {
        action->setIcon(QIcon::fromTheme("audio-volume-high"));
    }
}

void MainWindow::menuActions()
{
    connect(m_ui->actionQuit, SIGNAL(triggered()), this, SLOT(close()));
    connect(m_ui->actionSwitchInterface, SIGNAL(triggered()), this, SLOT(menuAction()));
    connect(m_ui->actionAboutQt, SIGNAL(triggered()), this, SLOT(menuAction()));
    connect(m_ui->actionAbout, SIGNAL(triggered()), this, SLOT(menuAction()));;
    connect(m_ui->actionPreferences, SIGNAL(triggered()), this, SLOT(menuAction()));
    connect(m_ui->actionRescan, SIGNAL(triggered()), this, SLOT(rescan()));
}

void MainWindow::menuAction()
{
    QObject *object = sender();

    if (object==m_ui->actionAbout)
    {
        QString text = tr("<b>%1 %2</b><br/>"
                "Simple music player."
                "<br/><br/>"
                "<i>Licence GPLv2+</i><br/><br/>"
                "<b>Author</b>: Marcin Baszczewski <marcin@baszczewski.pl>").arg(APPNAME, VERSION);

        QMessageBox::about(this, APPNAME, text);
    }

    if (object==m_ui->actionAboutQt)
    {
        QMessageBox::aboutQt(this);
    }

    if (object==m_ui->actionPreferences)
    {
        Preferences preferences;
        preferences.exec();
    }
}

void MainWindow::lateActions()
{
}

void MainWindow::changeTab(const QItemSelection &current, const QItemSelection &previous)
{
    Q_UNUSED(previous);

//    disconnect(SIGNAL(filterChanged(QString)));

    if (!current.count())
        return;

    const int row = current.indexes().first().row();
    const int previousRow = previous.indexes().first().row();

    changeTab(row, previousRow);
}

void MainWindow::changeTab(const int &row, const int &previousRow)
{
    this->setCursor(Qt::WaitCursor);

    foreach(QWidget *widget, m_tabs.values())
    {
        widget->hide();
        m_ui->widget->layout()->removeWidget(widget);
    }

    if (m_tabs.contains(row))
    {
        m_ui->widget->layout()->addWidget(m_tabs.value(row));
        m_tabs.value(row)->show();
    }
    else
    {
        m_tabs.insert(row, tab(row));
        m_ui->widget->layout()->addWidget(m_tabs.value(row));
    }

    if (m_tabs.contains(previousRow)) // save filter state for old tab
    {
        disconnect(this, SIGNAL(filterChanged(QString)), m_tabs.value(previousRow), SIGNAL(filterChanged(QString)));
        disconnect(this, SIGNAL(filterEnter()), m_tabs.value(previousRow), SIGNAL(filterEnter()));
        m_tabs.value(previousRow)->setProperty("filter", m_filter->text());
    }

    // restore filter state
    m_filter->blockSignals(true);
    m_filter->setText(m_tabs.value(row)->property("filter").toString());
    m_filter->blockSignals(false);

    connect(this, SIGNAL(filterChanged(QString)), m_tabs.value(row), SIGNAL(filterChanged(QString)));
    connect(this, SIGNAL(filterEnter()), m_tabs.value(row), SIGNAL(filterEnter()));

    this->setCursor(Qt::ArrowCursor);
}

QWidget *MainWindow::tab(const int &index)
{
    switch(index)
    {
        case 0:
        {
            if (Singleton::instance()->library()->isEmpty())
            {
                Welcome *welcome  = new Welcome(this);
                connect(welcome, SIGNAL(finished()), SLOT(libraryScaned()));
                return welcome;
            }
            else
            {
                return new Music(this);
            }
        }
        case 1:
            return new Radio(this);
        default:
            return new Music(this);
    }

    return 0;
}

void MainWindow::playerPlay()
{
    QAction *action = qobject_cast<QAction*>(sender());
    if (!action)
        return;

    Singleton::instance()->player()->play();
}

void MainWindow::playerNext()
{
    Singleton::instance()->player()->next();
}

void MainWindow::playerPrevious()
{
    Singleton::instance()->player()->previous();
}

void MainWindow::libraryScaned()
{
    bool show = m_tabs[0]->isVisible();

    delete m_tabs[0];
    m_tabs.remove(0);

    if (show)
    {
        changeTab(0,0);
    }

}

void MainWindow::rescan()
{
    Singleton::instance()->library()->rescan();
}
