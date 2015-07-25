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
#include "away.h"

Away * Away::Instance = 0;

Away * Away::instance()
{
    if (!Instance)
    {
        Instance = new Away();
        Instance->init();
    }

    return Instance;
}

Away::Away()
{
    awayAction = new QAction(this);
    awayAction->setIcon(QIcon(":/images/breeze/im-user-away.svg"));
    awayAction->setText(tr("Mark as away"));
    awayAction->setCheckable(true);
    awayAction->setChecked(false);
}

void Away::init()
{
    connect(awayAction, SIGNAL(triggered()), this, SLOT(awayTriggered()));
}

void Away::start()
{
    if (!Settings::instance()->getBool("away"))
    {
        Settings::instance()->setBool("away", true);
        awayAction->setChecked(true);
    }
}

void Away::stop()
{
    if (Settings::instance()->getBool("away"))
    {
        Settings::instance()->setBool("away", false);
        awayAction->setChecked(false);
    }
}

void Away::awayTriggered()
{
    if (Core::instance()->network->isConnected() && Settings::instance()->getBool("logged"))
    {
        bool bAway = Settings::instance()->getBool("away");

        QString strReason;
        if (bAway)
            strReason = QString::null;
        else
            strReason = tr("Not here right now");

        Core::instance()->network->send(QString("AWAY :%1").arg(strReason));
    }
}
