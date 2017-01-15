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
#include "core/core.h"
#include "common/convert.h"
#include "common/log.h"
#include "notification.h"
#include "my_profile.h"
#include "settings.h"
#include "offline.h"

Offline * Offline::Instance = 0;

Offline * Offline::instance()
{
    if (!Instance)
    {
        Instance = new Offline();
        Instance->init();
    }

    return Instance;
}

Offline::Offline()
{
    offlineMessagesAction = new QAction(QIcon(":/images/breeze/mail-mark-unread.svg") , tr("Offline messages"), this);
    offlineMessagesAction->setVisible(false);

    offmsgAllAction = new QAction(this);
    offmsgAllAction->setIcon(QIcon(":/images/breeze/mail-mark-notjunk.svg"));
    offmsgAllAction->setText(tr("Offline messages all"));
    offmsgAllAction->setCheckable(true);
    offmsgAllAction->setChecked(false);

    offmsgFriendAction = new QAction(this);
    offmsgFriendAction->setIcon(QIcon(":/images/breeze/mail-invitation.svg"));
    offmsgFriendAction->setText(tr("Offline messages friend"));
    offmsgFriendAction->setCheckable(true);
    offmsgFriendAction->setChecked(false);

    offmsgNoneAction = new QAction(this);
    offmsgNoneAction->setIcon(QIcon(":/images/breeze/mail-mark-junk.svg"));
    offmsgNoneAction->setText(tr("Offline messages none"));
    offmsgNoneAction->setCheckable(true);
    offmsgNoneAction->setChecked(false);
}

void Offline::init()
{
    lOfflineNicks.clear();
    lOfflineMessages.clear();

    connect(offmsgAllAction, SIGNAL(triggered()), this, SLOT(offmsgAllTriggered()));
    connect(offmsgFriendAction, SIGNAL(triggered()), this, SLOT(offmsgFriendTriggered()));
    connect(offmsgNoneAction, SIGNAL(triggered()), this, SLOT(offmsgNoneTriggered()));
}

void Offline::addMessage(qint64 iTime, const QString &strType, const QString &strNick, const QString &strMessage)
{
    QString strOfflineMessage = strMessage;

    Convert::simpleConvert(strOfflineMessage);

    // save offlinelog
    if (Settings::instance()->getBool("logs"))
    {
        QString strOfflineMessageLog = QString("%1 <%2> %3").arg(QDateTime::fromMSecsSinceEpoch(iTime).toString("[yyyy-MM-dd] [hh:mm:ss]"), strNick, strOfflineMessage);
        Log::save("offlinelog", strOfflineMessageLog, Log::Txt);

        //QString strSaveData = QString("%1 <%2> %3").arg(QDateTime::fromMSecsSinceEpoch(iTime).toString("[hh:mm:ss]"), strNick, strOfflineMessage);
        //Log::save(strNick, strSaveData, Log::Txt);
    }

    // add
    OnetOfflineMessage oOfflineMessage;
    oOfflineMessage.datetime = iTime;
    oOfflineMessage.type = strType;
    oOfflineMessage.message = strOfflineMessage;

    lOfflineMessages.insert(strNick, oOfflineMessage);
}

void Offline::removeMessage(const QString &strNick)
{
    lOfflineMessages.remove(strNick);
}

void Offline::clearMessages()
{
    lOfflineMessages.clear();
}

QList<OnetOfflineMessage> Offline::getMessages(const QString &strNick)
{
    return lOfflineMessages.values(strNick);
}

QList<OnetOfflineMessage> Offline::getMessagesReverted(const QString &strNick)
{
    QList<OnetOfflineMessage> lMessages = lOfflineMessages.values(strNick);
    QList<OnetOfflineMessage> lMessagesReverted;

    foreach (const OnetOfflineMessage &message, lMessages)
        lMessagesReverted.prepend(message);

    return lMessagesReverted;
}

bool Offline::isEmptyMessages()
{
    return lOfflineMessages.isEmpty();
}

void Offline::addNick(const QString &nick)
{
    if (!lOfflineNicks.contains(nick))
    {
        lOfflineNicks.append(nick);
        Notification::instance()->refreshOffline();
    }
}

void Offline::removeNick(const QString &nick)
{
    if (lOfflineNicks.contains(nick))
    {
        lOfflineNicks.removeAll(nick);
        Notification::instance()->refreshOffline();
    }
}

void Offline::clearNicks()
{
    lOfflineNicks.clear();
    Notification::instance()->refreshOffline();
}

QList<QString> Offline::getNicks()
{
    return lOfflineNicks;
}

bool Offline::isEmptyNicks()
{
    return lOfflineNicks.isEmpty();
}

int Offline::countNicks()
{
    return lOfflineNicks.size();
}

void Offline::updateOffmsgStatus()
{
    offmsgAllAction->setChecked(false);
    offmsgFriendAction->setChecked(false);
    offmsgNoneAction->setChecked(false);

    QString strOffmsg = MyProfile::instance()->get("offmsg");
    if (strOffmsg == "all")
        offmsgAllAction->setChecked(true);
    else if (strOffmsg == "friend")
        offmsgFriendAction->setChecked(true);
    else if (strOffmsg == "none")
        offmsgNoneAction->setChecked(true);
}

void Offline::offmsgAllTriggered()
{
    Core::instance()->network->send("NS SET offmsg all");
}

void Offline::offmsgFriendTriggered()
{
    Core::instance()->network->send("NS SET offmsg friend");
}

void Offline::offmsgNoneTriggered()
{
    Core::instance()->network->send("NS SET offmsg none");
}
