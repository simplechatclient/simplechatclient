/*
 * Simple Chat Client
 *
 *   Copyright (C) 2009-2017 Piotr Łuczko <piotr.luczko@gmail.com>
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

#include "channel_list.h"

ChannelList * ChannelList::Instance = 0;

ChannelList * ChannelList::instance()
{
    if (!Instance)
    {
        Instance = new ChannelList();
        Instance->clear();
    }

    return Instance;
}

ChannelList::ChannelList()
{
}

void ChannelList::clear()
{
    lChannelList.clear();
    channelListStatus = StatusAwaiting;
    iChannelListTime = 0;
}

void ChannelList::add(const OnetChannelList &channel)
{
    lChannelList.append(channel);
}

QList<OnetChannelList> ChannelList::getAll()
{
    return lChannelList;
}

void ChannelList::setTime(qint64 time)
{
    iChannelListTime = time;
}

qint64 ChannelList::getTime()
{
    return iChannelListTime;
}

void ChannelList::setStatus(ObjectStatus status)
{
    channelListStatus = status;
}

ObjectStatus ChannelList::getStatus()
{
    return channelListStatus;
}
