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

#ifndef OFFLINE_H
#define OFFLINE_H

#include "core/defines.h"
#include <QObject>

QT_BEGIN_NAMESPACE
class QAction;
QT_END_NAMESPACE

class Offline : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(Offline)
    static Offline *Instance;
public:
    static Offline *instance();

    Offline();
    void addMessage(qint64 iTime, const QString &strType, const QString &strNick, const QString &strMessage);
    void removeMessage(const QString &strNick);
    void clearMessages();
    QList<OnetOfflineMessage> getMessages(const QString &strNick);
    QList<OnetOfflineMessage> getMessagesReverted(const QString &strNick);
    bool isEmptyMessages();

    void addNick(const QString &nick);
    void removeNick(const QString &nick);
    void clearNicks();
    QList<QString> getNicks();
    bool isEmptyNicks();
    int countNicks();
    void updateOffmsgStatus();

    QAction *offlineMessagesAction;
    QAction *offmsgAllAction;
    QAction *offmsgFriendAction;
    QAction *offmsgNoneAction;

private:
    QMultiHash<QString, OnetOfflineMessage> lOfflineMessages;
    QList<QString> lOfflineNicks;

    void init();

private slots:
    void offmsgAllTriggered();
    void offmsgFriendTriggered();
    void offmsgNoneTriggered();
};

#endif // OFFLINE_H
