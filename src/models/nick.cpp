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

#include "nicklist.h"
#include "settings.h"
#include "nick.h"

Nick * Nick::Instance = 0;

Nick * Nick::instance()
{
    if (!Instance)
    {
        Instance = new Nick();
        Instance->clear();
    }

    return Instance;
}

Nick::Nick()
{
}

void Nick::clear()
{
    lNicks.clear();
    NickList::instance()->clearUsers();
}

void Nick::add(const QString &strNick, const QString &strChannel, QString strModes)
{
    // hide OP flag if owner
    if ((strModes.contains(FLAG_OWNER)) && (strModes.contains(FLAG_OP))) strModes.remove(FLAG_OP);

    int iMaxModes = createMaxModes(strModes);
    QChar cSex = USER_SEX_UNKNOWN;

    if (lNicks.contains(strNick))
    {
        if (lNicks[strNick].channels.contains(strChannel)) return;

        lNicks[strNick].channels.append(strChannel);
        lNicks[strNick].channel_modes[strChannel] = strModes;
        lNicks[strNick].channel_max_modes[strChannel] = iMaxModes;
        cSex = lNicks[strNick].sex;

        NickList::instance()->addUser(strNick, strChannel, strModes, iMaxModes, lNicks[strNick].avatar, cSex);
    }
    else
    {
        OnetNick oNick;
        oNick.avatar = QString::null;
        oNick.channels.append(strChannel);
        oNick.channel_modes[strChannel] = strModes;
        oNick.channel_max_modes[strChannel] = iMaxModes;
        oNick.sex = cSex;

        lNicks[strNick] = oNick;

        NickList::instance()->addUser(strNick, strChannel, strModes, iMaxModes, QString::null, cSex);
    }
}

void Nick::remove(const QString &strNick, const QString &strChannel)
{
    if (!lNicks.contains(strNick)) return;

    NickList::instance()->delUser(strNick, strChannel);

    lNicks[strNick].channels.removeOne(strChannel);
    lNicks[strNick].channel_modes.remove(strChannel);
    lNicks[strNick].channel_max_modes.remove(strChannel);

    if (lNicks[strNick].channels.count() == 0)
        lNicks.remove(strNick);

    QString strMe = Settings::instance()->get("nick");
    if (strNick == strMe)
        removeFromChannel(strChannel);
}

void Nick::removeFromChannel(const QString &strChannel)
{
    QList<CaseIgnoreString> lNicksFromChannel = getFromChannel(strChannel);

    foreach (const QString &strNick, lNicksFromChannel)
    {
        lNicks[strNick].channels.removeOne(strChannel);
        lNicks[strNick].channel_modes.remove(strChannel);
        lNicks[strNick].channel_max_modes.remove(strChannel);

        if (lNicks[strNick].channels.size() == 0)
            lNicks.remove(strNick);
    }
}

void Nick::rename(const QString &strNick, const QString &strNewNick, const QString &strDisplay)
{
    if (!lNicks.contains(strNick)) return;

    NickList::instance()->renameUser(strNick, strNewNick, lNicks[strNick].channels, strDisplay);

    lNicks[strNewNick] = lNicks[strNick];
    lNicks.remove(strNick);
}

void Nick::quit(const QString &strNick, const QString &strDisplay)
{
    if (!lNicks.contains(strNick)) return;

    QList<QString> lChannels = lNicks[strNick].channels;
    NickList::instance()->quitUser(strNick, lChannels, strDisplay);
    lNicks.remove(strNick);

    QString strMe = Settings::instance()->get("nick");
    if (strNick == strMe)
    {
        foreach (const QString &strChannel, lChannels)
            removeFromChannel(strChannel);
    }
}

void Nick::changeFlag(const QString &strNick, const QString &strChannel, QString strFlag)
{
    if ((strNick.isEmpty()) || (strChannel.isEmpty()) || (strFlag.isEmpty()))
        return;
    if (!lNicks.contains(strNick)) return;

    QString strModes = lNicks[strNick].channel_modes.value(strChannel, QString::null);
    QString plusminus = strFlag.at(0);
    strFlag.remove(0, 1);

    QString strConvertFrom = "qaohXYv";
    QString strConvertTo = "`&@%!=+";

    for (int i = 0; i < strConvertFrom.size(); ++i)
        strFlag.replace(strConvertFrom.at(i), strConvertTo.at(i));

    if (plusminus == "+")
    {
        // fix webcam flags
        if ((strFlag == FLAG_CAM_PUB) && (strModes.contains(FLAG_CAM_PRIV))) strModes.remove(FLAG_CAM_PRIV);
        if ((strFlag == FLAG_CAM_PRIV) && (strModes.contains(FLAG_CAM_PUB))) strModes.remove(FLAG_CAM_PUB);

        if (!strModes.contains(strFlag))
            strModes.append(strFlag);
    }
    else
    {
        if (strModes.contains(strFlag))
            strModes.remove(strFlag);
    }

    // if owner remove op
    if ((strModes.contains(FLAG_OWNER)) && (strModes.contains(FLAG_OP))) strModes.remove(FLAG_OP);

    int iMaxModes = createMaxModes(strModes);
    lNicks[strNick].channel_modes[strChannel] = strModes;
    lNicks[strNick].channel_max_modes[strChannel] = iMaxModes;

    NickList::instance()->setUserModes(strNick, strChannel, strModes, iMaxModes);
}

void Nick::changeFlag(const QString &strNick, const QString &strFlag)
{
    if (!lNicks.contains(strNick)) return;

    QList<QString> lChannels = lNicks[strNick].channels;
    foreach (const QString &strChannel, lChannels)
        changeFlag(strNick, strChannel, strFlag);
}

void Nick::setAvatar(const QString &strNick, const QString &strAvatar)
{
    if (!lNicks.contains(strNick)) return;

    NickList::instance()->setUserAvatar(strNick, lNicks[strNick].channels, strAvatar);

    lNicks[strNick].avatar = strAvatar;
}

QString Nick::getAvatar(const QString &strNick)
{
    if (!lNicks.contains(strNick)) return QString::null;

    return lNicks[strNick].avatar;
}

void Nick::setSex(const QString &strNick, const QChar &cSex)
{
    if (!lNicks.contains(strNick)) return;

    NickList::instance()->setUserSex(strNick, lNicks[strNick].channels, cSex);

    lNicks[strNick].sex = cSex;
}

QChar Nick::getSex(const QString &strNick)
{
    if (!lNicks.contains(strNick)) return USER_SEX_UNKNOWN;

    return lNicks[strNick].sex;
}

QString Nick::getModes(const QString &strNick, const QString &strChannel)
{
    if (!lNicks.contains(strNick)) return QString::null;

    return lNicks[strNick].channel_modes.value(strChannel, QString::null);
}

int Nick::getMaxModes(const QString &strNick, const QString &strChannel)
{
    if (!lNicks.contains(strNick)) return -1;

    return lNicks[strNick].channel_max_modes.value(strChannel, -1);
}

QList<CaseIgnoreString> Nick::getFromChannel(const QString &strChannel)
{
    QList<CaseIgnoreString> lNicksFromChannel;

    QHashIterator<QString, OnetNick> it(lNicks);
    while (it.hasNext())
    {
        it.next();

        QString strNick = it.key();

        if (lNicks[strNick].channels.contains(strChannel))
            lNicksFromChannel.append(strNick);
    }

    // sort
    qStableSort(lNicksFromChannel.begin(), lNicksFromChannel.end());

    return lNicksFromChannel;
}

int Nick::createMaxModes(const QString &strModes)
{
    if (strModes.contains(FLAG_DEV)) { return FLAG_DEV_INT; }
    else if (strModes.contains(FLAG_OWNER)) { return FLAG_OWNER_INT; }
    else if (strModes.contains(FLAG_ADMIN)) { return FLAG_ADMIN_INT; }
    else if (strModes.contains(FLAG_OP)) { return FLAG_OP_INT; }
    else if (strModes.contains(FLAG_HALFOP)) { return FLAG_HALFOP_INT; }
    else if (strModes.contains(FLAG_MOD)) { return FLAG_MOD_INT; }
    else if (strModes.contains(FLAG_SCREENER)) { return FLAG_SCREENER_INT; }
    else if (strModes.contains(FLAG_VOICE)) { return FLAG_VOICE_INT; }
    else return FLAG_UNKNOWN_INT;
}
