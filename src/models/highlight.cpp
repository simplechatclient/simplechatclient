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

#include "common/config.h"
#include "highlight.h"

Highlight * Highlight::Instance = 0;

Highlight * Highlight::instance()
{
    if (!Instance)
    {
        Instance = new Highlight();
        Instance->init();
    }

    return Instance;
}

Highlight::Highlight()
{
}

void Highlight::init()
{
    lHighlight.clear();
    read();
}

void Highlight::add(const QString &strHighlight)
{
    QString strHighlightClean = fix(strHighlight);

    if (!lHighlight.contains(strHighlightClean))
    {
        lHighlight.append(strHighlightClean);
        save();
    }
}

void Highlight::remove(const QString &strHighlight)
{
    QString strHighlightClean = fix(strHighlight);

    if (lHighlight.contains(strHighlightClean))
    {
        lHighlight.removeAll(strHighlightClean);
        save();
    }
}

void Highlight::replace(const QString &strOldHighlight, const QString &strNewHighlight)
{
    remove(strOldHighlight);
    add(strNewHighlight);
}

bool Highlight::contains(const QString &strHighlight)
{
    QString strHighlightClean = fix(strHighlight);
    return lHighlight.contains(strHighlightClean);
}

void Highlight::clear()
{
    lHighlight.clear();
}

QStringList Highlight::get()
{
    return lHighlight;
}

void Highlight::read()
{
    QString strHighlight;

    Config *pConfig = new Config();
    strHighlight = pConfig->get("highlight");
    delete pConfig;

    lHighlight = strHighlight.split(";", QString::SkipEmptyParts);
}

void Highlight::save()
{
    QString strHighlight;

    foreach (const QString &strNewHighlight, lHighlight)
        strHighlight.append(strNewHighlight+";");

    Config *pConfig = new Config();
    pConfig->set("highlight", strHighlight);
    delete pConfig;
}

QString Highlight::fix(const QString &strHighlight)
{
    QString strHighlightFixed = strHighlight;
    return strHighlightFixed.remove(QRegExp("[&<>\"'\\;]"));
}
