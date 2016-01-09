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

#ifndef DEFINES_H
#define DEFINES_H

#include <QListWidgetItem>
#include <QTableWidgetItem>

enum MessageCategory
{
    MessageDefault,
    MessageJoin,
    MessagePart,
    MessageQuit,
    MessageKick,
    MessageMode,
    MessageNotice,
    MessageInfo,
    MessageMe,
    MessageError,
    MessageHighlight,
    MessageModerNotice
};

enum ChannelColor
{
    ChannelGreen,
    ChannelRed,
    ChannelHighlight
};

enum ObjectStatus
{
    //StatusNotStarted,
    StatusAwaiting,
    //StatusInProgress,
    StatusCompleted
};

enum SortedListWidgetRoles
{
    SortedListWidgetNicklistRole = Qt::UserRole+10,
    SortedListWidgetStatusRole = Qt::UserRole+11
};

enum EmoticonsCategory
{
    EmoticonsStandard,
    EmoticonsEmoji
};

const int MAX_OPEN_CHANNELS = 20+2; // 20 - onet limit, 2 - status,debug
const int MAX_PUNISH_REASONS = 255;

const int HOURS_IN_DAY = 24;
const int MINUTES_IN_HOUR = 60;
const int SECONDS_IN_MINUTE = 60;

const int NOTICE_INFO = 100 | 0x070000;
const int NOTICE_WARNING = 101 | 0x070000;
const int NOTICE_ERROR = 102 | 0x070000;
const int NOTICE_QUESTION = 103 | 0x070000;

const int FLAG_DEV_INT = 64;
const int FLAG_ADMIN_INT = 32;
const int FLAG_OWNER_INT = 16;
const int FLAG_OP_INT = 8;
const int FLAG_HALFOP_INT = 4;
const int FLAG_MOD_INT = 2;
const int FLAG_SCREENER_INT = 1;
const int FLAG_VOICE_INT = 0;
const int FLAG_UNKNOWN_INT = -1;

#define STATUS_WINDOW "Status"
#define DEBUG_WINDOW "Debug"

#define FLAG_REGISTERED "r"
#define FLAG_AWAY "a"
#define FLAG_BUSY "b"
#define FLAG_CAM_PRIV "V"
#define FLAG_CAM_PUB "W"
#define FLAG_VOICE "+"
#define FLAG_SCREENER "="
#define FLAG_MOD "!"
#define FLAG_HALFOP "%"
#define FLAG_OP "@"
#define FLAG_OWNER "`"
#define FLAG_ADMIN "o"
#define FLAG_DEV "O"
#define FLAG_BOT "B"

#define PERMISSION_OWNER "q"
#define PERMISSION_OP "o"
#define PERMISSION_HALFOP "h"
#define PERMISSION_BAN "b"
#define PERMISSION_INVITE "I"

#define USER_SEX_MALE 'M'
#define USER_SEX_FEMALE 'F'
#define USER_SEX_UNKNOWN 'U'

#define UPDATE_STATUS_BETA "beta"
#define UPDATE_STATUS_UPTODATE "uptodate"
#define UPDATE_STATUS_OUTOFDATE "outofdate"
#define UPDATE_STATUS_UNKNOWN "unknown"

// struct Theme

// struct OnetChannel

struct OnetNick
{
    QString avatar;
    QList<QString> channels;
    QHash<QString,QString> channel_modes;
    QHash<QString,int> channel_max_modes;
    QChar sex;
};

struct OnetChannelList
{
    QString name;
    int people;
    int cat;
    int type;
    bool moderated;
    bool recommended;
};

struct OnetChannelFavourites
{
    QString avatar;
};

struct OnetChannelHomes
{
    QString avatar;
    QString flag;
};

struct OnetModerateMessage
{
    QString id;
    qint64 datetime;
    QString nick;
    QString message;
};

struct OnetOfflineMessage
{
    qint64 datetime;
    QString type;
    QString message;
};

struct OnetInvite
{
    QString id;
    qint64 datetime;
    QString nick;
    QString channel;
};

namespace ThemeFlags
{
    enum Flag {
        NoFlags = 0x0,
        Default = 0x1,
        ChannelAvatar = 0x2,
        NicklistAvatar = 0x4
    };
    Q_DECLARE_FLAGS(Flags, Flag)
}
Q_DECLARE_OPERATORS_FOR_FLAGS(ThemeFlags::Flags)

struct Theme
{
    QString name;
    ThemeFlags::Flags flags;
};

class CaseIgnoreString : public QString
{
public:
    CaseIgnoreString(const QString& other = QString()) : QString(other) {}

    bool operator<(const QString & other) const {
        return toLower() < other.toLower();
    }
    bool operator==(const QString& other) const {
        return toLower() == other.toLower();
    }
};

class SortedListWidgetItem : public QListWidgetItem
{
public:
    SortedListWidgetItem() {}
    SortedListWidgetItem(QListWidget *parent) : QListWidgetItem(parent) {}

    virtual bool operator< ( const QListWidgetItem & o ) const
    {
        QVariant my = data(Qt::DisplayRole); // compare value 1
        QVariant other = o.data(Qt::DisplayRole); // compare value 2
        QVariant isNickList = data(SortedListWidgetNicklistRole); // compare value 2
        if (!my.isValid() || !other.isValid() || my.type() != other.type()) // valid compare
            return QListWidgetItem::operator< (o);
        if (my.canConvert(QVariant::String)) // my compare
        {
            if (isNickList.toBool() == true) // sorting by status
            {
                int mModes = data(SortedListWidgetStatusRole).toInt();
                int oModes = o.data(SortedListWidgetStatusRole).toInt();
                if (mModes != oModes)
                {
                    if (mModes < oModes)
                        return false;
                    else
                        return true;
                }
                else
                    return compareNicks(text(), o.text()) < 0;
            }
            else
                return compareNicks(text(), o.text()) < 0;
        }
        else
            return QListWidgetItem::operator< (o); // other compare
    }

    static int compareNicks(const QString & nick1, const QString & nick2)
    {
        if (nick1.startsWith('~') && !nick2.startsWith('~'))
            return 1;
        else if (!nick1.startsWith('~') && nick2.startsWith('~'))
            return -1;
        else
            return nick1.toLower().localeAwareCompare(nick2.toLower());
    }
};

#endif // DEFINES_H
