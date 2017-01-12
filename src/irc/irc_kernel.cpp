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

#include <QDateTime>
#include <QMessageBox> // raw 433
#include <QUuid> // raw invite
#include "models/autoaway.h"
#include "models/avatar.h"
#include "models/away.h"
#include "models/busy.h"
#include "models/channel.h"
#include "models/channel_favourites.h"
#include "models/channel_homes.h"
#include "gui/channel_key_gui.h"
#include "models/channel_list.h"
#include "models/channel_settings.h"
#include "common/convert.h"
#include "core/core.h"
#include "models/find_nick.h"
#include "models/friends.h"
#include "models/ignore.h"
#include "gui/invite_gui.h"
#include "models/invite.h"
#include "models/lag.h"
#include "common/log.h"
#include "core/mainwindow.h"
#include "models/message.h"
#include "models/my_profile.h"
#include "models/my_stats.h"
#include "models/nick.h"
#include "models/offline.h"
#include "models/profile_manager.h"
#include "common/replace.h"
#include "models/settings.h"
#include "models/sound_notify.h"
#include "tab/tab_container.h"
#include "models/themes.h"
#include "models/tray.h"
#include "models/user_profile.h"
#include "common/config.h"
#include "common/simple_crypt.h"
#include "irc/irc_kernel.h"

IrcKernel::IrcKernel(TabContainer *_pTabC) : pTabC(_pTabC)
{
}

IrcKernel::~IrcKernel()
{
}

void IrcKernel::kernel(const QString &_strData)
{
    strData = _strData;
    strDataList = strData.split(" ");

    if (Settings::instance()->getBool("debug"))
        Message::instance()->showMessage(DEBUG_WINDOW, "<- "+strData, MessageDefault);

    bool bUnknownRaw1 = false;
    bool bUnknownRaw2 = false;
    bool bUnknownRaw3 = false;

    if (!strDataList.value(1).isEmpty())
    {
        QString strCmd0 = strDataList.at(0);
        QString strCmd1 = strDataList.at(1);

        if (strCmd0 == "PING") raw_ping();
        else if (strCmd0 == "ERROR") raw_error();
        else if (strCmd1 == "PONG") raw_pong();
        else if (strCmd1 == "JOIN") raw_join();
        else if (strCmd1 == "PART") raw_part();
        else if (strCmd1 == "QUIT") raw_quit();
        else if (strCmd1 == "KICK") raw_kick();
        else if (strCmd1 == "MODE") raw_mode();
        else if (strCmd1 == "PRIVMSG") raw_privmsg();
        else if (strCmd1 == "INVITE") raw_invite();
        else if (strCmd1 == "TOPIC") raw_topic();
        else if (strCmd1 == "INVREJECT") raw_invreject();
        else if (strCmd1 == "INVIGNORE") raw_invignore();
        else if (strCmd1 == "MODERMSG") raw_modermsg();
        else if (strCmd1 == "MODERNOTICE") raw_modernotice();
        else if (strCmd1 == "MODERATE") raw_moderate();
        else if (strCmd1 == "KILL") raw_kill();
        else if (strCmd1 == "NICK") raw_nick();
        else if (strCmd1 == "SNONOTICE") raw_snonotice();
        else
            bUnknownRaw1 = true;

        if (strCmd1.size() !=3 || !QMetaObject::invokeMethod(this, QString("raw_" + strCmd1).toStdString().c_str(), Qt::DirectConnection))
            bUnknownRaw2 = true;

        if ((strDataList.at(1) == "NOTICE") && (!strDataList.value(3).isEmpty()))
        {
            raw_notice();
        }
        else
            bUnknownRaw3 = true;
    }
    else
    {
        bUnknownRaw1 = true;
        bUnknownRaw2 = true;
        bUnknownRaw3 = true;
    }

    // detect unknown raw
    if (bUnknownRaw1 && bUnknownRaw2 && bUnknownRaw3)
    {
        Log::save("unknown-raw", strData, Log::Txt);
    }
}

// PING :cf1f1.onet
void IrcKernel::raw_ping()
{
    if (strDataList.size() < 2) return;

    QString strServer = strDataList.at(1);

    if (!strServer.isEmpty())
        Core::instance()->network->send(QString("PONG %1").arg(strServer));
}

// :cf1f4.onet PONG cf1f4.onet :1340185644095
void IrcKernel::raw_pong()
{
    if (strDataList.size() < 4) return;

    QString strServerMSecs = strDataList.at(3);
    if (strServerMSecs.at(0) == ':') strServerMSecs.remove(0,1);

    // check correct pong
    qint64 iServerMSecs = strServerMSecs.toLongLong();
    if (iServerMSecs != 0)
        Lag::instance()->calculate(iServerMSecs);
}

// ERROR :Closing link (unknown@95.48.183.154) [Registration timeout]
void IrcKernel::raw_error()
{
    QString strMessage;
    for (int i = 0; i < strDataList.size(); ++i) { if (i != 0) strMessage += " "; strMessage += strDataList.at(i); }
    if ((!strMessage.isEmpty()) && (strMessage.at(0) == ':')) strMessage.remove(0,1);

    Message::instance()->showMessageAll(strMessage, MessageInfo);
}

// :scc_test!51976824@3DE379.B7103A.6CF799.6902F4 JOIN #Quiz :rx,0
void IrcKernel::raw_join()
{
    if (strDataList.size() < 3) return;

    QString strNick = strDataList.at(0);
    if (strNick.at(0) == ':') strNick.remove(0,1);
    strNick = strNick.left(strNick.indexOf('!'));

    QString strZUO = strDataList.at(0);
    strZUO = strZUO.mid(strZUO.indexOf('!')+1, strZUO.indexOf('@')-strZUO.indexOf('!')-1);

    QString strIP = strDataList.at(0);
    if (strIP.at(0) == ':') strIP.remove(0,1);
    strIP = strIP.right(strIP.length()-strIP.indexOf('@')-1);

    QString strChannel = strDataList.at(2);
    if (strChannel.at(0) == ':') strChannel.remove(0,1);

    QString strSuffix;
    if (!strDataList.value(3).isEmpty())
    {
        strSuffix = strDataList.at(3);
        if (strSuffix.at(0) == ':') strSuffix.remove(0,1);
        strSuffix = strSuffix.left(strSuffix.length()-2);
    }

    QString strDisplay;

    if (strChannel.at(0) != '^')
    {
        if (Settings::instance()->getBool("zuo_and_ip"))
            strDisplay = QString(tr("* %1 [%2@%3] has joined %4")).arg(strNick, strZUO, strIP, strChannel);
        else
            strDisplay = QString(tr("* %1 has joined %2")).arg(strNick, strChannel);
    }
    else
    {
        if (Settings::instance()->getBool("zuo_and_ip"))
            strDisplay = QString(tr("* %1 [%2@%3] has joined priv")).arg(strNick, strZUO, strIP);
        else
            strDisplay = QString(tr("* %1 has joined priv")).arg(strNick);
    }

    // add tab
    pTabC->addTab(strChannel);

    // show message
    if (strChannel.at(0) == '^')
    {
        if (Channel::instance()->containsAlternativeName(strChannel))
            Message::instance()->showMessage(strChannel, strDisplay, MessageJoin);
    }
    else
        Message::instance()->showMessage(strChannel, strDisplay, MessageJoin);

    // nick
    Nick::instance()->add(strNick, strChannel, strSuffix);

    // nick avatar
    QString strMe = Settings::instance()->get("nick");
    if (strNick.at(0) != '~' && !strSuffix.contains(FLAG_BOT)
            && Themes::instance()->isCurrentWithAvatar()
            && Nick::instance()->getAvatar(strNick).isEmpty())
    {
        if (strNick == strMe)
        {
            QString strAvatar = MyProfile::instance()->get("avatar");
            if (!strAvatar.isEmpty())
                Avatar::instance()->get(strNick, "nick", strAvatar);
        }
        else
        {
            // Core::instance()->network->send(QString("NS INFO %1 s").arg(strNick));
        }
    }

    // my nick sex
    if (strNick == strMe && strNick.at(0) != '~' && Themes::instance()->isCurrentWithAvatar())
    {
        QString strSex = MyProfile::instance()->get("sex");
        if (strSex.size() == 1)
            Nick::instance()->setSex(strNick, strSex.at(0));
    }

    //if ((strNick == strMe) && (strChannel.at(0) != '^'))
        //Core::instance()->network->send(QString("CS INFO %1 i").arg(strChannel));
}

// :scc_test!51976824@3DE379.B7103A.6CF799.6902F4 PART #scc
void IrcKernel::raw_part()
{
    if (strDataList.size() < 3) return;

    QString strNick = strDataList.at(0);
    if (strNick.at(0) == ':') strNick.remove(0,1);
    strNick = strNick.left(strNick.indexOf('!'));

    QString strZUO = strDataList.at(0);
    strZUO = strZUO.mid(strZUO.indexOf('!')+1, strZUO.indexOf('@')-strZUO.indexOf('!')-1);

    QString strIP = strDataList.at(0);
    if (strIP.at(0) == ':') strIP.remove(0,1);
    strIP = strIP.right(strIP.length()-strIP.indexOf('@')-1);

    QString strChannel = strDataList.at(2);
    if (strChannel.at(0) == ':') strChannel.remove(0,1);

    QString strReason;
    for (int i = 3; i < strDataList.size(); ++i) { if (i != 3) strReason += " "; strReason += strDataList.at(i); }
    if ((!strReason.isEmpty()) && (strReason.at(0) == ':')) strReason.remove(0,1);

    QString strDisplay;

    if (strChannel.at(0) != '^')
    {
        if (!strReason.isEmpty())
        {
            if (Settings::instance()->getBool("zuo_and_ip"))
                strDisplay = QString(tr("* %1 [%2@%3] has left %4 [%5]")).arg(strNick, strZUO, strIP, strChannel, strReason);
            else
                strDisplay = QString(tr("* %1 has left %2 [%3]")).arg(strNick, strChannel, strReason);
        }
        else
        {
            if (Settings::instance()->getBool("zuo_and_ip"))
                strDisplay = QString(tr("* %1 [%2@%3] has left %4")).arg(strNick, strZUO, strIP, strChannel);
            else
                strDisplay = QString(tr("* %1 has left %2")).arg(strNick, strChannel);
        }
    }
    else
    {
        if (Settings::instance()->getBool("zuo_and_ip"))
            strDisplay = QString(tr("* %1 [%2@%3] has left priv")).arg(strNick, strZUO, strIP);
        else
            strDisplay = QString(tr("* %1 has left priv")).arg(strNick);
    }

    Message::instance()->showMessage(strChannel, strDisplay, MessagePart);
    Nick::instance()->remove(strNick, strChannel);

    // if self part
    QString strMe = Settings::instance()->get("nick");

    if (strNick == strMe)
    {
        // close channel
        if ((strChannel != DEBUG_WINDOW) && (strChannel != STATUS_WINDOW))
            pTabC->removeTab(strChannel);
    }
}

// :Stark!38566204@A5F2F1.68FE5E.DE32AF.62ECB9 QUIT :Client exited
void IrcKernel::raw_quit()
{
    if (strDataList.size() < 3) return;

    QString strNick = strDataList.at(0);
    if (strNick.at(0) == ':') strNick.remove(0,1);
    strNick = strNick.left(strNick.indexOf('!'));

    QString strZUO = strDataList.at(0);
    strZUO = strZUO.mid(strZUO.indexOf('!')+1, strZUO.indexOf('@')-strZUO.indexOf('!')-1);

    QString strIP = strDataList.at(0);
    if (strIP.at(0) == ':') strIP.remove(0,1);
    strIP = strIP.right(strIP.length()-strIP.indexOf('@')-1);

    QString strReason;
    for (int i = 2; i < strDataList.size(); ++i) { if (i != 2) strReason += " "; strReason += strDataList.at(i); }
    if ((!strReason.isEmpty()) && (strReason.at(0) == ':')) strReason.remove(0,1);

    QString strDisplay;
    if (Settings::instance()->getBool("zuo_and_ip"))
        strDisplay = QString(tr("* %1 [%2@%3] has quit [%4]")).arg(strNick, strZUO, strIP, strReason);
    else
        strDisplay = QString(tr("* %1 has quit [%2]")).arg(strNick, strReason);

    Nick::instance()->quit(strNick, strDisplay);
}

// :scc_test!51976824@3DE379.B7103A.6CF799.6902F4 KICK #scc Moment_w_atmosferze :sio
void IrcKernel::raw_kick()
{
    if (strDataList.size() < 3) return;

    QString strWho = strDataList.at(0);
    if (strWho.at(0) == ':') strWho.remove(0,1);
    strWho = strWho.left(strWho.indexOf('!'));

    QString strZUO = strDataList.at(0);
    strZUO = strZUO.mid(strZUO.indexOf('!')+1, strZUO.indexOf('@')-strZUO.indexOf('!')-1);

    QString strIP = strDataList.at(0);
    if (strIP.at(0) == ':') strIP.remove(0,1);
    strIP = strIP.right(strIP.length()-strIP.indexOf('@')-1);

    QString strChannel = strDataList.at(2);

    QString strNick = strDataList.at(3);
    if (strNick.at(0) == ':') strNick.remove(0,1);
    strNick = strNick.left(strNick.indexOf('!'));

    QString strReason;
    for (int i = 4; i < strDataList.size(); ++i) { if (i != 4) strReason += " "; strReason += strDataList.at(i); }
    if ((!strReason.isEmpty()) && (strReason.at(0) == ':')) strReason.remove(0,1);

    QString strMe = Settings::instance()->get("nick");

    Nick::instance()->remove(strNick, strChannel);

    if (strNick != strMe)
    {
        QString strDisplay = QString(tr("* %1 has been kicked from channel %2 by %3 Reason: %4")).arg(strNick, strChannel, strWho, strReason);
        Message::instance()->showMessage(strChannel, strDisplay, MessageKick);
    }
    else
    {
        QString strDisplay = QString(tr("* You have been kicked from channel %1 by %2 Reason: %3")).arg(strChannel, strWho, strReason);

        Message::instance()->showMessage(strChannel, strDisplay, MessageKick);
        Message::instance()->showMessage(STATUS_WINDOW, strDisplay, MessageKick);

        // remove tab
        pTabC->removeTab(strChannel);
    }
}

// :Merovingian!26269559@jest.piekny.i.uroczy.ma.przesliczne.oczy MODE Merovingian :+b
// :Merovingian!26269559@2294E8.94913F.2EAEC9.11F26D MODE Merovingian :+b
// :ankaszo!51613093@F4C727.446F67.966AC9.BAAE26 MODE ankaszo -W
// :ChanServ!service@service.onet MODE #glupia_nazwa +k bum
// :ChanServ!service@service.onet MODE #bzzzz -l
// :NickServ!service@service.onet MODE scc_test +r
// :ChanServ!service@service.onet MODE #scc +ips
// :ChanServ!service@service.onet MODE #scc +o scc_test
// :ChanServ!service@service.onet MODE #scc +eo *!51976824@* scc_test
// :ChanServ!service@service.onet MODE #abc123 +il-e 1 *!51976824@*
void IrcKernel::raw_mode()
{
    if (strDataList.size() < 4) return;

    QString strWho = strDataList.at(0);
    if (strWho.at(0) == ':') strWho.remove(0,1);
    strWho = strWho.left(strWho.indexOf('!'));

    QString strNickOrChannel = strDataList.at(2);

    if ((strNickOrChannel.at(0) == '#') || (strNickOrChannel.at(0) == '^'))
    {
        QString strChannel = strNickOrChannel;
        QString strFlags = strDataList.at(3);
        QMultiHash<QString, QString> flag_value;
        QString strMode3 = "GQLDMRVimnprstu";
        QString plusminus;

        int index = 4;
        for (int i = 0; i < strFlags.length(); ++i)
        {
            QString f = strFlags.at(i);
            if ((f == "+") || (f == "-"))
            {
                if (f == "+") plusminus = f;
                else if (f == "-") plusminus = f;
            }
            else
            {
                QString strFlag = plusminus+f;

                if (strMode3.contains(f))
                    flag_value.insert(strFlag, QString::null);
                else
                {
                    if (index < strDataList.size())
                    {
                        flag_value.insert(strFlag, strDataList.at(index));
                        index++;
                    }
                    else
                        flag_value.insert(strFlag, QString::null);
                }
            }
        }

        QHashIterator <QString, QString> it(flag_value);
        it.toBack();
        while (it.hasPrevious())
        {
            it.previous();
            QString strFlag = it.key();
            QString strValue = it.value();
            QString strDisplay;
            bool bNickFlag = false;

            if (strFlag == "+q") { strDisplay = QString(tr("* %1 is now the owner of the channel %2 (set by %3)")).arg(strValue, strChannel, strWho); bNickFlag = true; }
            else if (strFlag == "-q") { strDisplay = QString(tr("* %1 is no longer the owner of the channel %2 (set by %3)")).arg(strValue, strChannel, strWho); bNickFlag = true; }
            else if (strFlag == "+o") { strDisplay = QString(tr("* %1 is now super-operator on the channel %2 (set by %3)")).arg(strValue, strChannel, strWho); bNickFlag = true; }
            else if (strFlag == "-o") { strDisplay = QString(tr("* %1 is no longer super-operator on the channel %2 (set by %3)")).arg(strValue, strChannel, strWho); bNickFlag = true; }
            else if (strFlag == "+h") { strDisplay = QString(tr("* %1 is now the operator of the channel %2 (set by %3)")).arg(strValue, strChannel, strWho); bNickFlag = true; }
            else if (strFlag == "-h") { strDisplay = QString(tr("* %1 is no longer the operator of the channel %2 (set by %3)")).arg(strValue, strChannel, strWho); bNickFlag = true; }
            else if (strFlag == "+v") { strDisplay = QString(tr("* %1 is now a guest of channel %2 (set by %3)")).arg(strValue, strChannel, strWho); bNickFlag = true; }
            else if (strFlag == "-v") { strDisplay = QString(tr("* %1 is no longer a guest of channel %2 (set by %3)")).arg(strValue, strChannel, strWho); bNickFlag = true; }
            else if (strFlag == "+X") { strDisplay = QString(tr("* %1 is now moderator of the channel %2 (set by %3)")).arg(strValue, strChannel, strWho); bNickFlag = true; }
            else if (strFlag == "-X") { strDisplay = QString(tr("* %1 is no longer moderating channel %2 (set by %3)")).arg(strValue, strChannel, strWho); bNickFlag = true; }
            else if (strFlag == "+Y") { strDisplay = QString(tr("* %1 is now screener channel %2 (set by %3)")).arg(strValue, strChannel, strWho); bNickFlag = true; }
            else if (strFlag == "-Y") { strDisplay = QString(tr("* %1 is no longer a screener channel %2 (set by %3)")).arg(strValue, strChannel, strWho); bNickFlag = true; }
            else if (strFlag == "+b") { strDisplay = QString(tr("* %1 is now on the banned list in %2 (set by %3)")).arg(strValue, strChannel, strWho); bNickFlag = true; }
            else if (strFlag == "-b") { strDisplay = QString(tr("* %1 is no longer on the banned list in %2 (set by %3)")).arg(strValue, strChannel, strWho); bNickFlag = true; }
            else if (strFlag == "+I") { strDisplay = QString(tr("* %1 is now on the list of invitees in %2 (set by %3)")).arg(strValue, strChannel, strWho); bNickFlag = true; }
            else if (strFlag == "-I") { strDisplay = QString(tr("* %1 is no longer on the list of invitees in %2 (set by %3)")).arg(strValue, strChannel, strWho); bNickFlag = true; }
            else if (strFlag == "+e") { strDisplay = QString(tr("* %1 now has ban exception flag in %2 (set by %3)")).arg(strValue, strChannel, strWho); bNickFlag = true; }
            else if (strFlag == "-e") { strDisplay = QString(tr("* %1 no longer has a ban exception flag in %2 (set by %3)")).arg(strValue, strChannel, strWho); bNickFlag = true; }

            else if (strFlag == "+k") strDisplay = QString(tr("* Channel %1 now has key set to %2 (set by %3)")).arg(strChannel, strValue, strWho);
            else if (strFlag == "-k") strDisplay = QString(tr("* Channel %1 no longer has key set (set by %2)")).arg(strChannel, strWho);
            else if (strFlag == "+n") strDisplay = QString(tr("* Channel %1 will now have no external messages sent to the channel (set by %2)")).arg(strChannel, strWho);
            else if (strFlag == "-n") strDisplay = QString(tr("* Channel %1 will now allow external messages sent to the channel (set by %2)")).arg(strChannel, strWho);
            else if (strFlag == "+t") strDisplay = QString(tr("* Only channel operators can now change the topic in %1 channel (set by %2)")).arg(strChannel, strWho);
            else if (strFlag == "-t") strDisplay = QString(tr("* Anyone can now change the topic in %1 channel (set by %2)")).arg(strChannel, strWho);
            else if (strFlag == "+i") strDisplay = QString(tr("* Channel %1 is now a hidden channel (set by %2)")).arg(strChannel, strWho);
            else if (strFlag == "-i") strDisplay = QString(tr("* Channel %1 is no longer hidden channel (set by %2)")).arg(strChannel, strWho);
            else if (strFlag == "+p") strDisplay = QString(tr("* Channel %1 is now a private channel (set by %2)")).arg(strChannel, strWho);
            else if (strFlag == "-p") strDisplay = QString(tr("* Channel %1 is no longer a private channel (set by %2)")).arg(strChannel, strWho);
            else if (strFlag == "+s") strDisplay = QString(tr("* Channel %1 is now a secret channel (set by %2)")).arg(strChannel, strWho);
            else if (strFlag == "-s") strDisplay = QString(tr("* Channel %1 is no longer a secret channel (set by %2)")).arg(strChannel, strWho);
            else if (strFlag == "+m") strDisplay = QString(tr("* Channel %1 is now moderated channel (set by %2)")).arg(strChannel, strWho);
            else if (strFlag == "-m") strDisplay = QString(tr("* Channel %1 is no longer moderated channel (set by %2)")).arg(strChannel, strWho);
            else if (strFlag == "+u") strDisplay = QString(tr("* Channel %1 now has enabled auditorium mode (set by %2)")).arg(strChannel, strWho);
            else if (strFlag == "-u") strDisplay = QString(tr("* Channel %1 no longer has enabled auditorium mode (set by %2)")).arg(strChannel, strWho);
            else if (strFlag == "+l") strDisplay = QString(tr("* Channel %1 now has max number of users set to %2 (set by %3)")).arg(strChannel, strValue, strWho);
            else if (strFlag == "-l") strDisplay = QString(tr("* Channel %1 no longer has max number of users set (set by %2)")).arg(strChannel, strWho);
            else if (strFlag == "+G") strDisplay = QString(tr("* Channel %1 now has word censoring enabled (set by %2)")).arg(strChannel, strWho);
            else if (strFlag == "-G") strDisplay = QString(tr("* Channel %1 no longer has word censoring enabled (set by %2)")).arg(strChannel, strWho);
            else if (strFlag == "+Q") strDisplay = QString(tr("* Channel %1 now has blocked kick (set by %2)")).arg(strChannel, strWho);
            else if (strFlag == "-Q") strDisplay = QString(tr("* Channel %1 no longer has blocked kick (set by %2)")).arg(strChannel, strWho);
            else if (strFlag == "+V") strDisplay = QString(tr("* Channel %1 now has blocked invites for 1 hour (set by %2)")).arg(strChannel, strWho);
            else if (strFlag == "-V") strDisplay = QString(tr("* Channel %1 no longer has blocked invites (set by %2)")).arg(strChannel, strWho);
            else if (strFlag == "+L") strDisplay = QString(tr("* Channel %1 now has limit redirection to %2 channel (set by %3)")).arg(strChannel, strValue, strWho);
            else if (strFlag == "-L") strDisplay = QString(tr("* Channel %1 no longer has limit redirection (set by %2)")).arg(strChannel, strWho);
            else if (strFlag == "+J") strDisplay = QString(tr("* Channel %1 now prevents users from automatically rejoining the channel when they are kicked. Limit is set to %2 sec. (set by %3)")).arg(strChannel, strValue, strWho);
            else if (strFlag == "-J") strDisplay = QString(tr("* Channel %1 no longer prevents users from automatically rejoining the channel when they are kicked (set by %2)")).arg(strChannel, strWho);
            else if (strFlag == "+D") strDisplay = QString(tr("* Channel %1 is now protected channel (set by %2)")).arg(strChannel, strWho);
            else if (strFlag == "-D") strDisplay = QString(tr("* Channel %1 is no longer protected channel (set by %2)")).arg(strChannel, strWho);

            else if (strFlag.at(1) == 'F')
            {
                QString strStatus;
                if (strValue.toInt() == 0) strStatus = tr("Wild");
                else if (strValue.toInt() == 1) strStatus = tr("Tame");
                else if (strValue.toInt() == 2) strStatus = tr("Classy");
                else if (strValue.toInt() == 3) strStatus = tr("Cult");
                else strStatus = tr("unknown");

                if (strFlag.at(0) == '+')
                    strDisplay = QString(tr("* Channel %1 now has status %2 (set by %3)")).arg(strChannel, strStatus, strWho);
                else if (strFlag.at(0) == '-')
                    strDisplay = QString(tr("* %1 changed channel %2 status")).arg(strWho, strChannel);
            }
            else if (strFlag.at(1) == 'c')
            {
                QString strCategory;
                if (strValue.toInt() == 1) strCategory = tr("Teen");
                else if (strValue.toInt() == 2) strCategory = tr("Common");
                else if (strValue.toInt() == 3) strCategory = tr("Erotic");
                else if (strValue.toInt() == 4) strCategory = tr("Thematic");
                else if (strValue.toInt() == 5) strCategory = tr("Regional");
                else strCategory = tr("unknown");

                if (strFlag.at(0) == '+')
                    strDisplay = QString(tr("* Channel %1 now belongs to a category %2 (set by %3)")).arg(strChannel, strCategory, strWho);
                else if (strFlag.at(0) == '-')
                    strDisplay = QString(tr("* %1 changed channel %2 category")).arg(strWho, strChannel);
            }
            else
            {
                if (strValue.isEmpty())
                    strDisplay = QString(tr("* Channel %1 now has a flag %2 (set by %3)")).arg(strChannel, strFlag, strWho);
                else
                {
                    strDisplay = QString(tr("* %1 now has a flag %2 (set by %3)")).arg(strValue, strFlag, strWho);
                    bNickFlag = true;
                }
            }

            Message::instance()->showMessage(strChannel, strDisplay, MessageMode);

            if ((bNickFlag) && (!strFlag.isEmpty()))
                Nick::instance()->changeFlag(strValue, strChannel, strFlag);
        }
        flag_value.clear();
    }
    // nick
    else
    {
        // get args
        QString strNick = strNickOrChannel;
        QString strFlag = strDataList.at(3);
        if (strFlag.at(0) == ':') strFlag.remove(0,1);

        // get +-
        QString plusminus = strFlag.at(0);
        // fix flag
        strFlag.remove(0,1);

        // create flags list
        QStringList lFlags;
        for (int i = 0; i < strFlag.size(); ++i)
            lFlags << strFlag.at(i);

        foreach (strFlag, lFlags)
        {
            strFlag = plusminus+strFlag;

            QString strDisplay;
            if (strFlag == "+O") strDisplay = QString(tr("* %1 is marked as NetAdmin")).arg(strNick);
            else if (strFlag == "-O") strDisplay = QString(tr("* %1 is no longer marked as NetAdmin")).arg(strNick);
            else if (strFlag == "+b") strDisplay = QString(tr("* %1 is marked as busy")).arg(strNick);
            else if (strFlag == "-b") strDisplay = QString(tr("* %1 is no longer marked as busy")).arg(strNick);
            else if (strFlag == "+W") strDisplay = QString(tr("* %1 has now enabled public webcam")).arg(strNick);
            else if (strFlag == "-W") strDisplay = QString(tr("* %1 has no longer enabled public webcam")).arg(strNick);
            else if (strFlag == "+V") strDisplay = QString(tr("* %1 has now enabled private webcam")).arg(strNick);
            else if (strFlag == "-V") strDisplay = QString(tr("* %1 has no longer enabled private webcam")).arg(strNick);
            else if (strFlag == "+x") strDisplay = QString(tr("* %1 has now encrypted IP")).arg(strNick);
            else if (strFlag == "-x") strDisplay = QString(tr("* %1 has no longer encrypted IP")).arg(strNick);
            else if (strFlag == "+r") strDisplay = QString(tr("* %1 is marked as registered and identified")).arg(strNick);
            else if (strFlag == "-r") strDisplay = QString(tr("* %1 is no longer marked as registered and identified")).arg(strNick);
            else
                strDisplay = QString(tr("* %1 now has a flag %2")).arg(strNick, strFlag);

            if ((strFlag.contains("r")) || (strFlag.contains("x")))
                Message::instance()->showMessage(STATUS_WINDOW, strDisplay, MessageMode);

            if (!strFlag.isEmpty())
                Nick::instance()->changeFlag(strNick, strFlag);

            // registered nick
            if ((strNick == Settings::instance()->get("nick")) && (strFlag == "+r"))
            {
                // channel homes
                // Core::instance()->network->send("PRIVMSG NickServ :ALIST");

                // get my stats
                // Core::instance()->network->send(QString("RS INFO %1").arg(Settings::instance()->get("nick")));
            }
        }
    }
}

// :Merovingian!26269559@2294E8.94913F.2EAEC9.11F26D PRIVMSG @#scc :hello
// :Merovingian!26269559@2294E8.94913F.2EAEC9.11F26D PRIVMSG %#scc :hello
// :Merovingian!26269559@2294E8.94913F.2EAEC9.11F26D PRIVMSG +#scc :hello
// :Merovingian!26269559@2294E8.94913F.2EAEC9.11F26D PRIVMSG #scc :hello
// :Merovingian!26269559@2294E8.94913F.2EAEC9.11F26D PRIVMSG ^scc_test :hello
void IrcKernel::raw_privmsg()
{
    if (strDataList.size() < 3) return;

    QString strNick = strDataList.at(0);
    if (strNick.at(0) == ':') strNick.remove(0,1);
    strNick = strNick.left(strNick.indexOf('!'));

    QString strMessage;
    for (int i = 3; i < strDataList.size(); ++i) { if (i != 3) strMessage += " "; strMessage += strDataList.at(i); }
    if ((!strMessage.isEmpty()) && (strMessage.at(0) == ':')) strMessage.remove(0,1);

    // ctcp version
    if (strMessage == QString("%1VERSION%1").arg(QString(QByteArray("\x01")))) {
        return;
    }

    // convert emots //
    Convert::simpleReverseConvert(strMessage);
    // convert emots :)
    Replace::replaceEmots(strMessage);

    QString strNickOrChannel = strDataList.at(2);

    //channel
    if (strNickOrChannel.contains('#'))
    {
        QString strFullChannel = strNickOrChannel;
        QString strGroup = strFullChannel.left(strFullChannel.indexOf('#'));
        Q_UNUSED (strGroup);

        QString strChannel = strFullChannel.right(strFullChannel.length()-strFullChannel.indexOf('#'));
        Message::instance()->showMessage(strChannel, strMessage, MessageDefault, strNick);
    }
    // priv
    else if (strNickOrChannel.contains('^'))
    {
        QString strChannel = strNickOrChannel;
        Message::instance()->showMessage(strChannel, strMessage, MessageDefault, strNick);
    }
    // nick
    else
    {
        QString strDisplay = QString("-%1- %2").arg(strNick, strMessage);
        Message::instance()->showMessageActive(strDisplay, MessageNotice);
    }
}

// :Llanero!43347263@admin.onet NOTICE #channel :test
// :cf1f2.onet NOTICE scc_test :Your message has been filtered and opers notified: spam #2480
// :Llanero!43347263@admin.onet NOTICE $* :458852 * * :%Fb%%C008100%Weź udział w Konkursie Mikołajkowym - skompletuj zaprzęg Świetego Mikołaja! Więcej info w Wieściach z Czata ! http://czat.onet.pl/1632594,wiesci.html
// :Panie_kierowniku!57643619@devel.onet NOTICE Darom :458852 * * :bum
// :Panie_kierowniku!57643619@devel.onet NOTICE Darom :458853 * * :bum
// :Panie_kierowniku!57643619@devel.onet NOTICE Darom :458854 * * :bum
// :Panie_kierowniku!57643619@devel.onet NOTICE Darom :458855 * * :bum
void IrcKernel::raw_notice()
{
    if (strDataList.size() < 3) return;

    QString strWho = strDataList.at(0);
    if (strWho.at(0) == ':') strWho.remove(0,1);
    strWho = strWho.left(strWho.indexOf('!'));

    QString strNickOrChannel = strDataList.at(2);
    QString strMessage;

    QString strCategory = strDataList.at(3);
    if (strCategory.at(0) == ':') strCategory.remove(0,1);
    int iCategory = strCategory.toInt();
    QString strCategoryString;

    switch (iCategory)
    {
        case NOTICE_INFO:
            if (strCategoryString.isEmpty()) strCategoryString = tr("Information")+": ";
        case NOTICE_WARNING:
            if (strCategoryString.isEmpty()) strCategoryString = tr("Warning")+": ";
        case NOTICE_ERROR:
            if (strCategoryString.isEmpty()) strCategoryString = tr("Error")+": ";
        case NOTICE_QUESTION:
            if (strCategoryString.isEmpty()) strCategoryString = tr("Question")+": ";

            for (int i = 6; i < strDataList.size(); ++i) { if (i != 6) strMessage += " "; strMessage += strDataList.at(i); }
            if ((!strMessage.isEmpty()) && (strMessage.at(0) == ':')) strMessage.remove(0,1);
            break;
        default:
            for (int i = 3; i < strDataList.size(); ++i) { if (i != 3) strMessage += " "; strMessage += strDataList.at(i); }
            if ((!strMessage.isEmpty()) && (strMessage.at(0) == ':')) strMessage.remove(0,1);
            break;
    }

    QString strDisplay = QString("-%1- %2%3").arg(strWho, strCategoryString, strMessage);

    // channel
    if (strNickOrChannel.contains('#'))
    {
        QString strFullChannel = strNickOrChannel;
        QString strGroup = strFullChannel.left(strFullChannel.indexOf('#'));
        Q_UNUSED (strGroup);

        QString strChannel = strFullChannel.right(strFullChannel.length()-strFullChannel.indexOf('#'));
        Message::instance()->showMessage(strChannel, strDisplay, MessageNotice);
    }
    // priv
    else if (strNickOrChannel.contains('^'))
    {
        QString strChannel = strNickOrChannel;
        Message::instance()->showMessage(strChannel, strDisplay, MessageNotice);
    }
    // nick
    else
    {
        Message::instance()->showMessageActive(strDisplay, MessageNotice);
    }
}

// :osa1987!47751777@F4C727.DA810F.7E1789.E71ED5 INVITE scc_test :^cf1f41437962
// :Merovingian!26269559@jest.piekny.i.uroczy.ma.przesliczne.oczy INVITE scc_test :#Komputery
void IrcKernel::raw_invite()
{
    if (strDataList.size() < 4) return;

    QString strNick = strDataList.at(0);
    if (strNick.at(0) == ':') strNick.remove(0,1);
    strNick = strNick.left(strNick.indexOf('!'));

    QString strIP = strDataList.at(0);
    if (strIP.at(0) == ':') strIP.remove(0,1);
    strIP = strIP.right(strIP.length()-strIP.indexOf('@')-1);

    QString strChannel = strDataList.at(3);
    if (strChannel.at(0) == ':') strChannel.remove(0,1);

    // priv name
    if (strChannel.at(0) == '^')
        Channel::instance()->setAlternativeName(strChannel, strNick);

    QString strUuid = QUuid::createUuid().toString();
    qint64 iDateTime = QDateTime::currentMSecsSinceEpoch();

    // add invite notification
    Invite::instance()->add(strUuid, iDateTime, strNick, strChannel);

    // is active window
    if (Core::instance()->mainWindow()->isActiveWindow())
    {
        (new InviteGui(strUuid, iDateTime, strNick, strChannel))->show(); // should be show - prevent hangup!
    }

    // sound
    if (Settings::instance()->getBool("sound"))
        SoundNotify::instance()->play(Query);
}

// :cf1f3.onet TOPIC #scc :Simple Chat Client; current version: beta;
void IrcKernel::raw_topic()
{
    if (strDataList.size() < 3) return;

    QString strWho = strDataList.at(0);
    if (strWho.at(0) == ':') strWho.remove(0,1);
    strWho = strWho.left(strWho.indexOf('!'));

    QString strChannel = strDataList.at(2);

    QString strTopic;
    for (int i = 3; i < strDataList.size(); ++i) { if (i != 3) strTopic += " "; strTopic += strDataList.at(i); }
    if ((!strTopic.isEmpty()) && (strTopic.at(0) == ':')) strTopic.remove(0,1);

    QString strDisplay = QString(tr("* %1 changed the topic to: %2")).arg(strWho, strTopic);

    // exist channel
    if (Channel::instance()->contains(strChannel))
    {
        // show msg
        Message::instance()->showMessage(strChannel, strDisplay, MessageMode);

        // set topic in widget
        Channel::instance()->setTopic(strChannel, strTopic);
    }

    // get info
    // Core::instance()->network->send(QString("CS INFO %1 i").arg(strChannel));
}

// :~test34534!anonymous@2294E8.94913F.A00186.1A3C28 INVREJECT Merovingian #Scrabble
// :Merovingian!26269559@2294E8.94913F.2E3993.4AF50D INVREJECT scc_test ^cf1f41038619
void IrcKernel::raw_invreject()
{
    if (strDataList.size() < 4) return;

    QString strWho = strDataList.at(0);
    if (strWho.at(0) == ':') strWho.remove(0,1);
    strWho = strWho.left(strWho.indexOf('!'));

    QString strChannel = strDataList.at(3);

    QString strDisplay;
    if (strChannel.at(0) == '^')
        strDisplay = QString(tr("* %1 rejected an invitation to priv")).arg(strWho);
    else
        strDisplay = QString(tr("* %1 rejected an invitation to channel %2")).arg(strWho, strChannel);

    Message::instance()->showMessage(strChannel, strDisplay, MessageInfo);
}

// :~test34534!anonymous@2294E8.94913F.A00186.1A3C28 INVIGNORE Merovingian #Scrabble
// :Merovingian!26269559@2294E8.94913F.A00186.4A2B76 INVIGNORE scc_test ^cf1f31294352
void IrcKernel::raw_invignore()
{
    if (strDataList.size() < 4) return;

    QString strWho = strDataList.at(0);
    if (strWho.at(0) == ':') strWho.remove(0,1);
    strWho = strWho.left(strWho.indexOf('!'));

    QString strChannel = strDataList.at(3);

    if (strChannel.at(0) == '^')
    {
        Channel::instance()->setAlternativeName(strChannel, strWho);

        pTabC->renameTab(strChannel, strWho);
    }

    QString strDisplay;
    if (strChannel.at(0) == '^')
        strDisplay = QString(tr("* %1 ignored your invitation to the priv")).arg(strWho);
    else
        strDisplay = QString(tr("* %1 ignored your invitation to the channel %2")).arg(strWho, strChannel);

    // display
    Message::instance()->showMessage(strChannel, strDisplay, MessageInfo);
}

// :~testa!anonymous@3DE379.B7103A.6CF799.6902F4 MODERMSG test1 - #Scrabble :%F:verdana%%Ihehe%
void IrcKernel::raw_modermsg()
{
    if (strDataList.size() < 5) return;

    QString strModerator = strDataList.at(0);
    if (strModerator.at(0) == ':') strModerator.remove(0,1);
    strModerator = strModerator.left(strModerator.indexOf('!'));

    QString strNick = strDataList.at(2);
    QString strChannel = strDataList.at(4);

    QString strMessage;
    for (int i = 5; i < strDataList.size(); ++i) { if (i != 5) strMessage += " "; strMessage += strDataList.at(i); }
    if ((!strMessage.isEmpty()) && (strMessage.at(0) == ':')) strMessage.remove(0,1);

    QString strDisplay = QString("%1 [%2 %3]").arg(strMessage, tr("Moderated by"), strModerator);

    // display
    Message::instance()->showMessage(strChannel, strDisplay, MessageDefault, strNick);
}

// :~testa!anonymous@3DE379.B7103A.6CF799.6902F4 MODERNOTICE #Scrabble :a
void IrcKernel::raw_modernotice()
{
    if (strDataList.size() < 3) return;

    QString strNick = strDataList.at(0);
    if (strNick.at(0) == ':') strNick.remove(0,1);
    strNick = strNick.left(strNick.indexOf('!'));

    QString strChannel = strDataList.at(2);

    QString strMessage;
    for (int i = 3; i < strDataList.size(); ++i) { if (i != 3) strMessage += " "; strMessage += strDataList.at(i); }
    if ((!strMessage.isEmpty()) && (strMessage.at(0) == ':')) strMessage.remove(0,1);

    // display
    Message::instance()->showMessage(strChannel, strMessage, MessageModerNotice, strNick);
}

// :cf1f1.onet MODERATE ~testa opnick #channel cf1f44c3b4b870f8a :%F:verdana%ladnie to tak
void IrcKernel::raw_moderate()
{
    if (strDataList.size() < 6) return;

    QString strNick = strDataList.at(3);
    QString strChannel = strDataList.at(4);

    QString strMessage;
    for (int i = 6; i < strDataList.size(); ++i) { if (i != 6) strMessage += " "; strMessage += strDataList.at(i); }
    if ((!strMessage.isEmpty()) && (strMessage.at(0) == ':')) strMessage.remove(0,1);

    Convert::simpleConvert(strMessage);

    qint64 iTime = QDateTime::currentMSecsSinceEpoch();

    Channel::instance()->addModerateMessage(strChannel, iTime, strNick, strMessage);
}

// :cf1f4.onet KILL scc_test :cf1f4.onet (Killed (Nickname collision))
// :Darom!12265854@devel.onet KILL Merovingian :cf1f4.onet!devel.onet!Darom (Killed (Darom (bo tak)))
void IrcKernel::raw_kill()
{
    if (strDataList.size() < 4) return;

    QString strNick = strDataList.at(2);
    if (strNick.at(0) == ':') strNick.remove(0,1);
    strNick = strNick.left(strNick.indexOf('!'));

    QString strWho = strDataList.at(3);
    if (strWho.at(0) == ':') strWho.remove(0,1);
    QRegExp rx("(.*)!(.*)!(.*)");
    if (rx.exactMatch(strWho))
        strWho = strWho.replace(QRegExp("(.*)!(.*)!(.*)"), "\\3");

    QString strReason;
    for (int i = 4; i < strDataList.size(); ++i) { if (i != 4) strReason += " "; strReason += strDataList.at(i); }

    QString strDisplay = QString(tr("* You were killed by %1 %2")).arg(strWho, strReason);

    // display
    Message::instance()->showMessageAll(strDisplay, MessageError);
}

// :Darom!12265854@devel.onet NICK dm
void IrcKernel::raw_nick()
{
    if (strDataList.size() < 3) return;

    QString strNick = strDataList.at(0);
    if (strNick.at(0) == ':') strNick.remove(0,1);
    strNick = strNick.left(strNick.indexOf('!'));

    QString strNewNick = strDataList.at(2);
    if (strNewNick.at(0) == ':') strNewNick.remove(0,1);

    QString strDisplay = QString(tr("* %1 changed nick to %2")).arg(strNick, strNewNick);

    // update nick
    Nick::instance()->rename(strNick, strNewNick, strDisplay);

    // self
    QString strMe = Settings::instance()->get("nick");
    if (strNick == strMe)
    {
        ProfileManager::instance()->renameProfile(strNick, strNewNick);

        // update nick
        Core::instance()->mainWindow()->updateNick(strNewNick);
    }
}

// :cf1f1.onet SNONOTICE f :FILTER: StyX1 had their message filtered, target was #scc: Naruszanie regulaminu czata #29012
void IrcKernel::raw_snonotice()
{
    if (strDataList.size() < 3) return;

    QString strNick = strDataList.at(0);
    if (strNick.at(0) == ':') strNick.remove(0,1);
    strNick = strNick.left(strNick.indexOf('!'));

    QString strMessage;
    for (int i = 3; i < strDataList.size(); ++i) { if (i != 3) strMessage += " "; strMessage += strDataList.at(i); }
    if (strMessage.at(0) == ':') strMessage.remove(0,1);

    QString strDisplay = QString("-%1- %2").arg(strNick, strMessage);

    Message::instance()->showMessage(STATUS_WINDOW, strDisplay, MessageNotice);
}

// :cf1f4.onet 001 scc_test :Welcome to the OnetCzat IRC Network scc_test!51976824@83.28.35.219
void IrcKernel::raw_001()
{
    // logged
    Settings::instance()->setBool("logged", true);

    // clear
    Friends::instance()->clear();
    Ignore::instance()->clear();
    ChannelFavourites::instance()->clear();
    ChannelList::instance()->clear();
    MyStats::instance()->clear();
    MyProfile::instance()->clear();
    ChannelHomes::instance()->clear();
    // user profile
    UserProfile::instance()->clear();
    // channel settings
    ChannelSettings::instance()->clear();

    // me
    QString strMe = Settings::instance()->get("nick");

    // login
    Config *pConfig = new Config(ProfileConfig, strMe);
    QString strPassword = pConfig->get("pass");
    delete pConfig;

    // got password
    if (!strPassword.isEmpty()) {
        // decrypt pass
        SimpleCrypt *pSimpleCrypt = new SimpleCrypt();
        strPassword = pSimpleCrypt->decrypt(strMe, strPassword);
        delete pSimpleCrypt;

        Core::instance()->network->send(QString("PRIVMSG NickServ :IDENTIFY %1").arg(strPassword));
    }

    // flag to get all flags per nick, not only max
    Core::instance()->network->send("CAP REQ :multi-prefix");
    Core::instance()->network->send("CAP END");

    // busy
    Settings::instance()->setBool("busy", false);

    // away
    Settings::instance()->setBool("away", false);

    // auto busy
    //if (Settings::instance()->getBool("auto_busy"))
        //Core::instance()->network->send("BUSY 1");

    // block CTCP
    Core::instance()->network->send(QString("MODE %1 +T").arg(strMe));

    // ignore favourites
    if (Settings::instance()->getBool("autojoin_favourites"))
        Settings::instance()->setBool("ignore_favourites", false);
    else
        Settings::instance()->setBool("ignore_favourites", true);

    // override off
    Settings::instance()->setBool("override", false);

    // age check on
    Settings::instance()->setBool("age_check", true);

    // auto rejoin
    QList<CaseIgnoreString> lChannelsCaseIgnore = Channel::instance()->getListClearedSorted();
    if (!lChannelsCaseIgnore.isEmpty())
    {
        QString strMassRejoin;
        foreach (const QString &strChannel, lChannelsCaseIgnore)
        {
            if (strMassRejoin.isEmpty())
                strMassRejoin += "JOIN "+strChannel;
            else
                strMassRejoin += ","+strChannel;
        }
        Core::instance()->network->send(strMassRejoin);
    }

    // channel list
    Core::instance()->network->send("LIST");

    // update last active
    Settings::instance()->set("last_active", QString::number(QDateTime::currentMSecsSinceEpoch()));

    // auto-away
    if (Settings::instance()->getBool("auto_away"))
        Autoaway::instance()->start();
}

// :cf1f4.onet 002 Merovingian :Your host is cf1f4.onet, running version InspIRCd-1.1
void IrcKernel::raw_002()
{
}

// :cf1f4.onet 003 Merovingian :This server was created 06:35:35 Jan 12 2010
void IrcKernel::raw_003()
{
}

// :cf1f4.onet 004 Merovingian cf1f4.onet InspIRCd-1.1 BGQRVWbinoqrswx DFGIJLMPQRVXYabcehiklmnopqrstuv FIJLXYabcehkloqv
void IrcKernel::raw_004()
{
}

// :cf1f4.onet 005 ~test WALLCHOPS WALLVOICES MODES=19 CHANTYPES=^# PREFIX=(qaohXYv)`&@%!=+ MAP MAXCHANNELS=20 MAXBANS=60 VBANLIST NICKLEN=32 CASEMAPPING=rfc1459 STATUSMSG=@%+ CHARSET=ascii :are supported by this server
// :cf1f4.onet 005 ~test TOPICLEN=203 KICKLEN=255 MAXTARGETS=20 AWAYLEN=200 CHANMODES=Ibe,k,Fcl,DKMRVimnprstu FNC NETWORK=OnetCzat MAXPARA=32 ELIST=MU OVERRIDE ONETNAMESX INVEX=I EXCEPTS=e :are supported by this server
// :cf1f4.onet 005 ~test INVIGNORE=100 USERIP ESILENCE SILENCE=100 WATCH=200 NAMESX :are supported by this server
void IrcKernel::raw_005()
{
    QHash<QString,QString> mKeyValue;

    foreach (QString strData, strDataList)
    {
        if (strData.contains("="))
        {
            QString strKey = strData.mid(0, strData.indexOf('='));
            QString strValue = strData.mid(strData.indexOf('=')+1, strData.length() - strData.indexOf('='));

            mKeyValue.insert(strKey, strValue);
        }
    }
}

// STATS l
// <server> 211 <nick> irc1.sprynet.com 0 49243278 3327277 22095880 1208863 :134450
void IrcKernel::raw_211()
{
    if (strDataList.size() < 4) return;

    QString strMessage;
    for (int i = 3; i < strDataList.size(); ++i) { if (i != 3) strMessage += " "; strMessage += strDataList.at(i); }
    strMessage = "* "+strMessage;

    Message::instance()->showMessageActive(strMessage, MessageInfo);
}

// STATS m
// <server> 212 <nick> PRIVMSG 28931 1446042
void IrcKernel::raw_212()
{
    if (strDataList.size() < 4) return;

    QString strMessage;
    for (int i = 3; i < strDataList.size(); ++i) { if (i != 3) strMessage += " "; strMessage += strDataList.at(i); }
    strMessage = "* "+strMessage;

    Message::instance()->showMessageActive(strMessage, MessageInfo);
}

// STATS c
// <server> 213 <nick> C *@205.150.226.4 * toronto.on.ca.dal.net 0 50
void IrcKernel::raw_213()
{
    if (strDataList.size() < 4) return;

    QString strMessage;
    for (int i = 3; i < strDataList.size(); ++i) { if (i != 3) strMessage += " "; strMessage += strDataList.at(i); }
    strMessage = "* "+strMessage;

    Message::instance()->showMessageActive(strMessage, MessageInfo);
}

// STATS p
// <server> 215 I <nick> <ipmask> * <hostmask> <port> <class>
// <server> 215 I <nick> *@204.127.145.* * NOMATCH 0 3
void IrcKernel::raw_215()
{
    if (strDataList.size() < 4) return;

    QString strMessage;
    for (int i = 3; i < strDataList.size(); ++i) { if (i != 3) strMessage += " "; strMessage += strDataList.at(i); }
    strMessage = "* "+strMessage;

    Message::instance()->showMessageActive(strMessage, MessageInfo);
}

// STATS Y
// <server> 218 <nick> Y 100 120 240 1 5000000
void IrcKernel::raw_218()
{
    if (strDataList.size() < 4) return;

    QString strMessage;
    for (int i = 3; i < strDataList.size(); ++i) { if (i != 3) strMessage += " "; strMessage += strDataList.at(i); }
    strMessage = "* "+strMessage;

    Message::instance()->showMessageActive(strMessage, MessageInfo);
}

// :cf1f3.onet 219 Darom d :End of /STATS report
void IrcKernel::raw_219()
{
}

// STATS m
// <server> 240 <nick> :<info>
void IrcKernel::raw_240()
{
    if (strDataList.size() < 4) return;

    QString strMessage;
    for (int i = 3; i < strDataList.size(); ++i) { if (i != 3) strMessage += " "; strMessage += strDataList.at(i); }
    if ((!strMessage.isEmpty()) && (strMessage.at(0) == ':')) strMessage.remove(0,1);
    strMessage = "* "+strMessage;

    Message::instance()->showMessageActive(strMessage, MessageInfo);
}

// STATS u
// <server> 242 <nick> :Server Up 37 days, 9:20:39
void IrcKernel::raw_242()
{
    if (strDataList.size() < 4) return;

    QString strMessage;
    for (int i = 3; i < strDataList.size(); ++i) { if (i != 3) strMessage += " "; strMessage += strDataList.at(i); }
    if ((!strMessage.isEmpty()) && (strMessage.at(0) == ':')) strMessage.remove(0,1);
    strMessage = "* "+strMessage;

    Message::instance()->showMessageActive(strMessage, MessageInfo);
}

// STATS o
// <server> 243 <nick> O *@calley.stlnet.com * konjump 4293328895 1
void IrcKernel::raw_243()
{
    if (strDataList.size() < 4) return;

    QString strMessage;
    for (int i = 3; i < strDataList.size(); ++i) { if (i != 3) strMessage += " "; strMessage += strDataList.at(i); }
    strMessage = "* "+strMessage;

    Message::instance()->showMessageActive(strMessage, MessageInfo);
}

// STATS h
// <server> 244 <nick> H * * indy.in.us.dal.net 0 -1
void IrcKernel::raw_244()
{
    if (strDataList.size() < 4) return;

    QString strMessage;
    for (int i = 3; i < strDataList.size(); ++i) { if (i != 3) strMessage += " "; strMessage += strDataList.at(i); }
    strMessage = "* "+strMessage;

    Message::instance()->showMessageActive(strMessage, MessageInfo);
}

// STATS U
// <server> 248 <nick> U <host> * <??> <??> <??>
// <server> 248 <nick> U Uworld2.Undernet.Org * * 0 -1
void IrcKernel::raw_248()
{
    if (strDataList.size() < 4) return;

    QString strMessage;
    for (int i = 3; i < strDataList.size(); ++i) { if (i != 3) strMessage += " "; strMessage += strDataList.at(i); }
    strMessage = "* "+strMessage;

    Message::instance()->showMessageActive(strMessage, MessageInfo);
}

// STATS p
// <server> 249 <nick> :<info>
// <server> 249 <nick> :nick collisions 1879 unknown closes 11383991
void IrcKernel::raw_249()
{
    if (strDataList.size() < 4) return;

    QString strMessage;
    for (int i = 3; i < strDataList.size(); ++i) { if (i != 3) strMessage += " "; strMessage += strDataList.at(i); }
    if ((!strMessage.isEmpty()) && (strMessage.at(0) == ':')) strMessage.remove(0,1);
    strMessage = "* "+strMessage;

    Message::instance()->showMessageActive(strMessage, MessageInfo);
}

// LUSERS
// :cf1f4.onet 251 Merovingian :There are 2300 users and 5 invisible on 10 servers
void IrcKernel::raw_251()
{
}

// LUSERS
// :cf1f4.onet 252 Merovingian 5 :operator(s) online
void IrcKernel::raw_252()
{
}

// LUSERS
// :cf1f4.onet 253 Merovingian 1 :unknown connections
void IrcKernel::raw_253()
{
}

// LUSERS
// :cf1f4.onet 254 Merovingian 4641 :channels formed
void IrcKernel::raw_254()
{
}

// LUSERS
// :cf1f4.onet 255 Merovingian :I have 568 clients and 1 servers
void IrcKernel::raw_255()
{
}

// ADMIN
// :cf1f1.onet 256 ~test :Administrative info for cf1f1.onet
void IrcKernel::raw_256()
{
    if (strDataList.size() < 4) return;

    QString strMessage;
    for (int i = 3; i < strDataList.size(); ++i) { if (i != 3) strMessage += " "; strMessage += strDataList.at(i); }
    if ((!strMessage.isEmpty()) && (strMessage.at(0) == ':')) strMessage.remove(0,1);
    strMessage = "* "+strMessage;

    Message::instance()->showMessage(STATUS_WINDOW, strMessage, MessageInfo);
}

// ADMIN
// :cf1f1.onet 257 ~test :Name     - Czat Admin
void IrcKernel::raw_257()
{
    if (strDataList.size() < 4) return;

    QString strMessage;
    for (int i = 3; i < strDataList.size(); ++i) { if (i != 3) strMessage += " "; strMessage += strDataList.at(i); }
    if ((!strMessage.isEmpty()) && (strMessage.at(0) == ':')) strMessage.remove(0,1);
    strMessage = "* "+strMessage;

    Message::instance()->showMessage(STATUS_WINDOW, strMessage, MessageInfo);
}

// ADMIN
// :cf1f1.onet 258 ~test :Nickname - czat_admin
void IrcKernel::raw_258()
{
    if (strDataList.size() < 4) return;

    QString strMessage;
    for (int i = 3; i < strDataList.size(); ++i) { if (i != 3) strMessage += " "; strMessage += strDataList.at(i); }
    if ((!strMessage.isEmpty()) && (strMessage.at(0) == ':')) strMessage.remove(0,1);
    strMessage = "* "+strMessage;

    Message::instance()->showMessage(STATUS_WINDOW, strMessage, MessageInfo);
}

// ADMIN
// :cf1f1.onet 259 ~test :E-Mail   - czat_admin@czat.onet.pl
void IrcKernel::raw_259()
{
    if (strDataList.size() < 4) return;

    QString strMessage;
    for (int i = 3; i < strDataList.size(); ++i) { if (i != 3) strMessage += " "; strMessage += strDataList.at(i); }
    if ((!strMessage.isEmpty()) && (strMessage.at(0) == ':')) strMessage.remove(0,1);
    strMessage = "* "+strMessage;

    Message::instance()->showMessage(STATUS_WINDOW, strMessage, MessageInfo);
}

// LUSERS
// :cf1f4.onet 265 Merovingian :Current Local Users: 568  Max: 1633
void IrcKernel::raw_265()
{
}

// LUSERS
// :cf1f4.onet 266 Merovingian :Current Global Users: 2305  Max: 6562
void IrcKernel::raw_266()
{
}

// SILENCE
// :cf1f2.onet 271 Merovingian Merovingian Aldinach!*@* <privatemessages,channelmessages,invites>
void IrcKernel::raw_271()
{
// FEATURE
}

// SILENCE
// :cf1f2.onet 272 Merovingian :End of Silence List
void IrcKernel::raw_272()
{
}

// WHOIS Merovingian
// :cf1f2.onet 301 scc_test Merovingian :nie ma
void IrcKernel::raw_301()
{
    if (strDataList.size() < 5) return;

    QString strNick = strDataList.at(3);

    QString strMessage;
    for (int i = 4; i < strDataList.size(); ++i) { if (i != 4) strMessage += " "; strMessage += strDataList.at(i); }
    if ((!strMessage.isEmpty()) && (strMessage.at(0) == ':')) strMessage.remove(0,1);

    QString strDisplay = QString(tr("* %1 is away: %2")).arg(strNick, strMessage);
    Message::instance()->showMessageActive(strDisplay, MessageInfo);
}

// USERHOST a
// :cf1f2.onet 302 Merovingian :
// USERHOST aleksa7
// :cf1f2.onet 302 Merovingian :aleksa7=+14833406@44DC43.4DB130.368946.600B51
void IrcKernel::raw_302()
{
    if (strDataList.size() < 4) return;

    QString strMessage;
    for (int i = 3; i < strDataList.size(); ++i) { if (i != 3) strMessage += " "; strMessage += strDataList.at(i); }
    if ((!strMessage.isEmpty()) && (strMessage.at(0) == ':')) strMessage.remove(0,1);
    strMessage = "* "+strMessage;

    Message::instance()->showMessageActive(strMessage, MessageInfo);
}

// ISON
// :cf1f1.onet 303 ~test :Darom
void IrcKernel::raw_303()
{
    if (strDataList.size() < 4) return;

    QString strNick = strDataList.at(3);
    if (strNick.at(0) == ':') strNick.remove(0,1);

    QString strDisplay = QString("* ISON: %1").arg(strNick);
    Message::instance()->showMessageActive(strDisplay, MessageInfo);
}

// OPER
// :cf1f3.onet 304 ~test :SYNTAX OPER <username> <password>
void IrcKernel::raw_304()
{
    if (strDataList.size() < 4) return;

    QString strMessage;
    for (int i = 3; i < strDataList.size(); ++i) { if (i != 3) strMessage += " "; strMessage += strDataList.at(i); }
    if ((!strMessage.isEmpty()) && (strMessage.at(0) == ':')) strMessage.remove(0,1);
    strMessage = "* "+strMessage;

    Message::instance()->showMessageActive(strMessage, MessageInfo);
}

// AWAY :
// :cf1f3.onet 305 scc_test :You are no longer marked as being away
void IrcKernel::raw_305()
{
    Away::instance()->stop();

    QString strDisplay = tr("* You are no longer marked as being away");
    Message::instance()->showMessageActive(strDisplay, MessageInfo);
}

// AWAY :reason
// :cf1f3.onet 306 scc_test :You have been marked as being away
void IrcKernel::raw_306()
{
    Away::instance()->start();

    QString strDisplay = tr("* You have been marked as being away");
    Message::instance()->showMessageActive(strDisplay, MessageInfo);
}

// WHOIS
// :cf1f3.onet 307 scc_test Merovingian :is a registered nick
void IrcKernel::raw_307()
{
    if (strDataList.size() < 4) return;

    QString strNick = strDataList.at(3);

    QString strMessage;
    for (int i = 4; i < strDataList.size(); ++i) { if (i != 4) strMessage += " "; strMessage += strDataList.at(i); }
    if ((!strMessage.isEmpty()) && (strMessage.at(0) == ':')) strMessage.remove(0,1);

    if (strMessage == "is a registered nick")
        strMessage = tr("is a registered nick");

    QString strDisplay = QString("* %1 %2").arg(strNick, strMessage);
    Message::instance()->showMessageActive(strDisplay, MessageInfo);
}

// WHOIS
// :cf1f1.onet 311 scc_test Merovingian 26269559 2294E8.94913F.196694.9BAE58 * :Merovingian
void IrcKernel::raw_311()
{
    if (strDataList.size() < 8) return;

    QString strNick = strDataList.at(3);
    QString strZUO = strDataList.at(4);
    QString strIP = strDataList.at(5);
    QString strIrcname;

    for (int i = 7; i < strDataList.size(); ++i) { if (i != 7) strIrcname += " "; strIrcname += strDataList.at(i); }
    if ((!strIrcname.isEmpty()) && (strIrcname.at(0) == ':')) strIrcname.remove(0,1);

    QString strDisplayNick = QString(tr("* %1 is %2@%3")).arg(strNick, strZUO, strIP);
    Message::instance()->showMessageActive(strDisplayNick, MessageInfo);

    QString strDisplayIrcname = QString(tr("* %1 ircname: %2")).arg(strNick, strIrcname);
    Message::instance()->showMessageActive(strDisplayIrcname, MessageInfo);
}

// WHOIS
// :cf1f2.onet 312 scc_test Merovingian *.onet :OnetCzat
// WHOWAS
// :cf1f2.onet 312 Merovingian merovingian *.onet :Wed Sep  1 18:37:42 2010
void IrcKernel::raw_312()
{
    if (strDataList.size() < 4) return;

    QString strNick = strDataList.at(3);
    QString strServer = strDataList.at(4);

    QString strInfo;
    for (int i = 5; i < strDataList.size(); ++i) { if (i != 5) strInfo += " "; strInfo += strDataList.at(i); }
    if ((!strInfo.isEmpty()) && (strInfo.at(0) == ':')) strInfo.remove(0,1);

    if (strInfo.size() == 24) // WHOWAS
    {
        QString strDisplayServer = QString(tr("* %1 was online via %2")).arg(strNick, strServer);
        Message::instance()->showMessageActive(strDisplayServer, MessageInfo);

        QLocale enUsLocale = QLocale(QLocale::C, QLocale::AnyCountry);
        QDateTime signoff = enUsLocale.toDateTime(strInfo, "ddd MMM dd hh:mm:ss yyyy");
        if (signoff.isNull()) signoff = enUsLocale.toDateTime(strInfo, "ddd MMM d hh:mm:ss yyyy");

        QString strDisplaySignOff;
        if (!signoff.isNull())
            strDisplaySignOff = QString(tr("* %1 was online from %2")).arg(strNick, signoff.toString("dd MMM yyyy hh:mm:ss"));
        else
            strDisplaySignOff = QString(tr("* %1 was online from %2")).arg(strNick, strInfo);

        Message::instance()->showMessageActive(strDisplaySignOff, MessageInfo);
    }
    else // WHOIS
    {
        QString strDisplay = QString(tr("* %1 is online via %2 (%3)")).arg(strNick, strServer, strInfo);
        Message::instance()->showMessageActive(strDisplay, MessageInfo);
    }
}

// WHOIS
// :cf1f1.onet 313 scc_test Llanero :is a GlobalOp on OnetCzat
// :cf1f2.onet 313 Merovingian Darom :is a NetAdmin on OnetCzat
void IrcKernel::raw_313()
{
    if (strDataList.size() < 4) return;

    QString strNick = strDataList.at(3);

    QString strMessage;
    for (int i = 4; i < strDataList.size(); ++i) { if (i != 4) strMessage += " "; strMessage += strDataList.at(i); }
    if ((!strMessage.isEmpty()) && (strMessage.at(0) == ':')) strMessage.remove(0,1);

    QString strDisplay;
    if (strMessage == "is an IRC Operator")
        strDisplay = QString(tr("* %1 is an IRC Operator")).arg(strNick);
    else
        strDisplay = QString("* %1 %2").arg(strNick, strMessage);

    Message::instance()->showMessageActive(strDisplay, MessageInfo);
}

// WHOWAS
// :cf1f4.onet 314 Merovingian wilk 54995510 44DC43.553BE3.6C367A.FFF110 * :admin on ...
void IrcKernel::raw_314()
{
    // copy of raw 311
    if (strDataList.size() < 8) return;

    QString strNick = strDataList.at(3);
    QString strZUO = strDataList.at(4);
    QString strIP = strDataList.at(5);
    QString strIrcname;

    for (int i = 7; i < strDataList.size(); ++i) { if (i != 7) strIrcname += " "; strIrcname += strDataList.at(i); }
    if ((!strIrcname.isEmpty()) && (strIrcname.at(0) == ':')) strIrcname.remove(0,1);

    QString strDisplayNick = QString(tr("* %1 is %2@%3")).arg(strNick, strZUO, strIP);
    Message::instance()->showMessageActive(strDisplayNick, MessageInfo);

    QString strDisplayIrcname = QString(tr("* %1 ircname: %2")).arg(strNick, strIrcname);
    Message::instance()->showMessageActive(strDisplayIrcname, MessageInfo);
}

// WHO
// :cf1f2.onet 315 Merovingian a :End of /WHO list.
// :cf1f4.onet 315 Merovingian #16_17_18_19_lat :Too many results
void IrcKernel::raw_315()
{
    if (strDataList.size() < 4) return;

    QString strNickChannel = strDataList.at(3);

    QString strMessage;
    for (int i = 4; i < strDataList.size(); ++i) { if (i != 4) strMessage += " "; strMessage += strDataList.at(i); }
    if ((!strMessage.isEmpty()) && (strMessage.at(0) == ':')) strMessage.remove(0,1);

    if (strMessage == "End of /WHO list.")
        return;
    else if (strMessage == "Too many results")
        strMessage = tr("Too many results");

    QString strDisplay = QString("* %1 :%2").arg(strNickChannel, strMessage);
    Message::instance()->showMessageActive(strDisplay, MessageInfo);
}

// WHOWAS
// :cf1f2.onet 314 Merovingian merovingian 26269559 2294E8.94913F.75F4D7.D7A8A7 * :Merovingian
void IrcKernel::raw_316()
{
    // copy of raw 311
    if (strDataList.size() < 8) return;

    QString strNick = strDataList.at(3);
    QString strZUO = strDataList.at(4);
    QString strIP = strDataList.at(5);
    QString strIrcname;

    for (int i = 7; i < strDataList.size(); ++i) { if (i != 7) strIrcname += " "; strIrcname += strDataList.at(i); }
    if ((!strIrcname.isEmpty()) && (strIrcname.at(0) == ':')) strIrcname.remove(0,1);

    QString strDisplayNick = QString(tr("* %1 is %2@%3")).arg(strNick, strZUO, strIP);
    Message::instance()->showMessageActive(strDisplayNick, MessageInfo);

    QString strDisplayIrcname = QString(tr("* %1 ircname: %2")).arg(strNick, strIrcname);
    Message::instance()->showMessageActive(strDisplayIrcname, MessageInfo);
}

// WHOIS
// :cf1f3.onet 317 scc_test Merovingian 7 1263650617 :seconds idle, signon time
void IrcKernel::raw_317()
{
    if (strDataList.size() < 6) return;

    QString strNick = strDataList.at(3);
    int iIdle = strDataList.at(4).toInt();
    QString strTime = strDataList.at(5);
    QString strIdle;

    int iSeconds = iIdle % SECONDS_IN_MINUTE ;
    int iInMinutes = iIdle / SECONDS_IN_MINUTE ;
    int iMinutes = iInMinutes % MINUTES_IN_HOUR ;
    int iInHours = iInMinutes / MINUTES_IN_HOUR ;
    int iHours = iInHours % HOURS_IN_DAY ;
    int iDays = iInHours / HOURS_IN_DAY ;

    if (iDays > 0)
        strIdle += QString("%1d ").arg(iDays);
    if (iHours > 0)
        strIdle += QString("%1h ").arg(iHours);
    if (iMinutes > 0)
        strIdle += QString("%1m ").arg(iMinutes);
    if (iSeconds >= 0)
        strIdle += QString("%1s ").arg(iSeconds);

    if (!((iDays == 0) && (iHours == 0) && (iMinutes == 0) && (iSeconds == 0)))
    {
        QString strDisplayIdle = QString(tr("* %1 is away %2")).arg(strNick, strIdle);
        Message::instance()->showMessageActive(strDisplayIdle, MessageInfo);
    }

    QString strDateTime = QDateTime::fromTime_t(strTime.toInt()).toString("dd MMM yyyy hh:mm:ss");

    QString strDisplaySignon = QString(tr("* %1 is logged in since %2")).arg(strNick, strDateTime);
    Message::instance()->showMessageActive(strDisplaySignon, MessageInfo);
}

// WHOIS
// :cf1f4.onet 318 scc_test Merovingian :End of /WHOIS list.
void IrcKernel::raw_318()
{
}

// WHOIS
// :chommik.pirc.pl 319 nick_tymczasowy293 ovo_ :@#ezoteryka #bog @#onet ~#religie #towarzyski #main @#scc ~#medycyna_naturalna @#dla_rysownikow #www.elektroda.pl #mydevil #help ~#cisza #listekklonu #4programmers
void IrcKernel::raw_319()
{
    if (strDataList.size() < 4) return;

    QString strNick = strDataList.at(3);

    QString strChannels;
    for (int i = 4; i < strDataList.size(); ++i)
    {
        if (i != 4) strChannels += " ";
        QString strChannel = strDataList.at(i);
        if (strChannel.at(0) == ':') strChannel.remove(0,1);

        if (strChannel.at(0) != '#')
        {
            // move prefix `#scc => #`scc
            QString prefix = strChannel.left(strChannel.indexOf('#'));
            strChannel.remove(0, strChannel.indexOf("#"));
            strChannel.insert(1, prefix);
        }
        strChannels += strChannel;
    }

    QString strDisplay = QString(tr("* %1 is on channels: %2")).arg(strNick, strChannels);
    Message::instance()->showMessageActive(strDisplay, MessageInfo);
}

// :cf1f4.onet 320 Merovingian Merovingian :is Official Chat Hacker
void IrcKernel::raw_320()
{
    if (strDataList.size() < 5) return;

    QString strNick = strDataList.at(3);

    QString strMessage;
    for (int i = 4; i < strDataList.size(); ++i) { if (i != 4) strMessage += " "; strMessage += strDataList.at(i); }
    if ((!strMessage.isEmpty()) && (strMessage.at(0) == ':')) strMessage.remove(0,1);

    QString strDisplay;
    if (strMessage == "a Network Administrator")
        strDisplay = QString(tr("* %1 is a Network Administrator")).arg(strNick);
    else
        strDisplay = QString("* %1 %2").arg(strNick, strMessage);

    Message::instance()->showMessageActive(strDisplay, MessageInfo);
}

// :legowisko.pirc.pl 321 nick_tymczasowy293 Channel :Users  Name
void IrcKernel::raw_321()
{
    //
}

// :legowisko.pirc.pl 322 nick_tymczasowy293 #akap 2 :[+ntr] temat
void IrcKernel::raw_322()
{
    if (ChannelList::instance()->getStatus() == StatusCompleted)
        return;

    QString strChannelName = strDataList.at(3);
    int iChannelPeople = QString(strDataList.at(4)).toInt();

    OnetChannelList oChannelList;
    oChannelList.name = strChannelName;
    oChannelList.people = iChannelPeople;
    oChannelList.cat = 0;
    oChannelList.type = 0;
    oChannelList.moderated = false;
    oChannelList.recommended = false;

    ChannelList::instance()->add(oChannelList);
}

// :legowisko.pirc.pl 323 nick_tymczasowy293 :End of /LIST
void IrcKernel::raw_323()
{
    ChannelList::instance()->setStatus(StatusCompleted);
    ChannelList::instance()->setTime(QDateTime::currentMSecsSinceEpoch());
}

// :cf1f4.onet 324 Darom #gorący_pokój +DFJcnt 2 20 4
void IrcKernel::raw_324()
{
    if (strDataList.size() < 5) return;

    QString strChannel = strDataList.at(3);

    QString strFlags;
    for (int i = 4; i < strDataList.size(); ++i) { if (i != 4) strFlags += " "; strFlags += strDataList.at(i); }

    QString strDisplay = QString(tr("* Channel %1 flags: %2")).arg(strChannel, strFlags);
    Message::instance()->showMessageActive(strDisplay, MessageInfo);
}

// :cf1f4.onet 329 Darom #gorący_pokój 1174660325
void IrcKernel::raw_329()
{
    if (strDataList.size() < 5) return;

    QString strChannel = strDataList.at(3);
    int iCreatedTime = strDataList.at(4).toInt();

    QString strCreatedTime = QDateTime::fromTime_t(iCreatedTime).toString("dd MMM yyyy hh:mm:ss");

    QString strDisplay = QString(tr("* Channel %1 created at: %2")).arg(strChannel, strCreatedTime);
    Message::instance()->showMessageActive(strDisplay, MessageInfo);
}

// :legowisko.pirc.pl 330 nick_tymczasowy293 pid pidpawel :is logged in as
void IrcKernel::raw_330()
{
    if (strDataList.size() < 4) return;

    QString strNick = strDataList.at(3);
    QString strAccount = strDataList.at(4);

    QString strMessage;
    for (int i = 5; i < strDataList.size(); ++i) { if (i != 5) strMessage += " "; strMessage += strDataList.at(i); }
    if ((!strMessage.isEmpty()) && (strMessage.at(0) == ':')) strMessage.remove(0,1);

    QString strDisplay;
    if (strMessage == "is logged in as")
        strDisplay = QString(tr("* %1 is logged in as %2")).arg(strNick, strAccount);
    else
        strDisplay = strMessage;

    Message::instance()->showMessageActive(strDisplay, MessageInfo);
}

// :cf1f4.onet 332 scc_test #scc :Simple Chat Client; current version: beta;
void IrcKernel::raw_332()
{
    if (strDataList.size() < 5) return;

    QString strChannel = strDataList.at(3);

    QString strTopic;
    for (int i = 4; i < strDataList.size(); ++i) { if (i != 4) strTopic += " "; strTopic += strDataList.at(i); }
    if ((!strTopic.isEmpty()) && (strTopic.at(0) == ':')) strTopic.remove(0,1);

    if (Channel::instance()->contains(strChannel))
        Channel::instance()->setTopic(strChannel, strTopic);
}

// :cf1f1.onet 333 scc_test #scc Merovingian!26269559 1253193639
void IrcKernel::raw_333()
{
// supported by raw 161
}

// USERIP
// :cf1f1.onet 340 Darom :Darom*=+12265854@89.111.111.11
void IrcKernel::raw_340()
{
    if (strDataList.size() < 4) return;

    QString strMessage;
    for (int i = 3; i < strDataList.size(); ++i) { if (i != 3) strMessage += " "; strMessage += strDataList.at(i); }
    if ((!strMessage.isEmpty()) && (strMessage.at(0) == ':')) strMessage.remove(0,1);
    strMessage = "* "+strMessage;

    Message::instance()->showMessageActive(strMessage, MessageInfo);
}

// :cf1f4.onet 341 Merovingian ~test34534 #Scrabble
// :cf1f1.onet 341 scc_test Merovingian ^cf1f1162848
void IrcKernel::raw_341()
{
    if (strDataList.size() < 5) return;

    QString strNick = strDataList.at(3);
    QString strChannel = strDataList.at(4);

    if (strChannel.at(0) == '^')
    {
        Channel::instance()->setAlternativeName(strChannel, strNick);

        pTabC->renameTab(strChannel, strNick);
    }
}

// :cf1f4.onet 335 Merovingian Onet-KaOwiec :is a bot on OnetCzat
void IrcKernel::raw_335()
{
    if (strDataList.size() < 4) return;

    QString strNick = strDataList.at(3);

    QString strMessage;
    for (int i = 4; i < strDataList.size(); ++i) { if (i != 4) strMessage += " "; strMessage += strDataList.at(i); }
    if ((!strMessage.isEmpty()) && (strMessage.at(0) == ':')) strMessage.remove(0,1);

    QString strDisplay;
    if (strMessage == "is a bot on OnetCzat")
        strDisplay = QString(tr("* %1 is a bot on OnetCzat")).arg(strNick);
    else
        strDisplay = strMessage;

    Message::instance()->showMessageActive(strDisplay, MessageInfo);
}

// WHO
// :cf1f2.onet 352 Merovingian #testy 12265854 F3F8AF.464CED.BF6592.28AAB2 *.onet Darom G` :0 Darom
// :cf1f2.onet 352 Merovingian * 26269559 2294E8.94913F.75F4D7.D7A8A7 *.onet Merovingian H :0 Merovingian
// :cf1f2.onet 352 Merovingian #uwaga 43347263 0AD995.BF5FE3.665A1E.9BBABB *.onet Llanero G@ :0 Llanero
// :cf1f2.onet 352 Merovingian #RADIO_PIORUNFM_PL 14833406 44DC43.4DB130.368946.600B51 *.onet aleksa7 H% :0 aleksa7
// :cf1f2.onet 352 Merovingian * 18359115 admin.łona *.onet MAS_PSOTKA G :0 onet-czat
void IrcKernel::raw_352()
{
    if (strDataList.size() < 4) return;

    QString strMessage;
    for (int i = 3; i < strDataList.size(); ++i) { if (i != 3) strMessage += " "; strMessage += strDataList.at(i); }
    strMessage = "* "+strMessage;

    Message::instance()->showMessageActive(strMessage, MessageInfo);
}

// NAMES
// :cf1f1.onet 353 scc_test = #scc :scc_test|rx,0 `@Merovingian|brx,1 @chanky|rx,1
// :cf1f3.onet 353 Merovingian = #hack :%Hacker %weed %cvf @Majkel SzperaCZ_11 Merovingian `ChanServ %but
void IrcKernel::raw_353()
{
    if (strDataList.size() < 5) return;

    QString strChannel = strDataList.at(4);

    for (int i = 5; i < strDataList.size(); ++i)
    {
        if (!strDataList.at(i).isEmpty())
        {
            QString strNick = strDataList.at(i);
            if (i == 5) strNick.remove(0,1); // remove :
            strNick = strNick.left(strNick.indexOf("|"));

            QString strSuffix = QString::null;

            QString strCleanNick = strNick;

            QString strPrefix;
            if (strCleanNick.contains(FLAG_OWNER)) { strCleanNick.remove(FLAG_OWNER); strPrefix.append(FLAG_OWNER); }
            if (strCleanNick.contains(FLAG_ADMIN)) { strCleanNick.remove(FLAG_ADMIN); strPrefix.append(FLAG_ADMIN); }
            if (strCleanNick.contains(FLAG_OP)) { strCleanNick.remove(FLAG_OP); strPrefix.append(FLAG_OP); }
            if (strCleanNick.contains(FLAG_HALFOP)) { strCleanNick.remove(FLAG_HALFOP); strPrefix.append(FLAG_HALFOP); }
            if (strCleanNick.contains(FLAG_MOD)) { strCleanNick.remove(FLAG_MOD); strPrefix.append(FLAG_MOD); }
            if (strCleanNick.contains(FLAG_SCREENER)) { strCleanNick.remove(FLAG_SCREENER); strPrefix.append(FLAG_SCREENER); }
            if (strCleanNick.contains(FLAG_VOICE)) { strCleanNick.remove(FLAG_VOICE); strPrefix.append(FLAG_VOICE); }

            QString strModes = strPrefix+strSuffix;
            Nick::instance()->add(strCleanNick, strChannel, strModes);

            // nick avatar
            if (strCleanNick.at(0) != '~' && !strSuffix.contains(FLAG_BOT)
                    && Themes::instance()->isCurrentWithAvatar()
                    && Nick::instance()->getAvatar(strCleanNick).isEmpty())
            {
                QString strMe = Settings::instance()->get("nick");
                if (strCleanNick == strMe)
                {
                    QString strAvatar = MyProfile::instance()->get("avatar");
                    if (!strAvatar.isEmpty())
                        Avatar::instance()->get(strCleanNick, "nick", strAvatar);
                }
                else
                {
                    // Core::instance()->network->send(QString("NS INFO %1 s").arg(strCleanNick));
                }
            }
        }
    }
}

// :cf1f1.onet 355 Merovingian #uwaga 958 :users
void IrcKernel::raw_355()
{
}

// NAMES
// :cf1f2.onet 366 scc_test #scc :End of /NAMES list.
void IrcKernel::raw_366()
{
}

// WHOWAS
// :cf1f2.onet 369 Merovingian merovingian :End of WHOWAS
void IrcKernel::raw_369()
{
}

// INFO
// :cf1f3.onet 371 ~test :Core Developers:
void IrcKernel::raw_371()
{
    if (strDataList.size() < 4) return;

    QString strMessage;
    for (int i = 3; i < strDataList.size(); ++i) { if (i != 3) strMessage += " "; strMessage += strDataList.at(i); }
    if ((!strMessage.isEmpty()) && (strMessage.at(0) == ':')) strMessage.remove(0,1);
    strMessage = "* "+strMessage;

    Message::instance()->showMessage(STATUS_WINDOW, strMessage, MessageInfo);
}

// MOTD
// :cf1f4.onet 372 scc_test :- Onet Czat. Inny Wymiar Czatowania. Witamy
void IrcKernel::raw_372()
{
    if (strDataList.size() < 4) return;

    QString strMessage;
    for (int i = 3; i < strDataList.size(); ++i) { if (i != 3) strMessage += " "; strMessage += strDataList.at(i); }
    if ((!strMessage.isEmpty()) && (strMessage.at(0) == ':')) strMessage.remove(0,1);

    // Message::instance()->showMessage(STATUS_WINDOW, strMessage, MessageDefault);
}

// INFO
// :cf1f3.onet 374 ~test :End of /INFO list
void IrcKernel::raw_374()
{
}

// MOTD
// :cf1f4.onet 375 scc_test :cf1f4.onet message of the day
void IrcKernel::raw_375()
{
    //QString strDisplay = tr("Message Of The Day:");

    //Message::instance()->showMessage(STATUS_WINDOW, strDisplay, MessageDefault);
}

// MOTD
// :cf1f1.onet 376 scc_test :End of message of the day.
void IrcKernel::raw_376()
{
}

// WHOIS
// :cf1f1.onet 378 Merovingian Merovingian :is connecting from 26269559@46.113.153.49 46.113.153.49
void IrcKernel::raw_378()
{
    if (strDataList.size() < 9) return;

    QString strNick = strDataList.at(3);
    QString strZuoIP = strDataList.at(7);
    QString strIP = strDataList.at(8);

    QString strMessage = QString(tr("* %1 is connecting from %2 %3")).arg(strNick, strZuoIP, strIP);
    Message::instance()->showMessageActive(strMessage, MessageInfo);
}

// WHOIS
// :cf1f4.onet 379 Darom Darom :usermodes [+Oborx]
// WHOWAS
// :cf1f4.onet 379 %s %s :was connecting from *@%s
void IrcKernel::raw_379()
{
    if (strDataList.size() < 6) return;

    QString strNick = strDataList.at(3);

    QString strMessage;
    for (int i = 4; i < strDataList.size(); ++i) { if (i != 4) strMessage += " "; strMessage += strDataList.at(i); }
    if ((!strMessage.isEmpty()) && (strMessage.at(0) == ':')) strMessage.remove(0,1);

    if (strMessage.contains("modes"))
    {
        QString strUserModes = strMessage;
        strUserModes.remove("is using modes ");

        QString strMessage = QString(tr("* %1 is using modes %2")).arg(strNick, strUserModes);
        Message::instance()->showMessageActive(strMessage, MessageInfo);
    }
    else
    {
        QString strHost = strMessage;
        strHost.remove("was connecting from ");

        QString strMessage = QString(tr("* %1 was connecting from %2")).arg(strNick, strHost);
        Message::instance()->showMessageActive(strMessage, MessageInfo);
    }
}

// OPER
// :cf1f1.onet 381 Darom :You are now an IRC operator of type NetAdmin
void IrcKernel::raw_381()
{
    QString strMessage = QString(tr("* You are now an IRC operator of type NetAdmin"));
    Message::instance()->showMessageActive(strMessage, MessageInfo);
}

// TIME
// :cf1f2.onet 391 ~test cf1f2.onet :Tue Jul 13 18:33:05 2010
void IrcKernel::raw_391()
{
    if (strDataList.size() < 4) return;

    QString strServer = strDataList.at(3);

    QString strDateTime;
    for (int i = 4; i < strDataList.size(); ++i) { if (i != 4) strDateTime += " "; strDateTime += strDataList.at(i); }
    if ((!strDateTime.isEmpty()) && (strDateTime.at(0) == ':')) strDateTime.remove(0,1);

    QString strMessage = QString(tr("* Date and time of the server %1: %2")).arg(strServer, strDateTime);

    Message::instance()->showMessageActive(strMessage, MessageInfo);
}

// :cf1f2.onet 396 ~scc_test 3DE379.B7103A.6CF799.6902F4 :is now your displayed host
void IrcKernel::raw_396()
{
    if (strDataList.size() < 4) return;

    QString strHost = strDataList.at(3);

    QString strMessage = QString(tr("* %1 is now your displayed host")).arg(strHost);

    Message::instance()->showMessageActive(strMessage, MessageInfo);
}

// :cf1f4.onet 401 ~Merovingian ~Merovingian1 :No such nick
// :cf1f3.onet 401 Merovingian #asdasdasd :No such channel
// :cf1f3.onet 401 scc_test scc :No such nick/channel
// :cf1f4.onet 401 Merovingian ChanServ :is currently unavailable. Please try again later.
void IrcKernel::raw_401()
{
    if (strDataList.size() < 4) return;

    QString strNickChannel = strDataList.at(3);

    QString strMessage;
    for (int i = 4; i < strDataList.size(); ++i) { if (i != 4) strMessage += " "; strMessage += strDataList.at(i); }
    if ((!strMessage.isEmpty()) && (strMessage.at(0) == ':')) strMessage.remove(0,1);

    if (strMessage == "No such nick")
        strMessage = QString(tr("* %1 :No such nick")).arg(strNickChannel);
    else if (strMessage == "No such channel")
        strMessage = QString(tr("* %1 :No such channel")).arg(strNickChannel);
    else if (strMessage == "No such nick/channel")
        strMessage = QString(tr("* %1 :No such nick/channel")).arg(strNickChannel);
    else if (strMessage == "is currently unavailable. Please try again later.")
        strMessage = QString(tr("* %1 is currently unavailable. Please try again later.")).arg(strNickChannel);

    // display
    Message::instance()->showMessageActive(strMessage, MessageInfo);

    // close inactive priv
    if (strNickChannel.at(0) == '^')
    {
        if (Channel::instance()->contains(strNickChannel))
            pTabC->removeTab(strNickChannel);
    }
}

// :cf1f4.onet 402 Merovingian a :No such server
void IrcKernel::raw_402()
{
    if (strDataList.size() < 4) return;

    QString strServer = strDataList.at(3);

    QString strMessage = QString(tr("* %1 :No such server")).arg(strServer);

    Message::instance()->showMessageActive(strMessage, MessageInfo);
}

// :cf1f1.onet 403 ~testa #^cf1f41568 :Invalid channel name
void IrcKernel::raw_403()
{
    if (strDataList.size() < 4) return;

    QString strChannel = strDataList.at(3);

    QString strMessage;
    if ((strChannel.at(0) == '^') && (Channel::instance()->containsAlternativeName(strChannel)))
        strMessage = QString(tr("* Invalid priv with %1").arg(Channel::instance()->getAlternativeName(strChannel)));
    else
        strMessage = QString(tr("* %1 :Invalid channel name")).arg(strChannel);

    // display
    Message::instance()->showMessageActive(strMessage, MessageInfo);

    // close inactive priv
    if (strChannel.at(0) == '^')
    {
        if (Channel::instance()->contains(strChannel))
            pTabC->removeTab(strChannel);
    }
}

// :cf1f1.onet 404 scc_test #Quiz :Cannot send to channel (+m)
// :cf1f4.onet 404 ~scc_test #lunar :Cannot send to channel (no external messages)
void IrcKernel::raw_404()
{
    if (strDataList.size() < 4) return;

    QString strChannel = strDataList.at(3);

    QString strReason;
    for (int i = 8; i < strDataList.size(); ++i) { if (i != 8) strReason += " "; strReason += strDataList.at(i); }
    if ((!strReason.isEmpty()) && (strReason.at(0) == ':')) strReason.remove(0,1);

    if (strReason == "(+m)")
        strReason = tr("(No moderator on the channel)");

    QString strMessage = QString(tr("* Unable to send a message to %1 %2")).arg(strChannel, strReason);

    Message::instance()->showMessageActive(strMessage, MessageInfo);
}

// :cf1f2.onet 405 Merovingian #Warszawa :You are on too many channels
void IrcKernel::raw_405()
{
    if (strDataList.size() < 4) return;

    QString strMessage = QString(tr("* You are on too many channels"));

    Message::instance()->showMessageActive(strMessage, MessageInfo);
}

// WHOWAS a
//:cf1f2.onet 406 Merovingian a :There was no such nickname
void IrcKernel::raw_406()
{
    if (strDataList.size() < 4) return;

    QString strNick = strDataList.at(3);

    QString strMessage = QString(tr("* %1 :There was no such nickname")).arg(strNick);

    Message::instance()->showMessageActive(strMessage, MessageInfo);
}

// :cf1f3.onet 412 scc_test :No text to send
void IrcKernel::raw_412()
{
    QString strMessage = QString(tr("* No text to send"));
    Message::instance()->showMessageActive(strMessage, MessageInfo);
}

// :cf1f2.onet 421 ~test VERSION :This command has been disabled.
// :cf1f4.onet 421 scc_test MOD :Unknown command
void IrcKernel::raw_421()
{
    if (strDataList.size() < 4) return;

    QString strCmd = strDataList.at(3);

    QString strReason;
    for (int i = 4; i < strDataList.size(); ++i) { if (i != 4) strReason += " "; strReason += strDataList.at(i); }
    if ((!strReason.isEmpty()) && (strReason.at(0) == ':')) strReason.remove(0,1);

    if (strReason == "Unknown command")
        strReason = tr("Unknown command");
    else if (strReason == "This command has been disabled.")
        strReason = tr("This command has been disabled.");

    QString strMessage = QString("* %1 :%2").arg(strCmd, strReason);

    Message::instance()->showMessageActive(strMessage, MessageInfo);
}

// :cf1f4.onet 432 1501-unknown ~?o? :Erroneous Nickname
void IrcKernel::raw_432()
{
    if (strDataList.size() < 4) return;

    QString strNick = strDataList.at(3);

    QString strMessage = QString(tr("* %1 :Erroneous Nickname")).arg(strNick);

    Message::instance()->showMessageAll(strMessage, MessageError);
}

// :cf1f1.onet 433 * scc_test :Nickname is already in use.
void IrcKernel::raw_433()
{
    if (strDataList.size() < 4) return;

    QString strNick = strDataList.at(3);

    // disconnect
    Settings::instance()->setBool("reconnect", false);
    Core::instance()->network->disconnect();

    if (strNick.at(0) != '~')
    {
        QString strMessage =
            QString("%1\r\n%2").arg(
                    tr("Nickname %1 is already in use.").arg(strNick),
                    tr("Do you want to take over session?")
            );

        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setWindowIcon(QIcon(":/images/logo16x16.png"));
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox.setWindowTitle(tr("Warning"));
        msgBox.setText(strMessage);
        int iSelectedButton = msgBox.exec();

        if (iSelectedButton == QMessageBox::Yes)
        {
            // override
            Settings::instance()->setBool("override", true);
            Settings::instance()->setBool("reconnect", true);

            Core::instance()->network->connect();
        }
    }
    else
    {
        QString strMessage = QString(tr("* Nickname %1 is already in use.")).arg(strNick);
        Message::instance()->showMessageAll(strMessage, MessageError);
    }
}

// KICK #kusicielki ~prawdziwa
// :cf1f2.onet 441 Merovingian ~prawdziwa #kusicielki :They are not on that channel
void IrcKernel::raw_441()
{
    if (strDataList.size() < 5) return;

    QString strNick = strDataList.at(3);
    QString strChannel = strDataList.at(4);

    QString strMessage = QString(tr("* %1 is not on %2 channel")).arg(strNick, strChannel);

    Message::instance()->showMessageActive(strMessage, MessageInfo);
}

// KICK #rozrywka ~test
// :cf1f3.onet 442 Merovingian #Rozrywka :You're not on that channel!
void IrcKernel::raw_442()
{
    if (strDataList.size() < 4) return;

    QString strChannel = strDataList.at(3);

    QString strMessage = QString(tr("* You're not on %1 channel!")).arg(strChannel);

    Message::instance()->showMessageActive(strMessage, MessageInfo);
}

// :cf1f4.onet 443 Merovingian scc #Scrabble :is already on channel
void IrcKernel::raw_443()
{
    if (strDataList.size() < 5) return;

    QString strNick = strDataList.at(3);
    QString strChannel = strDataList.at(4);

    QString strMessage = QString(tr("* %1 is already on channel %2")).arg(strNick, strChannel);

    Message::instance()->showMessageActive(strMessage, MessageInfo);
}

// SUMMON
// :cf1f3.onet 445 ~test :SUMMON has been disabled (depreciated command)
void IrcKernel::raw_445()
{
    if (strDataList.size() < 4) return;

    QString strMessage;
    for (int i = 3; i < strDataList.size(); ++i) { if (i != 3) strMessage += " "; strMessage += strDataList.at(i); }
    if ((!strMessage.isEmpty()) && (strMessage.at(0) == ':')) strMessage.remove(0,1);
    strMessage = "* "+strMessage;

    Message::instance()->showMessageActive(strMessage, MessageInfo);
}

// USERS
// :cf1f2.onet 446 Merovingian :USERS has been disabled (depreciated command)
void IrcKernel::raw_446()
{
    if (strDataList.size() < 4) return;

    QString strMessage;
    for (int i = 3; i < strDataList.size(); ++i) { if (i != 3) strMessage += " "; strMessage += strDataList.at(i); }
    if ((!strMessage.isEmpty()) && (strMessage.at(0) == ':')) strMessage.remove(0,1);
    strMessage = "* "+strMessage;

    Message::instance()->showMessageActive(strMessage, MessageInfo);
}

// :cf1f3.onet 451 SLIST :You have not registered
void IrcKernel::raw_451()
{
    if (strDataList.size() < 3) return;

    QString strCommand = strDataList.at(2);

    QString strMessage = QString(tr("* You have not registered to perform operation %1")).arg(strCommand);
    Message::instance()->showMessageActive(strMessage, MessageInfo);
}

// :cf1f2.onet 461 ~test OPER :Not enough parameters.
void IrcKernel::raw_461()
{
    if (strDataList.size() < 4) return;

    QString strCmd = strDataList.at(3);

    QString strMessage = QString(tr("* %1 :Not enough parameters")).arg(strCmd);

    Message::instance()->showMessageActive(strMessage, MessageInfo);
}

// PASS
// :cf1f2.onet 462 Merovingian :You may not reregister
void IrcKernel::raw_462()
{
    QString strMessage = QString(tr("* You may not reregister"));
    Message::instance()->showMessageActive(strMessage, MessageInfo);
}

// :cf1f3.onet 470 ~Merovingian :#testy has become full, so you are automatically being transferred to the linked channel #Awaria
void IrcKernel::raw_470()
{
    if (strDataList.size() < 4) return;

    QString strChannel = strDataList.at(3);
    if (strChannel.at(0) == ':') strChannel.remove(0,1);

    QString strLinked = strDataList.at(strDataList.size()-1);

    QString strMessage = QString(tr("* %1 has become full, so you are automatically being transferred to the linked channel %2")).arg(strChannel, strLinked);

    Message::instance()->showMessage(STATUS_WINDOW, strMessage, MessageInfo);
}

// :cf1f2.onet 471 ~Merovingian #testy :Cannot join channel (Channel is full)
void IrcKernel::raw_471()
{
    if (strDataList.size() < 4) return;

    QString strChannel = strDataList.at(3);

    QString strMessage = QString(tr("* Cannot join channel %1 - channel is full")).arg(strChannel);
    Message::instance()->showMessageActive(strMessage, MessageInfo);
}

// :cf1f3.onet 473 ~scc_test #lunar :Cannot join channel (Invite only)
void IrcKernel::raw_473()
{
    if (strDataList.size() < 4) return;

    QString strChannel = strDataList.at(3);

    QString strMessage = QString(tr("* Cannot join channel %1 - Invite only")).arg(strChannel);
    Message::instance()->showMessageActive(strMessage, MessageInfo);

    // close inactive priv
    if (strChannel.at(0) == '^')
    {
        if (Channel::instance()->contains(strChannel))
            pTabC->removeTab(strChannel);
    }
}

// :cf1f3.onet 474 ~scc_test #Quiz :Cannot join channel (You're banned)
void IrcKernel::raw_474()
{
    if (strDataList.size() < 4) return;

    QString strChannel = strDataList.at(3);

    QString strMessage = QString(tr("* Cannot join channel %1 - You're banned")).arg(strChannel);
    Message::instance()->showMessageActive(strMessage, MessageInfo);
}

// :cf1f4.onet 475 Merovingian #glupia_nazwa :Cannot join channel (Incorrect channel key)
void IrcKernel::raw_475()
{
    if (strDataList.size() < 4) return;

    QString strChannel = strDataList.at(3);

    QString strMessage = QString(tr("* Cannot join channel %1 - Incorrect channel key")).arg(strChannel);
    Message::instance()->showMessageActive(strMessage, MessageInfo);

    (new ChannelKeyGui(strChannel))->show(); // should be show - prevent hangup!
}

// :cf1f3.onet 480 scc_test :Can't KNOCK on #Scrabble, channel is not invite only so knocking is pointless!
void IrcKernel::raw_480()
{
    if (strDataList.size() < 3) return;

    QString strMessage;
    for (int i = 3; i < strDataList.size(); ++i) { if (i != 3) strMessage += " "; strMessage += strDataList.at(i); }
    if ((!strMessage.isEmpty()) && (strMessage.at(0) == ':')) strMessage.remove(0,1);
    strMessage = "* "+strMessage;

    Message::instance()->showMessageActive(strMessage, MessageInfo);
}

// :cf1f4.onet 481 Merovingian :Permission Denied - You do not have the required operator privileges
void IrcKernel::raw_481()
{
    QString strMessage = QString(tr("* Permission Denied - You do not have the required operator privileges"));
    Message::instance()->showMessageActive(strMessage, MessageInfo);
}

// :cf1f1.onet 482 Merovingian #Scrabble :Only a u-line may kick a u-line from a channel.
void IrcKernel::raw_482()
{
    if (strDataList.size() < 4) return;

    QString strChannel = strDataList.at(3);

    QString strMessage = QString(tr("* Only a u-line may kick a u-line from a channel."));

    Message::instance()->showMessage(strChannel, strMessage, MessageInfo);
}

// :cf1f4.onet 484 Merovingian #scc :Can't kick Merovingian as they're a channel founder
// :cf1f4.onet 484 Merovingian #testy :Can't kick user advocato000 from channel (+Q set)
// :cf1f4.onet 484 scc_test #scc :Can't kick scc as your spells are not good enough
void IrcKernel::raw_484()
{
    if (strDataList.size() < 6) return;

    QString strChannel = strDataList.at(3);

    QString strMessage;
    for (int i = 4; i < strDataList.size(); ++i) { if (i != 4) strMessage += " "; strMessage += strDataList.at(i); }
    if ((!strMessage.isEmpty()) && (strMessage.at(0) == ':')) strMessage.remove(0,1);
    strMessage = "* "+strMessage;

    Message::instance()->showMessage(strChannel, strMessage, MessageInfo);
}

// :cf1f3.onet 491 ~test :Invalid oper credentials
void IrcKernel::raw_491()
{
    QString strMessage = QString(tr("* Invalid oper credentials"));

    Message::instance()->showMessageActive(strMessage, MessageInfo);
}

// :cf1f3.onet 492 Merovingian ^cf1f3954674 :Can't invite bezduszna_istota to channel (+V set)
void IrcKernel::raw_492()
{
    if (strDataList.size() < 10) return;

    QString strChannel = strDataList.at(3);

    QString strNick = strDataList.at(6);

    QString strReason;
    for (int i = 9; i < strDataList.size(); ++i) { if (i != 9) strReason += " "; strReason += strDataList.at(i); }

    if (strReason == "(+V set)")
        strReason = tr("(+V set)");

    QString strMessage = QString(tr("* Can't invite %1 to channel %2").arg(strNick, strReason));

    Message::instance()->showMessage(strChannel, strMessage, MessageInfo);
}

// :cf1f1.onet 495 Merovingian #scc :You cannot rejoin this channel yet after being kicked (+J)
void IrcKernel::raw_495()
{
    if (strDataList.size() < 4) return;

    QString strChannel = strDataList.at(3);

    QString strMessage = QString(tr("* %1 :You cannot rejoin this channel yet after being kicked")).arg(strChannel);

    Message::instance()->showMessageActive(strMessage, MessageInfo);
}

//:cf1f2.onet 530 Merovingian #f :Only IRC operators may create new channels
void IrcKernel::raw_530()
{
    if (strDataList.size() < 4) return;

    QString strChannel = strDataList.at(3);

    QString strMessage = QString(tr("* %1 :Only IRC operators may create new channels")).arg(strChannel);
    Message::instance()->showMessageActive(strMessage, MessageInfo);
}

// PRIVMSG a
// :cf1f2.onet 531 Merovingian chanky :You are not permitted to send private messages to this user
void IrcKernel::raw_531()
{
    if (strDataList.size() < 4) return;

    QString strNick = strDataList.at(3);

    QString strMessage = QString(tr("* %1 :You are not permitted to send private messages to this user")).arg(strNick);
    Message::instance()->showMessageActive(strMessage, MessageInfo);
}

// :cf1f4.onet 600 scc_test Radowsky 16172032 690A6F.A8219B.7F5EC1.35E57C 1267055769 :arrived online
void IrcKernel::raw_600()
{
    if (strDataList.size() < 4) return;

    QString strNick = strDataList.at(3);

    QString strMessage = QString(tr("* Your friend %1 arrived online")).arg(strNick);
    Message::instance()->showMessageActive(strMessage, MessageInfo);

    Friends::instance()->set(strNick, true);
}

// :cf1f4.onet 601 scc_test Radowsky 16172032 690A6F.A8219B.7F5EC1.35E57C 1267055692 :went offline
void IrcKernel::raw_601()
{
    if (strDataList.size() < 4) return;

    QString strNick = strDataList.at(3);

    QString strMessage = QString(tr("* Your friend %1 went offline")).arg(strNick);
    Message::instance()->showMessageActive(strMessage, MessageInfo);

    Friends::instance()->set(strNick, false);
}

// NS FRIENDS DEL nick
// :cf1f3.onet 602 scc_test aaa * * 0 :stopped watching
void IrcKernel::raw_602()
{
    if (strDataList.size() < 4) return;

    QString strNick = strDataList.at(3);

    Friends::instance()->remove(strNick);
}

//:cf1f1.onet 604 scc_test scc_test 51976824 3DE379.B7103A.6CF799.6902F4 1267054441 :is online
void IrcKernel::raw_604()
{
    if (strDataList.size() < 4) return;

    QString strNick = strDataList.at(3);

//    hidden
//    QString strMessage = QString(tr("* Your friend %1 is now on-line")).arg(strNick);
//    Message::instance()->showMessageActive(strMessage, InfoMessage);

    Friends::instance()->set(strNick, true);
}

// :cf1f1.onet 605 scc_test Radowsky * * 0 :is offline
void IrcKernel::raw_605()
{
    if (strDataList.size() < 4) return;

    QString strNick = strDataList.at(3);

//    hidden
//    QString strMessage = QString(tr("* Your friend %1 is now off-line")).arg(strNick);
//    Message::instance()->showMessageActive(strMessage, InfoMessage);

    Friends::instance()->set(strNick, false);
}

// WATCH
// :cf1f2.onet 607 Merovingian :End of WATCH list
void IrcKernel::raw_607()
{
}

// SERVER
// :cf1f1.onet 666 ~test :You cannot identify as a server, you are a USER. IRC Operators informed.
void IrcKernel::raw_666()
{
    QString strMessage = QString(tr("* You cannot identify as a server, you are a USER. IRC Operators informed."));

    Message::instance()->showMessageActive(strMessage, MessageInfo);
}

// :legowisko.pirc.pl 671 nick_tymczasowy293 pid :is using a Secure Connection
void IrcKernel::raw_671()
{
    if (strDataList.size() < 4) return;

    QString strNick = strDataList.at(3);

    QString strMessage;
    for (int i = 4; i < strDataList.size(); ++i) { if (i != 4) strMessage += " "; strMessage += strDataList.at(i); }
    if ((!strMessage.isEmpty()) && (strMessage.at(0) == ':')) strMessage.remove(0,1);

    QString strDisplay;
    if (strMessage == "is using a Secure Connection")
        strDisplay = QString(tr("* %1 is using a Secure Connection")).arg(strNick);
    else
        strDisplay = QString("* %1 %2").arg(strNick, strMessage);

    Message::instance()->showMessageActive(strDisplay, MessageInfo);
}

// COMMANDS
// :cf1f2.onet 702 Merovingian :ZLINE <core> 1
// MODULES
// :cf1f2.onet 702 Merovingian :m_onetauditorium.so
void IrcKernel::raw_702()
{
    if (strDataList.size() < 4) return;

    QString strMessage;
    for (int i = 3; i < strDataList.size(); ++i) { if (i != 3) strMessage += " "; strMessage += strDataList.at(i); }
    if ((!strMessage.isEmpty()) && (strMessage.at(0) == ':')) strMessage.remove(0,1);
    strMessage = "* "+strMessage;

    Message::instance()->showMessageActive(strMessage, MessageInfo);
}

// MODULES
// :cf1f2.onet 703 Merovingian :End of MODULES list
void IrcKernel::raw_703()
{
}

// COMMANDS
// :cf1f2.onet 704 Merovingian :End of COMMANDS list
void IrcKernel::raw_704()
{
}

// BUSY 1
// :cf1f2.onet 807 scc_test :You are marked as busy
void IrcKernel::raw_807()
{
    Busy::instance()->start();

    QString strDisplay = tr("* You are marked as busy");
    Message::instance()->showMessageActive(strDisplay, MessageInfo);
}

// BUSY 0
// :cf1f2.onet 808 scc_test :You are no longer marked busy
void IrcKernel::raw_808()
{
    Busy::instance()->stop();

    QString strDisplay = tr("* You are no longer marked busy");
    Message::instance()->showMessageActive(strDisplay, MessageInfo);
}

// WHOIS
// :cf1f2.onet 809 scc_test Succubi :is busy
void IrcKernel::raw_809()
{
    if (strDataList.size() < 4) return;

    QString strNick = strDataList.at(3);

    QString strMessage = QString(tr("* %1 is busy")).arg(strNick);
    Message::instance()->showMessageActive(strMessage, MessageInfo);
}

// :cf1f2.onet 811 scc_test Merovingian :Ignore invites
void IrcKernel::raw_811()
{
    if (strDataList.size() < 4) return;

    QString strNick = strDataList.at(3);

    QString strMessage;

    if (strNick.at(0) == '^')
        strMessage = QString(tr("* Ignored priv from %1")).arg(strNick);
    else
        strMessage = QString(tr("* Ignored invite from %1")).arg(strNick);

    Message::instance()->showMessageActive(strMessage, MessageInfo);
}

// :cf1f2.onet 812 scc_test Merovingian ^cf1f41284615 :Invite rejected
// :cf1f3.onet 812 Merovingian Vasquez_ #Kraina_Lagodności :Invite rejected
void IrcKernel::raw_812()
{
    if (strDataList.size() < 5) return;

    QString strNick = strDataList.at(3);
    QString strChannel = strDataList.at(4);

    QString strMessage;

    if (strChannel.at(0) == '^')
        strMessage = QString(tr("* Rejected priv from %1")).arg(strNick);
    else
        strMessage = QString(tr("* Rejected invite to %1 from %2")).arg(strChannel, strNick);

    Message::instance()->showMessageActive(strMessage, MessageInfo);
}

// WHOIS
// :cf1f4.onet 815 ~test testnick :Public webcam
void IrcKernel::raw_815()
{
    if (strDataList.size() < 4) return;

    QString strNick = strDataList.at(3);

    QString strDisplay = QString(tr("* %1 has public webcam")).arg(strNick);
    Message::instance()->showMessageActive(strDisplay, MessageInfo);
}

// WHOIS
// :cf1f4.onet 816 ~test testnick :Private webcam
void IrcKernel::raw_816()
{
    if (strDataList.size() < 4) return;

    QString strNick = strDataList.at(3);

    QString strDisplay = QString(tr("* %1 has private webcam")).arg(strNick);
    Message::instance()->showMessageActive(strDisplay, MessageInfo);
}

// :cf1f2.onet 817 scc_test #scc 1253216797 mikefear - :%Fb:arial%%Ce40f0f%re
void IrcKernel::raw_817()
{
    if (strDataList.size() < 6) return;

    QString strChannel = strDataList.at(3);
    qint64 iTime = QDateTime::fromTime_t(strDataList.at(4).toInt()).toMSecsSinceEpoch();
    QString strNick = strDataList.at(5);

    QString strMessage;
    for (int i = 7; i < strDataList.size(); ++i) { if (i != 7) strMessage += " "; strMessage += strDataList.at(i); }
    if ((!strMessage.isEmpty()) && (strMessage.at(0) == ':')) strMessage.remove(0,1);

    if (strMessage.isEmpty())
        return;

    // convert emots //
    Convert::simpleReverseConvert(strMessage);
    // convert emots :)
    Replace::replaceEmots(strMessage);

    Message::instance()->showMessage(strChannel, strMessage, MessageDefault, strNick, iTime);
}

// SLIST
// :cf1f3.onet 818 scc_test :Start of simple channels list.
void IrcKernel::raw_818()
{
}

// SLIST
// :cf1f3.onet 819 scc_test :#tarnów_dziki:g:1,#Żory:g:0,#Mława:O:0,#Lineage_II:_:1,#kakakak:O:0,#apostolat_yfl:_:0,#ITALIA_CLUB:i:23,#Finał_WOŚP:P:0,#sama_słodycz:O:0,#Suwałki:i:14,#Mamuśki:O:0,#Pokój_Radości:O:0,#Antwerpia:g:0,#Kolo_Gospodyn_Wiejskich:O:0,#Samotnia_Kurka:G:0,#Wszystko_o_grach:O:0,#VIPy_NowySącz:h:0,#tymczasowy:G:0,#Zielona_Góra:h:2,#45slonko:P:0,#kawalek_nieba:O:0,#Wirtualna_Przyjazn:a:11,#Magiczny_swiat:O:1,#herbatka_u_cynamonki:P:0,#DEUTSCHLAND:i:111,#informatyka:`:1
void IrcKernel::raw_819()
{
    if (strDataList.size() < 4) return;

    QString strChannelsString;
    for (int i = 3; i < strDataList.size(); ++i) { if (i != 3) strChannelsString += " "; strChannelsString += strDataList.at(i); }
    if ((!strChannelsString.isEmpty()) && (strChannelsString.at(0) == ':')) strChannelsString.remove(0,1);

    if (ChannelList::instance()->getStatus() == StatusCompleted)
        return;

    QStringList strChannelsList = strChannelsString.split(",");
    for (int i = 0; i < strChannelsList.size(); ++i)
    {
        QStringList strChannelParameters = strChannelsList.at(i).split(":");
        if (strChannelParameters.size() == 3)
        {
            QString strChannelName = strChannelParameters.at(0);
            int iChannelPeople = QString(strChannelParameters.at(2)).toInt();
            int iChannelCat = 0;
            int iChannelType = 0;
            bool bChannelModerated = false;
            bool bChannelRecommended = false;

            bool flag = false;
            bool flag1 = false;
            //bool flag2 = false;
            //bool flag3 = false;
            //bool flag4 = false;

            QString s1 = strChannelParameters.at(1);
            int c;
            if (s1.length() > 1)
            {
                c = s1.at(0).toLatin1();
                flag = (c & 0x6d) == 109;
                flag1 = (c & 0x70) == 112;
                c = s1.at(1).toLatin1();
            }
            else
                c = s1.at(0).toLatin1();
            c++;
            int k = c & 7;
            int l = (c & 0x38) >> 3;
            //flag4 = l == 3;

            iChannelType = l;
            iChannelCat = k;

            if (flag)
                bChannelModerated = true;
            if (flag1)
                bChannelRecommended = true;

            OnetChannelList oChannelList;
            oChannelList.name = strChannelName;
            oChannelList.people = iChannelPeople;
            oChannelList.cat = iChannelCat;
            oChannelList.type = iChannelType;
            oChannelList.moderated = bChannelModerated;
            oChannelList.recommended = bChannelRecommended;

            ChannelList::instance()->add(oChannelList);
        }
    }
}

// SLIST
// :cf1f3.onet 820 scc_test :End of simple channel list.
void IrcKernel::raw_820()
{
    ChannelList::instance()->setStatus(StatusCompleted);
    ChannelList::instance()->setTime(QDateTime::currentMSecsSinceEpoch());
}

// :cf1f3.onet 821 scc_test #scc :Channel is not moderated
void IrcKernel::raw_821()
{
    if (strDataList.size() < 4) return;

    QString strChannel = strDataList.at(3);

    QString strMessage = QString(tr("* Channel %1 is not moderated")).arg(strChannel);
    Message::instance()->showMessageActive(strMessage, MessageInfo);
}

// :cf1f4.onet 942 Merovingian nick_porzucony.64131723 :Invalid nickname
void IrcKernel::raw_942()
{
    if (strDataList.size() < 4) return;

    QString strNick = strDataList.at(3);

    QString strMessage = QString(tr("* %1 :Invalid nickname")).arg(strNick);

    Message::instance()->showMessageActive(strMessage, MessageInfo);
}

// NS IGNORE DEL nick
// :cf1f2.onet 950 Merovingian Merovingian :Removed Succubi!*@* <privatemessages,channelmessages,invites> from silence list
void IrcKernel::raw_950()
{
    if (strDataList.size() < 6) return;

    QString strNick = strDataList.at(5);

    Q_UNUSED (strNick);
//    hidden
//    QString strDisplay = QString(tr("* Removed %1 from silence list")).arg(strNick);
//    Message::instance()->showMessageActive(strDisplay, InfoMessage);
}

// NS IGNORE ADD nick
// :cf1f1.onet 951 scc_test scc_test :Added test!*@* <privatemessages,channelmessages,invites> to silence list
void IrcKernel::raw_951()
{
    if (strDataList.size() < 6) return;

    QString strNick = strDataList.at(5);

    Q_UNUSED (strNick);
//    hidden
//    QString strMessage = QString(tr("* Added %1 to silence list")).arg(strNick);
//    Message::instance()->showMessageActive(strMessage, InfoMessage);
}

// :cf1f4.onet 952 Merovingian Merovingian :Succubi!*@* <privatemessages,channelmessages,invites> is already on your silence list
void IrcKernel::raw_952()
{
    if (strDataList.size() < 5) return;

    QString strNick = strDataList.at(4);
    if (strNick.at(0) == ':') strNick.remove(0,1);

    QString strMessage = QString(tr("* %1 is already on your silence list")).arg(strNick);
    Message::instance()->showMessageActive(strMessage, MessageInfo);
}

// :cf1f4.onet 974 Darom m_cloaking.so :Failed to load module: Unknown module
void IrcKernel::raw_974()
{
    if (strDataList.size() < 5) return;

    QString strModule = strDataList.at(3);

    QString strInfo;
    for (int i = 4; i < strDataList.size(); ++i) { if (i != 4) strInfo += " "; strInfo += strDataList.at(i); }
    if ((!strInfo.isEmpty()) && (strInfo.at(0) == ':')) strInfo.remove(0,1);

    QString strMessage = QString("* %1 %2").arg(strModule, strInfo);
    Message::instance()->showMessageActive(strMessage, MessageInfo);
}

// :cf1f4.onet 975 Darom m_cloaking.so :Module successfully loaded.
void IrcKernel::raw_975()
{
    if (strDataList.size() < 5) return;

    QString strModule = strDataList.at(3);

    QString strInfo;
    for (int i = 4; i < strDataList.size(); ++i) { if (i != 4) strInfo += " "; strInfo += strDataList.at(i); }
    if ((!strInfo.isEmpty()) && (strInfo.at(0) == ':')) strInfo.remove(0,1);

    QString strMessage = QString("* %1 %2").arg(strModule, strInfo);
    Message::instance()->showMessageActive(strMessage, MessageInfo);
}
