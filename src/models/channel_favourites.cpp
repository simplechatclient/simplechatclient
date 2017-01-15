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

#include "channel_favourites.h"

ChannelFavourites * ChannelFavourites::Instance = 0;

ChannelFavourites * ChannelFavourites::instance()
{
    if (!Instance)
    {
        Instance = new ChannelFavourites();
        Instance->clear();
    }

    return Instance;
}

ChannelFavourites::ChannelFavourites()
{
}

void ChannelFavourites::clear()
{
    favourites.clear();
}

QHash<QString, OnetChannelFavourites> ChannelFavourites::getAll()
{
    return favourites;
}

QList<CaseIgnoreString> ChannelFavourites::getAllCaseIgnoreSorted()
{
    QList<CaseIgnoreString> lChannelsCaseIgnore;

    // copy to new list
    QList<QString> lChannels = getAll().keys();
    foreach (const QString &strChannel, lChannels)
        lChannelsCaseIgnore.append(strChannel);

    // sort
    qStableSort(lChannelsCaseIgnore.begin(), lChannelsCaseIgnore.end());

    return lChannelsCaseIgnore;
}

void ChannelFavourites::add(const QString &channel)
{
    if (!favourites.contains(channel))
    {
        OnetChannelFavourites oChannelFavourites;
        oChannelFavourites.avatar = QString::null;

        favourites[channel] = oChannelFavourites;
    }
}

void ChannelFavourites::remove(const QString &channel)
{
    if (favourites.contains(channel))
        favourites.remove(channel);
}

bool ChannelFavourites::contains(const QString &channel)
{
    return favourites.contains(channel);
}

void ChannelFavourites::setAvatar(const QString &channel, const QString &avatar)
{
    if (favourites.contains(channel))
        favourites[channel].avatar = avatar;
}

QString ChannelFavourites::getAvatar(const QString &channel)
{
    if (favourites.contains(channel))
        return favourites[channel].avatar;
    else
        return QString::null;
}
