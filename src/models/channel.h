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

#ifndef CHANNEL_H
#define CHANNEL_H

#include "core/defines.h"
#include "tab/tab_widget.h"
#include <QHash>
#include <QObject>

struct OnetChannel
{
    int index;
    QString name;
//    QString alternativeName;
    QString avatar;
//    QList<QString> flags;
//    QHash<QString,QString> options;
    bool displayedOptions;
    bool offline;
    TabWidget *tw;
    QList<OnetModerateMessage> moderateMessages;
};

class Channel : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(Channel)
    static Channel *Instance;
public:
    static Channel *instance();

    Channel();

    void add(const QString &channel);
    void remove(const QString &channel);
    void removeAll();
    void move(int from, int to);
    bool contains(const QString &channel);
    QString getNameFromIndex(int index);
    int getIndexFromName(const QString &channel);
    QString getCurrentName();
    QList<QString> getList();
    QList<QString> getListCleared();
    QList<CaseIgnoreString> getListClearedSorted();
    // displayed options
    bool getDisplayedOptions(const QString &channel);
    void setDisplayedOptions(const QString &channel, bool displayed);
    // avatar
    QString getAvatar(const QString &channel);
    void setAvatar(const QString &channel, const QString &avatar);
    void setPrivAvatar(const QString &nick, const QString &avatar);
    // offline
    void setOffline(const QString &channel, bool offline);
    bool getOffline(const QString &channel);
    // moderate messages
    void addModerateMessage(const QString &channel, qint64 time, const QString &nick, const QString &message);
    QList<OnetModerateMessage> getModerateMessages(const QString &channel);
    void removeModerateMessage(const QString &channel, const QString &id);
    // priv
    void setAlternativeName(const QString &channel, const QString &name);
    void removeAlternativeName(const QString &channel);
    QString getAlternativeName(const QString &channel);
    bool containsAlternativeName(const QString &channel);
    // channel text
    void setChannelText(const QString &channel, const QString &text);
    QString getChannelText(const QString &channel);
    // tw
    TabWidget* getTw(const QString &channel);
    TopicWidget* getTopic(const QString &channel);
    ChatView* getChatView(const QString &channel);
    QLabel* getUsers(const QString &channel);
    NickListWidget* getNickListWidget(const QString &channel);
    QSplitter* getSplitter(const QString &channel);
    // ctrl+c
    bool copySelectedText(const QString &channel);

    void setTopic(const QString &strChannel, const QString &strTopicContent);
    void setAuthorTopic(const QString &strChannel, const QString &strNick);

private:
    QHash<QString, OnetChannel> lChannels;
    QHash<QString, QString> lChannelAlternativeName;
    QHash<QString, QString> lChannelsText;

    void clear();
};

#endif // CHANNEL_H
