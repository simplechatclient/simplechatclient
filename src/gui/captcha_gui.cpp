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
#include <QNetworkAccessManager>
#include <QNetworkCookieJar>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QPushButton>
#include <QUrl>
#include "captcha_gui.h"

CaptchaGui::CaptchaGui(QNetworkCookieJar *_cookieJar, QString *_strCaptcha, QWidget *parent) : QDialog(parent), strCaptcha(_strCaptcha), cookieJar(_cookieJar)
{
    ui.setupUi(this);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowTitle(tr("Captcha"));
    // center screen
    move(QApplication::desktop()->screenGeometry(QApplication::desktop()->screenNumber(parent)).center()  - rect().center());

    createGui();
    createSignals();

    accessManager = new QNetworkAccessManager;
    accessManager->setCookieJar(cookieJar);
    connect(accessManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(networkFinished(QNetworkReply*)));

    getImg();
}

void CaptchaGui::createGui()
{
    ui.buttonBox->button(QDialogButtonBox::Ok)->setIcon(QIcon(":/images/breeze/dialog-ok.svg"));
    ui.buttonBox->button(QDialogButtonBox::Ok)->setText(tr("Ok"));

    ui.label_text->setText(tr("Enter the characters you see:"));
}

void CaptchaGui::createSignals()
{
    connect(ui.buttonBox, SIGNAL(accepted()), this, SLOT(buttonOk()));
}

void CaptchaGui::getImg()
{
    // clear
    ui.label_img->setText(tr("Loading..."));

    // request
    QNetworkReply *pReply = accessManager->get(QNetworkRequest(QUrl("http://czat.onet.pl/myimg.gif")));
    pReply->setProperty("category", "get_captcha");
}

void CaptchaGui::gotImg(const QByteArray &bData)
{
    // show img
    QPixmap pixmap;
    pixmap.loadFromData(bData);
    ui.label_img->setPixmap(pixmap);
}

void CaptchaGui::networkFinished(QNetworkReply *reply)
{
    reply->deleteLater();

    if (reply->error())
        return;

    QString strCategory = reply->property("category").toString();

    QVariant possibleRedirectUrl = reply->attribute(QNetworkRequest::RedirectionTargetAttribute);
    if (!possibleRedirectUrl.toUrl().isEmpty())
    {
        QNetworkReply *replyRedirect = accessManager->get(QNetworkRequest(possibleRedirectUrl.toUrl()));
        replyRedirect->setProperty("category", strCategory);
        return;
    }

    QByteArray bData = reply->readAll();

    if (bData.isEmpty())
        return;

    if (strCategory == "get_captcha")
        gotImg(bData);
}

void CaptchaGui::buttonOk()
{
    (*strCaptcha) = ui.lineEdit_code->text().trimmed();
    this->close();
}
