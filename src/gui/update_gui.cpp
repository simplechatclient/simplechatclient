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

#include <QDesktopServices>
#include <QDesktopWidget>
#include <QDir>
#include <QNetworkAccessManager>
#include <QNetworkCookieJar>
#include <QPushButton>
#include <QStandardPaths>
#include <QUrl>
#include "models/settings.h"
#include "update_gui.h"

#define DOWNLOAD_LINK "http://simplechatclient.github.io/download"
#define DOWNLOAD_SITE_LINK "https://github.com/simplechatclient/simplechatclient/releases/download/%1/scc-%2.exe"

UpdateGui::UpdateGui(QWidget *parent) : QDialog(parent)
{
    ui.setupUi(this);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowTitle(tr("Update"));
    // center screen
    move(QApplication::desktop()->screenGeometry(QApplication::desktop()->screenNumber(parent)).center()  - rect().center());

    createGui();
    setDefaultValues();
    createSignals();

    accessManager = new QNetworkAccessManager;
    cookieJar = new QNetworkCookieJar();
    accessManager->setCookieJar(cookieJar);
    connect(accessManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(networkFinished(QNetworkReply*)));
}

UpdateGui::~UpdateGui()
{
    accessManager->deleteLater();
    cookieJar->deleteLater();
}

void UpdateGui::createGui()
{
#ifndef Q_OS_WIN
    ui.progressBar->hide();
#endif
    ui.pushButton_download->setIcon(QIcon(":/images/breeze/download.svg"));
    ui.pushButton_download->setText(tr("Download"));
}

void UpdateGui::setDefaultValues()
{
    strFullVersion = Settings::instance()->get("available_version");

    QStringList lAvailableVersion = strFullVersion.split(".");
    QString strCurrentMajor = lAvailableVersion.at(0);
    QString strCurrentMinor = lAvailableVersion.at(1);
    QString strCurrentPatch = lAvailableVersion.at(2);

    strShortVersion = QString("%1.%2.%3").arg(strCurrentMajor).arg(strCurrentMinor).arg(strCurrentPatch);

    ui.label_title->setText(QString("Simple Chat Client %1").arg(strShortVersion));
    ui.label_content->setText(Settings::instance()->get("whats_new"));
}

void UpdateGui::createSignals()
{
    connect(ui.pushButton_download, SIGNAL(clicked()), this, SLOT(buttonDownload()));
}

void UpdateGui::buttonDownload()
{
    ui.pushButton_download->setEnabled(false);

#ifdef Q_OS_WIN
    QNetworkRequest request;
    request.setUrl(QUrl(QString(DOWNLOAD_SITE_LINK).arg(strShortVersion, strFullVersion)));
    request.setHeader(QNetworkRequest::UserAgentHeader, "SimpleChatClientUpdate (+http://simplechatclient.github.io)");

    accessManager->get(request);
#else
    QDesktopServices::openUrl(QUrl(DOWNLOAD_LINK));
#endif
}

void UpdateGui::downloadedFile(const QByteArray &bData)
{
    QString path = QFileInfo(QStandardPaths::writableLocation(QStandardPaths::TempLocation)).absoluteFilePath();

    // save
    QString fileName = path+"/scc-"+strFullVersion+".exe";

    QFile file(fileName);
    if (file.exists()) file.remove();
    if (!file.open(QIODevice::WriteOnly))
    {
        showError(tr("Cannot save file"));
        return;
    }
    file.write(bData);
    file.close();

    // execute
    QString strExecute = QDir::toNativeSeparators(fileName);
    bool bExecute = QDesktopServices::openUrl(QUrl::fromLocalFile(strExecute));

    if (!bExecute)
    {
        showError(tr("Cannot open saved file"));
        return;
    }

    exit(0);
}

void UpdateGui::networkFinished(QNetworkReply *reply)
{
    reply->deleteLater();

    if (reply->error())
    {
        showError(tr("Cannot download file"));
        return;
    }

    QVariant possibleRedirectUrl = reply->attribute(QNetworkRequest::RedirectionTargetAttribute);

    if (!possibleRedirectUrl.toUrl().isEmpty())
    {
        QNetworkRequest request;
        request.setUrl(possibleRedirectUrl.toUrl());
        request.setHeader(QNetworkRequest::UserAgentHeader, "SimpleChatClientUpdate (+http://simplechatclient.github.io)");

        QNetworkReply *pReply = accessManager->get(request);
        connect(pReply, SIGNAL(downloadProgress(qint64,qint64)), this, SLOT(downloadProgress(qint64,qint64)));
        connect(pReply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(downloadError(QNetworkReply::NetworkError)));
        return;
    }

    int iStatusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    QByteArray bData = reply->readAll();

    if ((bData.isEmpty()) || (iStatusCode != 200))
    {
        showError(tr("Cannot download file"));
    }
    else
    {
        downloadedFile(bData);
    }
}

void UpdateGui::downloadProgress(qint64 bytesReceived,qint64 bytesTotal)
{
    ui.progressBar->setValue(bytesReceived);
    ui.progressBar->setMaximum(bytesTotal);
}

void UpdateGui::downloadError(QNetworkReply::NetworkError)
{
    showError(tr("Cannot download file"));
}

void UpdateGui::showError(const QString &strError)
{
    ui.progressBar->hide();
    ui.pushButton_download->hide();

    ui.label_title->setText("");
    ui.label_content->setText(QString("<center><b>%1<br/>%2<br/><br/>%3<br/><a href=\"%4\">%4</a></b></center>").arg(tr("Error"), strError, tr("Visit homepage"), "http://simplechatclient.github.io"));
}
