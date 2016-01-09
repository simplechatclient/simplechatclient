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

QString findEmoticonEmoi(const QString &strEmoticon)
{
    QString path;
#ifdef Q_OS_WIN
    path = QCoreApplication::applicationDirPath();
#else
    path = SCC_DATA_DIR;
#endif

    QDir dAllEmoticonsDirs = path+"/emoticons_emoi/";
    QStringList lDirs = dAllEmoticonsDirs.entryList(QStringList("*"), QDir::Dirs | QDir::NoDotAndDotDot | QDir::NoSymLinks);

    foreach (const QString &strDir, lDirs)
    {
        QString strEmoticonCheck = QString("%1/emoticons_emoi/%2/%3%4").arg(path, strDir, strEmoticon, ".png");
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

void convertEmoticonsEmoi(QString &strData, bool qWebViewContext)
{
    QRegExp rx(":([\\w+-]+):");

    int pos = 0;
    while ((pos = rx.indexIn(strData, pos)) != -1)
    {
        QString strEmoticon = rx.cap(1);
        QString strEmoticonFull = ":"+strEmoticon+":";

        if (Settings::instance()->getBool("emoticons"))
        {
            QString strEmoticonPath = findEmoticonEmoi(strEmoticon);

            if (!strEmoticonPath.isEmpty())
            {
#ifdef Q_OS_WIN
                strEmoticonPath = "/"+strEmoticonPath;
#endif
                if (qWebViewContext)
                    strEmoticonPath = "file://"+strEmoticonPath;

                strData.replace(strEmoticonFull, QString("<img src=\"%1\" alt=\"&#58;%2&#58;\" title=\"&#58;%2&#58;\" width=\"20\" height=\"20\" />").arg(strEmoticonPath, strEmoticon));
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
void convertUtf8ToEmoi(QString &strData)
{
    QHash<QString, QString> lEmoticonsEmoi;
    lEmoticonsEmoi[":hash:"] = "\x23\xEF\xB8\x8F\xE2\x83\xA3";
    lEmoticonsEmoi["\x30\xEF\xB8\x8F\xE2\x83\xA3"] = ":zero:";
    lEmoticonsEmoi["\x31\xEF\xB8\x8F\xE2\x83\xA3"] = ":one:";
    lEmoticonsEmoi["\x32\xEF\xB8\x8F\xE2\x83\xA3"] = ":two:";
    lEmoticonsEmoi["\x33\xEF\xB8\x8F\xE2\x83\xA3"] = ":three:";
    lEmoticonsEmoi["\x34\xEF\xB8\x8F\xE2\x83\xA3"] = ":four:";
    lEmoticonsEmoi["\x35\xEF\xB8\x8F\xE2\x83\xA3"] = ":five:";
    lEmoticonsEmoi["\x36\xEF\xB8\x8F\xE2\x83\xA3"] = ":six:";
    lEmoticonsEmoi["\x37\xEF\xB8\x8F\xE2\x83\xA3"] = ":seven:";
    lEmoticonsEmoi["\x38\xEF\xB8\x8F\xE2\x83\xA3"] = ":eight:";
    lEmoticonsEmoi["\x39\xEF\xB8\x8F\xE2\x83\xA3"] = ":nine:";
    lEmoticonsEmoi["\xC2\xA9\xEF\xB8\x8F"] = ":copyright:";
    lEmoticonsEmoi["\xC2\xAE\xEF\xB8\x8F"] = ":registered:";
    lEmoticonsEmoi["\xE2\x80\xBC\xEF\xB8\x8F"] = ":bangbang:";
    lEmoticonsEmoi["\xE2\x81\x89\xEF\xB8\x8F"] = ":interrobang:";
    lEmoticonsEmoi["\xE2\x84\xA2\xEF\xB8\x8F"] = ":tm:";
    lEmoticonsEmoi["\xE2\x84\xB9\xEF\xB8\x8F"] = ":information_source:";
    lEmoticonsEmoi["\xE2\x86\x94\xEF\xB8\x8F"] = ":left_right_arrow:";
    lEmoticonsEmoi["\xE2\x86\x95\xEF\xB8\x8F"] = ":arrow_up_down:";
    lEmoticonsEmoi["\xE2\x86\x96\xEF\xB8\x8F"] = ":arrow_upper_left:";
    lEmoticonsEmoi["\xE2\x86\x97\xEF\xB8\x8F"] = ":arrow_upper_right:";
    lEmoticonsEmoi["\xE2\x86\x98\xEF\xB8\x8F"] = ":arrow_lower_right:";
    lEmoticonsEmoi["\xE2\x86\x99\xEF\xB8\x8F"] = ":arrow_lower_left:";
    lEmoticonsEmoi["\xE2\x86\xA9\xEF\xB8\x8F"] = ":leftwards_arrow_with_hook:";
    lEmoticonsEmoi["\xE2\x86\xAA\xEF\xB8\x8F"] = ":arrow_right_hook:";
    lEmoticonsEmoi["\xE2\x8C\x9A\xEF\xB8\x8F"] = ":watch:";
    lEmoticonsEmoi["\xE2\x8C\x9B\xEF\xB8\x8F"] = ":hourglass:";
    lEmoticonsEmoi["\xE2\x8F\xA9"] = ":fast_forward:";
    lEmoticonsEmoi["\xE2\x8F\xAA"] = ":rewind:";
    lEmoticonsEmoi["\xE2\x8F\xAB"] = ":arrow_double_up:";
    lEmoticonsEmoi["\xE2\x8F\xAC"] = ":arrow_double_down:";
    lEmoticonsEmoi["\xE2\x8F\xB0"] = ":alarm_clock:";
    lEmoticonsEmoi["\xE2\x8F\xB3"] = ":hourglass_flowing_sand:";
    lEmoticonsEmoi["\xE2\x93\x82\xEF\xB8\x8F"] = ":m:";
    lEmoticonsEmoi["\xE2\x96\xAA\xEF\xB8\x8F"] = ":black_small_square:";
    lEmoticonsEmoi["\xE2\x96\xAB\xEF\xB8\x8F"] = ":white_small_square:";
    lEmoticonsEmoi["\xE2\x96\xB6\xEF\xB8\x8F"] = ":arrow_forward:";
    lEmoticonsEmoi["\xE2\x97\x80\xEF\xB8\x8F"] = ":arrow_backward:";
    lEmoticonsEmoi["\xE2\x97\xBB\xEF\xB8\x8F"] = ":white_medium_square:";
    lEmoticonsEmoi["\xE2\x97\xBC\xEF\xB8\x8F"] = ":black_medium_square:";
    lEmoticonsEmoi["\xE2\x97\xBD\xEF\xB8\x8F"] = ":white_medium_small_square:";
    lEmoticonsEmoi["\xE2\x97\xBE\xEF\xB8\x8F"] = ":black_medium_small_square:";
    lEmoticonsEmoi["\xE2\x98\x80\xEF\xB8\x8F"] = ":sunny:";
    lEmoticonsEmoi["\xE2\x98\x81\xEF\xB8\x8F"] = ":cloud:";
    lEmoticonsEmoi["\xE2\x98\x8E\xEF\xB8\x8F"] = ":telephone:";
    lEmoticonsEmoi["\xE2\x98\x91\xEF\xB8\x8F"] = ":ballot_box_with_check:";
    lEmoticonsEmoi["\xE2\x98\x94\xEF\xB8\x8F"] = ":umbrella:";
    lEmoticonsEmoi["\xE2\x98\x95\xEF\xB8\x8F"] = ":coffee:";
    lEmoticonsEmoi["\xE2\x98\x9D\xEF\xB8\x8F"] = ":point_up:";
    lEmoticonsEmoi["\xE2\x98\xBA\xEF\xB8\x8F"] = ":relaxed:";
    lEmoticonsEmoi["\xE2\x99\x88\xEF\xB8\x8F"] = ":aries:";
    lEmoticonsEmoi["\xE2\x99\x89\xEF\xB8\x8F"] = ":taurus:";
    lEmoticonsEmoi["\xE2\x99\x8A\xEF\xB8\x8F"] = ":gemini:";
    lEmoticonsEmoi["\xE2\x99\x8B\xEF\xB8\x8F"] = ":cancer:";
    lEmoticonsEmoi["\xE2\x99\x8C\xEF\xB8\x8F"] = ":leo:";
    lEmoticonsEmoi["\xE2\x99\x8D\xEF\xB8\x8F"] = ":virgo:";
    lEmoticonsEmoi["\xE2\x99\x8E\xEF\xB8\x8F"] = ":libra:";
    lEmoticonsEmoi["\xE2\x99\x8F\xEF\xB8\x8F"] = ":scorpius:";
    lEmoticonsEmoi["\xE2\x99\x90\xEF\xB8\x8F"] = ":sagittarius:";
    lEmoticonsEmoi["\xE2\x99\x91\xEF\xB8\x8F"] = ":capricorn:";
    lEmoticonsEmoi["\xE2\x99\x92\xEF\xB8\x8F"] = ":aquarius:";
    lEmoticonsEmoi["\xE2\x99\x93\xEF\xB8\x8F"] = ":pisces:";
    lEmoticonsEmoi["\xE2\x99\xA0\xEF\xB8\x8F"] = ":spades:";
    lEmoticonsEmoi["\xE2\x99\xA3\xEF\xB8\x8F"] = ":clubs:";
    lEmoticonsEmoi["\xE2\x99\xA5\xEF\xB8\x8F"] = ":hearts:";
    lEmoticonsEmoi["\xE2\x99\xA6\xEF\xB8\x8F"] = ":diamonds:";
    lEmoticonsEmoi["\xE2\x99\xA8\xEF\xB8\x8F"] = ":hotsprings:";
    lEmoticonsEmoi["\xE2\x99\xBB\xEF\xB8\x8F"] = ":recycle:";
    lEmoticonsEmoi["\xE2\x99\xBF\xEF\xB8\x8F"] = ":wheelchair:";
    lEmoticonsEmoi["\xE2\x9A\x93\xEF\xB8\x8F"] = ":anchor:";
    lEmoticonsEmoi["\xE2\x9A\xA0\xEF\xB8\x8F"] = ":warning:";
    lEmoticonsEmoi["\xE2\x9A\xA1\xEF\xB8\x8F"] = ":zap:";
    lEmoticonsEmoi["\xE2\x9A\xAA\xEF\xB8\x8F"] = ":white_circle:";
    lEmoticonsEmoi["\xE2\x9A\xAB\xEF\xB8\x8F"] = ":black_circle:";
    lEmoticonsEmoi["\xE2\x9A\xBD\xEF\xB8\x8F"] = ":soccer:";
    lEmoticonsEmoi["\xE2\x9A\xBE\xEF\xB8\x8F"] = ":baseball:";
    lEmoticonsEmoi["\xE2\x9B\x84\xEF\xB8\x8F"] = ":snowman:";
    lEmoticonsEmoi["\xE2\x9B\x85\xEF\xB8\x8F"] = ":partly_sunny:";
    lEmoticonsEmoi["\xE2\x9B\x8E"] = ":ophiuchus:";
    lEmoticonsEmoi["\xE2\x9B\x94\xEF\xB8\x8F"] = ":no_entry:";
    lEmoticonsEmoi["\xE2\x9B\xAA\xEF\xB8\x8F"] = ":church:";
    lEmoticonsEmoi["\xE2\x9B\xB2\xEF\xB8\x8F"] = ":fountain:";
    lEmoticonsEmoi["\xE2\x9B\xB3\xEF\xB8\x8F"] = ":golf:";
    lEmoticonsEmoi["\xE2\x9B\xB5\xEF\xB8\x8F"] = ":sailboat:";
    lEmoticonsEmoi["\xE2\x9B\xBA\xEF\xB8\x8F"] = ":tent:";
    lEmoticonsEmoi["\xE2\x9B\xBD\xEF\xB8\x8F"] = ":fuelpump:";
    lEmoticonsEmoi["\xE2\x9C\x82\xEF\xB8\x8F"] = ":scissors:";
    lEmoticonsEmoi["\xE2\x9C\x85"] = ":white_check_mark:";
    lEmoticonsEmoi["\xE2\x9C\x88\xEF\xB8\x8F"] = ":airplane:";
    lEmoticonsEmoi["\xE2\x9C\x89\xEF\xB8\x8F"] = ":envelope:";
    lEmoticonsEmoi["\xE2\x9C\x8A"] = ":fist:";
    lEmoticonsEmoi["\xE2\x9C\x8B"] = ":raised_hand:";
    lEmoticonsEmoi["\xE2\x9C\x8C\xEF\xB8\x8F"] = ":v:";
    lEmoticonsEmoi["\xE2\x9C\x8F\xEF\xB8\x8F"] = ":pencil2:";
    lEmoticonsEmoi["\xE2\x9C\x92\xEF\xB8\x8F"] = ":black_nib:";
    lEmoticonsEmoi["\xE2\x9C\x94\xEF\xB8\x8F"] = ":heavy_check_mark:";
    lEmoticonsEmoi["\xE2\x9C\x96\xEF\xB8\x8F"] = ":heavy_multiplication_x:";
    lEmoticonsEmoi["\xE2\x9C\xA8"] = ":sparkles:";
    lEmoticonsEmoi["\xE2\x9C\xB3\xEF\xB8\x8F"] = ":eight_spoked_asterisk:";
    lEmoticonsEmoi["\xE2\x9C\xB4\xEF\xB8\x8F"] = ":eight_pointed_black_star:";
    lEmoticonsEmoi["\xE2\x9D\x84\xEF\xB8\x8F"] = ":snowflake:";
    lEmoticonsEmoi["\xE2\x9D\x87\xEF\xB8\x8F"] = ":sparkle:";
    lEmoticonsEmoi["\xE2\x9D\x8C"] = ":x:";
    lEmoticonsEmoi["\xE2\x9D\x8E"] = ":negative_squared_cross_mark:";
    lEmoticonsEmoi["\xE2\x9D\x93"] = ":question:";
    lEmoticonsEmoi["\xE2\x9D\x94"] = ":grey_question:";
    lEmoticonsEmoi["\xE2\x9D\x95"] = ":grey_exclamation:";
    lEmoticonsEmoi["\xE2\x9D\x97\xEF\xB8\x8F"] = ":exclamation:";
    lEmoticonsEmoi["\xE2\x9D\xA4\xEF\xB8\x8F"] = ":heart:";
    lEmoticonsEmoi["\xE2\x9E\x95"] = ":heavy_plus_sign:";
    lEmoticonsEmoi["\xE2\x9E\x96"] = ":heavy_minus_sign:";
    lEmoticonsEmoi["\xE2\x9E\x97"] = ":heavy_division_sign:";
    lEmoticonsEmoi["\xE2\x9E\xA1\xEF\xB8\x8F"] = ":arrow_right:";
    lEmoticonsEmoi["\xE2\x9E\xB0"] = ":curly_loop:";
    lEmoticonsEmoi["\xE2\xA4\xB4\xEF\xB8\x8F"] = ":arrow_heading_up:";
    lEmoticonsEmoi["\xE2\xA4\xB5\xEF\xB8\x8F"] = ":arrow_heading_down:";
    lEmoticonsEmoi["\xE2\xAC\x85\xEF\xB8\x8F"] = ":arrow_left:";
    lEmoticonsEmoi["\xE2\xAC\x86\xEF\xB8\x8F"] = ":arrow_up:";
    lEmoticonsEmoi["\xE2\xAC\x87\xEF\xB8\x8F"] = ":arrow_down:";
    lEmoticonsEmoi["\xE2\xAC\x9B\xEF\xB8\x8F"] = ":black_large_square:";
    lEmoticonsEmoi["\xE2\xAC\x9C\xEF\xB8\x8F"] = ":white_large_square:";
    lEmoticonsEmoi["\xE2\xAD\x90\xEF\xB8\x8F"] = ":star:";
    lEmoticonsEmoi["\xE2\xAD\x95\xEF\xB8\x8F"] = ":o:";
    lEmoticonsEmoi["\xE3\x80\xB0"] = ":wavy_dash:";
    lEmoticonsEmoi["\xE3\x80\xBD\xEF\xB8\x8F"] = ":part_alternation_mark:";
    lEmoticonsEmoi["\xE3\x8A\x97\xEF\xB8\x8F"] = ":congratulations:";
    lEmoticonsEmoi["\xE3\x8A\x99\xEF\xB8\x8F"] = ":secret:";
    lEmoticonsEmoi["\xF0\x9F\x80\x84\xEF\xB8\x8F"] = ":mahjong:";
    lEmoticonsEmoi["\xF0\x9F\x83\x8F"] = ":black_joker:";
    lEmoticonsEmoi["\xF0\x9F\x85\xB0"] = ":a:";
    lEmoticonsEmoi["\xF0\x9F\x85\xB1"] = ":b:";
    lEmoticonsEmoi["\xF0\x9F\x85\xBE"] = ":o2:";
    lEmoticonsEmoi["\xF0\x9F\x85\xBF\xEF\xB8\x8F"] = ":parking:";
    lEmoticonsEmoi["\xF0\x9F\x86\x8E"] = ":ab:";
    lEmoticonsEmoi["\xF0\x9F\x86\x91"] = ":cl:";
    lEmoticonsEmoi["\xF0\x9F\x86\x92"] = ":cool:";
    lEmoticonsEmoi["\xF0\x9F\x86\x93"] = ":free:";
    lEmoticonsEmoi["\xF0\x9F\x86\x94"] = ":id:";
    lEmoticonsEmoi["\xF0\x9F\x86\x95"] = ":new:";
    lEmoticonsEmoi["\xF0\x9F\x86\x96"] = ":ng:";
    lEmoticonsEmoi["\xF0\x9F\x86\x97"] = ":ok:";
    lEmoticonsEmoi["\xF0\x9F\x86\x98"] = ":sos:";
    lEmoticonsEmoi["\xF0\x9F\x86\x99"] = ":up:";
    lEmoticonsEmoi["\xF0\x9F\x86\x9A"] = ":vs:";
    lEmoticonsEmoi["\xF0\x9F\x87\xA8\xF0\x9F\x87\xB3"] = ":flag_cn:";
    lEmoticonsEmoi["\xF0\x9F\x87\xA8\xF0\x9F\x87\xB3"] = ":cn:";
    lEmoticonsEmoi["\xF0\x9F\x87\xA9\xF0\x9F\x87\xAA"] = ":flag_de:";
    lEmoticonsEmoi["\xF0\x9F\x87\xA9\xF0\x9F\x87\xAA"] = ":de:";
    lEmoticonsEmoi["\xF0\x9F\x87\xAA\xF0\x9F\x87\xB8"] = ":flag_es:";
    lEmoticonsEmoi["\xF0\x9F\x87\xAA\xF0\x9F\x87\xB8"] = ":es:";
    lEmoticonsEmoi["\xF0\x9F\x87\xAB\xF0\x9F\x87\xB7"] = ":flag_fr:";
    lEmoticonsEmoi["\xF0\x9F\x87\xAB\xF0\x9F\x87\xB7"] = ":fr:";
    lEmoticonsEmoi["\xF0\x9F\x87\xAC\xF0\x9F\x87\xA7"] = ":flag_gb:";
    lEmoticonsEmoi["\xF0\x9F\x87\xAC\xF0\x9F\x87\xA7"] = ":gb:";
    lEmoticonsEmoi["\xF0\x9F\x87\xAE\xF0\x9F\x87\xB9"] = ":flag_it:";
    lEmoticonsEmoi["\xF0\x9F\x87\xAE\xF0\x9F\x87\xB9"] = ":it:";
    lEmoticonsEmoi["\xF0\x9F\x87\xAF\xF0\x9F\x87\xB5"] = ":flag_jp:";
    lEmoticonsEmoi["\xF0\x9F\x87\xAF\xF0\x9F\x87\xB5"] = ":jp:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB0\xF0\x9F\x87\xB7"] = ":flag_kr:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB0\xF0\x9F\x87\xB7"] = ":kr:";
    lEmoticonsEmoi["\xF0\x9F\x87\xBA\xF0\x9F\x87\xB8"] = ":flag_us:";
    lEmoticonsEmoi["\xF0\x9F\x87\xBA\xF0\x9F\x87\xB8"] = ":us:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB7\xF0\x9F\x87\xBA"] = ":flag_ru:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB7\xF0\x9F\x87\xBA"] = ":ru:";
    lEmoticonsEmoi["\xF0\x9F\x88\x81"] = ":koko:";
    lEmoticonsEmoi["\xF0\x9F\x88\x82"] = ":sa:";
    lEmoticonsEmoi["\xF0\x9F\x88\x9A\xEF\xB8\x8F"] = ":u7121:";
    lEmoticonsEmoi["\xF0\x9F\x88\xAF\xEF\xB8\x8F"] = ":u6307:";
    lEmoticonsEmoi["\xF0\x9F\x88\xB2"] = ":u7981:";
    lEmoticonsEmoi["\xF0\x9F\x88\xB3"] = ":u7a7a:";
    lEmoticonsEmoi["\xF0\x9F\x88\xB4"] = ":u5408:";
    lEmoticonsEmoi["\xF0\x9F\x88\xB5"] = ":u6e80:";
    lEmoticonsEmoi["\xF0\x9F\x88\xB6"] = ":u6709:";
    lEmoticonsEmoi["\xF0\x9F\x88\xB7\xEF\xB8\x8F"] = ":u6708:";
    lEmoticonsEmoi["\xF0\x9F\x88\xB8"] = ":u7533:";
    lEmoticonsEmoi["\xF0\x9F\x88\xB9"] = ":u5272:";
    lEmoticonsEmoi["\xF0\x9F\x88\xBA"] = ":u55b6:";
    lEmoticonsEmoi["\xF0\x9F\x89\x90"] = ":ideograph_advantage:";
    lEmoticonsEmoi["\xF0\x9F\x89\x91"] = ":accept:";
    lEmoticonsEmoi["\xF0\x9F\x8C\x80"] = ":cyclone:";
    lEmoticonsEmoi["\xF0\x9F\x8C\x81"] = ":foggy:";
    lEmoticonsEmoi["\xF0\x9F\x8C\x82"] = ":closed_umbrella:";
    lEmoticonsEmoi["\xF0\x9F\x8C\x83"] = ":night_with_stars:";
    lEmoticonsEmoi["\xF0\x9F\x8C\x84"] = ":sunrise_over_mountains:";
    lEmoticonsEmoi["\xF0\x9F\x8C\x85"] = ":sunrise:";
    lEmoticonsEmoi["\xF0\x9F\x8C\x86"] = ":city_dusk:";
    lEmoticonsEmoi["\xF0\x9F\x8C\x87"] = ":city_sunset:";
    lEmoticonsEmoi["\xF0\x9F\x8C\x87"] = ":city_sunrise:";
    lEmoticonsEmoi["\xF0\x9F\x8C\x88"] = ":rainbow:";
    lEmoticonsEmoi["\xF0\x9F\x8C\x89"] = ":bridge_at_night:";
    lEmoticonsEmoi["\xF0\x9F\x8C\x8A"] = ":ocean:";
    lEmoticonsEmoi["\xF0\x9F\x8C\x8B"] = ":volcano:";
    lEmoticonsEmoi["\xF0\x9F\x8C\x8C"] = ":milky_way:";
    lEmoticonsEmoi["\xF0\x9F\x8C\x8F"] = ":earth_asia:";
    lEmoticonsEmoi["\xF0\x9F\x8C\x91"] = ":new_moon:";
    lEmoticonsEmoi["\xF0\x9F\x8C\x93"] = ":first_quarter_moon:";
    lEmoticonsEmoi["\xF0\x9F\x8C\x94"] = ":waxing_gibbous_moon:";
    lEmoticonsEmoi["\xF0\x9F\x8C\x95"] = ":full_moon:";
    lEmoticonsEmoi["\xF0\x9F\x8C\x99"] = ":crescent_moon:";
    lEmoticonsEmoi["\xF0\x9F\x8C\x9B"] = ":first_quarter_moon_with_face:";
    lEmoticonsEmoi["\xF0\x9F\x8C\x9F"] = ":star2:";
    lEmoticonsEmoi["\xF0\x9F\x8C\xA0"] = ":stars:";
    lEmoticonsEmoi["\xF0\x9F\x8C\xB0"] = ":chestnut:";
    lEmoticonsEmoi["\xF0\x9F\x8C\xB1"] = ":seedling:";
    lEmoticonsEmoi["\xF0\x9F\x8C\xB4"] = ":palm_tree:";
    lEmoticonsEmoi["\xF0\x9F\x8C\xB5"] = ":cactus:";
    lEmoticonsEmoi["\xF0\x9F\x8C\xB7"] = ":tulip:";
    lEmoticonsEmoi["\xF0\x9F\x8C\xB8"] = ":cherry_blossom:";
    lEmoticonsEmoi["\xF0\x9F\x8C\xB9"] = ":rose:";
    lEmoticonsEmoi["\xF0\x9F\x8C\xBA"] = ":hibiscus:";
    lEmoticonsEmoi["\xF0\x9F\x8C\xBB"] = ":sunflower:";
    lEmoticonsEmoi["\xF0\x9F\x8C\xBC"] = ":blossom:";
    lEmoticonsEmoi["\xF0\x9F\x8C\xBD"] = ":corn:";
    lEmoticonsEmoi["\xF0\x9F\x8C\xBE"] = ":ear_of_rice:";
    lEmoticonsEmoi["\xF0\x9F\x8C\xBF"] = ":herb:";
    lEmoticonsEmoi["\xF0\x9F\x8D\x80"] = ":four_leaf_clover:";
    lEmoticonsEmoi["\xF0\x9F\x8D\x81"] = ":maple_leaf:";
    lEmoticonsEmoi["\xF0\x9F\x8D\x82"] = ":fallen_leaf:";
    lEmoticonsEmoi["\xF0\x9F\x8D\x83"] = ":leaves:";
    lEmoticonsEmoi["\xF0\x9F\x8D\x84"] = ":mushroom:";
    lEmoticonsEmoi["\xF0\x9F\x8D\x85"] = ":tomato:";
    lEmoticonsEmoi["\xF0\x9F\x8D\x86"] = ":eggplant:";
    lEmoticonsEmoi["\xF0\x9F\x8D\x87"] = ":grapes:";
    lEmoticonsEmoi["\xF0\x9F\x8D\x88"] = ":melon:";
    lEmoticonsEmoi["\xF0\x9F\x8D\x89"] = ":watermelon:";
    lEmoticonsEmoi["\xF0\x9F\x8D\x8A"] = ":tangerine:";
    lEmoticonsEmoi["\xF0\x9F\x8D\x8C"] = ":banana:";
    lEmoticonsEmoi["\xF0\x9F\x8D\x8D"] = ":pineapple:";
    lEmoticonsEmoi["\xF0\x9F\x8D\x8E"] = ":apple:";
    lEmoticonsEmoi["\xF0\x9F\x8D\x8F"] = ":green_apple:";
    lEmoticonsEmoi["\xF0\x9F\x8D\x91"] = ":peach:";
    lEmoticonsEmoi["\xF0\x9F\x8D\x92"] = ":cherries:";
    lEmoticonsEmoi["\xF0\x9F\x8D\x93"] = ":strawberry:";
    lEmoticonsEmoi["\xF0\x9F\x8D\x94"] = ":hamburger:";
    lEmoticonsEmoi["\xF0\x9F\x8D\x95"] = ":pizza:";
    lEmoticonsEmoi["\xF0\x9F\x8D\x96"] = ":meat_on_bone:";
    lEmoticonsEmoi["\xF0\x9F\x8D\x97"] = ":poultry_leg:";
    lEmoticonsEmoi["\xF0\x9F\x8D\x98"] = ":rice_cracker:";
    lEmoticonsEmoi["\xF0\x9F\x8D\x99"] = ":rice_ball:";
    lEmoticonsEmoi["\xF0\x9F\x8D\x9A"] = ":rice:";
    lEmoticonsEmoi["\xF0\x9F\x8D\x9B"] = ":curry:";
    lEmoticonsEmoi["\xF0\x9F\x8D\x9C"] = ":ramen:";
    lEmoticonsEmoi["\xF0\x9F\x8D\x9D"] = ":spaghetti:";
    lEmoticonsEmoi["\xF0\x9F\x8D\x9E"] = ":bread:";
    lEmoticonsEmoi["\xF0\x9F\x8D\x9F"] = ":fries:";
    lEmoticonsEmoi["\xF0\x9F\x8D\xA0"] = ":sweet_potato:";
    lEmoticonsEmoi["\xF0\x9F\x8D\xA1"] = ":dango:";
    lEmoticonsEmoi["\xF0\x9F\x8D\xA2"] = ":oden:";
    lEmoticonsEmoi["\xF0\x9F\x8D\xA3"] = ":sushi:";
    lEmoticonsEmoi["\xF0\x9F\x8D\xA4"] = ":fried_shrimp:";
    lEmoticonsEmoi["\xF0\x9F\x8D\xA5"] = ":fish_cake:";
    lEmoticonsEmoi["\xF0\x9F\x8D\xA6"] = ":icecream:";
    lEmoticonsEmoi["\xF0\x9F\x8D\xA7"] = ":shaved_ice:";
    lEmoticonsEmoi["\xF0\x9F\x8D\xA8"] = ":ice_cream:";
    lEmoticonsEmoi["\xF0\x9F\x8D\xA9"] = ":doughnut:";
    lEmoticonsEmoi["\xF0\x9F\x8D\xAA"] = ":cookie:";
    lEmoticonsEmoi["\xF0\x9F\x8D\xAB"] = ":chocolate_bar:";
    lEmoticonsEmoi["\xF0\x9F\x8D\xAC"] = ":candy:";
    lEmoticonsEmoi["\xF0\x9F\x8D\xAD"] = ":lollipop:";
    lEmoticonsEmoi["\xF0\x9F\x8D\xAE"] = ":custard:";
    lEmoticonsEmoi["\xF0\x9F\x8D\xAF"] = ":honey_pot:";
    lEmoticonsEmoi["\xF0\x9F\x8D\xB0"] = ":cake:";
    lEmoticonsEmoi["\xF0\x9F\x8D\xB1"] = ":bento:";
    lEmoticonsEmoi["\xF0\x9F\x8D\xB2"] = ":stew:";
    lEmoticonsEmoi["\xF0\x9F\x8D\xB3"] = ":egg:";
    lEmoticonsEmoi["\xF0\x9F\x8D\xB4"] = ":fork_and_knife:";
    lEmoticonsEmoi["\xF0\x9F\x8D\xB5"] = ":tea:";
    lEmoticonsEmoi["\xF0\x9F\x8D\xB6"] = ":sake:";
    lEmoticonsEmoi["\xF0\x9F\x8D\xB7"] = ":wine_glass:";
    lEmoticonsEmoi["\xF0\x9F\x8D\xB8"] = ":cocktail:";
    lEmoticonsEmoi["\xF0\x9F\x8D\xB9"] = ":tropical_drink:";
    lEmoticonsEmoi["\xF0\x9F\x8D\xBA"] = ":beer:";
    lEmoticonsEmoi["\xF0\x9F\x8D\xBB"] = ":beers:";
    lEmoticonsEmoi["\xF0\x9F\x8E\x80"] = ":ribbon:";
    lEmoticonsEmoi["\xF0\x9F\x8E\x81"] = ":gift:";
    lEmoticonsEmoi["\xF0\x9F\x8E\x82"] = ":birthday:";
    lEmoticonsEmoi["\xF0\x9F\x8E\x83"] = ":jack_o_lantern:";
    lEmoticonsEmoi["\xF0\x9F\x8E\x84"] = ":christmas_tree:";
    lEmoticonsEmoi["\xF0\x9F\x8E\x85"] = ":santa:";
    lEmoticonsEmoi["\xF0\x9F\x8E\x86"] = ":fireworks:";
    lEmoticonsEmoi["\xF0\x9F\x8E\x87"] = ":sparkler:";
    lEmoticonsEmoi["\xF0\x9F\x8E\x88"] = ":balloon:";
    lEmoticonsEmoi["\xF0\x9F\x8E\x89"] = ":tada:";
    lEmoticonsEmoi["\xF0\x9F\x8E\x8A"] = ":confetti_ball:";
    lEmoticonsEmoi["\xF0\x9F\x8E\x8B"] = ":tanabata_tree:";
    lEmoticonsEmoi["\xF0\x9F\x8E\x8C"] = ":crossed_flags:";
    lEmoticonsEmoi["\xF0\x9F\x8E\x8D"] = ":bamboo:";
    lEmoticonsEmoi["\xF0\x9F\x8E\x8E"] = ":dolls:";
    lEmoticonsEmoi["\xF0\x9F\x8E\x8F"] = ":flags:";
    lEmoticonsEmoi["\xF0\x9F\x8E\x90"] = ":wind_chime:";
    lEmoticonsEmoi["\xF0\x9F\x8E\x91"] = ":rice_scene:";
    lEmoticonsEmoi["\xF0\x9F\x8E\x92"] = ":school_satchel:";
    lEmoticonsEmoi["\xF0\x9F\x8E\x93"] = ":mortar_board:";
    lEmoticonsEmoi["\xF0\x9F\x8E\xA0"] = ":carousel_horse:";
    lEmoticonsEmoi["\xF0\x9F\x8E\xA1"] = ":ferris_wheel:";
    lEmoticonsEmoi["\xF0\x9F\x8E\xA2"] = ":roller_coaster:";
    lEmoticonsEmoi["\xF0\x9F\x8E\xA3"] = ":fishing_pole_and_fish:";
    lEmoticonsEmoi["\xF0\x9F\x8E\xA4"] = ":microphone:";
    lEmoticonsEmoi["\xF0\x9F\x8E\xA5"] = ":movie_camera:";
    lEmoticonsEmoi["\xF0\x9F\x8E\xA6"] = ":cinema:";
    lEmoticonsEmoi["\xF0\x9F\x8E\xA7"] = ":headphones:";
    lEmoticonsEmoi["\xF0\x9F\x8E\xA8"] = ":art:";
    lEmoticonsEmoi["\xF0\x9F\x8E\xA9"] = ":tophat:";
    lEmoticonsEmoi["\xF0\x9F\x8E\xAA"] = ":circus_tent:";
    lEmoticonsEmoi["\xF0\x9F\x8E\xAB"] = ":ticket:";
    lEmoticonsEmoi["\xF0\x9F\x8E\xAC"] = ":clapper:";
    lEmoticonsEmoi["\xF0\x9F\x8E\xAD"] = ":performing_arts:";
    lEmoticonsEmoi["\xF0\x9F\x8E\xAE"] = ":video_game:";
    lEmoticonsEmoi["\xF0\x9F\x8E\xAF"] = ":dart:";
    lEmoticonsEmoi["\xF0\x9F\x8E\xB0"] = ":slot_machine:";
    lEmoticonsEmoi["\xF0\x9F\x8E\xB1"] = ":8ball:";
    lEmoticonsEmoi["\xF0\x9F\x8E\xB2"] = ":game_die:";
    lEmoticonsEmoi["\xF0\x9F\x8E\xB3"] = ":bowling:";
    lEmoticonsEmoi["\xF0\x9F\x8E\xB4"] = ":flower_playing_cards:";
    lEmoticonsEmoi["\xF0\x9F\x8E\xB5"] = ":musical_note:";
    lEmoticonsEmoi["\xF0\x9F\x8E\xB6"] = ":notes:";
    lEmoticonsEmoi["\xF0\x9F\x8E\xB7"] = ":saxophone:";
    lEmoticonsEmoi["\xF0\x9F\x8E\xB8"] = ":guitar:";
    lEmoticonsEmoi["\xF0\x9F\x8E\xB9"] = ":musical_keyboard:";
    lEmoticonsEmoi["\xF0\x9F\x8E\xBA"] = ":trumpet:";
    lEmoticonsEmoi["\xF0\x9F\x8E\xBB"] = ":violin:";
    lEmoticonsEmoi["\xF0\x9F\x8E\xBC"] = ":musical_score:";
    lEmoticonsEmoi["\xF0\x9F\x8E\xBD"] = ":running_shirt_with_sash:";
    lEmoticonsEmoi["\xF0\x9F\x8E\xBE"] = ":tennis:";
    lEmoticonsEmoi["\xF0\x9F\x8E\xBF"] = ":ski:";
    lEmoticonsEmoi["\xF0\x9F\x8F\x80"] = ":basketball:";
    lEmoticonsEmoi["\xF0\x9F\x8F\x81"] = ":checkered_flag:";
    lEmoticonsEmoi["\xF0\x9F\x8F\x82"] = ":snowboarder:";
    lEmoticonsEmoi["\xF0\x9F\x8F\x83"] = ":runner:";
    lEmoticonsEmoi["\xF0\x9F\x8F\x84"] = ":surfer:";
    lEmoticonsEmoi["\xF0\x9F\x8F\x86"] = ":trophy:";
    lEmoticonsEmoi["\xF0\x9F\x8F\x88"] = ":football:";
    lEmoticonsEmoi["\xF0\x9F\x8F\x8A"] = ":swimmer:";
    lEmoticonsEmoi["\xF0\x9F\x8F\xA0"] = ":house:";
    lEmoticonsEmoi["\xF0\x9F\x8F\xA1"] = ":house_with_garden:";
    lEmoticonsEmoi["\xF0\x9F\x8F\xA2"] = ":office:";
    lEmoticonsEmoi["\xF0\x9F\x8F\xA3"] = ":post_office:";
    lEmoticonsEmoi["\xF0\x9F\x8F\xA5"] = ":hospital:";
    lEmoticonsEmoi["\xF0\x9F\x8F\xA6"] = ":bank:";
    lEmoticonsEmoi["\xF0\x9F\x8F\xA7"] = ":atm:";
    lEmoticonsEmoi["\xF0\x9F\x8F\xA8"] = ":hotel:";
    lEmoticonsEmoi["\xF0\x9F\x8F\xA9"] = ":love_hotel:";
    lEmoticonsEmoi["\xF0\x9F\x8F\xAA"] = ":convenience_store:";
    lEmoticonsEmoi["\xF0\x9F\x8F\xAB"] = ":school:";
    lEmoticonsEmoi["\xF0\x9F\x8F\xAC"] = ":department_store:";
    lEmoticonsEmoi["\xF0\x9F\x8F\xAD"] = ":factory:";
    lEmoticonsEmoi["\xF0\x9F\x8F\xAE"] = ":izakaya_lantern:";
    lEmoticonsEmoi["\xF0\x9F\x8F\xAF"] = ":japanese_castle:";
    lEmoticonsEmoi["\xF0\x9F\x8F\xB0"] = ":european_castle:";
    lEmoticonsEmoi["\xF0\x9F\x90\x8C"] = ":snail:";
    lEmoticonsEmoi["\xF0\x9F\x90\x8D"] = ":snake:";
    lEmoticonsEmoi["\xF0\x9F\x90\x8E"] = ":racehorse:";
    lEmoticonsEmoi["\xF0\x9F\x90\x91"] = ":sheep:";
    lEmoticonsEmoi["\xF0\x9F\x90\x92"] = ":monkey:";
    lEmoticonsEmoi["\xF0\x9F\x90\x94"] = ":chicken:";
    lEmoticonsEmoi["\xF0\x9F\x90\x97"] = ":boar:";
    lEmoticonsEmoi["\xF0\x9F\x90\x98"] = ":elephant:";
    lEmoticonsEmoi["\xF0\x9F\x90\x99"] = ":octopus:";
    lEmoticonsEmoi["\xF0\x9F\x90\x9A"] = ":shell:";
    lEmoticonsEmoi["\xF0\x9F\x90\x9B"] = ":bug:";
    lEmoticonsEmoi["\xF0\x9F\x90\x9C"] = ":ant:";
    lEmoticonsEmoi["\xF0\x9F\x90\x9D"] = ":bee:";
    lEmoticonsEmoi["\xF0\x9F\x90\x9E"] = ":beetle:";
    lEmoticonsEmoi["\xF0\x9F\x90\x9F"] = ":fish:";
    lEmoticonsEmoi["\xF0\x9F\x90\xA0"] = ":tropical_fish:";
    lEmoticonsEmoi["\xF0\x9F\x90\xA1"] = ":blowfish:";
    lEmoticonsEmoi["\xF0\x9F\x90\xA2"] = ":turtle:";
    lEmoticonsEmoi["\xF0\x9F\x90\xA3"] = ":hatching_chick:";
    lEmoticonsEmoi["\xF0\x9F\x90\xA4"] = ":baby_chick:";
    lEmoticonsEmoi["\xF0\x9F\x90\xA5"] = ":hatched_chick:";
    lEmoticonsEmoi["\xF0\x9F\x90\xA6"] = ":bird:";
    lEmoticonsEmoi["\xF0\x9F\x90\xA7"] = ":penguin:";
    lEmoticonsEmoi["\xF0\x9F\x90\xA8"] = ":koala:";
    lEmoticonsEmoi["\xF0\x9F\x90\xA9"] = ":poodle:";
    lEmoticonsEmoi["\xF0\x9F\x90\xAB"] = ":camel:";
    lEmoticonsEmoi["\xF0\x9F\x90\xAC"] = ":dolphin:";
    lEmoticonsEmoi["\xF0\x9F\x90\xAD"] = ":mouse:";
    lEmoticonsEmoi["\xF0\x9F\x90\xAE"] = ":cow:";
    lEmoticonsEmoi["\xF0\x9F\x90\xAF"] = ":tiger:";
    lEmoticonsEmoi["\xF0\x9F\x90\xB0"] = ":rabbit:";
    lEmoticonsEmoi["\xF0\x9F\x90\xB1"] = ":cat:";
    lEmoticonsEmoi["\xF0\x9F\x90\xB2"] = ":dragon_face:";
    lEmoticonsEmoi["\xF0\x9F\x90\xB3"] = ":whale:";
    lEmoticonsEmoi["\xF0\x9F\x90\xB4"] = ":horse:";
    lEmoticonsEmoi["\xF0\x9F\x90\xB5"] = ":monkey_face:";
    lEmoticonsEmoi["\xF0\x9F\x90\xB6"] = ":dog:";
    lEmoticonsEmoi["\xF0\x9F\x90\xB7"] = ":pig:";
    lEmoticonsEmoi["\xF0\x9F\x90\xB8"] = ":frog:";
    lEmoticonsEmoi["\xF0\x9F\x90\xB9"] = ":hamster:";
    lEmoticonsEmoi["\xF0\x9F\x90\xBA"] = ":wolf:";
    lEmoticonsEmoi["\xF0\x9F\x90\xBB"] = ":bear:";
    lEmoticonsEmoi["\xF0\x9F\x90\xBC"] = ":panda_face:";
    lEmoticonsEmoi["\xF0\x9F\x90\xBD"] = ":pig_nose:";
    lEmoticonsEmoi["\xF0\x9F\x90\xBE"] = ":feet:";
    lEmoticonsEmoi["\xF0\x9F\x91\x80"] = ":eyes:";
    lEmoticonsEmoi["\xF0\x9F\x91\x82"] = ":ear:";
    lEmoticonsEmoi["\xF0\x9F\x91\x83"] = ":nose:";
    lEmoticonsEmoi["\xF0\x9F\x91\x84"] = ":lips:";
    lEmoticonsEmoi["\xF0\x9F\x91\x85"] = ":tongue:";
    lEmoticonsEmoi["\xF0\x9F\x91\x86"] = ":point_up_2:";
    lEmoticonsEmoi["\xF0\x9F\x91\x87"] = ":point_down:";
    lEmoticonsEmoi["\xF0\x9F\x91\x88"] = ":point_left:";
    lEmoticonsEmoi["\xF0\x9F\x91\x89"] = ":point_right:";
    lEmoticonsEmoi["\xF0\x9F\x91\x8A"] = ":punch:";
    lEmoticonsEmoi["\xF0\x9F\x91\x8B"] = ":wave:";
    lEmoticonsEmoi["\xF0\x9F\x91\x8C"] = ":ok_hand:";
    lEmoticonsEmoi["\xF0\x9F\x91\x8D"] = ":thumbsup:";
    lEmoticonsEmoi["\xF0\x9F\x91\x8D"] = ":+1:";
    lEmoticonsEmoi["\xF0\x9F\x91\x8E"] = ":thumbsdown:";
    lEmoticonsEmoi["\xF0\x9F\x91\x8E"] = ":-1:";
    lEmoticonsEmoi["\xF0\x9F\x91\x8F"] = ":clap:";
    lEmoticonsEmoi["\xF0\x9F\x91\x90"] = ":open_hands:";
    lEmoticonsEmoi["\xF0\x9F\x91\x91"] = ":crown:";
    lEmoticonsEmoi["\xF0\x9F\x91\x92"] = ":womans_hat:";
    lEmoticonsEmoi["\xF0\x9F\x91\x93"] = ":eyeglasses:";
    lEmoticonsEmoi["\xF0\x9F\x91\x94"] = ":necktie:";
    lEmoticonsEmoi["\xF0\x9F\x91\x95"] = ":shirt:";
    lEmoticonsEmoi["\xF0\x9F\x91\x96"] = ":jeans:";
    lEmoticonsEmoi["\xF0\x9F\x91\x97"] = ":dress:";
    lEmoticonsEmoi["\xF0\x9F\x91\x98"] = ":kimono:";
    lEmoticonsEmoi["\xF0\x9F\x91\x99"] = ":bikini:";
    lEmoticonsEmoi["\xF0\x9F\x91\x9A"] = ":womans_clothes:";
    lEmoticonsEmoi["\xF0\x9F\x91\x9B"] = ":purse:";
    lEmoticonsEmoi["\xF0\x9F\x91\x9C"] = ":handbag:";
    lEmoticonsEmoi["\xF0\x9F\x91\x9D"] = ":pouch:";
    lEmoticonsEmoi["\xF0\x9F\x91\x9E"] = ":mans_shoe:";
    lEmoticonsEmoi["\xF0\x9F\x91\x9F"] = ":athletic_shoe:";
    lEmoticonsEmoi["\xF0\x9F\x91\xA0"] = ":high_heel:";
    lEmoticonsEmoi["\xF0\x9F\x91\xA1"] = ":sandal:";
    lEmoticonsEmoi["\xF0\x9F\x91\xA2"] = ":boot:";
    lEmoticonsEmoi["\xF0\x9F\x91\xA3"] = ":footprints:";
    lEmoticonsEmoi["\xF0\x9F\x91\xA4"] = ":bust_in_silhouette:";
    lEmoticonsEmoi["\xF0\x9F\x91\xA6"] = ":boy:";
    lEmoticonsEmoi["\xF0\x9F\x91\xA7"] = ":girl:";
    lEmoticonsEmoi["\xF0\x9F\x91\xA8"] = ":man:";
    lEmoticonsEmoi["\xF0\x9F\x91\xA9"] = ":woman:";
    lEmoticonsEmoi["\xF0\x9F\x91\xAA"] = ":family:";
    lEmoticonsEmoi["\xF0\x9F\x91\xAB"] = ":couple:";
    lEmoticonsEmoi["\xF0\x9F\x91\xAE"] = ":cop:";
    lEmoticonsEmoi["\xF0\x9F\x91\xAF"] = ":dancers:";
    lEmoticonsEmoi["\xF0\x9F\x91\xB0"] = ":bride_with_veil:";
    lEmoticonsEmoi["\xF0\x9F\x91\xB1"] = ":person_with_blond_hair:";
    lEmoticonsEmoi["\xF0\x9F\x91\xB2"] = ":man_with_gua_pi_mao:";
    lEmoticonsEmoi["\xF0\x9F\x91\xB3"] = ":man_with_turban:";
    lEmoticonsEmoi["\xF0\x9F\x91\xB4"] = ":older_man:";
    lEmoticonsEmoi["\xF0\x9F\x91\xB5"] = ":older_woman:";
    lEmoticonsEmoi["\xF0\x9F\x91\xB5"] = ":grandma:";
    lEmoticonsEmoi["\xF0\x9F\x91\xB6"] = ":baby:";
    lEmoticonsEmoi["\xF0\x9F\x91\xB7"] = ":construction_worker:";
    lEmoticonsEmoi["\xF0\x9F\x91\xB8"] = ":princess:";
    lEmoticonsEmoi["\xF0\x9F\x91\xB9"] = ":japanese_ogre:";
    lEmoticonsEmoi["\xF0\x9F\x91\xBA"] = ":japanese_goblin:";
    lEmoticonsEmoi["\xF0\x9F\x91\xBB"] = ":ghost:";
    lEmoticonsEmoi["\xF0\x9F\x91\xBC"] = ":angel:";
    lEmoticonsEmoi["\xF0\x9F\x91\xBD"] = ":alien:";
    lEmoticonsEmoi["\xF0\x9F\x91\xBE"] = ":space_invader:";
    lEmoticonsEmoi["\xF0\x9F\x91\xBF"] = ":imp:";
    lEmoticonsEmoi["\xF0\x9F\x92\x80"] = ":skull:";
    lEmoticonsEmoi["\xF0\x9F\x92\x80"] = ":skeleton:";
    lEmoticonsEmoi["\xF0\x9F\x93\x87"] = ":card_index:";
    lEmoticonsEmoi["\xF0\x9F\x92\x81"] = ":information_desk_person:";
    lEmoticonsEmoi["\xF0\x9F\x92\x82"] = ":guardsman:";
    lEmoticonsEmoi["\xF0\x9F\x92\x83"] = ":dancer:";
    lEmoticonsEmoi["\xF0\x9F\x92\x84"] = ":lipstick:";
    lEmoticonsEmoi["\xF0\x9F\x92\x85"] = ":nail_care:";
    lEmoticonsEmoi["\xF0\x9F\x93\x92"] = ":ledger:";
    lEmoticonsEmoi["\xF0\x9F\x92\x86"] = ":massage:";
    lEmoticonsEmoi["\xF0\x9F\x93\x93"] = ":notebook:";
    lEmoticonsEmoi["\xF0\x9F\x92\x87"] = ":haircut:";
    lEmoticonsEmoi["\xF0\x9F\x93\x94"] = ":notebook_with_decorative_cover:";
    lEmoticonsEmoi["\xF0\x9F\x92\x88"] = ":barber:";
    lEmoticonsEmoi["\xF0\x9F\x93\x95"] = ":closed_book:";
    lEmoticonsEmoi["\xF0\x9F\x92\x89"] = ":syringe:";
    lEmoticonsEmoi["\xF0\x9F\x93\x96"] = ":book:";
    lEmoticonsEmoi["\xF0\x9F\x92\x8A"] = ":pill:";
    lEmoticonsEmoi["\xF0\x9F\x93\x97"] = ":green_book:";
    lEmoticonsEmoi["\xF0\x9F\x92\x8B"] = ":kiss:";
    lEmoticonsEmoi["\xF0\x9F\x93\x98"] = ":blue_book:";
    lEmoticonsEmoi["\xF0\x9F\x92\x8C"] = ":love_letter:";
    lEmoticonsEmoi["\xF0\x9F\x93\x99"] = ":orange_book:";
    lEmoticonsEmoi["\xF0\x9F\x92\x8D"] = ":ring:";
    lEmoticonsEmoi["\xF0\x9F\x93\x9A"] = ":books:";
    lEmoticonsEmoi["\xF0\x9F\x92\x8E"] = ":gem:";
    lEmoticonsEmoi["\xF0\x9F\x93\x9B"] = ":name_badge:";
    lEmoticonsEmoi["\xF0\x9F\x92\x8F"] = ":couplekiss:";
    lEmoticonsEmoi["\xF0\x9F\x93\x9C"] = ":scroll:";
    lEmoticonsEmoi["\xF0\x9F\x92\x90"] = ":bouquet:";
    lEmoticonsEmoi["\xF0\x9F\x93\x9D"] = ":pencil:";
    lEmoticonsEmoi["\xF0\x9F\x92\x91"] = ":couple_with_heart:";
    lEmoticonsEmoi["\xF0\x9F\x93\x9E"] = ":telephone_receiver:";
    lEmoticonsEmoi["\xF0\x9F\x92\x92"] = ":wedding:";
    lEmoticonsEmoi["\xF0\x9F\x93\x9F"] = ":pager:";
    lEmoticonsEmoi["\xF0\x9F\x93\xA0"] = ":fax:";
    lEmoticonsEmoi["\xF0\x9F\x92\x93"] = ":heartbeat:";
    lEmoticonsEmoi["\xF0\x9F\x93\xA1"] = ":satellite:";
    lEmoticonsEmoi["\xF0\x9F\x93\xA2"] = ":loudspeaker:";
    lEmoticonsEmoi["\xF0\x9F\x92\x94"] = ":broken_heart:";
    lEmoticonsEmoi["\xF0\x9F\x93\xA3"] = ":mega:";
    lEmoticonsEmoi["\xF0\x9F\x93\xA4"] = ":outbox_tray:";
    lEmoticonsEmoi["\xF0\x9F\x92\x95"] = ":two_hearts:";
    lEmoticonsEmoi["\xF0\x9F\x93\xA5"] = ":inbox_tray:";
    lEmoticonsEmoi["\xF0\x9F\x93\xA6"] = ":package:";
    lEmoticonsEmoi["\xF0\x9F\x92\x96"] = ":sparkling_heart:";
    lEmoticonsEmoi["\xF0\x9F\x93\xA7"] = ":e-mail:";
    lEmoticonsEmoi["\xF0\x9F\x93\xA7"] = ":email:";
    lEmoticonsEmoi["\xF0\x9F\x93\xA8"] = ":incoming_envelope:";
    lEmoticonsEmoi["\xF0\x9F\x92\x97"] = ":heartpulse:";
    lEmoticonsEmoi["\xF0\x9F\x93\xA9"] = ":envelope_with_arrow:";
    lEmoticonsEmoi["\xF0\x9F\x93\xAA"] = ":mailbox_closed:";
    lEmoticonsEmoi["\xF0\x9F\x92\x98"] = ":cupid:";
    lEmoticonsEmoi["\xF0\x9F\x93\xAB"] = ":mailbox:";
    lEmoticonsEmoi["\xF0\x9F\x93\xAE"] = ":postbox:";
    lEmoticonsEmoi["\xF0\x9F\x92\x99"] = ":blue_heart:";
    lEmoticonsEmoi["\xF0\x9F\x93\xB0"] = ":newspaper:";
    lEmoticonsEmoi["\xF0\x9F\x93\xB1"] = ":iphone:";
    lEmoticonsEmoi["\xF0\x9F\x92\x9A"] = ":green_heart:";
    lEmoticonsEmoi["\xF0\x9F\x93\xB2"] = ":calling:";
    lEmoticonsEmoi["\xF0\x9F\x93\xB3"] = ":vibration_mode:";
    lEmoticonsEmoi["\xF0\x9F\x92\x9B"] = ":yellow_heart:";
    lEmoticonsEmoi["\xF0\x9F\x93\xB4"] = ":mobile_phone_off:";
    lEmoticonsEmoi["\xF0\x9F\x93\xB6"] = ":signal_strength:";
    lEmoticonsEmoi["\xF0\x9F\x92\x9C"] = ":purple_heart:";
    lEmoticonsEmoi["\xF0\x9F\x93\xB7"] = ":camera:";
    lEmoticonsEmoi["\xF0\x9F\x93\xB9"] = ":video_camera:";
    lEmoticonsEmoi["\xF0\x9F\x92\x9D"] = ":gift_heart:";
    lEmoticonsEmoi["\xF0\x9F\x93\xBA"] = ":tv:";
    lEmoticonsEmoi["\xF0\x9F\x93\xBB"] = ":radio:";
    lEmoticonsEmoi["\xF0\x9F\x92\x9E"] = ":revolving_hearts:";
    lEmoticonsEmoi["\xF0\x9F\x93\xBC"] = ":vhs:";
    lEmoticonsEmoi["\xF0\x9F\x94\x83"] = ":arrows_clockwise:";
    lEmoticonsEmoi["\xF0\x9F\x92\x9F"] = ":heart_decoration:";
    lEmoticonsEmoi["\xF0\x9F\x94\x8A"] = ":loud_sound:";
    lEmoticonsEmoi["\xF0\x9F\x94\x8B"] = ":battery:";
    lEmoticonsEmoi["\xF0\x9F\x92\xA0"] = ":diamond_shape_with_a_dot_inside:";
    lEmoticonsEmoi["\xF0\x9F\x94\x8C"] = ":electric_plug:";
    lEmoticonsEmoi["\xF0\x9F\x94\x8D"] = ":mag:";
    lEmoticonsEmoi["\xF0\x9F\x92\xA1"] = ":bulb:";
    lEmoticonsEmoi["\xF0\x9F\x94\x8E"] = ":mag_right:";
    lEmoticonsEmoi["\xF0\x9F\x94\x8F"] = ":lock_with_ink_pen:";
    lEmoticonsEmoi["\xF0\x9F\x92\xA2"] = ":anger:";
    lEmoticonsEmoi["\xF0\x9F\x94\x90"] = ":closed_lock_with_key:";
    lEmoticonsEmoi["\xF0\x9F\x94\x91"] = ":key:";
    lEmoticonsEmoi["\xF0\x9F\x92\xA3"] = ":bomb:";
    lEmoticonsEmoi["\xF0\x9F\x94\x92"] = ":lock:";
    lEmoticonsEmoi["\xF0\x9F\x94\x93"] = ":unlock:";
    lEmoticonsEmoi["\xF0\x9F\x92\xA4"] = ":zzz:";
    lEmoticonsEmoi["\xF0\x9F\x94\x94"] = ":bell:";
    lEmoticonsEmoi["\xF0\x9F\x94\x96"] = ":bookmark:";
    lEmoticonsEmoi["\xF0\x9F\x92\xA5"] = ":boom:";
    lEmoticonsEmoi["\xF0\x9F\x94\x97"] = ":link:";
    lEmoticonsEmoi["\xF0\x9F\x94\x98"] = ":radio_button:";
    lEmoticonsEmoi["\xF0\x9F\x92\xA6"] = ":sweat_drops:";
    lEmoticonsEmoi["\xF0\x9F\x94\x99"] = ":back:";
    lEmoticonsEmoi["\xF0\x9F\x94\x9A"] = ":end:";
    lEmoticonsEmoi["\xF0\x9F\x92\xA7"] = ":droplet:";
    lEmoticonsEmoi["\xF0\x9F\x94\x9B"] = ":on:";
    lEmoticonsEmoi["\xF0\x9F\x94\x9C"] = ":soon:";
    lEmoticonsEmoi["\xF0\x9F\x92\xA8"] = ":dash:";
    lEmoticonsEmoi["\xF0\x9F\x94\x9D"] = ":top:";
    lEmoticonsEmoi["\xF0\x9F\x94\x9E"] = ":underage:";
    lEmoticonsEmoi["\xF0\x9F\x92\xA9"] = ":poop:";
    lEmoticonsEmoi["\xF0\x9F\x92\xA9"] = ":shit:";
    lEmoticonsEmoi["\xF0\x9F\x92\xA9"] = ":hankey:";
    lEmoticonsEmoi["\xF0\x9F\x92\xA9"] = ":poo:";
    lEmoticonsEmoi["\xF0\x9F\x94\x9F"] = ":ten:";
    lEmoticonsEmoi["\xF0\x9F\x92\xAA"] = ":muscle:";
    lEmoticonsEmoi["\xF0\x9F\x94\xA0"] = ":capital_abcd:";
    lEmoticonsEmoi["\xF0\x9F\x94\xA1"] = ":abcd:";
    lEmoticonsEmoi["\xF0\x9F\x92\xAB"] = ":dizzy:";
    lEmoticonsEmoi["\xF0\x9F\x94\xA2"] = ":1234:";
    lEmoticonsEmoi["\xF0\x9F\x94\xA3"] = ":symbols:";
    lEmoticonsEmoi["\xF0\x9F\x92\xAC"] = ":speech_balloon:";
    lEmoticonsEmoi["\xF0\x9F\x94\xA4"] = ":abc:";
    lEmoticonsEmoi["\xF0\x9F\x94\xA5"] = ":fire:";
    lEmoticonsEmoi["\xF0\x9F\x94\xA5"] = ":flame:";
    lEmoticonsEmoi["\xF0\x9F\x92\xAE"] = ":white_flower:";
    lEmoticonsEmoi["\xF0\x9F\x94\xA6"] = ":flashlight:";
    lEmoticonsEmoi["\xF0\x9F\x94\xA7"] = ":wrench:";
    lEmoticonsEmoi["\xF0\x9F\x92\xAF"] = ":100:";
    lEmoticonsEmoi["\xF0\x9F\x94\xA8"] = ":hammer:";
    lEmoticonsEmoi["\xF0\x9F\x94\xA9"] = ":nut_and_bolt:";
    lEmoticonsEmoi["\xF0\x9F\x92\xB0"] = ":moneybag:";
    lEmoticonsEmoi["\xF0\x9F\x94\xAA"] = ":knife:";
    lEmoticonsEmoi["\xF0\x9F\x94\xAB"] = ":gun:";
    lEmoticonsEmoi["\xF0\x9F\x92\xB1"] = ":currency_exchange:";
    lEmoticonsEmoi["\xF0\x9F\x94\xAE"] = ":crystal_ball:";
    lEmoticonsEmoi["\xF0\x9F\x92\xB2"] = ":heavy_dollar_sign:";
    lEmoticonsEmoi["\xF0\x9F\x94\xAF"] = ":six_pointed_star:";
    lEmoticonsEmoi["\xF0\x9F\x92\xB3"] = ":credit_card:";
    lEmoticonsEmoi["\xF0\x9F\x94\xB0"] = ":beginner:";
    lEmoticonsEmoi["\xF0\x9F\x94\xB1"] = ":trident:";
    lEmoticonsEmoi["\xF0\x9F\x92\xB4"] = ":yen:";
    lEmoticonsEmoi["\xF0\x9F\x94\xB2"] = ":black_square_button:";
    lEmoticonsEmoi["\xF0\x9F\x94\xB3"] = ":white_square_button:";
    lEmoticonsEmoi["\xF0\x9F\x92\xB5"] = ":dollar:";
    lEmoticonsEmoi["\xF0\x9F\x94\xB4"] = ":red_circle:";
    lEmoticonsEmoi["\xF0\x9F\x94\xB5"] = ":large_blue_circle:";
    lEmoticonsEmoi["\xF0\x9F\x92\xB8"] = ":money_with_wings:";
    lEmoticonsEmoi["\xF0\x9F\x94\xB6"] = ":large_orange_diamond:";
    lEmoticonsEmoi["\xF0\x9F\x94\xB7"] = ":large_blue_diamond:";
    lEmoticonsEmoi["\xF0\x9F\x92\xB9"] = ":chart:";
    lEmoticonsEmoi["\xF0\x9F\x94\xB8"] = ":small_orange_diamond:";
    lEmoticonsEmoi["\xF0\x9F\x94\xB9"] = ":small_blue_diamond:";
    lEmoticonsEmoi["\xF0\x9F\x92\xBA"] = ":seat:";
    lEmoticonsEmoi["\xF0\x9F\x94\xBA"] = ":small_red_triangle:";
    lEmoticonsEmoi["\xF0\x9F\x94\xBB"] = ":small_red_triangle_down:";
    lEmoticonsEmoi["\xF0\x9F\x92\xBB"] = ":computer:";
    lEmoticonsEmoi["\xF0\x9F\x94\xBC"] = ":arrow_up_small:";
    lEmoticonsEmoi["\xF0\x9F\x92\xBC"] = ":briefcase:";
    lEmoticonsEmoi["\xF0\x9F\x94\xBD"] = ":arrow_down_small:";
    lEmoticonsEmoi["\xF0\x9F\x95\x90"] = ":clock1:";
    lEmoticonsEmoi["\xF0\x9F\x92\xBD"] = ":minidisc:";
    lEmoticonsEmoi["\xF0\x9F\x95\x91"] = ":clock2:";
    lEmoticonsEmoi["\xF0\x9F\x92\xBE"] = ":floppy_disk:";
    lEmoticonsEmoi["\xF0\x9F\x95\x92"] = ":clock3:";
    lEmoticonsEmoi["\xF0\x9F\x92\xBF"] = ":cd:";
    lEmoticonsEmoi["\xF0\x9F\x95\x93"] = ":clock4:";
    lEmoticonsEmoi["\xF0\x9F\x93\x80"] = ":dvd:";
    lEmoticonsEmoi["\xF0\x9F\x95\x94"] = ":clock5:";
    lEmoticonsEmoi["\xF0\x9F\x95\x95"] = ":clock6:";
    lEmoticonsEmoi["\xF0\x9F\x93\x81"] = ":file_folder:";
    lEmoticonsEmoi["\xF0\x9F\x95\x96"] = ":clock7:";
    lEmoticonsEmoi["\xF0\x9F\x95\x97"] = ":clock8:";
    lEmoticonsEmoi["\xF0\x9F\x93\x82"] = ":open_file_folder:";
    lEmoticonsEmoi["\xF0\x9F\x95\x98"] = ":clock9:";
    lEmoticonsEmoi["\xF0\x9F\x95\x99"] = ":clock10:";
    lEmoticonsEmoi["\xF0\x9F\x93\x83"] = ":page_with_curl:";
    lEmoticonsEmoi["\xF0\x9F\x95\x9A"] = ":clock11:";
    lEmoticonsEmoi["\xF0\x9F\x95\x9B"] = ":clock12:";
    lEmoticonsEmoi["\xF0\x9F\x93\x84"] = ":page_facing_up:";
    lEmoticonsEmoi["\xF0\x9F\x97\xBB"] = ":mount_fuji:";
    lEmoticonsEmoi["\xF0\x9F\x97\xBC"] = ":tokyo_tower:";
    lEmoticonsEmoi["\xF0\x9F\x93\x85"] = ":date:";
    lEmoticonsEmoi["\xF0\x9F\x97\xBD"] = ":statue_of_liberty:";
    lEmoticonsEmoi["\xF0\x9F\x97\xBE"] = ":japan:";
    lEmoticonsEmoi["\xF0\x9F\x93\x86"] = ":calendar:";
    lEmoticonsEmoi["\xF0\x9F\x97\xBF"] = ":moyai:";
    lEmoticonsEmoi["\xF0\x9F\x98\x81"] = ":grin:";
    lEmoticonsEmoi["\xF0\x9F\x98\x82"] = ":joy:";
    lEmoticonsEmoi["\xF0\x9F\x98\x83"] = ":smiley:";
    lEmoticonsEmoi["\xF0\x9F\x93\x88"] = ":chart_with_upwards_trend:";
    lEmoticonsEmoi["\xF0\x9F\x98\x84"] = ":smile:";
    lEmoticonsEmoi["\xF0\x9F\x98\x85"] = ":sweat_smile:";
    lEmoticonsEmoi["\xF0\x9F\x93\x89"] = ":chart_with_downwards_trend:";
    lEmoticonsEmoi["\xF0\x9F\x98\x86"] = ":laughing:";
    lEmoticonsEmoi["\xF0\x9F\x98\x86"] = ":satisfied:";
    lEmoticonsEmoi["\xF0\x9F\x98\x89"] = ":wink:";
    lEmoticonsEmoi["\xF0\x9F\x93\x8A"] = ":bar_chart:";
    lEmoticonsEmoi["\xF0\x9F\x98\x8A"] = ":blush:";
    lEmoticonsEmoi["\xF0\x9F\x98\x8B"] = ":yum:";
    lEmoticonsEmoi["\xF0\x9F\x93\x8B"] = ":clipboard:";
    lEmoticonsEmoi["\xF0\x9F\x98\x8C"] = ":relieved:";
    lEmoticonsEmoi["\xF0\x9F\x98\x8D"] = ":heart_eyes:";
    lEmoticonsEmoi["\xF0\x9F\x93\x8C"] = ":pushpin:";
    lEmoticonsEmoi["\xF0\x9F\x98\x8F"] = ":smirk:";
    lEmoticonsEmoi["\xF0\x9F\x98\x92"] = ":unamused:";
    lEmoticonsEmoi["\xF0\x9F\x93\x8D"] = ":round_pushpin:";
    lEmoticonsEmoi["\xF0\x9F\x98\x93"] = ":sweat:";
    lEmoticonsEmoi["\xF0\x9F\x98\x94"] = ":pensive:";
    lEmoticonsEmoi["\xF0\x9F\x93\x8E"] = ":paperclip:";
    lEmoticonsEmoi["\xF0\x9F\x98\x96"] = ":confounded:";
    lEmoticonsEmoi["\xF0\x9F\x98\x98"] = ":kissing_heart:";
    lEmoticonsEmoi["\xF0\x9F\x93\x8F"] = ":straight_ruler:";
    lEmoticonsEmoi["\xF0\x9F\x98\x9A"] = ":kissing_closed_eyes:";
    lEmoticonsEmoi["\xF0\x9F\x98\x9C"] = ":stuck_out_tongue_winking_eye:";
    lEmoticonsEmoi["\xF0\x9F\x93\x90"] = ":triangular_ruler:";
    lEmoticonsEmoi["\xF0\x9F\x98\x9D"] = ":stuck_out_tongue_closed_eyes:";
    lEmoticonsEmoi["\xF0\x9F\x98\x9E"] = ":disappointed:";
    lEmoticonsEmoi["\xF0\x9F\x93\x91"] = ":bookmark_tabs:";
    lEmoticonsEmoi["\xF0\x9F\x98\xA0"] = ":angry:";
    lEmoticonsEmoi["\xF0\x9F\x98\xA1"] = ":rage:";
    lEmoticonsEmoi["\xF0\x9F\x98\xA2"] = ":cry:";
    lEmoticonsEmoi["\xF0\x9F\x98\xA3"] = ":persevere:";
    lEmoticonsEmoi["\xF0\x9F\x98\xA4"] = ":triumph:";
    lEmoticonsEmoi["\xF0\x9F\x98\xA5"] = ":disappointed_relieved:";
    lEmoticonsEmoi["\xF0\x9F\x98\xA8"] = ":fearful:";
    lEmoticonsEmoi["\xF0\x9F\x98\xA9"] = ":weary:";
    lEmoticonsEmoi["\xF0\x9F\x98\xAA"] = ":sleepy:";
    lEmoticonsEmoi["\xF0\x9F\x98\xAB"] = ":tired_face:";
    lEmoticonsEmoi["\xF0\x9F\x98\xAD"] = ":sob:";
    lEmoticonsEmoi["\xF0\x9F\x98\xB0"] = ":cold_sweat:";
    lEmoticonsEmoi["\xF0\x9F\x98\xB1"] = ":scream:";
    lEmoticonsEmoi["\xF0\x9F\x98\xB2"] = ":astonished:";
    lEmoticonsEmoi["\xF0\x9F\x98\xB3"] = ":flushed:";
    lEmoticonsEmoi["\xF0\x9F\x98\xB5"] = ":dizzy_face:";
    lEmoticonsEmoi["\xF0\x9F\x98\xB7"] = ":mask:";
    lEmoticonsEmoi["\xF0\x9F\x98\xB8"] = ":smile_cat:";
    lEmoticonsEmoi["\xF0\x9F\x98\xB9"] = ":joy_cat:";
    lEmoticonsEmoi["\xF0\x9F\x98\xBA"] = ":smiley_cat:";
    lEmoticonsEmoi["\xF0\x9F\x98\xBB"] = ":heart_eyes_cat:";
    lEmoticonsEmoi["\xF0\x9F\x98\xBC"] = ":smirk_cat:";
    lEmoticonsEmoi["\xF0\x9F\x98\xBD"] = ":kissing_cat:";
    lEmoticonsEmoi["\xF0\x9F\x98\xBE"] = ":pouting_cat:";
    lEmoticonsEmoi["\xF0\x9F\x98\xBF"] = ":crying_cat_face:";
    lEmoticonsEmoi["\xF0\x9F\x99\x80"] = ":scream_cat:";
    lEmoticonsEmoi["\xF0\x9F\x99\x85"] = ":no_good:";
    lEmoticonsEmoi["\xF0\x9F\x99\x86"] = ":ok_woman:";
    lEmoticonsEmoi["\xF0\x9F\x99\x87"] = ":bow:";
    lEmoticonsEmoi["\xF0\x9F\x99\x88"] = ":see_no_evil:";
    lEmoticonsEmoi["\xF0\x9F\x99\x89"] = ":hear_no_evil:";
    lEmoticonsEmoi["\xF0\x9F\x99\x8A"] = ":speak_no_evil:";
    lEmoticonsEmoi["\xF0\x9F\x99\x8B"] = ":raising_hand:";
    lEmoticonsEmoi["\xF0\x9F\x99\x8C"] = ":raised_hands:";
    lEmoticonsEmoi["\xF0\x9F\x99\x8D"] = ":person_frowning:";
    lEmoticonsEmoi["\xF0\x9F\x99\x8E"] = ":person_with_pouting_face:";
    lEmoticonsEmoi["\xF0\x9F\x99\x8F"] = ":pray:";
    lEmoticonsEmoi["\xF0\x9F\x9A\x80"] = ":rocket:";
    lEmoticonsEmoi["\xF0\x9F\x9A\x83"] = ":railway_car:";
    lEmoticonsEmoi["\xF0\x9F\x9A\x84"] = ":bullettrain_side:";
    lEmoticonsEmoi["\xF0\x9F\x9A\x85"] = ":bullettrain_front:";
    lEmoticonsEmoi["\xF0\x9F\x9A\x87"] = ":metro:";
    lEmoticonsEmoi["\xF0\x9F\x9A\x89"] = ":station:";
    lEmoticonsEmoi["\xF0\x9F\x9A\x8C"] = ":bus:";
    lEmoticonsEmoi["\xF0\x9F\x9A\x8F"] = ":busstop:";
    lEmoticonsEmoi["\xF0\x9F\x9A\x91"] = ":ambulance:";
    lEmoticonsEmoi["\xF0\x9F\x9A\x92"] = ":fire_engine:";
    lEmoticonsEmoi["\xF0\x9F\x9A\x93"] = ":police_car:";
    lEmoticonsEmoi["\xF0\x9F\x9A\x95"] = ":taxi:";
    lEmoticonsEmoi["\xF0\x9F\x9A\x97"] = ":red_car:";
    lEmoticonsEmoi["\xF0\x9F\x9A\x99"] = ":blue_car:";
    lEmoticonsEmoi["\xF0\x9F\x9A\x9A"] = ":truck:";
    lEmoticonsEmoi["\xF0\x9F\x9A\xA2"] = ":ship:";
    lEmoticonsEmoi["\xF0\x9F\x9A\xA4"] = ":speedboat:";
    lEmoticonsEmoi["\xF0\x9F\x9A\xA5"] = ":traffic_light:";
    lEmoticonsEmoi["\xF0\x9F\x9A\xA7"] = ":construction:";
    lEmoticonsEmoi["\xF0\x9F\x9A\xA8"] = ":rotating_light:";
    lEmoticonsEmoi["\xF0\x9F\x9A\xA9"] = ":triangular_flag_on_post:";
    lEmoticonsEmoi["\xF0\x9F\x9A\xAA"] = ":door:";
    lEmoticonsEmoi["\xF0\x9F\x9A\xAB"] = ":no_entry_sign:";
    lEmoticonsEmoi["\xF0\x9F\x9A\xAC"] = ":smoking:";
    lEmoticonsEmoi["\xF0\x9F\x9A\xAD"] = ":no_smoking:";
    lEmoticonsEmoi["\xF0\x9F\x9A\xB2"] = ":bike:";
    lEmoticonsEmoi["\xF0\x9F\x9A\xB6"] = ":walking:";
    lEmoticonsEmoi["\xF0\x9F\x9A\xB9"] = ":mens:";
    lEmoticonsEmoi["\xF0\x9F\x9A\xBA"] = ":womens:";
    lEmoticonsEmoi["\xF0\x9F\x9A\xBB"] = ":restroom:";
    lEmoticonsEmoi["\xF0\x9F\x9A\xBC"] = ":baby_symbol:";
    lEmoticonsEmoi["\xF0\x9F\x9A\xBD"] = ":toilet:";
    lEmoticonsEmoi["\xF0\x9F\x9A\xBE"] = ":wc:";
    lEmoticonsEmoi["\xF0\x9F\x9B\x80"] = ":bath:";
    lEmoticonsEmoi["\xF0\x9F\xA4\x98"] = ":metal:";
    lEmoticonsEmoi["\xF0\x9F\xA4\x98"] = ":sign_of_the_horns:";
    lEmoticonsEmoi["\xF0\x9F\x98\x80"] = ":grinning:";
    lEmoticonsEmoi["\xF0\x9F\x98\x87"] = ":innocent:";
    lEmoticonsEmoi["\xF0\x9F\x98\x88"] = ":smiling_imp:";
    lEmoticonsEmoi["\xF0\x9F\x98\x8E"] = ":sunglasses:";
    lEmoticonsEmoi["\xF0\x9F\x98\x90"] = ":neutral_face:";
    lEmoticonsEmoi["\xF0\x9F\x98\x91"] = ":expressionless:";
    lEmoticonsEmoi["\xF0\x9F\x98\x95"] = ":confused:";
    lEmoticonsEmoi["\xF0\x9F\x98\x97"] = ":kissing:";
    lEmoticonsEmoi["\xF0\x9F\x98\x99"] = ":kissing_smiling_eyes:";
    lEmoticonsEmoi["\xF0\x9F\x98\x9B"] = ":stuck_out_tongue:";
    lEmoticonsEmoi["\xF0\x9F\x98\x9F"] = ":worried:";
    lEmoticonsEmoi["\xF0\x9F\x98\xA6"] = ":frowning:";
    lEmoticonsEmoi["\xF0\x9F\x98\xA7"] = ":anguished:";
    lEmoticonsEmoi["\xF0\x9F\x98\xAC"] = ":grimacing:";
    lEmoticonsEmoi["\xF0\x9F\x98\xAE"] = ":open_mouth:";
    lEmoticonsEmoi["\xF0\x9F\x98\xAF"] = ":hushed:";
    lEmoticonsEmoi["\xF0\x9F\x98\xB4"] = ":sleeping:";
    lEmoticonsEmoi["\xF0\x9F\x98\xB6"] = ":no_mouth:";
    lEmoticonsEmoi["\xF0\x9F\x9A\x81"] = ":helicopter:";
    lEmoticonsEmoi["\xF0\x9F\x9A\x82"] = ":steam_locomotive:";
    lEmoticonsEmoi["\xF0\x9F\x9A\x86"] = ":train2:";
    lEmoticonsEmoi["\xF0\x9F\x9A\x88"] = ":light_rail:";
    lEmoticonsEmoi["\xF0\x9F\x9A\x8A"] = ":tram:";
    lEmoticonsEmoi["\xF0\x9F\x9A\x8D"] = ":oncoming_bus:";
    lEmoticonsEmoi["\xF0\x9F\x9A\x8E"] = ":trolleybus:";
    lEmoticonsEmoi["\xF0\x9F\x9A\x90"] = ":minibus:";
    lEmoticonsEmoi["\xF0\x9F\x9A\x94"] = ":oncoming_police_car:";
    lEmoticonsEmoi["\xF0\x9F\x9A\x96"] = ":oncoming_taxi:";
    lEmoticonsEmoi["\xF0\x9F\x9A\x98"] = ":oncoming_automobile:";
    lEmoticonsEmoi["\xF0\x9F\x9A\x9B"] = ":articulated_lorry:";
    lEmoticonsEmoi["\xF0\x9F\x9A\x9C"] = ":tractor:";
    lEmoticonsEmoi["\xF0\x9F\x9A\x9D"] = ":monorail:";
    lEmoticonsEmoi["\xF0\x9F\x9A\x9E"] = ":mountain_railway:";
    lEmoticonsEmoi["\xF0\x9F\x9A\x9F"] = ":suspension_railway:";
    lEmoticonsEmoi["\xF0\x9F\x9A\xA0"] = ":mountain_cableway:";
    lEmoticonsEmoi["\xF0\x9F\x9A\xA1"] = ":aerial_tramway:";
    lEmoticonsEmoi["\xF0\x9F\x9A\xA3"] = ":rowboat:";
    lEmoticonsEmoi["\xF0\x9F\x9A\xA6"] = ":vertical_traffic_light:";
    lEmoticonsEmoi["\xF0\x9F\x9A\xAE"] = ":put_litter_in_its_place:";
    lEmoticonsEmoi["\xF0\x9F\x9A\xAF"] = ":do_not_litter:";
    lEmoticonsEmoi["\xF0\x9F\x9A\xB0"] = ":potable_water:";
    lEmoticonsEmoi["\xF0\x9F\x9A\xB1"] = ":non-potable_water:";
    lEmoticonsEmoi["\xF0\x9F\x9A\xB3"] = ":no_bicycles:";
    lEmoticonsEmoi["\xF0\x9F\x9A\xB4"] = ":bicyclist:";
    lEmoticonsEmoi["\xF0\x9F\x9A\xB5"] = ":mountain_bicyclist:";
    lEmoticonsEmoi["\xF0\x9F\x9A\xB7"] = ":no_pedestrians:";
    lEmoticonsEmoi["\xF0\x9F\x9A\xB8"] = ":children_crossing:";
    lEmoticonsEmoi["\xF0\x9F\x9A\xBF"] = ":shower:";
    lEmoticonsEmoi["\xF0\x9F\x9B\x81"] = ":bathtub:";
    lEmoticonsEmoi["\xF0\x9F\x9B\x82"] = ":passport_control:";
    lEmoticonsEmoi["\xF0\x9F\x9B\x83"] = ":customs:";
    lEmoticonsEmoi["\xF0\x9F\x9B\x84"] = ":baggage_claim:";
    lEmoticonsEmoi["\xF0\x9F\x9B\x85"] = ":left_luggage:";
    lEmoticonsEmoi["\xF0\x9F\x8C\x8D"] = ":earth_africa:";
    lEmoticonsEmoi["\xF0\x9F\x8C\x8E"] = ":earth_americas:";
    lEmoticonsEmoi["\xF0\x9F\x8C\x90"] = ":globe_with_meridians:";
    lEmoticonsEmoi["\xF0\x9F\x8C\x92"] = ":waxing_crescent_moon:";
    lEmoticonsEmoi["\xF0\x9F\x8C\x96"] = ":waning_gibbous_moon:";
    lEmoticonsEmoi["\xF0\x9F\x8C\x97"] = ":last_quarter_moon:";
    lEmoticonsEmoi["\xF0\x9F\x8C\x98"] = ":waning_crescent_moon:";
    lEmoticonsEmoi["\xF0\x9F\x8C\x9A"] = ":new_moon_with_face:";
    lEmoticonsEmoi["\xF0\x9F\x8C\x9C"] = ":last_quarter_moon_with_face:";
    lEmoticonsEmoi["\xF0\x9F\x8C\x9D"] = ":full_moon_with_face:";
    lEmoticonsEmoi["\xF0\x9F\x8C\x9E"] = ":sun_with_face:";
    lEmoticonsEmoi["\xF0\x9F\x8C\xB2"] = ":evergreen_tree:";
    lEmoticonsEmoi["\xF0\x9F\x8C\xB3"] = ":deciduous_tree:";
    lEmoticonsEmoi["\xF0\x9F\x8D\x8B"] = ":lemon:";
    lEmoticonsEmoi["\xF0\x9F\x8D\x90"] = ":pear:";
    lEmoticonsEmoi["\xF0\x9F\x8D\xBC"] = ":baby_bottle:";
    lEmoticonsEmoi["\xF0\x9F\x8F\x87"] = ":horse_racing:";
    lEmoticonsEmoi["\xF0\x9F\x8F\x89"] = ":rugby_football:";
    lEmoticonsEmoi["\xF0\x9F\x8F\xA4"] = ":european_post_office:";
    lEmoticonsEmoi["\xF0\x9F\x90\x80"] = ":rat:";
    lEmoticonsEmoi["\xF0\x9F\x90\x81"] = ":mouse2:";
    lEmoticonsEmoi["\xF0\x9F\x90\x82"] = ":ox:";
    lEmoticonsEmoi["\xF0\x9F\x90\x83"] = ":water_buffalo:";
    lEmoticonsEmoi["\xF0\x9F\x90\x84"] = ":cow2:";
    lEmoticonsEmoi["\xF0\x9F\x90\x85"] = ":tiger2:";
    lEmoticonsEmoi["\xF0\x9F\x90\x86"] = ":leopard:";
    lEmoticonsEmoi["\xF0\x9F\x90\x87"] = ":rabbit2:";
    lEmoticonsEmoi["\xF0\x9F\x90\x88"] = ":cat2:";
    lEmoticonsEmoi["\xF0\x9F\x90\x89"] = ":dragon:";
    lEmoticonsEmoi["\xF0\x9F\x90\x8A"] = ":crocodile:";
    lEmoticonsEmoi["\xF0\x9F\x90\x8B"] = ":whale2:";
    lEmoticonsEmoi["\xF0\x9F\x90\x8F"] = ":ram:";
    lEmoticonsEmoi["\xF0\x9F\x90\x90"] = ":goat:";
    lEmoticonsEmoi["\xF0\x9F\x90\x93"] = ":rooster:";
    lEmoticonsEmoi["\xF0\x9F\x90\x95"] = ":dog2:";
    lEmoticonsEmoi["\xF0\x9F\x90\x96"] = ":pig2:";
    lEmoticonsEmoi["\xF0\x9F\x90\xAA"] = ":dromedary_camel:";
    lEmoticonsEmoi["\xF0\x9F\x91\xA5"] = ":busts_in_silhouette:";
    lEmoticonsEmoi["\xF0\x9F\x91\xAC"] = ":two_men_holding_hands:";
    lEmoticonsEmoi["\xF0\x9F\x91\xAD"] = ":two_women_holding_hands:";
    lEmoticonsEmoi["\xF0\x9F\x92\xAD"] = ":thought_balloon:";
    lEmoticonsEmoi["\xF0\x9F\x92\xB6"] = ":euro:";
    lEmoticonsEmoi["\xF0\x9F\x92\xB7"] = ":pound:";
    lEmoticonsEmoi["\xF0\x9F\x93\xAC"] = ":mailbox_with_mail:";
    lEmoticonsEmoi["\xF0\x9F\x93\xAD"] = ":mailbox_with_no_mail:";
    lEmoticonsEmoi["\xF0\x9F\x93\xAF"] = ":postal_horn:";
    lEmoticonsEmoi["\xF0\x9F\x93\xB5"] = ":no_mobile_phones:";
    lEmoticonsEmoi["\xF0\x9F\x94\x80"] = ":twisted_rightwards_arrows:";
    lEmoticonsEmoi["\xF0\x9F\x94\x81"] = ":repeat:";
    lEmoticonsEmoi["\xF0\x9F\x94\x82"] = ":repeat_one:";
    lEmoticonsEmoi["\xF0\x9F\x94\x84"] = ":arrows_counterclockwise:";
    lEmoticonsEmoi["\xF0\x9F\x94\x85"] = ":low_brightness:";
    lEmoticonsEmoi["\xF0\x9F\x94\x86"] = ":high_brightness:";
    lEmoticonsEmoi["\xF0\x9F\x94\x87"] = ":mute:";
    lEmoticonsEmoi["\xF0\x9F\x94\x89"] = ":sound:";
    lEmoticonsEmoi["\xF0\x9F\x94\x95"] = ":no_bell:";
    lEmoticonsEmoi["\xF0\x9F\x94\xAC"] = ":microscope:";
    lEmoticonsEmoi["\xF0\x9F\x94\xAD"] = ":telescope:";
    lEmoticonsEmoi["\xF0\x9F\x95\x9C"] = ":clock130:";
    lEmoticonsEmoi["\xF0\x9F\x95\x9D"] = ":clock230:";
    lEmoticonsEmoi["\xF0\x9F\x95\x9E"] = ":clock330:";
    lEmoticonsEmoi["\xF0\x9F\x95\x9F"] = ":clock430:";
    lEmoticonsEmoi["\xF0\x9F\x95\xA0"] = ":clock530:";
    lEmoticonsEmoi["\xF0\x9F\x95\xA1"] = ":clock630:";
    lEmoticonsEmoi["\xF0\x9F\x95\xA2"] = ":clock730:";
    lEmoticonsEmoi["\xF0\x9F\x95\xA3"] = ":clock830:";
    lEmoticonsEmoi["\xF0\x9F\x95\xA4"] = ":clock930:";
    lEmoticonsEmoi["\xF0\x9F\x95\xA5"] = ":clock1030:";
    lEmoticonsEmoi["\xF0\x9F\x95\xA6"] = ":clock1130:";
    lEmoticonsEmoi["\xF0\x9F\x95\xA7"] = ":clock1230:";
    lEmoticonsEmoi["\xF0\x9F\x94\x88"] = ":speaker:";
    lEmoticonsEmoi["\xF0\x9F\x9A\x8B"] = ":train:";
    lEmoticonsEmoi["\xE2\x9E\xBF"] = ":loop:";
    lEmoticonsEmoi["\xF0\x9F\x87\xA6\xF0\x9F\x87\xAB"] = ":flag_af:";
    lEmoticonsEmoi["\xF0\x9F\x87\xA6\xF0\x9F\x87\xAB"] = ":af:";
    lEmoticonsEmoi["\xF0\x9F\x87\xA6\xF0\x9F\x87\xB1"] = ":flag_al:";
    lEmoticonsEmoi["\xF0\x9F\x87\xA6\xF0\x9F\x87\xB1"] = ":al:";
    lEmoticonsEmoi["\xF0\x9F\x87\xA9\xF0\x9F\x87\xBF"] = ":flag_dz:";
    lEmoticonsEmoi["\xF0\x9F\x87\xA9\xF0\x9F\x87\xBF"] = ":dz:";
    lEmoticonsEmoi["\xF0\x9F\x87\xA6\xF0\x9F\x87\xA9"] = ":flag_ad:";
    lEmoticonsEmoi["\xF0\x9F\x87\xA6\xF0\x9F\x87\xA9"] = ":ad:";
    lEmoticonsEmoi["\xF0\x9F\x87\xA6\xF0\x9F\x87\xB4"] = ":flag_ao:";
    lEmoticonsEmoi["\xF0\x9F\x87\xA6\xF0\x9F\x87\xB4"] = ":ao:";
    lEmoticonsEmoi["\xF0\x9F\x87\xA6\xF0\x9F\x87\xAC"] = ":flag_ag:";
    lEmoticonsEmoi["\xF0\x9F\x87\xA6\xF0\x9F\x87\xAC"] = ":ag:";
    lEmoticonsEmoi["\xF0\x9F\x87\xA6\xF0\x9F\x87\xB7"] = ":flag_ar:";
    lEmoticonsEmoi["\xF0\x9F\x87\xA6\xF0\x9F\x87\xB7"] = ":ar:";
    lEmoticonsEmoi["\xF0\x9F\x87\xA6\xF0\x9F\x87\xB2"] = ":flag_am:";
    lEmoticonsEmoi["\xF0\x9F\x87\xA6\xF0\x9F\x87\xB2"] = ":am:";
    lEmoticonsEmoi["\xF0\x9F\x87\xA6\xF0\x9F\x87\xBA"] = ":flag_au:";
    lEmoticonsEmoi["\xF0\x9F\x87\xA6\xF0\x9F\x87\xBA"] = ":au:";
    lEmoticonsEmoi["\xF0\x9F\x87\xA6\xF0\x9F\x87\xB9"] = ":flag_at:";
    lEmoticonsEmoi["\xF0\x9F\x87\xA6\xF0\x9F\x87\xB9"] = ":at:";
    lEmoticonsEmoi["\xF0\x9F\x87\xA6\xF0\x9F\x87\xBF"] = ":flag_az:";
    lEmoticonsEmoi["\xF0\x9F\x87\xA6\xF0\x9F\x87\xBF"] = ":az:";
    lEmoticonsEmoi["\xF0\x9F\x87\xA7\xF0\x9F\x87\xB8"] = ":flag_bs:";
    lEmoticonsEmoi["\xF0\x9F\x87\xA7\xF0\x9F\x87\xB8"] = ":bs:";
    lEmoticonsEmoi["\xF0\x9F\x87\xA7\xF0\x9F\x87\xAD"] = ":flag_bh:";
    lEmoticonsEmoi["\xF0\x9F\x87\xA7\xF0\x9F\x87\xAD"] = ":bh:";
    lEmoticonsEmoi["\xF0\x9F\x87\xA7\xF0\x9F\x87\xA9"] = ":flag_bd:";
    lEmoticonsEmoi["\xF0\x9F\x87\xA7\xF0\x9F\x87\xA9"] = ":bd:";
    lEmoticonsEmoi["\xF0\x9F\x87\xA7\xF0\x9F\x87\xA7"] = ":flag_bb:";
    lEmoticonsEmoi["\xF0\x9F\x87\xA7\xF0\x9F\x87\xA7"] = ":bb:";
    lEmoticonsEmoi["\xF0\x9F\x87\xA7\xF0\x9F\x87\xBE"] = ":flag_by:";
    lEmoticonsEmoi["\xF0\x9F\x87\xA7\xF0\x9F\x87\xBE"] = ":by:";
    lEmoticonsEmoi["\xF0\x9F\x87\xA7\xF0\x9F\x87\xAA"] = ":flag_be:";
    lEmoticonsEmoi["\xF0\x9F\x87\xA7\xF0\x9F\x87\xAA"] = ":be:";
    lEmoticonsEmoi["\xF0\x9F\x87\xA7\xF0\x9F\x87\xBF"] = ":flag_bz:";
    lEmoticonsEmoi["\xF0\x9F\x87\xA7\xF0\x9F\x87\xBF"] = ":bz:";
    lEmoticonsEmoi["\xF0\x9F\x87\xA7\xF0\x9F\x87\xAF"] = ":flag_bj:";
    lEmoticonsEmoi["\xF0\x9F\x87\xA7\xF0\x9F\x87\xAF"] = ":bj:";
    lEmoticonsEmoi["\xF0\x9F\x87\xA7\xF0\x9F\x87\xB9"] = ":flag_bt:";
    lEmoticonsEmoi["\xF0\x9F\x87\xA7\xF0\x9F\x87\xB9"] = ":bt:";
    lEmoticonsEmoi["\xF0\x9F\x87\xA7\xF0\x9F\x87\xB4"] = ":flag_bo:";
    lEmoticonsEmoi["\xF0\x9F\x87\xA7\xF0\x9F\x87\xB4"] = ":bo:";
    lEmoticonsEmoi["\xF0\x9F\x87\xA7\xF0\x9F\x87\xA6"] = ":flag_ba:";
    lEmoticonsEmoi["\xF0\x9F\x87\xA7\xF0\x9F\x87\xA6"] = ":ba:";
    lEmoticonsEmoi["\xF0\x9F\x87\xA7\xF0\x9F\x87\xBC"] = ":flag_bw:";
    lEmoticonsEmoi["\xF0\x9F\x87\xA7\xF0\x9F\x87\xBC"] = ":bw:";
    lEmoticonsEmoi["\xF0\x9F\x87\xA7\xF0\x9F\x87\xB7"] = ":flag_br:";
    lEmoticonsEmoi["\xF0\x9F\x87\xA7\xF0\x9F\x87\xB7"] = ":br:";
    lEmoticonsEmoi["\xF0\x9F\x87\xA7\xF0\x9F\x87\xB3"] = ":flag_bn:";
    lEmoticonsEmoi["\xF0\x9F\x87\xA7\xF0\x9F\x87\xB3"] = ":bn:";
    lEmoticonsEmoi["\xF0\x9F\x87\xA7\xF0\x9F\x87\xAC"] = ":flag_bg:";
    lEmoticonsEmoi["\xF0\x9F\x87\xA7\xF0\x9F\x87\xAC"] = ":bg:";
    lEmoticonsEmoi["\xF0\x9F\x87\xA7\xF0\x9F\x87\xAB"] = ":flag_bf:";
    lEmoticonsEmoi["\xF0\x9F\x87\xA7\xF0\x9F\x87\xAB"] = ":bf:";
    lEmoticonsEmoi["\xF0\x9F\x87\xA7\xF0\x9F\x87\xAE"] = ":flag_bi:";
    lEmoticonsEmoi["\xF0\x9F\x87\xA7\xF0\x9F\x87\xAE"] = ":bi:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB0\xF0\x9F\x87\xAD"] = ":flag_kh:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB0\xF0\x9F\x87\xAD"] = ":kh:";
    lEmoticonsEmoi["\xF0\x9F\x87\xA8\xF0\x9F\x87\xB2"] = ":flag_cm:";
    lEmoticonsEmoi["\xF0\x9F\x87\xA8\xF0\x9F\x87\xB2"] = ":cm:";
    lEmoticonsEmoi["\xF0\x9F\x87\xA8\xF0\x9F\x87\xA6"] = ":flag_ca:";
    lEmoticonsEmoi["\xF0\x9F\x87\xA8\xF0\x9F\x87\xA6"] = ":ca:";
    lEmoticonsEmoi["\xF0\x9F\x87\xA8\xF0\x9F\x87\xBB"] = ":flag_cv:";
    lEmoticonsEmoi["\xF0\x9F\x87\xA8\xF0\x9F\x87\xBB"] = ":cv:";
    lEmoticonsEmoi["\xF0\x9F\x87\xA8\xF0\x9F\x87\xAB"] = ":flag_cf:";
    lEmoticonsEmoi["\xF0\x9F\x87\xA8\xF0\x9F\x87\xAB"] = ":cf:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB9\xF0\x9F\x87\xA9"] = ":flag_td:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB9\xF0\x9F\x87\xA9"] = ":td:";
    lEmoticonsEmoi["\xF0\x9F\x87\xA8\xF0\x9F\x87\xB1"] = ":flag_cl:";
    lEmoticonsEmoi["\xF0\x9F\x87\xA8\xF0\x9F\x87\xB1"] = ":chile:";
    lEmoticonsEmoi["\xF0\x9F\x87\xA8\xF0\x9F\x87\xB4"] = ":flag_co:";
    lEmoticonsEmoi["\xF0\x9F\x87\xA8\xF0\x9F\x87\xB4"] = ":co:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB0\xF0\x9F\x87\xB2"] = ":flag_km:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB0\xF0\x9F\x87\xB2"] = ":km:";
    lEmoticonsEmoi["\xF0\x9F\x87\xA8\xF0\x9F\x87\xB7"] = ":flag_cr:";
    lEmoticonsEmoi["\xF0\x9F\x87\xA8\xF0\x9F\x87\xB7"] = ":cr:";
    lEmoticonsEmoi["\xF0\x9F\x87\xA8\xF0\x9F\x87\xAE"] = ":flag_ci:";
    lEmoticonsEmoi["\xF0\x9F\x87\xA8\xF0\x9F\x87\xAE"] = ":ci:";
    lEmoticonsEmoi["\xF0\x9F\x87\xAD\xF0\x9F\x87\xB7"] = ":flag_hr:";
    lEmoticonsEmoi["\xF0\x9F\x87\xAD\xF0\x9F\x87\xB7"] = ":hr:";
    lEmoticonsEmoi["\xF0\x9F\x87\xA8\xF0\x9F\x87\xBA"] = ":flag_cu:";
    lEmoticonsEmoi["\xF0\x9F\x87\xA8\xF0\x9F\x87\xBA"] = ":cu:";
    lEmoticonsEmoi["\xF0\x9F\x87\xA8\xF0\x9F\x87\xBE"] = ":flag_cy:";
    lEmoticonsEmoi["\xF0\x9F\x87\xA8\xF0\x9F\x87\xBE"] = ":cy:";
    lEmoticonsEmoi["\xF0\x9F\x87\xA8\xF0\x9F\x87\xBF"] = ":flag_cz:";
    lEmoticonsEmoi["\xF0\x9F\x87\xA8\xF0\x9F\x87\xBF"] = ":cz:";
    lEmoticonsEmoi["\xF0\x9F\x87\xA8\xF0\x9F\x87\xA9"] = ":flag_cd:";
    lEmoticonsEmoi["\xF0\x9F\x87\xA8\xF0\x9F\x87\xA9"] = ":congo:";
    lEmoticonsEmoi["\xF0\x9F\x87\xA9\xF0\x9F\x87\xB0"] = ":flag_dk:";
    lEmoticonsEmoi["\xF0\x9F\x87\xA9\xF0\x9F\x87\xB0"] = ":dk:";
    lEmoticonsEmoi["\xF0\x9F\x87\xA9\xF0\x9F\x87\xAF"] = ":flag_dj:";
    lEmoticonsEmoi["\xF0\x9F\x87\xA9\xF0\x9F\x87\xAF"] = ":dj:";
    lEmoticonsEmoi["\xF0\x9F\x87\xA9\xF0\x9F\x87\xB2"] = ":flag_dm:";
    lEmoticonsEmoi["\xF0\x9F\x87\xA9\xF0\x9F\x87\xB2"] = ":dm:";
    lEmoticonsEmoi["\xF0\x9F\x87\xA9\xF0\x9F\x87\xB4"] = ":flag_do:";
    lEmoticonsEmoi["\xF0\x9F\x87\xA9\xF0\x9F\x87\xB4"] = ":do:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB9\xF0\x9F\x87\xB1"] = ":flag_tl:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB9\xF0\x9F\x87\xB1"] = ":tl:";
    lEmoticonsEmoi["\xF0\x9F\x87\xAA\xF0\x9F\x87\xA8"] = ":flag_ec:";
    lEmoticonsEmoi["\xF0\x9F\x87\xAA\xF0\x9F\x87\xA8"] = ":ec:";
    lEmoticonsEmoi["\xF0\x9F\x87\xAA\xF0\x9F\x87\xAC"] = ":flag_eg:";
    lEmoticonsEmoi["\xF0\x9F\x87\xAA\xF0\x9F\x87\xAC"] = ":eg:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB8\xF0\x9F\x87\xBB"] = ":flag_sv:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB8\xF0\x9F\x87\xBB"] = ":sv:";
    lEmoticonsEmoi["\xF0\x9F\x87\xAC\xF0\x9F\x87\xB6"] = ":flag_gq:";
    lEmoticonsEmoi["\xF0\x9F\x87\xAC\xF0\x9F\x87\xB6"] = ":gq:";
    lEmoticonsEmoi["\xF0\x9F\x87\xAA\xF0\x9F\x87\xB7"] = ":flag_er:";
    lEmoticonsEmoi["\xF0\x9F\x87\xAA\xF0\x9F\x87\xB7"] = ":er:";
    lEmoticonsEmoi["\xF0\x9F\x87\xAA\xF0\x9F\x87\xAA"] = ":flag_ee:";
    lEmoticonsEmoi["\xF0\x9F\x87\xAA\xF0\x9F\x87\xAA"] = ":ee:";
    lEmoticonsEmoi["\xF0\x9F\x87\xAA\xF0\x9F\x87\xB9"] = ":flag_et:";
    lEmoticonsEmoi["\xF0\x9F\x87\xAA\xF0\x9F\x87\xB9"] = ":et:";
    lEmoticonsEmoi["\xF0\x9F\x87\xAB\xF0\x9F\x87\xAF"] = ":flag_fj:";
    lEmoticonsEmoi["\xF0\x9F\x87\xAB\xF0\x9F\x87\xAF"] = ":fj:";
    lEmoticonsEmoi["\xF0\x9F\x87\xAB\xF0\x9F\x87\xAE"] = ":flag_fi:";
    lEmoticonsEmoi["\xF0\x9F\x87\xAB\xF0\x9F\x87\xAE"] = ":fi:";
    lEmoticonsEmoi["\xF0\x9F\x87\xAC\xF0\x9F\x87\xA6"] = ":flag_ga:";
    lEmoticonsEmoi["\xF0\x9F\x87\xAC\xF0\x9F\x87\xA6"] = ":ga:";
    lEmoticonsEmoi["\xF0\x9F\x87\xAC\xF0\x9F\x87\xB2"] = ":flag_gm:";
    lEmoticonsEmoi["\xF0\x9F\x87\xAC\xF0\x9F\x87\xB2"] = ":gm:";
    lEmoticonsEmoi["\xF0\x9F\x87\xAC\xF0\x9F\x87\xAA"] = ":flag_ge:";
    lEmoticonsEmoi["\xF0\x9F\x87\xAC\xF0\x9F\x87\xAA"] = ":ge:";
    lEmoticonsEmoi["\xF0\x9F\x87\xAC\xF0\x9F\x87\xAD"] = ":flag_gh:";
    lEmoticonsEmoi["\xF0\x9F\x87\xAC\xF0\x9F\x87\xAD"] = ":gh:";
    lEmoticonsEmoi["\xF0\x9F\x87\xAC\xF0\x9F\x87\xB7"] = ":flag_gr:";
    lEmoticonsEmoi["\xF0\x9F\x87\xAC\xF0\x9F\x87\xB7"] = ":gr:";
    lEmoticonsEmoi["\xF0\x9F\x87\xAC\xF0\x9F\x87\xA9"] = ":flag_gd:";
    lEmoticonsEmoi["\xF0\x9F\x87\xAC\xF0\x9F\x87\xA9"] = ":gd:";
    lEmoticonsEmoi["\xF0\x9F\x87\xAC\xF0\x9F\x87\xB9"] = ":flag_gt:";
    lEmoticonsEmoi["\xF0\x9F\x87\xAC\xF0\x9F\x87\xB9"] = ":gt:";
    lEmoticonsEmoi["\xF0\x9F\x87\xAC\xF0\x9F\x87\xB3"] = ":flag_gn:";
    lEmoticonsEmoi["\xF0\x9F\x87\xAC\xF0\x9F\x87\xB3"] = ":gn:";
    lEmoticonsEmoi["\xF0\x9F\x87\xAC\xF0\x9F\x87\xBC"] = ":flag_gw:";
    lEmoticonsEmoi["\xF0\x9F\x87\xAC\xF0\x9F\x87\xBC"] = ":gw:";
    lEmoticonsEmoi["\xF0\x9F\x87\xAC\xF0\x9F\x87\xBE"] = ":flag_gy:";
    lEmoticonsEmoi["\xF0\x9F\x87\xAC\xF0\x9F\x87\xBE"] = ":gy:";
    lEmoticonsEmoi["\xF0\x9F\x87\xAD\xF0\x9F\x87\xB9"] = ":flag_ht:";
    lEmoticonsEmoi["\xF0\x9F\x87\xAD\xF0\x9F\x87\xB9"] = ":ht:";
    lEmoticonsEmoi["\xF0\x9F\x87\xAD\xF0\x9F\x87\xB3"] = ":flag_hn:";
    lEmoticonsEmoi["\xF0\x9F\x87\xAD\xF0\x9F\x87\xB3"] = ":hn:";
    lEmoticonsEmoi["\xF0\x9F\x87\xAD\xF0\x9F\x87\xBA"] = ":flag_hu:";
    lEmoticonsEmoi["\xF0\x9F\x87\xAD\xF0\x9F\x87\xBA"] = ":hu:";
    lEmoticonsEmoi["\xF0\x9F\x87\xAE\xF0\x9F\x87\xB8"] = ":flag_is:";
    lEmoticonsEmoi["\xF0\x9F\x87\xAE\xF0\x9F\x87\xB8"] = ":is:";
    lEmoticonsEmoi["\xF0\x9F\x87\xAE\xF0\x9F\x87\xB3"] = ":flag_in:";
    lEmoticonsEmoi["\xF0\x9F\x87\xAE\xF0\x9F\x87\xB3"] = ":in:";
    lEmoticonsEmoi["\xF0\x9F\x87\xAE\xF0\x9F\x87\xA9"] = ":flag_id:";
    lEmoticonsEmoi["\xF0\x9F\x87\xAE\xF0\x9F\x87\xA9"] = ":indonesia:";
    lEmoticonsEmoi["\xF0\x9F\x87\xAE\xF0\x9F\x87\xB7"] = ":flag_ir:";
    lEmoticonsEmoi["\xF0\x9F\x87\xAE\xF0\x9F\x87\xB7"] = ":ir:";
    lEmoticonsEmoi["\xF0\x9F\x87\xAE\xF0\x9F\x87\xB6"] = ":flag_iq:";
    lEmoticonsEmoi["\xF0\x9F\x87\xAE\xF0\x9F\x87\xB6"] = ":iq:";
    lEmoticonsEmoi["\xF0\x9F\x87\xAE\xF0\x9F\x87\xAA"] = ":flag_ie:";
    lEmoticonsEmoi["\xF0\x9F\x87\xAE\xF0\x9F\x87\xAA"] = ":ie:";
    lEmoticonsEmoi["\xF0\x9F\x87\xAE\xF0\x9F\x87\xB1"] = ":flag_il:";
    lEmoticonsEmoi["\xF0\x9F\x87\xAE\xF0\x9F\x87\xB1"] = ":il:";
    lEmoticonsEmoi["\xF0\x9F\x87\xAF\xF0\x9F\x87\xB2"] = ":flag_jm:";
    lEmoticonsEmoi["\xF0\x9F\x87\xAF\xF0\x9F\x87\xB2"] = ":jm:";
    lEmoticonsEmoi["\xF0\x9F\x87\xAF\xF0\x9F\x87\xB4"] = ":flag_jo:";
    lEmoticonsEmoi["\xF0\x9F\x87\xAF\xF0\x9F\x87\xB4"] = ":jo:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB0\xF0\x9F\x87\xBF"] = ":flag_kz:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB0\xF0\x9F\x87\xBF"] = ":kz:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB0\xF0\x9F\x87\xAA"] = ":flag_ke:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB0\xF0\x9F\x87\xAA"] = ":ke:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB0\xF0\x9F\x87\xAE"] = ":flag_ki:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB0\xF0\x9F\x87\xAE"] = ":ki:";
    lEmoticonsEmoi["\xF0\x9F\x87\xBD\xF0\x9F\x87\xB0"] = ":flag_xk:";
    lEmoticonsEmoi["\xF0\x9F\x87\xBD\xF0\x9F\x87\xB0"] = ":xk:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB0\xF0\x9F\x87\xBC"] = ":flag_kw:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB0\xF0\x9F\x87\xBC"] = ":kw:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB0\xF0\x9F\x87\xAC"] = ":flag_kg:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB0\xF0\x9F\x87\xAC"] = ":kg:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB1\xF0\x9F\x87\xA6"] = ":flag_la:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB1\xF0\x9F\x87\xA6"] = ":la:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB1\xF0\x9F\x87\xBB"] = ":flag_lv:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB1\xF0\x9F\x87\xBB"] = ":lv:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB1\xF0\x9F\x87\xA7"] = ":flag_lb:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB1\xF0\x9F\x87\xA7"] = ":lb:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB1\xF0\x9F\x87\xB8"] = ":flag_ls:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB1\xF0\x9F\x87\xB8"] = ":ls:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB1\xF0\x9F\x87\xB7"] = ":flag_lr:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB1\xF0\x9F\x87\xB7"] = ":lr:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB1\xF0\x9F\x87\xBE"] = ":flag_ly:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB1\xF0\x9F\x87\xBE"] = ":ly:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB1\xF0\x9F\x87\xAE"] = ":flag_li:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB1\xF0\x9F\x87\xAE"] = ":li:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB1\xF0\x9F\x87\xB9"] = ":flag_lt:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB1\xF0\x9F\x87\xB9"] = ":lt:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB1\xF0\x9F\x87\xBA"] = ":flag_lu:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB1\xF0\x9F\x87\xBA"] = ":lu:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB2\xF0\x9F\x87\xB0"] = ":flag_mk:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB2\xF0\x9F\x87\xB0"] = ":mk:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB2\xF0\x9F\x87\xAC"] = ":flag_mg:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB2\xF0\x9F\x87\xAC"] = ":mg:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB2\xF0\x9F\x87\xBC"] = ":flag_mw:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB2\xF0\x9F\x87\xBC"] = ":mw:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB2\xF0\x9F\x87\xBE"] = ":flag_my:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB2\xF0\x9F\x87\xBE"] = ":my:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB2\xF0\x9F\x87\xBB"] = ":flag_mv:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB2\xF0\x9F\x87\xBB"] = ":mv:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB2\xF0\x9F\x87\xB1"] = ":flag_ml:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB2\xF0\x9F\x87\xB1"] = ":ml:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB2\xF0\x9F\x87\xB9"] = ":flag_mt:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB2\xF0\x9F\x87\xB9"] = ":mt:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB2\xF0\x9F\x87\xAD"] = ":flag_mh:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB2\xF0\x9F\x87\xAD"] = ":mh:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB2\xF0\x9F\x87\xB7"] = ":flag_mr:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB2\xF0\x9F\x87\xB7"] = ":mr:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB2\xF0\x9F\x87\xBA"] = ":flag_mu:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB2\xF0\x9F\x87\xBA"] = ":mu:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB2\xF0\x9F\x87\xBD"] = ":flag_mx:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB2\xF0\x9F\x87\xBD"] = ":mx:";
    lEmoticonsEmoi["\xF0\x9F\x87\xAB\xF0\x9F\x87\xB2"] = ":flag_fm:";
    lEmoticonsEmoi["\xF0\x9F\x87\xAB\xF0\x9F\x87\xB2"] = ":fm:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB2\xF0\x9F\x87\xA9"] = ":flag_md:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB2\xF0\x9F\x87\xA9"] = ":md:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB2\xF0\x9F\x87\xA8"] = ":flag_mc:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB2\xF0\x9F\x87\xA8"] = ":mc:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB2\xF0\x9F\x87\xB3"] = ":flag_mn:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB2\xF0\x9F\x87\xB3"] = ":mn:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB2\xF0\x9F\x87\xAA"] = ":flag_me:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB2\xF0\x9F\x87\xAA"] = ":me:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB2\xF0\x9F\x87\xA6"] = ":flag_ma:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB2\xF0\x9F\x87\xA6"] = ":ma:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB2\xF0\x9F\x87\xBF"] = ":flag_mz:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB2\xF0\x9F\x87\xBF"] = ":mz:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB2\xF0\x9F\x87\xB2"] = ":flag_mm:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB2\xF0\x9F\x87\xB2"] = ":mm:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB3\xF0\x9F\x87\xA6"] = ":flag_na:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB3\xF0\x9F\x87\xA6"] = ":na:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB3\xF0\x9F\x87\xB7"] = ":flag_nr:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB3\xF0\x9F\x87\xB7"] = ":nr:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB3\xF0\x9F\x87\xB5"] = ":flag_np:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB3\xF0\x9F\x87\xB5"] = ":np:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB3\xF0\x9F\x87\xB1"] = ":flag_nl:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB3\xF0\x9F\x87\xB1"] = ":nl:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB3\xF0\x9F\x87\xBF"] = ":flag_nz:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB3\xF0\x9F\x87\xBF"] = ":nz:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB3\xF0\x9F\x87\xAE"] = ":flag_ni:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB3\xF0\x9F\x87\xAE"] = ":ni:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB3\xF0\x9F\x87\xAA"] = ":flag_ne:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB3\xF0\x9F\x87\xAA"] = ":ne:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB3\xF0\x9F\x87\xAC"] = ":flag_ng:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB3\xF0\x9F\x87\xAC"] = ":nigeria:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB0\xF0\x9F\x87\xB5"] = ":flag_kp:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB0\xF0\x9F\x87\xB5"] = ":kp:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB3\xF0\x9F\x87\xB4"] = ":flag_no:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB3\xF0\x9F\x87\xB4"] = ":no:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB4\xF0\x9F\x87\xB2"] = ":flag_om:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB4\xF0\x9F\x87\xB2"] = ":om:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB5\xF0\x9F\x87\xB0"] = ":flag_pk:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB5\xF0\x9F\x87\xB0"] = ":pk:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB5\xF0\x9F\x87\xBC"] = ":flag_pw:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB5\xF0\x9F\x87\xBC"] = ":pw:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB5\xF0\x9F\x87\xA6"] = ":flag_pa:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB5\xF0\x9F\x87\xA6"] = ":pa:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB5\xF0\x9F\x87\xAC"] = ":flag_pg:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB5\xF0\x9F\x87\xAC"] = ":pg:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB5\xF0\x9F\x87\xBE"] = ":flag_py:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB5\xF0\x9F\x87\xBE"] = ":py:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB5\xF0\x9F\x87\xAA"] = ":flag_pe:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB5\xF0\x9F\x87\xAA"] = ":pe:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB5\xF0\x9F\x87\xAD"] = ":flag_ph:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB5\xF0\x9F\x87\xAD"] = ":ph:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB5\xF0\x9F\x87\xB1"] = ":flag_pl:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB5\xF0\x9F\x87\xB1"] = ":pl:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB5\xF0\x9F\x87\xB9"] = ":flag_pt:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB5\xF0\x9F\x87\xB9"] = ":pt:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB6\xF0\x9F\x87\xA6"] = ":flag_qa:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB6\xF0\x9F\x87\xA6"] = ":qa:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB9\xF0\x9F\x87\xBC"] = ":flag_tw:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB9\xF0\x9F\x87\xBC"] = ":tw:";
    lEmoticonsEmoi["\xF0\x9F\x87\xA8\xF0\x9F\x87\xAC"] = ":flag_cg:";
    lEmoticonsEmoi["\xF0\x9F\x87\xA8\xF0\x9F\x87\xAC"] = ":cg:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB7\xF0\x9F\x87\xB4"] = ":flag_ro:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB7\xF0\x9F\x87\xB4"] = ":ro:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB7\xF0\x9F\x87\xBC"] = ":flag_rw:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB7\xF0\x9F\x87\xBC"] = ":rw:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB0\xF0\x9F\x87\xB3"] = ":flag_kn:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB0\xF0\x9F\x87\xB3"] = ":kn:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB1\xF0\x9F\x87\xA8"] = ":flag_lc:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB1\xF0\x9F\x87\xA8"] = ":lc:";
    lEmoticonsEmoi["\xF0\x9F\x87\xBB\xF0\x9F\x87\xA8"] = ":flag_vc:";
    lEmoticonsEmoi["\xF0\x9F\x87\xBB\xF0\x9F\x87\xA8"] = ":vc:";
    lEmoticonsEmoi["\xF0\x9F\x87\xBC\xF0\x9F\x87\xB8"] = ":flag_ws:";
    lEmoticonsEmoi["\xF0\x9F\x87\xBC\xF0\x9F\x87\xB8"] = ":ws:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB8\xF0\x9F\x87\xB2"] = ":flag_sm:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB8\xF0\x9F\x87\xB2"] = ":sm:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB8\xF0\x9F\x87\xB9"] = ":flag_st:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB8\xF0\x9F\x87\xB9"] = ":st:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB8\xF0\x9F\x87\xA6"] = ":flag_sa:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB8\xF0\x9F\x87\xA6"] = ":saudiarabia:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB8\xF0\x9F\x87\xA6"] = ":saudi:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB8\xF0\x9F\x87\xB3"] = ":flag_sn:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB8\xF0\x9F\x87\xB3"] = ":sn:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB7\xF0\x9F\x87\xB8"] = ":flag_rs:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB7\xF0\x9F\x87\xB8"] = ":rs:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB8\xF0\x9F\x87\xA8"] = ":flag_sc:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB8\xF0\x9F\x87\xA8"] = ":sc:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB8\xF0\x9F\x87\xB1"] = ":flag_sl:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB8\xF0\x9F\x87\xB1"] = ":sl:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB8\xF0\x9F\x87\xAC"] = ":flag_sg:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB8\xF0\x9F\x87\xAC"] = ":sg:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB8\xF0\x9F\x87\xB0"] = ":flag_sk:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB8\xF0\x9F\x87\xB0"] = ":sk:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB8\xF0\x9F\x87\xAE"] = ":flag_si:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB8\xF0\x9F\x87\xAE"] = ":si:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB8\xF0\x9F\x87\xA7"] = ":flag_sb:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB8\xF0\x9F\x87\xA7"] = ":sb:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB8\xF0\x9F\x87\xB4"] = ":flag_so:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB8\xF0\x9F\x87\xB4"] = ":so:";
    lEmoticonsEmoi["\xF0\x9F\x87\xBF\xF0\x9F\x87\xA6"] = ":flag_za:";
    lEmoticonsEmoi["\xF0\x9F\x87\xBF\xF0\x9F\x87\xA6"] = ":za:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB1\xF0\x9F\x87\xB0"] = ":flag_lk:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB1\xF0\x9F\x87\xB0"] = ":lk:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB8\xF0\x9F\x87\xA9"] = ":flag_sd:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB8\xF0\x9F\x87\xA9"] = ":sd:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB8\xF0\x9F\x87\xB7"] = ":flag_sr:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB8\xF0\x9F\x87\xB7"] = ":sr:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB8\xF0\x9F\x87\xBF"] = ":flag_sz:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB8\xF0\x9F\x87\xBF"] = ":sz:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB8\xF0\x9F\x87\xAA"] = ":flag_se:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB8\xF0\x9F\x87\xAA"] = ":se:";
    lEmoticonsEmoi["\xF0\x9F\x87\xA8\xF0\x9F\x87\xAD"] = ":flag_ch:";
    lEmoticonsEmoi["\xF0\x9F\x87\xA8\xF0\x9F\x87\xAD"] = ":ch:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB8\xF0\x9F\x87\xBE"] = ":flag_sy:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB8\xF0\x9F\x87\xBE"] = ":sy:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB9\xF0\x9F\x87\xAF"] = ":flag_tj:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB9\xF0\x9F\x87\xAF"] = ":tj:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB9\xF0\x9F\x87\xBF"] = ":flag_tz:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB9\xF0\x9F\x87\xBF"] = ":tz:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB9\xF0\x9F\x87\xAD"] = ":flag_th:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB9\xF0\x9F\x87\xAD"] = ":th:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB9\xF0\x9F\x87\xAC"] = ":flag_tg:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB9\xF0\x9F\x87\xAC"] = ":tg:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB9\xF0\x9F\x87\xB4"] = ":flag_to:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB9\xF0\x9F\x87\xB4"] = ":to:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB9\xF0\x9F\x87\xB9"] = ":flag_tt:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB9\xF0\x9F\x87\xB9"] = ":tt:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB9\xF0\x9F\x87\xB3"] = ":flag_tn:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB9\xF0\x9F\x87\xB3"] = ":tn:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB9\xF0\x9F\x87\xB7"] = ":flag_tr:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB9\xF0\x9F\x87\xB7"] = ":tr:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB9\xF0\x9F\x87\xB2"] = ":flag_tm:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB9\xF0\x9F\x87\xB2"] = ":turkmenistan:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB9\xF0\x9F\x87\xBB"] = ":flag_tv:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB9\xF0\x9F\x87\xBB"] = ":tuvalu:";
    lEmoticonsEmoi["\xF0\x9F\x87\xBA\xF0\x9F\x87\xAC"] = ":flag_ug:";
    lEmoticonsEmoi["\xF0\x9F\x87\xBA\xF0\x9F\x87\xAC"] = ":ug:";
    lEmoticonsEmoi["\xF0\x9F\x87\xBA\xF0\x9F\x87\xA6"] = ":flag_ua:";
    lEmoticonsEmoi["\xF0\x9F\x87\xBA\xF0\x9F\x87\xA6"] = ":ua:";
    lEmoticonsEmoi["\xF0\x9F\x87\xA6\xF0\x9F\x87\xAA"] = ":flag_ae:";
    lEmoticonsEmoi["\xF0\x9F\x87\xA6\xF0\x9F\x87\xAA"] = ":ae:";
    lEmoticonsEmoi["\xF0\x9F\x87\xBA\xF0\x9F\x87\xBE"] = ":flag_uy:";
    lEmoticonsEmoi["\xF0\x9F\x87\xBA\xF0\x9F\x87\xBE"] = ":uy:";
    lEmoticonsEmoi["\xF0\x9F\x87\xBA\xF0\x9F\x87\xBF"] = ":flag_uz:";
    lEmoticonsEmoi["\xF0\x9F\x87\xBA\xF0\x9F\x87\xBF"] = ":uz:";
    lEmoticonsEmoi["\xF0\x9F\x87\xBB\xF0\x9F\x87\xBA"] = ":flag_vu:";
    lEmoticonsEmoi["\xF0\x9F\x87\xBB\xF0\x9F\x87\xBA"] = ":vu:";
    lEmoticonsEmoi["\xF0\x9F\x87\xBB\xF0\x9F\x87\xA6"] = ":flag_va:";
    lEmoticonsEmoi["\xF0\x9F\x87\xBB\xF0\x9F\x87\xA6"] = ":va:";
    lEmoticonsEmoi["\xF0\x9F\x87\xBB\xF0\x9F\x87\xAA"] = ":flag_ve:";
    lEmoticonsEmoi["\xF0\x9F\x87\xBB\xF0\x9F\x87\xAA"] = ":ve:";
    lEmoticonsEmoi["\xF0\x9F\x87\xBB\xF0\x9F\x87\xB3"] = ":flag_vn:";
    lEmoticonsEmoi["\xF0\x9F\x87\xBB\xF0\x9F\x87\xB3"] = ":vn:";
    lEmoticonsEmoi["\xF0\x9F\x87\xAA\xF0\x9F\x87\xAD"] = ":flag_eh:";
    lEmoticonsEmoi["\xF0\x9F\x87\xAA\xF0\x9F\x87\xAD"] = ":eh:";
    lEmoticonsEmoi["\xF0\x9F\x87\xBE\xF0\x9F\x87\xAA"] = ":flag_ye:";
    lEmoticonsEmoi["\xF0\x9F\x87\xBE\xF0\x9F\x87\xAA"] = ":ye:";
    lEmoticonsEmoi["\xF0\x9F\x87\xBF\xF0\x9F\x87\xB2"] = ":flag_zm:";
    lEmoticonsEmoi["\xF0\x9F\x87\xBF\xF0\x9F\x87\xB2"] = ":zm:";
    lEmoticonsEmoi["\xF0\x9F\x87\xBF\xF0\x9F\x87\xBC"] = ":flag_zw:";
    lEmoticonsEmoi["\xF0\x9F\x87\xBF\xF0\x9F\x87\xBC"] = ":zw:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB5\xF0\x9F\x87\xB7"] = ":flag_pr:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB5\xF0\x9F\x87\xB7"] = ":pr:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB0\xF0\x9F\x87\xBE"] = ":flag_ky:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB0\xF0\x9F\x87\xBE"] = ":ky:";
    lEmoticonsEmoi["\xF0\x9F\x87\xA7\xF0\x9F\x87\xB2"] = ":flag_bm:";
    lEmoticonsEmoi["\xF0\x9F\x87\xA7\xF0\x9F\x87\xB2"] = ":bm:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB5\xF0\x9F\x87\xAB"] = ":flag_pf:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB5\xF0\x9F\x87\xAB"] = ":pf:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB5\xF0\x9F\x87\xB8"] = ":flag_ps:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB5\xF0\x9F\x87\xB8"] = ":ps:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB3\xF0\x9F\x87\xA8"] = ":flag_nc:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB3\xF0\x9F\x87\xA8"] = ":nc:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB8\xF0\x9F\x87\xAD"] = ":flag_sh:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB8\xF0\x9F\x87\xAD"] = ":sh:";
    lEmoticonsEmoi["\xF0\x9F\x87\xA6\xF0\x9F\x87\xBC"] = ":flag_aw:";
    lEmoticonsEmoi["\xF0\x9F\x87\xA6\xF0\x9F\x87\xBC"] = ":aw:";
    lEmoticonsEmoi["\xF0\x9F\x87\xBB\xF0\x9F\x87\xAE"] = ":flag_vi:";
    lEmoticonsEmoi["\xF0\x9F\x87\xBB\xF0\x9F\x87\xAE"] = ":vi:";
    lEmoticonsEmoi["\xF0\x9F\x87\xAD\xF0\x9F\x87\xB0"] = ":flag_hk:";
    lEmoticonsEmoi["\xF0\x9F\x87\xAD\xF0\x9F\x87\xB0"] = ":hk:";
    lEmoticonsEmoi["\xF0\x9F\x87\xA6\xF0\x9F\x87\xA8"] = ":flag_ac:";
    lEmoticonsEmoi["\xF0\x9F\x87\xA6\xF0\x9F\x87\xA8"] = ":ac:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB2\xF0\x9F\x87\xB8"] = ":flag_ms:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB2\xF0\x9F\x87\xB8"] = ":ms:";
    lEmoticonsEmoi["\xF0\x9F\x87\xAC\xF0\x9F\x87\xBA"] = ":flag_gu:";
    lEmoticonsEmoi["\xF0\x9F\x87\xAC\xF0\x9F\x87\xBA"] = ":gu:";
    lEmoticonsEmoi["\xF0\x9F\x87\xAC\xF0\x9F\x87\xB1"] = ":flag_gl:";
    lEmoticonsEmoi["\xF0\x9F\x87\xAC\xF0\x9F\x87\xB1"] = ":gl:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB3\xF0\x9F\x87\xBA"] = ":flag_nu:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB3\xF0\x9F\x87\xBA"] = ":nu:";
    lEmoticonsEmoi["\xF0\x9F\x87\xBC\xF0\x9F\x87\xAB"] = ":flag_wf:";
    lEmoticonsEmoi["\xF0\x9F\x87\xBC\xF0\x9F\x87\xAB"] = ":wf:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB2\xF0\x9F\x87\xB4"] = ":flag_mo:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB2\xF0\x9F\x87\xB4"] = ":mo:";
    lEmoticonsEmoi["\xF0\x9F\x87\xAB\xF0\x9F\x87\xB4"] = ":flag_fo:";
    lEmoticonsEmoi["\xF0\x9F\x87\xAB\xF0\x9F\x87\xB4"] = ":fo:";
    lEmoticonsEmoi["\xF0\x9F\x87\xAB\xF0\x9F\x87\xB0"] = ":flag_fk:";
    lEmoticonsEmoi["\xF0\x9F\x87\xAB\xF0\x9F\x87\xB0"] = ":fk:";
    lEmoticonsEmoi["\xF0\x9F\x87\xAF\xF0\x9F\x87\xAA"] = ":flag_je:";
    lEmoticonsEmoi["\xF0\x9F\x87\xAF\xF0\x9F\x87\xAA"] = ":je:";
    lEmoticonsEmoi["\xF0\x9F\x87\xA6\xF0\x9F\x87\xAE"] = ":flag_ai:";
    lEmoticonsEmoi["\xF0\x9F\x87\xA6\xF0\x9F\x87\xAE"] = ":ai:";
    lEmoticonsEmoi["\xF0\x9F\x87\xAC\xF0\x9F\x87\xAE"] = ":flag_gi:";
    lEmoticonsEmoi["\xF0\x9F\x87\xAC\xF0\x9F\x87\xAE"] = ":gi:";
    lEmoticonsEmoi["\xF0\x9F\x8E\x9E"] = ":film_frames:";
    lEmoticonsEmoi["\xF0\x9F\x8E\x9F"] = ":tickets:";
    lEmoticonsEmoi["\xF0\x9F\x8E\x9F"] = ":admission_tickets:";
    lEmoticonsEmoi["\xF0\x9F\x8F\x85"] = ":medal:";
    lEmoticonsEmoi["\xF0\x9F\x8F\x85"] = ":sports_medal:";
    lEmoticonsEmoi["\xF0\x9F\x8F\x8B"] = ":lifter:";
    lEmoticonsEmoi["\xF0\x9F\x8F\x8B"] = ":weight_lifter:";
    lEmoticonsEmoi["\xF0\x9F\x8F\x8C"] = ":golfer:";
    lEmoticonsEmoi["\xF0\x9F\x8F\x8D"] = ":motorcycle:";
    lEmoticonsEmoi["\xF0\x9F\x8F\x8D"] = ":racing_motorcycle:";
    lEmoticonsEmoi["\xF0\x9F\x8F\x8E"] = ":race_car:";
    lEmoticonsEmoi["\xF0\x9F\x8F\x8E"] = ":racing_car:";
    lEmoticonsEmoi["\xF0\x9F\x8E\x96"] = ":military_medal:";
    lEmoticonsEmoi["\xF0\x9F\x8E\x97"] = ":reminder_ribbon:";
    lEmoticonsEmoi["\xF0\x9F\x8C\xB6"] = ":hot_pepper:";
    lEmoticonsEmoi["\xF0\x9F\x8C\xA7"] = ":cloud_rain:";
    lEmoticonsEmoi["\xF0\x9F\x8C\xA7"] = ":cloud_with_rain:";
    lEmoticonsEmoi["\xF0\x9F\x8C\xA8"] = ":cloud_snow:";
    lEmoticonsEmoi["\xF0\x9F\x8C\xA8"] = ":cloud_with_snow:";
    lEmoticonsEmoi["\xF0\x9F\x8C\xA9"] = ":cloud_lightning:";
    lEmoticonsEmoi["\xF0\x9F\x8C\xA9"] = ":cloud_with_lightning:";
    lEmoticonsEmoi["\xF0\x9F\x8C\xAA"] = ":cloud_tornado:";
    lEmoticonsEmoi["\xF0\x9F\x8C\xAA"] = ":cloud_with_tornado:";
    lEmoticonsEmoi["\xF0\x9F\x8C\xAB"] = ":fog:";
    lEmoticonsEmoi["\xF0\x9F\x8C\xAC"] = ":wind_blowing_face:";
    lEmoticonsEmoi["\xF0\x9F\x90\xBF"] = ":chipmunk:";
    lEmoticonsEmoi["\xF0\x9F\x95\xB7"] = ":spider:";
    lEmoticonsEmoi["\xF0\x9F\x95\xB8"] = ":spider_web:";
    lEmoticonsEmoi["\xF0\x9F\x8C\xA1"] = ":thermometer:";
    lEmoticonsEmoi["\xF0\x9F\x8E\x99"] = ":microphone2:";
    lEmoticonsEmoi["\xF0\x9F\x8E\x99"] = ":studio_microphone:";
    lEmoticonsEmoi["\xF0\x9F\x8E\x9A"] = ":level_slider:";
    lEmoticonsEmoi["\xF0\x9F\x8E\x9B"] = ":control_knobs:";
    lEmoticonsEmoi["\xF0\x9F\x8F\xB3"] = ":flag_white:";
    lEmoticonsEmoi["\xF0\x9F\x8F\xB3"] = ":waving_white_flag:";
    lEmoticonsEmoi["\xF0\x9F\x8F\xB4"] = ":flag_black:";
    lEmoticonsEmoi["\xF0\x9F\x8F\xB4"] = ":waving_black_flag:";
    lEmoticonsEmoi["\xF0\x9F\x8F\xB5"] = ":rosette:";
    lEmoticonsEmoi["\xF0\x9F\x8F\xB7"] = ":label:";
    lEmoticonsEmoi["\xF0\x9F\x93\xB8"] = ":camera_with_flash:";
    lEmoticonsEmoi["\xF0\x9F\x93\xBD"] = ":projector:";
    lEmoticonsEmoi["\xF0\x9F\x93\xBD"] = ":film_projector:";
    lEmoticonsEmoi["\xE2\x9C\x9D"] = ":cross:";
    lEmoticonsEmoi["\xE2\x9C\x9D"] = ":latin_cross:";
    lEmoticonsEmoi["\xF0\x9F\x95\x89"] = ":om_symbol:";
    lEmoticonsEmoi["\xF0\x9F\x95\x8A"] = ":dove:";
    lEmoticonsEmoi["\xF0\x9F\x95\x8A"] = ":dove_of_peace:";
    lEmoticonsEmoi["\xF0\x9F\x95\xAF"] = ":candle:";
    lEmoticonsEmoi["\xF0\x9F\x95\xB0"] = ":clock:";
    lEmoticonsEmoi["\xF0\x9F\x95\xB0"] = ":mantlepiece_clock:";
    lEmoticonsEmoi["\xF0\x9F\x95\xB3"] = ":hole:";
    lEmoticonsEmoi["\xF0\x9F\x95\xB6"] = ":dark_sunglasses:";
    lEmoticonsEmoi["\xF0\x9F\x95\xB9"] = ":joystick:";
    lEmoticonsEmoi["\xF0\x9F\x96\x87"] = ":paperclips:";
    lEmoticonsEmoi["\xF0\x9F\x96\x87"] = ":linked_paperclips:";
    lEmoticonsEmoi["\xF0\x9F\x96\x8A"] = ":pen_ballpoint:";
    lEmoticonsEmoi["\xF0\x9F\x96\x8A"] = ":lower_left_ballpoint_pen:";
    lEmoticonsEmoi["\xF0\x9F\x96\x8B"] = ":pen_fountain:";
    lEmoticonsEmoi["\xF0\x9F\x96\x8B"] = ":lower_left_fountain_pen:";
    lEmoticonsEmoi["\xF0\x9F\x96\x8C"] = ":paintbrush:";
    lEmoticonsEmoi["\xF0\x9F\x96\x8C"] = ":lower_left_paintbrush:";
    lEmoticonsEmoi["\xF0\x9F\x96\x8D"] = ":crayon:";
    lEmoticonsEmoi["\xF0\x9F\x96\x8D"] = ":lower_left_crayon:";
    lEmoticonsEmoi["\xF0\x9F\x96\xA5"] = ":desktop:";
    lEmoticonsEmoi["\xF0\x9F\x96\xA5"] = ":desktop_computer:";
    lEmoticonsEmoi["\xF0\x9F\x96\xA8"] = ":printer:";
    lEmoticonsEmoi["\xE2\x8C\xA8"] = ":keyboard:";
    lEmoticonsEmoi["\xF0\x9F\x96\xB2"] = ":trackball:";
    lEmoticonsEmoi["\xF0\x9F\x96\xBC"] = ":frame_photo:";
    lEmoticonsEmoi["\xF0\x9F\x96\xBC"] = ":frame_with_picture:";
    lEmoticonsEmoi["\xF0\x9F\x97\x82"] = ":dividers:";
    lEmoticonsEmoi["\xF0\x9F\x97\x82"] = ":card_index_dividers:";
    lEmoticonsEmoi["\xF0\x9F\x97\x83"] = ":card_box:";
    lEmoticonsEmoi["\xF0\x9F\x97\x83"] = ":card_file_box:";
    lEmoticonsEmoi["\xF0\x9F\x97\x84"] = ":file_cabinet:";
    lEmoticonsEmoi["\xF0\x9F\x97\x91"] = ":wastebasket:";
    lEmoticonsEmoi["\xF0\x9F\x97\x92"] = ":notepad_spiral:";
    lEmoticonsEmoi["\xF0\x9F\x97\x92"] = ":spiral_note_pad:";
    lEmoticonsEmoi["\xF0\x9F\x97\x93"] = ":calendar_spiral:";
    lEmoticonsEmoi["\xF0\x9F\x97\x93"] = ":spiral_calendar_pad:";
    lEmoticonsEmoi["\xF0\x9F\x97\x9C"] = ":compression:";
    lEmoticonsEmoi["\xF0\x9F\x97\x9D"] = ":key2:";
    lEmoticonsEmoi["\xF0\x9F\x97\x9D"] = ":old_key:";
    lEmoticonsEmoi["\xF0\x9F\x97\x9E"] = ":newspaper2:";
    lEmoticonsEmoi["\xF0\x9F\x97\x9E"] = ":rolled_up_newspaper:";
    lEmoticonsEmoi["\xF0\x9F\x97\xA1"] = ":dagger:";
    lEmoticonsEmoi["\xF0\x9F\x97\xA1"] = ":dagger_knife:";
    lEmoticonsEmoi["\xF0\x9F\x97\xA3"] = ":speaking_head:";
    lEmoticonsEmoi["\xF0\x9F\x97\xA3"] = ":speaking_head_in_silhouette:";
    lEmoticonsEmoi["\xF0\x9F\x97\xAF"] = ":anger_right:";
    lEmoticonsEmoi["\xF0\x9F\x97\xAF"] = ":right_anger_bubble:";
    lEmoticonsEmoi["\xF0\x9F\x97\xB3"] = ":ballot_box:";
    lEmoticonsEmoi["\xF0\x9F\x97\xB3"] = ":ballot_box_with_ballot:";
    lEmoticonsEmoi["\xF0\x9F\x97\xBA"] = ":map:";
    lEmoticonsEmoi["\xF0\x9F\x97\xBA"] = ":world_map:";
    lEmoticonsEmoi["\xF0\x9F\x9B\x8C"] = ":sleeping_accommodation:";
    lEmoticonsEmoi["\xF0\x9F\x9B\xA0"] = ":tools:";
    lEmoticonsEmoi["\xF0\x9F\x9B\xA0"] = ":hammer_and_wrench:";
    lEmoticonsEmoi["\xF0\x9F\x9B\xA1"] = ":shield:";
    lEmoticonsEmoi["\xF0\x9F\x9B\xA2"] = ":oil:";
    lEmoticonsEmoi["\xF0\x9F\x9B\xA2"] = ":oil_drum:";
    lEmoticonsEmoi["\xF0\x9F\x9B\xB0"] = ":satellite_orbital:";
    lEmoticonsEmoi["\xF0\x9F\x8D\xBD"] = ":fork_knife_plate:";
    lEmoticonsEmoi["\xF0\x9F\x8D\xBD"] = ":fork_and_knife_with_plate:";
    lEmoticonsEmoi["\xF0\x9F\x91\x81"] = ":eye:";
    lEmoticonsEmoi["\xF0\x9F\x95\xB4"] = ":levitate:";
    lEmoticonsEmoi["\xF0\x9F\x95\xB4"] = ":man_in_business_suit_levitating:";
    lEmoticonsEmoi["\xF0\x9F\x95\xB5"] = ":spy:";
    lEmoticonsEmoi["\xF0\x9F\x95\xB5"] = ":sleuth_or_spy:";
    lEmoticonsEmoi["\xE2\x9C\x8D"] = ":writing_hand:";
    lEmoticonsEmoi["\xF0\x9F\x96\x90"] = ":hand_splayed:";
    lEmoticonsEmoi["\xF0\x9F\x96\x90"] = ":raised_hand_with_fingers_splayed:";
    lEmoticonsEmoi["\xF0\x9F\x96\x95"] = ":middle_finger:";
    lEmoticonsEmoi["\xF0\x9F\x96\x95"] = ":reversed_hand_with_middle_finger_extended:";
    lEmoticonsEmoi["\xF0\x9F\x96\x96"] = ":vulcan:";
    lEmoticonsEmoi["\xF0\x9F\x96\x96"] = ":raised_hand_with_part_between_middle_and_ring_fingers:";
    lEmoticonsEmoi["\xF0\x9F\x99\x81"] = ":slight_frown:";
    lEmoticonsEmoi["\xF0\x9F\x99\x81"] = ":slightly_frowning_face:";
    lEmoticonsEmoi["\xF0\x9F\x99\x82"] = ":slight_smile:";
    lEmoticonsEmoi["\xF0\x9F\x99\x82"] = ":slightly_smiling_face:";
    lEmoticonsEmoi["\xF0\x9F\x8F\x94"] = ":mountain_snow:";
    lEmoticonsEmoi["\xF0\x9F\x8F\x94"] = ":snow_capped_mountain:";
    lEmoticonsEmoi["\xF0\x9F\x8F\x95"] = ":camping:";
    lEmoticonsEmoi["\xF0\x9F\x8F\x96"] = ":beach:";
    lEmoticonsEmoi["\xF0\x9F\x8F\x96"] = ":beach_with_umbrella:";
    lEmoticonsEmoi["\xF0\x9F\x8F\x97"] = ":construction_site:";
    lEmoticonsEmoi["\xF0\x9F\x8F\x97"] = ":building_construction:";
    lEmoticonsEmoi["\xF0\x9F\x8F\x98"] = ":homes:";
    lEmoticonsEmoi["\xF0\x9F\x8F\x98"] = ":house_buildings:";
    lEmoticonsEmoi["\xF0\x9F\x8F\x99"] = ":cityscape:";
    lEmoticonsEmoi["\xF0\x9F\x8F\x9A"] = ":house_abandoned:";
    lEmoticonsEmoi["\xF0\x9F\x8F\x9A"] = ":derelict_house_building:";
    lEmoticonsEmoi["\xF0\x9F\x8F\x9B"] = ":classical_building:";
    lEmoticonsEmoi["\xF0\x9F\x8F\x9C"] = ":desert:";
    lEmoticonsEmoi["\xF0\x9F\x8F\x9D"] = ":island:";
    lEmoticonsEmoi["\xF0\x9F\x8F\x9D"] = ":desert_island:";
    lEmoticonsEmoi["\xF0\x9F\x8F\x9E"] = ":park:";
    lEmoticonsEmoi["\xF0\x9F\x8F\x9E"] = ":national_park:";
    lEmoticonsEmoi["\xF0\x9F\x8F\x9F"] = ":stadium:";
    lEmoticonsEmoi["\xF0\x9F\x9B\x8B"] = ":couch:";
    lEmoticonsEmoi["\xF0\x9F\x9B\x8B"] = ":couch_and_lamp:";
    lEmoticonsEmoi["\xF0\x9F\x9B\x8D"] = ":shopping_bags:";
    lEmoticonsEmoi["\xF0\x9F\x9B\x8E"] = ":bellhop:";
    lEmoticonsEmoi["\xF0\x9F\x9B\x8E"] = ":bellhop_bell:";
    lEmoticonsEmoi["\xF0\x9F\x9B\x8F"] = ":bed:";
    lEmoticonsEmoi["\xF0\x9F\x9B\xA3"] = ":motorway:";
    lEmoticonsEmoi["\xF0\x9F\x9B\xA4"] = ":railway_track:";
    lEmoticonsEmoi["\xF0\x9F\x9B\xA4"] = ":railroad_track:";
    lEmoticonsEmoi["\xF0\x9F\x9B\xA5"] = ":motorboat:";
    lEmoticonsEmoi["\xF0\x9F\x9B\xA9"] = ":airplane_small:";
    lEmoticonsEmoi["\xF0\x9F\x9B\xA9"] = ":small_airplane:";
    lEmoticonsEmoi["\xF0\x9F\x9B\xAB"] = ":airplane_departure:";
    lEmoticonsEmoi["\xF0\x9F\x9B\xAC"] = ":airplane_arriving:";
    lEmoticonsEmoi["\xF0\x9F\x9B\xB3"] = ":cruise_ship:";
    lEmoticonsEmoi["\xF0\x9F\x9B\xB3"] = ":passenger_ship:";
    lEmoticonsEmoi["\xF0\x9F\x91\xB6\xF0\x9F\x8F\xBB"] = ":baby_tone1:";
    lEmoticonsEmoi["\xF0\x9F\x91\xB6\xF0\x9F\x8F\xBC"] = ":baby_tone2:";
    lEmoticonsEmoi["\xF0\x9F\x91\xB6\xF0\x9F\x8F\xBD"] = ":baby_tone3:";
    lEmoticonsEmoi["\xF0\x9F\x91\xB6\xF0\x9F\x8F\xBE"] = ":baby_tone4:";
    lEmoticonsEmoi["\xF0\x9F\x91\xB6\xF0\x9F\x8F\xBF"] = ":baby_tone5:";
    lEmoticonsEmoi["\xF0\x9F\x91\xA6\xF0\x9F\x8F\xBB"] = ":boy_tone1:";
    lEmoticonsEmoi["\xF0\x9F\x91\xA6\xF0\x9F\x8F\xBC"] = ":boy_tone2:";
    lEmoticonsEmoi["\xF0\x9F\x91\xA6\xF0\x9F\x8F\xBD"] = ":boy_tone3:";
    lEmoticonsEmoi["\xF0\x9F\x91\xA6\xF0\x9F\x8F\xBE"] = ":boy_tone4:";
    lEmoticonsEmoi["\xF0\x9F\x91\xA6\xF0\x9F\x8F\xBF"] = ":boy_tone5:";
    lEmoticonsEmoi["\xF0\x9F\x91\xA7\xF0\x9F\x8F\xBB"] = ":girl_tone1:";
    lEmoticonsEmoi["\xF0\x9F\x91\xA7\xF0\x9F\x8F\xBC"] = ":girl_tone2:";
    lEmoticonsEmoi["\xF0\x9F\x91\xA7\xF0\x9F\x8F\xBD"] = ":girl_tone3:";
    lEmoticonsEmoi["\xF0\x9F\x91\xA7\xF0\x9F\x8F\xBE"] = ":girl_tone4:";
    lEmoticonsEmoi["\xF0\x9F\x91\xA7\xF0\x9F\x8F\xBF"] = ":girl_tone5:";
    lEmoticonsEmoi["\xF0\x9F\x91\xA8\xF0\x9F\x8F\xBB"] = ":man_tone1:";
    lEmoticonsEmoi["\xF0\x9F\x91\xA8\xF0\x9F\x8F\xBC"] = ":man_tone2:";
    lEmoticonsEmoi["\xF0\x9F\x91\xA8\xF0\x9F\x8F\xBD"] = ":man_tone3:";
    lEmoticonsEmoi["\xF0\x9F\x91\xA8\xF0\x9F\x8F\xBE"] = ":man_tone4:";
    lEmoticonsEmoi["\xF0\x9F\x91\xA8\xF0\x9F\x8F\xBF"] = ":man_tone5:";
    lEmoticonsEmoi["\xF0\x9F\x91\xA9\xF0\x9F\x8F\xBB"] = ":woman_tone1:";
    lEmoticonsEmoi["\xF0\x9F\x91\xA9\xF0\x9F\x8F\xBC"] = ":woman_tone2:";
    lEmoticonsEmoi["\xF0\x9F\x91\xA9\xF0\x9F\x8F\xBD"] = ":woman_tone3:";
    lEmoticonsEmoi["\xF0\x9F\x91\xA9\xF0\x9F\x8F\xBE"] = ":woman_tone4:";
    lEmoticonsEmoi["\xF0\x9F\x91\xA9\xF0\x9F\x8F\xBF"] = ":woman_tone5:";
    lEmoticonsEmoi["\xF0\x9F\x91\xB0\xF0\x9F\x8F\xBB"] = ":bride_with_veil_tone1:";
    lEmoticonsEmoi["\xF0\x9F\x91\xB0\xF0\x9F\x8F\xBC"] = ":bride_with_veil_tone2:";
    lEmoticonsEmoi["\xF0\x9F\x91\xB0\xF0\x9F\x8F\xBD"] = ":bride_with_veil_tone3:";
    lEmoticonsEmoi["\xF0\x9F\x91\xB0\xF0\x9F\x8F\xBE"] = ":bride_with_veil_tone4:";
    lEmoticonsEmoi["\xF0\x9F\x91\xB0\xF0\x9F\x8F\xBF"] = ":bride_with_veil_tone5:";
    lEmoticonsEmoi["\xF0\x9F\x91\xB1\xF0\x9F\x8F\xBB"] = ":person_with_blond_hair_tone1:";
    lEmoticonsEmoi["\xF0\x9F\x91\xB1\xF0\x9F\x8F\xBC"] = ":person_with_blond_hair_tone2:";
    lEmoticonsEmoi["\xF0\x9F\x91\xB1\xF0\x9F\x8F\xBD"] = ":person_with_blond_hair_tone3:";
    lEmoticonsEmoi["\xF0\x9F\x91\xB1\xF0\x9F\x8F\xBE"] = ":person_with_blond_hair_tone4:";
    lEmoticonsEmoi["\xF0\x9F\x91\xB1\xF0\x9F\x8F\xBF"] = ":person_with_blond_hair_tone5:";
    lEmoticonsEmoi["\xF0\x9F\x91\xB2\xF0\x9F\x8F\xBB"] = ":man_with_gua_pi_mao_tone1:";
    lEmoticonsEmoi["\xF0\x9F\x91\xB2\xF0\x9F\x8F\xBC"] = ":man_with_gua_pi_mao_tone2:";
    lEmoticonsEmoi["\xF0\x9F\x91\xB2\xF0\x9F\x8F\xBD"] = ":man_with_gua_pi_mao_tone3:";
    lEmoticonsEmoi["\xF0\x9F\x91\xB2\xF0\x9F\x8F\xBE"] = ":man_with_gua_pi_mao_tone4:";
    lEmoticonsEmoi["\xF0\x9F\x91\xB2\xF0\x9F\x8F\xBF"] = ":man_with_gua_pi_mao_tone5:";
    lEmoticonsEmoi["\xF0\x9F\x91\xB3\xF0\x9F\x8F\xBB"] = ":man_with_turban_tone1:";
    lEmoticonsEmoi["\xF0\x9F\x91\xB3\xF0\x9F\x8F\xBC"] = ":man_with_turban_tone2:";
    lEmoticonsEmoi["\xF0\x9F\x91\xB3\xF0\x9F\x8F\xBD"] = ":man_with_turban_tone3:";
    lEmoticonsEmoi["\xF0\x9F\x91\xB3\xF0\x9F\x8F\xBE"] = ":man_with_turban_tone4:";
    lEmoticonsEmoi["\xF0\x9F\x91\xB3\xF0\x9F\x8F\xBF"] = ":man_with_turban_tone5:";
    lEmoticonsEmoi["\xF0\x9F\x91\xB4\xF0\x9F\x8F\xBB"] = ":older_man_tone1:";
    lEmoticonsEmoi["\xF0\x9F\x91\xB4\xF0\x9F\x8F\xBC"] = ":older_man_tone2:";
    lEmoticonsEmoi["\xF0\x9F\x91\xB4\xF0\x9F\x8F\xBD"] = ":older_man_tone3:";
    lEmoticonsEmoi["\xF0\x9F\x91\xB4\xF0\x9F\x8F\xBE"] = ":older_man_tone4:";
    lEmoticonsEmoi["\xF0\x9F\x91\xB4\xF0\x9F\x8F\xBF"] = ":older_man_tone5:";
    lEmoticonsEmoi["\xF0\x9F\x91\xB5\xF0\x9F\x8F\xBB"] = ":older_woman_tone1:";
    lEmoticonsEmoi["\xF0\x9F\x91\xB5\xF0\x9F\x8F\xBB"] = ":grandma_tone1:";
    lEmoticonsEmoi["\xF0\x9F\x91\xB5\xF0\x9F\x8F\xBC"] = ":older_woman_tone2:";
    lEmoticonsEmoi["\xF0\x9F\x91\xB5\xF0\x9F\x8F\xBC"] = ":grandma_tone2:";
    lEmoticonsEmoi["\xF0\x9F\x91\xB5\xF0\x9F\x8F\xBD"] = ":older_woman_tone3:";
    lEmoticonsEmoi["\xF0\x9F\x91\xB5\xF0\x9F\x8F\xBD"] = ":grandma_tone3:";
    lEmoticonsEmoi["\xF0\x9F\x91\xB5\xF0\x9F\x8F\xBE"] = ":older_woman_tone4:";
    lEmoticonsEmoi["\xF0\x9F\x91\xB5\xF0\x9F\x8F\xBE"] = ":grandma_tone4:";
    lEmoticonsEmoi["\xF0\x9F\x91\xB5\xF0\x9F\x8F\xBF"] = ":older_woman_tone5:";
    lEmoticonsEmoi["\xF0\x9F\x91\xB5\xF0\x9F\x8F\xBF"] = ":grandma_tone5:";
    lEmoticonsEmoi["\xF0\x9F\x91\xAE\xF0\x9F\x8F\xBB"] = ":cop_tone1:";
    lEmoticonsEmoi["\xF0\x9F\x91\xAE\xF0\x9F\x8F\xBC"] = ":cop_tone2:";
    lEmoticonsEmoi["\xF0\x9F\x91\xAE\xF0\x9F\x8F\xBD"] = ":cop_tone3:";
    lEmoticonsEmoi["\xF0\x9F\x91\xAE\xF0\x9F\x8F\xBE"] = ":cop_tone4:";
    lEmoticonsEmoi["\xF0\x9F\x91\xAE\xF0\x9F\x8F\xBF"] = ":cop_tone5:";
    lEmoticonsEmoi["\xF0\x9F\x91\xB7\xF0\x9F\x8F\xBB"] = ":construction_worker_tone1:";
    lEmoticonsEmoi["\xF0\x9F\x91\xB7\xF0\x9F\x8F\xBC"] = ":construction_worker_tone2:";
    lEmoticonsEmoi["\xF0\x9F\x91\xB7\xF0\x9F\x8F\xBD"] = ":construction_worker_tone3:";
    lEmoticonsEmoi["\xF0\x9F\x91\xB7\xF0\x9F\x8F\xBE"] = ":construction_worker_tone4:";
    lEmoticonsEmoi["\xF0\x9F\x91\xB7\xF0\x9F\x8F\xBF"] = ":construction_worker_tone5:";
    lEmoticonsEmoi["\xF0\x9F\x91\xB8\xF0\x9F\x8F\xBB"] = ":princess_tone1:";
    lEmoticonsEmoi["\xF0\x9F\x91\xB8\xF0\x9F\x8F\xBC"] = ":princess_tone2:";
    lEmoticonsEmoi["\xF0\x9F\x91\xB8\xF0\x9F\x8F\xBD"] = ":princess_tone3:";
    lEmoticonsEmoi["\xF0\x9F\x91\xB8\xF0\x9F\x8F\xBE"] = ":princess_tone4:";
    lEmoticonsEmoi["\xF0\x9F\x91\xB8\xF0\x9F\x8F\xBF"] = ":princess_tone5:";
    lEmoticonsEmoi["\xF0\x9F\x92\x82\xF0\x9F\x8F\xBB"] = ":guardsman_tone1:";
    lEmoticonsEmoi["\xF0\x9F\x92\x82\xF0\x9F\x8F\xBC"] = ":guardsman_tone2:";
    lEmoticonsEmoi["\xF0\x9F\x92\x82\xF0\x9F\x8F\xBD"] = ":guardsman_tone3:";
    lEmoticonsEmoi["\xF0\x9F\x92\x82\xF0\x9F\x8F\xBE"] = ":guardsman_tone4:";
    lEmoticonsEmoi["\xF0\x9F\x92\x82\xF0\x9F\x8F\xBF"] = ":guardsman_tone5:";
    lEmoticonsEmoi["\xF0\x9F\x91\xBC\xF0\x9F\x8F\xBB"] = ":angel_tone1:";
    lEmoticonsEmoi["\xF0\x9F\x91\xBC\xF0\x9F\x8F\xBC"] = ":angel_tone2:";
    lEmoticonsEmoi["\xF0\x9F\x91\xBC\xF0\x9F\x8F\xBD"] = ":angel_tone3:";
    lEmoticonsEmoi["\xF0\x9F\x91\xBC\xF0\x9F\x8F\xBE"] = ":angel_tone4:";
    lEmoticonsEmoi["\xF0\x9F\x91\xBC\xF0\x9F\x8F\xBF"] = ":angel_tone5:";
    lEmoticonsEmoi["\xF0\x9F\x99\x87\xF0\x9F\x8F\xBB"] = ":bow_tone1:";
    lEmoticonsEmoi["\xF0\x9F\x99\x87\xF0\x9F\x8F\xBC"] = ":bow_tone2:";
    lEmoticonsEmoi["\xF0\x9F\x99\x87\xF0\x9F\x8F\xBD"] = ":bow_tone3:";
    lEmoticonsEmoi["\xF0\x9F\x99\x87\xF0\x9F\x8F\xBE"] = ":bow_tone4:";
    lEmoticonsEmoi["\xF0\x9F\x99\x87\xF0\x9F\x8F\xBF"] = ":bow_tone5:";
    lEmoticonsEmoi["\xF0\x9F\x92\x81\xF0\x9F\x8F\xBB"] = ":information_desk_person_tone1:";
    lEmoticonsEmoi["\xF0\x9F\x92\x81\xF0\x9F\x8F\xBC"] = ":information_desk_person_tone2:";
    lEmoticonsEmoi["\xF0\x9F\x92\x81\xF0\x9F\x8F\xBD"] = ":information_desk_person_tone3:";
    lEmoticonsEmoi["\xF0\x9F\x92\x81\xF0\x9F\x8F\xBE"] = ":information_desk_person_tone4:";
    lEmoticonsEmoi["\xF0\x9F\x92\x81\xF0\x9F\x8F\xBF"] = ":information_desk_person_tone5:";
    lEmoticonsEmoi["\xF0\x9F\x99\x85\xF0\x9F\x8F\xBB"] = ":no_good_tone1:";
    lEmoticonsEmoi["\xF0\x9F\x99\x85\xF0\x9F\x8F\xBC"] = ":no_good_tone2:";
    lEmoticonsEmoi["\xF0\x9F\x99\x85\xF0\x9F\x8F\xBD"] = ":no_good_tone3:";
    lEmoticonsEmoi["\xF0\x9F\x99\x85\xF0\x9F\x8F\xBE"] = ":no_good_tone4:";
    lEmoticonsEmoi["\xF0\x9F\x99\x85\xF0\x9F\x8F\xBF"] = ":no_good_tone5:";
    lEmoticonsEmoi["\xF0\x9F\x99\x86\xF0\x9F\x8F\xBB"] = ":ok_woman_tone1:";
    lEmoticonsEmoi["\xF0\x9F\x99\x86\xF0\x9F\x8F\xBC"] = ":ok_woman_tone2:";
    lEmoticonsEmoi["\xF0\x9F\x99\x86\xF0\x9F\x8F\xBD"] = ":ok_woman_tone3:";
    lEmoticonsEmoi["\xF0\x9F\x99\x86\xF0\x9F\x8F\xBE"] = ":ok_woman_tone4:";
    lEmoticonsEmoi["\xF0\x9F\x99\x86\xF0\x9F\x8F\xBF"] = ":ok_woman_tone5:";
    lEmoticonsEmoi["\xF0\x9F\x99\x8B\xF0\x9F\x8F\xBB"] = ":raising_hand_tone1:";
    lEmoticonsEmoi["\xF0\x9F\x99\x8B\xF0\x9F\x8F\xBC"] = ":raising_hand_tone2:";
    lEmoticonsEmoi["\xF0\x9F\x99\x8B\xF0\x9F\x8F\xBD"] = ":raising_hand_tone3:";
    lEmoticonsEmoi["\xF0\x9F\x99\x8B\xF0\x9F\x8F\xBE"] = ":raising_hand_tone4:";
    lEmoticonsEmoi["\xF0\x9F\x99\x8B\xF0\x9F\x8F\xBF"] = ":raising_hand_tone5:";
    lEmoticonsEmoi["\xF0\x9F\x99\x8E\xF0\x9F\x8F\xBB"] = ":person_with_pouting_face_tone1:";
    lEmoticonsEmoi["\xF0\x9F\x99\x8E\xF0\x9F\x8F\xBC"] = ":person_with_pouting_face_tone2:";
    lEmoticonsEmoi["\xF0\x9F\x99\x8E\xF0\x9F\x8F\xBD"] = ":person_with_pouting_face_tone3:";
    lEmoticonsEmoi["\xF0\x9F\x99\x8E\xF0\x9F\x8F\xBE"] = ":person_with_pouting_face_tone4:";
    lEmoticonsEmoi["\xF0\x9F\x99\x8E\xF0\x9F\x8F\xBF"] = ":person_with_pouting_face_tone5:";
    lEmoticonsEmoi["\xF0\x9F\x99\x8D\xF0\x9F\x8F\xBB"] = ":person_frowning_tone1:";
    lEmoticonsEmoi["\xF0\x9F\x99\x8D\xF0\x9F\x8F\xBC"] = ":person_frowning_tone2:";
    lEmoticonsEmoi["\xF0\x9F\x99\x8D\xF0\x9F\x8F\xBD"] = ":person_frowning_tone3:";
    lEmoticonsEmoi["\xF0\x9F\x99\x8D\xF0\x9F\x8F\xBE"] = ":person_frowning_tone4:";
    lEmoticonsEmoi["\xF0\x9F\x99\x8D\xF0\x9F\x8F\xBF"] = ":person_frowning_tone5:";
    lEmoticonsEmoi["\xF0\x9F\x92\x86\xF0\x9F\x8F\xBB"] = ":massage_tone1:";
    lEmoticonsEmoi["\xF0\x9F\x92\x86\xF0\x9F\x8F\xBC"] = ":massage_tone2:";
    lEmoticonsEmoi["\xF0\x9F\x92\x86\xF0\x9F\x8F\xBD"] = ":massage_tone3:";
    lEmoticonsEmoi["\xF0\x9F\x92\x86\xF0\x9F\x8F\xBE"] = ":massage_tone4:";
    lEmoticonsEmoi["\xF0\x9F\x92\x86\xF0\x9F\x8F\xBF"] = ":massage_tone5:";
    lEmoticonsEmoi["\xF0\x9F\x92\x87\xF0\x9F\x8F\xBB"] = ":haircut_tone1:";
    lEmoticonsEmoi["\xF0\x9F\x92\x87\xF0\x9F\x8F\xBC"] = ":haircut_tone2:";
    lEmoticonsEmoi["\xF0\x9F\x92\x87\xF0\x9F\x8F\xBD"] = ":haircut_tone3:";
    lEmoticonsEmoi["\xF0\x9F\x92\x87\xF0\x9F\x8F\xBE"] = ":haircut_tone4:";
    lEmoticonsEmoi["\xF0\x9F\x92\x87\xF0\x9F\x8F\xBF"] = ":haircut_tone5:";
    lEmoticonsEmoi["\xF0\x9F\x99\x8C\xF0\x9F\x8F\xBB"] = ":raised_hands_tone1:";
    lEmoticonsEmoi["\xF0\x9F\x99\x8C\xF0\x9F\x8F\xBC"] = ":raised_hands_tone2:";
    lEmoticonsEmoi["\xF0\x9F\x99\x8C\xF0\x9F\x8F\xBD"] = ":raised_hands_tone3:";
    lEmoticonsEmoi["\xF0\x9F\x99\x8C\xF0\x9F\x8F\xBE"] = ":raised_hands_tone4:";
    lEmoticonsEmoi["\xF0\x9F\x99\x8C\xF0\x9F\x8F\xBF"] = ":raised_hands_tone5:";
    lEmoticonsEmoi["\xF0\x9F\x91\x8F\xF0\x9F\x8F\xBB"] = ":clap_tone1:";
    lEmoticonsEmoi["\xF0\x9F\x91\x8F\xF0\x9F\x8F\xBC"] = ":clap_tone2:";
    lEmoticonsEmoi["\xF0\x9F\x91\x8F\xF0\x9F\x8F\xBD"] = ":clap_tone3:";
    lEmoticonsEmoi["\xF0\x9F\x91\x8F\xF0\x9F\x8F\xBE"] = ":clap_tone4:";
    lEmoticonsEmoi["\xF0\x9F\x91\x8F\xF0\x9F\x8F\xBF"] = ":clap_tone5:";
    lEmoticonsEmoi["\xF0\x9F\x91\x82\xF0\x9F\x8F\xBB"] = ":ear_tone1:";
    lEmoticonsEmoi["\xF0\x9F\x91\x82\xF0\x9F\x8F\xBC"] = ":ear_tone2:";
    lEmoticonsEmoi["\xF0\x9F\x91\x82\xF0\x9F\x8F\xBD"] = ":ear_tone3:";
    lEmoticonsEmoi["\xF0\x9F\x91\x82\xF0\x9F\x8F\xBE"] = ":ear_tone4:";
    lEmoticonsEmoi["\xF0\x9F\x91\x82\xF0\x9F\x8F\xBF"] = ":ear_tone5:";
    lEmoticonsEmoi["\xF0\x9F\x91\x83\xF0\x9F\x8F\xBB"] = ":nose_tone1:";
    lEmoticonsEmoi["\xF0\x9F\x91\x83\xF0\x9F\x8F\xBC"] = ":nose_tone2:";
    lEmoticonsEmoi["\xF0\x9F\x91\x83\xF0\x9F\x8F\xBD"] = ":nose_tone3:";
    lEmoticonsEmoi["\xF0\x9F\x91\x83\xF0\x9F\x8F\xBE"] = ":nose_tone4:";
    lEmoticonsEmoi["\xF0\x9F\x91\x83\xF0\x9F\x8F\xBF"] = ":nose_tone5:";
    lEmoticonsEmoi["\xF0\x9F\x92\x85\xF0\x9F\x8F\xBB"] = ":nail_care_tone1:";
    lEmoticonsEmoi["\xF0\x9F\x92\x85\xF0\x9F\x8F\xBC"] = ":nail_care_tone2:";
    lEmoticonsEmoi["\xF0\x9F\x92\x85\xF0\x9F\x8F\xBD"] = ":nail_care_tone3:";
    lEmoticonsEmoi["\xF0\x9F\x92\x85\xF0\x9F\x8F\xBE"] = ":nail_care_tone4:";
    lEmoticonsEmoi["\xF0\x9F\x92\x85\xF0\x9F\x8F\xBF"] = ":nail_care_tone5:";
    lEmoticonsEmoi["\xF0\x9F\x91\x8B\xF0\x9F\x8F\xBB"] = ":wave_tone1:";
    lEmoticonsEmoi["\xF0\x9F\x91\x8B\xF0\x9F\x8F\xBC"] = ":wave_tone2:";
    lEmoticonsEmoi["\xF0\x9F\x91\x8B\xF0\x9F\x8F\xBD"] = ":wave_tone3:";
    lEmoticonsEmoi["\xF0\x9F\x91\x8B\xF0\x9F\x8F\xBE"] = ":wave_tone4:";
    lEmoticonsEmoi["\xF0\x9F\x91\x8B\xF0\x9F\x8F\xBF"] = ":wave_tone5:";
    lEmoticonsEmoi["\xF0\x9F\x91\x8D\xF0\x9F\x8F\xBB"] = ":thumbsup_tone1:";
    lEmoticonsEmoi["\xF0\x9F\x91\x8D\xF0\x9F\x8F\xBB"] = ":+1_tone1:";
    lEmoticonsEmoi["\xF0\x9F\x91\x8D\xF0\x9F\x8F\xBC"] = ":thumbsup_tone2:";
    lEmoticonsEmoi["\xF0\x9F\x91\x8D\xF0\x9F\x8F\xBC"] = ":+1_tone2:";
    lEmoticonsEmoi["\xF0\x9F\x91\x8D\xF0\x9F\x8F\xBD"] = ":thumbsup_tone3:";
    lEmoticonsEmoi["\xF0\x9F\x91\x8D\xF0\x9F\x8F\xBD"] = ":+1_tone3:";
    lEmoticonsEmoi["\xF0\x9F\x91\x8D\xF0\x9F\x8F\xBE"] = ":thumbsup_tone4:";
    lEmoticonsEmoi["\xF0\x9F\x91\x8D\xF0\x9F\x8F\xBE"] = ":+1_tone4:";
    lEmoticonsEmoi["\xF0\x9F\x91\x8D\xF0\x9F\x8F\xBF"] = ":thumbsup_tone5:";
    lEmoticonsEmoi["\xF0\x9F\x91\x8D\xF0\x9F\x8F\xBF"] = ":+1_tone5:";
    lEmoticonsEmoi["\xF0\x9F\x91\x8E\xF0\x9F\x8F\xBB"] = ":thumbsdown_tone1:";
    lEmoticonsEmoi["\xF0\x9F\x91\x8E\xF0\x9F\x8F\xBB"] = ":-1_tone1:";
    lEmoticonsEmoi["\xF0\x9F\x91\x8E\xF0\x9F\x8F\xBC"] = ":thumbsdown_tone2:";
    lEmoticonsEmoi["\xF0\x9F\x91\x8E\xF0\x9F\x8F\xBC"] = ":-1_tone2:";
    lEmoticonsEmoi["\xF0\x9F\x91\x8E\xF0\x9F\x8F\xBD"] = ":thumbsdown_tone3:";
    lEmoticonsEmoi["\xF0\x9F\x91\x8E\xF0\x9F\x8F\xBD"] = ":-1_tone3:";
    lEmoticonsEmoi["\xF0\x9F\x91\x8E\xF0\x9F\x8F\xBE"] = ":thumbsdown_tone4:";
    lEmoticonsEmoi["\xF0\x9F\x91\x8E\xF0\x9F\x8F\xBE"] = ":-1_tone4:";
    lEmoticonsEmoi["\xF0\x9F\x91\x8E\xF0\x9F\x8F\xBF"] = ":thumbsdown_tone5:";
    lEmoticonsEmoi["\xF0\x9F\x91\x8E\xF0\x9F\x8F\xBF"] = ":-1_tone5:";
    lEmoticonsEmoi["\xE2\x98\x9D\xF0\x9F\x8F\xBB"] = ":point_up_tone1:";
    lEmoticonsEmoi["\xE2\x98\x9D\xF0\x9F\x8F\xBC"] = ":point_up_tone2:";
    lEmoticonsEmoi["\xE2\x98\x9D\xF0\x9F\x8F\xBD"] = ":point_up_tone3:";
    lEmoticonsEmoi["\xE2\x98\x9D\xF0\x9F\x8F\xBE"] = ":point_up_tone4:";
    lEmoticonsEmoi["\xE2\x98\x9D\xF0\x9F\x8F\xBF"] = ":point_up_tone5:";
    lEmoticonsEmoi["\xF0\x9F\x91\x86\xF0\x9F\x8F\xBB"] = ":point_up_2_tone1:";
    lEmoticonsEmoi["\xF0\x9F\x91\x86\xF0\x9F\x8F\xBC"] = ":point_up_2_tone2:";
    lEmoticonsEmoi["\xF0\x9F\x91\x86\xF0\x9F\x8F\xBD"] = ":point_up_2_tone3:";
    lEmoticonsEmoi["\xF0\x9F\x91\x86\xF0\x9F\x8F\xBE"] = ":point_up_2_tone4:";
    lEmoticonsEmoi["\xF0\x9F\x91\x86\xF0\x9F\x8F\xBF"] = ":point_up_2_tone5:";
    lEmoticonsEmoi["\xF0\x9F\x91\x87\xF0\x9F\x8F\xBB"] = ":point_down_tone1:";
    lEmoticonsEmoi["\xF0\x9F\x91\x87\xF0\x9F\x8F\xBC"] = ":point_down_tone2:";
    lEmoticonsEmoi["\xF0\x9F\x91\x87\xF0\x9F\x8F\xBD"] = ":point_down_tone3:";
    lEmoticonsEmoi["\xF0\x9F\x91\x87\xF0\x9F\x8F\xBE"] = ":point_down_tone4:";
    lEmoticonsEmoi["\xF0\x9F\x91\x87\xF0\x9F\x8F\xBF"] = ":point_down_tone5:";
    lEmoticonsEmoi["\xF0\x9F\x91\x88\xF0\x9F\x8F\xBB"] = ":point_left_tone1:";
    lEmoticonsEmoi["\xF0\x9F\x91\x88\xF0\x9F\x8F\xBC"] = ":point_left_tone2:";
    lEmoticonsEmoi["\xF0\x9F\x91\x88\xF0\x9F\x8F\xBD"] = ":point_left_tone3:";
    lEmoticonsEmoi["\xF0\x9F\x91\x88\xF0\x9F\x8F\xBE"] = ":point_left_tone4:";
    lEmoticonsEmoi["\xF0\x9F\x91\x88\xF0\x9F\x8F\xBF"] = ":point_left_tone5:";
    lEmoticonsEmoi["\xF0\x9F\x91\x89\xF0\x9F\x8F\xBB"] = ":point_right_tone1:";
    lEmoticonsEmoi["\xF0\x9F\x91\x89\xF0\x9F\x8F\xBC"] = ":point_right_tone2:";
    lEmoticonsEmoi["\xF0\x9F\x91\x89\xF0\x9F\x8F\xBD"] = ":point_right_tone3:";
    lEmoticonsEmoi["\xF0\x9F\x91\x89\xF0\x9F\x8F\xBE"] = ":point_right_tone4:";
    lEmoticonsEmoi["\xF0\x9F\x91\x89\xF0\x9F\x8F\xBF"] = ":point_right_tone5:";
    lEmoticonsEmoi["\xF0\x9F\x91\x8C\xF0\x9F\x8F\xBB"] = ":ok_hand_tone1:";
    lEmoticonsEmoi["\xF0\x9F\x91\x8C\xF0\x9F\x8F\xBC"] = ":ok_hand_tone2:";
    lEmoticonsEmoi["\xF0\x9F\x91\x8C\xF0\x9F\x8F\xBD"] = ":ok_hand_tone3:";
    lEmoticonsEmoi["\xF0\x9F\x91\x8C\xF0\x9F\x8F\xBE"] = ":ok_hand_tone4:";
    lEmoticonsEmoi["\xF0\x9F\x91\x8C\xF0\x9F\x8F\xBF"] = ":ok_hand_tone5:";
    lEmoticonsEmoi["\xE2\x9C\x8C\xF0\x9F\x8F\xBB"] = ":v_tone1:";
    lEmoticonsEmoi["\xE2\x9C\x8C\xF0\x9F\x8F\xBC"] = ":v_tone2:";
    lEmoticonsEmoi["\xE2\x9C\x8C\xF0\x9F\x8F\xBD"] = ":v_tone3:";
    lEmoticonsEmoi["\xE2\x9C\x8C\xF0\x9F\x8F\xBE"] = ":v_tone4:";
    lEmoticonsEmoi["\xE2\x9C\x8C\xF0\x9F\x8F\xBF"] = ":v_tone5:";
    lEmoticonsEmoi["\xF0\x9F\x91\x8A\xF0\x9F\x8F\xBB"] = ":punch_tone1:";
    lEmoticonsEmoi["\xF0\x9F\x91\x8A\xF0\x9F\x8F\xBC"] = ":punch_tone2:";
    lEmoticonsEmoi["\xF0\x9F\x91\x8A\xF0\x9F\x8F\xBD"] = ":punch_tone3:";
    lEmoticonsEmoi["\xF0\x9F\x91\x8A\xF0\x9F\x8F\xBE"] = ":punch_tone4:";
    lEmoticonsEmoi["\xF0\x9F\x91\x8A\xF0\x9F\x8F\xBF"] = ":punch_tone5:";
    lEmoticonsEmoi["\xE2\x9C\x8A\xF0\x9F\x8F\xBB"] = ":fist_tone1:";
    lEmoticonsEmoi["\xE2\x9C\x8A\xF0\x9F\x8F\xBC"] = ":fist_tone2:";
    lEmoticonsEmoi["\xE2\x9C\x8A\xF0\x9F\x8F\xBD"] = ":fist_tone3:";
    lEmoticonsEmoi["\xE2\x9C\x8A\xF0\x9F\x8F\xBE"] = ":fist_tone4:";
    lEmoticonsEmoi["\xE2\x9C\x8A\xF0\x9F\x8F\xBF"] = ":fist_tone5:";
    lEmoticonsEmoi["\xE2\x9C\x8B\xF0\x9F\x8F\xBB"] = ":raised_hand_tone1:";
    lEmoticonsEmoi["\xE2\x9C\x8B\xF0\x9F\x8F\xBC"] = ":raised_hand_tone2:";
    lEmoticonsEmoi["\xE2\x9C\x8B\xF0\x9F\x8F\xBD"] = ":raised_hand_tone3:";
    lEmoticonsEmoi["\xE2\x9C\x8B\xF0\x9F\x8F\xBE"] = ":raised_hand_tone4:";
    lEmoticonsEmoi["\xE2\x9C\x8B\xF0\x9F\x8F\xBF"] = ":raised_hand_tone5:";
    lEmoticonsEmoi["\xF0\x9F\x92\xAA\xF0\x9F\x8F\xBB"] = ":muscle_tone1:";
    lEmoticonsEmoi["\xF0\x9F\x92\xAA\xF0\x9F\x8F\xBC"] = ":muscle_tone2:";
    lEmoticonsEmoi["\xF0\x9F\x92\xAA\xF0\x9F\x8F\xBD"] = ":muscle_tone3:";
    lEmoticonsEmoi["\xF0\x9F\x92\xAA\xF0\x9F\x8F\xBE"] = ":muscle_tone4:";
    lEmoticonsEmoi["\xF0\x9F\x92\xAA\xF0\x9F\x8F\xBF"] = ":muscle_tone5:";
    lEmoticonsEmoi["\xF0\x9F\x91\x90\xF0\x9F\x8F\xBB"] = ":open_hands_tone1:";
    lEmoticonsEmoi["\xF0\x9F\x91\x90\xF0\x9F\x8F\xBC"] = ":open_hands_tone2:";
    lEmoticonsEmoi["\xF0\x9F\x91\x90\xF0\x9F\x8F\xBD"] = ":open_hands_tone3:";
    lEmoticonsEmoi["\xF0\x9F\x91\x90\xF0\x9F\x8F\xBE"] = ":open_hands_tone4:";
    lEmoticonsEmoi["\xF0\x9F\x91\x90\xF0\x9F\x8F\xBF"] = ":open_hands_tone5:";
    lEmoticonsEmoi["\xF0\x9F\x99\x8F\xF0\x9F\x8F\xBB"] = ":pray_tone1:";
    lEmoticonsEmoi["\xF0\x9F\x99\x8F\xF0\x9F\x8F\xBC"] = ":pray_tone2:";
    lEmoticonsEmoi["\xF0\x9F\x99\x8F\xF0\x9F\x8F\xBD"] = ":pray_tone3:";
    lEmoticonsEmoi["\xF0\x9F\x99\x8F\xF0\x9F\x8F\xBE"] = ":pray_tone4:";
    lEmoticonsEmoi["\xF0\x9F\x99\x8F\xF0\x9F\x8F\xBF"] = ":pray_tone5:";
    lEmoticonsEmoi["\xF0\x9F\x8F\x83\xF0\x9F\x8F\xBB"] = ":runner_tone1:";
    lEmoticonsEmoi["\xF0\x9F\x8F\x83\xF0\x9F\x8F\xBC"] = ":runner_tone2:";
    lEmoticonsEmoi["\xF0\x9F\x8F\x83\xF0\x9F\x8F\xBD"] = ":runner_tone3:";
    lEmoticonsEmoi["\xF0\x9F\x8F\x83\xF0\x9F\x8F\xBE"] = ":runner_tone4:";
    lEmoticonsEmoi["\xF0\x9F\x8F\x83\xF0\x9F\x8F\xBF"] = ":runner_tone5:";
    lEmoticonsEmoi["\xF0\x9F\x9A\xB6\xF0\x9F\x8F\xBB"] = ":walking_tone1:";
    lEmoticonsEmoi["\xF0\x9F\x9A\xB6\xF0\x9F\x8F\xBC"] = ":walking_tone2:";
    lEmoticonsEmoi["\xF0\x9F\x9A\xB6\xF0\x9F\x8F\xBD"] = ":walking_tone3:";
    lEmoticonsEmoi["\xF0\x9F\x9A\xB6\xF0\x9F\x8F\xBE"] = ":walking_tone4:";
    lEmoticonsEmoi["\xF0\x9F\x9A\xB6\xF0\x9F\x8F\xBF"] = ":walking_tone5:";
    lEmoticonsEmoi["\xF0\x9F\x92\x83\xF0\x9F\x8F\xBB"] = ":dancer_tone1:";
    lEmoticonsEmoi["\xF0\x9F\x92\x83\xF0\x9F\x8F\xBC"] = ":dancer_tone2:";
    lEmoticonsEmoi["\xF0\x9F\x92\x83\xF0\x9F\x8F\xBD"] = ":dancer_tone3:";
    lEmoticonsEmoi["\xF0\x9F\x92\x83\xF0\x9F\x8F\xBE"] = ":dancer_tone4:";
    lEmoticonsEmoi["\xF0\x9F\x92\x83\xF0\x9F\x8F\xBF"] = ":dancer_tone5:";
    lEmoticonsEmoi["\xF0\x9F\x9A\xA3\xF0\x9F\x8F\xBB"] = ":rowboat_tone1:";
    lEmoticonsEmoi["\xF0\x9F\x9A\xA3\xF0\x9F\x8F\xBC"] = ":rowboat_tone2:";
    lEmoticonsEmoi["\xF0\x9F\x9A\xA3\xF0\x9F\x8F\xBD"] = ":rowboat_tone3:";
    lEmoticonsEmoi["\xF0\x9F\x9A\xA3\xF0\x9F\x8F\xBE"] = ":rowboat_tone4:";
    lEmoticonsEmoi["\xF0\x9F\x9A\xA3\xF0\x9F\x8F\xBF"] = ":rowboat_tone5:";
    lEmoticonsEmoi["\xF0\x9F\x8F\x8A\xF0\x9F\x8F\xBB"] = ":swimmer_tone1:";
    lEmoticonsEmoi["\xF0\x9F\x8F\x8A\xF0\x9F\x8F\xBC"] = ":swimmer_tone2:";
    lEmoticonsEmoi["\xF0\x9F\x8F\x8A\xF0\x9F\x8F\xBD"] = ":swimmer_tone3:";
    lEmoticonsEmoi["\xF0\x9F\x8F\x8A\xF0\x9F\x8F\xBE"] = ":swimmer_tone4:";
    lEmoticonsEmoi["\xF0\x9F\x8F\x8A\xF0\x9F\x8F\xBF"] = ":swimmer_tone5:";
    lEmoticonsEmoi["\xF0\x9F\x8F\x84\xF0\x9F\x8F\xBB"] = ":surfer_tone1:";
    lEmoticonsEmoi["\xF0\x9F\x8F\x84\xF0\x9F\x8F\xBC"] = ":surfer_tone2:";
    lEmoticonsEmoi["\xF0\x9F\x8F\x84\xF0\x9F\x8F\xBD"] = ":surfer_tone3:";
    lEmoticonsEmoi["\xF0\x9F\x8F\x84\xF0\x9F\x8F\xBE"] = ":surfer_tone4:";
    lEmoticonsEmoi["\xF0\x9F\x8F\x84\xF0\x9F\x8F\xBF"] = ":surfer_tone5:";
    lEmoticonsEmoi["\xF0\x9F\x9B\x80\xF0\x9F\x8F\xBB"] = ":bath_tone1:";
    lEmoticonsEmoi["\xF0\x9F\x9B\x80\xF0\x9F\x8F\xBC"] = ":bath_tone2:";
    lEmoticonsEmoi["\xF0\x9F\x9B\x80\xF0\x9F\x8F\xBD"] = ":bath_tone3:";
    lEmoticonsEmoi["\xF0\x9F\x9B\x80\xF0\x9F\x8F\xBE"] = ":bath_tone4:";
    lEmoticonsEmoi["\xF0\x9F\x9B\x80\xF0\x9F\x8F\xBF"] = ":bath_tone5:";
    lEmoticonsEmoi["\xF0\x9F\x9A\xB4\xF0\x9F\x8F\xBB"] = ":bicyclist_tone1:";
    lEmoticonsEmoi["\xF0\x9F\x9A\xB4\xF0\x9F\x8F\xBC"] = ":bicyclist_tone2:";
    lEmoticonsEmoi["\xF0\x9F\x9A\xB4\xF0\x9F\x8F\xBD"] = ":bicyclist_tone3:";
    lEmoticonsEmoi["\xF0\x9F\x9A\xB4\xF0\x9F\x8F\xBE"] = ":bicyclist_tone4:";
    lEmoticonsEmoi["\xF0\x9F\x9A\xB4\xF0\x9F\x8F\xBF"] = ":bicyclist_tone5:";
    lEmoticonsEmoi["\xF0\x9F\x9A\xB5\xF0\x9F\x8F\xBB"] = ":mountain_bicyclist_tone1:";
    lEmoticonsEmoi["\xF0\x9F\x9A\xB5\xF0\x9F\x8F\xBC"] = ":mountain_bicyclist_tone2:";
    lEmoticonsEmoi["\xF0\x9F\x9A\xB5\xF0\x9F\x8F\xBD"] = ":mountain_bicyclist_tone3:";
    lEmoticonsEmoi["\xF0\x9F\x9A\xB5\xF0\x9F\x8F\xBE"] = ":mountain_bicyclist_tone4:";
    lEmoticonsEmoi["\xF0\x9F\x9A\xB5\xF0\x9F\x8F\xBF"] = ":mountain_bicyclist_tone5:";
    lEmoticonsEmoi["\xF0\x9F\x8F\x87\xF0\x9F\x8F\xBB"] = ":horse_racing_tone1:";
    lEmoticonsEmoi["\xF0\x9F\x8F\x87\xF0\x9F\x8F\xBC"] = ":horse_racing_tone2:";
    lEmoticonsEmoi["\xF0\x9F\x8F\x87\xF0\x9F\x8F\xBD"] = ":horse_racing_tone3:";
    lEmoticonsEmoi["\xF0\x9F\x8F\x87\xF0\x9F\x8F\xBE"] = ":horse_racing_tone4:";
    lEmoticonsEmoi["\xF0\x9F\x8F\x87\xF0\x9F\x8F\xBF"] = ":horse_racing_tone5:";
    lEmoticonsEmoi["\xE2\x9C\x8D\xF0\x9F\x8F\xBB"] = ":writing_hand_tone1:";
    lEmoticonsEmoi["\xE2\x9C\x8D\xF0\x9F\x8F\xBC"] = ":writing_hand_tone2:";
    lEmoticonsEmoi["\xE2\x9C\x8D\xF0\x9F\x8F\xBD"] = ":writing_hand_tone3:";
    lEmoticonsEmoi["\xE2\x9C\x8D\xF0\x9F\x8F\xBE"] = ":writing_hand_tone4:";
    lEmoticonsEmoi["\xE2\x9C\x8D\xF0\x9F\x8F\xBF"] = ":writing_hand_tone5:";
    lEmoticonsEmoi["\xF0\x9F\x96\x90\xF0\x9F\x8F\xBB"] = ":hand_splayed_tone1:";
    lEmoticonsEmoi["\xF0\x9F\x96\x90\xF0\x9F\x8F\xBB"] = ":raised_hand_with_fingers_splayed_tone1:";
    lEmoticonsEmoi["\xF0\x9F\x96\x90\xF0\x9F\x8F\xBC"] = ":hand_splayed_tone2:";
    lEmoticonsEmoi["\xF0\x9F\x96\x90\xF0\x9F\x8F\xBC"] = ":raised_hand_with_fingers_splayed_tone2:";
    lEmoticonsEmoi["\xF0\x9F\x96\x90\xF0\x9F\x8F\xBD"] = ":hand_splayed_tone3:";
    lEmoticonsEmoi["\xF0\x9F\x96\x90\xF0\x9F\x8F\xBD"] = ":raised_hand_with_fingers_splayed_tone3:";
    lEmoticonsEmoi["\xF0\x9F\x96\x90\xF0\x9F\x8F\xBE"] = ":hand_splayed_tone4:";
    lEmoticonsEmoi["\xF0\x9F\x96\x90\xF0\x9F\x8F\xBE"] = ":raised_hand_with_fingers_splayed_tone4:";
    lEmoticonsEmoi["\xF0\x9F\x96\x90\xF0\x9F\x8F\xBF"] = ":hand_splayed_tone5:";
    lEmoticonsEmoi["\xF0\x9F\x96\x90\xF0\x9F\x8F\xBF"] = ":raised_hand_with_fingers_splayed_tone5:";
    lEmoticonsEmoi["\xF0\x9F\x96\x95\xF0\x9F\x8F\xBB"] = ":middle_finger_tone1:";
    lEmoticonsEmoi["\xF0\x9F\x96\x95\xF0\x9F\x8F\xBB"] = ":reversed_hand_with_middle_finger_extended_tone1:";
    lEmoticonsEmoi["\xF0\x9F\x96\x95\xF0\x9F\x8F\xBC"] = ":middle_finger_tone2:";
    lEmoticonsEmoi["\xF0\x9F\x96\x95\xF0\x9F\x8F\xBC"] = ":reversed_hand_with_middle_finger_extended_tone2:";
    lEmoticonsEmoi["\xF0\x9F\x96\x95\xF0\x9F\x8F\xBD"] = ":middle_finger_tone3:";
    lEmoticonsEmoi["\xF0\x9F\x96\x95\xF0\x9F\x8F\xBD"] = ":reversed_hand_with_middle_finger_extended_tone3:";
    lEmoticonsEmoi["\xF0\x9F\x96\x95\xF0\x9F\x8F\xBE"] = ":middle_finger_tone4:";
    lEmoticonsEmoi["\xF0\x9F\x96\x95\xF0\x9F\x8F\xBE"] = ":reversed_hand_with_middle_finger_extended_tone4:";
    lEmoticonsEmoi["\xF0\x9F\x96\x95\xF0\x9F\x8F\xBF"] = ":middle_finger_tone5:";
    lEmoticonsEmoi["\xF0\x9F\x96\x95\xF0\x9F\x8F\xBF"] = ":reversed_hand_with_middle_finger_extended_tone5:";
    lEmoticonsEmoi["\xF0\x9F\x96\x96\xF0\x9F\x8F\xBB"] = ":vulcan_tone1:";
    lEmoticonsEmoi["\xF0\x9F\x96\x96\xF0\x9F\x8F\xBB"] = ":raised_hand_with_part_between_middle_and_ring_fingers_tone1:";
    lEmoticonsEmoi["\xF0\x9F\x96\x96\xF0\x9F\x8F\xBC"] = ":vulcan_tone2:";
    lEmoticonsEmoi["\xF0\x9F\x96\x96\xF0\x9F\x8F\xBC"] = ":raised_hand_with_part_between_middle_and_ring_fingers_tone2:";
    lEmoticonsEmoi["\xF0\x9F\x96\x96\xF0\x9F\x8F\xBD"] = ":vulcan_tone3:";
    lEmoticonsEmoi["\xF0\x9F\x96\x96\xF0\x9F\x8F\xBD"] = ":raised_hand_with_part_between_middle_and_ring_fingers_tone3:";
    lEmoticonsEmoi["\xF0\x9F\x96\x96\xF0\x9F\x8F\xBE"] = ":vulcan_tone4:";
    lEmoticonsEmoi["\xF0\x9F\x96\x96\xF0\x9F\x8F\xBE"] = ":raised_hand_with_part_between_middle_and_ring_fingers_tone4:";
    lEmoticonsEmoi["\xF0\x9F\x96\x96\xF0\x9F\x8F\xBF"] = ":vulcan_tone5:";
    lEmoticonsEmoi["\xF0\x9F\x96\x96\xF0\x9F\x8F\xBF"] = ":raised_hand_with_part_between_middle_and_ring_fingers_tone5:";
    lEmoticonsEmoi["\xF0\x9F\x91\xA8\xE2\x80\x8D\xF0\x9F\x91\xA8\xE2\x80\x8D\xF0\x9F\x91\xA6"] = ":family_mmb:";
    lEmoticonsEmoi["\xF0\x9F\x91\xA8\xE2\x80\x8D\xF0\x9F\x91\xA8\xE2\x80\x8D\xF0\x9F\x91\xA6\xE2\x80\x8D\xF0\x9F\x91\xA6"] = ":family_mmbb:";
    lEmoticonsEmoi["\xF0\x9F\x91\xA8\xE2\x80\x8D\xF0\x9F\x91\xA8\xE2\x80\x8D\xF0\x9F\x91\xA7"] = ":family_mmg:";
    lEmoticonsEmoi["\xF0\x9F\x91\xA8\xE2\x80\x8D\xF0\x9F\x91\xA8\xE2\x80\x8D\xF0\x9F\x91\xA7\xE2\x80\x8D\xF0\x9F\x91\xA6"] = ":family_mmgb:";
    lEmoticonsEmoi["\xF0\x9F\x91\xA8\xE2\x80\x8D\xF0\x9F\x91\xA8\xE2\x80\x8D\xF0\x9F\x91\xA7\xE2\x80\x8D\xF0\x9F\x91\xA7"] = ":family_mmgg:";
    lEmoticonsEmoi["\xF0\x9F\x91\xA8\xE2\x80\x8D\xF0\x9F\x91\xA9\xE2\x80\x8D\xF0\x9F\x91\xA6\xE2\x80\x8D\xF0\x9F\x91\xA6"] = ":family_mwbb:";
    lEmoticonsEmoi["\xF0\x9F\x91\xA8\xE2\x80\x8D\xF0\x9F\x91\xA9\xE2\x80\x8D\xF0\x9F\x91\xA7"] = ":family_mwg:";
    lEmoticonsEmoi["\xF0\x9F\x91\xA8\xE2\x80\x8D\xF0\x9F\x91\xA9\xE2\x80\x8D\xF0\x9F\x91\xA7\xE2\x80\x8D\xF0\x9F\x91\xA6"] = ":family_mwgb:";
    lEmoticonsEmoi["\xF0\x9F\x91\xA8\xE2\x80\x8D\xF0\x9F\x91\xA9\xE2\x80\x8D\xF0\x9F\x91\xA7\xE2\x80\x8D\xF0\x9F\x91\xA7"] = ":family_mwgg:";
    lEmoticonsEmoi["\xF0\x9F\x91\xA9\xE2\x80\x8D\xF0\x9F\x91\xA9\xE2\x80\x8D\xF0\x9F\x91\xA6"] = ":family_wwb:";
    lEmoticonsEmoi["\xF0\x9F\x91\xA9\xE2\x80\x8D\xF0\x9F\x91\xA9\xE2\x80\x8D\xF0\x9F\x91\xA6\xE2\x80\x8D\xF0\x9F\x91\xA6"] = ":family_wwbb:";
    lEmoticonsEmoi["\xF0\x9F\x91\xA9\xE2\x80\x8D\xF0\x9F\x91\xA9\xE2\x80\x8D\xF0\x9F\x91\xA7"] = ":family_wwg:";
    lEmoticonsEmoi["\xF0\x9F\x91\xA9\xE2\x80\x8D\xF0\x9F\x91\xA9\xE2\x80\x8D\xF0\x9F\x91\xA7\xE2\x80\x8D\xF0\x9F\x91\xA6"] = ":family_wwgb:";
    lEmoticonsEmoi["\xF0\x9F\x91\xA9\xE2\x80\x8D\xF0\x9F\x91\xA9\xE2\x80\x8D\xF0\x9F\x91\xA7\xE2\x80\x8D\xF0\x9F\x91\xA7"] = ":family_wwgg:";
    lEmoticonsEmoi["\xF0\x9F\x91\xA9\xE2\x80\x8D\xE2\x9D\xA4\xEF\xB8\x8F\xE2\x80\x8D\xF0\x9F\x91\xA9"] = ":couple_ww:";
    lEmoticonsEmoi["\xF0\x9F\x91\xA9\xE2\x80\x8D\xE2\x9D\xA4\xEF\xB8\x8F\xE2\x80\x8D\xF0\x9F\x91\xA9"] = ":couple_with_heart_ww:";
    lEmoticonsEmoi["\xF0\x9F\x91\xA8\xE2\x80\x8D\xE2\x9D\xA4\xEF\xB8\x8F\xE2\x80\x8D\xF0\x9F\x91\xA8"] = ":couple_mm:";
    lEmoticonsEmoi["\xF0\x9F\x91\xA8\xE2\x80\x8D\xE2\x9D\xA4\xEF\xB8\x8F\xE2\x80\x8D\xF0\x9F\x91\xA8"] = ":couple_with_heart_mm:";
    lEmoticonsEmoi["\xF0\x9F\x91\xA9\xE2\x80\x8D\xE2\x9D\xA4\xEF\xB8\x8F\xE2\x80\x8D\xF0\x9F\x92\x8B\xE2\x80\x8D\xF0\x9F\x91\xA9"] = ":kiss_ww:";
    lEmoticonsEmoi["\xF0\x9F\x91\xA9\xE2\x80\x8D\xE2\x9D\xA4\xEF\xB8\x8F\xE2\x80\x8D\xF0\x9F\x92\x8B\xE2\x80\x8D\xF0\x9F\x91\xA9"] = ":couplekiss_ww:";
    lEmoticonsEmoi["\xF0\x9F\x91\xA8\xE2\x80\x8D\xE2\x9D\xA4\xEF\xB8\x8F\xE2\x80\x8D\xF0\x9F\x92\x8B\xE2\x80\x8D\xF0\x9F\x91\xA8"] = ":kiss_mm:";
    lEmoticonsEmoi["\xF0\x9F\x91\xA8\xE2\x80\x8D\xE2\x9D\xA4\xEF\xB8\x8F\xE2\x80\x8D\xF0\x9F\x92\x8B\xE2\x80\x8D\xF0\x9F\x91\xA8"] = ":couplekiss_mm:";
    lEmoticonsEmoi["\xF0\x9F\x8F\xBB"] = ":tone1:";
    lEmoticonsEmoi["\xF0\x9F\x8F\xBC"] = ":tone2:";
    lEmoticonsEmoi["\xF0\x9F\x8F\xBD"] = ":tone3:";
    lEmoticonsEmoi["\xF0\x9F\x8F\xBE"] = ":tone4:";
    lEmoticonsEmoi["\xF0\x9F\x8F\xBF"] = ":tone5:";
    lEmoticonsEmoi["\x2A\xEF\xB8\x8F\xE2\x83\xA3"] = ":asterisk:";
    lEmoticonsEmoi["\x2A\xEF\xB8\x8F\xE2\x83\xA3"] = ":keycap_asterisk:";
    lEmoticonsEmoi["\xE2\x8F\xAD"] = ":track_next:";
    lEmoticonsEmoi["\xE2\x8F\xAD"] = ":next_track:";
    lEmoticonsEmoi["\xE2\x8F\xAE"] = ":track_previous:";
    lEmoticonsEmoi["\xE2\x8F\xAE"] = ":previous_track:";
    lEmoticonsEmoi["\xE2\x8F\xAF"] = ":play_pause:";
    lEmoticonsEmoi["\xF0\x9F\x91\x81\xE2\x80\x8D\xF0\x9F\x97\xA8"] = ":eye_in_speech_bubble:";
    lEmoticonsEmoi["\xE2\x8F\xB1"] = ":stopwatch:";
    lEmoticonsEmoi["\xE2\x8F\xB2"] = ":timer:";
    lEmoticonsEmoi["\xE2\x8F\xB2"] = ":timer_clock:";
    lEmoticonsEmoi["\xE2\x8F\xB8"] = ":pause_button:";
    lEmoticonsEmoi["\xE2\x8F\xB8"] = ":double_vertical_bar:";
    lEmoticonsEmoi["\xE2\x8F\xB9"] = ":stop_button:";
    lEmoticonsEmoi["\xE2\x8F\xBA"] = ":record_button:";
    lEmoticonsEmoi["\xE2\x98\x82"] = ":umbrella2:";
    lEmoticonsEmoi["\xE2\x98\x83"] = ":snowman2:";
    lEmoticonsEmoi["\xE2\x98\x84"] = ":comet:";
    lEmoticonsEmoi["\xE2\x98\x98"] = ":shamrock:";
    lEmoticonsEmoi["\xE2\x98\xA0"] = ":skull_crossbones:";
    lEmoticonsEmoi["\xE2\x98\xA0"] = ":skull_and_crossbones:";
    lEmoticonsEmoi["\xE2\x98\xA2"] = ":radioactive:";
    lEmoticonsEmoi["\xE2\x98\xA2"] = ":radioactive_sign:";
    lEmoticonsEmoi["\xE2\x98\xA3"] = ":biohazard:";
    lEmoticonsEmoi["\xE2\x98\xA3"] = ":biohazard_sign:";
    lEmoticonsEmoi["\xE2\x98\xA6"] = ":orthodox_cross:";
    lEmoticonsEmoi["\xE2\x98\xAA"] = ":star_and_crescent:";
    lEmoticonsEmoi["\xE2\x98\xAE"] = ":peace:";
    lEmoticonsEmoi["\xE2\x98\xAE"] = ":peace_symbol:";
    lEmoticonsEmoi["\xE2\x98\xAF"] = ":yin_yang:";
    lEmoticonsEmoi["\xE2\x98\xB8"] = ":wheel_of_dharma:";
    lEmoticonsEmoi["\xE2\x98\xB9"] = ":frowning2:";
    lEmoticonsEmoi["\xE2\x98\xB9"] = ":white_frowning_face:";
    lEmoticonsEmoi["\xE2\x9A\x92"] = ":hammer_pick:";
    lEmoticonsEmoi["\xE2\x9A\x92"] = ":hammer_and_pick:";
    lEmoticonsEmoi["\xE2\x9A\x94"] = ":crossed_swords:";
    lEmoticonsEmoi["\xE2\x9A\x96"] = ":scales:";
    lEmoticonsEmoi["\xE2\x9A\x97"] = ":alembic:";
    lEmoticonsEmoi["\xE2\x9A\x99"] = ":gear:";
    lEmoticonsEmoi["\xE2\x9A\x9B"] = ":atom:";
    lEmoticonsEmoi["\xE2\x9A\x9B"] = ":atom_symbol:";
    lEmoticonsEmoi["\xE2\x9A\x9C"] = ":fleur-de-lis:";
    lEmoticonsEmoi["\xE2\x9A\xB0"] = ":coffin:";
    lEmoticonsEmoi["\xE2\x9A\xB1"] = ":urn:";
    lEmoticonsEmoi["\xE2\x9A\xB1"] = ":funeral_urn:";
    lEmoticonsEmoi["\xE2\x9B\x88"] = ":thunder_cloud_rain:";
    lEmoticonsEmoi["\xE2\x9B\x88"] = ":thunder_cloud_and_rain:";
    lEmoticonsEmoi["\xE2\x9B\x8F"] = ":pick:";
    lEmoticonsEmoi["\xE2\x9B\x91"] = ":helmet_with_cross:";
    lEmoticonsEmoi["\xE2\x9B\x91"] = ":helmet_with_white_cross:";
    lEmoticonsEmoi["\xE2\x9B\x93"] = ":chains:";
    lEmoticonsEmoi["\xE2\x9B\xA9"] = ":shinto_shrine:";
    lEmoticonsEmoi["\xE2\x9B\xB0"] = ":mountain:";
    lEmoticonsEmoi["\xE2\x9B\xB1"] = ":beach_umbrella:";
    lEmoticonsEmoi["\xE2\x9B\xB1"] = ":umbrella_on_ground:";
    lEmoticonsEmoi["\xE2\x9B\xB4"] = ":ferry:";
    lEmoticonsEmoi["\xE2\x9B\xB7"] = ":skier:";
    lEmoticonsEmoi["\xE2\x9B\xB8"] = ":ice_skate:";
    lEmoticonsEmoi["\xE2\x9B\xB9"] = ":basketball_player:";
    lEmoticonsEmoi["\xE2\x9B\xB9"] = ":person_with_ball:";
    lEmoticonsEmoi["\xE2\x9C\xA1"] = ":star_of_david:";
    lEmoticonsEmoi["\xE2\x9D\xA3"] = ":heart_exclamation:";
    lEmoticonsEmoi["\xE2\x9D\xA3"] = ":heavy_heart_exclamation_mark_ornament:";
    lEmoticonsEmoi["\xF0\x9F\x8C\xA4"] = ":white_sun_small_cloud:";
    lEmoticonsEmoi["\xF0\x9F\x8C\xA4"] = ":white_sun_with_small_cloud:";
    lEmoticonsEmoi["\xF0\x9F\x8C\xA5"] = ":white_sun_cloud:";
    lEmoticonsEmoi["\xF0\x9F\x8C\xA5"] = ":white_sun_behind_cloud:";
    lEmoticonsEmoi["\xF0\x9F\x8C\xA6"] = ":white_sun_rain_cloud:";
    lEmoticonsEmoi["\xF0\x9F\x8C\xA6"] = ":white_sun_behind_cloud_with_rain:";
    lEmoticonsEmoi["\xF0\x9F\x96\xB1"] = ":mouse_three_button:";
    lEmoticonsEmoi["\xF0\x9F\x96\xB1"] = ":three_button_mouse:";
    lEmoticonsEmoi["\xF0\x9F\x8E\x85\xF0\x9F\x8F\xBB"] = ":santa_tone1:";
    lEmoticonsEmoi["\xF0\x9F\x8E\x85\xF0\x9F\x8F\xBC"] = ":santa_tone2:";
    lEmoticonsEmoi["\xF0\x9F\x8E\x85\xF0\x9F\x8F\xBD"] = ":santa_tone3:";
    lEmoticonsEmoi["\xF0\x9F\x8E\x85\xF0\x9F\x8F\xBE"] = ":santa_tone4:";
    lEmoticonsEmoi["\xF0\x9F\x8E\x85\xF0\x9F\x8F\xBF"] = ":santa_tone5:";
    lEmoticonsEmoi["\xF0\x9F\xA4\x98\xF0\x9F\x8F\xBB"] = ":metal_tone1:";
    lEmoticonsEmoi["\xF0\x9F\xA4\x98\xF0\x9F\x8F\xBB"] = ":sign_of_the_horns_tone1:";
    lEmoticonsEmoi["\xF0\x9F\xA4\x98\xF0\x9F\x8F\xBC"] = ":metal_tone2:";
    lEmoticonsEmoi["\xF0\x9F\xA4\x98\xF0\x9F\x8F\xBC"] = ":sign_of_the_horns_tone2:";
    lEmoticonsEmoi["\xF0\x9F\xA4\x98\xF0\x9F\x8F\xBD"] = ":metal_tone3:";
    lEmoticonsEmoi["\xF0\x9F\xA4\x98\xF0\x9F\x8F\xBD"] = ":sign_of_the_horns_tone3:";
    lEmoticonsEmoi["\xF0\x9F\xA4\x98\xF0\x9F\x8F\xBE"] = ":metal_tone4:";
    lEmoticonsEmoi["\xF0\x9F\xA4\x98\xF0\x9F\x8F\xBE"] = ":sign_of_the_horns_tone4:";
    lEmoticonsEmoi["\xF0\x9F\xA4\x98\xF0\x9F\x8F\xBF"] = ":metal_tone5:";
    lEmoticonsEmoi["\xF0\x9F\xA4\x98\xF0\x9F\x8F\xBF"] = ":sign_of_the_horns_tone5:";
    lEmoticonsEmoi["\xF0\x9F\x8F\x8B\xF0\x9F\x8F\xBB"] = ":lifter_tone1:";
    lEmoticonsEmoi["\xF0\x9F\x8F\x8B\xF0\x9F\x8F\xBB"] = ":weight_lifter_tone1:";
    lEmoticonsEmoi["\xF0\x9F\x8F\x8B\xF0\x9F\x8F\xBC"] = ":lifter_tone2:";
    lEmoticonsEmoi["\xF0\x9F\x8F\x8B\xF0\x9F\x8F\xBC"] = ":weight_lifter_tone2:";
    lEmoticonsEmoi["\xF0\x9F\x8F\x8B\xF0\x9F\x8F\xBD"] = ":lifter_tone3:";
    lEmoticonsEmoi["\xF0\x9F\x8F\x8B\xF0\x9F\x8F\xBD"] = ":weight_lifter_tone3:";
    lEmoticonsEmoi["\xF0\x9F\x8F\x8B\xF0\x9F\x8F\xBE"] = ":lifter_tone4:";
    lEmoticonsEmoi["\xF0\x9F\x8F\x8B\xF0\x9F\x8F\xBE"] = ":weight_lifter_tone4:";
    lEmoticonsEmoi["\xF0\x9F\x8F\x8B\xF0\x9F\x8F\xBF"] = ":lifter_tone5:";
    lEmoticonsEmoi["\xF0\x9F\x8F\x8B\xF0\x9F\x8F\xBF"] = ":weight_lifter_tone5:";
    lEmoticonsEmoi["\xE2\x9B\xB9\xF0\x9F\x8F\xBB"] = ":basketball_player_tone1:";
    lEmoticonsEmoi["\xE2\x9B\xB9\xF0\x9F\x8F\xBB"] = ":person_with_ball_tone1:";
    lEmoticonsEmoi["\xE2\x9B\xB9\xF0\x9F\x8F\xBC"] = ":basketball_player_tone2:";
    lEmoticonsEmoi["\xE2\x9B\xB9\xF0\x9F\x8F\xBC"] = ":person_with_ball_tone2:";
    lEmoticonsEmoi["\xE2\x9B\xB9\xF0\x9F\x8F\xBD"] = ":basketball_player_tone3:";
    lEmoticonsEmoi["\xE2\x9B\xB9\xF0\x9F\x8F\xBD"] = ":person_with_ball_tone3:";
    lEmoticonsEmoi["\xE2\x9B\xB9\xF0\x9F\x8F\xBE"] = ":basketball_player_tone4:";
    lEmoticonsEmoi["\xE2\x9B\xB9\xF0\x9F\x8F\xBE"] = ":person_with_ball_tone4:";
    lEmoticonsEmoi["\xE2\x9B\xB9\xF0\x9F\x8F\xBF"] = ":basketball_player_tone5:";
    lEmoticonsEmoi["\xE2\x9B\xB9\xF0\x9F\x8F\xBF"] = ":person_with_ball_tone5:";
    lEmoticonsEmoi["\xF0\x9F\x99\x83"] = ":upside_down:";
    lEmoticonsEmoi["\xF0\x9F\x99\x83"] = ":upside_down_face:";
    lEmoticonsEmoi["\xF0\x9F\xA4\x91"] = ":money_mouth:";
    lEmoticonsEmoi["\xF0\x9F\xA4\x91"] = ":money_mouth_face:";
    lEmoticonsEmoi["\xF0\x9F\xA4\x93"] = ":nerd:";
    lEmoticonsEmoi["\xF0\x9F\xA4\x93"] = ":nerd_face:";
    lEmoticonsEmoi["\xF0\x9F\xA4\x97"] = ":hugging:";
    lEmoticonsEmoi["\xF0\x9F\xA4\x97"] = ":hugging_face:";
    lEmoticonsEmoi["\xF0\x9F\x99\x84"] = ":rolling_eyes:";
    lEmoticonsEmoi["\xF0\x9F\x99\x84"] = ":face_with_rolling_eyes:";
    lEmoticonsEmoi["\xF0\x9F\xA4\x94"] = ":thinking:";
    lEmoticonsEmoi["\xF0\x9F\xA4\x94"] = ":thinking_face:";
    lEmoticonsEmoi["\xF0\x9F\xA4\x90"] = ":zipper_mouth:";
    lEmoticonsEmoi["\xF0\x9F\xA4\x90"] = ":zipper_mouth_face:";
    lEmoticonsEmoi["\xF0\x9F\xA4\x92"] = ":thermometer_face:";
    lEmoticonsEmoi["\xF0\x9F\xA4\x92"] = ":face_with_thermometer:";
    lEmoticonsEmoi["\xF0\x9F\xA4\x95"] = ":head_bandage:";
    lEmoticonsEmoi["\xF0\x9F\xA4\x95"] = ":face_with_head_bandage:";
    lEmoticonsEmoi["\xF0\x9F\xA4\x96"] = ":robot:";
    lEmoticonsEmoi["\xF0\x9F\xA4\x96"] = ":robot_face:";
    lEmoticonsEmoi["\xF0\x9F\xA6\x81"] = ":lion_face:";
    lEmoticonsEmoi["\xF0\x9F\xA6\x81"] = ":lion:";
    lEmoticonsEmoi["\xF0\x9F\xA6\x84"] = ":unicorn:";
    lEmoticonsEmoi["\xF0\x9F\xA6\x84"] = ":unicorn_face:";
    lEmoticonsEmoi["\xF0\x9F\xA6\x82"] = ":scorpion:";
    lEmoticonsEmoi["\xF0\x9F\xA6\x80"] = ":crab:";
    lEmoticonsEmoi["\xF0\x9F\xA6\x83"] = ":turkey:";
    lEmoticonsEmoi["\xF0\x9F\xA7\x80"] = ":cheese:";
    lEmoticonsEmoi["\xF0\x9F\xA7\x80"] = ":cheese_wedge:";
    lEmoticonsEmoi["\xF0\x9F\x8C\xAD"] = ":hotdog:";
    lEmoticonsEmoi["\xF0\x9F\x8C\xAD"] = ":hot_dog:";
    lEmoticonsEmoi["\xF0\x9F\x8C\xAE"] = ":taco:";
    lEmoticonsEmoi["\xF0\x9F\x8C\xAF"] = ":burrito:";
    lEmoticonsEmoi["\xF0\x9F\x8D\xBF"] = ":popcorn:";
    lEmoticonsEmoi["\xF0\x9F\x8D\xBE"] = ":champagne:";
    lEmoticonsEmoi["\xF0\x9F\x8D\xBE"] = ":bottle_with_popping_cork:";
    lEmoticonsEmoi["\xF0\x9F\x8F\xB9"] = ":bow_and_arrow:";
    lEmoticonsEmoi["\xF0\x9F\x8F\xB9"] = ":archery:";
    lEmoticonsEmoi["\xF0\x9F\x8F\xBA"] = ":amphora:";
    lEmoticonsEmoi["\xF0\x9F\x9B\x90"] = ":place_of_worship:";
    lEmoticonsEmoi["\xF0\x9F\x9B\x90"] = ":worship_symbol:";
    lEmoticonsEmoi["\xF0\x9F\x95\x8B"] = ":kaaba:";
    lEmoticonsEmoi["\xF0\x9F\x95\x8C"] = ":mosque:";
    lEmoticonsEmoi["\xF0\x9F\x95\x8D"] = ":synagogue:";
    lEmoticonsEmoi["\xF0\x9F\x95\x8E"] = ":menorah:";
    lEmoticonsEmoi["\xF0\x9F\x93\xBF"] = ":prayer_beads:";
    lEmoticonsEmoi["\xF0\x9F\x8F\x8F"] = ":cricket:";
    lEmoticonsEmoi["\xF0\x9F\x8F\x8F"] = ":cricket_bat_ball:";
    lEmoticonsEmoi["\xF0\x9F\x8F\x90"] = ":volleyball:";
    lEmoticonsEmoi["\xF0\x9F\x8F\x91"] = ":field_hockey:";
    lEmoticonsEmoi["\xF0\x9F\x8F\x92"] = ":hockey:";
    lEmoticonsEmoi["\xF0\x9F\x8F\x93"] = ":ping_pong:";
    lEmoticonsEmoi["\xF0\x9F\x8F\x93"] = ":table_tennis:";
    lEmoticonsEmoi["\xF0\x9F\x8F\xB8"] = ":badminton:";
    lEmoticonsEmoi["\xF0\x9F\x87\xA6\xF0\x9F\x87\xBD"] = ":flag_ax:";
    lEmoticonsEmoi["\xF0\x9F\x87\xA6\xF0\x9F\x87\xBD"] = ":ax:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB9\xF0\x9F\x87\xA6"] = ":flag_ta:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB9\xF0\x9F\x87\xA6"] = ":ta:";
    lEmoticonsEmoi["\xF0\x9F\x87\xAE\xF0\x9F\x87\xB4"] = ":flag_io:";
    lEmoticonsEmoi["\xF0\x9F\x87\xAE\xF0\x9F\x87\xB4"] = ":io:";
    lEmoticonsEmoi["\xF0\x9F\x87\xA7\xF0\x9F\x87\xB6"] = ":flag_bq:";
    lEmoticonsEmoi["\xF0\x9F\x87\xA7\xF0\x9F\x87\xB6"] = ":bq:";
    lEmoticonsEmoi["\xF0\x9F\x87\xA8\xF0\x9F\x87\xBD"] = ":flag_cx:";
    lEmoticonsEmoi["\xF0\x9F\x87\xA8\xF0\x9F\x87\xBD"] = ":cx:";
    lEmoticonsEmoi["\xF0\x9F\x87\xA8\xF0\x9F\x87\xA8"] = ":flag_cc:";
    lEmoticonsEmoi["\xF0\x9F\x87\xA8\xF0\x9F\x87\xA8"] = ":cc:";
    lEmoticonsEmoi["\xF0\x9F\x87\xAC\xF0\x9F\x87\xAC"] = ":flag_gg:";
    lEmoticonsEmoi["\xF0\x9F\x87\xAC\xF0\x9F\x87\xAC"] = ":gg:";
    lEmoticonsEmoi["\xF0\x9F\x87\xAE\xF0\x9F\x87\xB2"] = ":flag_im:";
    lEmoticonsEmoi["\xF0\x9F\x87\xAE\xF0\x9F\x87\xB2"] = ":im:";
    lEmoticonsEmoi["\xF0\x9F\x87\xBE\xF0\x9F\x87\xB9"] = ":flag_yt:";
    lEmoticonsEmoi["\xF0\x9F\x87\xBE\xF0\x9F\x87\xB9"] = ":yt:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB3\xF0\x9F\x87\xAB"] = ":flag_nf:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB3\xF0\x9F\x87\xAB"] = ":nf:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB5\xF0\x9F\x87\xB3"] = ":flag_pn:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB5\xF0\x9F\x87\xB3"] = ":pn:";
    lEmoticonsEmoi["\xF0\x9F\x87\xA7\xF0\x9F\x87\xB1"] = ":flag_bl:";
    lEmoticonsEmoi["\xF0\x9F\x87\xA7\xF0\x9F\x87\xB1"] = ":bl:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB5\xF0\x9F\x87\xB2"] = ":flag_pm:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB5\xF0\x9F\x87\xB2"] = ":pm:";
    lEmoticonsEmoi["\xF0\x9F\x87\xAC\xF0\x9F\x87\xB8"] = ":flag_gs:";
    lEmoticonsEmoi["\xF0\x9F\x87\xAC\xF0\x9F\x87\xB8"] = ":gs:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB9\xF0\x9F\x87\xB0"] = ":flag_tk:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB9\xF0\x9F\x87\xB0"] = ":tk:";
    lEmoticonsEmoi["\xF0\x9F\x87\xA7\xF0\x9F\x87\xBB"] = ":flag_bv:";
    lEmoticonsEmoi["\xF0\x9F\x87\xA7\xF0\x9F\x87\xBB"] = ":bv:";
    lEmoticonsEmoi["\xF0\x9F\x87\xAD\xF0\x9F\x87\xB2"] = ":flag_hm:";
    lEmoticonsEmoi["\xF0\x9F\x87\xAD\xF0\x9F\x87\xB2"] = ":hm:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB8\xF0\x9F\x87\xAF"] = ":flag_sj:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB8\xF0\x9F\x87\xAF"] = ":sj:";
    lEmoticonsEmoi["\xF0\x9F\x87\xBA\xF0\x9F\x87\xB2"] = ":flag_um:";
    lEmoticonsEmoi["\xF0\x9F\x87\xBA\xF0\x9F\x87\xB2"] = ":um:";
    lEmoticonsEmoi["\xF0\x9F\x87\xAE\xF0\x9F\x87\xA8"] = ":flag_ic:";
    lEmoticonsEmoi["\xF0\x9F\x87\xAE\xF0\x9F\x87\xA8"] = ":ic:";
    lEmoticonsEmoi["\xF0\x9F\x87\xAA\xF0\x9F\x87\xA6"] = ":flag_ea:";
    lEmoticonsEmoi["\xF0\x9F\x87\xAA\xF0\x9F\x87\xA6"] = ":ea:";
    lEmoticonsEmoi["\xF0\x9F\x87\xA8\xF0\x9F\x87\xB5"] = ":flag_cp:";
    lEmoticonsEmoi["\xF0\x9F\x87\xA8\xF0\x9F\x87\xB5"] = ":cp:";
    lEmoticonsEmoi["\xF0\x9F\x87\xA9\xF0\x9F\x87\xAC"] = ":flag_dg:";
    lEmoticonsEmoi["\xF0\x9F\x87\xA9\xF0\x9F\x87\xAC"] = ":dg:";
    lEmoticonsEmoi["\xF0\x9F\x87\xA6\xF0\x9F\x87\xB8"] = ":flag_as:";
    lEmoticonsEmoi["\xF0\x9F\x87\xA6\xF0\x9F\x87\xB8"] = ":as:";
    lEmoticonsEmoi["\xF0\x9F\x87\xA6\xF0\x9F\x87\xB6"] = ":flag_aq:";
    lEmoticonsEmoi["\xF0\x9F\x87\xA6\xF0\x9F\x87\xB6"] = ":aq:";
    lEmoticonsEmoi["\xF0\x9F\x87\xBB\xF0\x9F\x87\xAC"] = ":flag_vg:";
    lEmoticonsEmoi["\xF0\x9F\x87\xBB\xF0\x9F\x87\xAC"] = ":vg:";
    lEmoticonsEmoi["\xF0\x9F\x87\xA8\xF0\x9F\x87\xB0"] = ":flag_ck:";
    lEmoticonsEmoi["\xF0\x9F\x87\xA8\xF0\x9F\x87\xB0"] = ":ck:";
    lEmoticonsEmoi["\xF0\x9F\x87\xA8\xF0\x9F\x87\xBC"] = ":flag_cw:";
    lEmoticonsEmoi["\xF0\x9F\x87\xA8\xF0\x9F\x87\xBC"] = ":cw:";
    lEmoticonsEmoi["\xF0\x9F\x87\xAA\xF0\x9F\x87\xBA"] = ":flag_eu:";
    lEmoticonsEmoi["\xF0\x9F\x87\xAA\xF0\x9F\x87\xBA"] = ":eu:";
    lEmoticonsEmoi["\xF0\x9F\x87\xAC\xF0\x9F\x87\xAB"] = ":flag_gf:";
    lEmoticonsEmoi["\xF0\x9F\x87\xAC\xF0\x9F\x87\xAB"] = ":gf:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB9\xF0\x9F\x87\xAB"] = ":flag_tf:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB9\xF0\x9F\x87\xAB"] = ":tf:";
    lEmoticonsEmoi["\xF0\x9F\x87\xAC\xF0\x9F\x87\xB5"] = ":flag_gp:";
    lEmoticonsEmoi["\xF0\x9F\x87\xAC\xF0\x9F\x87\xB5"] = ":gp:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB2\xF0\x9F\x87\xB6"] = ":flag_mq:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB2\xF0\x9F\x87\xB6"] = ":mq:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB2\xF0\x9F\x87\xB5"] = ":flag_mp:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB2\xF0\x9F\x87\xB5"] = ":mp:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB7\xF0\x9F\x87\xAA"] = ":flag_re:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB7\xF0\x9F\x87\xAA"] = ":re:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB8\xF0\x9F\x87\xBD"] = ":flag_sx:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB8\xF0\x9F\x87\xBD"] = ":sx:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB8\xF0\x9F\x87\xB8"] = ":flag_ss:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB8\xF0\x9F\x87\xB8"] = ":ss:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB9\xF0\x9F\x87\xA8"] = ":flag_tc:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB9\xF0\x9F\x87\xA8"] = ":tc:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB2\xF0\x9F\x87\xAB"] = ":flag_mf:";
    lEmoticonsEmoi["\xF0\x9F\x87\xB2\xF0\x9F\x87\xAB"] = ":mf:";
    lEmoticonsEmoi["\xF0\x9F\x95\xB5\xF0\x9F\x8F\xBB"] = ":spy_tone1:";
    lEmoticonsEmoi["\xF0\x9F\x95\xB5\xF0\x9F\x8F\xBB"] = ":sleuth_or_spy_tone1:";
    lEmoticonsEmoi["\xF0\x9F\x95\xB5\xF0\x9F\x8F\xBC"] = ":spy_tone2:";
    lEmoticonsEmoi["\xF0\x9F\x95\xB5\xF0\x9F\x8F\xBC"] = ":sleuth_or_spy_tone2:";
    lEmoticonsEmoi["\xF0\x9F\x95\xB5\xF0\x9F\x8F\xBD"] = ":spy_tone3:";
    lEmoticonsEmoi["\xF0\x9F\x95\xB5\xF0\x9F\x8F\xBD"] = ":sleuth_or_spy_tone3:";
    lEmoticonsEmoi["\xF0\x9F\x95\xB5\xF0\x9F\x8F\xBE"] = ":spy_tone4:";
    lEmoticonsEmoi["\xF0\x9F\x95\xB5\xF0\x9F\x8F\xBE"] = ":sleuth_or_spy_tone4:";
    lEmoticonsEmoi["\xF0\x9F\x95\xB5\xF0\x9F\x8F\xBF"] = ":spy_tone5:";
    lEmoticonsEmoi["\xF0\x9F\x95\xB5\xF0\x9F\x8F\xBF"] = ":sleuth_or_spy_tone5:";

    // TODO convert utf8 to emoi
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
    // TODO convertUtf8ToEmoi(strData);
    convertEmoticonsEmoi(strData, qWebViewContext);
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
