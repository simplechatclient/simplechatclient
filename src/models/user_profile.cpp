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

#include "user_profile.h"

UserProfile * UserProfile::Instance = 0;

UserProfile * UserProfile::instance()
{
    if (!Instance)
    {
        Instance = new UserProfile();
        Instance->clear();
    }

    return Instance;
}

UserProfile::UserProfile()
{
}

void UserProfile::clear()
{
    userProfileNick = QString::null;
    userProfile.clear();
    userProfileStatus = StatusAwaiting;
}

QString UserProfile::getNick()
{
    return userProfileNick;
}

void UserProfile::setNick(const QString &newNick)
{
    clear();

    userProfileNick = newNick;
}

QString UserProfile::get(const QString &key)
{
    return userProfile.value(key, QString::null);
}

QHash<QString,QString> UserProfile::getAll()
{
    return userProfile;
}

void UserProfile::set(const QString &key, const QString &value)
{
    userProfile[key] = value;
}

void UserProfile::setStatus(ObjectStatus status)
{
    userProfileStatus = status;
}

ObjectStatus UserProfile::getStatus()
{
    return userProfileStatus;
}
