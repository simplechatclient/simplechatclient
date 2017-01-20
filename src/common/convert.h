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

#ifndef CONVERT_H
#define CONVERT_H

/**
 * Convert text
 */

#include <QString>

namespace Convert
{
    void fixMeAction(QString &strData);
    void fixHtmlChars(QString &strData);
    void createText(QString &strData);
    void simpleReverseConvert(QString &strData);
    void simpleConvert(QString &strData);
    void removeStyles(QString &strData);
    void convertText(QString &strData, bool bInsertWidthHeight = false, bool qWebViewContext = false);
    void convertPrefix(QString &strData);
    void fixTopicUrl(QString &strData);

    bool isBold(const QString &strData);
    bool isItalic(const QString &strData);
    QString getFont(const QString &strData);
    int getColor(const QString &strData);
}

#endif // CONVERT_H
