/*
 * Simple Chat Client
 *
 *   Copyright (C) 2009-2016 Dariusz Markowicz <dmarkowicz@alari.pl>
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

#ifndef MPRIS_PLAYER_H
#define MPRIS_PLAYER_H

#include <QObject>
#include <QString>
#include <QVariantMap>

class MprisPlayer : public QObject
{
    Q_OBJECT
public:
    MprisPlayer();

    QString trackInfo(const QString &strPlayer, const QString &strService, const QString &strFormat);
    QString parse(const QString &str, const QString &strPlayer, const QVariantMap &metaData, uint position);
    QString formatLength(int length);
};

#endif // MPRIS_PLAYER_H
