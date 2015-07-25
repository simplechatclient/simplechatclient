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
                strData.replace(strEmoticonFull, QString("<img src=\"%1\" alt=\"%2\" title=\"%2\"%3 />").arg(strEmoticonPath, strEmoticon, strWidthHeight));
            }
            else
                strData.replace(strEmoticonFull, QString("//%1").arg(strEmoticon));
        }
        else
            strData.replace(strEmoticonFull, QString("//%1").arg(strEmoticon));
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
