/*
 * Simple Chat Client
 *
 *   Copyright (C) 2012 Piotr Łuczko <piotr.luczko@gmail.com>
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

#include "user_profile_model.h"

UserProfileModel * UserProfileModel::Instance = 0;

UserProfileModel * UserProfileModel::instance()
{
    if (!Instance)
    {
        Instance = new UserProfileModel();
        Instance->clear();
    }

    return Instance;
}

UserProfileModel::UserProfileModel()
{
}

void UserProfileModel::clear()
{
    userProfileNick = QString::null;
    userProfile.clear();
    userProfileStatus = StatusAwaiting;
}

QString UserProfileModel::getNick()
{
    return userProfileNick;
}

void UserProfileModel::setNick(const QString &newNick)
{
    clear();

    userProfileNick = newNick;
}

QString UserProfileModel::get(const QString &key)
{
    return userProfile.value(key, QString::null);
}

QHash<QString,QString> UserProfileModel::getAll()
{
    return userProfile;
}

void UserProfileModel::set(const QString &key, const QString &value)
{
    userProfile[key] = value;
}

void UserProfileModel::setStatus(ObjectStatus status)
{
    userProfileStatus = status;
}

ObjectStatus UserProfileModel::getStatus()
{
    return userProfileStatus;
}
