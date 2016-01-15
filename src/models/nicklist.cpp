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

#include <QLabel>
#include "channel.h"
#include "core/core.h"
#include "core/mainwindow.h"
#include "message.h"
#include "widgets/nicklist_widget.h"
#include "settings.h"
#include "nicklist.h"

NickList * NickList::Instance = 0;

NickList * NickList::instance()
{
    if (!Instance)
    {
        Instance = new NickList();
    }

    return Instance;
}

NickList::NickList()
{
}

void NickList::clearUsers()
{
    QList<QString> lChannels = Channel::instance()->getList();
    foreach (const QString &strChannel, lChannels)
    {
        if (strChannel.at(0) == '^')
            Channel::instance()->setOffline(strChannel, true);

        Channel::instance()->getNickListWidget(strChannel)->clear();
        int iUsersCount = Channel::instance()->getNickListWidget(strChannel)->count();
        Channel::instance()->getUsers(strChannel)->setText(QString(tr("Users (%1)").arg(iUsersCount)));
    }
}

void NickList::addUser(const QString &strNick, const QString &strChannel, const QString &strModes, int iMaxModes, const QString &strAvatar, QChar &cSex)
{
    if ((strNick.isEmpty()) || (strChannel.isEmpty())) return;
    if (!Channel::instance()->contains(strChannel)) return;

    if (strChannel.at(0) == '^')
        Channel::instance()->setOffline(strChannel, false);

    Channel::instance()->getNickListWidget(strChannel)->addUser(strNick, strModes, iMaxModes, strAvatar, cSex);
    int iUsersCount = Channel::instance()->getNickListWidget(strChannel)->count();
    Channel::instance()->getUsers(strChannel)->setText(QString(tr("Users (%1)").arg(iUsersCount)));
}

void NickList::delUser(const QString &strNick, const QString &strChannel)
{
    if (!Channel::instance()->contains(strChannel)) return;

    if (strChannel.at(0) == '^')
        Channel::instance()->setOffline(strChannel, true);

    Channel::instance()->getNickListWidget(strChannel)->delUser(strNick);
    int iUsersCount = Channel::instance()->getNickListWidget(strChannel)->count();
    Channel::instance()->getUsers(strChannel)->setText(QString(tr("Users (%1)").arg(iUsersCount)));
}

void NickList::renameUser(const QString &strNick, const QString &strNewNick, const QList<QString> &lChannels, const QString &strDisplay)
{
    MessageCategory eMessageCategory = MessageMode;

    foreach (const QString &strChannel, lChannels)
    {
        Message::instance()->showMessage(strChannel, strDisplay, eMessageCategory);
        Channel::instance()->getNickListWidget(strChannel)->renameUser(strNick, strNewNick);
    }
}

void NickList::quitUser(const QString &strNick, const QList<QString> &lChannels, const QString &strDisplay)
{
    MessageCategory eMessageCategory = MessageQuit;

    foreach (const QString &strChannel, lChannels)
    {
        if (strChannel.at(0) == '^')
            Channel::instance()->setOffline(strChannel, true);

        Message::instance()->showMessage(strChannel, strDisplay, eMessageCategory);
        Channel::instance()->getNickListWidget(strChannel)->delUser(strNick);
        int iUsersCount = Channel::instance()->getNickListWidget(strChannel)->count();
        Channel::instance()->getUsers(strChannel)->setText(QString(tr("Users (%1)").arg(iUsersCount)));
    }
}

void NickList::setUserModes(const QString &strNick, const QString &strChannel, const QString &strModes, int iMaxModes)
{
    if (!Channel::instance()->contains(strChannel)) return;

    Channel::instance()->getNickListWidget(strChannel)->setUserModes(strNick, strModes, iMaxModes);

    // channel tool buttons
    QString strMe = Settings::instance()->get("nick");
    QString strCurrentChannel = Channel::instance()->getCurrentName();

    if ((strCurrentChannel == strChannel) && (strNick == strMe))
        Core::instance()->mainWindow()->refreshToolButtons(strChannel);
}

void NickList::setUserSex(const QString &strNick, const QList<QString> &lChannels, const QChar &cSex)
{
    foreach (const QString &strChannel, lChannels)
    {
        Channel::instance()->getNickListWidget(strChannel)->setUserSex(strNick, cSex);
    }
}

void NickList::setUserAvatar(const QString &strNick, const QList<QString> &lChannels, const QString &strAvatar)
{
    foreach (const QString &strChannel, lChannels)
    {
        Channel::instance()->getNickListWidget(strChannel)->setUserAvatar(strNick, strAvatar);
    }
}
