/*
 * Simple Chat Client
 *
 *   Copyright (C) 2009-2015 Piotr Łuczko <piotr.luczko@gmail.com>
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
#include "core/core.h"
#include "settings.h"
#include "busy.h"

Busy * Busy::Instance = 0;

Busy * Busy::instance()
{
    if (!Instance)
    {
        Instance = new Busy();
        Instance->init();
    }

    return Instance;
}

Busy::Busy()
{
    busyAction = new QAction(this);
    busyAction->setIcon(QIcon(":/images/breeze/im-user-offline.svg"));
    busyAction->setText(tr("Mark as busy"));
    busyAction->setCheckable(true);
    busyAction->setChecked(false);
}

void Busy::init()
{
    connect(busyAction, SIGNAL(triggered()), this, SLOT(busyTriggered()));
}

void Busy::start()
{
    if (!Settings::instance()->getBool("busy"))
    {
        Settings::instance()->setBool("busy", true);
        busyAction->setChecked(true);
    }
}

void Busy::stop()
{
    if (Settings::instance()->getBool("busy"))
    {
        Settings::instance()->setBool("busy", false);
        busyAction->setChecked(false);
    }
}

void Busy::busyTriggered()
{
    if (Core::instance()->network->isConnected() && Settings::instance()->getBool("logged"))
    {
        bool bBusy = Settings::instance()->getBool("busy");

        if (bBusy)
            Core::instance()->network->send("BUSY 0");
        else
            Core::instance()->network->send("BUSY 1");
    }
}
