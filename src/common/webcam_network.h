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

#ifndef WEBCAM_NETWORK_H
#define WEBCAM_NETWORK_H

#include <QAbstractSocket>
#include <QObject>

QT_BEGIN_NAMESPACE
class QTcpSocket;
class QTimer;
class QTextCodec;
QT_END_NAMESPACE

class WebcamNetwork : public QObject
{
    Q_OBJECT
public:
    WebcamNetwork();
    virtual ~WebcamNetwork();
    inline void setBytesNeed(int _iBytesNeed) { iBytesNeed = _iBytesNeed; }
    inline void setBText(bool _bText) { bText = _bText; }
    inline void setLastKeepAlive(qint64 _iLastKeepAlive) { iLastKeepAlive = _iLastKeepAlive; }
    inline qint64 getLastKeepAlive() { return iLastKeepAlive; }
    inline void setReconnect(bool _bReconnecting) { bReconnecting = _bReconnecting; }

    void clearAll();
    bool isConnected();
    void networkConnect();
    void networkSend(const QString &strData);
    void networkSendb(const QByteArray &bData);
    void networkDisconnect();

private:
    QTcpSocket *socket;
    QString strData;
    QByteArray bData;
    int iBytesNeed;
    int iBytesRecv;
    QTimer *timerPingPong;
    qint64 iLastActive;
    qint64 iLastKeepAlive;
    bool bText;
    bool bReconnecting;
    QTextCodec *codec_iso8859_2;

private slots:
    void networkRead();
    void networkConnected();
    void networkDisconnected();
    void networkError(QAbstractSocket::SocketError);
    void slotNetworkConnect();
    void timeoutPingpong();

signals:
    void dataKernel(const QByteArray &bData);
    void textKernel(const QString &strData);
    void error(const QString &strError);
    void connected();
    void disconnected();
};

#endif // WEBCAM_NETWORK_H
