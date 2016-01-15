/*
 * Simple Chat Client
 *
 *   Copyright (C) 2009-2016 Piotr Łuczko <piotr.luczko@gmail.com>
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

#include "channel_settings.h"

ChannelSettings * ChannelSettings::Instance = 0;

ChannelSettings * ChannelSettings::instance()
{
    if (!Instance)
    {
        Instance = new ChannelSettings();
        Instance->clear();
    }

    return Instance;
}

ChannelSettings::ChannelSettings()
{
}

void ChannelSettings::clear()
{
    strChannel.clear();

    lSettingsInfo.clear();
    lSettingsPermissions.clear();
    infoStatus = StatusAwaiting;

    lSettingsStats.clear();
    statsStatus = StatusAwaiting;
}

QString ChannelSettings::getChannel()
{
    return strChannel;
}

void ChannelSettings::setChannel(const QString &channel)
{
    clear();

    strChannel = channel;
}

QString ChannelSettings::getInfo(const QString &key)
{
    return lSettingsInfo.value(key, QString::null);
}

QMap<QString,QString> ChannelSettings::getAllInfo()
{
    return lSettingsInfo;
}

void ChannelSettings::setInfo(const QString &key, const QString &value)
{
    lSettingsInfo[key] = value;
}

QMultiHash<QString,QString> ChannelSettings::getAllPermission()
{
    return lSettingsPermissions;
}

void ChannelSettings::setPermission(const QString &key, const QString &value)
{
    lSettingsPermissions.insert(key, value);
}

bool ChannelSettings::containsPermission(const QString &key, const QString &value)
{
    return lSettingsPermissions.contains(key, value);
}

void ChannelSettings::setStatusInfo(ObjectStatus status)
{
    infoStatus = status;
}

ObjectStatus ChannelSettings::getStatusInfo()
{
    return infoStatus;
}

QString ChannelSettings::getStats(const QString &key)
{
    return lSettingsStats.value(key, QString::null);
}

QHash<QString,QString> ChannelSettings::getAllStats()
{
    return lSettingsStats;
}

void ChannelSettings::setStats(const QString &key, const QString &value)
{
    lSettingsStats[key] = value;
}

void ChannelSettings::setStatusStats(ObjectStatus status)
{
    statsStatus = status;
}

ObjectStatus ChannelSettings::getStatusStats()
{
    return statsStatus;
}
