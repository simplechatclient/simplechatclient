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

#include "channel_homes.h"

ChannelHomes * ChannelHomes::Instance = 0;

ChannelHomes * ChannelHomes::instance()
{
    if (!Instance)
    {
        Instance = new ChannelHomes();
        Instance->clear();
    }

    return Instance;
}

ChannelHomes::ChannelHomes()
{
}

void ChannelHomes::clear()
{
    lChannelHomes.clear();

    channelHomesStatus = StatusAwaiting;
}

void ChannelHomes::add(const QString &channel, const QString &flag)
{
    if (!lChannelHomes.contains(channel))
    {
        OnetChannelHomes oChannelHomes;
        oChannelHomes.avatar = QString::null;
        oChannelHomes.flag = flag;

        lChannelHomes[channel] = oChannelHomes;
    }
}

QHash<QString, OnetChannelHomes> ChannelHomes::get()
{
    return lChannelHomes;
}

bool ChannelHomes::contains(const QString &channel)
{
    return lChannelHomes.contains(channel);
}

void ChannelHomes::setStatus(ObjectStatus status)
{
    channelHomesStatus = status;
}

ObjectStatus ChannelHomes::getStatus()
{
    return channelHomesStatus;
}

void ChannelHomes::setAvatar(const QString &channel, const QString &avatar)
{
    if (lChannelHomes.contains(channel))
        lChannelHomes[channel].avatar = avatar;
}

QString ChannelHomes::getAvatar(const QString &channel)
{
    if (lChannelHomes.contains(channel))
        return lChannelHomes[channel].avatar;
    else
        return QString::null;
}
