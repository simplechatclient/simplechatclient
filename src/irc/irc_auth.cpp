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

#include <QObject>
#include <QTextCodec>
#include <QTimer>
#include <QUrl>

#include "core/core.h"
#include "models/message.h"
#include "models/settings.h"
#include "irc/irc_auth.h"

IrcAuth::IrcAuth()
{
    Settings::instance()->setBool("authorizing", false);
    emit authStateChanged();
}

IrcAuth::~IrcAuth()
{
}

void IrcAuth::authorize(QString _strNick, QString _strPass)
{
    if (_strNick.isEmpty())
        return; // empty nick

    if (Settings::instance()->getBool("logged"))
        return; // already logged

    if (Settings::instance()->getBool("authorizing"))
        return; // already authorizing

    Settings::instance()->setBool("authorizing", true);
    emit authStateChanged();

    strFullNick = _strNick.left(30);
    strNick = (_strNick.at(0) == '~' ? _strNick.remove(0,1).left(29) : _strNick.left(30));
    strPass = _strPass;
    strNickLen = QString::number(strNick.length());
    bRegisteredNick = strFullNick.at(0) != '~';
    bOverride = Settings::instance()->getBool("override");

    Settings::instance()->set("uo_key", "*");
    Settings::instance()->set("uo_nick", strNick);

    if (Settings::instance()->getBool("debug"))
    {
        qDebug() << "Override: " << bOverride;
        qDebug() << "Logged: " << Settings::instance()->get("logged");
        qDebug() << "Authorizing: " << Settings::instance()->get("authorizing");
    }

    // update nick
    emit updateNick(strFullNick);

    // send auth
    if (Core::instance()->network->isConnected())
    {
        Core::instance()->network->send(QString("USER %1 * * :%1").arg(strNick));
        Core::instance()->network->send(QString("NICK %1").arg(strNick));
    }

    Settings::instance()->setBool("authorizing", false);
    emit authStateChanged();
}
