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

#include <QDomDocument>
#include <QNetworkAccessManager>
#include <QNetworkCookie>
#include <QNetworkCookieJar>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QObject>
#include <QTextCodec>
#include <QTimer>
#include <QUrl>

#include "gui/captcha_gui.h"
#include "core/core.h"
#include "models/message.h"
#include "models/settings.h"
#include "onet_auth.h"

#define AJAX_API "http://czat.onet.pl/include/ajaxapi.xml.php3"

OnetAuth::OnetAuth()
{
    accessManager = new QNetworkAccessManager();
    cookieJar = new QNetworkCookieJar();
    accessManager->setCookieJar(cookieJar);
    connect(accessManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(networkFinished(QNetworkReply*)));

    Settings::instance()->setBool("authorizing", false);

    timerSk = new QTimer();
    timerSk->setInterval(1000*60*9); // 9 min
    connect(timerSk, SIGNAL(timeout()), this, SLOT(refreshSk()));
    timerSk->start();
}

OnetAuth::~OnetAuth()
{
    timerSk->stop();
    delete timerSk;
    accessManager->deleteLater();
    cookieJar->deleteLater();
}

void OnetAuth::authorize(QString _strNick, QString _strPass)
{
    if (_strNick.isEmpty())
        return; // empty nick

    if (Settings::instance()->getBool("logged"))
        return; // already logged

    if (Settings::instance()->getBool("authorizing"))
        return; // already authorizing

    Settings::instance()->setBool("authorizing", true);
    emit authStateChanged();

    strFullNick = _strNick.left(32);
    strNick = (_strNick.at(0) == '~' ? _strNick.remove(0,1).left(31) : _strNick.left(32));
    strPass = _strPass;
    strNickLen = QString::number(strNick.length());
    bRegisteredNick = strFullNick.at(0) != '~';
    bOverride = Settings::instance()->getBool("override");

    if (Settings::instance()->getBool("debug"))
    {
        qDebug() << "Override: " << bOverride;
        qDebug() << "Logged: " << Settings::instance()->get("logged");
        qDebug() << "Authorizing: " << Settings::instance()->get("authorizing");
    }

    // update nick
    emit updateNick(strFullNick);

    // remove cookies
    removeCookies();

    getChat();
}

void OnetAuth::getChat()
{
    if (Settings::instance()->getBool("debug")) { qDebug() << "Request: chat"; }

    // chat
    QString strUrl = "http://czat.onet.pl/chat.html";
    QString strContent = "ch=&n=&p=&category=0";

    QNetworkRequest request;
    request.setUrl(QUrl(strUrl));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    QNetworkReply *pReply = accessManager->post(request, strContent.toLatin1());
    pReply->setProperty("category", AT_chat);
}

void OnetAuth::getDeploy()
{
    if (Settings::instance()->getBool("debug")) { qDebug() << "Request: deploy"; }

    // deploy
    QString strUrl = "http://czat.onet.pl/_s/deployOnetCzat.js";

    QNetworkReply *pReply = accessManager->get(QNetworkRequest(QUrl(strUrl)));
    pReply->setProperty("category", AT_deploy);
}

void OnetAuth::gotDeploy(const QString &strData)
{
    strVersion = this->getVersion(strData);
    strVersion = QString("1.1(%1 - R)").arg(strVersion);
    strVersionLen = QString::number(strVersion.length());
}

void OnetAuth::getKropka1()
{
    if (Settings::instance()->getBool("debug")) { qDebug() << "Request: kropka 1"; }

    // kropka
    QString strUrl = "http://kropka.onet.pl/_s/kropka/1?DV=czat/applet";

    QNetworkReply *pReply = accessManager->get(QNetworkRequest(QUrl(strUrl)));
    pReply->setProperty("category", AT_kropka_1);
}

void OnetAuth::getKropka1Full()
{
    if (Settings::instance()->getBool("debug")) { qDebug() << "Request: kropka 1 full"; }

    // full
    QString strUrl = "http://kropka.onet.pl/_s/kropka/1?DV=czat/applet/FULL";

    QNetworkReply *pReply = accessManager->get(QNetworkRequest(QUrl(strUrl)));
    pReply->setProperty("category", AT_kropka_1_full);
}

void OnetAuth::getKropka5Full()
{
    if (Settings::instance()->getBool("debug")) { qDebug() << "Request: kropka 5 full"; }

    // secure kropka
    QString strUrl = "http://kropka.onet.pl/_s/kropka/5?DV=czat/applet/FULL";

    QNetworkReply *pReply = accessManager->get(QNetworkRequest(QUrl(strUrl)));
    pReply->setProperty("category", AT_kropka_5_full);
}

void OnetAuth::getSk()
{
    if (Settings::instance()->getBool("debug")) { qDebug() << "Request: sk"; }

    // sk
    QString strUrl = "http://czat.onet.pl/sk.gif";

    QNetworkReply *pReply = accessManager->get(QNetworkRequest(QUrl(strUrl)));
    pReply->setProperty("category", AT_sk);
}

void OnetAuth::getSecureLogin()
{
    if (Settings::instance()->getBool("debug")) { qDebug() << "Request: secure login"; }

    // secure login
    QString strContent = QString("r=&url=&login=%1&haslo=%2&app_id=20&ssl=1&ok=1").arg(strNick, strPass);
    QString strUrl = "https://secure.onet.pl/mlogin.html";

    QNetworkRequest request;
    request.setUrl(QUrl(strUrl));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    QNetworkReply *pReply = accessManager->post(request, strContent.toLatin1());
    pReply->setProperty("category", AT_secure_login);
    pReply->ignoreSslErrors();
}

void OnetAuth::getOverride()
{
    if (Settings::instance()->getBool("debug")) { qDebug() << "Request: override"; }

    // override
    QString strContent = QString("api_function=userOverride&params=a:1:{s:4:\"nick\";s:%1:\"%2\";}").arg(strNickLen, strNick);
    QString strUrl = AJAX_API;

    QNetworkRequest request;
    request.setUrl(QUrl(strUrl));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    QNetworkReply *pReply = accessManager->post(request, strContent.toLatin1());
    pReply->setProperty("category", AT_override);
}

void OnetAuth::getUo()
{
    if (Settings::instance()->getBool("debug")) { qDebug() << "Request: getuo"; }

    // getuo
    QString strRegistered = bRegisteredNick == true ? "0" : "1";
    QString strContent = QString("api_function=getUoKey&params=a:3:{s:4:\"nick\";s:%1:\"%2\";s:8:\"tempNick\";i:%3;s:7:\"version\";s:%4:\"%5\";}").arg(strNickLen, strNick, strRegistered, strVersionLen, strVersion);
    QString strUrl = AJAX_API;

    QNetworkRequest request;
    request.setUrl(QUrl(strUrl));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    QNetworkReply *pReply = accessManager->post(request, strContent.toLatin1());
    pReply->setProperty("category", AT_uo);
}

void OnetAuth::showCaptchaDialog()
{
    // captcha dialog
    CaptchaGui(cookieJar, &strCaptcha).exec();
}

void OnetAuth::getCheckCode()
{
    if (Settings::instance()->getBool("debug")) { qDebug() << "Request: check code"; }

    // check code
    QString strCaptchaSize = QString::number(strCaptcha.size());
    QString strContent = QString("api_function=checkCode&params=a:1:{s:4:\"code\";s:%1:\"%2\";}").arg(strCaptchaSize, strCaptcha);
    QString strUrl = AJAX_API;

    QNetworkRequest request;
    request.setUrl(QUrl(strUrl));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    QNetworkReply *pReply = accessManager->post(request, strContent.toLatin1());
    pReply->setProperty("category", AT_check_code);
}

void OnetAuth::networkFinished(QNetworkReply *reply)
{
    reply->deleteLater();

    if (reply->error())
    {
        if (Settings::instance()->getBool("debug"))
            qWarning() << "Error OnetAuth network: " << reply->errorString();

        Settings::instance()->setBool("authorizing", false);
        emit authStateChanged();
        return;
    }

    int category = reply->property("category").toInt();

    QVariant possibleRedirectUrl = reply->attribute(QNetworkRequest::RedirectionTargetAttribute);
    if (!possibleRedirectUrl.toUrl().isEmpty())
    {
        QNetworkReply *replyRedirect = accessManager->get(QNetworkRequest(possibleRedirectUrl.toUrl()));
        replyRedirect->setProperty("category", category);
        return;
    }

    QByteArray bData = reply->readAll();

    switch (category)
    {
        case AT_chat:
            getDeploy();
            break;
        case AT_deploy:
            gotDeploy(QString(bData));
            getKropka1();
            break;
        case AT_kropka_1:
            getKropka1Full();
            break;
        case AT_kropka_1_full:
            getKropka5Full();
            break;
        case AT_kropka_5_full:
            getSk();
            break;
        case AT_sk:
            if (bRegisteredNick)
                getSecureLogin();
            else
            {
                showCaptchaDialog();
                getCheckCode();
            }
            break;
        case AT_secure_login:
            if (bOverride)
                getOverride();
            else
                getUo();
            break;
        case AT_override:
            getUo();
            break;
        case AT_check_code:
            getUo();
            break;
        case AT_uo:
            {
                if (!Settings::instance()->getBool("logged"))
                {
                    requestFinished(bData);

                    saveCookies();
                }

                Settings::instance()->setBool("authorizing", false);
                emit authStateChanged();
                break;
            }
        case AT_refresh_sk:
            saveCookies();
            break;
        default:
            if (Settings::instance()->getBool("debug"))
                qWarning() << "Error OnetAuth undefined category";

            Settings::instance()->setBool("authorizing", false);
            emit authStateChanged();
            break;
    }
}

void OnetAuth::saveCookies()
{
    QStringList constCookies;
    constCookies << "onet_ubi" << "onet_cid" << "onet_sid" << "onet_uid" << "onetzuo_ticket" << "onet_uoi" << "onet_sgn";

    // save cookies
    QList<QNetworkCookie> cookies = accessManager->cookieJar()->cookiesForUrl(QUrl("http://czat.onet.pl/"));
    foreach (const QNetworkCookie &cookie, cookies)
    {
        QString strKey = cookie.name();
        QString strValue = cookie.value();

        if (constCookies.contains(strKey))
            Settings::instance()->set(strKey, strValue);
    }
}

void OnetAuth::removeCookies()
{
    QStringList constCookies;
    constCookies << "onet_ubi" << "onet_cid" << "onet_sid" << "onet_uid" << "onetzuo_ticket" << "onet_uoi" << "onet_sgn";

    foreach (const QString &constCookie, constCookies)
        Settings::instance()->set(constCookie, QString::null);

    // clear from cookie jar
    QList<QNetworkCookie> cookies = accessManager->cookieJar()->cookiesForUrl(QUrl("http://czat.onet.pl/"));
    foreach (const QNetworkCookie &cookie, cookies)
        accessManager->cookieJar()->deleteCookie(cookie);
}

QString OnetAuth::getVersion(const QString &strData)
{
    if (!strData.isEmpty())
    {
        if (strData.contains("OnetCzatLoader"))
        {
            QString strFind1 = "signed-OnetCzatLoader-";
            QString strFind2 = ".jar";
            int iPos1 = strData.indexOf(strFind1)+strFind1.length();
            int iPos2 = strData.indexOf(strFind2, iPos1);
            QString strVersion = strData.mid(iPos1, iPos2-iPos1);

            if ((!strVersion.isEmpty()) && (strVersion.length() > 0) && (strVersion.length() < 20))
                return strVersion;
        }
    }

    return "20140526-0005-3";
}

void OnetAuth::requestFinished(const QByteArray &bData)
{
    if (!bData.isEmpty())
    {
        QTextCodec *codec = QTextCodec::codecForName("ISO-8859-2");
        QString strDataUtf8 = codec->toUnicode(bData);

        QDomDocument doc;
        doc.setContent(strDataUtf8);

        // <?xml version="1.0" encoding="ISO-8859-2"?><root><uoKey>LY9j2sXwio0G_yo3PdpukDL8iZJGHXKs</uoKey><zuoUsername>~Succubi_test</zuoUsername><error err_code="TRUE"  err_text="wartość prawdziwa" ></error></root>
        // <?xml version="1.0" encoding="ISO-8859-2"?><root><error err_code="-2"  err_text="U.ytkownik nie zalogowany" ></error></root>

        QDomNode dError = doc.elementsByTagName("error").item(0);
        QString strErrorCode = dError.attributes().namedItem("err_code").nodeValue();
        QString strErrorText = dError.attributes().namedItem("err_text").nodeValue();

        if (strErrorCode.toLower() == "true")
        {
            QString strUOKey = doc.elementsByTagName("uoKey").item(0).toElement().text();
            QString strUONick = doc.elementsByTagName("zuoUsername").item(0).toElement().text();

            Settings::instance()->set("uo_key", strUOKey);
            Settings::instance()->set("uo_nick", strUONick);

            // send auth
            if (Core::instance()->network->isConnected())
            {
                Core::instance()->network->send(QString("NICK %1").arg(strUONick));
                Core::instance()->network->send("AUTHKEY");
            }
        }
        else
        {
            QString strError = QString(tr("Error: Authentication error [%1]")).arg(strErrorText);
            Message::instance()->showMessageActive(strError, MessageError);

            Core::instance()->network->disconnect();
        }
    }
    else
    {
        QString strError = tr("Error: Authorization Failed.");
        Message::instance()->showMessageActive(strError, MessageError);

        Core::instance()->network->disconnect();
    }
}

void OnetAuth::refreshSk()
{
    if (Core::instance()->network->isConnected() && bRegisteredNick)
    {
        if (Settings::instance()->getBool("debug")) { qDebug() << "Request: refresh_sk"; }

        // sk
        QString strUrl = "http://czat.onet.pl/sk.gif";

        QNetworkReply *pReply = accessManager->get(QNetworkRequest(QUrl(strUrl)));
        pReply->setProperty("category", AT_refresh_sk);
    }
}
