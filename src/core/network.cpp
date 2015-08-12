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

#include <QDateTime>
#include <QTcpSocket>
#include <QTimer>
#include <QTextCodec>
#include "models/autoaway.h"
#include "models/away.h"
#include "models/busy.h"
#include "common/config.h"
#include "core.h"
#include "common/simple_crypt.h"
#include "models/lag.h"
#include "models/nick.h"
#include "models/message.h"
#include "models/settings.h"
#include "network.h"

Network::Network(const QString &_strServer, int _iPort) : strServer(_strServer), iPort(_iPort), iActive(0), bAuthorized(false)
{
    Settings::instance()->setBool("reconnect", true);

    timerReconnect = new QTimer();
    timerReconnect->setInterval(1000*60*2); // 2 min
    timerPong = new QTimer();
    timerPong->setInterval(1000*60); // 1 min
    timerPing = new QTimer();
    timerPing->setInterval(1000*60); // 1 min
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
    QObject::connect(timerQueue, SIGNAL(timeout()), this, SLOT(timeoutQueue()));

    QObject::connect(timerReconnect, SIGNAL(timeout()), this, SLOT(reconnect()));
}

Network::~Network()
{
    socket->deleteLater();
    timerQueue->stop();
    timerPing->stop();
    timerPong->stop();
    timerReconnect->stop();
    Autoaway::instance()->stop();
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

void Network::clearAll()
{
    // close cam socket
    if (Core::instance()->kamerzystaSocket->state() == QAbstractSocket::ConnectedState)
        Core::instance()->kamerzystaSocket->disconnectFromHost();

    // set lag
    Lag::instance()->reset();

    // update busy button
    Busy::instance()->stop();

    // update away button
    Away::instance()->stop();

    // update nick
    emit updateNick(tr("(Unregistered)"));

    // clear nick
    Nick::instance()->clear();

    // state
    Settings::instance()->setBool("logged", false);

    // timer
    timerPong->stop();
    timerPing->stop();
    timerQueue->stop();

    // reconnect
    timerReconnect->stop();

    // auto-away
    Autoaway::instance()->stop();

    // last active
    Settings::instance()->set("last_active", "0");

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
    QString strNick = Settings::instance()->get("nick");
    QString strPass = Settings::instance()->get("pass");

    // nick & pass is null
    if ((strNick.isEmpty()) && (strPass.isEmpty()))
        strNick = "~test";

    // decrypt pass
    if (!strPass.isEmpty())
    {
        SimpleCrypt *pSimpleCrypt = new SimpleCrypt();
        strPass = pSimpleCrypt->decrypt(strNick, strPass);
        delete pSimpleCrypt;
    }

    // request uo key
    emit authorize(strNick, strPass);
}

void Network::connect()
{
    if (socket->state() == QAbstractSocket::UnconnectedState)
    {
        // clear all
        clearAll();

        // connect
        socket->connectToHost(strServer, iPort);
    }
    else
    {
        QString strError = tr("Error: Could not connect to the server - connection already exists!");
        Message::instance()->showMessageAll(strError, MessageError);
    }
}

void Network::connected()
{
    Lag::instance()->reset();

    // set active
    iActive = QDateTime::currentMSecsSinceEpoch();

    // start timers
    timerPong->start();
    timerPing->start();
    timerQueue->start();

    // display
    QString strDisplay = tr("Connected to server");
    Message::instance()->showMessageAll(strDisplay, MessageError);

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
    if ((socket->isValid()) && (socket->state() == QAbstractSocket::ConnectedState))
    {
        socket->write("QUIT\r\n");
        socket->waitForBytesWritten();
    }

    // clear all
    clearAll();

    // close
    if (socket->state() == QAbstractSocket::ConnectedState)
        socket->disconnectFromHost();
}

void Network::disconnected()
{
    if (socket->error() != QAbstractSocket::UnknownSocketError)
    {
        QString strError = QString(tr("Disconnected from server [%1]")).arg(socket->errorString());
        Message::instance()->showMessageAll(strError, MessageError);
    }
    else
    {
        QString strError = tr("Disconnected from server");
        Message::instance()->showMessageAll(strError, MessageError);
    }

    // clear all
    clearAll();

    // reconnect
    if ((!timerReconnect->isActive()) && (!bAuthorized))
        timerReconnect->start();
}

void Network::reconnect()
{
    timerReconnect->stop();

    if (Settings::instance()->getBool("reconnect"))
    {
        if (!this->isConnected() && !Settings::instance()->getBool("logged"))
        {
            QString strDisplay = tr("Reconnecting...");
            Message::instance()->showMessageAll(strDisplay, MessageInfo);
            connect();
        }
    }
}

void Network::write(const QString &strData)
{
    if ((socket->isValid()) && (socket->state() == QAbstractSocket::ConnectedState))
    {
        if (Settings::instance()->getBool("debug"))
            Message::instance()->showMessage(DEBUG_WINDOW, "-> "+strData, MessageDefault);

        QTextCodec *codec = QTextCodec::codecForName("ISO-8859-2");
        QByteArray bISOData = codec->fromUnicode(strData+"\r\n");

        if (socket->write(bISOData) == -1)
        {
            if (socket->state() == QAbstractSocket::ConnectedState)
            {
                QString strError = QString(tr("Error: Could not send data! [%1]")).arg(socket->errorString());
                Message::instance()->showMessageActive(strError, MessageError);
            }
            else if (socket->state() == QAbstractSocket::UnconnectedState)
            {
                QString strError = tr("Error: Could not send data! [Not connected]");
                Message::instance()->showMessageActive(strError, MessageError);
            }
        }
    }
    else
    {
        QString strError = tr("Error: Could not send data! [Not connected]");
        Message::instance()->showMessageActive(strError, MessageError);
    }
}

void Network::send(const QString &strData)
{
    if (strData.startsWith("NS INFO"))
        msgSendQueueNS.append(strData);
    else
        write(strData);
}

void Network::sendQueue(const QString &strData)
{
    if (strData.startsWith("NS INFO"))
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
        iActive = QDateTime::currentMSecsSinceEpoch();

        QTextCodec *codec = QTextCodec::codecForName("ISO-8859-2");
        QString strData = codec->toUnicode(data);

        // disabled due to bug in detection iso/utf
        /*
        QString strData = QString::fromUtf8(data);
        if (strData.toUtf8() != data)
        {
            QTextCodec *codec = QTextCodec::codecForName("ISO-8859-2");
            strData = codec->toUnicode(data);
        }
        */

        // process to kernel
        emit kernel(strData);
    }
}

void Network::error(QAbstractSocket::SocketError error)
{
    if (error == QAbstractSocket::RemoteHostClosedError) return; // supported by disconnected

    QString strError = QString(tr("Disconnected from server [%1]")).arg(socket->errorString());
    Message::instance()->showMessageAll(strError, MessageError);

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
    if (Settings::instance()->getBool("debug"))
        qDebug() << "Network socket state changed to: " << socketState;

    if (socketState == QAbstractSocket::UnconnectedState)
        Settings::instance()->set("socket_state", "disconnected");
    else if (socketState == QAbstractSocket::ConnectedState)
        Settings::instance()->set("socket_state", "connected");
    else
        Settings::instance()->set("socket_state", "unknown");

    emit socketStateChanged();
}

void Network::timeoutPong()
{
    qint64 iCurrentTime = QDateTime::currentMSecsSinceEpoch();

    // update lag
    if (iActive+60000 < iCurrentTime)
        Lag::instance()->update(iCurrentTime-iActive);

    // check pong timeout
    if (iActive+301000 < iCurrentTime) // 301 sec
    {
        if (socket->state() == QAbstractSocket::ConnectedState)
        {
            QString strDisplay = tr("No PONG reply from server in 301 seconds. Disconnecting...");
            Message::instance()->showMessageAll(strDisplay, MessageError);

            // disconnect
            disconnect();
        }
        iActive = iCurrentTime;
    }
}

void Network::timeoutPing()
{
    QString strMSecs = QString::number(QDateTime::currentMSecsSinceEpoch());

    if (isConnected() && Settings::instance()->getBool("logged"))
        send(QString("PING :%1").arg(strMSecs));
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
