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

#include <QMenu>
#include <QToolButton>
#include "awaylog.h"
#include "core/core.h"
#include "invite.h"
#include "core/mainwindow.h"
#include "offline.h"
#include "settings.h"
#include "update.h"
#include "notification.h"

Notification * Notification::Instance = 0;

Notification * Notification::instance()
{
    if (!Instance)
    {
        Instance = new Notification();
    }

    return Instance;
}

Notification::Notification()
{
    notificationMenu = new QMenu(tr("N&otification"));
    notificationMenu->addAction(Awaylog::instance()->awaylogAction);
    notificationMenu->addAction(Invite::instance()->inviteAction);
    notificationMenu->addAction(Offline::instance()->offlineMessagesAction);
    notificationMenu->addAction(Update::instance()->updateAction);
}

QMenu *Notification::getNotificationMenu()
{
    return notificationMenu;
}

void Notification::refreshAwaylog()
{
    refreshMenu();

    if (Awaylog::instance()->isEmpty())
        Awaylog::instance()->awaylogAction->setVisible(false);
    else
        Awaylog::instance()->awaylogAction->setVisible(true);
}

void Notification::refreshInvite()
{
    refreshMenu();

    if (Invite::instance()->isEmpty())
        Invite::instance()->inviteAction->setVisible(false);
    else
        Invite::instance()->inviteAction->setVisible(true);
}

void Notification::refreshOffline()
{
    refreshMenu();

    if (Offline::instance()->isEmptyNicks())
        Offline::instance()->offlineMessagesAction->setVisible(false);
    else
        Offline::instance()->offlineMessagesAction->setVisible(true);
}

void Notification::refreshUpdate()
{
    refreshMenu();

    if (Settings::instance()->get("version_status") == UPDATE_STATUS_OUTOFDATE)
        Update::instance()->updateAction->setVisible(true);
    else
        Update::instance()->updateAction->setVisible(false);
}

void Notification::refreshMenu()
{
    if ((!Awaylog::instance()->isEmpty()) || (!Invite::instance()->isEmpty()) || (!Offline::instance()->isEmptyNicks()) || (Settings::instance()->get("version_status") == UPDATE_STATUS_OUTOFDATE))
    {
        int iAwayCount = Awaylog::instance()->count();
        int iInviteCount = Invite::instance()->count();
        int iOfflineMessagesCount = Offline::instance()->countNicks();
        int iUpdateCount = Settings::instance()->get("version_status") == UPDATE_STATUS_OUTOFDATE ? 1 : 0;
        int iNotificationCount = iAwayCount + iInviteCount + iOfflineMessagesCount + iUpdateCount;

        Awaylog::instance()->awaylogAction->setText(tr("Awaylog (%1)").arg(iAwayCount));
        Invite::instance()->inviteAction->setText(tr("Invite (%1)").arg(iInviteCount));
        Offline::instance()->offlineMessagesAction->setText(tr("Offline messages (%1)").arg(iOfflineMessagesCount));
        Update::instance()->updateAction->setText(tr("Updates (%1)").arg(iUpdateCount));

        Core::instance()->mainWindow()->getNotificationToolButton()->setText(tr("N&otification (%1)").arg(iNotificationCount));
        Core::instance()->mainWindow()->getNotificationAction()->setVisible(true);
        Core::instance()->mainWindow()->startAlert();
    }
    else
    {
        Core::instance()->mainWindow()->getNotificationAction()->setVisible(false);
    }
}
