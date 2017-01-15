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

#include <QCryptographicHash>
#include <QDir>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QPixmap>
#include "channel.h"
#include "cache.h"

#ifdef Q_OS_WIN
    #include <QCoreApplication>
    #include <QStandardPaths>
#else
    #include "scc-config.h"
#endif

Cache * Cache::Instance = 0;

Cache * Cache::instance()
{
    if (!Instance)
    {
        Instance = new Cache();
    }

    return Instance;
}

Cache::Cache()
{
    accessManager = new QNetworkAccessManager;
    connect(accessManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(httpFinished(QNetworkReply*)));
}

Cache::~Cache()
{
    accessManager->deleteLater();
}

QString Cache::get(const QString &strChannel, const QString &strUrl)
{
    QString strFileName = QString(QCryptographicHash::hash(strUrl.toLatin1(), QCryptographicHash::Md5).toHex())+"."+QFileInfo(strUrl).suffix();
    QString strPathPlusFileName = getCachePath(strFileName);

    bool bDownloadFile = true;
    if (QFile::exists(strPathPlusFileName))
    {
        QList<QString> lSupportedImages;
        lSupportedImages << "jpg" << "jpeg" << "png" << "bmp";

        if (lSupportedImages.contains(QFileInfo(strFileName).suffix().toLower()))
        {
            QPixmap pixmap;
            if (!pixmap.load(strPathPlusFileName))
                bDownloadFile = true;
            else
                bDownloadFile = false;
        }
        else
            bDownloadFile = false;
    }
    else
        bDownloadFile = true;

    if (bDownloadFile)
    {
        QNetworkReply *reply = accessManager->get(QNetworkRequest(strUrl));
        reply->setProperty("file", strPathPlusFileName);
        reply->setProperty("channel", strChannel);
    }

    return strPathPlusFileName;
}

QString Cache::getCachePath(const QString &strFile)
{
    QString path;
#ifdef Q_OS_WIN
    path = QFileInfo(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)).absoluteFilePath();
    path += "/scc/";
#else
    path = QDir::homePath()+"/.scc/";
#endif

    path += "cache/web/";

    // create dir if not exist
    if (!QDir().exists(path))
        QDir().mkpath(path);

    return path+strFile;
}

void Cache::httpFinished(QNetworkReply *reply)
{
    reply->deleteLater();

    if (reply->error())
        return;

    QString strFileName = reply->property("file").toString();
    QString strChannel = reply->property("channel").toString();

    QVariant possibleRedirectUrl = reply->attribute(QNetworkRequest::RedirectionTargetAttribute);
    if (!possibleRedirectUrl.toUrl().isEmpty())
    {
        QNetworkReply *replyRedirect = accessManager->get(QNetworkRequest(possibleRedirectUrl.toUrl()));
        replyRedirect->setProperty("file", strFileName);
        replyRedirect->setProperty("channel", strChannel);
        return;
    }

    QByteArray bFileContent = reply->readAll();
    if (bFileContent.isEmpty())
        return;

    QList<QString> lSupportedImages;
    lSupportedImages << "jpg" << "jpeg" << "png" << "bmp";

    if (lSupportedImages.contains(QFileInfo(strFileName).suffix().toLower()))
    {
        QPixmap pixmap;
        if (pixmap.loadFromData(bFileContent))
        {
            pixmap = pixmap.scaled(QSize(75,75), Qt::KeepAspectRatio, Qt::SmoothTransformation);
            pixmap.save(strFileName);

            // refresh
            if (!strChannel.isEmpty())
                Channel::instance()->getChatView(strChannel)->reloadCacheImage(strFileName);
        }
        else
        {
            QFile f(strFileName);
            if (f.open(QIODevice::WriteOnly))
            {
                f.write(bFileContent);
                f.close();
            }
        }
    }
    else
    {
        QFile f(strFileName);
        if (f.open(QIODevice::WriteOnly))
        {
            f.write(bFileContent);
            f.close();
        }
    }
}
