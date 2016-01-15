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

#include "ignore.h"

Ignore * Ignore::Instance = 0;

Ignore * Ignore::instance()
{
    if (!Instance)
    {
        Instance = new Ignore();
        Instance->clear();
    }

    return Instance;
}

Ignore::Ignore()
{
}

void Ignore::clear()
{
    ignore.clear();
}

QList<QString> Ignore::getAll()
{
    return ignore;
}

void Ignore::add(const QString &key)
{
    if (!ignore.contains(key))
        ignore.append(key);
}

void Ignore::remove(const QString &key)
{
    if (ignore.contains(key))
        ignore.removeOne(key);
}

bool Ignore::contains(const QString &key)
{
    return ignore.contains(key);
}
