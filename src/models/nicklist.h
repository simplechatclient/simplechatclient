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

#ifndef NICK_LIST_H
#define NICK_LIST_H

#include <QObject>

class NickList : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(NickList)
    static NickList *Instance;
public:
    static NickList *instance();

    NickList();
    void clearUsers();
    void addUser(const QString &strNick, const QString &strChannel, const QString &strModes, int iMaxModes, const QString &strAvatar, QChar &cSex);
    void delUser(const QString &strNick, const QString &strChannel);
    void renameUser(const QString &strNick, const QString &strNewNick, const QList<QString> &lChannels, const QString &strDisplay);
    void quitUser(const QString &strNick, const QList<QString> &lChannels, const QString &strDisplay);
    void setUserModes(const QString &strNick, const QString &strChannel, const QString &strModes, int iMaxModes);
    void setUserSex(const QString &strNick, const QList<QString> &lChannels, const QChar &cSex);
    void setUserAvatar(const QString &strNick, const QList<QString> &lChannels, const QString &strAvatar);
};

#endif // NICK_LIST_H
