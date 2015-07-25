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
#include <QDateTime>
#include "channel.h"
#include "common/convert.h"
#include "common/log.h"
#include "notification.h"
#include "settings.h"
#include "awaylog.h"

Awaylog * Awaylog::Instance = 0;

Awaylog * Awaylog::instance()
{
    if (!Instance)
    {
        Instance = new Awaylog();
        Instance->init();
    }

    return Instance;
}

Awaylog::Awaylog()
{
    awaylogAction = new QAction(QIcon(":/images/breeze/view-pim-tasks.svg"), tr("Awaylog"), this);
    awaylogAction->setVisible(false);
}

void Awaylog::init()
{
    lAwaylog.clear();
}

void Awaylog::add(qint64 iTime, const QString &strChannel, const QString &strData)
{
    if ((strChannel.isEmpty()) || (strData.isEmpty()))
        return;

    // channel
    QString strAwayLogChannel = strChannel;

    // fix priv
    if ((strAwayLogChannel.at(0) == '^') && (Channel::instance()->containsAlternativeName(strAwayLogChannel)))
        strAwayLogChannel = Channel::instance()->getAlternativeName(strAwayLogChannel);

    // data
    QString strAwayLogData = strData;

    // fix /me
    QString strRegExpMe = QString("%1ACTION %2%3").arg(QString(QByteArray("\x01")), "(.*)", QString(QByteArray("\x01")));
    if (strAwayLogData.contains(QRegExp(strRegExpMe)))
    {
        strAwayLogData.replace(QRegExp(strRegExpMe), "\\1");
        if (strAwayLogData.contains("<")) strAwayLogData = strAwayLogData.remove(strAwayLogData.indexOf("<"),1);
        if (strAwayLogData.contains(">")) strAwayLogData = strAwayLogData.remove(strAwayLogData.indexOf(">"),1);
        strAwayLogData = "*"+strAwayLogData;
    }

    Convert::simpleConvert(strAwayLogData);

    // save awaylog
    if (Settings::instance()->getBool("logs"))
    {
        QString strAwaylogFileData = QString("%1 %2 %3").arg(QDateTime::fromMSecsSinceEpoch(iTime).toString("[yyyy-MM-dd] [hh:mm:ss]"), strAwayLogChannel, strData);
        Log::save("awaylog", strAwaylogFileData, Log::Txt);
    }

    strAwayLogData = QString("%1\n%2 %3").arg(strAwayLogChannel, QDateTime::fromMSecsSinceEpoch(iTime).toString("[hh:mm:ss]"), strAwayLogData);

    if (!lAwaylog.contains(strAwayLogData))
    {
        lAwaylog.append(strAwayLogData);
        Notification::instance()->refreshAwaylog();
    }
}

QList<QString> Awaylog::get()
{
    return lAwaylog;
}

void Awaylog::clear()
{
    lAwaylog.clear();
    Notification::instance()->refreshAwaylog();
}

bool Awaylog::isEmpty()
{
    return lAwaylog.isEmpty();
}

int Awaylog::count()
{
    return lAwaylog.size();
}
