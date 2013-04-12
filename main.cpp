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

#include <QTextCodec>
#include <QTranslator>
#include <QMessageBox>
#include <QDesktopServices>
#include <QFile>
#include <QDir>
#include <QDebug>

#include "singleapplication.h"

#include "core/main_window.h"
#include "core/helper.h"

int main(int argc, char *argv[])
{
    QtSingleApplication app(argc, argv);

    QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));

    if (app.isRunning())
    {
        qDebug() << "qtiko is running";
        return 0;
    }

    QFile file(Helper::preferedStyleSheet());
    file.open(QFile::ReadOnly);
    app.setStyleSheet(QLatin1String(file.readAll()));

    QTranslator translator;
    if (translator.load("qt_pl",":/i18n"))
        QApplication::instance()->installTranslator(&translator);

    app.setOrganizationName("Baszczewski");
    app.setApplicationName("Qtiko");
    app.setApplicationVersion(VERSION);
    app.setWindowIcon(QIcon(":/icons/main.png"));

    QString directory = QDesktopServices::storageLocation(QDesktopServices::DataLocation);
    if (directory.isEmpty())
        directory = QDir::homePath() + "/." + QCoreApplication::applicationName();

    if (!QFile::exists(directory))
    {
        QDir dir;
        dir.mkpath(directory);
    }

    if (!QDir::setCurrent(directory))
        qDebug() << "Directory problem.";

    MainWindow mainWindow;
    mainWindow.show();

    return app.exec();
}
