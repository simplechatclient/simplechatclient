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

#ifndef CHANNEL_FAVOURITES_H
#define CHANNEL_FAVOURITES_H

#include <QObject>
#include "core/defines.h"

class ChannelFavourites : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(ChannelFavourites)
    static ChannelFavourites *Instance;
public:
    static ChannelFavourites *instance();

    ChannelFavourites();
    void clear();
    QHash<QString, OnetChannelFavourites> getAll();
    QList<CaseIgnoreString> getAllCaseIgnoreSorted();
    void add(const QString &channel);
    void remove(const QString &channel);
    bool contains(const QString &channel);
    void setAvatar(const QString &channel, const QString &avatar);
    QString getAvatar(const QString &channel);

private:
    QHash<QString, OnetChannelFavourites> favourites;
};

#endif // CHANNEL_FAVOURITES_H
