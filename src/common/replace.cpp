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

#include <QStringList>
#include "models/settings.h"
#include "replace.h"

void Replace::replaceEmots(QString &strData)
{
    if (!Settings::instance()->getBool("replace_emoticons"))
        return;

    QHash<QString, QString> lEmoticons;
    lEmoticons[":*"] = "%Icmok%";
    lEmoticons[";)"] = "%Ioczko%";
    lEmoticons[":P"] = "%Ijezor%";
    lEmoticons[";P"] = "%Ixjezyk%";
    lEmoticons[":)"] = "%Ihaha%";
    lEmoticons[":("] = "%Izal%";
    lEmoticons[":x"] = "%Inie_powiem%";
    lEmoticons[":?"] = "%Inie_rozumiem%";
    lEmoticons[":(("] = "%Ibardzo_smutny%";
    lEmoticons[":|"] = "%Ixdepresja%";
    lEmoticons[":]"] = "%Iusmieszek%";
    lEmoticons[":>"] = "%Ixluzak%";
    lEmoticons[";>"] = "%Iwazniak%";
    lEmoticons[":<"] = "%Iumm%";
    lEmoticons[":$"] = "%Iskwaszony%";
    lEmoticons[";$"] = "%Ixkwas%";
    lEmoticons[";/"] = "%Ixsceptyk%";
    //lEmoticons[":/"] = "%Isceptyczny%";
    lEmoticons[";D"] = "%Ixhehe%";
    lEmoticons[":D"] = "%Ihehe%";
    //lEmoticons["o_O"] = "%Iswir%";
    lEmoticons["!!"] = "%Iwykrzyknik%";
    lEmoticons["??"] = "%Ipytanie%";
    //lEmoticons["xD"] = "%Ilol%";
    //lEmoticons["-_-"] = "%Iwrr%";
    lEmoticons[";("] = "%Iszloch%";

    // scc
    lEmoticons[":))"] = "%Ihaha%";
    lEmoticons[";))"] = "%Ioczko%";
    lEmoticons[";(("] = "%Iszloch%";
    lEmoticons[";("] = "%Iszloch%";
    lEmoticons[":p"] = "%Ijezyk%";
    lEmoticons[";p"] = "%Ijezor%";
    lEmoticons[":d"] = "%Ihehe%";
    lEmoticons[";d"] = "%Ihehe%";
    lEmoticons[";x"] = "%Inie_powiem%";
    lEmoticons[":o"] = "%Ipanda%";
    lEmoticons[";o"] = "%Ipanda%";
    lEmoticons[";<"] = "%Ibuu%";
    lEmoticons[";]"] = "%Ioczko%";
    lEmoticons[":["] = "%Izal%";
    lEmoticons[";["] = "%Iszloch%";
    lEmoticons[";|"] = "%Ixdepresja%";
    lEmoticons[";*"] = "%Icmok2%";
    lEmoticons[":s"] = "%Iskwaszony%";
    lEmoticons[";s"] = "%Iskwaszony%";
    lEmoticons["]:->"] = "%Ixdiabel%";
    lEmoticons["];->"] = "%Ixdiabel%";
    lEmoticons[";?"] = "%Ixco%";

    QHashIterator<QString, QString> it(lEmoticons);
    while (it.hasNext())
    {
        it.next();

        QString strKey = it.key();
        QString strValue = it.value();

        strData.replace(strKey, strValue);
    }

    // exception
    // :/

    // TODO fix in Qt 5.x
    // using http://qt-project.org/doc/qt-5.0/qtcore/qregularexpression.html
    if (!strData.contains(QRegExp("(http:|https:)//"))) strData.replace(":/", "%Isceptyczny%");
}
