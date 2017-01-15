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

#ifndef NICK_H
#define NICK_H

#include <QHash>
#include <QObject>
#include "core/defines.h"

class Nick : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(Nick)
    static Nick *Instance;
public:
    static Nick *instance();

    Nick();
    void clear();
    void add(const QString &strNick, const QString &strChannel, QString strModes);
    void remove(const QString &strNick, const QString &strChannel);
    void rename(const QString &strNick, const QString &strNewNick, const QString &strDisplay);
    void quit(const QString &strNick, const QString &strDisplay);
    void changeFlag(const QString &strNick, const QString &strChannel, QString strFlag);
    void changeFlag(const QString &strNick, const QString &strFlag);

    void setAvatar(const QString &strNick, const QString &strAvatar);
    QString getAvatar(const QString &strNick);

    void setSex(const QString &strNick, const QChar &cSex);
    QChar getSex(const QString &strNick);

    QString getModes(const QString &strNick, const QString &strChannel);
    int getMaxModes(const QString &strNick, const QString &strChannel);

    QList<CaseIgnoreString> getFromChannel(const QString &strChannel);

private:
    QHash<QString, OnetNick> lNicks;

    void removeFromChannel(const QString &strChannel);
    int createMaxModes(const QString &strModes);
};

#endif // NICK_H
