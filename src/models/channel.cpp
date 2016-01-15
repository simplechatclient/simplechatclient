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

#include <QLabel>
#include <QUuid>
#include "avatar.h"
#include "core/core.h"
#include "common/convert.h"
#include "common/log.h"
#include "core/mainwindow.h"
#include "channel.h"

Channel * Channel::Instance = 0;

Channel * Channel::instance()
{
    if (!Instance)
    {
        Instance = new Channel();
        Instance->clear();
    }

    return Instance;
}

Channel::Channel()
{
}

void Channel::clear()
{
    removeAll();

    lChannelAlternativeName.clear();
    lChannelsText.clear();
}

void Channel::add(const QString &channel)
{
    if ((channel.isEmpty()) || (lChannels.contains(channel)))
        return;

    OnetChannel ochannel;
    ochannel.name = channel;
    ochannel.displayedOptions = false;
    ochannel.offline = false;
    ochannel.index = lChannels.size();
    ochannel.tw = new TabWidget(channel);

    lChannels[channel] = ochannel;

    // log
    if (channel.at(0) == '^')
    {
        QString alternativeName = getAlternativeName(channel);
        if (!alternativeName.isEmpty())
            Log::logOpened(channel);
    }
    else
        Log::logOpened(channel);
}

void Channel::remove(const QString &channel)
{
    if (!lChannels.contains(channel))
        return;

    OnetChannel ochannel = lChannels.value(channel);
    lChannels.remove(channel);
    int index = ochannel.index;

    delete ochannel.tw;

    // log
    Log::logClosed(channel);

    // remove alternative name
    if (channel.at(0) == '^')
        removeAlternativeName(channel);

    // fix index
    foreach (const OnetChannel &ochannel, lChannels)
    {
        if (ochannel.index >= index)
            lChannels[ochannel.name].index--;
    }
}

void Channel::removeAll()
{
    foreach (const OnetChannel &ochannel, lChannels)
    {
        QString channel = ochannel.name;

        delete ochannel.tw;

        // log
        Log::logClosed(channel);
    }

    lChannels.clear();
}

void Channel::move(int from, int to)
{
    QString channelFrom = getNameFromIndex(from);
    QString channelTo = getNameFromIndex(to);

    if (!channelFrom.isEmpty()) lChannels[channelFrom].index = to;
    if (!channelTo.isEmpty()) lChannels[channelTo].index = from;
}

bool Channel::contains(const QString &channel)
{
    return lChannels.contains(channel);
}

QString Channel::getNameFromIndex(int index)
{
    foreach (const OnetChannel &ochannel, lChannels)
    {
        if (ochannel.index == index)
            return ochannel.name;
    }
    return QString::null;
}

int Channel::getIndexFromName(const QString &channel)
{
    if (lChannels.contains(channel))
        return lChannels[channel].index;
    else
        return -1;
}

QString Channel::getCurrentName()
{
    int index = Core::instance()->mainWindow()->getCurrentTabIndex();

    return getNameFromIndex(index);
}

QList<QString> Channel::getList()
{
    return lChannels.keys();
}

QList<QString> Channel::getListCleared()
{
    QList<QString> lChannelsCleared = lChannels.keys();
    lChannelsCleared.removeOne(DEBUG_WINDOW);
    lChannelsCleared.removeOne(STATUS_WINDOW);
    return lChannelsCleared;
}

QList<CaseIgnoreString> Channel::getListClearedSorted()
{
    // copy to new list
    QList<QString> lChannels = getListCleared();

    QList<CaseIgnoreString> lChannelsCaseIgnore;

    foreach (const QString &strChannel, lChannels)
        lChannelsCaseIgnore.append(strChannel);

    // sort
    qStableSort(lChannelsCaseIgnore.begin(), lChannelsCaseIgnore.end());

    return lChannelsCaseIgnore;
}

// displayed options
bool Channel::getDisplayedOptions(const QString &channel)
{
    if (lChannels.contains(channel))
        return lChannels[channel].displayedOptions;
    else
        return false;
}

void Channel::setDisplayedOptions(const QString &channel, bool displayed)
{
    if (!lChannels.contains(channel))
        return;

    lChannels[channel].displayedOptions = displayed;
}

// avatar
QString Channel::getAvatar(const QString &channel)
{
    if (lChannels.contains(channel))
        return lChannels[channel].avatar;
    else
        return QString::null;
}

void Channel::setAvatar(const QString &channel, const QString &avatar)
{
    if (!lChannels.contains(channel))
        return;

    lChannels[channel].avatar = avatar;

    if (!avatar.isEmpty())
    {
        int index = lChannels[channel].index;

        Core::instance()->mainWindow()->updateChannelIcon(index, Avatar::instance()->getAvatarPath(avatar));
    }
}

void Channel::setPrivAvatar(const QString &nick, const QString &avatar)
{
    foreach (const OnetChannel &ochannel, lChannels)
    {
        int index = ochannel.index;
        QString strChannel = ochannel.name;
        QString strChannelAvatar = ochannel.avatar;
        QString strChannelAlternativeName = lChannelAlternativeName.value(ochannel.name, QString::null);

        if ((strChannelAlternativeName == nick) && (strChannelAvatar != avatar))
        {
            lChannels[strChannel].avatar = avatar;

            Core::instance()->mainWindow()->updateChannelIcon(index, Avatar::instance()->getAvatarPath(avatar));
        }
    }
}

// offline
void Channel::setOffline(const QString &channel, bool offline)
{
    if (!lChannels.contains(channel))
        return;

    lChannels[channel].offline = offline;
}

bool Channel::getOffline(const QString &channel)
{
    if (lChannels.contains(channel))
        return lChannels[channel].offline;
    else
        return false;
}

// moderate mesages
QList<OnetModerateMessage> Channel::getModerateMessages(const QString &channel)
{
    QList<OnetModerateMessage> lMessages;

    if (lChannels.contains(channel))
        lMessages = lChannels[channel].moderateMessages;

    return lMessages;
}

void Channel::addModerateMessage(const QString &channel, qint64 time, const QString &nick, const QString &message)
{
    OnetModerateMessage oModerateMessage;
    oModerateMessage.id = QUuid::createUuid().toString();
    oModerateMessage.datetime = time;
    oModerateMessage.nick = nick;
    oModerateMessage.message = message;

    lChannels[channel].moderateMessages.append(oModerateMessage);
}

void Channel::removeModerateMessage(const QString &channel, const QString &id)
{
    if (!lChannels.contains(channel))
        return;

    QMutableListIterator<OnetModerateMessage> it(lChannels[channel].moderateMessages);
    while (it.hasNext())
    {
        OnetModerateMessage oModerateMessage = it.next();
        if (oModerateMessage.id == id)
            it.remove();
    }
}

// alternative name
void Channel::setAlternativeName(const QString &channel, const QString &name)
{
    lChannelAlternativeName[channel] = name;
}

QString Channel::getAlternativeName(const QString &channel)
{
    return lChannelAlternativeName.value(channel, QString::null);
}

void Channel::removeAlternativeName(const QString &channel)
{
    lChannelAlternativeName.remove(channel);
}

bool Channel::containsAlternativeName(const QString &channel)
{
    return lChannelAlternativeName.contains(channel);
}

// channel text
void Channel::setChannelText(const QString &channel, const QString &text)
{
    lChannelsText[channel] = text;
}

QString Channel::getChannelText(const QString &channel)
{
    return lChannelsText.value(channel, QString::null);
}

// tw
TabWidget* Channel::getTw(const QString &channel)
{
    if (lChannels.contains(channel))
    {
        return lChannels[channel].tw;
    }
    else
    {
        add(channel);
        return getTw(channel);
    }
}

TopicWidget* Channel::getTopic(const QString &channel)
{
    return getTw(channel)->topic;
}

ChatView* Channel::getChatView(const QString &channel)
{
    return getTw(channel)->pChatView;
}

QLabel* Channel::getUsers(const QString &channel)
{
    return getTw(channel)->users;
}

NickListWidget* Channel::getNickListWidget(const QString &channel)
{
    return getTw(channel)->pNickListWidget;
}

QSplitter* Channel::getSplitter(const QString &channel)
{
    return getTw(channel)->splitter;
}

bool Channel::copySelectedText(const QString &channel)
{
    return getTw(channel)->pChatView->copySelectedText();
}

// topic
void Channel::setTopic(const QString &strChannel, const QString &strTopicContent)
{
    if (!lChannels.contains(strChannel))
        return;

    QString strTopic = strTopicContent;
    QString strTooltip = strTopicContent;

    // convert emoticons, font
    Convert::fixHtmlChars(strTopic);
    Convert::fixTopicUrl(strTopic);
    Convert::convertText(strTopic);

    // set topic
    getTopic(strChannel)->setHtml(QString("<b>%1</b>&nbsp;%2").arg(tr("Topic:"), strTopic));

    // tooltip
    Convert::fixHtmlChars(strTooltip);
    Convert::simpleConvert(strTooltip);

    getTopic(strChannel)->setToolTip(strTooltip);
}

void Channel::setAuthorTopic(const QString &strChannel, const QString &strNick)
{
    if (!lChannels.contains(strChannel))
        return;

    QString strTopicDetails = QString(tr("Topic set by %1")).arg(strNick);

    getTopic(strChannel)->setToolTip(strTopicDetails);
}
