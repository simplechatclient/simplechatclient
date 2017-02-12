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

#include <QDir>
#include <QPixmap>
#include <QMap>
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
            if (QFile::exists(strEmoticonCheck)) {
                return strEmoticonCheck;
            }
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

    QStringList lSupportedEmoticons;
    lSupportedEmoticons << ".gif" << ".jpg" << ".jpeg" << ".png" << ".bmp";

    foreach (const QString &strDir, lDirs)
    {
        foreach (const QString &strSupportedEmoticon, lSupportedEmoticons)
        {
            QString strEmoticonCheck = QString("%1/emoticons_emoji/%2/%3%4").arg(path, strDir, strEmoticon, strSupportedEmoticon);
            if (QFile::exists(strEmoticonCheck)) {
                return strEmoticonCheck;
            }
        }
    }
    return QString::null;
}

void convertOnetColorToHtml(QString &strData)
{
    QList<QString> lColors;
    lColors << "000000" << "623c00" << "c86c00" << "ff6500" << "ff0000" << "e40f0f" << "990033" << "8800ab" << "ce00ff" << "0f2ab1" << "3030ce" << "006699" << "1a866e" << "008100" << "959595";

    if (Settings::instance()->getBool("font_formatting"))
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
        foreach (const QString &strColor, lColors) {
            strData.remove("%C" + strColor + "%");
        }
    }
}

void convertIrcColorToHtml(QString &strData)
{
    /*
    var codes = {
  reset: [0, 0],

  bold: [1, 22],
  dim: [2, 22],
  italic: [3, 23],
  underline: [4, 24],
  inverse: [7, 27],
  hidden: [8, 28],
  strikethrough: [9, 29],

  black: [30, 39],
  red: [31, 39],
  green: [32, 39],
  yellow: [33, 39],
  blue: [34, 39],
  magenta: [35, 39],
  cyan: [36, 39],
  white: [37, 39],
  gray: [90, 39],
  grey: [90, 39],

  bgBlack: [40, 49],
  bgRed: [41, 49],
  bgGreen: [42, 49],
  bgYellow: [43, 49],
  bgBlue: [44, 49],
  bgMagenta: [45, 49],
  bgCyan: [46, 49],
  bgWhite: [47, 49],

  // legacy styles for colors pre v1.0.0
  blackBG: [40, 49],
  redBG: [41, 49],
  greenBG: [42, 49],
  yellowBG: [43, 49],
  blueBG: [44, 49],
  magentaBG: [45, 49],
  cyanBG: [46, 49],
  whiteBG: [47, 49]

};

Object.keys(codes).forEach(function (key) {
  var val = codes[key];
  var style = styles[key] = [];
  style.open = '\u001b[' + val[0] + 'm';
  style.close = '\u001b[' + val[1] + 'm';
});
     */

    // \x033[38;5;
    // 256-color mode — foreground: \0x1B[38;5;m   background: \0x1B[48;5;m

    /*
         # Primary 3-bit (8 colors). Unique representation!
    ('00',  '000000'),
    ('01',  '800000'),
    ('02',  '008000'),
    ('03',  '808000'),
    ('04',  '000080'),
    ('05',  '800080'),
    ('06',  '008080'),
    ('07',  'c0c0c0'),

    # Equivalent "bright" versions of original 8 colors.
    ('08',  '808080'),
    ('09',  'ff0000'),
    ('10',  '00ff00'),
    ('11',  'ffff00'),
    ('12',  '0000ff'),
    ('13',  'ff00ff'),
    ('14',  '00ffff'),
    ('15',  'ffffff'),

    # Strictly ascending.
    ('16',  '000000'),
    ('17',  '00005f'),
    ('18',  '000087'),
    ('19',  '0000af'),
    ('20',  '0000d7'),
    ('21',  '0000ff'),
    ('22',  '005f00'),
    ('23',  '005f5f'),
    ('24',  '005f87'),
    ('25',  '005faf'),
    ('26',  '005fd7'),
    ('27',  '005fff'),
    ('28',  '008700'),
    ('29',  '00875f'),
    ('30',  '008787'),
    ('31',  '0087af'),
    ('32',  '0087d7'),
    ('33',  '0087ff'),
    ('34',  '00af00'),
    ('35',  '00af5f'),
    ('36',  '00af87'),
    ('37',  '00afaf'),
    ('38',  '00afd7'),
    ('39',  '00afff'),
    ('40',  '00d700'),
    ('41',  '00d75f'),
    ('42',  '00d787'),
    ('43',  '00d7af'),
    ('44',  '00d7d7'),
    ('45',  '00d7ff'),
    ('46',  '00ff00'),
    ('47',  '00ff5f'),
    ('48',  '00ff87'),
    ('49',  '00ffaf'),
    ('50',  '00ffd7'),
    ('51',  '00ffff'),
    ('52',  '5f0000'),
    ('53',  '5f005f'),
    ('54',  '5f0087'),
    ('55',  '5f00af'),
    ('56',  '5f00d7'),
    ('57',  '5f00ff'),
    ('58',  '5f5f00'),
    ('59',  '5f5f5f'),
    ('60',  '5f5f87'),
    ('61',  '5f5faf'),
    ('62',  '5f5fd7'),
    ('63',  '5f5fff'),
    ('64',  '5f8700'),
    ('65',  '5f875f'),
    ('66',  '5f8787'),
    ('67',  '5f87af'),
    ('68',  '5f87d7'),
    ('69',  '5f87ff'),
    ('70',  '5faf00'),
    ('71',  '5faf5f'),
    ('72',  '5faf87'),
    ('73',  '5fafaf'),
    ('74',  '5fafd7'),
    ('75',  '5fafff'),
    ('76',  '5fd700'),
    ('77',  '5fd75f'),
    ('78',  '5fd787'),
    ('79',  '5fd7af'),
    ('80',  '5fd7d7'),
    ('81',  '5fd7ff'),
    ('82',  '5fff00'),
    ('83',  '5fff5f'),
    ('84',  '5fff87'),
    ('85',  '5fffaf'),
    ('86',  '5fffd7'),
    ('87',  '5fffff'),
    ('88',  '870000'),
    ('89',  '87005f'),
    ('90',  '870087'),
    ('91',  '8700af'),
    ('92',  '8700d7'),
    ('93',  '8700ff'),
    ('94',  '875f00'),
    ('95',  '875f5f'),
    ('96',  '875f87'),
    ('97',  '875faf'),
    ('98',  '875fd7'),
    ('99',  '875fff'),
    ('100', '878700'),
    ('101', '87875f'),
    ('102', '878787'),
    ('103', '8787af'),
    ('104', '8787d7'),
    ('105', '8787ff'),
    ('106', '87af00'),
    ('107', '87af5f'),
    ('108', '87af87'),
    ('109', '87afaf'),
    ('110', '87afd7'),
    ('111', '87afff'),
    ('112', '87d700'),
    ('113', '87d75f'),
    ('114', '87d787'),
    ('115', '87d7af'),
    ('116', '87d7d7'),
    ('117', '87d7ff'),
    ('118', '87ff00'),
    ('119', '87ff5f'),
    ('120', '87ff87'),
    ('121', '87ffaf'),
    ('122', '87ffd7'),
    ('123', '87ffff'),
    ('124', 'af0000'),
    ('125', 'af005f'),
    ('126', 'af0087'),
    ('127', 'af00af'),
    ('128', 'af00d7'),
    ('129', 'af00ff'),
    ('130', 'af5f00'),
    ('131', 'af5f5f'),
    ('132', 'af5f87'),
    ('133', 'af5faf'),
    ('134', 'af5fd7'),
    ('135', 'af5fff'),
    ('136', 'af8700'),
    ('137', 'af875f'),
    ('138', 'af8787'),
    ('139', 'af87af'),
    ('140', 'af87d7'),
    ('141', 'af87ff'),
    ('142', 'afaf00'),
    ('143', 'afaf5f'),
    ('144', 'afaf87'),
    ('145', 'afafaf'),
    ('146', 'afafd7'),
    ('147', 'afafff'),
    ('148', 'afd700'),
    ('149', 'afd75f'),
    ('150', 'afd787'),
    ('151', 'afd7af'),
    ('152', 'afd7d7'),
    ('153', 'afd7ff'),
    ('154', 'afff00'),
    ('155', 'afff5f'),
    ('156', 'afff87'),
    ('157', 'afffaf'),
    ('158', 'afffd7'),
    ('159', 'afffff'),
    ('160', 'd70000'),
    ('161', 'd7005f'),
    ('162', 'd70087'),
    ('163', 'd700af'),
    ('164', 'd700d7'),
    ('165', 'd700ff'),
    ('166', 'd75f00'),
    ('167', 'd75f5f'),
    ('168', 'd75f87'),
    ('169', 'd75faf'),
    ('170', 'd75fd7'),
    ('171', 'd75fff'),
    ('172', 'd78700'),
    ('173', 'd7875f'),
    ('174', 'd78787'),
    ('175', 'd787af'),
    ('176', 'd787d7'),
    ('177', 'd787ff'),
    ('178', 'd7af00'),
    ('179', 'd7af5f'),
    ('180', 'd7af87'),
    ('181', 'd7afaf'),
    ('182', 'd7afd7'),
    ('183', 'd7afff'),
    ('184', 'd7d700'),
    ('185', 'd7d75f'),
    ('186', 'd7d787'),
    ('187', 'd7d7af'),
    ('188', 'd7d7d7'),
    ('189', 'd7d7ff'),
    ('190', 'd7ff00'),
    ('191', 'd7ff5f'),
    ('192', 'd7ff87'),
    ('193', 'd7ffaf'),
    ('194', 'd7ffd7'),
    ('195', 'd7ffff'),
    ('196', 'ff0000'),
    ('197', 'ff005f'),
    ('198', 'ff0087'),
    ('199', 'ff00af'),
    ('200', 'ff00d7'),
    ('201', 'ff00ff'),
    ('202', 'ff5f00'),
    ('203', 'ff5f5f'),
    ('204', 'ff5f87'),
    ('205', 'ff5faf'),
    ('206', 'ff5fd7'),
    ('207', 'ff5fff'),
    ('208', 'ff8700'),
    ('209', 'ff875f'),
    ('210', 'ff8787'),
    ('211', 'ff87af'),
    ('212', 'ff87d7'),
    ('213', 'ff87ff'),
    ('214', 'ffaf00'),
    ('215', 'ffaf5f'),
    ('216', 'ffaf87'),
    ('217', 'ffafaf'),
    ('218', 'ffafd7'),
    ('219', 'ffafff'),
    ('220', 'ffd700'),
    ('221', 'ffd75f'),
    ('222', 'ffd787'),
    ('223', 'ffd7af'),
    ('224', 'ffd7d7'),
    ('225', 'ffd7ff'),
    ('226', 'ffff00'),
    ('227', 'ffff5f'),
    ('228', 'ffff87'),
    ('229', 'ffffaf'),
    ('230', 'ffffd7'),
    ('231', 'ffffff'),

    # Gray-scale range.
    ('232', '080808'),
    ('233', '121212'),
    ('234', '1c1c1c'),
    ('235', '262626'),
    ('236', '303030'),
    ('237', '3a3a3a'),
    ('238', '444444'),
    ('239', '4e4e4e'),
    ('240', '585858'),
    ('241', '626262'),
    ('242', '6c6c6c'),
    ('243', '767676'),
    ('244', '808080'),
    ('245', '8a8a8a'),
    ('246', '949494'),
    ('247', '9e9e9e'),
    ('248', 'a8a8a8'),
    ('249', 'b2b2b2'),
    ('250', 'bcbcbc'),
    ('251', 'c6c6c6'),
    ('252', 'd0d0d0'),
    ('253', 'dadada'),
    ('254', 'e4e4e4'),
    ('255', 'eeeeee'),
]
     */
}

void convertMircColorToHtml(QString &strData)
{
    QMap<QString, QColor> lMircColors;
    lMircColors["00"] = QColor(255, 255, 255);
    lMircColors["01"] = QColor(0, 0, 0);
    lMircColors["02"] = QColor(0, 0, 127);
    lMircColors["03"] = QColor(0, 147, 0);
    lMircColors["04"] = QColor(255, 0, 0);
    lMircColors["05"] = QColor(127, 0, 0);
    lMircColors["06"] = QColor(156, 0, 156);
    lMircColors["07"] = QColor(252, 127, 0);
    lMircColors["08"] = QColor(255, 255, 0);
    lMircColors["09"] = QColor(0, 252, 0);
    lMircColors["10"] = QColor(0, 147, 147);
    lMircColors["11"] = QColor(0, 255, 255);
    lMircColors["12"] = QColor(0, 0, 252);
    lMircColors["13"] = QColor(255, 0, 255);
    lMircColors["14"] = QColor(127, 127, 127);
    lMircColors["15"] = QColor(210, 210, 210);

    if (Settings::instance()->getBool("font_formatting"))
    {
        QMapIterator<QString, QColor> iteratorForeground (lMircColors);
        while (iteratorForeground.hasNext()) {
            iteratorForeground.next();

            QString foregroundKey = iteratorForeground.key();
            QColor foregroundColor = iteratorForeground.value();

            QMapIterator<QString, QColor> iteratorBackground (lMircColors);
            while (iteratorBackground.hasNext()) {
                iteratorBackground.next();

                QString backgroundKey = iteratorBackground.key();
                QColor backgroundColor = iteratorBackground.value();

                QRegExp rx(QString(QByteArray("\x03")) + foregroundKey + ","+ backgroundKey);
                int pos = 0;
                while ((pos = rx.indexIn(strData, pos)) != -1) {
                    int first = pos;
                    int second = first + rx.matchedLength();

                    strData.replace(first, second - first, QString("<span style=\"color:%1;background-color:%2;\">").arg(foregroundColor.name(QColor::HexRgb), backgroundColor.name(QColor::HexRgb)));
                    strData += "</span>";
                }
            }

            QRegExp rx(QString(QByteArray("\x03")) + foregroundKey + "");
            int pos = 0;
            while ((pos = rx.indexIn(strData, pos)) != -1) {
                int first = pos;
                int second = first + rx.matchedLength();

                strData.replace(first, second - first, QString("<span style=\"color:%1;\">").arg(foregroundColor.name(QColor::HexRgb)));
                strData += "</span>";
            }
        }
    }
    else
    {
        foreach (const QString &strForegroundColor, lMircColors.keys()) {
            foreach (const QString &strBackgroundColor, lMircColors.keys()) {
                strData.remove(QString(QByteArray("\x03")) + strForegroundColor + ","+strBackgroundColor);
            }
            strData.remove(QString(QByteArray("\x03")) + strForegroundColor + "");
        }
    }
}

void convertOnetFontToHtml(QString &strData)
{
    QRegExp rx("%F(b|i|bi)?:?(arial|times|verdana|tahoma|courier)?%");

    int pos = 0;
    while ((pos = rx.indexIn(strData, pos)) != -1)
    {
        int first = pos;
        int second = first + rx.matchedLength();

        if (Settings::instance()->getBool("font_formatting"))
        {
            QString strAttributes;
            QString strFontStyle = rx.cap(1);
            QString strFontName = rx.cap(2);

            if (strFontStyle.contains("b")) {
                strAttributes += "font-weight:bold;";
            } else {
                strAttributes += "font-weight:normal;";
            }
            if (strFontStyle.contains("i")) {
                strAttributes += "font-style:italic;";
            } else {
                strAttributes += "font-style:normal;";
            }

            if (strFontName == "arial") {
                strAttributes += "font-family:Arial;";
            } else if (strFontName == "times") {
                strAttributes += "font-family:Times New Roman;";
            } else if (strFontName == "verdana") {
                strAttributes += "font-family:Verdana;";
            } else if (strFontName == "tahoma") {
                strAttributes += "font-family:Tahoma;";
            } else if (strFontName == "courier") {
                strAttributes += "font-family:Courier New;";
            } else {
                strAttributes += "font-family:Verdana;";
            }

            strData.replace(first, second-first, QString("<span style=\"%1\">").arg(strAttributes));
            strData += "</span>";
        } else {
            strData.remove(first, second - first);
        }
    }
}

void convertIrcFontToHtml(QString &strData)
{
    // TODO
}

void convertMircFontToHtml(QString &strData)
{
    QMap<QString, QString> lMircFonts;
    // bold
    lMircFonts[QString(QByteArray("\x02"))] = "font-weight: bold;";
    // italic
    lMircFonts[QString(QByteArray("\x09"))] = "font-style: italic;";
    // strike through
    lMircFonts[QString(QByteArray("\x13"))] = "text-decoration: line-through;";
    // reset
    lMircFonts[QString(QByteArray("\x0f"))] = "text-decoration: none; font-style: normal; font-weight: normal; color: inherit; background-color: inherit;";
    //  underline
    lMircFonts[QString(QByteArray("\x15"))] = "text-decoration: underline;";
    //  underline
    lMircFonts[QString(QByteArray("\x1f"))] = "text-decoration: underline;";
    //  reverse foreground color and background color
    // lMircFonts[QString(QByteArray("\x16"))] = "";

    bool bold = false;
    bool italic = false;
    bool strike = false;
    bool underline = false;
//
//    QStringList list = strData.split('');
//    QStringListIterator iterator(list);
//    while (iterator.hasNext())
//    {
//        QString s = iterator.next();
//
//        // trzeba pokolei bo gdy widzi drugiego bolda to go wylacza - dlatego w kodzie bylo togole
//
//    }
//
//    QMapIterator<QString, QString> iterator (lMircFonts);
//    while (iterator.hasNext()) {
//        iterator.next();
//
//        QString key = iterator.key();
//        QString style = iterator.value();
//
//        QRegExp rx(key);
//        int pos = 0;
//        while ((pos = rx.indexIn(strData, pos)) != -1) {
//            int first = pos;
//            int second = first + rx.matchedLength();
//
//            strData.replace(first, second - first, QString("<span style=\"%1\">").arg(style));
//            strData += "</span>";
//        }
//    }
}

void convertOnetEmoticons(QString &strData, bool bInsertWidthHeight, bool qWebViewContext)
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
                    strWidthHeight = "width=\""+QString::number(p.width())+"\" height=\""+QString::number(p.height())+"\"";
                }
#ifdef Q_OS_WIN
                strEmoticonPath = "/"+strEmoticonPath;
#endif
                if (qWebViewContext)
                    strEmoticonPath = "file://"+strEmoticonPath;

                strData.replace(strEmoticonFull, QString("<img src=\"%1\" alt=\"//%2\" title=\"//%2\" %3 />").arg(strEmoticonPath, strEmoticon, strWidthHeight));
            }
            else
                strData.replace(strEmoticonFull, QString("//%1").arg(strEmoticon));
        }
        else
            strData.replace(strEmoticonFull, QString("//%1").arg(strEmoticon));
    }
}

void convertEmoticonsEmoji(QString &strData, bool bInsertWidthHeight, bool qWebViewContext)
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
                QString strWidthHeight;
                if (bInsertWidthHeight)
                {
                    QPixmap p(strEmoticonPath);
                    strWidthHeight = "width=\""+QString::number(p.width())+"\" height=\""+QString::number(p.height())+"\"";
                }
#ifdef Q_OS_WIN
                strEmoticonPath = "/"+strEmoticonPath;
#endif
                if (qWebViewContext)
                    strEmoticonPath = "file://"+strEmoticonPath;

                strData.replace(strEmoticonFull, QString("<img src=\"%1\" alt=\"&#58;%2&#58;\" title=\"&#58;%2&#58;\" %3 />").arg(strEmoticonPath, strEmoticon, strWidthHeight));
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
void convertSlashToOnetEmoticons(QString &strData)
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

void removeOnetColor(QString &strData)
{
    QList<QString> lColors = Utils::instance()->getColors();
    foreach (const QString &strColor, lColors)
        strData.remove("%C"+strColor+"%");
}

void removeOnetFont(QString &strData)
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

void removeOnetEmoticons(QString &strData)
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
    //strData.replace("&", "&amp;");
    strData.replace("<", "&lt;");
    strData.replace(">", "&gt;");
    strData.replace("\"", "&quot;");
    strData.replace("'", "&#039;");
    strData.replace("\\", "&#92;");
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
    //convertSlashToEmoticons(strData);
}

void Convert::simpleConvert(QString &strData)
{
//    removeColor(strData);
//    removeFont(strData);
    strData.replace(QRegExp("%I([a-zA-Z0-9_-]+)%"), "//\\1");
    //convertEmoticonsToSlash(strData);
}

void Convert::removeStyles(QString &strData)
{
//    removeColor(strData);
//    removeFont(strData);
//    removeEmoticons(strData);
}

void Convert::convertText(QString &strData, bool bInsertWidthHeight, bool qWebViewContext)
{
    // colors
    convertIrcColorToHtml(strData);
    convertMircColorToHtml(strData);
    convertOnetColorToHtml(strData);
    // fonts
    convertIrcFontToHtml(strData);
    convertMircFontToHtml(strData);
    convertOnetFontToHtml(strData);
    // emoticons
    convertOnetEmoticons(strData, bInsertWidthHeight, qWebViewContext);
    convertEmoticonsEmoji(strData, bInsertWidthHeight, qWebViewContext);
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

QHash<QChar, QString> createPrefixHash()
{
    QHash<QChar, QString> m;
    m['~'] = "owner";
    m['&'] = "admin";
    m['@'] = "op";
    m['%'] = "halfop";
    m['+'] = "voice";
    return m;
}

void Convert::convertPrefix(QString &strData)
{
    if (strData.isEmpty()) return;

    static const QHash<QChar, QString> m = createPrefixHash();

    QString html = "";

    if (strData.at(0) == '#')
    {
        for (int i = 0; i < strData.size(); ++i)
        {
            QChar prefix = strData.at(1).toLatin1();
            QString icon = m[prefix];
            if (icon.isNull()) {
                break;
            }

            strData.remove(1, 1);
            html.append(QString("<img src=\"qrc:/images/%1.png\" alt=\"%1\" />").arg(icon));
        }
    }

    strData = html+strData;
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
