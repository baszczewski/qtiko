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
#include <QtNetwork>
#include <QImage>

#include "library_last_fm.h"

LibraryLastFM::LibraryLastFM(QObject *parent)
    : QObject(parent)
{
    if (!QDir("artist").exists())
        QDir().mkdir("artist");

    if (!QDir("album").exists())
        QDir().mkdir("album");

    m_manager = new QNetworkAccessManager(this);
    connect(m_manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));
}

void LibraryLastFM::findArtistCover(const int &id, const QString &artist)
{
    if (artist.isEmpty())
        return;

    QUrl url(host());
    url.addQueryItem("api_key", key());
    url.addQueryItem("method", "artist.search");
    url.addQueryItem("artist", artist);
    url.addQueryItem("limit", "5");

    QNetworkReply *reply = m_manager->get(QNetworkRequest(url));
    reply->setProperty("artist_id", id);
    m_replays << reply;

    qDebug() << artist << url;
}

void LibraryLastFM::findAlbumCover(const int &id, const QString &album)
{
    if (album.isEmpty())
        return;

    QUrl url(host());
    url.addQueryItem("api_key", key());
    url.addQueryItem("method", "album.search");
    url.addQueryItem("album", album);
    url.addQueryItem("limit", "5");

    QNetworkReply *reply = m_manager->get(QNetworkRequest(url));
    reply->setProperty("album_id", id);
    m_replays << reply;
}

void LibraryLastFM::replyFinished(QNetworkReply * reply)
{
    if (!reply)
        return;

    m_replays.removeAll(reply);

    if (reply->error() == QNetworkReply::NoError)
    {
        const QByteArray data = reply->readAll();

        // artist
        if (reply->property("artist_id").isValid())
        {
            if (!parse(reply->property("artist_id").toInt(), data, "artist"))
                emit ready();
        }

        // album
        if (reply->property("album_id").isValid())
        {
            if (!parse(reply->property("album_id").toInt(), data, "album"))
                emit ready();
        }

        // artist cover
        if (reply->property("artist").isValid())
        {
            QImage image;
            image.loadFromData(data);
            if (!image.isNull())
            {
                processImage(image).save(QString("artist/%1.png").arg(reply->property("artist").toInt()));
            }
            emit ready();
        }

        // album cover
        if (reply->property("album").isValid())
        {
            QImage image;
            image.loadFromData(data);
            if (!image.isNull())
            {
                processImage(image).save(QString("album/%1.png").arg(reply->property("album").toInt()));
            }
            emit ready();
        }
    }
    else
    {
        int httpStatus = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        emit error(httpStatus);
        emit ready();
    }

    reply->deleteLater();
}

QImage LibraryLastFM::processImage(const QImage &img) const
{
    if (img.height()>img.width())
    {
        return img.copy(QRect(0,0,img.width(),img.width())).scaled(QSize(136,136), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    }
    else
    {
        const int x = img.width()/2-img.height()/2;
        return img.copy(QRect(x,0,img.height(),img.height())).scaled(QSize(136,136), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    }
}

bool LibraryLastFM::parse(const int &id, const QByteArray &data, const QString &what)
{
    QXmlStreamReader xml(data);

    while (!xml.atEnd() && !xml.hasError())
    {
        xml.readNext();

        QXmlStreamReader::TokenType token = xml.readNext();
        if(token == QXmlStreamReader::StartDocument)
        {
            qDebug() << xml.name();
            continue;
        }

        if (xml.name()=="lfm")
        {
            QXmlStreamAttributes attributes = xml.attributes();
            if(attributes.hasAttribute("status"))
            {
                if (attributes.value("status").toString()!="ok")
                    return false;
            }
        }

        if (xml.name()=="artist")
        {
            xml.readNext();

            while(!(xml.tokenType() == QXmlStreamReader::EndElement && xml.name() == what))
            {
                if (xml.name()=="image" && xml.tokenType() == QXmlStreamReader::StartElement)
                {
                    QXmlStreamAttributes attributes = xml.attributes();
                    if (attributes.value("size").toString()=="extralarge")
                    {
                        xml.readNext();
                        if(xml.tokenType() != QXmlStreamReader::Characters)
                        {
                            continue;
                        }

                        downloadImage(id, xml.text().toString(), what);

                        return true;
                    }
                }

                xml.readNext();
            }
            return false;
        }
    }

    return false;
}

QString LibraryLastFM::key() const
{
    return "2e23f0b83cc39d1a146c736f5807de18";
}

QString LibraryLastFM::host() const
{
    return "http://ws.audioscrobbler.com/2.0/";
}

void LibraryLastFM::downloadImage(const int &id, const QString &url, const QString &directory)
{
    QNetworkReply *reply = m_manager->get(QNetworkRequest(QUrl(url)));
    reply->setProperty(directory.toAscii(), id);
    m_replays << reply;
}

bool LibraryLastFM::isBusy() const
{
    return m_replays.count();
}
