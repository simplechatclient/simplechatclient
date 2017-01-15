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

#include "find_nick.h"

FindNick * FindNick::Instance = 0;

FindNick * FindNick::instance()
{
    if (!Instance)
    {
        Instance = new FindNick();
        Instance->clear();
    }

    return Instance;
}

FindNick::FindNick()
{
}

void FindNick::add(const QString &nick)
{
    if (!lNickList.contains(nick))
        lNickList.append(nick);
}

QList<QString> FindNick::get()
{
    return lNickList;
}

void FindNick::clear()
{
    lNickList.clear();
    nickListStatus = StatusAwaiting;
}

void FindNick::setStatus(ObjectStatus status)
{
    nickListStatus = status;
}

ObjectStatus FindNick::getStatus()
{
    return nickListStatus;
}
