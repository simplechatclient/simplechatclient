/****************************************************************************
 *                                                                          *
 *   This file is part of Simple Chat Client                                *
 *   Copyright (C) 2011 Piotr Łuczko <piotr.luczko@gmail.com>               *
 *                                                                          *
 *   This program is free software: you can redistribute it and/or modify   *
 *   it under the terms of the GNU General Public License as published by   *
 *   the Free Software Foundation, either version 3 of the License, or      *
 *   (at your option) any later version.                                    *
 *                                                                          *
 *   This program is distributed in the hope that it will be useful,        *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *   GNU General Public License for more details.                           *
 *                                                                          *
 *   You should have received a copy of the GNU General Public License      *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 *                                                                          *
 ****************************************************************************/

#include <QAction>
#include <QDateTime>
#include <QHostInfo>
#include <QSettings>
#include <QTcpSocket>
#include <QTimer>
#include "config.h"
#include "core.h"
#include "crypt.h"
#include "network.h"

Network::Network(QString param1, int param2)
{
    strServer = param1;
    iPort = param2;

    iActive = 0;
    bAuthorized = false;
    timerReconnect = new QTimer();
    timerReconnect->setInterval(1000*30); // 30 sec
    QSettings settings;
    settings.setValue("reconnect", "true");
    timerPong = new QTimer();
    timerPong->setInterval(1000*60*1); // 1 min
    timerPing = new QTimer();
    timerPing->setInterval(1000*30); // 30 sec
    timerLag = new QTimer();
    timerLag->setInterval(1000*10); // 10 sec
    timerQueue = new QTimer();
    timerQueue->setInterval(300); // 0.3 sec

    socket = new QTcpSocket(this);
    socket->setSocketOption(QAbstractSocket::LowDelayOption, 1);
    socket->setSocketOption(QAbstractSocket::KeepAliveOption, 0);

    QObject::connect(socket, SIGNAL(readyRead()), this, SLOT(recv()));
    QObject::connect(socket, SIGNAL(connected()), this, SLOT(connected()));
    QObject::connect(socket, SIGNAL(disconnected()), this, SLOT(disconnected()));
    QObject::connect(socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(error(QAbstractSocket::SocketError)));
    QObject::connect(socket, SIGNAL(stateChanged(QAbstractSocket::SocketState)), this, SLOT(stateChanged(QAbstractSocket::SocketState)));

    QObject::connect(timerPong, SIGNAL(timeout()), this, SLOT(timeoutPong()));
    QObject::connect(timerPing, SIGNAL(timeout()), this, SLOT(timeoutPing()));
    QObject::connect(timerLag, SIGNAL(timeout()), this, SLOT(timeoutLag()));
    QObject::connect(timerQueue, SIGNAL(timeout()), this, SLOT(timeoutQueue()));

    QObject::connect(timerReconnect, SIGNAL(timeout()), this, SLOT(reconnect()));
}

Network::~Network()
{
    socket->deleteLater();
    timerQueue->stop();
    timerLag->stop();
    timerPing->stop();
    timerPong->stop();
    timerReconnect->stop();
    Core::instance()->autoAwayTimer->stop();
}

void Network::run()
{
    exec();
}

bool Network::isConnected()
{
    if (socket->state() == QAbstractSocket::ConnectedState)
        return true;
    else
        return false;
}

bool Network::isWritable()
{
    return socket->isWritable();
}

void Network::clearAll()
{
    QSettings settings;

    // close cam socket
    if (Core::instance()->kamerzystaSocket->state() == QAbstractSocket::ConnectedState)
        Core::instance()->kamerzystaSocket->disconnectFromHost();

    // set button
    emit setDisconnected();

    // set lag
    Core::instance()->lagAct->setText("Lag: ?");

    // update busy button
    Core::instance()->busyAct->setChecked(false);

    // update away button
    Core::instance()->awayAct->setChecked(false);

    // update nick
    emit updateNick(tr("(Unregistered)"));

    // clear nicklist
    emit clearAllNicklist();

    // state
    settings.setValue("logged", "off");

    // timer
    if (timerPong->isActive())
        timerPong->stop();
    if (timerPing->isActive())
        timerPing->stop();
    if (timerLag->isActive())
        timerLag->stop();
    if (timerQueue->isActive())
        timerQueue->stop();

    // reconnect
    if (timerReconnect->isActive())
        timerReconnect->stop();

    // auto-away
    if (Core::instance()->autoAwayTimer->isActive())
        Core::instance()->autoAwayTimer->stop();

    // last active
    settings.setValue("last_active", "0");

    // clear queue
    msgSendQueue.clear();
    msgSendQueueNS.clear();

    // authorized
    bAuthorized = false;
}

void Network::authorize()
{
    // authorized
    bAuthorized = true;

    // get nick
    QSettings settings;
    QString strNick = settings.value("nick").toString();
    QString strPass = settings.value("pass").toString();

    // nick & pass is null
    if ((strNick.isEmpty()) && (strPass.isEmpty()))
        strNick = "~test";

    // decrypt pass
    if (!strPass.isEmpty())
    {
        Crypt *pCrypt = new Crypt();
        strPass = pCrypt->decrypt(strNick, strPass);
        delete pCrypt;
    }

    // correct nick
    if ((strPass.isEmpty()) && (strNick[0] != '~'))
        strNick = "~"+strNick;
    if ((!strPass.isEmpty()) && (strNick[0] == '~'))
        strNick = strNick.right(strNick.length()-1);

    Config *pConfig = new Config();
    settings.setValue("nick", strNick);
    pConfig->setValue("nick", strNick);
    delete pConfig;

    // update nick
    emit updateNick(strNick);
    // update actions
    emit updateActions();

    // set current nick
    QString strCurrentNick = strNick;
        if (strCurrentNick[0] == '~')
    strCurrentNick = strNick.right(strNick.length()-1);

    // request uo key
    emit authorize(strCurrentNick, strNick, strPass);
}

void Network::connect()
{
    if (socket->state() == QAbstractSocket::UnconnectedState)
    {
        // host
        QHostInfo hInfo = QHostInfo::fromName(strServer);

        if (hInfo.error() != QHostInfo::NoError)
        {
            QString strError = QString(tr("Error: Could not connect to the server [%1]")).arg(hInfo.errorString());
            emit showMsgAll(strError, ErrorMessage);

            // clear all
            clearAll();

            // reconnect
            if ((!timerReconnect->isActive()) && (!bAuthorized))
                timerReconnect->start();

            return;
        }

        // clear all
        clearAll();

        // random
        int iRandom = qrand() % hInfo.addresses().size();

        // set active
        QDateTime dt = QDateTime::currentDateTime();
        iActive = (int)dt.toTime_t();

        // connect
        socket->connectToHost(hInfo.addresses().at(iRandom).toString(), iPort);

        // start timers
        timerPong->start();
        timerPing->start();
        timerLag->start();
        timerQueue->start();
    }
    else
    {
        QString strError = tr("Error: Could not connect to the server - connection already exists!");
        emit showMsgAll(strError, ErrorMessage);
    }
}

void Network::connected()
{
    emit setConnected();
    Core::instance()->lagAct->setText("Lag: ?");

    QString strDisplay = tr("Connected to server");
    emit showMsgAll(strDisplay, ErrorMessage);

    // authorize
    if (!bAuthorized)
        authorize();
}

void Network::disconnect()
{
    // clear queue
    msgSendQueue.clear();
    msgSendQueueNS.clear();

    // send quit
    if ((socket->isValid()) && (socket->state() == QAbstractSocket::ConnectedState) && (socket->isWritable()))
    {
        socket->write("QUIT\r\n");
        socket->waitForBytesWritten();
    }

    // close
    if (socket->state() == QAbstractSocket::ConnectedState)
        socket->disconnectFromHost();

    // clear all
    clearAll();
}

void Network::disconnected()
{
    if (socket->error() != QAbstractSocket::UnknownSocketError)
    {
        QString strError = QString(tr("Disconnected from server [%1]")).arg(socket->errorString());
        emit showMsgAll(strError, ErrorMessage);
    }
    else
    {
        QString strError = tr("Disconnected from server");
        emit showMsgAll(strError, ErrorMessage);
    }

    // clear all
    clearAll();

    // reconnect
    if ((!timerReconnect->isActive()) && (!bAuthorized))
        timerReconnect->start();
}

void Network::reconnect()
{
    if (timerReconnect->isActive())
        timerReconnect->stop();

    QSettings settings;
    if (settings.value("reconnect").toString() == "true")
    {
        if ((!this->isConnected()) && (settings.value("logged").toString() == "off"))
        {
            QString strDisplay = tr("Reconnecting...");
            emit showMsgAll(strDisplay, InfoMessage);
            connect();
        }
    }
}

void Network::write(QString strData)
{
    if ((socket->isValid()) && (socket->state() == QAbstractSocket::ConnectedState) && (socket->isWritable()))
    {
#ifdef Q_WS_X11
        QSettings settings;
        if (settings.value("debug").toString() == "on")
            qDebug() << "-> " << strData;
#endif
        strData += "\r\n";
        QByteArray qbaData;
        for ( int i = 0; i < strData.size(); i++)
            qbaData.insert(i, strData.at(i).toAscii());

        if (socket->write(qbaData) == -1)
        {
            if (socket->state() == QAbstractSocket::ConnectedState)
            {
                QString strError = QString(tr("Error: Could not send data! [%1]")).arg(socket->errorString());
                emit showMsgActive(strError, ErrorMessage);
            }
            else if (socket->state() == QAbstractSocket::UnconnectedState)
            {
                QString strError = tr("Error: Could not send data! [Not connected]");
                emit showMsgActive(strError, ErrorMessage);
            }
        }
    }
    else
    {
        QString strError = tr("Error: Could not send data! [Not Connected]");
        emit showMsgActive(strError, ErrorMessage);
    }
}

void Network::send(QString strData)
{
    if (strData.startsWith("NS"))
        msgSendQueueNS.append(strData);
    else
        write(strData);
}

void Network::sendQueue(QString strData)
{
    if (strData.startsWith("NS"))
        msgSendQueueNS.append(strData);
    else
        msgSendQueue.append(strData);
}

void Network::recv()
{
    while (socket->canReadLine())
    {
        // read line
        QByteArray data = socket->readLine().trimmed();

        // set active
        QDateTime dt = QDateTime::currentDateTime();
        iActive = (int)dt.toTime_t();

        // process to kernel
        emit kernel(QString(data));
    }
}

void Network::error(QAbstractSocket::SocketError error)
{
    if (error == QAbstractSocket::RemoteHostClosedError) return; // supported by disconnected

    QString strError = QString(tr("Disconnected from server [%1]")).arg(socket->errorString());
    emit showMsgAll(strError, ErrorMessage);

    // clear all
    clearAll();

    // reconnect
    if ((!timerReconnect->isActive()) && (!bAuthorized))
        timerReconnect->start();
}

/**
 * Disable connect button if state not connected and not unconnected
 */
void Network::stateChanged(QAbstractSocket::SocketState socketState)
{
    if ((socketState != QAbstractSocket::UnconnectedState) && (socketState != QAbstractSocket::ConnectedState))
        emit setConnectEnabled(false);
    else
        emit setConnectEnabled(true);
}

void Network::timeoutLag()
{
    QDateTime dt = QDateTime::currentDateTime();
    int iCurrent = (int)dt.toTime_t();

    // update lag
    if (iCurrent-iActive > 30+10)
        Core::instance()->lagAct->setText(QString("Lag: %1s").arg(iCurrent-iActive));
}

void Network::timeoutPong()
{
    QDateTime dt = QDateTime::currentDateTime();
    int iCurrent = (int)dt.toTime_t();

    // check timeout
    if (iActive+301 < iCurrent)
    {
        if (socket->state() == QAbstractSocket::ConnectedState)
        {
            QString strDisplay = tr("No PONG reply from server in 301 seconds. Disconnecting...");
            emit showMsgAll(strDisplay, ErrorMessage);

            // disconnect
            disconnect();
        }
        iActive = iCurrent;
    }
}

void Network::timeoutPing()
{
    QDateTime dta = QDateTime::currentDateTime();
    int i1 = (int)dta.toTime_t(); // seconds that have passed since 1970
    QString t2 = dta.toString("zzz"); // miliseconds

    QSettings settings;
    if ((isConnected()) && (isWritable()) && (settings.value("logged").toString() == "on"))
        emit send(QString("PING :%1.%2").arg(i1).arg(t2));
}

void Network::timeoutQueue()
{
    if (socket->state() != QAbstractSocket::ConnectedState)
    {
        msgSendQueue.clear();
        msgSendQueueNS.clear();
        return;
    }

    if (msgSendQueue.size() > 0)
        write(msgSendQueue.takeFirst());
    else
    {
        if (msgSendQueueNS.size() > 0)
            write(msgSendQueueNS.takeFirst());
    }
}
