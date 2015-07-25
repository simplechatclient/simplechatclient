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

#include <QDir>
#include <QDebug>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include "avatar.h"
#include "channel.h"
#include "channel_favourites.h"
#include "channel_homes.h"
#include "nick.h"
#include "settings.h"

#ifdef Q_OS_WIN
    #include <QCoreApplication>
    #include <QStandardPaths>
#else
    #include "scc-config.h"
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
    QFileInfo fi(strUrl);
    QString strFile = fi.fileName();
    QString strPath = getAvatarPath(strFile);

    bool bDownload = true;
    if (QFile::exists(strPath))
    {
        QPixmap pixmap;
        if (!pixmap.load(strPath))
            bDownload = true;
        else
            bDownload = false;
    }
    else
        bDownload = true;

    if (bDownload)
    {
        QNetworkReply *reply = accessManager->get(QNetworkRequest(strUrl));
        reply->setProperty("nickorchannel", strNickOrChannel);
        reply->setProperty("category", strCategory);
        reply->setProperty("file", strFile);
        reply->setProperty("path", strPath);
    }
    else
    {
        updateAvatar(strCategory, strNickOrChannel, strFile);
    }
}

void Avatar::httpFinished(QNetworkReply *reply)
{
    reply->deleteLater();

    if (reply->error())
        return;

    QString strNickOrChannel = reply->property("nickorchannel").toString();
    QString strCategory = reply->property("category").toString();
    QString strAvatarFile = reply->property("file").toString();
    QString strAvatarPath = reply->property("path").toString();

    QVariant possibleRedirectUrl = reply->attribute(QNetworkRequest::RedirectionTargetAttribute);
    if (!possibleRedirectUrl.toUrl().isEmpty())
    {
        QNetworkReply *replyRedirect = accessManager->get(QNetworkRequest(possibleRedirectUrl.toUrl()));
        replyRedirect->setProperty("nickorchannel", strNickOrChannel);
        replyRedirect->setProperty("category", strCategory);
        replyRedirect->setProperty("file", strAvatarFile);
        replyRedirect->setProperty("path", strAvatarPath);
        return;
    }

    QByteArray bAvatar = reply->readAll();
    if (bAvatar.isEmpty())
        return;

    saveAvatar(strAvatarPath, bAvatar);
    updateAvatar(strCategory, strNickOrChannel, strAvatarFile);
}

void Avatar::updateAvatar(const QString &strCategory, const QString &strNickOrChannel, const QString &strAvatarFile)
{
    if (strCategory == "nick")
    {
        Nick::instance()->setAvatar(strNickOrChannel, strAvatarFile);
        Channel::instance()->setPrivAvatar(strNickOrChannel, strAvatarFile);
    }
    else if (strCategory == "channel")
    {
        Channel::instance()->setAvatar(strNickOrChannel, strAvatarFile);
        ChannelHomes::instance()->setAvatar(strNickOrChannel, strAvatarFile);
        ChannelFavourites::instance()->setAvatar(strNickOrChannel, strAvatarFile);
    }
}

void Avatar::saveAvatar(const QString &strAvatarPath, const QByteArray &bAvatar)
{
    QFile f(strAvatarPath);
    if (f.open(QIODevice::WriteOnly))
    {
        f.write(bAvatar);
        f.close();
    }
    else
    {
        if (Settings::instance()->getBool("debug"))
            qWarning() << tr("Error: Cannot save avatar file %1").arg(strAvatarPath);
    }
}

QString Avatar::getAvatarPath(const QString &strAvatar)
{
    QString path;
#ifdef Q_OS_WIN
    path = QFileInfo(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)).absoluteFilePath();
    path += "/scc/";
#else
    path = QDir::homePath()+"/.scc/";
#endif

    path += "cache/avatars/";

    // create dir if not exist
    if (!QDir().exists(path))
        QDir().mkpath(path);

    return path+strAvatar;
}

QString Avatar::getEmptyRegisteredUserAvatar()
{
    QString path;
#ifdef Q_OS_WIN
    path = QCoreApplication::applicationDirPath();
#else
    path = SCC_DATA_DIR;
#endif

    return path+"/images/registered_user_avatar.png";
}

QString Avatar::getEmptyRegisteredUserAvatarBig()
{
    QString path;
#ifdef Q_OS_WIN
    path = QCoreApplication::applicationDirPath();
#else
    path = SCC_DATA_DIR;
#endif

    return path+"/images/registered_user_avatar_big.png";
}

QString Avatar::getEmptyUnregisteredUserAvatar()
{
    QString path;
#ifdef Q_OS_WIN
    path = QCoreApplication::applicationDirPath();
#else
    path = SCC_DATA_DIR;
#endif

    return path+"/images/unregistered_user_avatar.png";
}

QString Avatar::getEmptyUnregisteredUserAvatarBig()
{
    QString path;
#ifdef Q_OS_WIN
    path = QCoreApplication::applicationDirPath();
#else
    path = SCC_DATA_DIR;
#endif

    return path+"/images/unregistered_user_avatar_big.png";
}
