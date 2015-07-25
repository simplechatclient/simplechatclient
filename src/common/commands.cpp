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

#include <QIcon>
#include <QMessageBox>
#include <QPushButton>
#include "models/channel.h"
#include "convert.h"
#include "models/message.h"
#include "replace.h"
#include "models/settings.h"
#include "models/utils.h"
#include "commands.h"

#ifdef Q_OS_WIN
    #include "winamp.h"
#else
    #include "mpris_player.h"
#endif

Commands::Commands(const QString &_strChan, const QString &_strData) : strChan(_strChan), strData(_strData)
{
    if (!strData.isEmpty())
        strDataList = strData.split(" ", QString::SkipEmptyParts);
}

QString Commands::execute()
{
    if (strDataList.size() == 0)
        return QString::null;

    QString strCmd = strDataList.at(0).toLower();

    if ((strCmd == "amsg") || (strCmd == "all"))
        return cmdAmsg();
    else if (strCmd == "away")
        return cmdAway();
    else if (strCmd == "busy")
        return cmdBusy();
    else if (strCmd == "clear")
        return cmdClear();
    else if (strCmd == "friend")
        return cmdFriend();
    else if (strCmd == "help" || (Settings::instance()->get("language") == "pl" && strCmd == "pomoc"))
        return cmdHelp();
    else if (strCmd == "ignore")
        return cmdIgnore();
    else if ((strCmd == "join") || (strCmd == "j"))
        return cmdJoin();
    else if ((strCmd == "logout") || (strCmd == "quit") || (strCmd == "q"))
        return cmdQuit();
    else if ((strCmd == "mp3") || (strCmd == "winamp"))
        return cmdMp3();
    else if (strCmd == "offmsg")
        return cmdOffmsg();
    else if ((strCmd == "priv") || (strCmd == "query"))
        return cmdPriv();
    else if ((strCmd == "raw") || (strCmd == "quote"))
        return cmdRaw();
    else if ((strCmd == "whereis") || (strCmd == "whois"))
        return cmdWhereis();

    if ((strChan != DEBUG_WINDOW) && (strChan != STATUS_WINDOW))
    {
        if (strCmd == "ban")
            return cmdBan();
        else if (strCmd == "banip")
            return cmdBanip();
        else if ((strCmd == "cycle") || (strCmd == "hop"))
            return cmdCycle();
        else if (strCmd == "invite")
            return cmdInvite();
        else if (strCmd == "kban")
            return cmdKBan();
        else if (strCmd == "kbanip")
            return cmdKBanip();
        else if ((strCmd == "kick") || (strCmd == "k"))
            return cmdKick();
        else if (strCmd == "me")
            return cmdMe();
        else if ((strCmd == "moder") || (strCmd == "moderator"))
            return cmdModer();
        else if (strCmd == "op")
            return cmdOp();
        else if ((strCmd == "part") || (strCmd == "p"))
            return cmdPart();
        else if (strCmd == "sop")
            return cmdSop();
        else if (strCmd == "topic")
            return cmdTopic();
        else if (strCmd == "vip")
            return cmdVip();
    }

    return strData;
}

QList<QString> Commands::getList()
{
    QList<QString> lCommands;
    lCommands << "/all"
              << "/amsg"
              << "/away"
              << "/ban"
              << "/banip"
              << "/busy"
              << "/clear"
              << "/cycle"
              << "/friend"
              << "/help"
              << "/hop"
              << "/ignore"
              << "/invite"
              << "/join"
              << "/kban"
              << "/kbanip"
              << "/kick"
              << "/logout"
              << "/me"
              << "/moder"
              << "/moderator"
              << "/mp3"
              << "/offmsg"
              << "/op"
              << "/part"
              << "/priv"
              << "/query"
              << "/quit"
              << "/quote"
              << "/raw"
              << "/sop"
              << "/topic"
              << "/vip"
              << "/whereis"
              << "/whois"
              << "/winamp";

    if (Settings::instance()->get("language") == "pl")
        lCommands << "/pomoc";

    return lCommands;
}

QString Commands::cmdAmsg()
{
    if (strDataList.value(1).isEmpty()) return QString::null;

    QString strMessage;
    for (int i = 1; i < strDataList.size(); ++i) { if (i != 1) strMessage += " "; strMessage += strDataList.at(i); }

    return strMessage;
}

QString Commands::cmdAway()
{
    QString strMessage;
    for (int i = 1; i < strDataList.size(); ++i) { if (i != 1) strMessage += " "; strMessage += strDataList.at(i); }

    Convert::simpleReverseConvert(strMessage);
    Replace::replaceEmots(strMessage);

    return QString("AWAY :%1").arg(strMessage);
}

QString Commands::cmdBan()
{
    if (strDataList.size() < 2) return QString::null;

    QString strChannel = strChan;

    QString strNick = strDataList.at(1);
    QChar cPlusMinus =  '+';

    if ((strNick.at(0) == '+') || (strNick.at(0) == '-'))
    {
        cPlusMinus = strNick.at(0);
        strNick.remove(0,1);
    }

    if ((strNick.contains("*")) && (cPlusMinus == '+'))
    {
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setWindowIcon(QIcon(":/images/logo16x16.png"));
        msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
        msgBox.setWindowTitle(tr("Warning"));
        msgBox.setText(tr("This ban will ban large numbers of people. Are you sure you want to use this ban?"));
        int iResult = msgBox.exec();

        if (iResult == QMessageBox::Ok)
            return QString("CS BAN %1 ADD %2").arg(strChannel, strNick);
        else
            return QString::null;
    }

    if (cPlusMinus == '+')
        return QString("CS BAN %1 ADD %2").arg(strChannel, strNick);
    else if (cPlusMinus == '-')
        return QString("CS BAN %1 DEL %2").arg(strChannel, strNick);
    else
        return QString::null;
}

QString Commands::cmdBanip()
{
    if (strDataList.size() < 2) return QString::null;

    QString strChannel = strChan;

    QString strNick = strDataList.at(1);
    QChar cPlusMinus =  '+';

    if ((strNick.at(0) == '+') || (strNick.at(0) == '-'))
    {
        cPlusMinus = strNick.at(0);
        strNick.remove(0,1);
    }

    if (cPlusMinus == '+')
        return QString("CS BANIP %1 ADD %2").arg(strChannel, strNick);
    else if (cPlusMinus == '-')
        return QString("CS BANIP %1 DEL %2").arg(strChannel, strNick);
    else
        return QString::null;
}

QString Commands::cmdBusy()
{
    bool bBusy = Settings::instance()->getBool("busy");

    if (bBusy)
        return "BUSY 0";
    else
        return "BUSY 1";
}

QString Commands::cmdClear()
{
    QString strChannel = strChan;

    Channel::instance()->getChatView(strChannel)->clearMessages();

    return QString::null;
}

QString Commands::cmdCycle()
{
    QString strChannel = strChan;

    QString strMessage;
    for (int i = 1; i < strDataList.size(); ++i) { if (i != 1) strMessage += " "; strMessage += strDataList.at(i); }

    if (!strMessage.isEmpty())
        return QString("PART %1 :%2\nJOIN %1").arg(strChannel, strMessage);
    else
        return QString("PART %1\nJOIN %1").arg(strChannel);
}

QString Commands::cmdFriend()
{
    if (strDataList.size() < 2) return QString::null;

    QString strNick = strDataList.at(1);
    QChar cPlusMinus =  '+';

    if ((strNick.at(0) == '+') || (strNick.at(0) == '-'))
    {
        cPlusMinus = strNick.at(0);
        strNick.remove(0,1);
    }

    if (cPlusMinus == '+')
        return QString("NS FRIENDS ADD %1").arg(strNick);
    else if (cPlusMinus == '-')
        return QString("NS FRIENDS DEL %1").arg(strNick);
    else
        return QString::null;
}

QString Commands::cmdHelp()
{
    QStringList lHelp;
    lHelp << tr("* Available commands:")
          << tr("/amsg [text] or /all [text] - send message to all opened channels")
          << tr("/away [text] - mark/unmark as being away")
          << tr("/ban [nick] or /ban -[nick] - ban or unban nick on current channel")
          << tr("/banip [nick] or /banip -[nick] - ban user ip or unban user ip on current channel")
          << tr("/busy - mark/unmark as busy")
          << tr("/clear - clear current screen")
          << tr("/cycle [text] or /hop [text] - part and join current channel")
          << tr("/friend [nick] or /friend -[nick] - add or remove nick from friends list")
          << tr("/help")
          << tr("/ignore [nick] or /ignore -[nick] - add or remove nick from ignore list")
          << tr("/invite [nick] - invite nick to current channel")
          << tr("/join [channel] [key] or /j [channel] [key] - join channel")
          << tr("/kban [nick] [reason] - kick and ban nick on current channel")
          << tr("/kbanip [nick] [reason] - kick and ban user ip on current channel")
          << tr("/kick [nick] [reason] or /k [nick] [reason] - kick user from current channel")
          << tr("/logout [text] or /quit [text] or /q [text] - logout")
          << tr("/me [text] - action command")
          << tr("/moder [nick] or /moder -[nick] or /moderator [nick] or /moderator -[nick] - add or remove mod privilege")
          << tr("/mp3 or /winamp - show current song from winamp/aimp")
          << tr("/offmsg [nick] [text] - send offline message to user")
          << tr("/op [nick] or /op -[nick] - add or remove operator privilege")
          << tr("/part [text] or /p [text] - part current channel")
          << tr("/priv [nick] - open priv")
          << tr("/raw [text] - send command directly to server")
          << tr("/sop [nick] or /sop -[nick] - add or remove super-operator privilege")
          << tr("/topic [text] - change topic on current channel")
          << tr("/vip [nick] or /vip -[nick] - add or remove vip privilege")
          << tr("/whereis [nick] or /whois [nick] - show where is user")
          << tr("/whowas [nick] - returns a list of times the user was last seen on chat");

    foreach (const QString &strDisplay, lHelp)
        Message::instance()->showMessage(strChan, strDisplay, MessageInfo);

    return QString::null;
}

QString Commands::cmdIgnore()
{
    if (strDataList.size() < 2) return QString::null;

    QString strNick = strDataList.at(1);
    QChar cPlusMinus =  '+';

    if ((strNick.at(0) == '+') || (strNick.at(0) == '-'))
    {
        cPlusMinus = strNick.at(0);
        strNick.remove(0,1);
    }

    if (cPlusMinus == '+')
        return QString("NS IGNORE ADD %1").arg(strNick);
    else if (cPlusMinus == '-')
        return QString("NS IGNORE DEL %1").arg(strNick);
    else
        return QString::null;
}

QString Commands::cmdInvite()
{
    if (strDataList.size() < 2) return QString::null;

    QString strNick = strDataList.at(1);
    QString strChannel = strChan;

    return QString("INVITE %1 %2").arg(strNick, strChannel);
}

QString Commands::cmdJoin()
{
    if (strDataList.size() < 2) return QString::null;

    QString strChannel = strDataList.at(1);
    if ((strChannel.at(0) != '#') && (strChannel.at(0) != '^'))
        strChannel = "#"+strChannel;

    QString strKey;
    for (int i = 2; i < strDataList.size(); ++i) { if (i != 2) strKey += " "; strKey += strDataList.at(i); }

    if (Utils::instance()->isErotic(strChannel))
    {
        if (Settings::instance()->getBool("age_check"))
        {
            QMessageBox msgBox;
            msgBox.setStyleSheet("background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:0.557, stop:0 rgba(198, 0, 0, 255), stop:1 rgba(255, 0, 0, 255));");
            msgBox.setIcon(QMessageBox::Warning);
            msgBox.setWindowIcon(QIcon(":/images/logo16x16.png"));
            msgBox.setWindowTitle(tr("Warning"));
            msgBox.setText(QString("%1\n%2").arg(tr("Erotic category may contain content intended only for adults."), tr("To enter you must be over 18 years.")));
            QPushButton *exitButton = msgBox.addButton(tr("Exit"), QMessageBox::AcceptRole);
            exitButton->setIcon(QIcon(":/images/breeze/dialog-cancel.svg"));
            QPushButton *enterButton = msgBox.addButton(tr("Enter"), QMessageBox::RejectRole);
            enterButton->setIcon(QIcon(":/images/breeze/dialog-ok.svg"));
            msgBox.exec();

            if (msgBox.clickedButton() == enterButton)
                Settings::instance()->setBool("age_check", false);
            else
                return QString::null;
        }
    }

    if (!strKey.isEmpty())
        return QString("JOIN %1 %2").arg(strChannel, strKey);
    else
        return QString("JOIN %1").arg(strChannel);
}

QString Commands::cmdKBan()
{
    if (strDataList.size() < 2) return QString::null;

    QString strChannel = strChan;

    QString strNick = strDataList.at(1);

    QString strReason;
    for (int i = 2; i < strDataList.size(); ++i) { if (i != 2) strReason += " "; strReason += strDataList.at(i); }

    if (strReason.isEmpty())
        strReason = tr("No reason");

    return QString("CS BAN %1 ADD %2\nKICK %3 %4 :%5").arg(strChannel, strNick, strChannel, strNick, strReason);
}

QString Commands::cmdKBanip()
{
    if (strDataList.size() < 2) return QString::null;

    QString strChannel = strChan;

    QString strNick = strDataList.at(1);

    QString strReason;
    for (int i = 2; i < strDataList.size(); ++i) { if (i != 2) strReason += " "; strReason += strDataList.at(i); }

    if (strReason.isEmpty())
        strReason = tr("No reason");

    return QString("CS BANIP %1 ADD %2\nKICK %3 %4 :%5").arg(strChannel, strNick, strChannel, strNick, strReason);
}

QString Commands::cmdKick()
{
    if (strDataList.size() < 2) return QString::null;

    QString strChannel = strChan;
    QString strNick = strDataList.at(1);

    QString strReason;
    for (int i = 2; i < strDataList.size(); ++i) { if (i != 2) strReason += " "; strReason += strDataList.at(i); }

    if (strReason.isEmpty())
        strReason = tr("No reason");

    return QString("KICK %1 %2 :%3").arg(strChannel, strNick, strReason);
}

QString Commands::cmdMe()
{
    if (strDataList.size() < 2) return QString::null;

    QString strChannel = strChan;

    QString strMessage;
    for (int i = 1; i < strDataList.size(); ++i) { if (i != 1) strMessage += " "; strMessage += strDataList.at(i); }

    Convert::simpleReverseConvert(strMessage);
    Replace::replaceEmots(strMessage);
    Convert::createText(strMessage);

    return QString("PRIVMSG %1 :%2ACTION %3%4").arg(strChannel, QString(QByteArray("\x01")), strMessage, QString(QByteArray("\x01")));
}

QString Commands::cmdModer()
{
    if (strDataList.size() < 2) return QString::null;

    QString strChannel = strChan;
    QString strNick = strDataList.at(1);
    QChar cPlusMinus =  '+';

    if ((strNick.at(0) == '+') || (strNick.at(0) == '-'))
    {
        cPlusMinus = strNick.at(0);
        strNick.remove(0,1);
    }

    if (cPlusMinus == '+')
        return QString("CS MODERATOR %1 ADD %2").arg(strChannel, strNick);
    else if (cPlusMinus == '-')
        return QString("CS MODERATOR %1 DEL %2").arg(strChannel, strNick);
    else
        return QString::null;
}

QString Commands::cmdMp3()
{
    QString strMessage = QString::null;

#ifdef Q_OS_WIN
    const QString strPlayer = "Winamp";
    Winamp *pWinamp = new Winamp();
    bool bIsRunning = pWinamp->isRunning();
    int iState = pWinamp->state();
    delete pWinamp;

    if (bIsRunning)
    {
        if (iState == 1)
        {
            Winamp *pWinamp = new Winamp();
            QString strVersion = pWinamp->version();
            QString strSong = pWinamp->song();
            QString strPosition = pWinamp->position();
            QString strLength = pWinamp->length();
            delete pWinamp;

            QString strWinamp = Settings::instance()->get("winamp");

            strWinamp.replace("$version", strVersion);
            strWinamp.replace("$song", strSong);
            strWinamp.replace("$position", strPosition);
            strWinamp.replace("$length", strLength);

            strMessage = strWinamp;
        }
        else if (iState == 3)
            strMessage = tr("%1 is paused").arg(strPlayer);
        else if (iState == 0)
            strMessage = tr("%1 is not playing").arg(strPlayer);
        else
            strMessage = tr("%1 is not running").arg(strPlayer);
    }
    else
        strMessage = tr("%1 is not running").arg(strPlayer);
#else
    const QString strPlayer = Settings::instance()->get("mpris_player");
    const QString strService = Settings::instance()->get("mpris_service");
    const QString strFormat = Settings::instance()->get("mpris_format");

    MprisPlayer mprisPlayer;
    strMessage = mprisPlayer.trackInfo(strPlayer, strService, strFormat);
#endif

    return strMessage;
}

QString Commands::cmdOffmsg()
{
    if (strDataList.size() < 3) return QString::null;

    QString strNick = strDataList.at(1);

    QString strMessage;
    for (int i = 2; i < strDataList.size(); ++i) { if (i != 2) strMessage += " "; strMessage += strDataList.at(i); }

    Convert::simpleReverseConvert(strMessage);
    Replace::replaceEmots(strMessage);

    return QString("NS OFFLINE MSG %1 %2").arg(strNick, strMessage);
}

QString Commands::cmdOp()
{
    if (strDataList.size() < 2) return QString::null;

    QString strChannel = strChan;
    QString strNick = strDataList.at(1);
    QChar cPlusMinus =  '+';

    if ((strNick.at(0) == '+') || (strNick.at(0) == '-'))
    {
        cPlusMinus = strNick.at(0);
        strNick.remove(0,1);
    }

    if (cPlusMinus == '+')
        return QString("CS HALFOP %1 ADD %2").arg(strChannel, strNick);
    else if (cPlusMinus == '-')
        return QString("CS HALFOP %1 DEL %2").arg(strChannel, strNick);
    else
        return QString::null;
}

QString Commands::cmdPart()
{
    QString strChannel = strChan;

    QString strMessage;
    for (int i = 1; i < strDataList.size(); ++i) { if (i != 1) strMessage += " "; strMessage += strDataList.at(i); }

    if (!strMessage.isEmpty())
        return QString("PART %1 :%2").arg(strChannel, strMessage);
    else
        return QString("PART %1").arg(strChannel);
}

QString Commands::cmdPriv()
{
    if (strDataList.size() < 2) return QString::null;

    QString strNick = strDataList.at(1);

    return QString("PRIV %1").arg(strNick);
}

QString Commands::cmdQuit()
{
    QString strReason;
    for (int i = 1; i < strDataList.size(); ++i) { if (i != 1) strReason += " "; strReason += strDataList.at(i); }

    Settings::instance()->setBool("reconnect", false);

    if (!strReason.isEmpty())
    {
        Convert::simpleReverseConvert(strReason);
        Replace::replaceEmots(strReason);

        return QString("QUIT :%1").arg(strReason);
    }
    else
        return "QUIT";
}

QString Commands::cmdRaw()
{
    if (strDataList.size() < 2) return QString::null;

    QString strCommand;
    for (int i = 1; i < strDataList.size(); ++i) { if (i != 1) strCommand += " "; strCommand += strDataList.at(i); }

    return strCommand;
}

QString Commands::cmdSop()
{
    if (strDataList.size() < 2) return QString::null;

    QString strChannel = strChan;
    QString strNick = strDataList.at(1);
    QChar cPlusMinus =  '+';

    if ((strNick.at(0) == '+') || (strNick.at(0) == '-'))
    {
        cPlusMinus = strNick.at(0);
        strNick.remove(0,1);
    }

    if (cPlusMinus == '+')
        return QString("CS OP %1 ADD %2").arg(strChannel, strNick);
    else if (cPlusMinus == '-')
        return QString("CS OP %1 DEL %2").arg(strChannel, strNick);
    else
        return QString::null;
}

QString Commands::cmdTopic()
{
    //if (strDataList.size() < 2) return QString::null;

    QString strChannel = strChan;

    QString strMessage;
    for (int i = 1; i < strDataList.size(); ++i) { if (i != 1) strMessage += " "; strMessage += strDataList.at(i); }

    Convert::simpleReverseConvert(strMessage);
    Replace::replaceEmots(strMessage);
    Convert::createText(strMessage);

    return QString("CS SET %1 TOPIC %2").arg(strChannel, strMessage);
}

QString Commands::cmdVip()
{
    if (strDataList.size() < 2) return QString::null;

    QString strChannel = strChan;
    QString strNick = strDataList.at(1);
    QChar cPlusMinus =  '+';

    if ((strNick.at(0) == '+') || (strNick.at(0) == '-'))
    {
        cPlusMinus = strNick.at(0);
        strNick.remove(0,1);
    }

    if (cPlusMinus == '+')
        return QString("CS VOICE %1 ADD %2").arg(strChannel, strNick);
    else if (cPlusMinus == '-')
        return QString("CS VOICE %1 DEL %2").arg(strChannel, strNick);
    else
        return QString::null;
}

QString Commands::cmdWhereis()
{
    if (strDataList.size() < 2) return QString::null;

    QString strNick = strDataList.at(1);

    return QString("WHOIS %1 :%1").arg(strNick);
}
