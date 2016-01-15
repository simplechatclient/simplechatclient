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

#ifndef CHANNEL_LIST_H
#define CHANNEL_LIST_H

#include <QObject>
#include "core/defines.h"

class ChannelList : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(ChannelList)
    static ChannelList *Instance;
public:
    static ChannelList *instance();

    ChannelList();
    void clear();
    void add(const OnetChannelList &channel);
    QList<OnetChannelList> getAll();
    void setTime(qint64 time);
    qint64 getTime();
    void setStatus(ObjectStatus status);
    ObjectStatus getStatus();

private:
    QList<OnetChannelList> lChannelList;
    qint64 iChannelListTime;
    ObjectStatus channelListStatus;
};

#endif // CHANNEL_LIST_H
