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
#include <QDir>
#include <QFile>
#include <QHostAddress>
#include <QIcon>
#include <QMessageBox>
#include <QProcess>
#include <QPushButton>
#include <QSettings>
#include <QTcpSocket>
#include <QTimer>
#include <QTextStream>
#include <QUrl>
#include "core/core.h"
#include "log.h"
#include "models/settings.h"
#include "kamerzysta.h"

Kamerzysta::Kamerzysta(QTcpSocket *_socket) : socket(_socket)
{
    log("Constructor");

    timerGetPort = new QTimer();
    timerGetPort->setInterval(500);

    connect(socket, SIGNAL(connected()), this, SLOT(networkConnected()));
    connect(socket, SIGNAL(disconnected()), this, SLOT(networkDisconnected()));
    connect(socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(error(QAbstractSocket::SocketError)));
    connect(socket, SIGNAL(readyRead()), this, SLOT(networkRead()));
    connect(timerGetPort, SIGNAL(timeout()), this, SLOT(getPort()));
}

void Kamerzysta::close()
{
    log("Destructor");

    QObject::disconnect(timerGetPort, SIGNAL(timeout()), this, SLOT(getPort()));
    QObject::disconnect(socket, SIGNAL(readyRead()), this, SLOT(networkRead()));
    QObject::disconnect(socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(error(QAbstractSocket::SocketError)));
    QObject::disconnect(socket, SIGNAL(disconnected()), this, SLOT(networkDisconnected()));
    QObject::disconnect(socket, SIGNAL(connected()), this, SLOT(networkConnected()));

    delete timerGetPort;
}

void Kamerzysta::show(const QString &_strNick)
{
    strNick = _strNick;

    QString strUOKey = Settings::instance()->get("uo_key");

    log("Nick:"+strNick);
    log("UO:"+strUOKey);

    getPath();
}

void Kamerzysta::log(const QString &strData)
{
    if (Settings::instance()->getBool("debug"))
        Log::save("kamerzysta", strData, Log::Txt);
}

void Kamerzysta::getPath()
{
    QSettings winSettings(QSettings::UserScope, "Onet.pl", "InstalledApps");
    winSettings.beginGroup("Kamerzysta");
    strAppPath = winSettings.value("DataPath").toString();

    QDir dir;
    if (!dir.exists(strAppPath))
    {
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setWindowIcon(QIcon(":/images/logo16x16.png"));
        msgBox.setWindowTitle(tr("Error"));
        msgBox.setText(tr("Kamerzysta not found!"));
        QPushButton *downloadButton = msgBox.addButton(tr("Download"), QMessageBox::AcceptRole);
        downloadButton->setIcon(QIcon(":/images/breeze/preferences-web-browser-shortcuts.svg"));
        QPushButton *cancelButton = msgBox.addButton(tr("Cancel"), QMessageBox::RejectRole);
        cancelButton->setIcon(QIcon(":/images/breeze/dialog-cancel.svg"));
        msgBox.exec();

        if (msgBox.clickedButton() == downloadButton)
        {
            close();

            QString strWebsite = "http://pliki.onet.pl/Onet.Kamerzysta,Komunikatory,Windows,112343.html";
            QDesktopServices::openUrl(QUrl(strWebsite));

            return;
        }
        else
        {
            close();
            return;
        }
    }

    log("App path:"+strAppPath);

    if (QFile::exists(strAppPath+"\\port"))
    {
        log("Port:exist");
        kamerzystaRunning();
    }
    else
    {
        log("Port:not exist");
        kamerzystaNotRunning();
    }
}

void Kamerzysta::kamerzystaNotRunning()
{
    log("Kamerzysta not running");

    if (!QFile::exists(strAppPath+"\\installPath"))
    {
        QMessageBox::critical(0, tr("Error"), tr("Kamerzysta is wrong installed!"));
        close();
        return;
    }
    else
    {
        QFile file(strAppPath+"\\installPath");
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            QMessageBox::critical(0, tr("Error"), tr("Kamerzysta is wrong installed!"));
            close();
            return;
        }

        QTextStream ts(&file);
        strKamerzystaFile = ts.readLine();
        file.close();
    }

    log("Install path:"+strKamerzystaFile);

    if ((!strKamerzystaFile.isEmpty()) && (QFile::exists(strKamerzystaFile)))
    {
        int iPos = strKamerzystaFile.lastIndexOf("\\");
        QString strKamerzystaPath = strKamerzystaFile.left(iPos);
        strKamerzystaPath.replace("\\", "\\\\");
        strKamerzystaFile = strKamerzystaFile.right(strKamerzystaFile.length() - iPos -1);

        // change current path
        QString path = QDir::currentPath();
        QDir::setCurrent(strKamerzystaPath);

        // start kamerzysta.exe
        QProcess::startDetached(strKamerzystaFile);

        // change current path
        QDir::setCurrent(path);

        // network
        iTryGetPort = 0;

        // get port
        timerGetPort->start();
    }
    else
    {
        QMessageBox::critical(0, tr("Error"), tr("Kamerzysta is wrong installed!"));
        close();
        return;
    }
}

void Kamerzysta::kamerzystaRunning()
{
    log("Kamerzysta running");

    // send
    networkSend(QString("e%1").arg(strNick));

    // close
    close();
}

void Kamerzysta::getPort()
{
    QString strPort;

    QFile file(strAppPath+"\\port");
    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream ts(&file);
        strPort = ts.readLine();
        iPort = strPort.toInt();
        file.close();
    }

    log("Port:"+strPort);

    if (!strPort.isEmpty())
    {
        timerGetPort->stop();
        networkConnect();
    }
    else
    {
        iTryGetPort++;
    }
}

void Kamerzysta::authorize()
{
    QString strMe = Settings::instance()->get("nick");
    QString strUOKey = Settings::instance()->get("uo_key");

    networkSend(QString("d%1|%2").arg(strMe, strUOKey));

    if (strNick.isEmpty())
        log("Nick empty!");

    if ((strNick != strMe) && (!strNick.isEmpty()))
        networkSend(QString("e%1").arg(strNick));
}

void Kamerzysta::invite(QString strInviteNick)
{
    strInviteNick.remove(0,1);
    log(QString("Invite:%1").arg(strInviteNick));

    Core::instance()->network->send(QString("PRIV %1").arg(strInviteNick));
}

void Kamerzysta::networkConnect()
{
    if (socket->state() == QAbstractSocket::UnconnectedState)
    {
        log("Connecting...");
        socket->connectToHost(QHostAddress::LocalHost, iPort);
    }
}

void Kamerzysta::networkConnected()
{
    log("Connected");
}

void Kamerzysta::networkDisconnect()
{
    log("Disconnecting...");

    if (socket->state() == QAbstractSocket::ConnectedState)
        socket->disconnectFromHost();
}

void Kamerzysta::networkDisconnected()
{
    log("Disconnected");

    // clear nick
    strNick.clear();

    // stop timer
    if (timerGetPort->isActive())
        timerGetPort->stop();

    // like destructor
    close();
}

void Kamerzysta::networkSend(const QString &strData)
{
    if ((socket->isValid()) && (socket->state() == QAbstractSocket::ConnectedState))
    {
        log("Send:"+strData);

        socket->write((strData+"\r\n").toLatin1());
    }
}

void Kamerzysta::networkRead()
{
    log("Ready read");

    while(socket->canReadLine())
    {
        // read line
        QByteArray data = socket->readLine().trimmed();
        QString strDataRecv = QString(data);

        // log
        log("Recv:"+strDataRecv);

        // data
        if (!strDataRecv.isEmpty())
        {
            if (strDataRecv == "d")
                authorize();
            else if (strDataRecv.at(0) == 'e')
                invite(strDataRecv);
        }
    }
}

void Kamerzysta::error(QAbstractSocket::SocketError)
{
    log("Error:"+socket->errorString());

    if (socket->state() == QAbstractSocket::ConnectedState)
        networkDisconnect();
    else
    {
        // clear nick
        strNick.clear();

        // stop timer
        if (timerGetPort->isActive())
            timerGetPort->stop();

        // like destructor
        close();
    }
}
