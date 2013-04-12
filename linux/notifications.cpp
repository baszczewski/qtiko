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
#include <QtDBus>
#include <QImage>

#include <stdint.h>

#include "linux/notifications.h"

class NotificationImage
{
public:
    NotificationImage() {}
    NotificationImage(const QImage &img);
    QVariant toVariant();

protected:
    friend QDBusArgument &operator<<(QDBusArgument &a, const NotificationImage &b);
    friend const QDBusArgument &operator>>(const QDBusArgument &a, NotificationImage &b);

private:
    QByteArray m_image;
    int m_channels;
    int m_bitsPerSample;
    int m_width, m_height, m_stride;
    bool m_hasAlpha;
};
Q_DECLARE_METATYPE(NotificationImage)

NotificationImage::NotificationImage(const QImage &img)
{
    QImage tmp = img.scaled(QSize(128,128), Qt::IgnoreAspectRatio, Qt::SmoothTransformation).convertToFormat(QImage::Format_ARGB32);

    m_width = tmp.width();
    m_height = tmp.height();
    m_stride = m_width * 4;
    m_hasAlpha = true;
    m_channels = 4;
    m_bitsPerSample = 8;

    const uint32_t *data = reinterpret_cast<const uint32_t*>(tmp.constBits());

    unsigned int pixels = m_width * m_height;
    m_image.resize(pixels * 4);

    for(unsigned int i = 0; i < pixels; ++i)
    {
        m_image[i*4+0] = data[i] >> 16; // R
        m_image[i*4+1] = data[i] >> 8;  // G
        m_image[i*4+2] = data[i];       // B
        m_image[i*4+3] = data[i] >> 24; // A
    }
}

QDBusArgument &operator<<(QDBusArgument &a, const NotificationImage &b)
{
    a.beginStructure();
    a << b.m_width << b.m_height << b.m_stride << b.m_hasAlpha << b.m_bitsPerSample << b.m_channels << b.m_image;
    a.endStructure();
    return a;
}

const QDBusArgument &operator>>(const QDBusArgument &a, NotificationImage &b)
{
    a.beginStructure();
    a >> b.m_width >> b.m_height >> b.m_stride >> b.m_hasAlpha >> b.m_bitsPerSample >> b.m_channels >> b.m_image;
    a.endStructure();
    return a;
}

QVariant NotificationImage::toVariant()
{
    return QVariant(qDBusRegisterMetaType<NotificationImage>(), this);
}

Notifications::Notifications(QObject *parent)
    : QObject(parent)
{
    m_interface = new QDBusInterface("org.freedesktop.Notifications", "/org/freedesktop/Notifications", "org.freedesktop.Notifications");
}

void Notifications::show(const QString &title, const QString &subtitle, const QImage &image)
{
    if (!m_interface)
        return;

    if (!m_interface->isValid())
        return;

    quint32 m_id = 1234;

    QList<QVariant> args;

    // name
    args.append("qtiko");

    // id
    args.append(m_id);

    // icon
    args.append(QString());

    // title
    args.append(title);

    // body
    args.append(subtitle);

    QStringList actions;
    args.append(actions);

    QVariantMap hints;
    hints["icon_data"] = NotificationImage(image).toVariant();
    args.append(hints);

    // timeout
    args.append(1000);

    m_interface->asyncCall("CloseNotification", m_id);
    m_interface->callWithArgumentList(QDBus::NoBlock, "Notify", args);
}
