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

#ifndef CACHE_H
#define CACHE_H

#include <QObject>

QT_BEGIN_NAMESPACE
class QNetworkAccessManager;
class QNetworkReply;
QT_END_NAMESPACE

class Cache : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(Cache)
    static Cache *Instance;
public:
    static Cache *instance();

    Cache();
    virtual ~Cache();
    QString get(const QString &strChannel, const QString &strUrl);

private:
    QNetworkAccessManager *accessManager;

    QString getCachePath(const QString &strImage);

public slots:
    void httpFinished(QNetworkReply *reply);
};

#endif // CACHE_H
