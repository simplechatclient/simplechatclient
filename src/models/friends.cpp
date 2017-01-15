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

#include "friends.h"

Friends * Friends::Instance = 0;

Friends * Friends::instance()
{
    if (!Instance)
    {
        Instance = new Friends();
        Instance->clear();
    }

    return Instance;
}

Friends::Friends()
{
}

void Friends::clear()
{
    friends.clear();
}

bool Friends::get(const QString &key)
{
    return friends.value(key, false);
}

QHash<QString,bool> Friends::getAll()
{
    return friends;
}

void Friends::set(const QString &key, bool value)
{
    friends[key] = value;
}

void Friends::remove(const QString &key)
{
    friends.remove(key);
}

bool Friends::contains(const QString &key)
{
    return friends.contains(key);
}
