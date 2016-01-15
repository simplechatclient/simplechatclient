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

#ifndef AUTOAWAY_H
#define AUTOAWAY_H

#include <QObject>

QT_BEGIN_NAMESPACE
class QTimer;
QT_END_NAMESPACE

class Autoaway : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(Autoaway)
    static Autoaway *Instance;
public:
    static Autoaway *instance();

    Autoaway();
    void start();
    void stop();

private:
    QTimer *autoAwayTimer;

    void init();

private slots:
    void timeoutAutoaway();
};

#endif // AUTOAWAY_H
