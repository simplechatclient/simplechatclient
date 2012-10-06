/*
 * Simple Chat Client
 *
 *   Copyright (C) 2012 Piotr Łuczko <piotr.luczko@gmail.com>
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
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include "avatar.h"
#include "channel.h"
#include "nicklist.h"
#include "settings.h"

#ifdef Q_WS_WIN
    #include <QDesktopServices>
#endif


Avatar * Avatar::Instance = 0;

Avatar * Avatar::instance()
{
    if (!Instance)
    {
        Instance = new Avatar();
    }

    return Instance;
}

Avatar::Avatar()
{
    accessManager = new QNetworkAccessManager;
    connect(accessManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(httpFinished(QNetworkReply*)));
}

Avatar::~Avatar()
{
    accessManager->deleteLater();
}

void Avatar::get(const QString &strNickOrChannel, const QString &strCategory, const QString &strUrl)
{
    QNetworkReply *reply = accessManager->get(QNetworkRequest(strUrl));
    reply->setProperty("nickorchannel", strNickOrChannel);
    reply->setProperty("category", strCategory);
}

void Avatar::httpFinished(QNetworkReply *reply)
{
    reply->deleteLater();

    if (reply->error())
        return;

    QByteArray bAvatar = reply->readAll();
    if (!bAvatar.isEmpty())
    {
        QString strNickOrChannel = reply->property("nickorchannel").toString();
        QString strCategory = reply->property("category").toString();
        QFileInfo fi(reply->url().toString());

        QString strAvatarFile = fi.fileName();

        saveAvatar(strAvatarFile, bAvatar);

        if (strCategory == "nick")
        {
            Nicklist::instance()->setUserAvatar(strNickOrChannel, strAvatarFile);
        }
        else if (strCategory == "channel")
        {
            Channel::instance()->setAvatar(strNickOrChannel, strAvatarFile);
        }
    }
}

QString Avatar::getAvatarPath(const QString &strAvatar)
{
    QString strCurrentProfile = Settings::instance()->get("current_profile");
    QString path;
#ifdef Q_WS_WIN
    path = QFileInfo(QDesktopServices::storageLocation(QDesktopServices::DocumentsLocation)).absoluteFilePath();
    path += "/scc/";
#else
    path = QDir::homePath()+"/.scc/";
#endif

    path += "profiles/"+strCurrentProfile+"/avatars/";

    // create dir if not exist
    if (!QDir().exists(path))
        QDir().mkpath(path);

    return path+strAvatar;
}

void Avatar::saveAvatar(const QString &strAvatar, const QByteArray &bAvatar)
{
    QString strAvatarPath = getAvatarPath(strAvatar);

    QFile f(strAvatarPath);
    if (f.open(QIODevice::WriteOnly))
    {
        f.write(bAvatar);
        f.close();
    }
}