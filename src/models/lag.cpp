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

#include <QAction>
#include <QDateTime>
#include "lag.h"

Lag * Lag::Instance = 0;

Lag * Lag::instance()
{
    if (!Instance)
    {
        Instance = new Lag();
    }

    return Instance;
}

Lag::Lag()
{
    lagAction = new QAction(this);
    lagAction->setEnabled(false);
    lagAction->setText("Lag: -");
}

void Lag::update(qint64 iLag)
{
    QString lag = createLagText(iLag);
    lagAction->setText("Lag: "+lag);
}

void Lag::reset()
{
    lagAction->setText("Lag: -");
}

QString Lag::createLagText(qint64 iLag)
{
    QString lag = QString::number(iLag);

    if (lag.size() > 3)
        lag.insert(lag.size()-3,".");
    else if (lag.size() == 3)
        lag = "0."+lag;
    else if (lag.size() == 2)
        lag = "0.0"+lag;
    else if (lag.size() == 1)
        lag = "0.00"+lag;

    return lag+"s";
}

void Lag::calculate(qint64 iServerMSecs)
{
    qint64 iCurrentMSecs = QDateTime::currentMSecsSinceEpoch();
    qint64 iLag = iCurrentMSecs - iServerMSecs;

    if ((iLag >= 0) && (iLag <= 301000))
        update(iLag);
}
