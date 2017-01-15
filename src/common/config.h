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

#ifndef CONFIG_H
#define CONFIG_H

#include <QHash>
#include <QObject>
class Xml;

QT_BEGIN_NAMESPACE
class QDomElement;
QT_END_NAMESPACE

enum ConfigCategory {ProfileConfig, SettingsConfig};

class Config : public QObject
{
    Q_OBJECT
public:
    Config(ConfigCategory _eConfigCategory = ProfileConfig, QString _strProfileName = QString::null);
    virtual ~Config();

    QString get(const QString &strKey);
    void set(const QString &strKey, const QString &strValue);
    void setDefault(const QString &strKey);
    void remove(const QString &strKey);
    QHash<QString,QString> read();

private:
    ConfigCategory eConfigCategory;
    QString strProfile;
    QString strConfigFile;
    QHash<QString,QString> lDefaultValues;
    Xml *xml;

    void fix();
    QHash<QString,QString> getDefaultValues();
};

#endif // CONFIG_H
