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

#ifndef WEBCAM_ENGINE_H
#define WEBCAM_ENGINE_H

#include <QObject>
class WebcamNetwork;

QT_BEGIN_NAMESPACE
class QStringList;
class QTextCodec;
QT_END_NAMESPACE

class WebcamEngine : public QObject
{
    Q_OBJECT
public:
    WebcamEngine(const QString &_strNick, bool _bMini);
    virtual ~WebcamEngine();
    void closeEngine();

public slots:
    void networkSend(const QString &_strData);
    void setUser(const QString &_strNick);

private:
    QString strNick;
    bool bMini;
    WebcamNetwork *pWebcamNetwork;

    QTextCodec *codec_iso8859_2;
    QTextCodec *codec_cp1250;

    void createSignals();
    int iCamCmd;
    // data
    void raw_202b(const QByteArray &data);
    void raw_250b(const QByteArray &data);
    void raw_251b(const QByteArray &data);
    void raw_252b(const QByteArray &data);
    void raw_254b(const QByteArray &data);
    void raw_403b(const QByteArray &data);
    // text
    void raw_200();
    void raw_202(const QStringList &strDataList);
    void raw_211(const QStringList &strDataList);
    void raw_221();
    void raw_231();
    void raw_232();
    void raw_233();
    void raw_250(const QStringList &strDataList);
    void raw_251(const QStringList &strDataList);
    void raw_252(const QStringList &strDataList);
    void raw_253(const QStringList &strDataList);
    void raw_254(const QStringList &strDataList);
    void raw_264();
    void raw_261();
    void raw_262();
    void raw_263();
    void raw_267();
    void raw_268();
    void raw_403(const QStringList &strDataList);
    void raw_405(const QStringList &strDataList);
    void raw_406();
    void raw_408(const QStringList &strDataList);
    void raw_410();
    void raw_411();
    void raw_412(const QStringList &strDataList);
    void raw_413(const QStringList &strDataList);
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
    void dataKernel(const QByteArray &bData);
    void textKernel(const QString &strData);
    void slotError(const QString &strError);

signals:
    void updateImage(const QByteArray &data);
    void updateText(const QString &text);
    void updateRank(int iRank);
    void updateStatus(const QString &status);
    void userError(const QString &error);
    void voteOk();
    void error(const QString &error);
    void updateUser(const QString &strUser, int iSpectators, int iRank, int iCamOnOff, const QString &strUdget, const QStringList &lUserChannels);
    void updateUserCount(const QString &strUser, int iSpectators, int iRank);
    void clearUsers();
};

#endif // WEBCAM_ENGINE_H
