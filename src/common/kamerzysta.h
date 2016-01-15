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

#ifndef KAMERZYSTA_H
#define KAMERZYSTA_H

#include <QAbstractSocket>
#include <QObject>

QT_BEGIN_NAMESPACE
class QTcpSocket;
class QTimer;
QT_END_NAMESPACE

/**
 * Cams support using Kamerzysta (http://programy.onet.pl/72,34,10195,,,Kamerzysta_3.00.159,programy.html)
 */
class Kamerzysta : public QObject
{
    Q_OBJECT
public:
    Kamerzysta(QTcpSocket *_socket);
    void show(const QString &_strNick);
    void close();

private:
    QTcpSocket *socket;
    QString strNick;
    QString strAppPath;
    QString strKamerzystaFile;
    int iPort;
    int iTryGetPort;
    QTimer *timerGetPort;

    void log(const QString &strData);
    void getPath();
    void authorize();
    void invite(QString strInviteNick);
    void kamerzystaNotRunning();
    void kamerzystaRunning();

private slots:
    void getPort();
    void networkConnect();
    void networkConnected();
    void networkDisconnect();
    void networkDisconnected();
    void networkSend(const QString &strData);
    void networkRead();
    void error(QAbstractSocket::SocketError);
};

#endif // KAMERZYSTA_H
