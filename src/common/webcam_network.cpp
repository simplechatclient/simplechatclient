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
#include <QTextCodec>
#include <QTimer>
#include "models/settings.h"
#include "webcam_network.h"

WebcamNetwork::WebcamNetwork() : iLastActive(0), iLastKeepAlive(0), bReconnecting(true)
{
    timerPingPong = new QTimer();
    timerPingPong->setInterval(1000*60*1); // 1 min

    codec_iso8859_2 = QTextCodec::codecForName("ISO-8859-2");

    socket = new QTcpSocket(this);
    socket->setSocketOption(QAbstractSocket::LowDelayOption, 1);
    socket->setSocketOption(QAbstractSocket::KeepAliveOption, 0);

    connect(socket, SIGNAL(connected()), this, SLOT(networkConnected()));
    connect(socket, SIGNAL(disconnected()), this, SLOT(networkDisconnected()));
    connect(socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(networkError(QAbstractSocket::SocketError)));
    connect(socket, SIGNAL(readyRead()), this, SLOT(networkRead()));
    connect(timerPingPong, SIGNAL(timeout()), this, SLOT(timeoutPingpong()));
}

WebcamNetwork::~WebcamNetwork()
{
    socket->deleteLater();
    timerPingPong->stop();
}

bool WebcamNetwork::isConnected()
{
    if (socket->state() == QAbstractSocket::ConnectedState)
        return true;
    else
        return false;
}

void WebcamNetwork::clearAll()
{
    bText = true;
    iBytesNeed = 0;
    iBytesRecv = 0;

    // timer
    if (timerPingPong->isActive())
        timerPingPong->stop();
}

void WebcamNetwork::networkConnect()
{
    if (socket->state() == QAbstractSocket::UnconnectedState)
    {
        // clear all
        clearAll();

        // reconnect
        bReconnecting = true;

        // set last active
        iLastActive = QDateTime::currentMSecsSinceEpoch();

        // connect
        socket->connectToHost("czat-s.onet.pl", 5008);

        // timer
        timerPingPong->start();
    }
}

void WebcamNetwork::networkDisconnect()
{
    bReconnecting = false;
    if (socket->state() == QAbstractSocket::ConnectedState)
        socket->disconnectFromHost();
}

void WebcamNetwork::networkSend(const QString &strData)
{
    QByteArray bData = (strData+"\n").toLatin1();

    networkSendb(bData);
}

void WebcamNetwork::networkSendb(const QByteArray &bData)
{
    if ((socket->isValid()) && (socket->state() == QAbstractSocket::ConnectedState))
    {
        int ret = socket->write(bData);
        if (ret == -1)
        {
            if (socket->state() == QAbstractSocket::ConnectedState)
                emit error(QString(tr("Error: Failed to send data! [%1]")).arg(socket->errorString()));
            else if (socket->state() == QAbstractSocket::UnconnectedState)
                emit error(tr("Error: Failed to send data! [Not connected]"));
        }
        else
        {
            if (Settings::instance()->getBool("debug"))
                qDebug() << "CAM -> " << QString(bData);
        }
    }
    else
        emit error(tr("Error: Failed to send data! [Not connected]"));
}

void WebcamNetwork::networkRead()
{
    // set last active
    iLastActive = QDateTime::currentMSecsSinceEpoch();

    // read text
    if (bText)
    {
        // read line
        QByteArray data = socket->readLine().trimmed();
        QString strData = codec_iso8859_2->toUnicode(data);
        emit textKernel(strData);
    }
    // read data (image, description)
    else
    {
        for (int i = 0; i < socket->bytesAvailable(); i++)
        {
            if (iBytesRecv < iBytesNeed)
            {
                bData += socket->read(1);
                iBytesRecv++;
            }
        }

        if (iBytesRecv == iBytesNeed)
        {
            emit dataKernel(bData);

            // clear bdata
            bData.clear();
            bText = true;
            iBytesRecv = 0;
            iBytesNeed = 0;
        }
    }

    // again
    if (socket->bytesAvailable() != 0)
        networkRead();
}

void WebcamNetwork::networkConnected()
{
    emit connected();
}

void WebcamNetwork::networkDisconnected()
{
    emit disconnected();

    // clear all
    clearAll();

    // reconnect
    if (bReconnecting)
        QTimer::singleShot(1000*10, this, SLOT(slotNetworkConnect())); // 10 sec
}

void WebcamNetwork::networkError(QAbstractSocket::SocketError)
{
    emit error(socket->errorString());

    if (socket->state() == QAbstractSocket::ConnectedState)
        networkDisconnect();
    else if (socket->state() == QAbstractSocket::UnconnectedState)
    {
        // clear all
        clearAll();

        // reconnect
        if (bReconnecting)
            QTimer::singleShot(1000*10, this, SLOT(slotNetworkConnect())); // 10 sec
    }
}

void WebcamNetwork::slotNetworkConnect()
{
    networkConnect();
}

void WebcamNetwork::timeoutPingpong()
{
    qint64 iCurrent = QDateTime::currentMSecsSinceEpoch();

    if (iLastActive+301000 < iCurrent)
    {
        networkDisconnect();
        iLastActive = iCurrent;
    }
}
