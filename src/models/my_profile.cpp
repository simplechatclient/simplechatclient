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

#include "my_profile.h"

MyProfile * MyProfile::Instance = 0;

MyProfile * MyProfile::instance()
{
    if (!Instance)
    {
        Instance = new MyProfile();
        Instance->clear();
    }

    return Instance;
}

MyProfile::MyProfile()
{
}

void MyProfile::clear()
{
    myProfile.clear();
}

QString MyProfile::get(const QString &key)
{
    return myProfile.value(key, QString::null);
}

QHash<QString,QString> MyProfile::getAll()
{
    return myProfile;
}

void MyProfile::set(const QString &key, const QString &value)
{
    myProfile[key] = value;
}
