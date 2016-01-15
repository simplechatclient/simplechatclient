/*
 * Simple Chat Client
 *
 *   Copyright (C) 2009-2016 Dariusz Markowicz <dmarkowicz@alari.pl>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <QtDBus/QtDBus>
#include <QtCore/QCoreApplication>
#include <QtDBus/QDBusVariant>
#include <QtDBus/QDBusServiceWatcher>
#include <QtDBus/QDBusConnection>
#include <QObject>
#include <QDebug>

#include "mpris_player.h"

MprisPlayer::MprisPlayer()
{
}

QString MprisPlayer::trackInfo(const QString &strPlayer, const QString &strService, const QString &strFormat)
{
    if (strPlayer.isEmpty() || strService.isEmpty() || strFormat.isEmpty())
        return QString::null;

    QString strMessage = QString::null;
    QDBusReply<bool> reply = QDBusConnection::sessionBus().interface()->isServiceRegistered(strService);
    if (reply)
    {
        QDBusConnection bus = QDBusConnection::sessionBus();
        QDBusInterface dbus_iface(strService,"/org/mpris/MediaPlayer2","org.freedesktop.DBus.Properties", bus);
        QDBusReply<QDBusVariant> rstatus = dbus_iface.call("Get","org.mpris.MediaPlayer2.Player","PlaybackStatus");
        if (rstatus.isValid())
        {
            QString strStatus = qdbus_cast<QString>(rstatus.value().variant());
            if (strStatus == "Playing")
            {
                QDBusReply<QDBusVariant> results = dbus_iface.call("Get","org.mpris.MediaPlayer2.Player","Metadata");
                if (results.isValid())
                {
                    QVariantMap metaData = qdbus_cast<QVariantMap>(results.value().variant());

                    uint position = 0;
                    QDBusReply<QDBusVariant> rposition = dbus_iface.call("Get","org.mpris.MediaPlayer2.Player","Position");
                    if (rposition.isValid())
                    {
                        position = qdbus_cast<uint>(rposition.value().variant()) / 1000;
                    }
                    else if (rposition.error().type() == QDBusError::InvalidSignature)
                    {
                        /* GNOME MPlayer do it in a different way */
                        QDBusReply<qlonglong> rposition2 = dbus_iface.call("Get","org.mpris.MediaPlayer2.Player","Position");
                        if (rposition2.isValid())
                            position = qdbus_cast<uint>(rposition2.value()) / 1000;
                        else
                            qDebug() << "mpris Position invalid" << rposition2.error().name() << rposition2.error().message();
                    }
                    else
                        qDebug() << "mpris Position invalid" << rposition.error().name() << rposition.error().message();


                    strMessage = parse(strFormat, strPlayer, metaData, position);
                }
                else
                    qDebug() << "mpris Metadata invalid" << results.error().name() << results.error().message();
            }
            else if (strStatus == "Paused")
                strMessage = tr("%1 is paused").arg(strPlayer);
            else
                strMessage = tr("%1 is stopped").arg(strPlayer);
        }
        else
            qDebug() << "mpris PlaybackStatus invalid" << rstatus.error().name() << rstatus.error().message();

        bus.disconnectFromBus(strService);
    }
    else
        strMessage = tr("%1 is not running").arg(strPlayer);

    return strMessage;
}

QString MprisPlayer::parse(const QString &str, const QString &strPlayer, const QVariantMap &metaData, uint position)
{
    /*
        Code of this function is partialy
        borrown from Kadu. Thanks to Kadu Team! :)
    */

    uint sl = str.length();
    QString r;

    for ( uint i = 0; i < sl; i++ )
    {
        while ((i < sl) && (str[i] != '%'))
        {
            r += str[i];
            ++i;
        }

        if (i >= sl) i = sl - 1;

        if (str[i] == '%')
        {
            i++;
            switch(str[i].toLatin1())
            {
            case 't':
                r += metaData.value("xesam:title").toString();
                break;

            case 'a':
                r += metaData.value("xesam:album").toString();
                break;

            case 'r':
                r += metaData.value("xesam:artist").toString();
                break;

            case 'f':
                r += metaData.value("xesam:url").toString();
                break;

            case 'l':
                r += formatLength(metaData.value("mpris:length").toInt() / 1000);
                break;

            case 'c':
                r += formatLength(position);
                break;
            case 'p':
            {
                QString tmp;
                int len = metaData.value("mpris:length").toInt() / 1000;
                if (len != 0)
                {
                    int perc = 100 * position / len;
                    tmp = QString::number(perc) + '%';
                    r += tmp;
                }
                break;
            }

            case 'n':
                r += strPlayer;
                break;

            case 'v':
                r += strPlayer;
                break;

            default:
                r += str[i];
            }
        }
    }
    return r;
}

QString MprisPlayer::formatLength(int length)
{
    QString ms;
    if (length < 1000)
        length = 1000;

    int lgt = length / 1000, m, s;
    m = lgt / 60;
    s = lgt % 60;
    ms = QString::number(m) + ':';
    if (s < 10)
        ms += '0';

    ms += QString::number(s);

    return ms;
}
