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

#ifndef CHANNEL_HOMES_H
#define CHANNEL_HOMES_H

#include <QObject>
#include "core/defines.h"

class ChannelHomes : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(ChannelHomes)
    static ChannelHomes *Instance;
public:
    static ChannelHomes *instance();

    ChannelHomes();
    void clear();
    void add(const QString &channel, const QString &flag);
    QHash<QString, OnetChannelHomes> get();
    bool contains(const QString &channel);
    void setStatus(ObjectStatus status);
    ObjectStatus getStatus();
    void setAvatar(const QString &channel, const QString &avatar);
    QString getAvatar(const QString &channel);

private:
    QHash<QString, OnetChannelHomes> lChannelHomes;
    ObjectStatus channelHomesStatus;
};

#endif // CHANNEL_HOMES_H
