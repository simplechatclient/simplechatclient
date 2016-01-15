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

#ifndef ONET_AUTH_H
#define ONET_AUTH_H

#include <QObject>

QT_BEGIN_NAMESPACE
class QNetworkAccessManager;
class QNetworkCookieJar;
class QNetworkReply;
class QTimer;
QT_END_NAMESPACE

/**
 * Authorize class for onet
 */
class OnetAuth : public QObject
{
    Q_OBJECT
public:
    OnetAuth();
    virtual ~OnetAuth();

public slots:
    void authorize(QString _strNick, QString _strPass);

private:
    enum AuthType
    {
        AT_chat,
        AT_deploy,
        AT_kropka_1,
        AT_kropka_1_full,
        AT_kropka_5_full,
        AT_sk,
        AT_secure_login,
        AT_override,
        AT_check_code,
        AT_uo,
        AT_refresh_sk,
        AT_undefined
    };
    QNetworkAccessManager *accessManager;
    QNetworkCookieJar *cookieJar;

    QTimer *timerSk;

    QString strNick;
    QString strFullNick;
    QString strPass;
    QString strNickLen;
    QString strVersionLen;
    bool bRegisteredNick;
    bool bOverride;

    QString strVersion;
    QString strCaptcha;

    void getChat();
    void getDeploy();
    void gotDeploy(const QString &strData);
    void getKropka1();
    void getKropka1Full();
    void getKropka5Full();
    void getSk();
    void getSecureLogin();
    void getOverride();
    void getUo();
    void showCaptchaDialog();
    void getCheckCode();

    QString getVersion(const QString &strData);
    void saveCookies();
    void removeCookies();
    void requestFinished(const QByteArray &bData);

private slots:
    void networkFinished(QNetworkReply *reply);
    void refreshSk();

signals:
    void updateNick(const QString &strNick);
    void authStateChanged();
};

#endif // ONET_AUTH_H
