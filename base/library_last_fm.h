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

#ifndef LIBRARY_LAST_FM_H
#define LIBRARY_LAST_FM_H

#include <QObject>
#include <QHash>

class QNetworkAccessManager;
class QNetworkReply;
class QImage;

class LibraryLastFM : public QObject
{
    Q_OBJECT
public:
    explicit LibraryLastFM(QObject *parent = 0);

public slots:
    void findArtistCover(const int &id, const QString &artist);
    void findAlbumCover(const int &id, const QString &album);

    bool isBusy() const;

protected slots:
    void replyFinished(QNetworkReply * reply);

signals:
    void ready();

protected:
    QImage processImage(const QImage &img) const;
    void downloadImage(const int &id, const QString &url, const QString &directory);
    QString key() const;
    QString host() const;
    bool parse(const int &id, const QByteArray &data, const QString &what);

signals:
    void error(const int &code);


private:
    QNetworkAccessManager *m_manager;
    QList<QNetworkReply*> m_replays;
};

#endif // LIBRARY_LAST_FM_H
