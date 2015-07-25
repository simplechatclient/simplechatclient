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

#include <QDateTime>
#include <QFileInfo>
#include "models/avatar.h"
#include "models/cache.h"
#include "convert.h"
#include "models/nick.h"
#include "models/settings.h"
#include "html_messages_renderer.h"

HtmlMessagesRenderer * HtmlMessagesRenderer::Instance = 0;

HtmlMessagesRenderer * HtmlMessagesRenderer::instance()
{
    if (!Instance)
    {
        Instance = new HtmlMessagesRenderer();
    }

    return Instance;
}

HtmlMessagesRenderer::HtmlMessagesRenderer()
{
}

void HtmlMessagesRenderer::fixContextMenu(QString &strData, MessageCategory eMessageCategory, const QString &strChannel)
{
    QStringList strDataList = strData.split(" ");
    QStringList lUrlImages;
    QStringList lYoutubeImages;

    // nick
    if ((strDataList.at(0) == "*") && ((eMessageCategory == MessageJoin) || (eMessageCategory == MessagePart) || (eMessageCategory == MessageQuit)  || (eMessageCategory == MessageKick)))
    {
        QString strWord = strDataList.at(1); Convert::removeStyles(strWord);
        strDataList[1] = QString("<a onclick=\"return false\" name=\"nick\" href=\"#\">%1</a>").arg(strWord);
    }

    for (int i = 0; i < strDataList.size(); ++i)
    {
        QString strWord = strDataList.at(i);

        // channel
        if ((!strWord.isEmpty()) && (strWord.contains("#")))
        {
            QString strChannelWord = strWord;
            Convert::removeStyles(strChannelWord);
            if (strChannelWord.at(0) == '#')
            {
                if (eMessageCategory == MessageInfo)
                    Convert::convertPrefix(strChannelWord);

                strDataList[i] = QString("<a onclick=\"return false\" name=\"channel\" href=\"#\" class=\"ChannelColor\">%1</a>").arg(strChannelWord);
            }
        }

        // web
        if ((strWord.startsWith("http:")) || (strWord.startsWith("https:")) || (strWord.startsWith("www.")))
        {
            Convert::removeStyles(strWord);

            QRegExp exYoutube_1("youtube.com/watch\\?.*v=([a-zA-Z0-9_-]{11})");
            QRegExp exYoutube_2("youtube.com/v/([a-zA-Z0-9_-]{11})");
            QRegExp exYoutube_3("youtu.be/([a-zA-Z0-9_-]{11})");

            QList<QString> lSupportedImages;
            lSupportedImages << "jpg" << "jpeg" << "png" << "bmp";

            if ((strWord.contains(exYoutube_1)) || (strWord.contains(exYoutube_2)) || (strWord.contains(exYoutube_3)))
            {
                if (!exYoutube_1.cap(1).isEmpty()) lYoutubeImages << exYoutube_1.cap(1);
                if (!exYoutube_2.cap(1).isEmpty()) lYoutubeImages << exYoutube_2.cap(1);
                if (!exYoutube_3.cap(1).isEmpty()) lYoutubeImages << exYoutube_3.cap(1);

                QString strYoutubeLink = QString("<a onclick=\"return false\" name=\"youtube\" href=\"%1\"><img src=\"qrc:/images/breeze/media-playback-start.svg\" alt=\"%2\" /></a>").arg(strWord, tr("Watch video"));
                strDataList[i] = QString("<a onclick=\"return false\" name=\"website\" href=\"%1\">%2</a> %3").arg(strWord, strWord, strYoutubeLink);
            }
            else if (lSupportedImages.contains(QFileInfo(strWord).suffix().toLower()))
            {
                lUrlImages << strWord;

                strDataList[i] = QString("<a onclick=\"return false\" name=\"website\" href=\"%1\">%2</a>").arg(strWord, strWord);
            }
            else
            {
                strDataList[i] = QString("<a onclick=\"return false\" name=\"website\" href=\"%1\">%2</a>").arg(strWord, strWord);
            }
        }
        else if (((strWord.contains("http:")) && (!strWord.startsWith("http:"))) || ((strWord.contains("https:")) && (!strWord.startsWith("https:"))) || ((strWord.contains("www.")) && (!strWord.startsWith("www."))))
        {
            if ((strWord.contains("http:")) || (strWord.contains("https:")))
            {
                strWord.replace("http:", " http:");
                strWord.replace("https:", " https:");
            }
            else
                strWord.replace("www.", " www.");

            QStringList strWords = strWord.split(" ");
            if (strWords.size() == 2)
            {
                QString strBeforeLink = strWords.at(0);
                QString strAfterLink = strWords.at(1);

                Convert::removeStyles(strAfterLink);

                QRegExp exYoutube_1("youtube.com/watch\\?.*v=([a-zA-Z0-9_-]{11})");
                QRegExp exYoutube_2("youtube.com/v/([a-zA-Z0-9_-]{11})");
                QRegExp exYoutube_3("youtu.be/([a-zA-Z0-9_-]{11})");

                QList<QString> lSupportedImages;
                lSupportedImages << "jpg" << "jpeg" << "png" << "bmp";

                if ((strAfterLink.contains(exYoutube_1)) || (strAfterLink.contains(exYoutube_2)) || (strAfterLink.contains(exYoutube_3)))
                {
                    if (!exYoutube_1.cap(1).isEmpty()) lYoutubeImages << exYoutube_1.cap(1);
                    if (!exYoutube_2.cap(1).isEmpty()) lYoutubeImages << exYoutube_2.cap(1);
                    if (!exYoutube_3.cap(1).isEmpty()) lYoutubeImages << exYoutube_3.cap(1);

                    QString strYoutubeLink = QString("<a onclick=\"return false\" name=\"youtube\" href=\"%1\"><img src=\"qrc:/images/breeze/media-playback-start.svg\" alt=\"%2\" /></a>").arg(strAfterLink, tr("Watch video"));
                    strAfterLink = QString("<a onclick=\"return false\" name=\"website\" href=\"%1\">%2</a> %3").arg(strAfterLink, strAfterLink, strYoutubeLink);
                }
                else if (lSupportedImages.contains(QFileInfo(strAfterLink).suffix().toLower()))
                {
                    lUrlImages << strAfterLink;

                    strAfterLink = QString("<a onclick=\"return false\" name=\"website\" href=\"%1\">%2</a>").arg(strAfterLink, strAfterLink);
                }
                else
                {
                    strAfterLink = QString("<a onclick=\"return false\" name=\"website\" href=\"%1\">%2</a>").arg(strAfterLink, strAfterLink);
                }

                strDataList[i] = strBeforeLink+strAfterLink;
            }
        }
    }

    if (((!lUrlImages.isEmpty()) || (!lYoutubeImages.isEmpty())) && (Settings::instance()->getBool("img_thumbs")))
    {
        strDataList << "<br/><span class=\"thumbs\">";

        foreach (const QString strImage, lUrlImages)
        {
            QString strCacheImage = Cache::instance()->get(strChannel, strImage);
#ifdef Q_OS_WIN
            strCacheImage = "/"+strCacheImage;
#endif
            strDataList << QString("<a onclick=\"return false\" name=\"website\" href=\"%1\"><img class=\"thumb\" src=\"file://%2\" alt=\"image\" onerror=\"imgError(this);\" /></a>").arg(strImage, strCacheImage);
        }

        foreach (const QString strImage, lYoutubeImages)
        {
            QString strFullImage = QString("http://youtu.be/%1").arg(strImage);
            QString strThumbImage = QString("http://img.youtube.com/vi/%1/default.jpg").arg(strImage);
            QString strCacheImage = Cache::instance()->get(strChannel, strThumbImage);
#ifdef Q_OS_WIN
            strCacheImage = "/"+strCacheImage;
#endif
            strDataList << QString("<a onclick=\"return false\" name=\"website\" href=\"%1\"><img class=\"thumb\" src=\"file://%2\" alt=\"image\" onerror=\"imgError(this);\" /></a>").arg(strFullImage, strCacheImage);
        }

        strDataList << "</span>";
    }

    strData = strDataList.join(" ");
}

QString HtmlMessagesRenderer::renderer(QString strData, MessageCategory eMessageCategory, qint64 iTime, QString strNick, QString strChannel)
{
    QDateTime dt = QDateTime::fromMSecsSinceEpoch(iTime);

    // fix html chars
    Convert::fixHtmlChars(strData);

    // fix for context menu
    fixContextMenu(strData, eMessageCategory, strChannel);

    // /me
    QString strRegExpMe = QString("%1ACTION %2%3").arg(QString(QByteArray("\x01")), "(.*)", QString(QByteArray("\x01")));
    if (strData.contains(QRegExp(strRegExpMe)))
    {
        strData.replace(QRegExp(strRegExpMe), "\\1");
        eMessageCategory = MessageMe;
    }

    // convert emoticons, font
    Convert::convertText(strData, true, true);

    // fix utf8
    strData = Convert::fixUtf8Chars(strData);

    // font class
    QString strFontClass;
    switch(eMessageCategory)
    {
        case MessageDefault: strFontClass = "DefaultColor"; break;
        case MessageJoin: strFontClass = "MessageJoin"; break;
        case MessagePart: strFontClass = "MessagePart"; break;
        case MessageQuit: strFontClass = "MessageQuit"; break;
        case MessageKick: strFontClass = "MessageKick"; break;
        case MessageMode: strFontClass = "MessageMode"; break;
        case MessageNotice: strFontClass = "MessageNotice"; break;
        case MessageInfo: strFontClass = "MessageInfo"; break;
        case MessageMe: strFontClass = "MessageMe"; break;
        case MessageError: strFontClass = "MessageError"; break;
        case MessageHighlight: strFontClass = "DefaultColor"; break;
        case MessageModerNotice: strFontClass = "NoticeColor"; break;
    }

    // themes
    QString strThemes = Settings::instance()->get("themes");

    // highlight
    QString strExtraClass;
    if (eMessageCategory == MessageHighlight)
        strExtraClass = "underline";

    // me & modernotice
    QString strBeforeNick;
    QString strAfterNick;
    if ((eMessageCategory == MessageMe) || (eMessageCategory == MessageModerNotice))
        strBeforeNick = "* ";
    else
    {
        if ((strThemes == "Adara") || (strThemes == "Alhena"))
            strAfterNick = ":";
        else
        {
            strBeforeNick = "&lt;";
            strAfterNick = "&gt;";
        }
    }

    if (strThemes == "Adara")
    {
        if (!strNick.isEmpty())
        {
            QString strUserAvatar = Nick::instance()->getAvatar(strNick);

            // is valid avatar
            if (strUserAvatar.isEmpty())
            {
                if (strNick.at(0) == '~')
                    strUserAvatar = Avatar::instance()->getEmptyUnregisteredUserAvatarBig();
                else
                    strUserAvatar = Avatar::instance()->getEmptyRegisteredUserAvatarBig();
            }
            else
                strUserAvatar = Avatar::instance()->getAvatarPath(strUserAvatar);

#ifdef Q_OS_WIN
            strUserAvatar = "/"+strUserAvatar;
#endif
            strUserAvatar = "file://"+strUserAvatar;
            QString strUserAvatarImg = QString("<img src=\"%1\" alt=\"avatar\" class=\"avatar\" />").arg(strUserAvatar);

            return QString("<section>"
                               "%1"
                               "<span class=\"message\">"
                                   "<span class=\"DefaultColor\">"
                                       "%2<a href=\"#\" onclick=\"return false\" name=\"nick\">%3</a>%4"
                                   "</span>"
                                   "&nbsp;"
                                   "<span class=\"%5 %6\">%7</span>"
                               "</span>"
                               "<time class=\"right\">%8</time>"
                           "</section>")
                    .arg(strUserAvatarImg, strBeforeNick, strNick, strAfterNick, strFontClass, strExtraClass, strData, dt.toString(Settings::instance()->get("time_format")));
        }
        else
        {
            return QString("<section>"
                               "<span class=\"message\">"
                                   "<span class=\"%1\">%2</span>"
                               "</span>"
                               "<time class=\"right\">%3</time>"
                           "</section>")
                    .arg(strFontClass, strData, dt.toString(Settings::instance()->get("time_format")));
        }
    }
    else if (strThemes == "Alhena")
    {
        if (!strNick.isEmpty())
            return QString("<section>"
                               "<span class=\"message\">"
                                   "<span class=\"DefaultColor\">"
                                     "%1<a href=\"#\" onclick=\"return false\" name=\"nick\">%2</a>%3"
                                   "</span>"
                                   "&nbsp;"
                                   "<span class=\"%4 %5\">%6</span>"
                               "</span>"
                               "<time class=\"right\">[%7]</time>"
                           "</section>")
                    .arg(strBeforeNick, strNick, strAfterNick, strFontClass, strExtraClass, strData, dt.toString(Settings::instance()->get("time_format")));
        else
            return QString("<section>"
                               "<span class=\"message\">"
                                   "<span class=\"%1\">%2</span>"
                               "</span>"
                               "<time class=\"right\">[%3]</time>"
                           "</section>")
                    .arg(strFontClass, strData, dt.toString(Settings::instance()->get("time_format")));
    }
    else if (strThemes == "Origin")
    {
        if (!strNick.isEmpty())
            return QString("<section>"
                               "<time class=\"time_small\">[%1]</time>"
                               "&nbsp;"
                               "<span class=\"message DefaultColor\">"
                                   "%2<a href=\"#\" onclick=\"return false\" name=\"nick\">%3</a>%4"
                                   "&nbsp;"
                                   "<span class=\"%5 %6\">%7</span>"
                               "</span>"
                           "</section>")
                    .arg(dt.toString(Settings::instance()->get("time_format")), strBeforeNick, strNick, strAfterNick, strFontClass, strExtraClass, strData);
        else
            return QString("<section>"
                               "<time class=\"time_small\">[%1]</time>"
                               "&nbsp;"
                               "<span class=\"message DefaultColor\">"
                                   "<span class=\"%2\">%3</span>"
                               "</span>"
                           "</section>")
                    .arg(dt.toString(Settings::instance()->get("time_format")), strFontClass, strData);
    }
    else // standard
    {
        if (!strNick.isEmpty())
            return QString("<section>"
                               "<time>[%1]</time>"
                               "&nbsp;"
                               "<span class=\"message DefaultColor\">"
                                   "%2<a href=\"#\" onclick=\"return false\" name=\"nick\">%3</a>%4"
                                   "&nbsp;"
                                   "<span class=\"%5 %6\">%7</span>"
                               "</span>"
                           "</section>")
                    .arg(dt.toString(Settings::instance()->get("time_format")), strBeforeNick, strNick, strAfterNick, strFontClass, strExtraClass, strData);
        else
            return QString("<section>"
                               "<time>[%1]</time>"
                               "&nbsp;"
                               "<span class=\"message DefaultColor\">"
                                   "<span class=\"%2\">%3</span>"
                               "</span>"
                           "</section>")
                    .arg(dt.toString(Settings::instance()->get("time_format")), strFontClass, strData);
    }
}

QString HtmlMessagesRenderer::rendererDebug(QString strData, qint64 iTime)
{
    QString strColor = "#000000";
    if (strData.startsWith("<- "))
    {
        strColor = "#ee0000";
        strData.remove(0,3);
    }
    else if (strData.startsWith("-> "))
    {
        strColor = "#0000ee";
        strData.remove(0,3);
    }

    // fix html chars
    Convert::fixHtmlChars(strData);

    // time
    QString strTime = QDateTime::fromMSecsSinceEpoch(iTime).toString("[hh:mm:ss]");

    // display
    return QString("<section>"
                       "<time>%1</time>"
                       "&nbsp;"
                       "<code style=\"color:%2;\">%3</code>"
                   "</section>")
            .arg(strTime, strColor, strData);
}

QString HtmlMessagesRenderer::headCSS()
{
    QString strDefaultColor = Settings::instance()->get("default_color");
    QString strChannelColor = Settings::instance()->get("channel_color");
    QString strTimeColor = Settings::instance()->get("time_color");
    QString strFontSize = Settings::instance()->get("font_size");

    QString strMessageJoin = Settings::instance()->get("message_join_color");
    QString strMessagePart = Settings::instance()->get("message_part_color");
    QString strMessageQuit = Settings::instance()->get("message_quit_color");
    QString strMessageKick = Settings::instance()->get("message_kick_color");
    QString strMessageMode = Settings::instance()->get("message_mode_color");
    QString strMessageNotice = Settings::instance()->get("message_notice_color");
    QString strMessageInfo = Settings::instance()->get("message_info_color");
    QString strMessageMe = Settings::instance()->get("message_me_color");
    QString strMessageError = Settings::instance()->get("message_error_color");

    QString strHeadCSS
            = "article {margin-bottom:2px; vertical-align:bottom;}"
            + QString("section {margin:0; padding:0; font-family:sans-serif; font-size:%1; white-space: pre-wrap; display:table-row;}").arg(strFontSize)
            + QString("time {color:%1; display:table-cell;}").arg(strTimeColor)
            + ".message {width:100%; word-break: break-word; display:table-cell;}"
            + "a {color:inherit; text-decoration:none;}"
            + ".thumb {max-width:75px; max-height:75px; box-shadow: 0 0 5px #888;}"
            + ".thumbs {display: block; margin: 6px 0px 3px 100px;}"
            + ".underline {text-decoration:underline;}"
            + ".avatar {margin-left:4px; margin-right:4px; width:30px; height:30px; display:table-cell;}"
            + ".right {float:right;}"
            + ".time_small {font-size:0.95em;}"

            + QString(".DefaultColor{color:%1;}").arg(strDefaultColor)
            + QString(".ChannelColor{color:%1;}").arg(strChannelColor)

            + QString(".MessageDefault{color:%1;}").arg(strDefaultColor)
            + QString(".MessageJoin{color:%1;}").arg(strMessageJoin)
            + QString(".MessagePart{color:%1;}").arg(strMessagePart)
            + QString(".MessageQuit{color:%1;}").arg(strMessageQuit)
            + QString(".MessageKick{color:%1;}").arg(strMessageKick)
            + QString(".MessageMode{color:%1;}").arg(strMessageMode)
            + QString(".MessageNotice{color:%1;}").arg(strMessageNotice)
            + QString(".MessageInfo{color:%1;}").arg(strMessageInfo)
            + QString(".MessageMe{color:%1;}").arg(strMessageMe)
            + QString(".MessageError{color:%1;}").arg(strMessageError)
            + QString(".MessageHighlight{color:%1;}").arg(strDefaultColor)
            + QString(".MessageModerNotice{color:%1;}").arg(strDefaultColor);

    return strHeadCSS;
}

QString HtmlMessagesRenderer::bodyCSS()
{
    QString strFontSize = Settings::instance()->get("font_size");
    QString strBackgroundImage = Settings::instance()->get("background_image");

    QString strBackground;
    if (Settings::instance()->getBool("show_background_image") && !strBackgroundImage.isEmpty())
    {
#ifdef Q_OS_WIN
    strBackgroundImage = "/"+strBackgroundImage;
#endif
        strBackgroundImage = "file://"+strBackgroundImage;
        strBackground = QString("background-image: url('%1'); background-attachment: fixed; background-position: center; background-repeat: no-repeat;").arg(strBackgroundImage);
    }

    return QString("margin: 0; padding: 0; font-family: sans-serif; font-size:%1; %2").arg(strFontSize, strBackground);
}
