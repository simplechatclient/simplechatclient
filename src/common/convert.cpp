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

#include <QDir>
#include <QPixmap>
#include "models/settings.h"
#include "models/utils.h"
#include "convert.h"

#ifdef Q_OS_WIN
    #include <QCoreApplication>
#else
    #include "scc-config.h"
#endif

QString findEmoticon(const QString &strEmoticon)
{
    QString path;
#ifdef Q_OS_WIN
    path = QCoreApplication::applicationDirPath();
#else
    path = SCC_DATA_DIR;
#endif

    QDir dAllEmoticonsDirs = path+"/emoticons/";
    QStringList lDirs = dAllEmoticonsDirs.entryList(QStringList("*"), QDir::Dirs | QDir::NoDotAndDotDot | QDir::NoSymLinks);

    QStringList lSupportedEmoticons;
    lSupportedEmoticons << ".gif" << ".jpg" << ".jpeg" << ".png" << ".bmp";

    foreach (const QString &strDir, lDirs)
    {
        foreach (const QString &strSupportedEmoticon, lSupportedEmoticons)
        {
            QString strEmoticonCheck = QString("%1/emoticons/%2/%3%4").arg(path, strDir, strEmoticon, strSupportedEmoticon);
            if (QFile::exists(strEmoticonCheck))
                return strEmoticonCheck;
        }
    }
    return QString::null;
}

QString findEmoticonEmoji(const QString &strEmoticon)
{
    QString path;
#ifdef Q_OS_WIN
    path = QCoreApplication::applicationDirPath();
#else
    path = SCC_DATA_DIR;
#endif

    QDir dAllEmoticonsDirs = path+"/emoticons_emoji/";
    QStringList lDirs = dAllEmoticonsDirs.entryList(QStringList("*"), QDir::Dirs | QDir::NoDotAndDotDot | QDir::NoSymLinks);

    foreach (const QString &strDir, lDirs)
    {
        QString strEmoticonCheck = QString("%1/emoticons_emoji/%2/%3%4").arg(path, strDir, strEmoticon, ".png");
        if (QFile::exists(strEmoticonCheck))
            return strEmoticonCheck;
    }
    return QString::null;
}

void convertColor(QString &strData)
{
    QList<QString> lColors = Utils::instance()->getColors();

    if (Settings::instance()->getBool("font_formating"))
    {
        foreach (const QString &strColor, lColors)
        {
            QRegExp rx("%C"+strColor+"%");

            int pos = 0;
            while ((pos = rx.indexIn(strData, pos)) != -1)
            {
                int first = pos;
                int second = first + rx.matchedLength();

                strData.replace(first, second-first, QString("<span style=\"color:#%1;\">").arg(strColor));
                strData += "</span>";
            }
        }
    }
    else
    {
        foreach (const QString &strColor, lColors)
            strData.remove("%C"+strColor+"%");
    }
}

void convertFont(QString &strData)
{
    QRegExp rx("%F(b|i|bi)?:?(arial|times|verdana|tahoma|courier)?%");

    int pos = 0;
    while ((pos = rx.indexIn(strData, pos)) != -1)
    {
        int first = pos;
        int second = first + rx.matchedLength();

        if (Settings::instance()->getBool("font_formating"))
        {
            QString strAtributes;
            QString strFontStyle = rx.cap(1);
            QString strFontName = rx.cap(2);

            if (strFontStyle.contains("b")) strAtributes += "font-weight:bold;";
            else strAtributes += "font-weight:normal;";
            if (strFontStyle.contains("i")) strAtributes += "font-style:italic;";
            else strAtributes += "font-style:normal;";

            if (strFontName == "arial") strAtributes += "font-family:Arial;";
            else if (strFontName == "times") strAtributes += "font-family:Times New Roman;";
            else if (strFontName == "verdana") strAtributes += "font-family:Verdana;";
            else if (strFontName == "tahoma") strAtributes += "font-family:Tahoma;";
            else if (strFontName == "courier") strAtributes += "font-family:Courier New;";
            else strAtributes += "font-family:Verdana;";

            strData.replace(first, second-first, QString("<span style=\"%1\">").arg(strAtributes));
            strData += "</span>";
        }
        else
            strData.remove(first, second-first);
    }
}

void convertEmoticons(QString &strData, bool bInsertWidthHeight, bool qWebViewContext)
{
    QRegExp rx("%I([a-zA-Z0-9_-]+)%");

    int pos = 0;
    while ((pos = rx.indexIn(strData, pos)) != -1)
    {
        QString strEmoticon = rx.cap(1);
        QString strEmoticonFull = "%I"+strEmoticon+"%";

        if (Settings::instance()->getBool("emoticons"))
        {
            QString strEmoticonPath = findEmoticon(strEmoticon);

            if (!strEmoticonPath.isEmpty())
            {
                QString strWidthHeight;
                if (bInsertWidthHeight)
                {
                    QPixmap p(strEmoticonPath);
                    strWidthHeight = " width=\""+QString::number(p.width())+"\" height=\""+QString::number(p.height())+"\"";
                }
#ifdef Q_OS_WIN
                strEmoticonPath = "/"+strEmoticonPath;
#endif
                if (qWebViewContext)
                    strEmoticonPath = "file://"+strEmoticonPath;
                strData.replace(strEmoticonFull, QString("<img src=\"%1\" alt=\"//%2\" title=\"//%2\"%3 />").arg(strEmoticonPath, strEmoticon, strWidthHeight));
            }
            else
                strData.replace(strEmoticonFull, QString("//%1").arg(strEmoticon));
        }
        else
            strData.replace(strEmoticonFull, QString("//%1").arg(strEmoticon));
    }
}

void convertEmoticonsEmoji(QString &strData, bool qWebViewContext)
{
    QRegExp rx(":([\\w+-]+):");

    int pos = 0;
    while ((pos = rx.indexIn(strData, pos)) != -1)
    {
        QString strEmoticon = rx.cap(1);
        QString strEmoticonFull = ":"+strEmoticon+":";

        if (Settings::instance()->getBool("emoticons"))
        {
            QString strEmoticonPath = findEmoticonEmoji(strEmoticon);

            if (!strEmoticonPath.isEmpty())
            {
#ifdef Q_OS_WIN
                strEmoticonPath = "/"+strEmoticonPath;
#endif
                if (qWebViewContext)
                    strEmoticonPath = "file://"+strEmoticonPath;

                strData.replace(strEmoticonFull, QString("<img src=\"%1\" alt=\"&#58;%2&#58;\" title=\"&#58;%2&#58;\" width=\"22\" height=\"22\" />").arg(strEmoticonPath, strEmoticon));
            }
            else
                pos += rx.matchedLength();
        }
        else
            pos += rx.matchedLength();
    }
}

/*
void convertEmoticonsToSlash(QString &strData)
{
    QRegExp rx("%I([a-zA-Z0-9_-]+)%");

    int pos = 0;
    while ((pos = rx.indexIn(strData, pos)) != -1)
    {
        QString strEmoticon = rx.cap(1);
        QString strEmoticonFull = "%I"+strEmoticon+"%";

        QString strEmoticonPath = findEmoticon(strEmoticon);

        if (!strEmoticonPath.isEmpty())
            strData.replace(strEmoticonFull, QString("//%1").arg(strEmoticon));
        else
            pos += rx.matchedLength();
    }
}
*/
void convertSlashToEmoticons(QString &strData)
{
    QRegExp rx("(http:|https:){0,}//([a-zA-Z0-9_-]+)");

    int pos = 0;
    while ((pos = rx.indexIn(strData, pos)) != -1)
    {
        if (rx.cap(1).isEmpty())
        {
            int first = pos;
            int matchedLength = rx.cap(2).length() + 2;
            int second = first + matchedLength;

            QString strEmoticon = rx.cap(2);

            if (!findEmoticon(strEmoticon).isEmpty())
                strData.replace(first, second-first, "%I"+strEmoticon+"%");
            else
                pos += matchedLength;
        }
        else
            pos += rx.matchedLength();
    }
}

void removeColor(QString &strData)
{
    QList<QString> lColors = Utils::instance()->getColors();
    foreach (const QString &strColor, lColors)
        strData.remove("%C"+strColor+"%");
}

void removeFont(QString &strData)
{
    QRegExp rx("%F(b|i|bi)?:?(arial|times|verdana|tahoma|courier)?%");

    int pos = 0;
    while ((pos = rx.indexIn(strData, pos)) != -1)
    {
        int first = pos;
        int second = first + rx.matchedLength();

        strData.remove(first, second-first);
    }
}

void removeEmoticons(QString &strData)
{
    QRegExp rx("%I([a-zA-Z0-9_-]+)%");

    int pos = 0;
    while ((pos = rx.indexIn(strData, pos)) != -1)
    {
        int first = pos;
        int second = first + rx.matchedLength();

        QString strEmoticon = rx.cap(1);
        QString strEmoticonPath = findEmoticon(strEmoticon);

        if (!strEmoticonPath.isEmpty())
            strData.remove(first, second-first);
        else
            pos += rx.matchedLength();
    }
}

void Convert::fixMeAction(QString &strData)
{
    QString strRegExpMe = QString("%1ACTION %2%3").arg(QString(QByteArray("\x01")), "(.*)", QString(QByteArray("\x01")));
    if (strData.contains(QRegExp(strRegExpMe)))
        strData.replace(QRegExp(strRegExpMe), "\\1");
}
/*
void convertUtf8ToEmoji(QString &strData)
{
    QHash<QString, QString> lEmoticonsEmoji;
    lEmoticonsEmoji[":hash:"] = "\x23\xEF\xB8\x8F\xE2\x83\xA3";
    lEmoticonsEmoji["\x30\xEF\xB8\x8F\xE2\x83\xA3"] = ":zero:";
    lEmoticonsEmoji["\x31\xEF\xB8\x8F\xE2\x83\xA3"] = ":one:";
    lEmoticonsEmoji["\x32\xEF\xB8\x8F\xE2\x83\xA3"] = ":two:";
    lEmoticonsEmoji["\x33\xEF\xB8\x8F\xE2\x83\xA3"] = ":three:";
    lEmoticonsEmoji["\x34\xEF\xB8\x8F\xE2\x83\xA3"] = ":four:";
    lEmoticonsEmoji["\x35\xEF\xB8\x8F\xE2\x83\xA3"] = ":five:";
    lEmoticonsEmoji["\x36\xEF\xB8\x8F\xE2\x83\xA3"] = ":six:";
    lEmoticonsEmoji["\x37\xEF\xB8\x8F\xE2\x83\xA3"] = ":seven:";
    lEmoticonsEmoji["\x38\xEF\xB8\x8F\xE2\x83\xA3"] = ":eight:";
    lEmoticonsEmoji["\x39\xEF\xB8\x8F\xE2\x83\xA3"] = ":nine:";
    lEmoticonsEmoji["\xC2\xA9\xEF\xB8\x8F"] = ":copyright:";
    lEmoticonsEmoji["\xC2\xAE\xEF\xB8\x8F"] = ":registered:";
    lEmoticonsEmoji["\xE2\x80\xBC\xEF\xB8\x8F"] = ":bangbang:";
    lEmoticonsEmoji["\xE2\x81\x89\xEF\xB8\x8F"] = ":interrobang:";
    lEmoticonsEmoji["\xE2\x84\xA2\xEF\xB8\x8F"] = ":tm:";
    lEmoticonsEmoji["\xE2\x84\xB9\xEF\xB8\x8F"] = ":information_source:";
    lEmoticonsEmoji["\xE2\x86\x94\xEF\xB8\x8F"] = ":left_right_arrow:";
    lEmoticonsEmoji["\xE2\x86\x95\xEF\xB8\x8F"] = ":arrow_up_down:";
    lEmoticonsEmoji["\xE2\x86\x96\xEF\xB8\x8F"] = ":arrow_upper_left:";
    lEmoticonsEmoji["\xE2\x86\x97\xEF\xB8\x8F"] = ":arrow_upper_right:";
    lEmoticonsEmoji["\xE2\x86\x98\xEF\xB8\x8F"] = ":arrow_lower_right:";
    lEmoticonsEmoji["\xE2\x86\x99\xEF\xB8\x8F"] = ":arrow_lower_left:";
    lEmoticonsEmoji["\xE2\x86\xA9\xEF\xB8\x8F"] = ":leftwards_arrow_with_hook:";
    lEmoticonsEmoji["\xE2\x86\xAA\xEF\xB8\x8F"] = ":arrow_right_hook:";
    lEmoticonsEmoji["\xE2\x8C\x9A\xEF\xB8\x8F"] = ":watch:";
    lEmoticonsEmoji["\xE2\x8C\x9B\xEF\xB8\x8F"] = ":hourglass:";
    lEmoticonsEmoji["\xE2\x8F\xA9"] = ":fast_forward:";
    lEmoticonsEmoji["\xE2\x8F\xAA"] = ":rewind:";
    lEmoticonsEmoji["\xE2\x8F\xAB"] = ":arrow_double_up:";
    lEmoticonsEmoji["\xE2\x8F\xAC"] = ":arrow_double_down:";
    lEmoticonsEmoji["\xE2\x8F\xB0"] = ":alarm_clock:";
    lEmoticonsEmoji["\xE2\x8F\xB3"] = ":hourglass_flowing_sand:";
    lEmoticonsEmoji["\xE2\x93\x82\xEF\xB8\x8F"] = ":m:";
    lEmoticonsEmoji["\xE2\x96\xAA\xEF\xB8\x8F"] = ":black_small_square:";
    lEmoticonsEmoji["\xE2\x96\xAB\xEF\xB8\x8F"] = ":white_small_square:";
    lEmoticonsEmoji["\xE2\x96\xB6\xEF\xB8\x8F"] = ":arrow_forward:";
    lEmoticonsEmoji["\xE2\x97\x80\xEF\xB8\x8F"] = ":arrow_backward:";
    lEmoticonsEmoji["\xE2\x97\xBB\xEF\xB8\x8F"] = ":white_medium_square:";
    lEmoticonsEmoji["\xE2\x97\xBC\xEF\xB8\x8F"] = ":black_medium_square:";
    lEmoticonsEmoji["\xE2\x97\xBD\xEF\xB8\x8F"] = ":white_medium_small_square:";
    lEmoticonsEmoji["\xE2\x97\xBE\xEF\xB8\x8F"] = ":black_medium_small_square:";
    lEmoticonsEmoji["\xE2\x98\x80\xEF\xB8\x8F"] = ":sunny:";
    lEmoticonsEmoji["\xE2\x98\x81\xEF\xB8\x8F"] = ":cloud:";
    lEmoticonsEmoji["\xE2\x98\x8E\xEF\xB8\x8F"] = ":telephone:";
    lEmoticonsEmoji["\xE2\x98\x91\xEF\xB8\x8F"] = ":ballot_box_with_check:";
    lEmoticonsEmoji["\xE2\x98\x94\xEF\xB8\x8F"] = ":umbrella:";
    lEmoticonsEmoji["\xE2\x98\x95\xEF\xB8\x8F"] = ":coffee:";
    lEmoticonsEmoji["\xE2\x98\x9D\xEF\xB8\x8F"] = ":point_up:";
    lEmoticonsEmoji["\xE2\x98\xBA\xEF\xB8\x8F"] = ":relaxed:";
    lEmoticonsEmoji["\xE2\x99\x88\xEF\xB8\x8F"] = ":aries:";
    lEmoticonsEmoji["\xE2\x99\x89\xEF\xB8\x8F"] = ":taurus:";
    lEmoticonsEmoji["\xE2\x99\x8A\xEF\xB8\x8F"] = ":gemini:";
    lEmoticonsEmoji["\xE2\x99\x8B\xEF\xB8\x8F"] = ":cancer:";
    lEmoticonsEmoji["\xE2\x99\x8C\xEF\xB8\x8F"] = ":leo:";
    lEmoticonsEmoji["\xE2\x99\x8D\xEF\xB8\x8F"] = ":virgo:";
    lEmoticonsEmoji["\xE2\x99\x8E\xEF\xB8\x8F"] = ":libra:";
    lEmoticonsEmoji["\xE2\x99\x8F\xEF\xB8\x8F"] = ":scorpius:";
    lEmoticonsEmoji["\xE2\x99\x90\xEF\xB8\x8F"] = ":sagittarius:";
    lEmoticonsEmoji["\xE2\x99\x91\xEF\xB8\x8F"] = ":capricorn:";
    lEmoticonsEmoji["\xE2\x99\x92\xEF\xB8\x8F"] = ":aquarius:";
    lEmoticonsEmoji["\xE2\x99\x93\xEF\xB8\x8F"] = ":pisces:";
    lEmoticonsEmoji["\xE2\x99\xA0\xEF\xB8\x8F"] = ":spades:";
    lEmoticonsEmoji["\xE2\x99\xA3\xEF\xB8\x8F"] = ":clubs:";
    lEmoticonsEmoji["\xE2\x99\xA5\xEF\xB8\x8F"] = ":hearts:";
    lEmoticonsEmoji["\xE2\x99\xA6\xEF\xB8\x8F"] = ":diamonds:";
    lEmoticonsEmoji["\xE2\x99\xA8\xEF\xB8\x8F"] = ":hotsprings:";
    lEmoticonsEmoji["\xE2\x99\xBB\xEF\xB8\x8F"] = ":recycle:";
    lEmoticonsEmoji["\xE2\x99\xBF\xEF\xB8\x8F"] = ":wheelchair:";
    lEmoticonsEmoji["\xE2\x9A\x93\xEF\xB8\x8F"] = ":anchor:";
    lEmoticonsEmoji["\xE2\x9A\xA0\xEF\xB8\x8F"] = ":warning:";
    lEmoticonsEmoji["\xE2\x9A\xA1\xEF\xB8\x8F"] = ":zap:";
    lEmoticonsEmoji["\xE2\x9A\xAA\xEF\xB8\x8F"] = ":white_circle:";
    lEmoticonsEmoji["\xE2\x9A\xAB\xEF\xB8\x8F"] = ":black_circle:";
    lEmoticonsEmoji["\xE2\x9A\xBD\xEF\xB8\x8F"] = ":soccer:";
    lEmoticonsEmoji["\xE2\x9A\xBE\xEF\xB8\x8F"] = ":baseball:";
    lEmoticonsEmoji["\xE2\x9B\x84\xEF\xB8\x8F"] = ":snowman:";
    lEmoticonsEmoji["\xE2\x9B\x85\xEF\xB8\x8F"] = ":partly_sunny:";
    lEmoticonsEmoji["\xE2\x9B\x8E"] = ":ophiuchus:";
    lEmoticonsEmoji["\xE2\x9B\x94\xEF\xB8\x8F"] = ":no_entry:";
    lEmoticonsEmoji["\xE2\x9B\xAA\xEF\xB8\x8F"] = ":church:";
    lEmoticonsEmoji["\xE2\x9B\xB2\xEF\xB8\x8F"] = ":fountain:";
    lEmoticonsEmoji["\xE2\x9B\xB3\xEF\xB8\x8F"] = ":golf:";
    lEmoticonsEmoji["\xE2\x9B\xB5\xEF\xB8\x8F"] = ":sailboat:";
    lEmoticonsEmoji["\xE2\x9B\xBA\xEF\xB8\x8F"] = ":tent:";
    lEmoticonsEmoji["\xE2\x9B\xBD\xEF\xB8\x8F"] = ":fuelpump:";
    lEmoticonsEmoji["\xE2\x9C\x82\xEF\xB8\x8F"] = ":scissors:";
    lEmoticonsEmoji["\xE2\x9C\x85"] = ":white_check_mark:";
    lEmoticonsEmoji["\xE2\x9C\x88\xEF\xB8\x8F"] = ":airplane:";
    lEmoticonsEmoji["\xE2\x9C\x89\xEF\xB8\x8F"] = ":envelope:";
    lEmoticonsEmoji["\xE2\x9C\x8A"] = ":fist:";
    lEmoticonsEmoji["\xE2\x9C\x8B"] = ":raised_hand:";
    lEmoticonsEmoji["\xE2\x9C\x8C\xEF\xB8\x8F"] = ":v:";
    lEmoticonsEmoji["\xE2\x9C\x8F\xEF\xB8\x8F"] = ":pencil2:";
    lEmoticonsEmoji["\xE2\x9C\x92\xEF\xB8\x8F"] = ":black_nib:";
    lEmoticonsEmoji["\xE2\x9C\x94\xEF\xB8\x8F"] = ":heavy_check_mark:";
    lEmoticonsEmoji["\xE2\x9C\x96\xEF\xB8\x8F"] = ":heavy_multiplication_x:";
    lEmoticonsEmoji["\xE2\x9C\xA8"] = ":sparkles:";
    lEmoticonsEmoji["\xE2\x9C\xB3\xEF\xB8\x8F"] = ":eight_spoked_asterisk:";
    lEmoticonsEmoji["\xE2\x9C\xB4\xEF\xB8\x8F"] = ":eight_pointed_black_star:";
    lEmoticonsEmoji["\xE2\x9D\x84\xEF\xB8\x8F"] = ":snowflake:";
    lEmoticonsEmoji["\xE2\x9D\x87\xEF\xB8\x8F"] = ":sparkle:";
    lEmoticonsEmoji["\xE2\x9D\x8C"] = ":x:";
    lEmoticonsEmoji["\xE2\x9D\x8E"] = ":negative_squared_cross_mark:";
    lEmoticonsEmoji["\xE2\x9D\x93"] = ":question:";
    lEmoticonsEmoji["\xE2\x9D\x94"] = ":grey_question:";
    lEmoticonsEmoji["\xE2\x9D\x95"] = ":grey_exclamation:";
    lEmoticonsEmoji["\xE2\x9D\x97\xEF\xB8\x8F"] = ":exclamation:";
    lEmoticonsEmoji["\xE2\x9D\xA4\xEF\xB8\x8F"] = ":heart:";
    lEmoticonsEmoji["\xE2\x9E\x95"] = ":heavy_plus_sign:";
    lEmoticonsEmoji["\xE2\x9E\x96"] = ":heavy_minus_sign:";
    lEmoticonsEmoji["\xE2\x9E\x97"] = ":heavy_division_sign:";
    lEmoticonsEmoji["\xE2\x9E\xA1\xEF\xB8\x8F"] = ":arrow_right:";
    lEmoticonsEmoji["\xE2\x9E\xB0"] = ":curly_loop:";
    lEmoticonsEmoji["\xE2\xA4\xB4\xEF\xB8\x8F"] = ":arrow_heading_up:";
    lEmoticonsEmoji["\xE2\xA4\xB5\xEF\xB8\x8F"] = ":arrow_heading_down:";
    lEmoticonsEmoji["\xE2\xAC\x85\xEF\xB8\x8F"] = ":arrow_left:";
    lEmoticonsEmoji["\xE2\xAC\x86\xEF\xB8\x8F"] = ":arrow_up:";
    lEmoticonsEmoji["\xE2\xAC\x87\xEF\xB8\x8F"] = ":arrow_down:";
    lEmoticonsEmoji["\xE2\xAC\x9B\xEF\xB8\x8F"] = ":black_large_square:";
    lEmoticonsEmoji["\xE2\xAC\x9C\xEF\xB8\x8F"] = ":white_large_square:";
    lEmoticonsEmoji["\xE2\xAD\x90\xEF\xB8\x8F"] = ":star:";
    lEmoticonsEmoji["\xE2\xAD\x95\xEF\xB8\x8F"] = ":o:";
    lEmoticonsEmoji["\xE3\x80\xB0"] = ":wavy_dash:";
    lEmoticonsEmoji["\xE3\x80\xBD\xEF\xB8\x8F"] = ":part_alternation_mark:";
    lEmoticonsEmoji["\xE3\x8A\x97\xEF\xB8\x8F"] = ":congratulations:";
    lEmoticonsEmoji["\xE3\x8A\x99\xEF\xB8\x8F"] = ":secret:";
    lEmoticonsEmoji["\xF0\x9F\x80\x84\xEF\xB8\x8F"] = ":mahjong:";
    lEmoticonsEmoji["\xF0\x9F\x83\x8F"] = ":black_joker:";
    lEmoticonsEmoji["\xF0\x9F\x85\xB0"] = ":a:";
    lEmoticonsEmoji["\xF0\x9F\x85\xB1"] = ":b:";
    lEmoticonsEmoji["\xF0\x9F\x85\xBE"] = ":o2:";
    lEmoticonsEmoji["\xF0\x9F\x85\xBF\xEF\xB8\x8F"] = ":parking:";
    lEmoticonsEmoji["\xF0\x9F\x86\x8E"] = ":ab:";
    lEmoticonsEmoji["\xF0\x9F\x86\x91"] = ":cl:";
    lEmoticonsEmoji["\xF0\x9F\x86\x92"] = ":cool:";
    lEmoticonsEmoji["\xF0\x9F\x86\x93"] = ":free:";
    lEmoticonsEmoji["\xF0\x9F\x86\x94"] = ":id:";
    lEmoticonsEmoji["\xF0\x9F\x86\x95"] = ":new:";
    lEmoticonsEmoji["\xF0\x9F\x86\x96"] = ":ng:";
    lEmoticonsEmoji["\xF0\x9F\x86\x97"] = ":ok:";
    lEmoticonsEmoji["\xF0\x9F\x86\x98"] = ":sos:";
    lEmoticonsEmoji["\xF0\x9F\x86\x99"] = ":up:";
    lEmoticonsEmoji["\xF0\x9F\x86\x9A"] = ":vs:";
    lEmoticonsEmoji["\xF0\x9F\x87\xA8\xF0\x9F\x87\xB3"] = ":flag_cn:";
    lEmoticonsEmoji["\xF0\x9F\x87\xA8\xF0\x9F\x87\xB3"] = ":cn:";
    lEmoticonsEmoji["\xF0\x9F\x87\xA9\xF0\x9F\x87\xAA"] = ":flag_de:";
    lEmoticonsEmoji["\xF0\x9F\x87\xA9\xF0\x9F\x87\xAA"] = ":de:";
    lEmoticonsEmoji["\xF0\x9F\x87\xAA\xF0\x9F\x87\xB8"] = ":flag_es:";
    lEmoticonsEmoji["\xF0\x9F\x87\xAA\xF0\x9F\x87\xB8"] = ":es:";
    lEmoticonsEmoji["\xF0\x9F\x87\xAB\xF0\x9F\x87\xB7"] = ":flag_fr:";
    lEmoticonsEmoji["\xF0\x9F\x87\xAB\xF0\x9F\x87\xB7"] = ":fr:";
    lEmoticonsEmoji["\xF0\x9F\x87\xAC\xF0\x9F\x87\xA7"] = ":flag_gb:";
    lEmoticonsEmoji["\xF0\x9F\x87\xAC\xF0\x9F\x87\xA7"] = ":gb:";
    lEmoticonsEmoji["\xF0\x9F\x87\xAE\xF0\x9F\x87\xB9"] = ":flag_it:";
    lEmoticonsEmoji["\xF0\x9F\x87\xAE\xF0\x9F\x87\xB9"] = ":it:";
    lEmoticonsEmoji["\xF0\x9F\x87\xAF\xF0\x9F\x87\xB5"] = ":flag_jp:";
    lEmoticonsEmoji["\xF0\x9F\x87\xAF\xF0\x9F\x87\xB5"] = ":jp:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB0\xF0\x9F\x87\xB7"] = ":flag_kr:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB0\xF0\x9F\x87\xB7"] = ":kr:";
    lEmoticonsEmoji["\xF0\x9F\x87\xBA\xF0\x9F\x87\xB8"] = ":flag_us:";
    lEmoticonsEmoji["\xF0\x9F\x87\xBA\xF0\x9F\x87\xB8"] = ":us:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB7\xF0\x9F\x87\xBA"] = ":flag_ru:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB7\xF0\x9F\x87\xBA"] = ":ru:";
    lEmoticonsEmoji["\xF0\x9F\x88\x81"] = ":koko:";
    lEmoticonsEmoji["\xF0\x9F\x88\x82"] = ":sa:";
    lEmoticonsEmoji["\xF0\x9F\x88\x9A\xEF\xB8\x8F"] = ":u7121:";
    lEmoticonsEmoji["\xF0\x9F\x88\xAF\xEF\xB8\x8F"] = ":u6307:";
    lEmoticonsEmoji["\xF0\x9F\x88\xB2"] = ":u7981:";
    lEmoticonsEmoji["\xF0\x9F\x88\xB3"] = ":u7a7a:";
    lEmoticonsEmoji["\xF0\x9F\x88\xB4"] = ":u5408:";
    lEmoticonsEmoji["\xF0\x9F\x88\xB5"] = ":u6e80:";
    lEmoticonsEmoji["\xF0\x9F\x88\xB6"] = ":u6709:";
    lEmoticonsEmoji["\xF0\x9F\x88\xB7\xEF\xB8\x8F"] = ":u6708:";
    lEmoticonsEmoji["\xF0\x9F\x88\xB8"] = ":u7533:";
    lEmoticonsEmoji["\xF0\x9F\x88\xB9"] = ":u5272:";
    lEmoticonsEmoji["\xF0\x9F\x88\xBA"] = ":u55b6:";
    lEmoticonsEmoji["\xF0\x9F\x89\x90"] = ":ideograph_advantage:";
    lEmoticonsEmoji["\xF0\x9F\x89\x91"] = ":accept:";
    lEmoticonsEmoji["\xF0\x9F\x8C\x80"] = ":cyclone:";
    lEmoticonsEmoji["\xF0\x9F\x8C\x81"] = ":foggy:";
    lEmoticonsEmoji["\xF0\x9F\x8C\x82"] = ":closed_umbrella:";
    lEmoticonsEmoji["\xF0\x9F\x8C\x83"] = ":night_with_stars:";
    lEmoticonsEmoji["\xF0\x9F\x8C\x84"] = ":sunrise_over_mountains:";
    lEmoticonsEmoji["\xF0\x9F\x8C\x85"] = ":sunrise:";
    lEmoticonsEmoji["\xF0\x9F\x8C\x86"] = ":city_dusk:";
    lEmoticonsEmoji["\xF0\x9F\x8C\x87"] = ":city_sunset:";
    lEmoticonsEmoji["\xF0\x9F\x8C\x87"] = ":city_sunrise:";
    lEmoticonsEmoji["\xF0\x9F\x8C\x88"] = ":rainbow:";
    lEmoticonsEmoji["\xF0\x9F\x8C\x89"] = ":bridge_at_night:";
    lEmoticonsEmoji["\xF0\x9F\x8C\x8A"] = ":ocean:";
    lEmoticonsEmoji["\xF0\x9F\x8C\x8B"] = ":volcano:";
    lEmoticonsEmoji["\xF0\x9F\x8C\x8C"] = ":milky_way:";
    lEmoticonsEmoji["\xF0\x9F\x8C\x8F"] = ":earth_asia:";
    lEmoticonsEmoji["\xF0\x9F\x8C\x91"] = ":new_moon:";
    lEmoticonsEmoji["\xF0\x9F\x8C\x93"] = ":first_quarter_moon:";
    lEmoticonsEmoji["\xF0\x9F\x8C\x94"] = ":waxing_gibbous_moon:";
    lEmoticonsEmoji["\xF0\x9F\x8C\x95"] = ":full_moon:";
    lEmoticonsEmoji["\xF0\x9F\x8C\x99"] = ":crescent_moon:";
    lEmoticonsEmoji["\xF0\x9F\x8C\x9B"] = ":first_quarter_moon_with_face:";
    lEmoticonsEmoji["\xF0\x9F\x8C\x9F"] = ":star2:";
    lEmoticonsEmoji["\xF0\x9F\x8C\xA0"] = ":stars:";
    lEmoticonsEmoji["\xF0\x9F\x8C\xB0"] = ":chestnut:";
    lEmoticonsEmoji["\xF0\x9F\x8C\xB1"] = ":seedling:";
    lEmoticonsEmoji["\xF0\x9F\x8C\xB4"] = ":palm_tree:";
    lEmoticonsEmoji["\xF0\x9F\x8C\xB5"] = ":cactus:";
    lEmoticonsEmoji["\xF0\x9F\x8C\xB7"] = ":tulip:";
    lEmoticonsEmoji["\xF0\x9F\x8C\xB8"] = ":cherry_blossom:";
    lEmoticonsEmoji["\xF0\x9F\x8C\xB9"] = ":rose:";
    lEmoticonsEmoji["\xF0\x9F\x8C\xBA"] = ":hibiscus:";
    lEmoticonsEmoji["\xF0\x9F\x8C\xBB"] = ":sunflower:";
    lEmoticonsEmoji["\xF0\x9F\x8C\xBC"] = ":blossom:";
    lEmoticonsEmoji["\xF0\x9F\x8C\xBD"] = ":corn:";
    lEmoticonsEmoji["\xF0\x9F\x8C\xBE"] = ":ear_of_rice:";
    lEmoticonsEmoji["\xF0\x9F\x8C\xBF"] = ":herb:";
    lEmoticonsEmoji["\xF0\x9F\x8D\x80"] = ":four_leaf_clover:";
    lEmoticonsEmoji["\xF0\x9F\x8D\x81"] = ":maple_leaf:";
    lEmoticonsEmoji["\xF0\x9F\x8D\x82"] = ":fallen_leaf:";
    lEmoticonsEmoji["\xF0\x9F\x8D\x83"] = ":leaves:";
    lEmoticonsEmoji["\xF0\x9F\x8D\x84"] = ":mushroom:";
    lEmoticonsEmoji["\xF0\x9F\x8D\x85"] = ":tomato:";
    lEmoticonsEmoji["\xF0\x9F\x8D\x86"] = ":eggplant:";
    lEmoticonsEmoji["\xF0\x9F\x8D\x87"] = ":grapes:";
    lEmoticonsEmoji["\xF0\x9F\x8D\x88"] = ":melon:";
    lEmoticonsEmoji["\xF0\x9F\x8D\x89"] = ":watermelon:";
    lEmoticonsEmoji["\xF0\x9F\x8D\x8A"] = ":tangerine:";
    lEmoticonsEmoji["\xF0\x9F\x8D\x8C"] = ":banana:";
    lEmoticonsEmoji["\xF0\x9F\x8D\x8D"] = ":pineapple:";
    lEmoticonsEmoji["\xF0\x9F\x8D\x8E"] = ":apple:";
    lEmoticonsEmoji["\xF0\x9F\x8D\x8F"] = ":green_apple:";
    lEmoticonsEmoji["\xF0\x9F\x8D\x91"] = ":peach:";
    lEmoticonsEmoji["\xF0\x9F\x8D\x92"] = ":cherries:";
    lEmoticonsEmoji["\xF0\x9F\x8D\x93"] = ":strawberry:";
    lEmoticonsEmoji["\xF0\x9F\x8D\x94"] = ":hamburger:";
    lEmoticonsEmoji["\xF0\x9F\x8D\x95"] = ":pizza:";
    lEmoticonsEmoji["\xF0\x9F\x8D\x96"] = ":meat_on_bone:";
    lEmoticonsEmoji["\xF0\x9F\x8D\x97"] = ":poultry_leg:";
    lEmoticonsEmoji["\xF0\x9F\x8D\x98"] = ":rice_cracker:";
    lEmoticonsEmoji["\xF0\x9F\x8D\x99"] = ":rice_ball:";
    lEmoticonsEmoji["\xF0\x9F\x8D\x9A"] = ":rice:";
    lEmoticonsEmoji["\xF0\x9F\x8D\x9B"] = ":curry:";
    lEmoticonsEmoji["\xF0\x9F\x8D\x9C"] = ":ramen:";
    lEmoticonsEmoji["\xF0\x9F\x8D\x9D"] = ":spaghetti:";
    lEmoticonsEmoji["\xF0\x9F\x8D\x9E"] = ":bread:";
    lEmoticonsEmoji["\xF0\x9F\x8D\x9F"] = ":fries:";
    lEmoticonsEmoji["\xF0\x9F\x8D\xA0"] = ":sweet_potato:";
    lEmoticonsEmoji["\xF0\x9F\x8D\xA1"] = ":dango:";
    lEmoticonsEmoji["\xF0\x9F\x8D\xA2"] = ":oden:";
    lEmoticonsEmoji["\xF0\x9F\x8D\xA3"] = ":sushi:";
    lEmoticonsEmoji["\xF0\x9F\x8D\xA4"] = ":fried_shrimp:";
    lEmoticonsEmoji["\xF0\x9F\x8D\xA5"] = ":fish_cake:";
    lEmoticonsEmoji["\xF0\x9F\x8D\xA6"] = ":icecream:";
    lEmoticonsEmoji["\xF0\x9F\x8D\xA7"] = ":shaved_ice:";
    lEmoticonsEmoji["\xF0\x9F\x8D\xA8"] = ":ice_cream:";
    lEmoticonsEmoji["\xF0\x9F\x8D\xA9"] = ":doughnut:";
    lEmoticonsEmoji["\xF0\x9F\x8D\xAA"] = ":cookie:";
    lEmoticonsEmoji["\xF0\x9F\x8D\xAB"] = ":chocolate_bar:";
    lEmoticonsEmoji["\xF0\x9F\x8D\xAC"] = ":candy:";
    lEmoticonsEmoji["\xF0\x9F\x8D\xAD"] = ":lollipop:";
    lEmoticonsEmoji["\xF0\x9F\x8D\xAE"] = ":custard:";
    lEmoticonsEmoji["\xF0\x9F\x8D\xAF"] = ":honey_pot:";
    lEmoticonsEmoji["\xF0\x9F\x8D\xB0"] = ":cake:";
    lEmoticonsEmoji["\xF0\x9F\x8D\xB1"] = ":bento:";
    lEmoticonsEmoji["\xF0\x9F\x8D\xB2"] = ":stew:";
    lEmoticonsEmoji["\xF0\x9F\x8D\xB3"] = ":egg:";
    lEmoticonsEmoji["\xF0\x9F\x8D\xB4"] = ":fork_and_knife:";
    lEmoticonsEmoji["\xF0\x9F\x8D\xB5"] = ":tea:";
    lEmoticonsEmoji["\xF0\x9F\x8D\xB6"] = ":sake:";
    lEmoticonsEmoji["\xF0\x9F\x8D\xB7"] = ":wine_glass:";
    lEmoticonsEmoji["\xF0\x9F\x8D\xB8"] = ":cocktail:";
    lEmoticonsEmoji["\xF0\x9F\x8D\xB9"] = ":tropical_drink:";
    lEmoticonsEmoji["\xF0\x9F\x8D\xBA"] = ":beer:";
    lEmoticonsEmoji["\xF0\x9F\x8D\xBB"] = ":beers:";
    lEmoticonsEmoji["\xF0\x9F\x8E\x80"] = ":ribbon:";
    lEmoticonsEmoji["\xF0\x9F\x8E\x81"] = ":gift:";
    lEmoticonsEmoji["\xF0\x9F\x8E\x82"] = ":birthday:";
    lEmoticonsEmoji["\xF0\x9F\x8E\x83"] = ":jack_o_lantern:";
    lEmoticonsEmoji["\xF0\x9F\x8E\x84"] = ":christmas_tree:";
    lEmoticonsEmoji["\xF0\x9F\x8E\x85"] = ":santa:";
    lEmoticonsEmoji["\xF0\x9F\x8E\x86"] = ":fireworks:";
    lEmoticonsEmoji["\xF0\x9F\x8E\x87"] = ":sparkler:";
    lEmoticonsEmoji["\xF0\x9F\x8E\x88"] = ":balloon:";
    lEmoticonsEmoji["\xF0\x9F\x8E\x89"] = ":tada:";
    lEmoticonsEmoji["\xF0\x9F\x8E\x8A"] = ":confetti_ball:";
    lEmoticonsEmoji["\xF0\x9F\x8E\x8B"] = ":tanabata_tree:";
    lEmoticonsEmoji["\xF0\x9F\x8E\x8C"] = ":crossed_flags:";
    lEmoticonsEmoji["\xF0\x9F\x8E\x8D"] = ":bamboo:";
    lEmoticonsEmoji["\xF0\x9F\x8E\x8E"] = ":dolls:";
    lEmoticonsEmoji["\xF0\x9F\x8E\x8F"] = ":flags:";
    lEmoticonsEmoji["\xF0\x9F\x8E\x90"] = ":wind_chime:";
    lEmoticonsEmoji["\xF0\x9F\x8E\x91"] = ":rice_scene:";
    lEmoticonsEmoji["\xF0\x9F\x8E\x92"] = ":school_satchel:";
    lEmoticonsEmoji["\xF0\x9F\x8E\x93"] = ":mortar_board:";
    lEmoticonsEmoji["\xF0\x9F\x8E\xA0"] = ":carousel_horse:";
    lEmoticonsEmoji["\xF0\x9F\x8E\xA1"] = ":ferris_wheel:";
    lEmoticonsEmoji["\xF0\x9F\x8E\xA2"] = ":roller_coaster:";
    lEmoticonsEmoji["\xF0\x9F\x8E\xA3"] = ":fishing_pole_and_fish:";
    lEmoticonsEmoji["\xF0\x9F\x8E\xA4"] = ":microphone:";
    lEmoticonsEmoji["\xF0\x9F\x8E\xA5"] = ":movie_camera:";
    lEmoticonsEmoji["\xF0\x9F\x8E\xA6"] = ":cinema:";
    lEmoticonsEmoji["\xF0\x9F\x8E\xA7"] = ":headphones:";
    lEmoticonsEmoji["\xF0\x9F\x8E\xA8"] = ":art:";
    lEmoticonsEmoji["\xF0\x9F\x8E\xA9"] = ":tophat:";
    lEmoticonsEmoji["\xF0\x9F\x8E\xAA"] = ":circus_tent:";
    lEmoticonsEmoji["\xF0\x9F\x8E\xAB"] = ":ticket:";
    lEmoticonsEmoji["\xF0\x9F\x8E\xAC"] = ":clapper:";
    lEmoticonsEmoji["\xF0\x9F\x8E\xAD"] = ":performing_arts:";
    lEmoticonsEmoji["\xF0\x9F\x8E\xAE"] = ":video_game:";
    lEmoticonsEmoji["\xF0\x9F\x8E\xAF"] = ":dart:";
    lEmoticonsEmoji["\xF0\x9F\x8E\xB0"] = ":slot_machine:";
    lEmoticonsEmoji["\xF0\x9F\x8E\xB1"] = ":8ball:";
    lEmoticonsEmoji["\xF0\x9F\x8E\xB2"] = ":game_die:";
    lEmoticonsEmoji["\xF0\x9F\x8E\xB3"] = ":bowling:";
    lEmoticonsEmoji["\xF0\x9F\x8E\xB4"] = ":flower_playing_cards:";
    lEmoticonsEmoji["\xF0\x9F\x8E\xB5"] = ":musical_note:";
    lEmoticonsEmoji["\xF0\x9F\x8E\xB6"] = ":notes:";
    lEmoticonsEmoji["\xF0\x9F\x8E\xB7"] = ":saxophone:";
    lEmoticonsEmoji["\xF0\x9F\x8E\xB8"] = ":guitar:";
    lEmoticonsEmoji["\xF0\x9F\x8E\xB9"] = ":musical_keyboard:";
    lEmoticonsEmoji["\xF0\x9F\x8E\xBA"] = ":trumpet:";
    lEmoticonsEmoji["\xF0\x9F\x8E\xBB"] = ":violin:";
    lEmoticonsEmoji["\xF0\x9F\x8E\xBC"] = ":musical_score:";
    lEmoticonsEmoji["\xF0\x9F\x8E\xBD"] = ":running_shirt_with_sash:";
    lEmoticonsEmoji["\xF0\x9F\x8E\xBE"] = ":tennis:";
    lEmoticonsEmoji["\xF0\x9F\x8E\xBF"] = ":ski:";
    lEmoticonsEmoji["\xF0\x9F\x8F\x80"] = ":basketball:";
    lEmoticonsEmoji["\xF0\x9F\x8F\x81"] = ":checkered_flag:";
    lEmoticonsEmoji["\xF0\x9F\x8F\x82"] = ":snowboarder:";
    lEmoticonsEmoji["\xF0\x9F\x8F\x83"] = ":runner:";
    lEmoticonsEmoji["\xF0\x9F\x8F\x84"] = ":surfer:";
    lEmoticonsEmoji["\xF0\x9F\x8F\x86"] = ":trophy:";
    lEmoticonsEmoji["\xF0\x9F\x8F\x88"] = ":football:";
    lEmoticonsEmoji["\xF0\x9F\x8F\x8A"] = ":swimmer:";
    lEmoticonsEmoji["\xF0\x9F\x8F\xA0"] = ":house:";
    lEmoticonsEmoji["\xF0\x9F\x8F\xA1"] = ":house_with_garden:";
    lEmoticonsEmoji["\xF0\x9F\x8F\xA2"] = ":office:";
    lEmoticonsEmoji["\xF0\x9F\x8F\xA3"] = ":post_office:";
    lEmoticonsEmoji["\xF0\x9F\x8F\xA5"] = ":hospital:";
    lEmoticonsEmoji["\xF0\x9F\x8F\xA6"] = ":bank:";
    lEmoticonsEmoji["\xF0\x9F\x8F\xA7"] = ":atm:";
    lEmoticonsEmoji["\xF0\x9F\x8F\xA8"] = ":hotel:";
    lEmoticonsEmoji["\xF0\x9F\x8F\xA9"] = ":love_hotel:";
    lEmoticonsEmoji["\xF0\x9F\x8F\xAA"] = ":convenience_store:";
    lEmoticonsEmoji["\xF0\x9F\x8F\xAB"] = ":school:";
    lEmoticonsEmoji["\xF0\x9F\x8F\xAC"] = ":department_store:";
    lEmoticonsEmoji["\xF0\x9F\x8F\xAD"] = ":factory:";
    lEmoticonsEmoji["\xF0\x9F\x8F\xAE"] = ":izakaya_lantern:";
    lEmoticonsEmoji["\xF0\x9F\x8F\xAF"] = ":japanese_castle:";
    lEmoticonsEmoji["\xF0\x9F\x8F\xB0"] = ":european_castle:";
    lEmoticonsEmoji["\xF0\x9F\x90\x8C"] = ":snail:";
    lEmoticonsEmoji["\xF0\x9F\x90\x8D"] = ":snake:";
    lEmoticonsEmoji["\xF0\x9F\x90\x8E"] = ":racehorse:";
    lEmoticonsEmoji["\xF0\x9F\x90\x91"] = ":sheep:";
    lEmoticonsEmoji["\xF0\x9F\x90\x92"] = ":monkey:";
    lEmoticonsEmoji["\xF0\x9F\x90\x94"] = ":chicken:";
    lEmoticonsEmoji["\xF0\x9F\x90\x97"] = ":boar:";
    lEmoticonsEmoji["\xF0\x9F\x90\x98"] = ":elephant:";
    lEmoticonsEmoji["\xF0\x9F\x90\x99"] = ":octopus:";
    lEmoticonsEmoji["\xF0\x9F\x90\x9A"] = ":shell:";
    lEmoticonsEmoji["\xF0\x9F\x90\x9B"] = ":bug:";
    lEmoticonsEmoji["\xF0\x9F\x90\x9C"] = ":ant:";
    lEmoticonsEmoji["\xF0\x9F\x90\x9D"] = ":bee:";
    lEmoticonsEmoji["\xF0\x9F\x90\x9E"] = ":beetle:";
    lEmoticonsEmoji["\xF0\x9F\x90\x9F"] = ":fish:";
    lEmoticonsEmoji["\xF0\x9F\x90\xA0"] = ":tropical_fish:";
    lEmoticonsEmoji["\xF0\x9F\x90\xA1"] = ":blowfish:";
    lEmoticonsEmoji["\xF0\x9F\x90\xA2"] = ":turtle:";
    lEmoticonsEmoji["\xF0\x9F\x90\xA3"] = ":hatching_chick:";
    lEmoticonsEmoji["\xF0\x9F\x90\xA4"] = ":baby_chick:";
    lEmoticonsEmoji["\xF0\x9F\x90\xA5"] = ":hatched_chick:";
    lEmoticonsEmoji["\xF0\x9F\x90\xA6"] = ":bird:";
    lEmoticonsEmoji["\xF0\x9F\x90\xA7"] = ":penguin:";
    lEmoticonsEmoji["\xF0\x9F\x90\xA8"] = ":koala:";
    lEmoticonsEmoji["\xF0\x9F\x90\xA9"] = ":poodle:";
    lEmoticonsEmoji["\xF0\x9F\x90\xAB"] = ":camel:";
    lEmoticonsEmoji["\xF0\x9F\x90\xAC"] = ":dolphin:";
    lEmoticonsEmoji["\xF0\x9F\x90\xAD"] = ":mouse:";
    lEmoticonsEmoji["\xF0\x9F\x90\xAE"] = ":cow:";
    lEmoticonsEmoji["\xF0\x9F\x90\xAF"] = ":tiger:";
    lEmoticonsEmoji["\xF0\x9F\x90\xB0"] = ":rabbit:";
    lEmoticonsEmoji["\xF0\x9F\x90\xB1"] = ":cat:";
    lEmoticonsEmoji["\xF0\x9F\x90\xB2"] = ":dragon_face:";
    lEmoticonsEmoji["\xF0\x9F\x90\xB3"] = ":whale:";
    lEmoticonsEmoji["\xF0\x9F\x90\xB4"] = ":horse:";
    lEmoticonsEmoji["\xF0\x9F\x90\xB5"] = ":monkey_face:";
    lEmoticonsEmoji["\xF0\x9F\x90\xB6"] = ":dog:";
    lEmoticonsEmoji["\xF0\x9F\x90\xB7"] = ":pig:";
    lEmoticonsEmoji["\xF0\x9F\x90\xB8"] = ":frog:";
    lEmoticonsEmoji["\xF0\x9F\x90\xB9"] = ":hamster:";
    lEmoticonsEmoji["\xF0\x9F\x90\xBA"] = ":wolf:";
    lEmoticonsEmoji["\xF0\x9F\x90\xBB"] = ":bear:";
    lEmoticonsEmoji["\xF0\x9F\x90\xBC"] = ":panda_face:";
    lEmoticonsEmoji["\xF0\x9F\x90\xBD"] = ":pig_nose:";
    lEmoticonsEmoji["\xF0\x9F\x90\xBE"] = ":feet:";
    lEmoticonsEmoji["\xF0\x9F\x91\x80"] = ":eyes:";
    lEmoticonsEmoji["\xF0\x9F\x91\x82"] = ":ear:";
    lEmoticonsEmoji["\xF0\x9F\x91\x83"] = ":nose:";
    lEmoticonsEmoji["\xF0\x9F\x91\x84"] = ":lips:";
    lEmoticonsEmoji["\xF0\x9F\x91\x85"] = ":tongue:";
    lEmoticonsEmoji["\xF0\x9F\x91\x86"] = ":point_up_2:";
    lEmoticonsEmoji["\xF0\x9F\x91\x87"] = ":point_down:";
    lEmoticonsEmoji["\xF0\x9F\x91\x88"] = ":point_left:";
    lEmoticonsEmoji["\xF0\x9F\x91\x89"] = ":point_right:";
    lEmoticonsEmoji["\xF0\x9F\x91\x8A"] = ":punch:";
    lEmoticonsEmoji["\xF0\x9F\x91\x8B"] = ":wave:";
    lEmoticonsEmoji["\xF0\x9F\x91\x8C"] = ":ok_hand:";
    lEmoticonsEmoji["\xF0\x9F\x91\x8D"] = ":thumbsup:";
    lEmoticonsEmoji["\xF0\x9F\x91\x8D"] = ":+1:";
    lEmoticonsEmoji["\xF0\x9F\x91\x8E"] = ":thumbsdown:";
    lEmoticonsEmoji["\xF0\x9F\x91\x8E"] = ":-1:";
    lEmoticonsEmoji["\xF0\x9F\x91\x8F"] = ":clap:";
    lEmoticonsEmoji["\xF0\x9F\x91\x90"] = ":open_hands:";
    lEmoticonsEmoji["\xF0\x9F\x91\x91"] = ":crown:";
    lEmoticonsEmoji["\xF0\x9F\x91\x92"] = ":womans_hat:";
    lEmoticonsEmoji["\xF0\x9F\x91\x93"] = ":eyeglasses:";
    lEmoticonsEmoji["\xF0\x9F\x91\x94"] = ":necktie:";
    lEmoticonsEmoji["\xF0\x9F\x91\x95"] = ":shirt:";
    lEmoticonsEmoji["\xF0\x9F\x91\x96"] = ":jeans:";
    lEmoticonsEmoji["\xF0\x9F\x91\x97"] = ":dress:";
    lEmoticonsEmoji["\xF0\x9F\x91\x98"] = ":kimono:";
    lEmoticonsEmoji["\xF0\x9F\x91\x99"] = ":bikini:";
    lEmoticonsEmoji["\xF0\x9F\x91\x9A"] = ":womans_clothes:";
    lEmoticonsEmoji["\xF0\x9F\x91\x9B"] = ":purse:";
    lEmoticonsEmoji["\xF0\x9F\x91\x9C"] = ":handbag:";
    lEmoticonsEmoji["\xF0\x9F\x91\x9D"] = ":pouch:";
    lEmoticonsEmoji["\xF0\x9F\x91\x9E"] = ":mans_shoe:";
    lEmoticonsEmoji["\xF0\x9F\x91\x9F"] = ":athletic_shoe:";
    lEmoticonsEmoji["\xF0\x9F\x91\xA0"] = ":high_heel:";
    lEmoticonsEmoji["\xF0\x9F\x91\xA1"] = ":sandal:";
    lEmoticonsEmoji["\xF0\x9F\x91\xA2"] = ":boot:";
    lEmoticonsEmoji["\xF0\x9F\x91\xA3"] = ":footprints:";
    lEmoticonsEmoji["\xF0\x9F\x91\xA4"] = ":bust_in_silhouette:";
    lEmoticonsEmoji["\xF0\x9F\x91\xA6"] = ":boy:";
    lEmoticonsEmoji["\xF0\x9F\x91\xA7"] = ":girl:";
    lEmoticonsEmoji["\xF0\x9F\x91\xA8"] = ":man:";
    lEmoticonsEmoji["\xF0\x9F\x91\xA9"] = ":woman:";
    lEmoticonsEmoji["\xF0\x9F\x91\xAA"] = ":family:";
    lEmoticonsEmoji["\xF0\x9F\x91\xAB"] = ":couple:";
    lEmoticonsEmoji["\xF0\x9F\x91\xAE"] = ":cop:";
    lEmoticonsEmoji["\xF0\x9F\x91\xAF"] = ":dancers:";
    lEmoticonsEmoji["\xF0\x9F\x91\xB0"] = ":bride_with_veil:";
    lEmoticonsEmoji["\xF0\x9F\x91\xB1"] = ":person_with_blond_hair:";
    lEmoticonsEmoji["\xF0\x9F\x91\xB2"] = ":man_with_gua_pi_mao:";
    lEmoticonsEmoji["\xF0\x9F\x91\xB3"] = ":man_with_turban:";
    lEmoticonsEmoji["\xF0\x9F\x91\xB4"] = ":older_man:";
    lEmoticonsEmoji["\xF0\x9F\x91\xB5"] = ":older_woman:";
    lEmoticonsEmoji["\xF0\x9F\x91\xB5"] = ":grandma:";
    lEmoticonsEmoji["\xF0\x9F\x91\xB6"] = ":baby:";
    lEmoticonsEmoji["\xF0\x9F\x91\xB7"] = ":construction_worker:";
    lEmoticonsEmoji["\xF0\x9F\x91\xB8"] = ":princess:";
    lEmoticonsEmoji["\xF0\x9F\x91\xB9"] = ":japanese_ogre:";
    lEmoticonsEmoji["\xF0\x9F\x91\xBA"] = ":japanese_goblin:";
    lEmoticonsEmoji["\xF0\x9F\x91\xBB"] = ":ghost:";
    lEmoticonsEmoji["\xF0\x9F\x91\xBC"] = ":angel:";
    lEmoticonsEmoji["\xF0\x9F\x91\xBD"] = ":alien:";
    lEmoticonsEmoji["\xF0\x9F\x91\xBE"] = ":space_invader:";
    lEmoticonsEmoji["\xF0\x9F\x91\xBF"] = ":imp:";
    lEmoticonsEmoji["\xF0\x9F\x92\x80"] = ":skull:";
    lEmoticonsEmoji["\xF0\x9F\x92\x80"] = ":skeleton:";
    lEmoticonsEmoji["\xF0\x9F\x93\x87"] = ":card_index:";
    lEmoticonsEmoji["\xF0\x9F\x92\x81"] = ":information_desk_person:";
    lEmoticonsEmoji["\xF0\x9F\x92\x82"] = ":guardsman:";
    lEmoticonsEmoji["\xF0\x9F\x92\x83"] = ":dancer:";
    lEmoticonsEmoji["\xF0\x9F\x92\x84"] = ":lipstick:";
    lEmoticonsEmoji["\xF0\x9F\x92\x85"] = ":nail_care:";
    lEmoticonsEmoji["\xF0\x9F\x93\x92"] = ":ledger:";
    lEmoticonsEmoji["\xF0\x9F\x92\x86"] = ":massage:";
    lEmoticonsEmoji["\xF0\x9F\x93\x93"] = ":notebook:";
    lEmoticonsEmoji["\xF0\x9F\x92\x87"] = ":haircut:";
    lEmoticonsEmoji["\xF0\x9F\x93\x94"] = ":notebook_with_decorative_cover:";
    lEmoticonsEmoji["\xF0\x9F\x92\x88"] = ":barber:";
    lEmoticonsEmoji["\xF0\x9F\x93\x95"] = ":closed_book:";
    lEmoticonsEmoji["\xF0\x9F\x92\x89"] = ":syringe:";
    lEmoticonsEmoji["\xF0\x9F\x93\x96"] = ":book:";
    lEmoticonsEmoji["\xF0\x9F\x92\x8A"] = ":pill:";
    lEmoticonsEmoji["\xF0\x9F\x93\x97"] = ":green_book:";
    lEmoticonsEmoji["\xF0\x9F\x92\x8B"] = ":kiss:";
    lEmoticonsEmoji["\xF0\x9F\x93\x98"] = ":blue_book:";
    lEmoticonsEmoji["\xF0\x9F\x92\x8C"] = ":love_letter:";
    lEmoticonsEmoji["\xF0\x9F\x93\x99"] = ":orange_book:";
    lEmoticonsEmoji["\xF0\x9F\x92\x8D"] = ":ring:";
    lEmoticonsEmoji["\xF0\x9F\x93\x9A"] = ":books:";
    lEmoticonsEmoji["\xF0\x9F\x92\x8E"] = ":gem:";
    lEmoticonsEmoji["\xF0\x9F\x93\x9B"] = ":name_badge:";
    lEmoticonsEmoji["\xF0\x9F\x92\x8F"] = ":couplekiss:";
    lEmoticonsEmoji["\xF0\x9F\x93\x9C"] = ":scroll:";
    lEmoticonsEmoji["\xF0\x9F\x92\x90"] = ":bouquet:";
    lEmoticonsEmoji["\xF0\x9F\x93\x9D"] = ":pencil:";
    lEmoticonsEmoji["\xF0\x9F\x92\x91"] = ":couple_with_heart:";
    lEmoticonsEmoji["\xF0\x9F\x93\x9E"] = ":telephone_receiver:";
    lEmoticonsEmoji["\xF0\x9F\x92\x92"] = ":wedding:";
    lEmoticonsEmoji["\xF0\x9F\x93\x9F"] = ":pager:";
    lEmoticonsEmoji["\xF0\x9F\x93\xA0"] = ":fax:";
    lEmoticonsEmoji["\xF0\x9F\x92\x93"] = ":heartbeat:";
    lEmoticonsEmoji["\xF0\x9F\x93\xA1"] = ":satellite:";
    lEmoticonsEmoji["\xF0\x9F\x93\xA2"] = ":loudspeaker:";
    lEmoticonsEmoji["\xF0\x9F\x92\x94"] = ":broken_heart:";
    lEmoticonsEmoji["\xF0\x9F\x93\xA3"] = ":mega:";
    lEmoticonsEmoji["\xF0\x9F\x93\xA4"] = ":outbox_tray:";
    lEmoticonsEmoji["\xF0\x9F\x92\x95"] = ":two_hearts:";
    lEmoticonsEmoji["\xF0\x9F\x93\xA5"] = ":inbox_tray:";
    lEmoticonsEmoji["\xF0\x9F\x93\xA6"] = ":package:";
    lEmoticonsEmoji["\xF0\x9F\x92\x96"] = ":sparkling_heart:";
    lEmoticonsEmoji["\xF0\x9F\x93\xA7"] = ":e-mail:";
    lEmoticonsEmoji["\xF0\x9F\x93\xA7"] = ":email:";
    lEmoticonsEmoji["\xF0\x9F\x93\xA8"] = ":incoming_envelope:";
    lEmoticonsEmoji["\xF0\x9F\x92\x97"] = ":heartpulse:";
    lEmoticonsEmoji["\xF0\x9F\x93\xA9"] = ":envelope_with_arrow:";
    lEmoticonsEmoji["\xF0\x9F\x93\xAA"] = ":mailbox_closed:";
    lEmoticonsEmoji["\xF0\x9F\x92\x98"] = ":cupid:";
    lEmoticonsEmoji["\xF0\x9F\x93\xAB"] = ":mailbox:";
    lEmoticonsEmoji["\xF0\x9F\x93\xAE"] = ":postbox:";
    lEmoticonsEmoji["\xF0\x9F\x92\x99"] = ":blue_heart:";
    lEmoticonsEmoji["\xF0\x9F\x93\xB0"] = ":newspaper:";
    lEmoticonsEmoji["\xF0\x9F\x93\xB1"] = ":iphone:";
    lEmoticonsEmoji["\xF0\x9F\x92\x9A"] = ":green_heart:";
    lEmoticonsEmoji["\xF0\x9F\x93\xB2"] = ":calling:";
    lEmoticonsEmoji["\xF0\x9F\x93\xB3"] = ":vibration_mode:";
    lEmoticonsEmoji["\xF0\x9F\x92\x9B"] = ":yellow_heart:";
    lEmoticonsEmoji["\xF0\x9F\x93\xB4"] = ":mobile_phone_off:";
    lEmoticonsEmoji["\xF0\x9F\x93\xB6"] = ":signal_strength:";
    lEmoticonsEmoji["\xF0\x9F\x92\x9C"] = ":purple_heart:";
    lEmoticonsEmoji["\xF0\x9F\x93\xB7"] = ":camera:";
    lEmoticonsEmoji["\xF0\x9F\x93\xB9"] = ":video_camera:";
    lEmoticonsEmoji["\xF0\x9F\x92\x9D"] = ":gift_heart:";
    lEmoticonsEmoji["\xF0\x9F\x93\xBA"] = ":tv:";
    lEmoticonsEmoji["\xF0\x9F\x93\xBB"] = ":radio:";
    lEmoticonsEmoji["\xF0\x9F\x92\x9E"] = ":revolving_hearts:";
    lEmoticonsEmoji["\xF0\x9F\x93\xBC"] = ":vhs:";
    lEmoticonsEmoji["\xF0\x9F\x94\x83"] = ":arrows_clockwise:";
    lEmoticonsEmoji["\xF0\x9F\x92\x9F"] = ":heart_decoration:";
    lEmoticonsEmoji["\xF0\x9F\x94\x8A"] = ":loud_sound:";
    lEmoticonsEmoji["\xF0\x9F\x94\x8B"] = ":battery:";
    lEmoticonsEmoji["\xF0\x9F\x92\xA0"] = ":diamond_shape_with_a_dot_inside:";
    lEmoticonsEmoji["\xF0\x9F\x94\x8C"] = ":electric_plug:";
    lEmoticonsEmoji["\xF0\x9F\x94\x8D"] = ":mag:";
    lEmoticonsEmoji["\xF0\x9F\x92\xA1"] = ":bulb:";
    lEmoticonsEmoji["\xF0\x9F\x94\x8E"] = ":mag_right:";
    lEmoticonsEmoji["\xF0\x9F\x94\x8F"] = ":lock_with_ink_pen:";
    lEmoticonsEmoji["\xF0\x9F\x92\xA2"] = ":anger:";
    lEmoticonsEmoji["\xF0\x9F\x94\x90"] = ":closed_lock_with_key:";
    lEmoticonsEmoji["\xF0\x9F\x94\x91"] = ":key:";
    lEmoticonsEmoji["\xF0\x9F\x92\xA3"] = ":bomb:";
    lEmoticonsEmoji["\xF0\x9F\x94\x92"] = ":lock:";
    lEmoticonsEmoji["\xF0\x9F\x94\x93"] = ":unlock:";
    lEmoticonsEmoji["\xF0\x9F\x92\xA4"] = ":zzz:";
    lEmoticonsEmoji["\xF0\x9F\x94\x94"] = ":bell:";
    lEmoticonsEmoji["\xF0\x9F\x94\x96"] = ":bookmark:";
    lEmoticonsEmoji["\xF0\x9F\x92\xA5"] = ":boom:";
    lEmoticonsEmoji["\xF0\x9F\x94\x97"] = ":link:";
    lEmoticonsEmoji["\xF0\x9F\x94\x98"] = ":radio_button:";
    lEmoticonsEmoji["\xF0\x9F\x92\xA6"] = ":sweat_drops:";
    lEmoticonsEmoji["\xF0\x9F\x94\x99"] = ":back:";
    lEmoticonsEmoji["\xF0\x9F\x94\x9A"] = ":end:";
    lEmoticonsEmoji["\xF0\x9F\x92\xA7"] = ":droplet:";
    lEmoticonsEmoji["\xF0\x9F\x94\x9B"] = ":on:";
    lEmoticonsEmoji["\xF0\x9F\x94\x9C"] = ":soon:";
    lEmoticonsEmoji["\xF0\x9F\x92\xA8"] = ":dash:";
    lEmoticonsEmoji["\xF0\x9F\x94\x9D"] = ":top:";
    lEmoticonsEmoji["\xF0\x9F\x94\x9E"] = ":underage:";
    lEmoticonsEmoji["\xF0\x9F\x92\xA9"] = ":poop:";
    lEmoticonsEmoji["\xF0\x9F\x92\xA9"] = ":shit:";
    lEmoticonsEmoji["\xF0\x9F\x92\xA9"] = ":hankey:";
    lEmoticonsEmoji["\xF0\x9F\x92\xA9"] = ":poo:";
    lEmoticonsEmoji["\xF0\x9F\x94\x9F"] = ":ten:";
    lEmoticonsEmoji["\xF0\x9F\x92\xAA"] = ":muscle:";
    lEmoticonsEmoji["\xF0\x9F\x94\xA0"] = ":capital_abcd:";
    lEmoticonsEmoji["\xF0\x9F\x94\xA1"] = ":abcd:";
    lEmoticonsEmoji["\xF0\x9F\x92\xAB"] = ":dizzy:";
    lEmoticonsEmoji["\xF0\x9F\x94\xA2"] = ":1234:";
    lEmoticonsEmoji["\xF0\x9F\x94\xA3"] = ":symbols:";
    lEmoticonsEmoji["\xF0\x9F\x92\xAC"] = ":speech_balloon:";
    lEmoticonsEmoji["\xF0\x9F\x94\xA4"] = ":abc:";
    lEmoticonsEmoji["\xF0\x9F\x94\xA5"] = ":fire:";
    lEmoticonsEmoji["\xF0\x9F\x94\xA5"] = ":flame:";
    lEmoticonsEmoji["\xF0\x9F\x92\xAE"] = ":white_flower:";
    lEmoticonsEmoji["\xF0\x9F\x94\xA6"] = ":flashlight:";
    lEmoticonsEmoji["\xF0\x9F\x94\xA7"] = ":wrench:";
    lEmoticonsEmoji["\xF0\x9F\x92\xAF"] = ":100:";
    lEmoticonsEmoji["\xF0\x9F\x94\xA8"] = ":hammer:";
    lEmoticonsEmoji["\xF0\x9F\x94\xA9"] = ":nut_and_bolt:";
    lEmoticonsEmoji["\xF0\x9F\x92\xB0"] = ":moneybag:";
    lEmoticonsEmoji["\xF0\x9F\x94\xAA"] = ":knife:";
    lEmoticonsEmoji["\xF0\x9F\x94\xAB"] = ":gun:";
    lEmoticonsEmoji["\xF0\x9F\x92\xB1"] = ":currency_exchange:";
    lEmoticonsEmoji["\xF0\x9F\x94\xAE"] = ":crystal_ball:";
    lEmoticonsEmoji["\xF0\x9F\x92\xB2"] = ":heavy_dollar_sign:";
    lEmoticonsEmoji["\xF0\x9F\x94\xAF"] = ":six_pointed_star:";
    lEmoticonsEmoji["\xF0\x9F\x92\xB3"] = ":credit_card:";
    lEmoticonsEmoji["\xF0\x9F\x94\xB0"] = ":beginner:";
    lEmoticonsEmoji["\xF0\x9F\x94\xB1"] = ":trident:";
    lEmoticonsEmoji["\xF0\x9F\x92\xB4"] = ":yen:";
    lEmoticonsEmoji["\xF0\x9F\x94\xB2"] = ":black_square_button:";
    lEmoticonsEmoji["\xF0\x9F\x94\xB3"] = ":white_square_button:";
    lEmoticonsEmoji["\xF0\x9F\x92\xB5"] = ":dollar:";
    lEmoticonsEmoji["\xF0\x9F\x94\xB4"] = ":red_circle:";
    lEmoticonsEmoji["\xF0\x9F\x94\xB5"] = ":large_blue_circle:";
    lEmoticonsEmoji["\xF0\x9F\x92\xB8"] = ":money_with_wings:";
    lEmoticonsEmoji["\xF0\x9F\x94\xB6"] = ":large_orange_diamond:";
    lEmoticonsEmoji["\xF0\x9F\x94\xB7"] = ":large_blue_diamond:";
    lEmoticonsEmoji["\xF0\x9F\x92\xB9"] = ":chart:";
    lEmoticonsEmoji["\xF0\x9F\x94\xB8"] = ":small_orange_diamond:";
    lEmoticonsEmoji["\xF0\x9F\x94\xB9"] = ":small_blue_diamond:";
    lEmoticonsEmoji["\xF0\x9F\x92\xBA"] = ":seat:";
    lEmoticonsEmoji["\xF0\x9F\x94\xBA"] = ":small_red_triangle:";
    lEmoticonsEmoji["\xF0\x9F\x94\xBB"] = ":small_red_triangle_down:";
    lEmoticonsEmoji["\xF0\x9F\x92\xBB"] = ":computer:";
    lEmoticonsEmoji["\xF0\x9F\x94\xBC"] = ":arrow_up_small:";
    lEmoticonsEmoji["\xF0\x9F\x92\xBC"] = ":briefcase:";
    lEmoticonsEmoji["\xF0\x9F\x94\xBD"] = ":arrow_down_small:";
    lEmoticonsEmoji["\xF0\x9F\x95\x90"] = ":clock1:";
    lEmoticonsEmoji["\xF0\x9F\x92\xBD"] = ":minidisc:";
    lEmoticonsEmoji["\xF0\x9F\x95\x91"] = ":clock2:";
    lEmoticonsEmoji["\xF0\x9F\x92\xBE"] = ":floppy_disk:";
    lEmoticonsEmoji["\xF0\x9F\x95\x92"] = ":clock3:";
    lEmoticonsEmoji["\xF0\x9F\x92\xBF"] = ":cd:";
    lEmoticonsEmoji["\xF0\x9F\x95\x93"] = ":clock4:";
    lEmoticonsEmoji["\xF0\x9F\x93\x80"] = ":dvd:";
    lEmoticonsEmoji["\xF0\x9F\x95\x94"] = ":clock5:";
    lEmoticonsEmoji["\xF0\x9F\x95\x95"] = ":clock6:";
    lEmoticonsEmoji["\xF0\x9F\x93\x81"] = ":file_folder:";
    lEmoticonsEmoji["\xF0\x9F\x95\x96"] = ":clock7:";
    lEmoticonsEmoji["\xF0\x9F\x95\x97"] = ":clock8:";
    lEmoticonsEmoji["\xF0\x9F\x93\x82"] = ":open_file_folder:";
    lEmoticonsEmoji["\xF0\x9F\x95\x98"] = ":clock9:";
    lEmoticonsEmoji["\xF0\x9F\x95\x99"] = ":clock10:";
    lEmoticonsEmoji["\xF0\x9F\x93\x83"] = ":page_with_curl:";
    lEmoticonsEmoji["\xF0\x9F\x95\x9A"] = ":clock11:";
    lEmoticonsEmoji["\xF0\x9F\x95\x9B"] = ":clock12:";
    lEmoticonsEmoji["\xF0\x9F\x93\x84"] = ":page_facing_up:";
    lEmoticonsEmoji["\xF0\x9F\x97\xBB"] = ":mount_fuji:";
    lEmoticonsEmoji["\xF0\x9F\x97\xBC"] = ":tokyo_tower:";
    lEmoticonsEmoji["\xF0\x9F\x93\x85"] = ":date:";
    lEmoticonsEmoji["\xF0\x9F\x97\xBD"] = ":statue_of_liberty:";
    lEmoticonsEmoji["\xF0\x9F\x97\xBE"] = ":japan:";
    lEmoticonsEmoji["\xF0\x9F\x93\x86"] = ":calendar:";
    lEmoticonsEmoji["\xF0\x9F\x97\xBF"] = ":moyai:";
    lEmoticonsEmoji["\xF0\x9F\x98\x81"] = ":grin:";
    lEmoticonsEmoji["\xF0\x9F\x98\x82"] = ":joy:";
    lEmoticonsEmoji["\xF0\x9F\x98\x83"] = ":smiley:";
    lEmoticonsEmoji["\xF0\x9F\x93\x88"] = ":chart_with_upwards_trend:";
    lEmoticonsEmoji["\xF0\x9F\x98\x84"] = ":smile:";
    lEmoticonsEmoji["\xF0\x9F\x98\x85"] = ":sweat_smile:";
    lEmoticonsEmoji["\xF0\x9F\x93\x89"] = ":chart_with_downwards_trend:";
    lEmoticonsEmoji["\xF0\x9F\x98\x86"] = ":laughing:";
    lEmoticonsEmoji["\xF0\x9F\x98\x86"] = ":satisfied:";
    lEmoticonsEmoji["\xF0\x9F\x98\x89"] = ":wink:";
    lEmoticonsEmoji["\xF0\x9F\x93\x8A"] = ":bar_chart:";
    lEmoticonsEmoji["\xF0\x9F\x98\x8A"] = ":blush:";
    lEmoticonsEmoji["\xF0\x9F\x98\x8B"] = ":yum:";
    lEmoticonsEmoji["\xF0\x9F\x93\x8B"] = ":clipboard:";
    lEmoticonsEmoji["\xF0\x9F\x98\x8C"] = ":relieved:";
    lEmoticonsEmoji["\xF0\x9F\x98\x8D"] = ":heart_eyes:";
    lEmoticonsEmoji["\xF0\x9F\x93\x8C"] = ":pushpin:";
    lEmoticonsEmoji["\xF0\x9F\x98\x8F"] = ":smirk:";
    lEmoticonsEmoji["\xF0\x9F\x98\x92"] = ":unamused:";
    lEmoticonsEmoji["\xF0\x9F\x93\x8D"] = ":round_pushpin:";
    lEmoticonsEmoji["\xF0\x9F\x98\x93"] = ":sweat:";
    lEmoticonsEmoji["\xF0\x9F\x98\x94"] = ":pensive:";
    lEmoticonsEmoji["\xF0\x9F\x93\x8E"] = ":paperclip:";
    lEmoticonsEmoji["\xF0\x9F\x98\x96"] = ":confounded:";
    lEmoticonsEmoji["\xF0\x9F\x98\x98"] = ":kissing_heart:";
    lEmoticonsEmoji["\xF0\x9F\x93\x8F"] = ":straight_ruler:";
    lEmoticonsEmoji["\xF0\x9F\x98\x9A"] = ":kissing_closed_eyes:";
    lEmoticonsEmoji["\xF0\x9F\x98\x9C"] = ":stuck_out_tongue_winking_eye:";
    lEmoticonsEmoji["\xF0\x9F\x93\x90"] = ":triangular_ruler:";
    lEmoticonsEmoji["\xF0\x9F\x98\x9D"] = ":stuck_out_tongue_closed_eyes:";
    lEmoticonsEmoji["\xF0\x9F\x98\x9E"] = ":disappointed:";
    lEmoticonsEmoji["\xF0\x9F\x93\x91"] = ":bookmark_tabs:";
    lEmoticonsEmoji["\xF0\x9F\x98\xA0"] = ":angry:";
    lEmoticonsEmoji["\xF0\x9F\x98\xA1"] = ":rage:";
    lEmoticonsEmoji["\xF0\x9F\x98\xA2"] = ":cry:";
    lEmoticonsEmoji["\xF0\x9F\x98\xA3"] = ":persevere:";
    lEmoticonsEmoji["\xF0\x9F\x98\xA4"] = ":triumph:";
    lEmoticonsEmoji["\xF0\x9F\x98\xA5"] = ":disappointed_relieved:";
    lEmoticonsEmoji["\xF0\x9F\x98\xA8"] = ":fearful:";
    lEmoticonsEmoji["\xF0\x9F\x98\xA9"] = ":weary:";
    lEmoticonsEmoji["\xF0\x9F\x98\xAA"] = ":sleepy:";
    lEmoticonsEmoji["\xF0\x9F\x98\xAB"] = ":tired_face:";
    lEmoticonsEmoji["\xF0\x9F\x98\xAD"] = ":sob:";
    lEmoticonsEmoji["\xF0\x9F\x98\xB0"] = ":cold_sweat:";
    lEmoticonsEmoji["\xF0\x9F\x98\xB1"] = ":scream:";
    lEmoticonsEmoji["\xF0\x9F\x98\xB2"] = ":astonished:";
    lEmoticonsEmoji["\xF0\x9F\x98\xB3"] = ":flushed:";
    lEmoticonsEmoji["\xF0\x9F\x98\xB5"] = ":dizzy_face:";
    lEmoticonsEmoji["\xF0\x9F\x98\xB7"] = ":mask:";
    lEmoticonsEmoji["\xF0\x9F\x98\xB8"] = ":smile_cat:";
    lEmoticonsEmoji["\xF0\x9F\x98\xB9"] = ":joy_cat:";
    lEmoticonsEmoji["\xF0\x9F\x98\xBA"] = ":smiley_cat:";
    lEmoticonsEmoji["\xF0\x9F\x98\xBB"] = ":heart_eyes_cat:";
    lEmoticonsEmoji["\xF0\x9F\x98\xBC"] = ":smirk_cat:";
    lEmoticonsEmoji["\xF0\x9F\x98\xBD"] = ":kissing_cat:";
    lEmoticonsEmoji["\xF0\x9F\x98\xBE"] = ":pouting_cat:";
    lEmoticonsEmoji["\xF0\x9F\x98\xBF"] = ":crying_cat_face:";
    lEmoticonsEmoji["\xF0\x9F\x99\x80"] = ":scream_cat:";
    lEmoticonsEmoji["\xF0\x9F\x99\x85"] = ":no_good:";
    lEmoticonsEmoji["\xF0\x9F\x99\x86"] = ":ok_woman:";
    lEmoticonsEmoji["\xF0\x9F\x99\x87"] = ":bow:";
    lEmoticonsEmoji["\xF0\x9F\x99\x88"] = ":see_no_evil:";
    lEmoticonsEmoji["\xF0\x9F\x99\x89"] = ":hear_no_evil:";
    lEmoticonsEmoji["\xF0\x9F\x99\x8A"] = ":speak_no_evil:";
    lEmoticonsEmoji["\xF0\x9F\x99\x8B"] = ":raising_hand:";
    lEmoticonsEmoji["\xF0\x9F\x99\x8C"] = ":raised_hands:";
    lEmoticonsEmoji["\xF0\x9F\x99\x8D"] = ":person_frowning:";
    lEmoticonsEmoji["\xF0\x9F\x99\x8E"] = ":person_with_pouting_face:";
    lEmoticonsEmoji["\xF0\x9F\x99\x8F"] = ":pray:";
    lEmoticonsEmoji["\xF0\x9F\x9A\x80"] = ":rocket:";
    lEmoticonsEmoji["\xF0\x9F\x9A\x83"] = ":railway_car:";
    lEmoticonsEmoji["\xF0\x9F\x9A\x84"] = ":bullettrain_side:";
    lEmoticonsEmoji["\xF0\x9F\x9A\x85"] = ":bullettrain_front:";
    lEmoticonsEmoji["\xF0\x9F\x9A\x87"] = ":metro:";
    lEmoticonsEmoji["\xF0\x9F\x9A\x89"] = ":station:";
    lEmoticonsEmoji["\xF0\x9F\x9A\x8C"] = ":bus:";
    lEmoticonsEmoji["\xF0\x9F\x9A\x8F"] = ":busstop:";
    lEmoticonsEmoji["\xF0\x9F\x9A\x91"] = ":ambulance:";
    lEmoticonsEmoji["\xF0\x9F\x9A\x92"] = ":fire_engine:";
    lEmoticonsEmoji["\xF0\x9F\x9A\x93"] = ":police_car:";
    lEmoticonsEmoji["\xF0\x9F\x9A\x95"] = ":taxi:";
    lEmoticonsEmoji["\xF0\x9F\x9A\x97"] = ":red_car:";
    lEmoticonsEmoji["\xF0\x9F\x9A\x99"] = ":blue_car:";
    lEmoticonsEmoji["\xF0\x9F\x9A\x9A"] = ":truck:";
    lEmoticonsEmoji["\xF0\x9F\x9A\xA2"] = ":ship:";
    lEmoticonsEmoji["\xF0\x9F\x9A\xA4"] = ":speedboat:";
    lEmoticonsEmoji["\xF0\x9F\x9A\xA5"] = ":traffic_light:";
    lEmoticonsEmoji["\xF0\x9F\x9A\xA7"] = ":construction:";
    lEmoticonsEmoji["\xF0\x9F\x9A\xA8"] = ":rotating_light:";
    lEmoticonsEmoji["\xF0\x9F\x9A\xA9"] = ":triangular_flag_on_post:";
    lEmoticonsEmoji["\xF0\x9F\x9A\xAA"] = ":door:";
    lEmoticonsEmoji["\xF0\x9F\x9A\xAB"] = ":no_entry_sign:";
    lEmoticonsEmoji["\xF0\x9F\x9A\xAC"] = ":smoking:";
    lEmoticonsEmoji["\xF0\x9F\x9A\xAD"] = ":no_smoking:";
    lEmoticonsEmoji["\xF0\x9F\x9A\xB2"] = ":bike:";
    lEmoticonsEmoji["\xF0\x9F\x9A\xB6"] = ":walking:";
    lEmoticonsEmoji["\xF0\x9F\x9A\xB9"] = ":mens:";
    lEmoticonsEmoji["\xF0\x9F\x9A\xBA"] = ":womens:";
    lEmoticonsEmoji["\xF0\x9F\x9A\xBB"] = ":restroom:";
    lEmoticonsEmoji["\xF0\x9F\x9A\xBC"] = ":baby_symbol:";
    lEmoticonsEmoji["\xF0\x9F\x9A\xBD"] = ":toilet:";
    lEmoticonsEmoji["\xF0\x9F\x9A\xBE"] = ":wc:";
    lEmoticonsEmoji["\xF0\x9F\x9B\x80"] = ":bath:";
    lEmoticonsEmoji["\xF0\x9F\xA4\x98"] = ":metal:";
    lEmoticonsEmoji["\xF0\x9F\xA4\x98"] = ":sign_of_the_horns:";
    lEmoticonsEmoji["\xF0\x9F\x98\x80"] = ":grinning:";
    lEmoticonsEmoji["\xF0\x9F\x98\x87"] = ":innocent:";
    lEmoticonsEmoji["\xF0\x9F\x98\x88"] = ":smiling_imp:";
    lEmoticonsEmoji["\xF0\x9F\x98\x8E"] = ":sunglasses:";
    lEmoticonsEmoji["\xF0\x9F\x98\x90"] = ":neutral_face:";
    lEmoticonsEmoji["\xF0\x9F\x98\x91"] = ":expressionless:";
    lEmoticonsEmoji["\xF0\x9F\x98\x95"] = ":confused:";
    lEmoticonsEmoji["\xF0\x9F\x98\x97"] = ":kissing:";
    lEmoticonsEmoji["\xF0\x9F\x98\x99"] = ":kissing_smiling_eyes:";
    lEmoticonsEmoji["\xF0\x9F\x98\x9B"] = ":stuck_out_tongue:";
    lEmoticonsEmoji["\xF0\x9F\x98\x9F"] = ":worried:";
    lEmoticonsEmoji["\xF0\x9F\x98\xA6"] = ":frowning:";
    lEmoticonsEmoji["\xF0\x9F\x98\xA7"] = ":anguished:";
    lEmoticonsEmoji["\xF0\x9F\x98\xAC"] = ":grimacing:";
    lEmoticonsEmoji["\xF0\x9F\x98\xAE"] = ":open_mouth:";
    lEmoticonsEmoji["\xF0\x9F\x98\xAF"] = ":hushed:";
    lEmoticonsEmoji["\xF0\x9F\x98\xB4"] = ":sleeping:";
    lEmoticonsEmoji["\xF0\x9F\x98\xB6"] = ":no_mouth:";
    lEmoticonsEmoji["\xF0\x9F\x9A\x81"] = ":helicopter:";
    lEmoticonsEmoji["\xF0\x9F\x9A\x82"] = ":steam_locomotive:";
    lEmoticonsEmoji["\xF0\x9F\x9A\x86"] = ":train2:";
    lEmoticonsEmoji["\xF0\x9F\x9A\x88"] = ":light_rail:";
    lEmoticonsEmoji["\xF0\x9F\x9A\x8A"] = ":tram:";
    lEmoticonsEmoji["\xF0\x9F\x9A\x8D"] = ":oncoming_bus:";
    lEmoticonsEmoji["\xF0\x9F\x9A\x8E"] = ":trolleybus:";
    lEmoticonsEmoji["\xF0\x9F\x9A\x90"] = ":minibus:";
    lEmoticonsEmoji["\xF0\x9F\x9A\x94"] = ":oncoming_police_car:";
    lEmoticonsEmoji["\xF0\x9F\x9A\x96"] = ":oncoming_taxi:";
    lEmoticonsEmoji["\xF0\x9F\x9A\x98"] = ":oncoming_automobile:";
    lEmoticonsEmoji["\xF0\x9F\x9A\x9B"] = ":articulated_lorry:";
    lEmoticonsEmoji["\xF0\x9F\x9A\x9C"] = ":tractor:";
    lEmoticonsEmoji["\xF0\x9F\x9A\x9D"] = ":monorail:";
    lEmoticonsEmoji["\xF0\x9F\x9A\x9E"] = ":mountain_railway:";
    lEmoticonsEmoji["\xF0\x9F\x9A\x9F"] = ":suspension_railway:";
    lEmoticonsEmoji["\xF0\x9F\x9A\xA0"] = ":mountain_cableway:";
    lEmoticonsEmoji["\xF0\x9F\x9A\xA1"] = ":aerial_tramway:";
    lEmoticonsEmoji["\xF0\x9F\x9A\xA3"] = ":rowboat:";
    lEmoticonsEmoji["\xF0\x9F\x9A\xA6"] = ":vertical_traffic_light:";
    lEmoticonsEmoji["\xF0\x9F\x9A\xAE"] = ":put_litter_in_its_place:";
    lEmoticonsEmoji["\xF0\x9F\x9A\xAF"] = ":do_not_litter:";
    lEmoticonsEmoji["\xF0\x9F\x9A\xB0"] = ":potable_water:";
    lEmoticonsEmoji["\xF0\x9F\x9A\xB1"] = ":non-potable_water:";
    lEmoticonsEmoji["\xF0\x9F\x9A\xB3"] = ":no_bicycles:";
    lEmoticonsEmoji["\xF0\x9F\x9A\xB4"] = ":bicyclist:";
    lEmoticonsEmoji["\xF0\x9F\x9A\xB5"] = ":mountain_bicyclist:";
    lEmoticonsEmoji["\xF0\x9F\x9A\xB7"] = ":no_pedestrians:";
    lEmoticonsEmoji["\xF0\x9F\x9A\xB8"] = ":children_crossing:";
    lEmoticonsEmoji["\xF0\x9F\x9A\xBF"] = ":shower:";
    lEmoticonsEmoji["\xF0\x9F\x9B\x81"] = ":bathtub:";
    lEmoticonsEmoji["\xF0\x9F\x9B\x82"] = ":passport_control:";
    lEmoticonsEmoji["\xF0\x9F\x9B\x83"] = ":customs:";
    lEmoticonsEmoji["\xF0\x9F\x9B\x84"] = ":baggage_claim:";
    lEmoticonsEmoji["\xF0\x9F\x9B\x85"] = ":left_luggage:";
    lEmoticonsEmoji["\xF0\x9F\x8C\x8D"] = ":earth_africa:";
    lEmoticonsEmoji["\xF0\x9F\x8C\x8E"] = ":earth_americas:";
    lEmoticonsEmoji["\xF0\x9F\x8C\x90"] = ":globe_with_meridians:";
    lEmoticonsEmoji["\xF0\x9F\x8C\x92"] = ":waxing_crescent_moon:";
    lEmoticonsEmoji["\xF0\x9F\x8C\x96"] = ":waning_gibbous_moon:";
    lEmoticonsEmoji["\xF0\x9F\x8C\x97"] = ":last_quarter_moon:";
    lEmoticonsEmoji["\xF0\x9F\x8C\x98"] = ":waning_crescent_moon:";
    lEmoticonsEmoji["\xF0\x9F\x8C\x9A"] = ":new_moon_with_face:";
    lEmoticonsEmoji["\xF0\x9F\x8C\x9C"] = ":last_quarter_moon_with_face:";
    lEmoticonsEmoji["\xF0\x9F\x8C\x9D"] = ":full_moon_with_face:";
    lEmoticonsEmoji["\xF0\x9F\x8C\x9E"] = ":sun_with_face:";
    lEmoticonsEmoji["\xF0\x9F\x8C\xB2"] = ":evergreen_tree:";
    lEmoticonsEmoji["\xF0\x9F\x8C\xB3"] = ":deciduous_tree:";
    lEmoticonsEmoji["\xF0\x9F\x8D\x8B"] = ":lemon:";
    lEmoticonsEmoji["\xF0\x9F\x8D\x90"] = ":pear:";
    lEmoticonsEmoji["\xF0\x9F\x8D\xBC"] = ":baby_bottle:";
    lEmoticonsEmoji["\xF0\x9F\x8F\x87"] = ":horse_racing:";
    lEmoticonsEmoji["\xF0\x9F\x8F\x89"] = ":rugby_football:";
    lEmoticonsEmoji["\xF0\x9F\x8F\xA4"] = ":european_post_office:";
    lEmoticonsEmoji["\xF0\x9F\x90\x80"] = ":rat:";
    lEmoticonsEmoji["\xF0\x9F\x90\x81"] = ":mouse2:";
    lEmoticonsEmoji["\xF0\x9F\x90\x82"] = ":ox:";
    lEmoticonsEmoji["\xF0\x9F\x90\x83"] = ":water_buffalo:";
    lEmoticonsEmoji["\xF0\x9F\x90\x84"] = ":cow2:";
    lEmoticonsEmoji["\xF0\x9F\x90\x85"] = ":tiger2:";
    lEmoticonsEmoji["\xF0\x9F\x90\x86"] = ":leopard:";
    lEmoticonsEmoji["\xF0\x9F\x90\x87"] = ":rabbit2:";
    lEmoticonsEmoji["\xF0\x9F\x90\x88"] = ":cat2:";
    lEmoticonsEmoji["\xF0\x9F\x90\x89"] = ":dragon:";
    lEmoticonsEmoji["\xF0\x9F\x90\x8A"] = ":crocodile:";
    lEmoticonsEmoji["\xF0\x9F\x90\x8B"] = ":whale2:";
    lEmoticonsEmoji["\xF0\x9F\x90\x8F"] = ":ram:";
    lEmoticonsEmoji["\xF0\x9F\x90\x90"] = ":goat:";
    lEmoticonsEmoji["\xF0\x9F\x90\x93"] = ":rooster:";
    lEmoticonsEmoji["\xF0\x9F\x90\x95"] = ":dog2:";
    lEmoticonsEmoji["\xF0\x9F\x90\x96"] = ":pig2:";
    lEmoticonsEmoji["\xF0\x9F\x90\xAA"] = ":dromedary_camel:";
    lEmoticonsEmoji["\xF0\x9F\x91\xA5"] = ":busts_in_silhouette:";
    lEmoticonsEmoji["\xF0\x9F\x91\xAC"] = ":two_men_holding_hands:";
    lEmoticonsEmoji["\xF0\x9F\x91\xAD"] = ":two_women_holding_hands:";
    lEmoticonsEmoji["\xF0\x9F\x92\xAD"] = ":thought_balloon:";
    lEmoticonsEmoji["\xF0\x9F\x92\xB6"] = ":euro:";
    lEmoticonsEmoji["\xF0\x9F\x92\xB7"] = ":pound:";
    lEmoticonsEmoji["\xF0\x9F\x93\xAC"] = ":mailbox_with_mail:";
    lEmoticonsEmoji["\xF0\x9F\x93\xAD"] = ":mailbox_with_no_mail:";
    lEmoticonsEmoji["\xF0\x9F\x93\xAF"] = ":postal_horn:";
    lEmoticonsEmoji["\xF0\x9F\x93\xB5"] = ":no_mobile_phones:";
    lEmoticonsEmoji["\xF0\x9F\x94\x80"] = ":twisted_rightwards_arrows:";
    lEmoticonsEmoji["\xF0\x9F\x94\x81"] = ":repeat:";
    lEmoticonsEmoji["\xF0\x9F\x94\x82"] = ":repeat_one:";
    lEmoticonsEmoji["\xF0\x9F\x94\x84"] = ":arrows_counterclockwise:";
    lEmoticonsEmoji["\xF0\x9F\x94\x85"] = ":low_brightness:";
    lEmoticonsEmoji["\xF0\x9F\x94\x86"] = ":high_brightness:";
    lEmoticonsEmoji["\xF0\x9F\x94\x87"] = ":mute:";
    lEmoticonsEmoji["\xF0\x9F\x94\x89"] = ":sound:";
    lEmoticonsEmoji["\xF0\x9F\x94\x95"] = ":no_bell:";
    lEmoticonsEmoji["\xF0\x9F\x94\xAC"] = ":microscope:";
    lEmoticonsEmoji["\xF0\x9F\x94\xAD"] = ":telescope:";
    lEmoticonsEmoji["\xF0\x9F\x95\x9C"] = ":clock130:";
    lEmoticonsEmoji["\xF0\x9F\x95\x9D"] = ":clock230:";
    lEmoticonsEmoji["\xF0\x9F\x95\x9E"] = ":clock330:";
    lEmoticonsEmoji["\xF0\x9F\x95\x9F"] = ":clock430:";
    lEmoticonsEmoji["\xF0\x9F\x95\xA0"] = ":clock530:";
    lEmoticonsEmoji["\xF0\x9F\x95\xA1"] = ":clock630:";
    lEmoticonsEmoji["\xF0\x9F\x95\xA2"] = ":clock730:";
    lEmoticonsEmoji["\xF0\x9F\x95\xA3"] = ":clock830:";
    lEmoticonsEmoji["\xF0\x9F\x95\xA4"] = ":clock930:";
    lEmoticonsEmoji["\xF0\x9F\x95\xA5"] = ":clock1030:";
    lEmoticonsEmoji["\xF0\x9F\x95\xA6"] = ":clock1130:";
    lEmoticonsEmoji["\xF0\x9F\x95\xA7"] = ":clock1230:";
    lEmoticonsEmoji["\xF0\x9F\x94\x88"] = ":speaker:";
    lEmoticonsEmoji["\xF0\x9F\x9A\x8B"] = ":train:";
    lEmoticonsEmoji["\xE2\x9E\xBF"] = ":loop:";
    lEmoticonsEmoji["\xF0\x9F\x87\xA6\xF0\x9F\x87\xAB"] = ":flag_af:";
    lEmoticonsEmoji["\xF0\x9F\x87\xA6\xF0\x9F\x87\xAB"] = ":af:";
    lEmoticonsEmoji["\xF0\x9F\x87\xA6\xF0\x9F\x87\xB1"] = ":flag_al:";
    lEmoticonsEmoji["\xF0\x9F\x87\xA6\xF0\x9F\x87\xB1"] = ":al:";
    lEmoticonsEmoji["\xF0\x9F\x87\xA9\xF0\x9F\x87\xBF"] = ":flag_dz:";
    lEmoticonsEmoji["\xF0\x9F\x87\xA9\xF0\x9F\x87\xBF"] = ":dz:";
    lEmoticonsEmoji["\xF0\x9F\x87\xA6\xF0\x9F\x87\xA9"] = ":flag_ad:";
    lEmoticonsEmoji["\xF0\x9F\x87\xA6\xF0\x9F\x87\xA9"] = ":ad:";
    lEmoticonsEmoji["\xF0\x9F\x87\xA6\xF0\x9F\x87\xB4"] = ":flag_ao:";
    lEmoticonsEmoji["\xF0\x9F\x87\xA6\xF0\x9F\x87\xB4"] = ":ao:";
    lEmoticonsEmoji["\xF0\x9F\x87\xA6\xF0\x9F\x87\xAC"] = ":flag_ag:";
    lEmoticonsEmoji["\xF0\x9F\x87\xA6\xF0\x9F\x87\xAC"] = ":ag:";
    lEmoticonsEmoji["\xF0\x9F\x87\xA6\xF0\x9F\x87\xB7"] = ":flag_ar:";
    lEmoticonsEmoji["\xF0\x9F\x87\xA6\xF0\x9F\x87\xB7"] = ":ar:";
    lEmoticonsEmoji["\xF0\x9F\x87\xA6\xF0\x9F\x87\xB2"] = ":flag_am:";
    lEmoticonsEmoji["\xF0\x9F\x87\xA6\xF0\x9F\x87\xB2"] = ":am:";
    lEmoticonsEmoji["\xF0\x9F\x87\xA6\xF0\x9F\x87\xBA"] = ":flag_au:";
    lEmoticonsEmoji["\xF0\x9F\x87\xA6\xF0\x9F\x87\xBA"] = ":au:";
    lEmoticonsEmoji["\xF0\x9F\x87\xA6\xF0\x9F\x87\xB9"] = ":flag_at:";
    lEmoticonsEmoji["\xF0\x9F\x87\xA6\xF0\x9F\x87\xB9"] = ":at:";
    lEmoticonsEmoji["\xF0\x9F\x87\xA6\xF0\x9F\x87\xBF"] = ":flag_az:";
    lEmoticonsEmoji["\xF0\x9F\x87\xA6\xF0\x9F\x87\xBF"] = ":az:";
    lEmoticonsEmoji["\xF0\x9F\x87\xA7\xF0\x9F\x87\xB8"] = ":flag_bs:";
    lEmoticonsEmoji["\xF0\x9F\x87\xA7\xF0\x9F\x87\xB8"] = ":bs:";
    lEmoticonsEmoji["\xF0\x9F\x87\xA7\xF0\x9F\x87\xAD"] = ":flag_bh:";
    lEmoticonsEmoji["\xF0\x9F\x87\xA7\xF0\x9F\x87\xAD"] = ":bh:";
    lEmoticonsEmoji["\xF0\x9F\x87\xA7\xF0\x9F\x87\xA9"] = ":flag_bd:";
    lEmoticonsEmoji["\xF0\x9F\x87\xA7\xF0\x9F\x87\xA9"] = ":bd:";
    lEmoticonsEmoji["\xF0\x9F\x87\xA7\xF0\x9F\x87\xA7"] = ":flag_bb:";
    lEmoticonsEmoji["\xF0\x9F\x87\xA7\xF0\x9F\x87\xA7"] = ":bb:";
    lEmoticonsEmoji["\xF0\x9F\x87\xA7\xF0\x9F\x87\xBE"] = ":flag_by:";
    lEmoticonsEmoji["\xF0\x9F\x87\xA7\xF0\x9F\x87\xBE"] = ":by:";
    lEmoticonsEmoji["\xF0\x9F\x87\xA7\xF0\x9F\x87\xAA"] = ":flag_be:";
    lEmoticonsEmoji["\xF0\x9F\x87\xA7\xF0\x9F\x87\xAA"] = ":be:";
    lEmoticonsEmoji["\xF0\x9F\x87\xA7\xF0\x9F\x87\xBF"] = ":flag_bz:";
    lEmoticonsEmoji["\xF0\x9F\x87\xA7\xF0\x9F\x87\xBF"] = ":bz:";
    lEmoticonsEmoji["\xF0\x9F\x87\xA7\xF0\x9F\x87\xAF"] = ":flag_bj:";
    lEmoticonsEmoji["\xF0\x9F\x87\xA7\xF0\x9F\x87\xAF"] = ":bj:";
    lEmoticonsEmoji["\xF0\x9F\x87\xA7\xF0\x9F\x87\xB9"] = ":flag_bt:";
    lEmoticonsEmoji["\xF0\x9F\x87\xA7\xF0\x9F\x87\xB9"] = ":bt:";
    lEmoticonsEmoji["\xF0\x9F\x87\xA7\xF0\x9F\x87\xB4"] = ":flag_bo:";
    lEmoticonsEmoji["\xF0\x9F\x87\xA7\xF0\x9F\x87\xB4"] = ":bo:";
    lEmoticonsEmoji["\xF0\x9F\x87\xA7\xF0\x9F\x87\xA6"] = ":flag_ba:";
    lEmoticonsEmoji["\xF0\x9F\x87\xA7\xF0\x9F\x87\xA6"] = ":ba:";
    lEmoticonsEmoji["\xF0\x9F\x87\xA7\xF0\x9F\x87\xBC"] = ":flag_bw:";
    lEmoticonsEmoji["\xF0\x9F\x87\xA7\xF0\x9F\x87\xBC"] = ":bw:";
    lEmoticonsEmoji["\xF0\x9F\x87\xA7\xF0\x9F\x87\xB7"] = ":flag_br:";
    lEmoticonsEmoji["\xF0\x9F\x87\xA7\xF0\x9F\x87\xB7"] = ":br:";
    lEmoticonsEmoji["\xF0\x9F\x87\xA7\xF0\x9F\x87\xB3"] = ":flag_bn:";
    lEmoticonsEmoji["\xF0\x9F\x87\xA7\xF0\x9F\x87\xB3"] = ":bn:";
    lEmoticonsEmoji["\xF0\x9F\x87\xA7\xF0\x9F\x87\xAC"] = ":flag_bg:";
    lEmoticonsEmoji["\xF0\x9F\x87\xA7\xF0\x9F\x87\xAC"] = ":bg:";
    lEmoticonsEmoji["\xF0\x9F\x87\xA7\xF0\x9F\x87\xAB"] = ":flag_bf:";
    lEmoticonsEmoji["\xF0\x9F\x87\xA7\xF0\x9F\x87\xAB"] = ":bf:";
    lEmoticonsEmoji["\xF0\x9F\x87\xA7\xF0\x9F\x87\xAE"] = ":flag_bi:";
    lEmoticonsEmoji["\xF0\x9F\x87\xA7\xF0\x9F\x87\xAE"] = ":bi:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB0\xF0\x9F\x87\xAD"] = ":flag_kh:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB0\xF0\x9F\x87\xAD"] = ":kh:";
    lEmoticonsEmoji["\xF0\x9F\x87\xA8\xF0\x9F\x87\xB2"] = ":flag_cm:";
    lEmoticonsEmoji["\xF0\x9F\x87\xA8\xF0\x9F\x87\xB2"] = ":cm:";
    lEmoticonsEmoji["\xF0\x9F\x87\xA8\xF0\x9F\x87\xA6"] = ":flag_ca:";
    lEmoticonsEmoji["\xF0\x9F\x87\xA8\xF0\x9F\x87\xA6"] = ":ca:";
    lEmoticonsEmoji["\xF0\x9F\x87\xA8\xF0\x9F\x87\xBB"] = ":flag_cv:";
    lEmoticonsEmoji["\xF0\x9F\x87\xA8\xF0\x9F\x87\xBB"] = ":cv:";
    lEmoticonsEmoji["\xF0\x9F\x87\xA8\xF0\x9F\x87\xAB"] = ":flag_cf:";
    lEmoticonsEmoji["\xF0\x9F\x87\xA8\xF0\x9F\x87\xAB"] = ":cf:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB9\xF0\x9F\x87\xA9"] = ":flag_td:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB9\xF0\x9F\x87\xA9"] = ":td:";
    lEmoticonsEmoji["\xF0\x9F\x87\xA8\xF0\x9F\x87\xB1"] = ":flag_cl:";
    lEmoticonsEmoji["\xF0\x9F\x87\xA8\xF0\x9F\x87\xB1"] = ":chile:";
    lEmoticonsEmoji["\xF0\x9F\x87\xA8\xF0\x9F\x87\xB4"] = ":flag_co:";
    lEmoticonsEmoji["\xF0\x9F\x87\xA8\xF0\x9F\x87\xB4"] = ":co:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB0\xF0\x9F\x87\xB2"] = ":flag_km:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB0\xF0\x9F\x87\xB2"] = ":km:";
    lEmoticonsEmoji["\xF0\x9F\x87\xA8\xF0\x9F\x87\xB7"] = ":flag_cr:";
    lEmoticonsEmoji["\xF0\x9F\x87\xA8\xF0\x9F\x87\xB7"] = ":cr:";
    lEmoticonsEmoji["\xF0\x9F\x87\xA8\xF0\x9F\x87\xAE"] = ":flag_ci:";
    lEmoticonsEmoji["\xF0\x9F\x87\xA8\xF0\x9F\x87\xAE"] = ":ci:";
    lEmoticonsEmoji["\xF0\x9F\x87\xAD\xF0\x9F\x87\xB7"] = ":flag_hr:";
    lEmoticonsEmoji["\xF0\x9F\x87\xAD\xF0\x9F\x87\xB7"] = ":hr:";
    lEmoticonsEmoji["\xF0\x9F\x87\xA8\xF0\x9F\x87\xBA"] = ":flag_cu:";
    lEmoticonsEmoji["\xF0\x9F\x87\xA8\xF0\x9F\x87\xBA"] = ":cu:";
    lEmoticonsEmoji["\xF0\x9F\x87\xA8\xF0\x9F\x87\xBE"] = ":flag_cy:";
    lEmoticonsEmoji["\xF0\x9F\x87\xA8\xF0\x9F\x87\xBE"] = ":cy:";
    lEmoticonsEmoji["\xF0\x9F\x87\xA8\xF0\x9F\x87\xBF"] = ":flag_cz:";
    lEmoticonsEmoji["\xF0\x9F\x87\xA8\xF0\x9F\x87\xBF"] = ":cz:";
    lEmoticonsEmoji["\xF0\x9F\x87\xA8\xF0\x9F\x87\xA9"] = ":flag_cd:";
    lEmoticonsEmoji["\xF0\x9F\x87\xA8\xF0\x9F\x87\xA9"] = ":congo:";
    lEmoticonsEmoji["\xF0\x9F\x87\xA9\xF0\x9F\x87\xB0"] = ":flag_dk:";
    lEmoticonsEmoji["\xF0\x9F\x87\xA9\xF0\x9F\x87\xB0"] = ":dk:";
    lEmoticonsEmoji["\xF0\x9F\x87\xA9\xF0\x9F\x87\xAF"] = ":flag_dj:";
    lEmoticonsEmoji["\xF0\x9F\x87\xA9\xF0\x9F\x87\xAF"] = ":dj:";
    lEmoticonsEmoji["\xF0\x9F\x87\xA9\xF0\x9F\x87\xB2"] = ":flag_dm:";
    lEmoticonsEmoji["\xF0\x9F\x87\xA9\xF0\x9F\x87\xB2"] = ":dm:";
    lEmoticonsEmoji["\xF0\x9F\x87\xA9\xF0\x9F\x87\xB4"] = ":flag_do:";
    lEmoticonsEmoji["\xF0\x9F\x87\xA9\xF0\x9F\x87\xB4"] = ":do:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB9\xF0\x9F\x87\xB1"] = ":flag_tl:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB9\xF0\x9F\x87\xB1"] = ":tl:";
    lEmoticonsEmoji["\xF0\x9F\x87\xAA\xF0\x9F\x87\xA8"] = ":flag_ec:";
    lEmoticonsEmoji["\xF0\x9F\x87\xAA\xF0\x9F\x87\xA8"] = ":ec:";
    lEmoticonsEmoji["\xF0\x9F\x87\xAA\xF0\x9F\x87\xAC"] = ":flag_eg:";
    lEmoticonsEmoji["\xF0\x9F\x87\xAA\xF0\x9F\x87\xAC"] = ":eg:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB8\xF0\x9F\x87\xBB"] = ":flag_sv:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB8\xF0\x9F\x87\xBB"] = ":sv:";
    lEmoticonsEmoji["\xF0\x9F\x87\xAC\xF0\x9F\x87\xB6"] = ":flag_gq:";
    lEmoticonsEmoji["\xF0\x9F\x87\xAC\xF0\x9F\x87\xB6"] = ":gq:";
    lEmoticonsEmoji["\xF0\x9F\x87\xAA\xF0\x9F\x87\xB7"] = ":flag_er:";
    lEmoticonsEmoji["\xF0\x9F\x87\xAA\xF0\x9F\x87\xB7"] = ":er:";
    lEmoticonsEmoji["\xF0\x9F\x87\xAA\xF0\x9F\x87\xAA"] = ":flag_ee:";
    lEmoticonsEmoji["\xF0\x9F\x87\xAA\xF0\x9F\x87\xAA"] = ":ee:";
    lEmoticonsEmoji["\xF0\x9F\x87\xAA\xF0\x9F\x87\xB9"] = ":flag_et:";
    lEmoticonsEmoji["\xF0\x9F\x87\xAA\xF0\x9F\x87\xB9"] = ":et:";
    lEmoticonsEmoji["\xF0\x9F\x87\xAB\xF0\x9F\x87\xAF"] = ":flag_fj:";
    lEmoticonsEmoji["\xF0\x9F\x87\xAB\xF0\x9F\x87\xAF"] = ":fj:";
    lEmoticonsEmoji["\xF0\x9F\x87\xAB\xF0\x9F\x87\xAE"] = ":flag_fi:";
    lEmoticonsEmoji["\xF0\x9F\x87\xAB\xF0\x9F\x87\xAE"] = ":fi:";
    lEmoticonsEmoji["\xF0\x9F\x87\xAC\xF0\x9F\x87\xA6"] = ":flag_ga:";
    lEmoticonsEmoji["\xF0\x9F\x87\xAC\xF0\x9F\x87\xA6"] = ":ga:";
    lEmoticonsEmoji["\xF0\x9F\x87\xAC\xF0\x9F\x87\xB2"] = ":flag_gm:";
    lEmoticonsEmoji["\xF0\x9F\x87\xAC\xF0\x9F\x87\xB2"] = ":gm:";
    lEmoticonsEmoji["\xF0\x9F\x87\xAC\xF0\x9F\x87\xAA"] = ":flag_ge:";
    lEmoticonsEmoji["\xF0\x9F\x87\xAC\xF0\x9F\x87\xAA"] = ":ge:";
    lEmoticonsEmoji["\xF0\x9F\x87\xAC\xF0\x9F\x87\xAD"] = ":flag_gh:";
    lEmoticonsEmoji["\xF0\x9F\x87\xAC\xF0\x9F\x87\xAD"] = ":gh:";
    lEmoticonsEmoji["\xF0\x9F\x87\xAC\xF0\x9F\x87\xB7"] = ":flag_gr:";
    lEmoticonsEmoji["\xF0\x9F\x87\xAC\xF0\x9F\x87\xB7"] = ":gr:";
    lEmoticonsEmoji["\xF0\x9F\x87\xAC\xF0\x9F\x87\xA9"] = ":flag_gd:";
    lEmoticonsEmoji["\xF0\x9F\x87\xAC\xF0\x9F\x87\xA9"] = ":gd:";
    lEmoticonsEmoji["\xF0\x9F\x87\xAC\xF0\x9F\x87\xB9"] = ":flag_gt:";
    lEmoticonsEmoji["\xF0\x9F\x87\xAC\xF0\x9F\x87\xB9"] = ":gt:";
    lEmoticonsEmoji["\xF0\x9F\x87\xAC\xF0\x9F\x87\xB3"] = ":flag_gn:";
    lEmoticonsEmoji["\xF0\x9F\x87\xAC\xF0\x9F\x87\xB3"] = ":gn:";
    lEmoticonsEmoji["\xF0\x9F\x87\xAC\xF0\x9F\x87\xBC"] = ":flag_gw:";
    lEmoticonsEmoji["\xF0\x9F\x87\xAC\xF0\x9F\x87\xBC"] = ":gw:";
    lEmoticonsEmoji["\xF0\x9F\x87\xAC\xF0\x9F\x87\xBE"] = ":flag_gy:";
    lEmoticonsEmoji["\xF0\x9F\x87\xAC\xF0\x9F\x87\xBE"] = ":gy:";
    lEmoticonsEmoji["\xF0\x9F\x87\xAD\xF0\x9F\x87\xB9"] = ":flag_ht:";
    lEmoticonsEmoji["\xF0\x9F\x87\xAD\xF0\x9F\x87\xB9"] = ":ht:";
    lEmoticonsEmoji["\xF0\x9F\x87\xAD\xF0\x9F\x87\xB3"] = ":flag_hn:";
    lEmoticonsEmoji["\xF0\x9F\x87\xAD\xF0\x9F\x87\xB3"] = ":hn:";
    lEmoticonsEmoji["\xF0\x9F\x87\xAD\xF0\x9F\x87\xBA"] = ":flag_hu:";
    lEmoticonsEmoji["\xF0\x9F\x87\xAD\xF0\x9F\x87\xBA"] = ":hu:";
    lEmoticonsEmoji["\xF0\x9F\x87\xAE\xF0\x9F\x87\xB8"] = ":flag_is:";
    lEmoticonsEmoji["\xF0\x9F\x87\xAE\xF0\x9F\x87\xB8"] = ":is:";
    lEmoticonsEmoji["\xF0\x9F\x87\xAE\xF0\x9F\x87\xB3"] = ":flag_in:";
    lEmoticonsEmoji["\xF0\x9F\x87\xAE\xF0\x9F\x87\xB3"] = ":in:";
    lEmoticonsEmoji["\xF0\x9F\x87\xAE\xF0\x9F\x87\xA9"] = ":flag_id:";
    lEmoticonsEmoji["\xF0\x9F\x87\xAE\xF0\x9F\x87\xA9"] = ":indonesia:";
    lEmoticonsEmoji["\xF0\x9F\x87\xAE\xF0\x9F\x87\xB7"] = ":flag_ir:";
    lEmoticonsEmoji["\xF0\x9F\x87\xAE\xF0\x9F\x87\xB7"] = ":ir:";
    lEmoticonsEmoji["\xF0\x9F\x87\xAE\xF0\x9F\x87\xB6"] = ":flag_iq:";
    lEmoticonsEmoji["\xF0\x9F\x87\xAE\xF0\x9F\x87\xB6"] = ":iq:";
    lEmoticonsEmoji["\xF0\x9F\x87\xAE\xF0\x9F\x87\xAA"] = ":flag_ie:";
    lEmoticonsEmoji["\xF0\x9F\x87\xAE\xF0\x9F\x87\xAA"] = ":ie:";
    lEmoticonsEmoji["\xF0\x9F\x87\xAE\xF0\x9F\x87\xB1"] = ":flag_il:";
    lEmoticonsEmoji["\xF0\x9F\x87\xAE\xF0\x9F\x87\xB1"] = ":il:";
    lEmoticonsEmoji["\xF0\x9F\x87\xAF\xF0\x9F\x87\xB2"] = ":flag_jm:";
    lEmoticonsEmoji["\xF0\x9F\x87\xAF\xF0\x9F\x87\xB2"] = ":jm:";
    lEmoticonsEmoji["\xF0\x9F\x87\xAF\xF0\x9F\x87\xB4"] = ":flag_jo:";
    lEmoticonsEmoji["\xF0\x9F\x87\xAF\xF0\x9F\x87\xB4"] = ":jo:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB0\xF0\x9F\x87\xBF"] = ":flag_kz:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB0\xF0\x9F\x87\xBF"] = ":kz:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB0\xF0\x9F\x87\xAA"] = ":flag_ke:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB0\xF0\x9F\x87\xAA"] = ":ke:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB0\xF0\x9F\x87\xAE"] = ":flag_ki:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB0\xF0\x9F\x87\xAE"] = ":ki:";
    lEmoticonsEmoji["\xF0\x9F\x87\xBD\xF0\x9F\x87\xB0"] = ":flag_xk:";
    lEmoticonsEmoji["\xF0\x9F\x87\xBD\xF0\x9F\x87\xB0"] = ":xk:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB0\xF0\x9F\x87\xBC"] = ":flag_kw:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB0\xF0\x9F\x87\xBC"] = ":kw:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB0\xF0\x9F\x87\xAC"] = ":flag_kg:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB0\xF0\x9F\x87\xAC"] = ":kg:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB1\xF0\x9F\x87\xA6"] = ":flag_la:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB1\xF0\x9F\x87\xA6"] = ":la:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB1\xF0\x9F\x87\xBB"] = ":flag_lv:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB1\xF0\x9F\x87\xBB"] = ":lv:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB1\xF0\x9F\x87\xA7"] = ":flag_lb:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB1\xF0\x9F\x87\xA7"] = ":lb:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB1\xF0\x9F\x87\xB8"] = ":flag_ls:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB1\xF0\x9F\x87\xB8"] = ":ls:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB1\xF0\x9F\x87\xB7"] = ":flag_lr:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB1\xF0\x9F\x87\xB7"] = ":lr:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB1\xF0\x9F\x87\xBE"] = ":flag_ly:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB1\xF0\x9F\x87\xBE"] = ":ly:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB1\xF0\x9F\x87\xAE"] = ":flag_li:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB1\xF0\x9F\x87\xAE"] = ":li:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB1\xF0\x9F\x87\xB9"] = ":flag_lt:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB1\xF0\x9F\x87\xB9"] = ":lt:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB1\xF0\x9F\x87\xBA"] = ":flag_lu:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB1\xF0\x9F\x87\xBA"] = ":lu:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB2\xF0\x9F\x87\xB0"] = ":flag_mk:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB2\xF0\x9F\x87\xB0"] = ":mk:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB2\xF0\x9F\x87\xAC"] = ":flag_mg:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB2\xF0\x9F\x87\xAC"] = ":mg:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB2\xF0\x9F\x87\xBC"] = ":flag_mw:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB2\xF0\x9F\x87\xBC"] = ":mw:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB2\xF0\x9F\x87\xBE"] = ":flag_my:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB2\xF0\x9F\x87\xBE"] = ":my:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB2\xF0\x9F\x87\xBB"] = ":flag_mv:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB2\xF0\x9F\x87\xBB"] = ":mv:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB2\xF0\x9F\x87\xB1"] = ":flag_ml:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB2\xF0\x9F\x87\xB1"] = ":ml:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB2\xF0\x9F\x87\xB9"] = ":flag_mt:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB2\xF0\x9F\x87\xB9"] = ":mt:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB2\xF0\x9F\x87\xAD"] = ":flag_mh:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB2\xF0\x9F\x87\xAD"] = ":mh:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB2\xF0\x9F\x87\xB7"] = ":flag_mr:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB2\xF0\x9F\x87\xB7"] = ":mr:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB2\xF0\x9F\x87\xBA"] = ":flag_mu:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB2\xF0\x9F\x87\xBA"] = ":mu:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB2\xF0\x9F\x87\xBD"] = ":flag_mx:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB2\xF0\x9F\x87\xBD"] = ":mx:";
    lEmoticonsEmoji["\xF0\x9F\x87\xAB\xF0\x9F\x87\xB2"] = ":flag_fm:";
    lEmoticonsEmoji["\xF0\x9F\x87\xAB\xF0\x9F\x87\xB2"] = ":fm:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB2\xF0\x9F\x87\xA9"] = ":flag_md:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB2\xF0\x9F\x87\xA9"] = ":md:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB2\xF0\x9F\x87\xA8"] = ":flag_mc:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB2\xF0\x9F\x87\xA8"] = ":mc:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB2\xF0\x9F\x87\xB3"] = ":flag_mn:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB2\xF0\x9F\x87\xB3"] = ":mn:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB2\xF0\x9F\x87\xAA"] = ":flag_me:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB2\xF0\x9F\x87\xAA"] = ":me:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB2\xF0\x9F\x87\xA6"] = ":flag_ma:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB2\xF0\x9F\x87\xA6"] = ":ma:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB2\xF0\x9F\x87\xBF"] = ":flag_mz:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB2\xF0\x9F\x87\xBF"] = ":mz:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB2\xF0\x9F\x87\xB2"] = ":flag_mm:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB2\xF0\x9F\x87\xB2"] = ":mm:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB3\xF0\x9F\x87\xA6"] = ":flag_na:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB3\xF0\x9F\x87\xA6"] = ":na:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB3\xF0\x9F\x87\xB7"] = ":flag_nr:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB3\xF0\x9F\x87\xB7"] = ":nr:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB3\xF0\x9F\x87\xB5"] = ":flag_np:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB3\xF0\x9F\x87\xB5"] = ":np:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB3\xF0\x9F\x87\xB1"] = ":flag_nl:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB3\xF0\x9F\x87\xB1"] = ":nl:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB3\xF0\x9F\x87\xBF"] = ":flag_nz:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB3\xF0\x9F\x87\xBF"] = ":nz:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB3\xF0\x9F\x87\xAE"] = ":flag_ni:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB3\xF0\x9F\x87\xAE"] = ":ni:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB3\xF0\x9F\x87\xAA"] = ":flag_ne:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB3\xF0\x9F\x87\xAA"] = ":ne:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB3\xF0\x9F\x87\xAC"] = ":flag_ng:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB3\xF0\x9F\x87\xAC"] = ":nigeria:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB0\xF0\x9F\x87\xB5"] = ":flag_kp:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB0\xF0\x9F\x87\xB5"] = ":kp:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB3\xF0\x9F\x87\xB4"] = ":flag_no:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB3\xF0\x9F\x87\xB4"] = ":no:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB4\xF0\x9F\x87\xB2"] = ":flag_om:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB4\xF0\x9F\x87\xB2"] = ":om:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB5\xF0\x9F\x87\xB0"] = ":flag_pk:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB5\xF0\x9F\x87\xB0"] = ":pk:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB5\xF0\x9F\x87\xBC"] = ":flag_pw:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB5\xF0\x9F\x87\xBC"] = ":pw:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB5\xF0\x9F\x87\xA6"] = ":flag_pa:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB5\xF0\x9F\x87\xA6"] = ":pa:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB5\xF0\x9F\x87\xAC"] = ":flag_pg:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB5\xF0\x9F\x87\xAC"] = ":pg:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB5\xF0\x9F\x87\xBE"] = ":flag_py:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB5\xF0\x9F\x87\xBE"] = ":py:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB5\xF0\x9F\x87\xAA"] = ":flag_pe:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB5\xF0\x9F\x87\xAA"] = ":pe:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB5\xF0\x9F\x87\xAD"] = ":flag_ph:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB5\xF0\x9F\x87\xAD"] = ":ph:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB5\xF0\x9F\x87\xB1"] = ":flag_pl:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB5\xF0\x9F\x87\xB1"] = ":pl:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB5\xF0\x9F\x87\xB9"] = ":flag_pt:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB5\xF0\x9F\x87\xB9"] = ":pt:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB6\xF0\x9F\x87\xA6"] = ":flag_qa:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB6\xF0\x9F\x87\xA6"] = ":qa:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB9\xF0\x9F\x87\xBC"] = ":flag_tw:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB9\xF0\x9F\x87\xBC"] = ":tw:";
    lEmoticonsEmoji["\xF0\x9F\x87\xA8\xF0\x9F\x87\xAC"] = ":flag_cg:";
    lEmoticonsEmoji["\xF0\x9F\x87\xA8\xF0\x9F\x87\xAC"] = ":cg:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB7\xF0\x9F\x87\xB4"] = ":flag_ro:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB7\xF0\x9F\x87\xB4"] = ":ro:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB7\xF0\x9F\x87\xBC"] = ":flag_rw:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB7\xF0\x9F\x87\xBC"] = ":rw:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB0\xF0\x9F\x87\xB3"] = ":flag_kn:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB0\xF0\x9F\x87\xB3"] = ":kn:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB1\xF0\x9F\x87\xA8"] = ":flag_lc:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB1\xF0\x9F\x87\xA8"] = ":lc:";
    lEmoticonsEmoji["\xF0\x9F\x87\xBB\xF0\x9F\x87\xA8"] = ":flag_vc:";
    lEmoticonsEmoji["\xF0\x9F\x87\xBB\xF0\x9F\x87\xA8"] = ":vc:";
    lEmoticonsEmoji["\xF0\x9F\x87\xBC\xF0\x9F\x87\xB8"] = ":flag_ws:";
    lEmoticonsEmoji["\xF0\x9F\x87\xBC\xF0\x9F\x87\xB8"] = ":ws:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB8\xF0\x9F\x87\xB2"] = ":flag_sm:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB8\xF0\x9F\x87\xB2"] = ":sm:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB8\xF0\x9F\x87\xB9"] = ":flag_st:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB8\xF0\x9F\x87\xB9"] = ":st:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB8\xF0\x9F\x87\xA6"] = ":flag_sa:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB8\xF0\x9F\x87\xA6"] = ":saudiarabia:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB8\xF0\x9F\x87\xA6"] = ":saudi:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB8\xF0\x9F\x87\xB3"] = ":flag_sn:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB8\xF0\x9F\x87\xB3"] = ":sn:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB7\xF0\x9F\x87\xB8"] = ":flag_rs:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB7\xF0\x9F\x87\xB8"] = ":rs:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB8\xF0\x9F\x87\xA8"] = ":flag_sc:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB8\xF0\x9F\x87\xA8"] = ":sc:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB8\xF0\x9F\x87\xB1"] = ":flag_sl:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB8\xF0\x9F\x87\xB1"] = ":sl:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB8\xF0\x9F\x87\xAC"] = ":flag_sg:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB8\xF0\x9F\x87\xAC"] = ":sg:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB8\xF0\x9F\x87\xB0"] = ":flag_sk:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB8\xF0\x9F\x87\xB0"] = ":sk:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB8\xF0\x9F\x87\xAE"] = ":flag_si:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB8\xF0\x9F\x87\xAE"] = ":si:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB8\xF0\x9F\x87\xA7"] = ":flag_sb:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB8\xF0\x9F\x87\xA7"] = ":sb:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB8\xF0\x9F\x87\xB4"] = ":flag_so:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB8\xF0\x9F\x87\xB4"] = ":so:";
    lEmoticonsEmoji["\xF0\x9F\x87\xBF\xF0\x9F\x87\xA6"] = ":flag_za:";
    lEmoticonsEmoji["\xF0\x9F\x87\xBF\xF0\x9F\x87\xA6"] = ":za:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB1\xF0\x9F\x87\xB0"] = ":flag_lk:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB1\xF0\x9F\x87\xB0"] = ":lk:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB8\xF0\x9F\x87\xA9"] = ":flag_sd:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB8\xF0\x9F\x87\xA9"] = ":sd:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB8\xF0\x9F\x87\xB7"] = ":flag_sr:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB8\xF0\x9F\x87\xB7"] = ":sr:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB8\xF0\x9F\x87\xBF"] = ":flag_sz:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB8\xF0\x9F\x87\xBF"] = ":sz:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB8\xF0\x9F\x87\xAA"] = ":flag_se:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB8\xF0\x9F\x87\xAA"] = ":se:";
    lEmoticonsEmoji["\xF0\x9F\x87\xA8\xF0\x9F\x87\xAD"] = ":flag_ch:";
    lEmoticonsEmoji["\xF0\x9F\x87\xA8\xF0\x9F\x87\xAD"] = ":ch:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB8\xF0\x9F\x87\xBE"] = ":flag_sy:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB8\xF0\x9F\x87\xBE"] = ":sy:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB9\xF0\x9F\x87\xAF"] = ":flag_tj:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB9\xF0\x9F\x87\xAF"] = ":tj:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB9\xF0\x9F\x87\xBF"] = ":flag_tz:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB9\xF0\x9F\x87\xBF"] = ":tz:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB9\xF0\x9F\x87\xAD"] = ":flag_th:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB9\xF0\x9F\x87\xAD"] = ":th:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB9\xF0\x9F\x87\xAC"] = ":flag_tg:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB9\xF0\x9F\x87\xAC"] = ":tg:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB9\xF0\x9F\x87\xB4"] = ":flag_to:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB9\xF0\x9F\x87\xB4"] = ":to:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB9\xF0\x9F\x87\xB9"] = ":flag_tt:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB9\xF0\x9F\x87\xB9"] = ":tt:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB9\xF0\x9F\x87\xB3"] = ":flag_tn:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB9\xF0\x9F\x87\xB3"] = ":tn:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB9\xF0\x9F\x87\xB7"] = ":flag_tr:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB9\xF0\x9F\x87\xB7"] = ":tr:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB9\xF0\x9F\x87\xB2"] = ":flag_tm:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB9\xF0\x9F\x87\xB2"] = ":turkmenistan:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB9\xF0\x9F\x87\xBB"] = ":flag_tv:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB9\xF0\x9F\x87\xBB"] = ":tuvalu:";
    lEmoticonsEmoji["\xF0\x9F\x87\xBA\xF0\x9F\x87\xAC"] = ":flag_ug:";
    lEmoticonsEmoji["\xF0\x9F\x87\xBA\xF0\x9F\x87\xAC"] = ":ug:";
    lEmoticonsEmoji["\xF0\x9F\x87\xBA\xF0\x9F\x87\xA6"] = ":flag_ua:";
    lEmoticonsEmoji["\xF0\x9F\x87\xBA\xF0\x9F\x87\xA6"] = ":ua:";
    lEmoticonsEmoji["\xF0\x9F\x87\xA6\xF0\x9F\x87\xAA"] = ":flag_ae:";
    lEmoticonsEmoji["\xF0\x9F\x87\xA6\xF0\x9F\x87\xAA"] = ":ae:";
    lEmoticonsEmoji["\xF0\x9F\x87\xBA\xF0\x9F\x87\xBE"] = ":flag_uy:";
    lEmoticonsEmoji["\xF0\x9F\x87\xBA\xF0\x9F\x87\xBE"] = ":uy:";
    lEmoticonsEmoji["\xF0\x9F\x87\xBA\xF0\x9F\x87\xBF"] = ":flag_uz:";
    lEmoticonsEmoji["\xF0\x9F\x87\xBA\xF0\x9F\x87\xBF"] = ":uz:";
    lEmoticonsEmoji["\xF0\x9F\x87\xBB\xF0\x9F\x87\xBA"] = ":flag_vu:";
    lEmoticonsEmoji["\xF0\x9F\x87\xBB\xF0\x9F\x87\xBA"] = ":vu:";
    lEmoticonsEmoji["\xF0\x9F\x87\xBB\xF0\x9F\x87\xA6"] = ":flag_va:";
    lEmoticonsEmoji["\xF0\x9F\x87\xBB\xF0\x9F\x87\xA6"] = ":va:";
    lEmoticonsEmoji["\xF0\x9F\x87\xBB\xF0\x9F\x87\xAA"] = ":flag_ve:";
    lEmoticonsEmoji["\xF0\x9F\x87\xBB\xF0\x9F\x87\xAA"] = ":ve:";
    lEmoticonsEmoji["\xF0\x9F\x87\xBB\xF0\x9F\x87\xB3"] = ":flag_vn:";
    lEmoticonsEmoji["\xF0\x9F\x87\xBB\xF0\x9F\x87\xB3"] = ":vn:";
    lEmoticonsEmoji["\xF0\x9F\x87\xAA\xF0\x9F\x87\xAD"] = ":flag_eh:";
    lEmoticonsEmoji["\xF0\x9F\x87\xAA\xF0\x9F\x87\xAD"] = ":eh:";
    lEmoticonsEmoji["\xF0\x9F\x87\xBE\xF0\x9F\x87\xAA"] = ":flag_ye:";
    lEmoticonsEmoji["\xF0\x9F\x87\xBE\xF0\x9F\x87\xAA"] = ":ye:";
    lEmoticonsEmoji["\xF0\x9F\x87\xBF\xF0\x9F\x87\xB2"] = ":flag_zm:";
    lEmoticonsEmoji["\xF0\x9F\x87\xBF\xF0\x9F\x87\xB2"] = ":zm:";
    lEmoticonsEmoji["\xF0\x9F\x87\xBF\xF0\x9F\x87\xBC"] = ":flag_zw:";
    lEmoticonsEmoji["\xF0\x9F\x87\xBF\xF0\x9F\x87\xBC"] = ":zw:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB5\xF0\x9F\x87\xB7"] = ":flag_pr:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB5\xF0\x9F\x87\xB7"] = ":pr:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB0\xF0\x9F\x87\xBE"] = ":flag_ky:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB0\xF0\x9F\x87\xBE"] = ":ky:";
    lEmoticonsEmoji["\xF0\x9F\x87\xA7\xF0\x9F\x87\xB2"] = ":flag_bm:";
    lEmoticonsEmoji["\xF0\x9F\x87\xA7\xF0\x9F\x87\xB2"] = ":bm:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB5\xF0\x9F\x87\xAB"] = ":flag_pf:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB5\xF0\x9F\x87\xAB"] = ":pf:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB5\xF0\x9F\x87\xB8"] = ":flag_ps:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB5\xF0\x9F\x87\xB8"] = ":ps:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB3\xF0\x9F\x87\xA8"] = ":flag_nc:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB3\xF0\x9F\x87\xA8"] = ":nc:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB8\xF0\x9F\x87\xAD"] = ":flag_sh:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB8\xF0\x9F\x87\xAD"] = ":sh:";
    lEmoticonsEmoji["\xF0\x9F\x87\xA6\xF0\x9F\x87\xBC"] = ":flag_aw:";
    lEmoticonsEmoji["\xF0\x9F\x87\xA6\xF0\x9F\x87\xBC"] = ":aw:";
    lEmoticonsEmoji["\xF0\x9F\x87\xBB\xF0\x9F\x87\xAE"] = ":flag_vi:";
    lEmoticonsEmoji["\xF0\x9F\x87\xBB\xF0\x9F\x87\xAE"] = ":vi:";
    lEmoticonsEmoji["\xF0\x9F\x87\xAD\xF0\x9F\x87\xB0"] = ":flag_hk:";
    lEmoticonsEmoji["\xF0\x9F\x87\xAD\xF0\x9F\x87\xB0"] = ":hk:";
    lEmoticonsEmoji["\xF0\x9F\x87\xA6\xF0\x9F\x87\xA8"] = ":flag_ac:";
    lEmoticonsEmoji["\xF0\x9F\x87\xA6\xF0\x9F\x87\xA8"] = ":ac:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB2\xF0\x9F\x87\xB8"] = ":flag_ms:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB2\xF0\x9F\x87\xB8"] = ":ms:";
    lEmoticonsEmoji["\xF0\x9F\x87\xAC\xF0\x9F\x87\xBA"] = ":flag_gu:";
    lEmoticonsEmoji["\xF0\x9F\x87\xAC\xF0\x9F\x87\xBA"] = ":gu:";
    lEmoticonsEmoji["\xF0\x9F\x87\xAC\xF0\x9F\x87\xB1"] = ":flag_gl:";
    lEmoticonsEmoji["\xF0\x9F\x87\xAC\xF0\x9F\x87\xB1"] = ":gl:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB3\xF0\x9F\x87\xBA"] = ":flag_nu:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB3\xF0\x9F\x87\xBA"] = ":nu:";
    lEmoticonsEmoji["\xF0\x9F\x87\xBC\xF0\x9F\x87\xAB"] = ":flag_wf:";
    lEmoticonsEmoji["\xF0\x9F\x87\xBC\xF0\x9F\x87\xAB"] = ":wf:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB2\xF0\x9F\x87\xB4"] = ":flag_mo:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB2\xF0\x9F\x87\xB4"] = ":mo:";
    lEmoticonsEmoji["\xF0\x9F\x87\xAB\xF0\x9F\x87\xB4"] = ":flag_fo:";
    lEmoticonsEmoji["\xF0\x9F\x87\xAB\xF0\x9F\x87\xB4"] = ":fo:";
    lEmoticonsEmoji["\xF0\x9F\x87\xAB\xF0\x9F\x87\xB0"] = ":flag_fk:";
    lEmoticonsEmoji["\xF0\x9F\x87\xAB\xF0\x9F\x87\xB0"] = ":fk:";
    lEmoticonsEmoji["\xF0\x9F\x87\xAF\xF0\x9F\x87\xAA"] = ":flag_je:";
    lEmoticonsEmoji["\xF0\x9F\x87\xAF\xF0\x9F\x87\xAA"] = ":je:";
    lEmoticonsEmoji["\xF0\x9F\x87\xA6\xF0\x9F\x87\xAE"] = ":flag_ai:";
    lEmoticonsEmoji["\xF0\x9F\x87\xA6\xF0\x9F\x87\xAE"] = ":ai:";
    lEmoticonsEmoji["\xF0\x9F\x87\xAC\xF0\x9F\x87\xAE"] = ":flag_gi:";
    lEmoticonsEmoji["\xF0\x9F\x87\xAC\xF0\x9F\x87\xAE"] = ":gi:";
    lEmoticonsEmoji["\xF0\x9F\x8E\x9E"] = ":film_frames:";
    lEmoticonsEmoji["\xF0\x9F\x8E\x9F"] = ":tickets:";
    lEmoticonsEmoji["\xF0\x9F\x8E\x9F"] = ":admission_tickets:";
    lEmoticonsEmoji["\xF0\x9F\x8F\x85"] = ":medal:";
    lEmoticonsEmoji["\xF0\x9F\x8F\x85"] = ":sports_medal:";
    lEmoticonsEmoji["\xF0\x9F\x8F\x8B"] = ":lifter:";
    lEmoticonsEmoji["\xF0\x9F\x8F\x8B"] = ":weight_lifter:";
    lEmoticonsEmoji["\xF0\x9F\x8F\x8C"] = ":golfer:";
    lEmoticonsEmoji["\xF0\x9F\x8F\x8D"] = ":motorcycle:";
    lEmoticonsEmoji["\xF0\x9F\x8F\x8D"] = ":racing_motorcycle:";
    lEmoticonsEmoji["\xF0\x9F\x8F\x8E"] = ":race_car:";
    lEmoticonsEmoji["\xF0\x9F\x8F\x8E"] = ":racing_car:";
    lEmoticonsEmoji["\xF0\x9F\x8E\x96"] = ":military_medal:";
    lEmoticonsEmoji["\xF0\x9F\x8E\x97"] = ":reminder_ribbon:";
    lEmoticonsEmoji["\xF0\x9F\x8C\xB6"] = ":hot_pepper:";
    lEmoticonsEmoji["\xF0\x9F\x8C\xA7"] = ":cloud_rain:";
    lEmoticonsEmoji["\xF0\x9F\x8C\xA7"] = ":cloud_with_rain:";
    lEmoticonsEmoji["\xF0\x9F\x8C\xA8"] = ":cloud_snow:";
    lEmoticonsEmoji["\xF0\x9F\x8C\xA8"] = ":cloud_with_snow:";
    lEmoticonsEmoji["\xF0\x9F\x8C\xA9"] = ":cloud_lightning:";
    lEmoticonsEmoji["\xF0\x9F\x8C\xA9"] = ":cloud_with_lightning:";
    lEmoticonsEmoji["\xF0\x9F\x8C\xAA"] = ":cloud_tornado:";
    lEmoticonsEmoji["\xF0\x9F\x8C\xAA"] = ":cloud_with_tornado:";
    lEmoticonsEmoji["\xF0\x9F\x8C\xAB"] = ":fog:";
    lEmoticonsEmoji["\xF0\x9F\x8C\xAC"] = ":wind_blowing_face:";
    lEmoticonsEmoji["\xF0\x9F\x90\xBF"] = ":chipmunk:";
    lEmoticonsEmoji["\xF0\x9F\x95\xB7"] = ":spider:";
    lEmoticonsEmoji["\xF0\x9F\x95\xB8"] = ":spider_web:";
    lEmoticonsEmoji["\xF0\x9F\x8C\xA1"] = ":thermometer:";
    lEmoticonsEmoji["\xF0\x9F\x8E\x99"] = ":microphone2:";
    lEmoticonsEmoji["\xF0\x9F\x8E\x99"] = ":studio_microphone:";
    lEmoticonsEmoji["\xF0\x9F\x8E\x9A"] = ":level_slider:";
    lEmoticonsEmoji["\xF0\x9F\x8E\x9B"] = ":control_knobs:";
    lEmoticonsEmoji["\xF0\x9F\x8F\xB3"] = ":flag_white:";
    lEmoticonsEmoji["\xF0\x9F\x8F\xB3"] = ":waving_white_flag:";
    lEmoticonsEmoji["\xF0\x9F\x8F\xB4"] = ":flag_black:";
    lEmoticonsEmoji["\xF0\x9F\x8F\xB4"] = ":waving_black_flag:";
    lEmoticonsEmoji["\xF0\x9F\x8F\xB5"] = ":rosette:";
    lEmoticonsEmoji["\xF0\x9F\x8F\xB7"] = ":label:";
    lEmoticonsEmoji["\xF0\x9F\x93\xB8"] = ":camera_with_flash:";
    lEmoticonsEmoji["\xF0\x9F\x93\xBD"] = ":projector:";
    lEmoticonsEmoji["\xF0\x9F\x93\xBD"] = ":film_projector:";
    lEmoticonsEmoji["\xE2\x9C\x9D"] = ":cross:";
    lEmoticonsEmoji["\xE2\x9C\x9D"] = ":latin_cross:";
    lEmoticonsEmoji["\xF0\x9F\x95\x89"] = ":om_symbol:";
    lEmoticonsEmoji["\xF0\x9F\x95\x8A"] = ":dove:";
    lEmoticonsEmoji["\xF0\x9F\x95\x8A"] = ":dove_of_peace:";
    lEmoticonsEmoji["\xF0\x9F\x95\xAF"] = ":candle:";
    lEmoticonsEmoji["\xF0\x9F\x95\xB0"] = ":clock:";
    lEmoticonsEmoji["\xF0\x9F\x95\xB0"] = ":mantlepiece_clock:";
    lEmoticonsEmoji["\xF0\x9F\x95\xB3"] = ":hole:";
    lEmoticonsEmoji["\xF0\x9F\x95\xB6"] = ":dark_sunglasses:";
    lEmoticonsEmoji["\xF0\x9F\x95\xB9"] = ":joystick:";
    lEmoticonsEmoji["\xF0\x9F\x96\x87"] = ":paperclips:";
    lEmoticonsEmoji["\xF0\x9F\x96\x87"] = ":linked_paperclips:";
    lEmoticonsEmoji["\xF0\x9F\x96\x8A"] = ":pen_ballpoint:";
    lEmoticonsEmoji["\xF0\x9F\x96\x8A"] = ":lower_left_ballpoint_pen:";
    lEmoticonsEmoji["\xF0\x9F\x96\x8B"] = ":pen_fountain:";
    lEmoticonsEmoji["\xF0\x9F\x96\x8B"] = ":lower_left_fountain_pen:";
    lEmoticonsEmoji["\xF0\x9F\x96\x8C"] = ":paintbrush:";
    lEmoticonsEmoji["\xF0\x9F\x96\x8C"] = ":lower_left_paintbrush:";
    lEmoticonsEmoji["\xF0\x9F\x96\x8D"] = ":crayon:";
    lEmoticonsEmoji["\xF0\x9F\x96\x8D"] = ":lower_left_crayon:";
    lEmoticonsEmoji["\xF0\x9F\x96\xA5"] = ":desktop:";
    lEmoticonsEmoji["\xF0\x9F\x96\xA5"] = ":desktop_computer:";
    lEmoticonsEmoji["\xF0\x9F\x96\xA8"] = ":printer:";
    lEmoticonsEmoji["\xE2\x8C\xA8"] = ":keyboard:";
    lEmoticonsEmoji["\xF0\x9F\x96\xB2"] = ":trackball:";
    lEmoticonsEmoji["\xF0\x9F\x96\xBC"] = ":frame_photo:";
    lEmoticonsEmoji["\xF0\x9F\x96\xBC"] = ":frame_with_picture:";
    lEmoticonsEmoji["\xF0\x9F\x97\x82"] = ":dividers:";
    lEmoticonsEmoji["\xF0\x9F\x97\x82"] = ":card_index_dividers:";
    lEmoticonsEmoji["\xF0\x9F\x97\x83"] = ":card_box:";
    lEmoticonsEmoji["\xF0\x9F\x97\x83"] = ":card_file_box:";
    lEmoticonsEmoji["\xF0\x9F\x97\x84"] = ":file_cabinet:";
    lEmoticonsEmoji["\xF0\x9F\x97\x91"] = ":wastebasket:";
    lEmoticonsEmoji["\xF0\x9F\x97\x92"] = ":notepad_spiral:";
    lEmoticonsEmoji["\xF0\x9F\x97\x92"] = ":spiral_note_pad:";
    lEmoticonsEmoji["\xF0\x9F\x97\x93"] = ":calendar_spiral:";
    lEmoticonsEmoji["\xF0\x9F\x97\x93"] = ":spiral_calendar_pad:";
    lEmoticonsEmoji["\xF0\x9F\x97\x9C"] = ":compression:";
    lEmoticonsEmoji["\xF0\x9F\x97\x9D"] = ":key2:";
    lEmoticonsEmoji["\xF0\x9F\x97\x9D"] = ":old_key:";
    lEmoticonsEmoji["\xF0\x9F\x97\x9E"] = ":newspaper2:";
    lEmoticonsEmoji["\xF0\x9F\x97\x9E"] = ":rolled_up_newspaper:";
    lEmoticonsEmoji["\xF0\x9F\x97\xA1"] = ":dagger:";
    lEmoticonsEmoji["\xF0\x9F\x97\xA1"] = ":dagger_knife:";
    lEmoticonsEmoji["\xF0\x9F\x97\xA3"] = ":speaking_head:";
    lEmoticonsEmoji["\xF0\x9F\x97\xA3"] = ":speaking_head_in_silhouette:";
    lEmoticonsEmoji["\xF0\x9F\x97\xAF"] = ":anger_right:";
    lEmoticonsEmoji["\xF0\x9F\x97\xAF"] = ":right_anger_bubble:";
    lEmoticonsEmoji["\xF0\x9F\x97\xB3"] = ":ballot_box:";
    lEmoticonsEmoji["\xF0\x9F\x97\xB3"] = ":ballot_box_with_ballot:";
    lEmoticonsEmoji["\xF0\x9F\x97\xBA"] = ":map:";
    lEmoticonsEmoji["\xF0\x9F\x97\xBA"] = ":world_map:";
    lEmoticonsEmoji["\xF0\x9F\x9B\x8C"] = ":sleeping_accommodation:";
    lEmoticonsEmoji["\xF0\x9F\x9B\xA0"] = ":tools:";
    lEmoticonsEmoji["\xF0\x9F\x9B\xA0"] = ":hammer_and_wrench:";
    lEmoticonsEmoji["\xF0\x9F\x9B\xA1"] = ":shield:";
    lEmoticonsEmoji["\xF0\x9F\x9B\xA2"] = ":oil:";
    lEmoticonsEmoji["\xF0\x9F\x9B\xA2"] = ":oil_drum:";
    lEmoticonsEmoji["\xF0\x9F\x9B\xB0"] = ":satellite_orbital:";
    lEmoticonsEmoji["\xF0\x9F\x8D\xBD"] = ":fork_knife_plate:";
    lEmoticonsEmoji["\xF0\x9F\x8D\xBD"] = ":fork_and_knife_with_plate:";
    lEmoticonsEmoji["\xF0\x9F\x91\x81"] = ":eye:";
    lEmoticonsEmoji["\xF0\x9F\x95\xB4"] = ":levitate:";
    lEmoticonsEmoji["\xF0\x9F\x95\xB4"] = ":man_in_business_suit_levitating:";
    lEmoticonsEmoji["\xF0\x9F\x95\xB5"] = ":spy:";
    lEmoticonsEmoji["\xF0\x9F\x95\xB5"] = ":sleuth_or_spy:";
    lEmoticonsEmoji["\xE2\x9C\x8D"] = ":writing_hand:";
    lEmoticonsEmoji["\xF0\x9F\x96\x90"] = ":hand_splayed:";
    lEmoticonsEmoji["\xF0\x9F\x96\x90"] = ":raised_hand_with_fingers_splayed:";
    lEmoticonsEmoji["\xF0\x9F\x96\x95"] = ":middle_finger:";
    lEmoticonsEmoji["\xF0\x9F\x96\x95"] = ":reversed_hand_with_middle_finger_extended:";
    lEmoticonsEmoji["\xF0\x9F\x96\x96"] = ":vulcan:";
    lEmoticonsEmoji["\xF0\x9F\x96\x96"] = ":raised_hand_with_part_between_middle_and_ring_fingers:";
    lEmoticonsEmoji["\xF0\x9F\x99\x81"] = ":slight_frown:";
    lEmoticonsEmoji["\xF0\x9F\x99\x81"] = ":slightly_frowning_face:";
    lEmoticonsEmoji["\xF0\x9F\x99\x82"] = ":slight_smile:";
    lEmoticonsEmoji["\xF0\x9F\x99\x82"] = ":slightly_smiling_face:";
    lEmoticonsEmoji["\xF0\x9F\x8F\x94"] = ":mountain_snow:";
    lEmoticonsEmoji["\xF0\x9F\x8F\x94"] = ":snow_capped_mountain:";
    lEmoticonsEmoji["\xF0\x9F\x8F\x95"] = ":camping:";
    lEmoticonsEmoji["\xF0\x9F\x8F\x96"] = ":beach:";
    lEmoticonsEmoji["\xF0\x9F\x8F\x96"] = ":beach_with_umbrella:";
    lEmoticonsEmoji["\xF0\x9F\x8F\x97"] = ":construction_site:";
    lEmoticonsEmoji["\xF0\x9F\x8F\x97"] = ":building_construction:";
    lEmoticonsEmoji["\xF0\x9F\x8F\x98"] = ":homes:";
    lEmoticonsEmoji["\xF0\x9F\x8F\x98"] = ":house_buildings:";
    lEmoticonsEmoji["\xF0\x9F\x8F\x99"] = ":cityscape:";
    lEmoticonsEmoji["\xF0\x9F\x8F\x9A"] = ":house_abandoned:";
    lEmoticonsEmoji["\xF0\x9F\x8F\x9A"] = ":derelict_house_building:";
    lEmoticonsEmoji["\xF0\x9F\x8F\x9B"] = ":classical_building:";
    lEmoticonsEmoji["\xF0\x9F\x8F\x9C"] = ":desert:";
    lEmoticonsEmoji["\xF0\x9F\x8F\x9D"] = ":island:";
    lEmoticonsEmoji["\xF0\x9F\x8F\x9D"] = ":desert_island:";
    lEmoticonsEmoji["\xF0\x9F\x8F\x9E"] = ":park:";
    lEmoticonsEmoji["\xF0\x9F\x8F\x9E"] = ":national_park:";
    lEmoticonsEmoji["\xF0\x9F\x8F\x9F"] = ":stadium:";
    lEmoticonsEmoji["\xF0\x9F\x9B\x8B"] = ":couch:";
    lEmoticonsEmoji["\xF0\x9F\x9B\x8B"] = ":couch_and_lamp:";
    lEmoticonsEmoji["\xF0\x9F\x9B\x8D"] = ":shopping_bags:";
    lEmoticonsEmoji["\xF0\x9F\x9B\x8E"] = ":bellhop:";
    lEmoticonsEmoji["\xF0\x9F\x9B\x8E"] = ":bellhop_bell:";
    lEmoticonsEmoji["\xF0\x9F\x9B\x8F"] = ":bed:";
    lEmoticonsEmoji["\xF0\x9F\x9B\xA3"] = ":motorway:";
    lEmoticonsEmoji["\xF0\x9F\x9B\xA4"] = ":railway_track:";
    lEmoticonsEmoji["\xF0\x9F\x9B\xA4"] = ":railroad_track:";
    lEmoticonsEmoji["\xF0\x9F\x9B\xA5"] = ":motorboat:";
    lEmoticonsEmoji["\xF0\x9F\x9B\xA9"] = ":airplane_small:";
    lEmoticonsEmoji["\xF0\x9F\x9B\xA9"] = ":small_airplane:";
    lEmoticonsEmoji["\xF0\x9F\x9B\xAB"] = ":airplane_departure:";
    lEmoticonsEmoji["\xF0\x9F\x9B\xAC"] = ":airplane_arriving:";
    lEmoticonsEmoji["\xF0\x9F\x9B\xB3"] = ":cruise_ship:";
    lEmoticonsEmoji["\xF0\x9F\x9B\xB3"] = ":passenger_ship:";
    lEmoticonsEmoji["\xF0\x9F\x91\xB6\xF0\x9F\x8F\xBB"] = ":baby_tone1:";
    lEmoticonsEmoji["\xF0\x9F\x91\xB6\xF0\x9F\x8F\xBC"] = ":baby_tone2:";
    lEmoticonsEmoji["\xF0\x9F\x91\xB6\xF0\x9F\x8F\xBD"] = ":baby_tone3:";
    lEmoticonsEmoji["\xF0\x9F\x91\xB6\xF0\x9F\x8F\xBE"] = ":baby_tone4:";
    lEmoticonsEmoji["\xF0\x9F\x91\xB6\xF0\x9F\x8F\xBF"] = ":baby_tone5:";
    lEmoticonsEmoji["\xF0\x9F\x91\xA6\xF0\x9F\x8F\xBB"] = ":boy_tone1:";
    lEmoticonsEmoji["\xF0\x9F\x91\xA6\xF0\x9F\x8F\xBC"] = ":boy_tone2:";
    lEmoticonsEmoji["\xF0\x9F\x91\xA6\xF0\x9F\x8F\xBD"] = ":boy_tone3:";
    lEmoticonsEmoji["\xF0\x9F\x91\xA6\xF0\x9F\x8F\xBE"] = ":boy_tone4:";
    lEmoticonsEmoji["\xF0\x9F\x91\xA6\xF0\x9F\x8F\xBF"] = ":boy_tone5:";
    lEmoticonsEmoji["\xF0\x9F\x91\xA7\xF0\x9F\x8F\xBB"] = ":girl_tone1:";
    lEmoticonsEmoji["\xF0\x9F\x91\xA7\xF0\x9F\x8F\xBC"] = ":girl_tone2:";
    lEmoticonsEmoji["\xF0\x9F\x91\xA7\xF0\x9F\x8F\xBD"] = ":girl_tone3:";
    lEmoticonsEmoji["\xF0\x9F\x91\xA7\xF0\x9F\x8F\xBE"] = ":girl_tone4:";
    lEmoticonsEmoji["\xF0\x9F\x91\xA7\xF0\x9F\x8F\xBF"] = ":girl_tone5:";
    lEmoticonsEmoji["\xF0\x9F\x91\xA8\xF0\x9F\x8F\xBB"] = ":man_tone1:";
    lEmoticonsEmoji["\xF0\x9F\x91\xA8\xF0\x9F\x8F\xBC"] = ":man_tone2:";
    lEmoticonsEmoji["\xF0\x9F\x91\xA8\xF0\x9F\x8F\xBD"] = ":man_tone3:";
    lEmoticonsEmoji["\xF0\x9F\x91\xA8\xF0\x9F\x8F\xBE"] = ":man_tone4:";
    lEmoticonsEmoji["\xF0\x9F\x91\xA8\xF0\x9F\x8F\xBF"] = ":man_tone5:";
    lEmoticonsEmoji["\xF0\x9F\x91\xA9\xF0\x9F\x8F\xBB"] = ":woman_tone1:";
    lEmoticonsEmoji["\xF0\x9F\x91\xA9\xF0\x9F\x8F\xBC"] = ":woman_tone2:";
    lEmoticonsEmoji["\xF0\x9F\x91\xA9\xF0\x9F\x8F\xBD"] = ":woman_tone3:";
    lEmoticonsEmoji["\xF0\x9F\x91\xA9\xF0\x9F\x8F\xBE"] = ":woman_tone4:";
    lEmoticonsEmoji["\xF0\x9F\x91\xA9\xF0\x9F\x8F\xBF"] = ":woman_tone5:";
    lEmoticonsEmoji["\xF0\x9F\x91\xB0\xF0\x9F\x8F\xBB"] = ":bride_with_veil_tone1:";
    lEmoticonsEmoji["\xF0\x9F\x91\xB0\xF0\x9F\x8F\xBC"] = ":bride_with_veil_tone2:";
    lEmoticonsEmoji["\xF0\x9F\x91\xB0\xF0\x9F\x8F\xBD"] = ":bride_with_veil_tone3:";
    lEmoticonsEmoji["\xF0\x9F\x91\xB0\xF0\x9F\x8F\xBE"] = ":bride_with_veil_tone4:";
    lEmoticonsEmoji["\xF0\x9F\x91\xB0\xF0\x9F\x8F\xBF"] = ":bride_with_veil_tone5:";
    lEmoticonsEmoji["\xF0\x9F\x91\xB1\xF0\x9F\x8F\xBB"] = ":person_with_blond_hair_tone1:";
    lEmoticonsEmoji["\xF0\x9F\x91\xB1\xF0\x9F\x8F\xBC"] = ":person_with_blond_hair_tone2:";
    lEmoticonsEmoji["\xF0\x9F\x91\xB1\xF0\x9F\x8F\xBD"] = ":person_with_blond_hair_tone3:";
    lEmoticonsEmoji["\xF0\x9F\x91\xB1\xF0\x9F\x8F\xBE"] = ":person_with_blond_hair_tone4:";
    lEmoticonsEmoji["\xF0\x9F\x91\xB1\xF0\x9F\x8F\xBF"] = ":person_with_blond_hair_tone5:";
    lEmoticonsEmoji["\xF0\x9F\x91\xB2\xF0\x9F\x8F\xBB"] = ":man_with_gua_pi_mao_tone1:";
    lEmoticonsEmoji["\xF0\x9F\x91\xB2\xF0\x9F\x8F\xBC"] = ":man_with_gua_pi_mao_tone2:";
    lEmoticonsEmoji["\xF0\x9F\x91\xB2\xF0\x9F\x8F\xBD"] = ":man_with_gua_pi_mao_tone3:";
    lEmoticonsEmoji["\xF0\x9F\x91\xB2\xF0\x9F\x8F\xBE"] = ":man_with_gua_pi_mao_tone4:";
    lEmoticonsEmoji["\xF0\x9F\x91\xB2\xF0\x9F\x8F\xBF"] = ":man_with_gua_pi_mao_tone5:";
    lEmoticonsEmoji["\xF0\x9F\x91\xB3\xF0\x9F\x8F\xBB"] = ":man_with_turban_tone1:";
    lEmoticonsEmoji["\xF0\x9F\x91\xB3\xF0\x9F\x8F\xBC"] = ":man_with_turban_tone2:";
    lEmoticonsEmoji["\xF0\x9F\x91\xB3\xF0\x9F\x8F\xBD"] = ":man_with_turban_tone3:";
    lEmoticonsEmoji["\xF0\x9F\x91\xB3\xF0\x9F\x8F\xBE"] = ":man_with_turban_tone4:";
    lEmoticonsEmoji["\xF0\x9F\x91\xB3\xF0\x9F\x8F\xBF"] = ":man_with_turban_tone5:";
    lEmoticonsEmoji["\xF0\x9F\x91\xB4\xF0\x9F\x8F\xBB"] = ":older_man_tone1:";
    lEmoticonsEmoji["\xF0\x9F\x91\xB4\xF0\x9F\x8F\xBC"] = ":older_man_tone2:";
    lEmoticonsEmoji["\xF0\x9F\x91\xB4\xF0\x9F\x8F\xBD"] = ":older_man_tone3:";
    lEmoticonsEmoji["\xF0\x9F\x91\xB4\xF0\x9F\x8F\xBE"] = ":older_man_tone4:";
    lEmoticonsEmoji["\xF0\x9F\x91\xB4\xF0\x9F\x8F\xBF"] = ":older_man_tone5:";
    lEmoticonsEmoji["\xF0\x9F\x91\xB5\xF0\x9F\x8F\xBB"] = ":older_woman_tone1:";
    lEmoticonsEmoji["\xF0\x9F\x91\xB5\xF0\x9F\x8F\xBB"] = ":grandma_tone1:";
    lEmoticonsEmoji["\xF0\x9F\x91\xB5\xF0\x9F\x8F\xBC"] = ":older_woman_tone2:";
    lEmoticonsEmoji["\xF0\x9F\x91\xB5\xF0\x9F\x8F\xBC"] = ":grandma_tone2:";
    lEmoticonsEmoji["\xF0\x9F\x91\xB5\xF0\x9F\x8F\xBD"] = ":older_woman_tone3:";
    lEmoticonsEmoji["\xF0\x9F\x91\xB5\xF0\x9F\x8F\xBD"] = ":grandma_tone3:";
    lEmoticonsEmoji["\xF0\x9F\x91\xB5\xF0\x9F\x8F\xBE"] = ":older_woman_tone4:";
    lEmoticonsEmoji["\xF0\x9F\x91\xB5\xF0\x9F\x8F\xBE"] = ":grandma_tone4:";
    lEmoticonsEmoji["\xF0\x9F\x91\xB5\xF0\x9F\x8F\xBF"] = ":older_woman_tone5:";
    lEmoticonsEmoji["\xF0\x9F\x91\xB5\xF0\x9F\x8F\xBF"] = ":grandma_tone5:";
    lEmoticonsEmoji["\xF0\x9F\x91\xAE\xF0\x9F\x8F\xBB"] = ":cop_tone1:";
    lEmoticonsEmoji["\xF0\x9F\x91\xAE\xF0\x9F\x8F\xBC"] = ":cop_tone2:";
    lEmoticonsEmoji["\xF0\x9F\x91\xAE\xF0\x9F\x8F\xBD"] = ":cop_tone3:";
    lEmoticonsEmoji["\xF0\x9F\x91\xAE\xF0\x9F\x8F\xBE"] = ":cop_tone4:";
    lEmoticonsEmoji["\xF0\x9F\x91\xAE\xF0\x9F\x8F\xBF"] = ":cop_tone5:";
    lEmoticonsEmoji["\xF0\x9F\x91\xB7\xF0\x9F\x8F\xBB"] = ":construction_worker_tone1:";
    lEmoticonsEmoji["\xF0\x9F\x91\xB7\xF0\x9F\x8F\xBC"] = ":construction_worker_tone2:";
    lEmoticonsEmoji["\xF0\x9F\x91\xB7\xF0\x9F\x8F\xBD"] = ":construction_worker_tone3:";
    lEmoticonsEmoji["\xF0\x9F\x91\xB7\xF0\x9F\x8F\xBE"] = ":construction_worker_tone4:";
    lEmoticonsEmoji["\xF0\x9F\x91\xB7\xF0\x9F\x8F\xBF"] = ":construction_worker_tone5:";
    lEmoticonsEmoji["\xF0\x9F\x91\xB8\xF0\x9F\x8F\xBB"] = ":princess_tone1:";
    lEmoticonsEmoji["\xF0\x9F\x91\xB8\xF0\x9F\x8F\xBC"] = ":princess_tone2:";
    lEmoticonsEmoji["\xF0\x9F\x91\xB8\xF0\x9F\x8F\xBD"] = ":princess_tone3:";
    lEmoticonsEmoji["\xF0\x9F\x91\xB8\xF0\x9F\x8F\xBE"] = ":princess_tone4:";
    lEmoticonsEmoji["\xF0\x9F\x91\xB8\xF0\x9F\x8F\xBF"] = ":princess_tone5:";
    lEmoticonsEmoji["\xF0\x9F\x92\x82\xF0\x9F\x8F\xBB"] = ":guardsman_tone1:";
    lEmoticonsEmoji["\xF0\x9F\x92\x82\xF0\x9F\x8F\xBC"] = ":guardsman_tone2:";
    lEmoticonsEmoji["\xF0\x9F\x92\x82\xF0\x9F\x8F\xBD"] = ":guardsman_tone3:";
    lEmoticonsEmoji["\xF0\x9F\x92\x82\xF0\x9F\x8F\xBE"] = ":guardsman_tone4:";
    lEmoticonsEmoji["\xF0\x9F\x92\x82\xF0\x9F\x8F\xBF"] = ":guardsman_tone5:";
    lEmoticonsEmoji["\xF0\x9F\x91\xBC\xF0\x9F\x8F\xBB"] = ":angel_tone1:";
    lEmoticonsEmoji["\xF0\x9F\x91\xBC\xF0\x9F\x8F\xBC"] = ":angel_tone2:";
    lEmoticonsEmoji["\xF0\x9F\x91\xBC\xF0\x9F\x8F\xBD"] = ":angel_tone3:";
    lEmoticonsEmoji["\xF0\x9F\x91\xBC\xF0\x9F\x8F\xBE"] = ":angel_tone4:";
    lEmoticonsEmoji["\xF0\x9F\x91\xBC\xF0\x9F\x8F\xBF"] = ":angel_tone5:";
    lEmoticonsEmoji["\xF0\x9F\x99\x87\xF0\x9F\x8F\xBB"] = ":bow_tone1:";
    lEmoticonsEmoji["\xF0\x9F\x99\x87\xF0\x9F\x8F\xBC"] = ":bow_tone2:";
    lEmoticonsEmoji["\xF0\x9F\x99\x87\xF0\x9F\x8F\xBD"] = ":bow_tone3:";
    lEmoticonsEmoji["\xF0\x9F\x99\x87\xF0\x9F\x8F\xBE"] = ":bow_tone4:";
    lEmoticonsEmoji["\xF0\x9F\x99\x87\xF0\x9F\x8F\xBF"] = ":bow_tone5:";
    lEmoticonsEmoji["\xF0\x9F\x92\x81\xF0\x9F\x8F\xBB"] = ":information_desk_person_tone1:";
    lEmoticonsEmoji["\xF0\x9F\x92\x81\xF0\x9F\x8F\xBC"] = ":information_desk_person_tone2:";
    lEmoticonsEmoji["\xF0\x9F\x92\x81\xF0\x9F\x8F\xBD"] = ":information_desk_person_tone3:";
    lEmoticonsEmoji["\xF0\x9F\x92\x81\xF0\x9F\x8F\xBE"] = ":information_desk_person_tone4:";
    lEmoticonsEmoji["\xF0\x9F\x92\x81\xF0\x9F\x8F\xBF"] = ":information_desk_person_tone5:";
    lEmoticonsEmoji["\xF0\x9F\x99\x85\xF0\x9F\x8F\xBB"] = ":no_good_tone1:";
    lEmoticonsEmoji["\xF0\x9F\x99\x85\xF0\x9F\x8F\xBC"] = ":no_good_tone2:";
    lEmoticonsEmoji["\xF0\x9F\x99\x85\xF0\x9F\x8F\xBD"] = ":no_good_tone3:";
    lEmoticonsEmoji["\xF0\x9F\x99\x85\xF0\x9F\x8F\xBE"] = ":no_good_tone4:";
    lEmoticonsEmoji["\xF0\x9F\x99\x85\xF0\x9F\x8F\xBF"] = ":no_good_tone5:";
    lEmoticonsEmoji["\xF0\x9F\x99\x86\xF0\x9F\x8F\xBB"] = ":ok_woman_tone1:";
    lEmoticonsEmoji["\xF0\x9F\x99\x86\xF0\x9F\x8F\xBC"] = ":ok_woman_tone2:";
    lEmoticonsEmoji["\xF0\x9F\x99\x86\xF0\x9F\x8F\xBD"] = ":ok_woman_tone3:";
    lEmoticonsEmoji["\xF0\x9F\x99\x86\xF0\x9F\x8F\xBE"] = ":ok_woman_tone4:";
    lEmoticonsEmoji["\xF0\x9F\x99\x86\xF0\x9F\x8F\xBF"] = ":ok_woman_tone5:";
    lEmoticonsEmoji["\xF0\x9F\x99\x8B\xF0\x9F\x8F\xBB"] = ":raising_hand_tone1:";
    lEmoticonsEmoji["\xF0\x9F\x99\x8B\xF0\x9F\x8F\xBC"] = ":raising_hand_tone2:";
    lEmoticonsEmoji["\xF0\x9F\x99\x8B\xF0\x9F\x8F\xBD"] = ":raising_hand_tone3:";
    lEmoticonsEmoji["\xF0\x9F\x99\x8B\xF0\x9F\x8F\xBE"] = ":raising_hand_tone4:";
    lEmoticonsEmoji["\xF0\x9F\x99\x8B\xF0\x9F\x8F\xBF"] = ":raising_hand_tone5:";
    lEmoticonsEmoji["\xF0\x9F\x99\x8E\xF0\x9F\x8F\xBB"] = ":person_with_pouting_face_tone1:";
    lEmoticonsEmoji["\xF0\x9F\x99\x8E\xF0\x9F\x8F\xBC"] = ":person_with_pouting_face_tone2:";
    lEmoticonsEmoji["\xF0\x9F\x99\x8E\xF0\x9F\x8F\xBD"] = ":person_with_pouting_face_tone3:";
    lEmoticonsEmoji["\xF0\x9F\x99\x8E\xF0\x9F\x8F\xBE"] = ":person_with_pouting_face_tone4:";
    lEmoticonsEmoji["\xF0\x9F\x99\x8E\xF0\x9F\x8F\xBF"] = ":person_with_pouting_face_tone5:";
    lEmoticonsEmoji["\xF0\x9F\x99\x8D\xF0\x9F\x8F\xBB"] = ":person_frowning_tone1:";
    lEmoticonsEmoji["\xF0\x9F\x99\x8D\xF0\x9F\x8F\xBC"] = ":person_frowning_tone2:";
    lEmoticonsEmoji["\xF0\x9F\x99\x8D\xF0\x9F\x8F\xBD"] = ":person_frowning_tone3:";
    lEmoticonsEmoji["\xF0\x9F\x99\x8D\xF0\x9F\x8F\xBE"] = ":person_frowning_tone4:";
    lEmoticonsEmoji["\xF0\x9F\x99\x8D\xF0\x9F\x8F\xBF"] = ":person_frowning_tone5:";
    lEmoticonsEmoji["\xF0\x9F\x92\x86\xF0\x9F\x8F\xBB"] = ":massage_tone1:";
    lEmoticonsEmoji["\xF0\x9F\x92\x86\xF0\x9F\x8F\xBC"] = ":massage_tone2:";
    lEmoticonsEmoji["\xF0\x9F\x92\x86\xF0\x9F\x8F\xBD"] = ":massage_tone3:";
    lEmoticonsEmoji["\xF0\x9F\x92\x86\xF0\x9F\x8F\xBE"] = ":massage_tone4:";
    lEmoticonsEmoji["\xF0\x9F\x92\x86\xF0\x9F\x8F\xBF"] = ":massage_tone5:";
    lEmoticonsEmoji["\xF0\x9F\x92\x87\xF0\x9F\x8F\xBB"] = ":haircut_tone1:";
    lEmoticonsEmoji["\xF0\x9F\x92\x87\xF0\x9F\x8F\xBC"] = ":haircut_tone2:";
    lEmoticonsEmoji["\xF0\x9F\x92\x87\xF0\x9F\x8F\xBD"] = ":haircut_tone3:";
    lEmoticonsEmoji["\xF0\x9F\x92\x87\xF0\x9F\x8F\xBE"] = ":haircut_tone4:";
    lEmoticonsEmoji["\xF0\x9F\x92\x87\xF0\x9F\x8F\xBF"] = ":haircut_tone5:";
    lEmoticonsEmoji["\xF0\x9F\x99\x8C\xF0\x9F\x8F\xBB"] = ":raised_hands_tone1:";
    lEmoticonsEmoji["\xF0\x9F\x99\x8C\xF0\x9F\x8F\xBC"] = ":raised_hands_tone2:";
    lEmoticonsEmoji["\xF0\x9F\x99\x8C\xF0\x9F\x8F\xBD"] = ":raised_hands_tone3:";
    lEmoticonsEmoji["\xF0\x9F\x99\x8C\xF0\x9F\x8F\xBE"] = ":raised_hands_tone4:";
    lEmoticonsEmoji["\xF0\x9F\x99\x8C\xF0\x9F\x8F\xBF"] = ":raised_hands_tone5:";
    lEmoticonsEmoji["\xF0\x9F\x91\x8F\xF0\x9F\x8F\xBB"] = ":clap_tone1:";
    lEmoticonsEmoji["\xF0\x9F\x91\x8F\xF0\x9F\x8F\xBC"] = ":clap_tone2:";
    lEmoticonsEmoji["\xF0\x9F\x91\x8F\xF0\x9F\x8F\xBD"] = ":clap_tone3:";
    lEmoticonsEmoji["\xF0\x9F\x91\x8F\xF0\x9F\x8F\xBE"] = ":clap_tone4:";
    lEmoticonsEmoji["\xF0\x9F\x91\x8F\xF0\x9F\x8F\xBF"] = ":clap_tone5:";
    lEmoticonsEmoji["\xF0\x9F\x91\x82\xF0\x9F\x8F\xBB"] = ":ear_tone1:";
    lEmoticonsEmoji["\xF0\x9F\x91\x82\xF0\x9F\x8F\xBC"] = ":ear_tone2:";
    lEmoticonsEmoji["\xF0\x9F\x91\x82\xF0\x9F\x8F\xBD"] = ":ear_tone3:";
    lEmoticonsEmoji["\xF0\x9F\x91\x82\xF0\x9F\x8F\xBE"] = ":ear_tone4:";
    lEmoticonsEmoji["\xF0\x9F\x91\x82\xF0\x9F\x8F\xBF"] = ":ear_tone5:";
    lEmoticonsEmoji["\xF0\x9F\x91\x83\xF0\x9F\x8F\xBB"] = ":nose_tone1:";
    lEmoticonsEmoji["\xF0\x9F\x91\x83\xF0\x9F\x8F\xBC"] = ":nose_tone2:";
    lEmoticonsEmoji["\xF0\x9F\x91\x83\xF0\x9F\x8F\xBD"] = ":nose_tone3:";
    lEmoticonsEmoji["\xF0\x9F\x91\x83\xF0\x9F\x8F\xBE"] = ":nose_tone4:";
    lEmoticonsEmoji["\xF0\x9F\x91\x83\xF0\x9F\x8F\xBF"] = ":nose_tone5:";
    lEmoticonsEmoji["\xF0\x9F\x92\x85\xF0\x9F\x8F\xBB"] = ":nail_care_tone1:";
    lEmoticonsEmoji["\xF0\x9F\x92\x85\xF0\x9F\x8F\xBC"] = ":nail_care_tone2:";
    lEmoticonsEmoji["\xF0\x9F\x92\x85\xF0\x9F\x8F\xBD"] = ":nail_care_tone3:";
    lEmoticonsEmoji["\xF0\x9F\x92\x85\xF0\x9F\x8F\xBE"] = ":nail_care_tone4:";
    lEmoticonsEmoji["\xF0\x9F\x92\x85\xF0\x9F\x8F\xBF"] = ":nail_care_tone5:";
    lEmoticonsEmoji["\xF0\x9F\x91\x8B\xF0\x9F\x8F\xBB"] = ":wave_tone1:";
    lEmoticonsEmoji["\xF0\x9F\x91\x8B\xF0\x9F\x8F\xBC"] = ":wave_tone2:";
    lEmoticonsEmoji["\xF0\x9F\x91\x8B\xF0\x9F\x8F\xBD"] = ":wave_tone3:";
    lEmoticonsEmoji["\xF0\x9F\x91\x8B\xF0\x9F\x8F\xBE"] = ":wave_tone4:";
    lEmoticonsEmoji["\xF0\x9F\x91\x8B\xF0\x9F\x8F\xBF"] = ":wave_tone5:";
    lEmoticonsEmoji["\xF0\x9F\x91\x8D\xF0\x9F\x8F\xBB"] = ":thumbsup_tone1:";
    lEmoticonsEmoji["\xF0\x9F\x91\x8D\xF0\x9F\x8F\xBB"] = ":+1_tone1:";
    lEmoticonsEmoji["\xF0\x9F\x91\x8D\xF0\x9F\x8F\xBC"] = ":thumbsup_tone2:";
    lEmoticonsEmoji["\xF0\x9F\x91\x8D\xF0\x9F\x8F\xBC"] = ":+1_tone2:";
    lEmoticonsEmoji["\xF0\x9F\x91\x8D\xF0\x9F\x8F\xBD"] = ":thumbsup_tone3:";
    lEmoticonsEmoji["\xF0\x9F\x91\x8D\xF0\x9F\x8F\xBD"] = ":+1_tone3:";
    lEmoticonsEmoji["\xF0\x9F\x91\x8D\xF0\x9F\x8F\xBE"] = ":thumbsup_tone4:";
    lEmoticonsEmoji["\xF0\x9F\x91\x8D\xF0\x9F\x8F\xBE"] = ":+1_tone4:";
    lEmoticonsEmoji["\xF0\x9F\x91\x8D\xF0\x9F\x8F\xBF"] = ":thumbsup_tone5:";
    lEmoticonsEmoji["\xF0\x9F\x91\x8D\xF0\x9F\x8F\xBF"] = ":+1_tone5:";
    lEmoticonsEmoji["\xF0\x9F\x91\x8E\xF0\x9F\x8F\xBB"] = ":thumbsdown_tone1:";
    lEmoticonsEmoji["\xF0\x9F\x91\x8E\xF0\x9F\x8F\xBB"] = ":-1_tone1:";
    lEmoticonsEmoji["\xF0\x9F\x91\x8E\xF0\x9F\x8F\xBC"] = ":thumbsdown_tone2:";
    lEmoticonsEmoji["\xF0\x9F\x91\x8E\xF0\x9F\x8F\xBC"] = ":-1_tone2:";
    lEmoticonsEmoji["\xF0\x9F\x91\x8E\xF0\x9F\x8F\xBD"] = ":thumbsdown_tone3:";
    lEmoticonsEmoji["\xF0\x9F\x91\x8E\xF0\x9F\x8F\xBD"] = ":-1_tone3:";
    lEmoticonsEmoji["\xF0\x9F\x91\x8E\xF0\x9F\x8F\xBE"] = ":thumbsdown_tone4:";
    lEmoticonsEmoji["\xF0\x9F\x91\x8E\xF0\x9F\x8F\xBE"] = ":-1_tone4:";
    lEmoticonsEmoji["\xF0\x9F\x91\x8E\xF0\x9F\x8F\xBF"] = ":thumbsdown_tone5:";
    lEmoticonsEmoji["\xF0\x9F\x91\x8E\xF0\x9F\x8F\xBF"] = ":-1_tone5:";
    lEmoticonsEmoji["\xE2\x98\x9D\xF0\x9F\x8F\xBB"] = ":point_up_tone1:";
    lEmoticonsEmoji["\xE2\x98\x9D\xF0\x9F\x8F\xBC"] = ":point_up_tone2:";
    lEmoticonsEmoji["\xE2\x98\x9D\xF0\x9F\x8F\xBD"] = ":point_up_tone3:";
    lEmoticonsEmoji["\xE2\x98\x9D\xF0\x9F\x8F\xBE"] = ":point_up_tone4:";
    lEmoticonsEmoji["\xE2\x98\x9D\xF0\x9F\x8F\xBF"] = ":point_up_tone5:";
    lEmoticonsEmoji["\xF0\x9F\x91\x86\xF0\x9F\x8F\xBB"] = ":point_up_2_tone1:";
    lEmoticonsEmoji["\xF0\x9F\x91\x86\xF0\x9F\x8F\xBC"] = ":point_up_2_tone2:";
    lEmoticonsEmoji["\xF0\x9F\x91\x86\xF0\x9F\x8F\xBD"] = ":point_up_2_tone3:";
    lEmoticonsEmoji["\xF0\x9F\x91\x86\xF0\x9F\x8F\xBE"] = ":point_up_2_tone4:";
    lEmoticonsEmoji["\xF0\x9F\x91\x86\xF0\x9F\x8F\xBF"] = ":point_up_2_tone5:";
    lEmoticonsEmoji["\xF0\x9F\x91\x87\xF0\x9F\x8F\xBB"] = ":point_down_tone1:";
    lEmoticonsEmoji["\xF0\x9F\x91\x87\xF0\x9F\x8F\xBC"] = ":point_down_tone2:";
    lEmoticonsEmoji["\xF0\x9F\x91\x87\xF0\x9F\x8F\xBD"] = ":point_down_tone3:";
    lEmoticonsEmoji["\xF0\x9F\x91\x87\xF0\x9F\x8F\xBE"] = ":point_down_tone4:";
    lEmoticonsEmoji["\xF0\x9F\x91\x87\xF0\x9F\x8F\xBF"] = ":point_down_tone5:";
    lEmoticonsEmoji["\xF0\x9F\x91\x88\xF0\x9F\x8F\xBB"] = ":point_left_tone1:";
    lEmoticonsEmoji["\xF0\x9F\x91\x88\xF0\x9F\x8F\xBC"] = ":point_left_tone2:";
    lEmoticonsEmoji["\xF0\x9F\x91\x88\xF0\x9F\x8F\xBD"] = ":point_left_tone3:";
    lEmoticonsEmoji["\xF0\x9F\x91\x88\xF0\x9F\x8F\xBE"] = ":point_left_tone4:";
    lEmoticonsEmoji["\xF0\x9F\x91\x88\xF0\x9F\x8F\xBF"] = ":point_left_tone5:";
    lEmoticonsEmoji["\xF0\x9F\x91\x89\xF0\x9F\x8F\xBB"] = ":point_right_tone1:";
    lEmoticonsEmoji["\xF0\x9F\x91\x89\xF0\x9F\x8F\xBC"] = ":point_right_tone2:";
    lEmoticonsEmoji["\xF0\x9F\x91\x89\xF0\x9F\x8F\xBD"] = ":point_right_tone3:";
    lEmoticonsEmoji["\xF0\x9F\x91\x89\xF0\x9F\x8F\xBE"] = ":point_right_tone4:";
    lEmoticonsEmoji["\xF0\x9F\x91\x89\xF0\x9F\x8F\xBF"] = ":point_right_tone5:";
    lEmoticonsEmoji["\xF0\x9F\x91\x8C\xF0\x9F\x8F\xBB"] = ":ok_hand_tone1:";
    lEmoticonsEmoji["\xF0\x9F\x91\x8C\xF0\x9F\x8F\xBC"] = ":ok_hand_tone2:";
    lEmoticonsEmoji["\xF0\x9F\x91\x8C\xF0\x9F\x8F\xBD"] = ":ok_hand_tone3:";
    lEmoticonsEmoji["\xF0\x9F\x91\x8C\xF0\x9F\x8F\xBE"] = ":ok_hand_tone4:";
    lEmoticonsEmoji["\xF0\x9F\x91\x8C\xF0\x9F\x8F\xBF"] = ":ok_hand_tone5:";
    lEmoticonsEmoji["\xE2\x9C\x8C\xF0\x9F\x8F\xBB"] = ":v_tone1:";
    lEmoticonsEmoji["\xE2\x9C\x8C\xF0\x9F\x8F\xBC"] = ":v_tone2:";
    lEmoticonsEmoji["\xE2\x9C\x8C\xF0\x9F\x8F\xBD"] = ":v_tone3:";
    lEmoticonsEmoji["\xE2\x9C\x8C\xF0\x9F\x8F\xBE"] = ":v_tone4:";
    lEmoticonsEmoji["\xE2\x9C\x8C\xF0\x9F\x8F\xBF"] = ":v_tone5:";
    lEmoticonsEmoji["\xF0\x9F\x91\x8A\xF0\x9F\x8F\xBB"] = ":punch_tone1:";
    lEmoticonsEmoji["\xF0\x9F\x91\x8A\xF0\x9F\x8F\xBC"] = ":punch_tone2:";
    lEmoticonsEmoji["\xF0\x9F\x91\x8A\xF0\x9F\x8F\xBD"] = ":punch_tone3:";
    lEmoticonsEmoji["\xF0\x9F\x91\x8A\xF0\x9F\x8F\xBE"] = ":punch_tone4:";
    lEmoticonsEmoji["\xF0\x9F\x91\x8A\xF0\x9F\x8F\xBF"] = ":punch_tone5:";
    lEmoticonsEmoji["\xE2\x9C\x8A\xF0\x9F\x8F\xBB"] = ":fist_tone1:";
    lEmoticonsEmoji["\xE2\x9C\x8A\xF0\x9F\x8F\xBC"] = ":fist_tone2:";
    lEmoticonsEmoji["\xE2\x9C\x8A\xF0\x9F\x8F\xBD"] = ":fist_tone3:";
    lEmoticonsEmoji["\xE2\x9C\x8A\xF0\x9F\x8F\xBE"] = ":fist_tone4:";
    lEmoticonsEmoji["\xE2\x9C\x8A\xF0\x9F\x8F\xBF"] = ":fist_tone5:";
    lEmoticonsEmoji["\xE2\x9C\x8B\xF0\x9F\x8F\xBB"] = ":raised_hand_tone1:";
    lEmoticonsEmoji["\xE2\x9C\x8B\xF0\x9F\x8F\xBC"] = ":raised_hand_tone2:";
    lEmoticonsEmoji["\xE2\x9C\x8B\xF0\x9F\x8F\xBD"] = ":raised_hand_tone3:";
    lEmoticonsEmoji["\xE2\x9C\x8B\xF0\x9F\x8F\xBE"] = ":raised_hand_tone4:";
    lEmoticonsEmoji["\xE2\x9C\x8B\xF0\x9F\x8F\xBF"] = ":raised_hand_tone5:";
    lEmoticonsEmoji["\xF0\x9F\x92\xAA\xF0\x9F\x8F\xBB"] = ":muscle_tone1:";
    lEmoticonsEmoji["\xF0\x9F\x92\xAA\xF0\x9F\x8F\xBC"] = ":muscle_tone2:";
    lEmoticonsEmoji["\xF0\x9F\x92\xAA\xF0\x9F\x8F\xBD"] = ":muscle_tone3:";
    lEmoticonsEmoji["\xF0\x9F\x92\xAA\xF0\x9F\x8F\xBE"] = ":muscle_tone4:";
    lEmoticonsEmoji["\xF0\x9F\x92\xAA\xF0\x9F\x8F\xBF"] = ":muscle_tone5:";
    lEmoticonsEmoji["\xF0\x9F\x91\x90\xF0\x9F\x8F\xBB"] = ":open_hands_tone1:";
    lEmoticonsEmoji["\xF0\x9F\x91\x90\xF0\x9F\x8F\xBC"] = ":open_hands_tone2:";
    lEmoticonsEmoji["\xF0\x9F\x91\x90\xF0\x9F\x8F\xBD"] = ":open_hands_tone3:";
    lEmoticonsEmoji["\xF0\x9F\x91\x90\xF0\x9F\x8F\xBE"] = ":open_hands_tone4:";
    lEmoticonsEmoji["\xF0\x9F\x91\x90\xF0\x9F\x8F\xBF"] = ":open_hands_tone5:";
    lEmoticonsEmoji["\xF0\x9F\x99\x8F\xF0\x9F\x8F\xBB"] = ":pray_tone1:";
    lEmoticonsEmoji["\xF0\x9F\x99\x8F\xF0\x9F\x8F\xBC"] = ":pray_tone2:";
    lEmoticonsEmoji["\xF0\x9F\x99\x8F\xF0\x9F\x8F\xBD"] = ":pray_tone3:";
    lEmoticonsEmoji["\xF0\x9F\x99\x8F\xF0\x9F\x8F\xBE"] = ":pray_tone4:";
    lEmoticonsEmoji["\xF0\x9F\x99\x8F\xF0\x9F\x8F\xBF"] = ":pray_tone5:";
    lEmoticonsEmoji["\xF0\x9F\x8F\x83\xF0\x9F\x8F\xBB"] = ":runner_tone1:";
    lEmoticonsEmoji["\xF0\x9F\x8F\x83\xF0\x9F\x8F\xBC"] = ":runner_tone2:";
    lEmoticonsEmoji["\xF0\x9F\x8F\x83\xF0\x9F\x8F\xBD"] = ":runner_tone3:";
    lEmoticonsEmoji["\xF0\x9F\x8F\x83\xF0\x9F\x8F\xBE"] = ":runner_tone4:";
    lEmoticonsEmoji["\xF0\x9F\x8F\x83\xF0\x9F\x8F\xBF"] = ":runner_tone5:";
    lEmoticonsEmoji["\xF0\x9F\x9A\xB6\xF0\x9F\x8F\xBB"] = ":walking_tone1:";
    lEmoticonsEmoji["\xF0\x9F\x9A\xB6\xF0\x9F\x8F\xBC"] = ":walking_tone2:";
    lEmoticonsEmoji["\xF0\x9F\x9A\xB6\xF0\x9F\x8F\xBD"] = ":walking_tone3:";
    lEmoticonsEmoji["\xF0\x9F\x9A\xB6\xF0\x9F\x8F\xBE"] = ":walking_tone4:";
    lEmoticonsEmoji["\xF0\x9F\x9A\xB6\xF0\x9F\x8F\xBF"] = ":walking_tone5:";
    lEmoticonsEmoji["\xF0\x9F\x92\x83\xF0\x9F\x8F\xBB"] = ":dancer_tone1:";
    lEmoticonsEmoji["\xF0\x9F\x92\x83\xF0\x9F\x8F\xBC"] = ":dancer_tone2:";
    lEmoticonsEmoji["\xF0\x9F\x92\x83\xF0\x9F\x8F\xBD"] = ":dancer_tone3:";
    lEmoticonsEmoji["\xF0\x9F\x92\x83\xF0\x9F\x8F\xBE"] = ":dancer_tone4:";
    lEmoticonsEmoji["\xF0\x9F\x92\x83\xF0\x9F\x8F\xBF"] = ":dancer_tone5:";
    lEmoticonsEmoji["\xF0\x9F\x9A\xA3\xF0\x9F\x8F\xBB"] = ":rowboat_tone1:";
    lEmoticonsEmoji["\xF0\x9F\x9A\xA3\xF0\x9F\x8F\xBC"] = ":rowboat_tone2:";
    lEmoticonsEmoji["\xF0\x9F\x9A\xA3\xF0\x9F\x8F\xBD"] = ":rowboat_tone3:";
    lEmoticonsEmoji["\xF0\x9F\x9A\xA3\xF0\x9F\x8F\xBE"] = ":rowboat_tone4:";
    lEmoticonsEmoji["\xF0\x9F\x9A\xA3\xF0\x9F\x8F\xBF"] = ":rowboat_tone5:";
    lEmoticonsEmoji["\xF0\x9F\x8F\x8A\xF0\x9F\x8F\xBB"] = ":swimmer_tone1:";
    lEmoticonsEmoji["\xF0\x9F\x8F\x8A\xF0\x9F\x8F\xBC"] = ":swimmer_tone2:";
    lEmoticonsEmoji["\xF0\x9F\x8F\x8A\xF0\x9F\x8F\xBD"] = ":swimmer_tone3:";
    lEmoticonsEmoji["\xF0\x9F\x8F\x8A\xF0\x9F\x8F\xBE"] = ":swimmer_tone4:";
    lEmoticonsEmoji["\xF0\x9F\x8F\x8A\xF0\x9F\x8F\xBF"] = ":swimmer_tone5:";
    lEmoticonsEmoji["\xF0\x9F\x8F\x84\xF0\x9F\x8F\xBB"] = ":surfer_tone1:";
    lEmoticonsEmoji["\xF0\x9F\x8F\x84\xF0\x9F\x8F\xBC"] = ":surfer_tone2:";
    lEmoticonsEmoji["\xF0\x9F\x8F\x84\xF0\x9F\x8F\xBD"] = ":surfer_tone3:";
    lEmoticonsEmoji["\xF0\x9F\x8F\x84\xF0\x9F\x8F\xBE"] = ":surfer_tone4:";
    lEmoticonsEmoji["\xF0\x9F\x8F\x84\xF0\x9F\x8F\xBF"] = ":surfer_tone5:";
    lEmoticonsEmoji["\xF0\x9F\x9B\x80\xF0\x9F\x8F\xBB"] = ":bath_tone1:";
    lEmoticonsEmoji["\xF0\x9F\x9B\x80\xF0\x9F\x8F\xBC"] = ":bath_tone2:";
    lEmoticonsEmoji["\xF0\x9F\x9B\x80\xF0\x9F\x8F\xBD"] = ":bath_tone3:";
    lEmoticonsEmoji["\xF0\x9F\x9B\x80\xF0\x9F\x8F\xBE"] = ":bath_tone4:";
    lEmoticonsEmoji["\xF0\x9F\x9B\x80\xF0\x9F\x8F\xBF"] = ":bath_tone5:";
    lEmoticonsEmoji["\xF0\x9F\x9A\xB4\xF0\x9F\x8F\xBB"] = ":bicyclist_tone1:";
    lEmoticonsEmoji["\xF0\x9F\x9A\xB4\xF0\x9F\x8F\xBC"] = ":bicyclist_tone2:";
    lEmoticonsEmoji["\xF0\x9F\x9A\xB4\xF0\x9F\x8F\xBD"] = ":bicyclist_tone3:";
    lEmoticonsEmoji["\xF0\x9F\x9A\xB4\xF0\x9F\x8F\xBE"] = ":bicyclist_tone4:";
    lEmoticonsEmoji["\xF0\x9F\x9A\xB4\xF0\x9F\x8F\xBF"] = ":bicyclist_tone5:";
    lEmoticonsEmoji["\xF0\x9F\x9A\xB5\xF0\x9F\x8F\xBB"] = ":mountain_bicyclist_tone1:";
    lEmoticonsEmoji["\xF0\x9F\x9A\xB5\xF0\x9F\x8F\xBC"] = ":mountain_bicyclist_tone2:";
    lEmoticonsEmoji["\xF0\x9F\x9A\xB5\xF0\x9F\x8F\xBD"] = ":mountain_bicyclist_tone3:";
    lEmoticonsEmoji["\xF0\x9F\x9A\xB5\xF0\x9F\x8F\xBE"] = ":mountain_bicyclist_tone4:";
    lEmoticonsEmoji["\xF0\x9F\x9A\xB5\xF0\x9F\x8F\xBF"] = ":mountain_bicyclist_tone5:";
    lEmoticonsEmoji["\xF0\x9F\x8F\x87\xF0\x9F\x8F\xBB"] = ":horse_racing_tone1:";
    lEmoticonsEmoji["\xF0\x9F\x8F\x87\xF0\x9F\x8F\xBC"] = ":horse_racing_tone2:";
    lEmoticonsEmoji["\xF0\x9F\x8F\x87\xF0\x9F\x8F\xBD"] = ":horse_racing_tone3:";
    lEmoticonsEmoji["\xF0\x9F\x8F\x87\xF0\x9F\x8F\xBE"] = ":horse_racing_tone4:";
    lEmoticonsEmoji["\xF0\x9F\x8F\x87\xF0\x9F\x8F\xBF"] = ":horse_racing_tone5:";
    lEmoticonsEmoji["\xE2\x9C\x8D\xF0\x9F\x8F\xBB"] = ":writing_hand_tone1:";
    lEmoticonsEmoji["\xE2\x9C\x8D\xF0\x9F\x8F\xBC"] = ":writing_hand_tone2:";
    lEmoticonsEmoji["\xE2\x9C\x8D\xF0\x9F\x8F\xBD"] = ":writing_hand_tone3:";
    lEmoticonsEmoji["\xE2\x9C\x8D\xF0\x9F\x8F\xBE"] = ":writing_hand_tone4:";
    lEmoticonsEmoji["\xE2\x9C\x8D\xF0\x9F\x8F\xBF"] = ":writing_hand_tone5:";
    lEmoticonsEmoji["\xF0\x9F\x96\x90\xF0\x9F\x8F\xBB"] = ":hand_splayed_tone1:";
    lEmoticonsEmoji["\xF0\x9F\x96\x90\xF0\x9F\x8F\xBB"] = ":raised_hand_with_fingers_splayed_tone1:";
    lEmoticonsEmoji["\xF0\x9F\x96\x90\xF0\x9F\x8F\xBC"] = ":hand_splayed_tone2:";
    lEmoticonsEmoji["\xF0\x9F\x96\x90\xF0\x9F\x8F\xBC"] = ":raised_hand_with_fingers_splayed_tone2:";
    lEmoticonsEmoji["\xF0\x9F\x96\x90\xF0\x9F\x8F\xBD"] = ":hand_splayed_tone3:";
    lEmoticonsEmoji["\xF0\x9F\x96\x90\xF0\x9F\x8F\xBD"] = ":raised_hand_with_fingers_splayed_tone3:";
    lEmoticonsEmoji["\xF0\x9F\x96\x90\xF0\x9F\x8F\xBE"] = ":hand_splayed_tone4:";
    lEmoticonsEmoji["\xF0\x9F\x96\x90\xF0\x9F\x8F\xBE"] = ":raised_hand_with_fingers_splayed_tone4:";
    lEmoticonsEmoji["\xF0\x9F\x96\x90\xF0\x9F\x8F\xBF"] = ":hand_splayed_tone5:";
    lEmoticonsEmoji["\xF0\x9F\x96\x90\xF0\x9F\x8F\xBF"] = ":raised_hand_with_fingers_splayed_tone5:";
    lEmoticonsEmoji["\xF0\x9F\x96\x95\xF0\x9F\x8F\xBB"] = ":middle_finger_tone1:";
    lEmoticonsEmoji["\xF0\x9F\x96\x95\xF0\x9F\x8F\xBB"] = ":reversed_hand_with_middle_finger_extended_tone1:";
    lEmoticonsEmoji["\xF0\x9F\x96\x95\xF0\x9F\x8F\xBC"] = ":middle_finger_tone2:";
    lEmoticonsEmoji["\xF0\x9F\x96\x95\xF0\x9F\x8F\xBC"] = ":reversed_hand_with_middle_finger_extended_tone2:";
    lEmoticonsEmoji["\xF0\x9F\x96\x95\xF0\x9F\x8F\xBD"] = ":middle_finger_tone3:";
    lEmoticonsEmoji["\xF0\x9F\x96\x95\xF0\x9F\x8F\xBD"] = ":reversed_hand_with_middle_finger_extended_tone3:";
    lEmoticonsEmoji["\xF0\x9F\x96\x95\xF0\x9F\x8F\xBE"] = ":middle_finger_tone4:";
    lEmoticonsEmoji["\xF0\x9F\x96\x95\xF0\x9F\x8F\xBE"] = ":reversed_hand_with_middle_finger_extended_tone4:";
    lEmoticonsEmoji["\xF0\x9F\x96\x95\xF0\x9F\x8F\xBF"] = ":middle_finger_tone5:";
    lEmoticonsEmoji["\xF0\x9F\x96\x95\xF0\x9F\x8F\xBF"] = ":reversed_hand_with_middle_finger_extended_tone5:";
    lEmoticonsEmoji["\xF0\x9F\x96\x96\xF0\x9F\x8F\xBB"] = ":vulcan_tone1:";
    lEmoticonsEmoji["\xF0\x9F\x96\x96\xF0\x9F\x8F\xBB"] = ":raised_hand_with_part_between_middle_and_ring_fingers_tone1:";
    lEmoticonsEmoji["\xF0\x9F\x96\x96\xF0\x9F\x8F\xBC"] = ":vulcan_tone2:";
    lEmoticonsEmoji["\xF0\x9F\x96\x96\xF0\x9F\x8F\xBC"] = ":raised_hand_with_part_between_middle_and_ring_fingers_tone2:";
    lEmoticonsEmoji["\xF0\x9F\x96\x96\xF0\x9F\x8F\xBD"] = ":vulcan_tone3:";
    lEmoticonsEmoji["\xF0\x9F\x96\x96\xF0\x9F\x8F\xBD"] = ":raised_hand_with_part_between_middle_and_ring_fingers_tone3:";
    lEmoticonsEmoji["\xF0\x9F\x96\x96\xF0\x9F\x8F\xBE"] = ":vulcan_tone4:";
    lEmoticonsEmoji["\xF0\x9F\x96\x96\xF0\x9F\x8F\xBE"] = ":raised_hand_with_part_between_middle_and_ring_fingers_tone4:";
    lEmoticonsEmoji["\xF0\x9F\x96\x96\xF0\x9F\x8F\xBF"] = ":vulcan_tone5:";
    lEmoticonsEmoji["\xF0\x9F\x96\x96\xF0\x9F\x8F\xBF"] = ":raised_hand_with_part_between_middle_and_ring_fingers_tone5:";
    lEmoticonsEmoji["\xF0\x9F\x91\xA8\xE2\x80\x8D\xF0\x9F\x91\xA8\xE2\x80\x8D\xF0\x9F\x91\xA6"] = ":family_mmb:";
    lEmoticonsEmoji["\xF0\x9F\x91\xA8\xE2\x80\x8D\xF0\x9F\x91\xA8\xE2\x80\x8D\xF0\x9F\x91\xA6\xE2\x80\x8D\xF0\x9F\x91\xA6"] = ":family_mmbb:";
    lEmoticonsEmoji["\xF0\x9F\x91\xA8\xE2\x80\x8D\xF0\x9F\x91\xA8\xE2\x80\x8D\xF0\x9F\x91\xA7"] = ":family_mmg:";
    lEmoticonsEmoji["\xF0\x9F\x91\xA8\xE2\x80\x8D\xF0\x9F\x91\xA8\xE2\x80\x8D\xF0\x9F\x91\xA7\xE2\x80\x8D\xF0\x9F\x91\xA6"] = ":family_mmgb:";
    lEmoticonsEmoji["\xF0\x9F\x91\xA8\xE2\x80\x8D\xF0\x9F\x91\xA8\xE2\x80\x8D\xF0\x9F\x91\xA7\xE2\x80\x8D\xF0\x9F\x91\xA7"] = ":family_mmgg:";
    lEmoticonsEmoji["\xF0\x9F\x91\xA8\xE2\x80\x8D\xF0\x9F\x91\xA9\xE2\x80\x8D\xF0\x9F\x91\xA6\xE2\x80\x8D\xF0\x9F\x91\xA6"] = ":family_mwbb:";
    lEmoticonsEmoji["\xF0\x9F\x91\xA8\xE2\x80\x8D\xF0\x9F\x91\xA9\xE2\x80\x8D\xF0\x9F\x91\xA7"] = ":family_mwg:";
    lEmoticonsEmoji["\xF0\x9F\x91\xA8\xE2\x80\x8D\xF0\x9F\x91\xA9\xE2\x80\x8D\xF0\x9F\x91\xA7\xE2\x80\x8D\xF0\x9F\x91\xA6"] = ":family_mwgb:";
    lEmoticonsEmoji["\xF0\x9F\x91\xA8\xE2\x80\x8D\xF0\x9F\x91\xA9\xE2\x80\x8D\xF0\x9F\x91\xA7\xE2\x80\x8D\xF0\x9F\x91\xA7"] = ":family_mwgg:";
    lEmoticonsEmoji["\xF0\x9F\x91\xA9\xE2\x80\x8D\xF0\x9F\x91\xA9\xE2\x80\x8D\xF0\x9F\x91\xA6"] = ":family_wwb:";
    lEmoticonsEmoji["\xF0\x9F\x91\xA9\xE2\x80\x8D\xF0\x9F\x91\xA9\xE2\x80\x8D\xF0\x9F\x91\xA6\xE2\x80\x8D\xF0\x9F\x91\xA6"] = ":family_wwbb:";
    lEmoticonsEmoji["\xF0\x9F\x91\xA9\xE2\x80\x8D\xF0\x9F\x91\xA9\xE2\x80\x8D\xF0\x9F\x91\xA7"] = ":family_wwg:";
    lEmoticonsEmoji["\xF0\x9F\x91\xA9\xE2\x80\x8D\xF0\x9F\x91\xA9\xE2\x80\x8D\xF0\x9F\x91\xA7\xE2\x80\x8D\xF0\x9F\x91\xA6"] = ":family_wwgb:";
    lEmoticonsEmoji["\xF0\x9F\x91\xA9\xE2\x80\x8D\xF0\x9F\x91\xA9\xE2\x80\x8D\xF0\x9F\x91\xA7\xE2\x80\x8D\xF0\x9F\x91\xA7"] = ":family_wwgg:";
    lEmoticonsEmoji["\xF0\x9F\x91\xA9\xE2\x80\x8D\xE2\x9D\xA4\xEF\xB8\x8F\xE2\x80\x8D\xF0\x9F\x91\xA9"] = ":couple_ww:";
    lEmoticonsEmoji["\xF0\x9F\x91\xA9\xE2\x80\x8D\xE2\x9D\xA4\xEF\xB8\x8F\xE2\x80\x8D\xF0\x9F\x91\xA9"] = ":couple_with_heart_ww:";
    lEmoticonsEmoji["\xF0\x9F\x91\xA8\xE2\x80\x8D\xE2\x9D\xA4\xEF\xB8\x8F\xE2\x80\x8D\xF0\x9F\x91\xA8"] = ":couple_mm:";
    lEmoticonsEmoji["\xF0\x9F\x91\xA8\xE2\x80\x8D\xE2\x9D\xA4\xEF\xB8\x8F\xE2\x80\x8D\xF0\x9F\x91\xA8"] = ":couple_with_heart_mm:";
    lEmoticonsEmoji["\xF0\x9F\x91\xA9\xE2\x80\x8D\xE2\x9D\xA4\xEF\xB8\x8F\xE2\x80\x8D\xF0\x9F\x92\x8B\xE2\x80\x8D\xF0\x9F\x91\xA9"] = ":kiss_ww:";
    lEmoticonsEmoji["\xF0\x9F\x91\xA9\xE2\x80\x8D\xE2\x9D\xA4\xEF\xB8\x8F\xE2\x80\x8D\xF0\x9F\x92\x8B\xE2\x80\x8D\xF0\x9F\x91\xA9"] = ":couplekiss_ww:";
    lEmoticonsEmoji["\xF0\x9F\x91\xA8\xE2\x80\x8D\xE2\x9D\xA4\xEF\xB8\x8F\xE2\x80\x8D\xF0\x9F\x92\x8B\xE2\x80\x8D\xF0\x9F\x91\xA8"] = ":kiss_mm:";
    lEmoticonsEmoji["\xF0\x9F\x91\xA8\xE2\x80\x8D\xE2\x9D\xA4\xEF\xB8\x8F\xE2\x80\x8D\xF0\x9F\x92\x8B\xE2\x80\x8D\xF0\x9F\x91\xA8"] = ":couplekiss_mm:";
    lEmoticonsEmoji["\xF0\x9F\x8F\xBB"] = ":tone1:";
    lEmoticonsEmoji["\xF0\x9F\x8F\xBC"] = ":tone2:";
    lEmoticonsEmoji["\xF0\x9F\x8F\xBD"] = ":tone3:";
    lEmoticonsEmoji["\xF0\x9F\x8F\xBE"] = ":tone4:";
    lEmoticonsEmoji["\xF0\x9F\x8F\xBF"] = ":tone5:";
    lEmoticonsEmoji["\x2A\xEF\xB8\x8F\xE2\x83\xA3"] = ":asterisk:";
    lEmoticonsEmoji["\x2A\xEF\xB8\x8F\xE2\x83\xA3"] = ":keycap_asterisk:";
    lEmoticonsEmoji["\xE2\x8F\xAD"] = ":track_next:";
    lEmoticonsEmoji["\xE2\x8F\xAD"] = ":next_track:";
    lEmoticonsEmoji["\xE2\x8F\xAE"] = ":track_previous:";
    lEmoticonsEmoji["\xE2\x8F\xAE"] = ":previous_track:";
    lEmoticonsEmoji["\xE2\x8F\xAF"] = ":play_pause:";
    lEmoticonsEmoji["\xF0\x9F\x91\x81\xE2\x80\x8D\xF0\x9F\x97\xA8"] = ":eye_in_speech_bubble:";
    lEmoticonsEmoji["\xE2\x8F\xB1"] = ":stopwatch:";
    lEmoticonsEmoji["\xE2\x8F\xB2"] = ":timer:";
    lEmoticonsEmoji["\xE2\x8F\xB2"] = ":timer_clock:";
    lEmoticonsEmoji["\xE2\x8F\xB8"] = ":pause_button:";
    lEmoticonsEmoji["\xE2\x8F\xB8"] = ":double_vertical_bar:";
    lEmoticonsEmoji["\xE2\x8F\xB9"] = ":stop_button:";
    lEmoticonsEmoji["\xE2\x8F\xBA"] = ":record_button:";
    lEmoticonsEmoji["\xE2\x98\x82"] = ":umbrella2:";
    lEmoticonsEmoji["\xE2\x98\x83"] = ":snowman2:";
    lEmoticonsEmoji["\xE2\x98\x84"] = ":comet:";
    lEmoticonsEmoji["\xE2\x98\x98"] = ":shamrock:";
    lEmoticonsEmoji["\xE2\x98\xA0"] = ":skull_crossbones:";
    lEmoticonsEmoji["\xE2\x98\xA0"] = ":skull_and_crossbones:";
    lEmoticonsEmoji["\xE2\x98\xA2"] = ":radioactive:";
    lEmoticonsEmoji["\xE2\x98\xA2"] = ":radioactive_sign:";
    lEmoticonsEmoji["\xE2\x98\xA3"] = ":biohazard:";
    lEmoticonsEmoji["\xE2\x98\xA3"] = ":biohazard_sign:";
    lEmoticonsEmoji["\xE2\x98\xA6"] = ":orthodox_cross:";
    lEmoticonsEmoji["\xE2\x98\xAA"] = ":star_and_crescent:";
    lEmoticonsEmoji["\xE2\x98\xAE"] = ":peace:";
    lEmoticonsEmoji["\xE2\x98\xAE"] = ":peace_symbol:";
    lEmoticonsEmoji["\xE2\x98\xAF"] = ":yin_yang:";
    lEmoticonsEmoji["\xE2\x98\xB8"] = ":wheel_of_dharma:";
    lEmoticonsEmoji["\xE2\x98\xB9"] = ":frowning2:";
    lEmoticonsEmoji["\xE2\x98\xB9"] = ":white_frowning_face:";
    lEmoticonsEmoji["\xE2\x9A\x92"] = ":hammer_pick:";
    lEmoticonsEmoji["\xE2\x9A\x92"] = ":hammer_and_pick:";
    lEmoticonsEmoji["\xE2\x9A\x94"] = ":crossed_swords:";
    lEmoticonsEmoji["\xE2\x9A\x96"] = ":scales:";
    lEmoticonsEmoji["\xE2\x9A\x97"] = ":alembic:";
    lEmoticonsEmoji["\xE2\x9A\x99"] = ":gear:";
    lEmoticonsEmoji["\xE2\x9A\x9B"] = ":atom:";
    lEmoticonsEmoji["\xE2\x9A\x9B"] = ":atom_symbol:";
    lEmoticonsEmoji["\xE2\x9A\x9C"] = ":fleur-de-lis:";
    lEmoticonsEmoji["\xE2\x9A\xB0"] = ":coffin:";
    lEmoticonsEmoji["\xE2\x9A\xB1"] = ":urn:";
    lEmoticonsEmoji["\xE2\x9A\xB1"] = ":funeral_urn:";
    lEmoticonsEmoji["\xE2\x9B\x88"] = ":thunder_cloud_rain:";
    lEmoticonsEmoji["\xE2\x9B\x88"] = ":thunder_cloud_and_rain:";
    lEmoticonsEmoji["\xE2\x9B\x8F"] = ":pick:";
    lEmoticonsEmoji["\xE2\x9B\x91"] = ":helmet_with_cross:";
    lEmoticonsEmoji["\xE2\x9B\x91"] = ":helmet_with_white_cross:";
    lEmoticonsEmoji["\xE2\x9B\x93"] = ":chains:";
    lEmoticonsEmoji["\xE2\x9B\xA9"] = ":shinto_shrine:";
    lEmoticonsEmoji["\xE2\x9B\xB0"] = ":mountain:";
    lEmoticonsEmoji["\xE2\x9B\xB1"] = ":beach_umbrella:";
    lEmoticonsEmoji["\xE2\x9B\xB1"] = ":umbrella_on_ground:";
    lEmoticonsEmoji["\xE2\x9B\xB4"] = ":ferry:";
    lEmoticonsEmoji["\xE2\x9B\xB7"] = ":skier:";
    lEmoticonsEmoji["\xE2\x9B\xB8"] = ":ice_skate:";
    lEmoticonsEmoji["\xE2\x9B\xB9"] = ":basketball_player:";
    lEmoticonsEmoji["\xE2\x9B\xB9"] = ":person_with_ball:";
    lEmoticonsEmoji["\xE2\x9C\xA1"] = ":star_of_david:";
    lEmoticonsEmoji["\xE2\x9D\xA3"] = ":heart_exclamation:";
    lEmoticonsEmoji["\xE2\x9D\xA3"] = ":heavy_heart_exclamation_mark_ornament:";
    lEmoticonsEmoji["\xF0\x9F\x8C\xA4"] = ":white_sun_small_cloud:";
    lEmoticonsEmoji["\xF0\x9F\x8C\xA4"] = ":white_sun_with_small_cloud:";
    lEmoticonsEmoji["\xF0\x9F\x8C\xA5"] = ":white_sun_cloud:";
    lEmoticonsEmoji["\xF0\x9F\x8C\xA5"] = ":white_sun_behind_cloud:";
    lEmoticonsEmoji["\xF0\x9F\x8C\xA6"] = ":white_sun_rain_cloud:";
    lEmoticonsEmoji["\xF0\x9F\x8C\xA6"] = ":white_sun_behind_cloud_with_rain:";
    lEmoticonsEmoji["\xF0\x9F\x96\xB1"] = ":mouse_three_button:";
    lEmoticonsEmoji["\xF0\x9F\x96\xB1"] = ":three_button_mouse:";
    lEmoticonsEmoji["\xF0\x9F\x8E\x85\xF0\x9F\x8F\xBB"] = ":santa_tone1:";
    lEmoticonsEmoji["\xF0\x9F\x8E\x85\xF0\x9F\x8F\xBC"] = ":santa_tone2:";
    lEmoticonsEmoji["\xF0\x9F\x8E\x85\xF0\x9F\x8F\xBD"] = ":santa_tone3:";
    lEmoticonsEmoji["\xF0\x9F\x8E\x85\xF0\x9F\x8F\xBE"] = ":santa_tone4:";
    lEmoticonsEmoji["\xF0\x9F\x8E\x85\xF0\x9F\x8F\xBF"] = ":santa_tone5:";
    lEmoticonsEmoji["\xF0\x9F\xA4\x98\xF0\x9F\x8F\xBB"] = ":metal_tone1:";
    lEmoticonsEmoji["\xF0\x9F\xA4\x98\xF0\x9F\x8F\xBB"] = ":sign_of_the_horns_tone1:";
    lEmoticonsEmoji["\xF0\x9F\xA4\x98\xF0\x9F\x8F\xBC"] = ":metal_tone2:";
    lEmoticonsEmoji["\xF0\x9F\xA4\x98\xF0\x9F\x8F\xBC"] = ":sign_of_the_horns_tone2:";
    lEmoticonsEmoji["\xF0\x9F\xA4\x98\xF0\x9F\x8F\xBD"] = ":metal_tone3:";
    lEmoticonsEmoji["\xF0\x9F\xA4\x98\xF0\x9F\x8F\xBD"] = ":sign_of_the_horns_tone3:";
    lEmoticonsEmoji["\xF0\x9F\xA4\x98\xF0\x9F\x8F\xBE"] = ":metal_tone4:";
    lEmoticonsEmoji["\xF0\x9F\xA4\x98\xF0\x9F\x8F\xBE"] = ":sign_of_the_horns_tone4:";
    lEmoticonsEmoji["\xF0\x9F\xA4\x98\xF0\x9F\x8F\xBF"] = ":metal_tone5:";
    lEmoticonsEmoji["\xF0\x9F\xA4\x98\xF0\x9F\x8F\xBF"] = ":sign_of_the_horns_tone5:";
    lEmoticonsEmoji["\xF0\x9F\x8F\x8B\xF0\x9F\x8F\xBB"] = ":lifter_tone1:";
    lEmoticonsEmoji["\xF0\x9F\x8F\x8B\xF0\x9F\x8F\xBB"] = ":weight_lifter_tone1:";
    lEmoticonsEmoji["\xF0\x9F\x8F\x8B\xF0\x9F\x8F\xBC"] = ":lifter_tone2:";
    lEmoticonsEmoji["\xF0\x9F\x8F\x8B\xF0\x9F\x8F\xBC"] = ":weight_lifter_tone2:";
    lEmoticonsEmoji["\xF0\x9F\x8F\x8B\xF0\x9F\x8F\xBD"] = ":lifter_tone3:";
    lEmoticonsEmoji["\xF0\x9F\x8F\x8B\xF0\x9F\x8F\xBD"] = ":weight_lifter_tone3:";
    lEmoticonsEmoji["\xF0\x9F\x8F\x8B\xF0\x9F\x8F\xBE"] = ":lifter_tone4:";
    lEmoticonsEmoji["\xF0\x9F\x8F\x8B\xF0\x9F\x8F\xBE"] = ":weight_lifter_tone4:";
    lEmoticonsEmoji["\xF0\x9F\x8F\x8B\xF0\x9F\x8F\xBF"] = ":lifter_tone5:";
    lEmoticonsEmoji["\xF0\x9F\x8F\x8B\xF0\x9F\x8F\xBF"] = ":weight_lifter_tone5:";
    lEmoticonsEmoji["\xE2\x9B\xB9\xF0\x9F\x8F\xBB"] = ":basketball_player_tone1:";
    lEmoticonsEmoji["\xE2\x9B\xB9\xF0\x9F\x8F\xBB"] = ":person_with_ball_tone1:";
    lEmoticonsEmoji["\xE2\x9B\xB9\xF0\x9F\x8F\xBC"] = ":basketball_player_tone2:";
    lEmoticonsEmoji["\xE2\x9B\xB9\xF0\x9F\x8F\xBC"] = ":person_with_ball_tone2:";
    lEmoticonsEmoji["\xE2\x9B\xB9\xF0\x9F\x8F\xBD"] = ":basketball_player_tone3:";
    lEmoticonsEmoji["\xE2\x9B\xB9\xF0\x9F\x8F\xBD"] = ":person_with_ball_tone3:";
    lEmoticonsEmoji["\xE2\x9B\xB9\xF0\x9F\x8F\xBE"] = ":basketball_player_tone4:";
    lEmoticonsEmoji["\xE2\x9B\xB9\xF0\x9F\x8F\xBE"] = ":person_with_ball_tone4:";
    lEmoticonsEmoji["\xE2\x9B\xB9\xF0\x9F\x8F\xBF"] = ":basketball_player_tone5:";
    lEmoticonsEmoji["\xE2\x9B\xB9\xF0\x9F\x8F\xBF"] = ":person_with_ball_tone5:";
    lEmoticonsEmoji["\xF0\x9F\x99\x83"] = ":upside_down:";
    lEmoticonsEmoji["\xF0\x9F\x99\x83"] = ":upside_down_face:";
    lEmoticonsEmoji["\xF0\x9F\xA4\x91"] = ":money_mouth:";
    lEmoticonsEmoji["\xF0\x9F\xA4\x91"] = ":money_mouth_face:";
    lEmoticonsEmoji["\xF0\x9F\xA4\x93"] = ":nerd:";
    lEmoticonsEmoji["\xF0\x9F\xA4\x93"] = ":nerd_face:";
    lEmoticonsEmoji["\xF0\x9F\xA4\x97"] = ":hugging:";
    lEmoticonsEmoji["\xF0\x9F\xA4\x97"] = ":hugging_face:";
    lEmoticonsEmoji["\xF0\x9F\x99\x84"] = ":rolling_eyes:";
    lEmoticonsEmoji["\xF0\x9F\x99\x84"] = ":face_with_rolling_eyes:";
    lEmoticonsEmoji["\xF0\x9F\xA4\x94"] = ":thinking:";
    lEmoticonsEmoji["\xF0\x9F\xA4\x94"] = ":thinking_face:";
    lEmoticonsEmoji["\xF0\x9F\xA4\x90"] = ":zipper_mouth:";
    lEmoticonsEmoji["\xF0\x9F\xA4\x90"] = ":zipper_mouth_face:";
    lEmoticonsEmoji["\xF0\x9F\xA4\x92"] = ":thermometer_face:";
    lEmoticonsEmoji["\xF0\x9F\xA4\x92"] = ":face_with_thermometer:";
    lEmoticonsEmoji["\xF0\x9F\xA4\x95"] = ":head_bandage:";
    lEmoticonsEmoji["\xF0\x9F\xA4\x95"] = ":face_with_head_bandage:";
    lEmoticonsEmoji["\xF0\x9F\xA4\x96"] = ":robot:";
    lEmoticonsEmoji["\xF0\x9F\xA4\x96"] = ":robot_face:";
    lEmoticonsEmoji["\xF0\x9F\xA6\x81"] = ":lion_face:";
    lEmoticonsEmoji["\xF0\x9F\xA6\x81"] = ":lion:";
    lEmoticonsEmoji["\xF0\x9F\xA6\x84"] = ":unicorn:";
    lEmoticonsEmoji["\xF0\x9F\xA6\x84"] = ":unicorn_face:";
    lEmoticonsEmoji["\xF0\x9F\xA6\x82"] = ":scorpion:";
    lEmoticonsEmoji["\xF0\x9F\xA6\x80"] = ":crab:";
    lEmoticonsEmoji["\xF0\x9F\xA6\x83"] = ":turkey:";
    lEmoticonsEmoji["\xF0\x9F\xA7\x80"] = ":cheese:";
    lEmoticonsEmoji["\xF0\x9F\xA7\x80"] = ":cheese_wedge:";
    lEmoticonsEmoji["\xF0\x9F\x8C\xAD"] = ":hotdog:";
    lEmoticonsEmoji["\xF0\x9F\x8C\xAD"] = ":hot_dog:";
    lEmoticonsEmoji["\xF0\x9F\x8C\xAE"] = ":taco:";
    lEmoticonsEmoji["\xF0\x9F\x8C\xAF"] = ":burrito:";
    lEmoticonsEmoji["\xF0\x9F\x8D\xBF"] = ":popcorn:";
    lEmoticonsEmoji["\xF0\x9F\x8D\xBE"] = ":champagne:";
    lEmoticonsEmoji["\xF0\x9F\x8D\xBE"] = ":bottle_with_popping_cork:";
    lEmoticonsEmoji["\xF0\x9F\x8F\xB9"] = ":bow_and_arrow:";
    lEmoticonsEmoji["\xF0\x9F\x8F\xB9"] = ":archery:";
    lEmoticonsEmoji["\xF0\x9F\x8F\xBA"] = ":amphora:";
    lEmoticonsEmoji["\xF0\x9F\x9B\x90"] = ":place_of_worship:";
    lEmoticonsEmoji["\xF0\x9F\x9B\x90"] = ":worship_symbol:";
    lEmoticonsEmoji["\xF0\x9F\x95\x8B"] = ":kaaba:";
    lEmoticonsEmoji["\xF0\x9F\x95\x8C"] = ":mosque:";
    lEmoticonsEmoji["\xF0\x9F\x95\x8D"] = ":synagogue:";
    lEmoticonsEmoji["\xF0\x9F\x95\x8E"] = ":menorah:";
    lEmoticonsEmoji["\xF0\x9F\x93\xBF"] = ":prayer_beads:";
    lEmoticonsEmoji["\xF0\x9F\x8F\x8F"] = ":cricket:";
    lEmoticonsEmoji["\xF0\x9F\x8F\x8F"] = ":cricket_bat_ball:";
    lEmoticonsEmoji["\xF0\x9F\x8F\x90"] = ":volleyball:";
    lEmoticonsEmoji["\xF0\x9F\x8F\x91"] = ":field_hockey:";
    lEmoticonsEmoji["\xF0\x9F\x8F\x92"] = ":hockey:";
    lEmoticonsEmoji["\xF0\x9F\x8F\x93"] = ":ping_pong:";
    lEmoticonsEmoji["\xF0\x9F\x8F\x93"] = ":table_tennis:";
    lEmoticonsEmoji["\xF0\x9F\x8F\xB8"] = ":badminton:";
    lEmoticonsEmoji["\xF0\x9F\x87\xA6\xF0\x9F\x87\xBD"] = ":flag_ax:";
    lEmoticonsEmoji["\xF0\x9F\x87\xA6\xF0\x9F\x87\xBD"] = ":ax:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB9\xF0\x9F\x87\xA6"] = ":flag_ta:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB9\xF0\x9F\x87\xA6"] = ":ta:";
    lEmoticonsEmoji["\xF0\x9F\x87\xAE\xF0\x9F\x87\xB4"] = ":flag_io:";
    lEmoticonsEmoji["\xF0\x9F\x87\xAE\xF0\x9F\x87\xB4"] = ":io:";
    lEmoticonsEmoji["\xF0\x9F\x87\xA7\xF0\x9F\x87\xB6"] = ":flag_bq:";
    lEmoticonsEmoji["\xF0\x9F\x87\xA7\xF0\x9F\x87\xB6"] = ":bq:";
    lEmoticonsEmoji["\xF0\x9F\x87\xA8\xF0\x9F\x87\xBD"] = ":flag_cx:";
    lEmoticonsEmoji["\xF0\x9F\x87\xA8\xF0\x9F\x87\xBD"] = ":cx:";
    lEmoticonsEmoji["\xF0\x9F\x87\xA8\xF0\x9F\x87\xA8"] = ":flag_cc:";
    lEmoticonsEmoji["\xF0\x9F\x87\xA8\xF0\x9F\x87\xA8"] = ":cc:";
    lEmoticonsEmoji["\xF0\x9F\x87\xAC\xF0\x9F\x87\xAC"] = ":flag_gg:";
    lEmoticonsEmoji["\xF0\x9F\x87\xAC\xF0\x9F\x87\xAC"] = ":gg:";
    lEmoticonsEmoji["\xF0\x9F\x87\xAE\xF0\x9F\x87\xB2"] = ":flag_im:";
    lEmoticonsEmoji["\xF0\x9F\x87\xAE\xF0\x9F\x87\xB2"] = ":im:";
    lEmoticonsEmoji["\xF0\x9F\x87\xBE\xF0\x9F\x87\xB9"] = ":flag_yt:";
    lEmoticonsEmoji["\xF0\x9F\x87\xBE\xF0\x9F\x87\xB9"] = ":yt:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB3\xF0\x9F\x87\xAB"] = ":flag_nf:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB3\xF0\x9F\x87\xAB"] = ":nf:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB5\xF0\x9F\x87\xB3"] = ":flag_pn:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB5\xF0\x9F\x87\xB3"] = ":pn:";
    lEmoticonsEmoji["\xF0\x9F\x87\xA7\xF0\x9F\x87\xB1"] = ":flag_bl:";
    lEmoticonsEmoji["\xF0\x9F\x87\xA7\xF0\x9F\x87\xB1"] = ":bl:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB5\xF0\x9F\x87\xB2"] = ":flag_pm:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB5\xF0\x9F\x87\xB2"] = ":pm:";
    lEmoticonsEmoji["\xF0\x9F\x87\xAC\xF0\x9F\x87\xB8"] = ":flag_gs:";
    lEmoticonsEmoji["\xF0\x9F\x87\xAC\xF0\x9F\x87\xB8"] = ":gs:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB9\xF0\x9F\x87\xB0"] = ":flag_tk:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB9\xF0\x9F\x87\xB0"] = ":tk:";
    lEmoticonsEmoji["\xF0\x9F\x87\xA7\xF0\x9F\x87\xBB"] = ":flag_bv:";
    lEmoticonsEmoji["\xF0\x9F\x87\xA7\xF0\x9F\x87\xBB"] = ":bv:";
    lEmoticonsEmoji["\xF0\x9F\x87\xAD\xF0\x9F\x87\xB2"] = ":flag_hm:";
    lEmoticonsEmoji["\xF0\x9F\x87\xAD\xF0\x9F\x87\xB2"] = ":hm:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB8\xF0\x9F\x87\xAF"] = ":flag_sj:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB8\xF0\x9F\x87\xAF"] = ":sj:";
    lEmoticonsEmoji["\xF0\x9F\x87\xBA\xF0\x9F\x87\xB2"] = ":flag_um:";
    lEmoticonsEmoji["\xF0\x9F\x87\xBA\xF0\x9F\x87\xB2"] = ":um:";
    lEmoticonsEmoji["\xF0\x9F\x87\xAE\xF0\x9F\x87\xA8"] = ":flag_ic:";
    lEmoticonsEmoji["\xF0\x9F\x87\xAE\xF0\x9F\x87\xA8"] = ":ic:";
    lEmoticonsEmoji["\xF0\x9F\x87\xAA\xF0\x9F\x87\xA6"] = ":flag_ea:";
    lEmoticonsEmoji["\xF0\x9F\x87\xAA\xF0\x9F\x87\xA6"] = ":ea:";
    lEmoticonsEmoji["\xF0\x9F\x87\xA8\xF0\x9F\x87\xB5"] = ":flag_cp:";
    lEmoticonsEmoji["\xF0\x9F\x87\xA8\xF0\x9F\x87\xB5"] = ":cp:";
    lEmoticonsEmoji["\xF0\x9F\x87\xA9\xF0\x9F\x87\xAC"] = ":flag_dg:";
    lEmoticonsEmoji["\xF0\x9F\x87\xA9\xF0\x9F\x87\xAC"] = ":dg:";
    lEmoticonsEmoji["\xF0\x9F\x87\xA6\xF0\x9F\x87\xB8"] = ":flag_as:";
    lEmoticonsEmoji["\xF0\x9F\x87\xA6\xF0\x9F\x87\xB8"] = ":as:";
    lEmoticonsEmoji["\xF0\x9F\x87\xA6\xF0\x9F\x87\xB6"] = ":flag_aq:";
    lEmoticonsEmoji["\xF0\x9F\x87\xA6\xF0\x9F\x87\xB6"] = ":aq:";
    lEmoticonsEmoji["\xF0\x9F\x87\xBB\xF0\x9F\x87\xAC"] = ":flag_vg:";
    lEmoticonsEmoji["\xF0\x9F\x87\xBB\xF0\x9F\x87\xAC"] = ":vg:";
    lEmoticonsEmoji["\xF0\x9F\x87\xA8\xF0\x9F\x87\xB0"] = ":flag_ck:";
    lEmoticonsEmoji["\xF0\x9F\x87\xA8\xF0\x9F\x87\xB0"] = ":ck:";
    lEmoticonsEmoji["\xF0\x9F\x87\xA8\xF0\x9F\x87\xBC"] = ":flag_cw:";
    lEmoticonsEmoji["\xF0\x9F\x87\xA8\xF0\x9F\x87\xBC"] = ":cw:";
    lEmoticonsEmoji["\xF0\x9F\x87\xAA\xF0\x9F\x87\xBA"] = ":flag_eu:";
    lEmoticonsEmoji["\xF0\x9F\x87\xAA\xF0\x9F\x87\xBA"] = ":eu:";
    lEmoticonsEmoji["\xF0\x9F\x87\xAC\xF0\x9F\x87\xAB"] = ":flag_gf:";
    lEmoticonsEmoji["\xF0\x9F\x87\xAC\xF0\x9F\x87\xAB"] = ":gf:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB9\xF0\x9F\x87\xAB"] = ":flag_tf:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB9\xF0\x9F\x87\xAB"] = ":tf:";
    lEmoticonsEmoji["\xF0\x9F\x87\xAC\xF0\x9F\x87\xB5"] = ":flag_gp:";
    lEmoticonsEmoji["\xF0\x9F\x87\xAC\xF0\x9F\x87\xB5"] = ":gp:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB2\xF0\x9F\x87\xB6"] = ":flag_mq:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB2\xF0\x9F\x87\xB6"] = ":mq:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB2\xF0\x9F\x87\xB5"] = ":flag_mp:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB2\xF0\x9F\x87\xB5"] = ":mp:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB7\xF0\x9F\x87\xAA"] = ":flag_re:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB7\xF0\x9F\x87\xAA"] = ":re:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB8\xF0\x9F\x87\xBD"] = ":flag_sx:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB8\xF0\x9F\x87\xBD"] = ":sx:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB8\xF0\x9F\x87\xB8"] = ":flag_ss:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB8\xF0\x9F\x87\xB8"] = ":ss:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB9\xF0\x9F\x87\xA8"] = ":flag_tc:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB9\xF0\x9F\x87\xA8"] = ":tc:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB2\xF0\x9F\x87\xAB"] = ":flag_mf:";
    lEmoticonsEmoji["\xF0\x9F\x87\xB2\xF0\x9F\x87\xAB"] = ":mf:";
    lEmoticonsEmoji["\xF0\x9F\x95\xB5\xF0\x9F\x8F\xBB"] = ":spy_tone1:";
    lEmoticonsEmoji["\xF0\x9F\x95\xB5\xF0\x9F\x8F\xBB"] = ":sleuth_or_spy_tone1:";
    lEmoticonsEmoji["\xF0\x9F\x95\xB5\xF0\x9F\x8F\xBC"] = ":spy_tone2:";
    lEmoticonsEmoji["\xF0\x9F\x95\xB5\xF0\x9F\x8F\xBC"] = ":sleuth_or_spy_tone2:";
    lEmoticonsEmoji["\xF0\x9F\x95\xB5\xF0\x9F\x8F\xBD"] = ":spy_tone3:";
    lEmoticonsEmoji["\xF0\x9F\x95\xB5\xF0\x9F\x8F\xBD"] = ":sleuth_or_spy_tone3:";
    lEmoticonsEmoji["\xF0\x9F\x95\xB5\xF0\x9F\x8F\xBE"] = ":spy_tone4:";
    lEmoticonsEmoji["\xF0\x9F\x95\xB5\xF0\x9F\x8F\xBE"] = ":sleuth_or_spy_tone4:";
    lEmoticonsEmoji["\xF0\x9F\x95\xB5\xF0\x9F\x8F\xBF"] = ":spy_tone5:";
    lEmoticonsEmoji["\xF0\x9F\x95\xB5\xF0\x9F\x8F\xBF"] = ":sleuth_or_spy_tone5:";

    // TODO convert utf8 to emoji
}
*/
void Convert::fixHtmlChars(QString &strData)
{
    strData.replace("&", "&amp;");
    strData.replace("<", "&lt;");
    strData.replace(">", "&gt;");
    strData.replace("\"", "&quot;");
    strData.replace("'", "&#039;");
    strData.replace("\\", "&#92;");
}

QString Convert::fixUtf8Chars(const QString &string)
{
    QString encoded;
    for (int i = 0; i < string.size(); ++i)
    {
        QChar ch = string.at(i);
        if ((ch.unicode() < 32) || (ch.unicode() > 126))
            encoded += QString("&#%1;").arg((int)ch.unicode());
        else
            encoded += ch;
    }
    return encoded;
}

void Convert::createText(QString &strText)
{
    QString strMyColor = Settings::instance()->get("my_color");
    QString strMyFont = Settings::instance()->get("my_font");
    bool bMyBold = Settings::instance()->getBool("my_bold");
    bool bMyItalic = Settings::instance()->getBool("my_italic");

    QString font = strMyFont.toLower();
    QString weight;

    if (bMyBold) weight += "b";
    if (bMyItalic) weight += "i";

    if (font == "verdana")
        font = QString::null;
    if ((strMyColor != "#000000") && (!strMyColor.isEmpty()))
        strText = "%C"+strMyColor.right(6)+"%"+strText;
    if (!font.isEmpty())
        font = ":"+font;
    if ((!weight.isEmpty()) || (!font.isEmpty()))
        strText = "%F"+weight+font+"%"+strText;
}

void Convert::simpleReverseConvert(QString &strData)
{
    convertSlashToEmoticons(strData);
}

void Convert::simpleConvert(QString &strData)
{
    removeColor(strData);
    removeFont(strData);
    strData.replace(QRegExp("%I([a-zA-Z0-9_-]+)%"), "//\\1");
    //convertEmoticonsToSlash(strData);
}

void Convert::removeStyles(QString &strData)
{
    removeColor(strData);
    removeFont(strData);
    removeEmoticons(strData);
}

void Convert::convertText(QString &strData, bool bInsertWidthHeight, bool qWebViewContext)
{
    convertColor(strData);
    convertFont(strData);
    convertEmoticons(strData, bInsertWidthHeight, qWebViewContext);
    // TODO convertUtf8ToEmoji(strData);
    convertEmoticonsEmoji(strData, qWebViewContext);
}

bool Convert::isBold(const QString &strData)
{
    QRegExp rx("%F(b|i|bi)?:?(arial|times|verdana|tahoma|courier)?%");

    if (rx.indexIn(strData) != -1)
    {
        QString strFontStyle = rx.cap(1);

        if (strFontStyle.contains("b"))
            return true;
        else
            return false;
    }
    else
        return false;
}

bool Convert::isItalic(const QString &strData)
{
    QRegExp rx("%F(b|i|bi)?:?(arial|times|verdana|tahoma|courier)?%");

    if (rx.indexIn(strData) != -1)
    {
        QString strFontStyle = rx.cap(1);

        if (strFontStyle.contains("i"))
            return true;
        else
            return false;
    }
    else
        return false;
}

QString Convert::getFont(const QString &strData)
{
    QRegExp rx("%F(b|i|bi)?:?(arial|times|verdana|tahoma|courier)?%");

    if (rx.indexIn(strData) != -1)
    {
        QString strFontName = rx.cap(2);
        return strFontName;
    }
    else
        return QString::null;
}

int Convert::getColor(const QString &strData)
{
    QList<QString> lColors = Utils::instance()->getColors();

    int iColor = 0;
    foreach (const QString &strColor, lColors)
    {
        if (strData.contains("%C"+strColor+"%"))
            return iColor;
        iColor++;
    }
    return -1;
}

QHash<char, QString> createPrefixHash()
{
    QHash<char, QString> m;
    m['&'] = "protect";
    m['!'] = "mod";
    m['='] = "screener";
    m['+'] = "voice";
    m['%'] = "halfop";
    m['@'] = "op";
    m['`'] = "owner";
    return m;
}

void Convert::convertPrefix(QString &strData)
{
    if (strData.isEmpty()) return;

    static const QHash<char, QString> m = createPrefixHash();

    char prefix = strData.at(1).toLatin1();
    QString title = m[prefix];
    if (!title.isNull()) {
        strData.remove(1, 1);
        strData.prepend(QString("<img src=\"qrc:/images/%1.png\" alt=\"%1\" />").arg(title));
    }
}

void Convert::fixTopicUrl(QString &strData)
{
    QStringList strDataList = strData.split(" ");

    for (int i = 0; i < strDataList.size(); ++i)
    {
        QString strWord = strDataList.at(i);

        if ((strWord.startsWith("http:")) || (strWord.startsWith("https:")) || (strWord.startsWith("www.")))
        {
            Convert::removeStyles(strWord);

            strDataList[i] = QString("<a style=\"text-decoration:none;\" href=\"%1\">%2</a>").arg(strWord, strWord);
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

                strAfterLink = QString("<a style=\"text-decoration:none;\" href=\"%1\">%2</a>").arg(strAfterLink, strAfterLink);
                strDataList[i] = strBeforeLink+strAfterLink;
            }
        }
    }

    strData = strDataList.join(" ");
}
