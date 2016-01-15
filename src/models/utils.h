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

#ifndef UTILS_H
#define UTILS_H

#include <QObject>

class Utils : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(Utils)
    static Utils *Instance;
public:
    static Utils *instance();

    Utils();

    bool isErotic(const QString &strChannel);

    QString convertCodeToCountry(const QString &strCountryCode);
    QString convertCountryToCode(const QString &strCountry);
    QString convertIntToMonth(const QString &strConvertMonth);
    QString convertMonthToInt(const QString &strMonth);
    QString convertIntToType(int iType);
    QString convertIntToSexChar(int iSex);
    QString convertDateToAge(const QString &strDate);
    QString convertCharToSex(const QString &strSex);
    int convertSexToInt(const QString &strSex);
    QString convertChannelTypeToString(int type);
    QString convertChannelCatToString(int cat);
    QString convertChannelCatToString(int cat, bool moderated, bool recommended);
    int timeToPercentage(const QString &strValue);

    QList<QString> getFonts();
    QList<QString> getColors();
    QList<QString> getSex();
    QList<QString> getDays();
    QList<QString> getMonths();
    QList<QString> getYears();
    QString getCountries();
};

#endif // UTILS_H
