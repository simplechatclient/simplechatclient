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

#include <QStringList>
#include <QRegularExpression>
#include "models/settings.h"
#include "replace.h"

void Replace::replaceEmots(QString &strData)
{
    if (!Settings::instance()->getBool("replace_emoticons"))
        return;

    QHash<QString, QString> lEmoticons;
    lEmoticons[":*"] = "//cmok";
    lEmoticons[";)"] = "//oczko";
    lEmoticons[":P"] = "//jezor";
    lEmoticons[";P"] = "//xjezyk";
    lEmoticons[":)"] = "//haha";
    lEmoticons[":("] = "//zal";
    lEmoticons[":x"] = "//nie_powiem";
    lEmoticons[":?"] = "//nie_rozumiem";
    lEmoticons[":(("] = "//bardzo_smutny";
    lEmoticons[":|"] = "//xdepresja";
    lEmoticons[":]"] = "//usmieszek";
    lEmoticons[":>"] = "//xluzak";
    lEmoticons[";>"] = "//wazniak";
    lEmoticons[":<"] = "//umm";
    lEmoticons[":$"] = "//skwaszony";
    lEmoticons[";$"] = "//xkwas";
    lEmoticons[";/"] = "//xsceptyk";
    lEmoticons[":/"] = "//sceptyczny";
    lEmoticons[";D"] = "//xhehe";
    lEmoticons[":D"] = "//hehe";
    //lEmoticons["o_O"] = "//swir";
    lEmoticons["!!"] = "//wykrzyknik";
    lEmoticons["??"] = "//pytanie";
    //lEmoticons["xD"] = "//lol";
    //lEmoticons["-_-"] = "//wrr";
    lEmoticons[";("] = "//szloch";

    // scc
    lEmoticons[":))"] = "//haha";
    lEmoticons[";))"] = "//oczko";
    lEmoticons[";(("] = "//szloch";
    lEmoticons[";("] = "//szloch";
    lEmoticons[":p"] = "//jezyk";
    lEmoticons[";p"] = "//jezor";
    lEmoticons[":d"] = "//hehe";
    lEmoticons[";d"] = "//hehe";
    lEmoticons[";x"] = "//nie_powiem";
    lEmoticons[":o"] = "//panda";
    lEmoticons[";o"] = "//panda";
    lEmoticons[";<"] = "//buu";
    lEmoticons[";]"] = "//oczko";
    lEmoticons[":["] = "//zal";
    lEmoticons[";["] = "//szloch";
    lEmoticons[";|"] = "//xdepresja";
    lEmoticons[";*"] = "//cmok2";
    lEmoticons[":s"] = "//skwaszony";
    lEmoticons[";s"] = "//skwaszony";
    lEmoticons["]:->"] = "//xdiabel";
    lEmoticons["];->"] = "//xdiabel";
    lEmoticons[";?"] = "//xco";

    QString strEmoticonsRegex = ";p|;s|:\\]|:P|:\\(\\(|;\\(|:d|\\?\\?|:\\)|:o|;>|;\\)\\)|:\\*|:\\||;D|;P|\\!\\!|;\\[|:\\$|;\\||:>|:x|:\\[|;\\)|:\\(|:\\/|:p|;<|;\\?|;\\$|\\];\\->|;\\*|:s|:D|:\\)\\)|:\\?|\\]:\\->|;\\(\\(|;x|;\\/|;o|;\\]|;d|:<";

    QRegularExpression re("(?<!http)(?<!https)("+strEmoticonsRegex+")(?!\\w)");

    QRegularExpressionMatchIterator i = re.globalMatch(strData);
    while (i.hasNext()) {
        QRegularExpressionMatch match = i.next();
        QString strEmoticon = match.captured(0);

        strData.replace(strEmoticon, lEmoticons[strEmoticon]);
    }
}
