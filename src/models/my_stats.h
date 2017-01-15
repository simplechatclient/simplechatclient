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

#ifndef MY_STATS_H
#define MY_STATS_H

#include <QHash>
#include <QObject>

class MyStats : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(MyStats)
    static MyStats *Instance;
public:
    static MyStats *instance();

    MyStats();
    void clear();
    QString get(const QString &key);
    QHash<QString,QString> getAll();
    void set(const QString &key, const QString &value);

private:
    QHash<QString,QString> myStats;
};

#endif // MY_STATS_H
