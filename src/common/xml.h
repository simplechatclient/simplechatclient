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

#ifndef XML_H
#define XML_H

#include <QDomDocument>
#include <QHash>
#include <QObject>

QT_BEGIN_NAMESPACE
class QDomElement;
QT_END_NAMESPACE

class Xml : public QObject
{
    Q_OBJECT
public:
    Xml(const QString &_strFile, const QString &_strRootName, const QHash<QString, QString> &_lDefaultValues);
    QString get(const QString &strKey);
    void set(const QString &strKey, const QString &strValue);
    void remove(const QString &strKey);
    QHash<QString,QString> read();

private:
    QString strFile;
    QString strRootName;
    QHash<QString, QString> lDefaultValues;
    QDomDocument doc;

    void createNewFile();
    void addKeyValue(QDomDocument *doc, QDomElement *root, const QString &strKey, const QString &strValue);
    void save();

};

#endif // XML_H
