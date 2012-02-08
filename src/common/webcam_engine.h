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

#ifndef WEBCAM_ENGINE_H
#define WEBCAM_ENGINE_H

#include <QObject>
#include <QTextCodec>

class WebcamNetwork;

class WebcamEngine : public QObject
{
    Q_OBJECT
public:
    WebcamEngine(QString, bool);
    virtual ~WebcamEngine();
    void closeEngine();

public slots:
    void networkSend(QString);
    void setUser(QString);

private:
    QString strNick;
    bool bMini;
    WebcamNetwork *pWebcamNetwork;

    QTextCodec *codec_cp1250;
    QString cp2unicode(const QByteArray &);

    void createSignals();
    int iCamCmd;
    // data
    void raw_202b(QByteArray&);
    void raw_250b(QByteArray&);
    void raw_251b(QByteArray&);
    void raw_252b(QByteArray&);
    void raw_254b(QByteArray&);
    void raw_403b(QByteArray&);
    // text
    void raw_200();
    void raw_202(QStringList&);
    void raw_211(QStringList&);
    void raw_221();
    void raw_231();
    void raw_232();
    void raw_233();
    void raw_250(QStringList&);
    void raw_251(QStringList&);
    void raw_252(QStringList&);
    void raw_253(QStringList&);
    void raw_254(QStringList&);
    void raw_264();
    void raw_261();
    void raw_262();
    void raw_263();
    void raw_267();
    void raw_268();
    void raw_403(QStringList&);
    void raw_405(QStringList&);
    void raw_406();
    void raw_408(QStringList&);
    void raw_410();
    void raw_411();
    void raw_412(QStringList&);
    void raw_413(QStringList&);
    void raw_418();
    void raw_501();
    void raw_502();
    void raw_504();
    void raw_508();
    void raw_515();
    void raw_520();

private slots:
    void connected();
    void disconnected();
    void dataKernel(QByteArray);
    void textKernel(QString);
    void slotError(QString);

signals:
    void updateImage(QByteArray);
    void updateText(QString);
    void updateRank(int);
    void updateStatus(QString);
    void userError(QString);
    void voteOk();
    void error(QString);
    void addUser(QString,int,QString);
    void updateUser(QString,int,QString);
    void removeUser(QString);
    void clearUsers();
};

#endif // WEBCAM_ENGINE_H
