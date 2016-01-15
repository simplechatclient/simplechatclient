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

#ifndef USER_PROFILE_H
#define USER_PROFILE_H

#include <QHash>
#include <QObject>
#include "core/defines.h"

class UserProfile : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(UserProfile)
    static UserProfile *Instance;
public:
    static UserProfile *instance();

    UserProfile();
    void clear();
    QString getNick();
    void setNick(const QString &newNick);
    QString get(const QString &key);
    QHash<QString,QString> getAll();
    void set(const QString &key, const QString &value);
    void setStatus(ObjectStatus status);
    ObjectStatus getStatus();

private:
    QString userProfileNick;
    QHash<QString,QString> userProfile;
    ObjectStatus userProfileStatus;
};

#endif // USER_PROFILE_H
