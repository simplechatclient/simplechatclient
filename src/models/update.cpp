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

#include <QAction>
#include <QDomDocument>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QStringList>
#include "core/core.h"
#include "gui/update_gui.h"
#include "message.h"
#include "notification.h"
#include "settings.h"
#include "update.h"

#ifdef Q_OS_WIN
    #include <QSysInfo>
#endif

#define UPDATE_URL_GITHUB "http://simplechatclient.github.io/update.xml"
#define UPDATE_URL_SOURCEFORGE "http://simplechatclien.sourceforge.net/update.xml"

Update * Update::Instance = 0;

Update * Update::instance()
{
    if (!Instance)
    {
        Instance = new Update();
    }

    return Instance;
}

Update::Update()
{
    updateAction = new QAction(QIcon(":/images/breeze/download.svg") , tr("Updates"), this);
    updateAction->setVisible(false);

    accessManager = new QNetworkAccessManager;
    connect(accessManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(updateFinished(QNetworkReply*)));
}

Update::~Update()
{
    accessManager->deleteLater();
}

void Update::checkUpdate()
{
    checkUpdateGithub();
}

void Update::updateRequest(const QString &strMethod, const QString &strUrl, const QString &strUrlMarker)
{
    QString strAgentPlatform = this->getPlatform();
    QString strAgentUrl = "http://simplechatclient.github.io";
    QString strAgentProgram = "SimpleChatClient";
    QString strAgentVersion = Settings::instance()->get("version");
    QString strUserAgent = QString("Mozilla/5.0 (%1; +%2) %3/%4").arg(strAgentPlatform, strAgentUrl, strAgentProgram, strAgentVersion);

    QNetworkRequest request;
    request.setUrl(QUrl(strUrl));
    request.setHeader(QNetworkRequest::UserAgentHeader, strUserAgent.toLatin1());

    QString strUUID = Settings::instance()->get("unique_id");
    QRegExp rUUID("^[a-f0-9]{8}-([a-f0-9]{4}-){3}[a-f0-9]{12}$");
    if ((strMethod == "POST") && (!strUUID.isEmpty()) && (rUUID.exactMatch(strUUID)))
    {
        QString strContent = QString("{\"uuid\":\"%1\"}").arg(strUUID);

        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
        QNetworkReply *pReply = accessManager->post(request, strContent.toLatin1());
        pReply->setProperty("update_url", strUrlMarker);
    }
    else
    {
        QNetworkReply *pReply = accessManager->get(request);
        pReply->setProperty("update_url", strUrlMarker);
    }
}

void Update::checkUpdateGithub()
{
    updateRequest("GET", UPDATE_URL_GITHUB, UPDATE_URL_GITHUB);
}

void Update::checkUpdateSourceforge()
{
    updateRequest("POST", UPDATE_URL_SOURCEFORGE, UPDATE_URL_SOURCEFORGE);
}

int Update::fastParseVersion(QString strVersionXml)
{
    if (strVersionXml.isEmpty()) return 0;

    QDomDocument doc;
    doc.setContent(strVersionXml);

    if (doc.isNull()) return 0;

    QString strVersion = doc.elementsByTagName("version").item(0).toElement().text();

    if (strVersion.isEmpty()) return 0;

    QStringList lVersion = strVersion.split(".");
    int iMajor = lVersion.at(0).toInt();
    int iMinor = lVersion.at(1).toInt();
    int iPatch = lVersion.at(2).toInt();
    return (QString("%1%2%3").arg(iMajor).arg(iMinor).arg(iPatch)).toInt();
}

QString Update::fullParseVersion()
{
    QString strCurrentVersion = Settings::instance()->get("version");
    QStringList lCurrentVersion = strCurrentVersion.split(".");
    int iCurrentMajor = lCurrentVersion.at(0).toInt();
    int iCurrentMinor = lCurrentVersion.at(1).toInt();
    int iCurrentPatch = lCurrentVersion.at(2).toInt();
    int iCurrentVersion = (QString("%1%2%3").arg(iCurrentMajor).arg(iCurrentMinor).arg(iCurrentPatch)).toInt();

    QString strAvailableVersion = Settings::instance()->get("available_version");
    QStringList lAvailableVersion = strAvailableVersion.split(".");
    int iAvailableMajor = lAvailableVersion.at(0).toInt();
    int iAvailableMinor = lAvailableVersion.at(1).toInt();
    int iAvailablePatch = lAvailableVersion.at(2).toInt();
    int iAvailableVersion = (QString("%1%2%3").arg(iAvailableMajor).arg(iAvailableMinor).arg(iAvailablePatch)).toInt();

    if (iAvailableVersion == iCurrentVersion)
        return UPDATE_STATUS_UPTODATE;
    else if (iAvailableVersion < iCurrentVersion)
        return UPDATE_STATUS_BETA;
    else if (iAvailableVersion > iCurrentVersion)
        return UPDATE_STATUS_OUTOFDATE;
    else
        return UPDATE_STATUS_UNKNOWN;
}

void Update::saveUpdate(QString strXml)
{
    QDomDocument doc;
    doc.setContent(strXml);

    QString strAvailableVersion = doc.elementsByTagName("version").item(0).toElement().text();
    QString strWhatsNew = doc.elementsByTagName("whats_new").item(0).toElement().text();
    QString strMOTD = doc.elementsByTagName("motd").item(0).toElement().text();

    if (!strWhatsNew.isEmpty())
        Settings::instance()->set("whats_new", strWhatsNew);

    if (!strMOTD.isEmpty())
        Settings::instance()->set("motd", strMOTD);

    if (!strAvailableVersion.isEmpty())
        Settings::instance()->set("available_version", strAvailableVersion);
}

void Update::updateFinished(QNetworkReply *reply)
{
    reply->deleteLater();

    QString update_url = reply->property("update_url").toString();

    if (reply->error())
        hUpdateResults[update_url] = QString::null;
    else
    {
        QVariant possibleRedirectUrl = reply->attribute(QNetworkRequest::RedirectionTargetAttribute);
        if (!possibleRedirectUrl.toUrl().isEmpty())
        {
            updateRequest("GET", possibleRedirectUrl.toString(), update_url);
            return;
        }

        QString strUpdateXml = reply->readAll();

        hUpdateResults[update_url] = strUpdateXml;
    }

    if (hUpdateResults.size() == 1)
    {
        checkUpdateSourceforge();
    }
    else if (hUpdateResults.size() == 2)
    {
        int updateGithub = fastParseVersion(hUpdateResults.value(UPDATE_URL_GITHUB));
        int updateSourceforge = fastParseVersion(hUpdateResults.value(UPDATE_URL_SOURCEFORGE));

        if ((updateGithub != 0) || (updateSourceforge != 0))
        {
            if (updateGithub >= updateSourceforge)
                saveUpdate(hUpdateResults.value(UPDATE_URL_GITHUB));
            else
                saveUpdate(hUpdateResults.value(UPDATE_URL_SOURCEFORGE));

            if (!Settings::instance()->get("motd").isEmpty())
            {
                QString strMOTD = Settings::instance()->get("motd");
                QString strMessageOfTheDay = QString("%Fb%%1 %2").arg(tr("Message Of The Day:"), strMOTD);
                Message::instance()->showMessage(STATUS_WINDOW, strMessageOfTheDay, MessageDefault);
            }

            if (Settings::instance()->get("available_version") != "0.0.0.0")
            {
                QString strVersionStatus = fullParseVersion();

                // save status
                Settings::instance()->set("version_status", strVersionStatus);

                if (Settings::instance()->getBool("updates"))
                    Notification::instance()->refreshUpdate();
            }
        }
    }
}

QString Update::getPlatform()
{
    QString strPlatform = "Unknown OS";
#ifdef Q_OS_WIN
    switch (QSysInfo::windowsVersion())
    {
        case QSysInfo::WV_32s: strPlatform = "Windows 3.1"; break;
        case QSysInfo::WV_95: strPlatform = "Windows 95"; break;
        case QSysInfo::WV_98: strPlatform = "Windows 98"; break;
        case QSysInfo::WV_Me: strPlatform = "Windows Me"; break;

        case QSysInfo::WV_NT: strPlatform = "Windows NT"; break;
        case QSysInfo::WV_2000: strPlatform = "Windows 2000"; break;
        case QSysInfo::WV_XP: strPlatform = "Windows XP"; break;
        case QSysInfo::WV_2003: strPlatform = "Windows 2003"; break;
        case QSysInfo::WV_VISTA: strPlatform = "Windows Vista"; break;
        case QSysInfo::WV_WINDOWS7: strPlatform = "Windows 7"; break;
        case QSysInfo::WV_WINDOWS8: strPlatform = "Windows 8"; break;
#if QT_VERSION >= QT_VERSION_CHECK(5, 2, 0)
        case QSysInfo::WV_WINDOWS8_1: strPlatform = "Windows 8.1"; break;
#endif
#if QT_VERSION >= QT_VERSION_CHECK(5, 5, 0)
        case QSysInfo::WV_WINDOWS10: strPlatform = "Windows 10"; break;
#endif

        case QSysInfo::WV_CE: strPlatform = "Windows CE"; break;
        case QSysInfo::WV_CENET: strPlatform = "Windows CE .NET"; break;
        case QSysInfo::WV_CE_5: strPlatform = "Windows CE 5.x"; break;
        case QSysInfo::WV_CE_6: strPlatform = "Windows CE 6.x"; break;

        default: strPlatform = "Windows"; break;
    }
#else
    strPlatform = "Linux";
#endif

    return strPlatform;
}
