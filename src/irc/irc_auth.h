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

#ifndef IRC_AUTH_H
#define IRC_AUTH_H

#include <QObject>

/**
 * Authorize class for irc
 */
class IrcAuth : public QObject
{
    Q_OBJECT
public:
    IrcAuth();
    virtual ~IrcAuth();

public slots:
    void authorize(QString _strNick, QString _strPass);

private:
    QString strNick;
    QString strFullNick;
    QString strPass;
    QString strNickLen;
    QString strVersionLen;
    bool bRegisteredNick;
    bool bOverride;

signals:
    void updateNick(const QString &strNick);
    void authStateChanged();
};

#endif // IRC_AUTH_H
