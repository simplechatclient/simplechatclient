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

#include <QDesktopWidget>
#include <QDomDocument>
#include <QMessageBox>
#include <QNetworkAccessManager>
#include <QNetworkCookie>
#include <QNetworkCookieJar>
#include <QNetworkReply>
#include <QUrl>
#include "profile_add_gui.h"
#include "register_nick_gui.h"

#define AJAX_API "http://czat.onet.pl/include/ajaxapi.xml.php3"

RegisterNickGui::RegisterNickGui(ProfileAddGui *_pProfileAddGui, QWidget *parent) : QDialog(parent), pProfileAddGui(_pProfileAddGui)
{
    ui.setupUi(this);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowTitle(tr("Register nick"));
    // center screen
    move(QApplication::desktop()->screenGeometry(QApplication::desktop()->screenNumber(parent)).center()  - rect().center());

    createGui();
    createSignals();

    accessManager = new QNetworkAccessManager;
    cookieJar = new QNetworkCookieJar();
    accessManager->setCookieJar(cookieJar);
    connect(accessManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(networkFinished(QNetworkReply*)));

    getCookies();
    getImg();
}

RegisterNickGui::~RegisterNickGui()
{
    delete cookieJar;
    accessManager->deleteLater();
}

void RegisterNickGui::createGui()
{
    ui.buttonBox->button(QDialogButtonBox::Ok)->setIcon(QIcon(":/images/breeze/dialog-ok.svg"));
    ui.buttonBox->button(QDialogButtonBox::Cancel)->setIcon(QIcon(":/images/breeze/dialog-cancel.svg"));
    ui.pushButton_refresh->setIcon(QIcon(":/images/breeze/view-refresh.svg"));

    ui.label_nick->setText(tr("Nick:"));
    ui.label_password->setText(tr("Password:"));
    ui.label_confirm_password->setText(tr("Confirm password:"));
    ui.label_code->setText(tr("Code:"));
    ui.pushButton_refresh->setText(tr("Refresh"));
    ui.buttonBox->button(QDialogButtonBox::Ok)->setText(tr("Ok"));
    ui.buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Cancel"));
}

void RegisterNickGui::createSignals()
{
    connect(ui.pushButton_refresh, SIGNAL(clicked()), this, SLOT(buttonRefresh()));
    connect(ui.buttonBox, SIGNAL(accepted()), this, SLOT(buttonOk()));
    connect(ui.buttonBox, SIGNAL(rejected()), this, SLOT(close()));
}

void RegisterNickGui::getCookies()
{
    QNetworkReply *pReply = accessManager->get(QNetworkRequest(QUrl("http://kropka.onet.pl/_s/kropka/1?DV=czat%2Findex")));
    pReply->setProperty("category", RT_cookies);
}

void RegisterNickGui::gotCookies()
{
    // save cookies
    QList<QNetworkCookie> cookies = accessManager->cookieJar()->cookiesForUrl(QUrl("http://czat.onet.pl/"));
    foreach (const QNetworkCookie &cookie, cookies)
    {
        QString strKey = cookie.name();
        QString strValue = cookie.value();

        mCookies.insert(strKey, strValue);
    }
}

void RegisterNickGui::getImg()
{
    // disable button
    ui.pushButton_refresh->setEnabled(false);

    // clear
    ui.label_img->setText(tr("Loading..."));

    // request
    QNetworkReply *pReply = accessManager->get(QNetworkRequest(QUrl("http://czat.onet.pl/myimg.gif")));
    pReply->setProperty("category", RT_img);
}

void RegisterNickGui::gotImg(const QByteArray &bData)
{
    // show img
    QPixmap pixmap;
    pixmap.loadFromData(bData);
    ui.label_img->setPixmap(pixmap);

    // enable button
    ui.pushButton_refresh->setEnabled(true);
}

void RegisterNickGui::registerNick()
{
    QString strNick = ui.lineEdit_nick->text().trimmed();
    QString strNickLength = QString::number(strNick.length());
    QString strPassword = ui.lineEdit_password->text();
    QString strPasswordLength = QString::number(strPassword.length());
    QString strCode = ui.lineEdit_code->text().trimmed();
    QString strCodeLength = QString::number(strCode.length());

    if ((strNick.isEmpty()) || (strPassword.isEmpty()) || (strCode.isEmpty()))
        return;

    // set cookies
    QList<QNetworkCookie> cookieList;
    QNetworkCookie cookie;

    QHashIterator <QString, QString> it(mCookies);
    while (it.hasNext())
    {
         it.next();

         QString strKey = it.key();
         QString strValue = it.value();

         cookie.setName(strKey.toLatin1());
         cookie.setValue(strValue.toLatin1());
         cookieList.append(cookie);
     }

    accessManager->cookieJar()->setCookiesFromUrl(cookieList, QUrl("http://czat.onet.pl/"));

    // request
    QString strUrl = AJAX_API;
    QString strContent = QString("api_function=registerNick&params=a:3:{s:4:\"nick\";s:%1:\"%2\";s:4:\"pass\";s:%3:\"%4\";s:4:\"code\";s:%5:\"%6\";}").arg(strNickLength, strNick, strPasswordLength, strPassword, strCodeLength, strCode);

    QNetworkRequest request;
    request.setUrl(QUrl(strUrl));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    QNetworkReply *pReply = accessManager->post(request, strContent.toLatin1());
    pReply->setProperty("category", RT_register);
}

// <?xml version="1.0" encoding="ISO-8859-2"?><root><status>-1</status><error err_code="0"  err_text="OK" ></error></root>
// <?xml version="1.0" encoding="ISO-8859-2"?><root><status>1</status><error err_code="0"  err_text="OK" ></error></root>
void RegisterNickGui::parseResult(const QString &strResult)
{
    QDomDocument doc;
    doc.setContent(strResult);

    int iErrCode = doc.elementsByTagName("status").item(0).toElement().text().toInt();

    // correct
    if (iErrCode == 1)
    {
        // encrypt pass
        QString strNick = ui.lineEdit_nick->text().trimmed();
        QString strPassword = ui.lineEdit_password->text();

        pProfileAddGui->setNickPass(strNick, strPassword);

        // close
        this->close();
    }
    else
    {
        ui.lineEdit_code->clear();
        QString strErrText;

        switch(iErrCode)
        {
            case -1:
                strErrText = tr("Password must be at least 6 characters including at least one number.");
                break;
            case -2:
                strErrText = tr("Nick already exist");
                break;
            case -3:
                strErrText = tr("Invalid nick");
                break;
            case -4:
                strErrText = tr("Given nickname is unavailable");
                break;
            case -99:
                strErrText = tr("Unknown error. Please try again later.");
                break;
            case -101:
                strErrText = tr("Unable to connect to the server. Please try again later.");
                break;
            case -102:
                strErrText = tr("Server Error. Please try again later.");
                break;
            case -104:
                strErrText = tr("Invalid code");
                break;
            default:
                strErrText = QString(tr("Unknown error: %1")).arg(iErrCode);
                break;
        }

        QMessageBox::critical(0, tr("Error"), strErrText);
        getImg();
    }
}

void RegisterNickGui::networkFinished(QNetworkReply *reply)
{
    reply->deleteLater();

    if (reply->error())
        return;

    int category = reply->property("category").toInt();

    QVariant possibleRedirectUrl = reply->attribute(QNetworkRequest::RedirectionTargetAttribute);
    if (!possibleRedirectUrl.toUrl().isEmpty())
    {
        QNetworkReply *replyRedirect = accessManager->get(QNetworkRequest(possibleRedirectUrl.toUrl()));
        replyRedirect->setProperty("category", category);
        return;
    }

    QByteArray bData = reply->readAll();

    if (bData.isEmpty())
        return;

    switch (category)
    {
        case RT_img:
            gotImg(bData);
            break;
        case RT_cookies:
            gotCookies();
            break;
        case RT_register:
            parseResult(QString(bData));
            break;
    }
}

void RegisterNickGui::buttonOk()
{
    bool identical = false;

    if (ui.lineEdit_password->text() == ui.lineEdit_confirm_password->text())
        identical = true;

    if (identical)
        registerNick();
    else
    {
        ui.lineEdit_code->clear();
        QMessageBox::critical(0, tr("Error"), tr("Given passwords are not identical."));
        getImg();
    }
}

void RegisterNickGui::buttonRefresh()
{
    ui.lineEdit_code->clear();
    getImg();
}
