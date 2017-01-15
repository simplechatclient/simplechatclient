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

#ifndef LAG_H
#define LAG_H

#include <QObject>

QT_BEGIN_NAMESPACE
class QAction;
QT_END_NAMESPACE

class Lag : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(Lag)
    static Lag *Instance;
public:
    static Lag *instance();
    QAction *lagAction;

    Lag();
    void update(qint64 iLag);
    void reset();
    QString createLagText(qint64 iLag);
    void calculate(qint64 iServerMSecs);
};

#endif // LAG_H
