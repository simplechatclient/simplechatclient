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

#ifndef UPDATE_H
#define UPDATE_H

#include <QObject>

QT_BEGIN_NAMESPACE
class QAction;
class QNetworkAccessManager;
class QNetworkReply;
QT_END_NAMESPACE

class Update : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(Update)
    static Update *Instance;
public:
    static Update *instance();

    Update();
    virtual ~Update();
    void checkUpdate();

    QAction *updateAction;

private:
    QNetworkAccessManager *accessManager;
    QHash<QString, QString> hUpdateResults;

    void updateRequest(const QString &strMethod, const QString &strUrl, const QString &strUrlMarker);
    void checkUpdateGithub();
    void checkUpdateSourceforge();
    int fastParseVersion(QString strXml);
    QString fullParseVersion();
    void saveUpdate(QString strXml);
    QString getPlatform();

private slots:
    void updateFinished(QNetworkReply *reply);
};

#endif // UPDATE_H
