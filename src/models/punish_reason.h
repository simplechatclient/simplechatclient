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

#ifndef PUNISH_REASON_H
#define PUNISH_REASON_H

#include <QObject>
#include <QStringList>

class PunishReason : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(PunishReason)
    static PunishReason *Instance;
public:
    static PunishReason *instance();

    PunishReason();
    void init();
    void add(const QString &strPunishReason);
    void remove(const QString &strPunishReason);
    void replace(const QString &strOldPunishReason, const QString &strNewPunishReason);
    bool contains(const QString &strPunishReason);
    void clear();
    QStringList get();

private:
    QStringList lPunishReason;

    void read();
    void save();
    QString fix(const QString &strPunishReason);
};

#endif // PUNISH_REASON_H
