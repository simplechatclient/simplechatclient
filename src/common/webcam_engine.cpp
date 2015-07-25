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
#include <QStringList>
#include <QTextCodec>

#include "models/settings.h"
#include "webcam_engine.h"
#include "webcam_network.h"

WebcamEngine::WebcamEngine(const QString &_strNick, bool _bMini) : strNick(_strNick), bMini(_bMini)
{
    codec_iso8859_2 = QTextCodec::codecForName("ISO-8859-2");
    codec_cp1250 = QTextCodec::codecForName("CP1250");

    pWebcamNetwork = new WebcamNetwork();

    createSignals();

    emit updateText(tr("Starting the service webcams"));

    pWebcamNetwork->networkConnect();
}

WebcamEngine::~WebcamEngine()
{
    delete pWebcamNetwork;
}

void WebcamEngine::createSignals()
{
    connect(pWebcamNetwork, SIGNAL(connected()), this, SLOT(connected()));
    connect(pWebcamNetwork, SIGNAL(dataKernel(const QByteArray&)), this, SLOT(dataKernel(const QByteArray&)));
    connect(pWebcamNetwork, SIGNAL(textKernel(const QString&)), this, SLOT(textKernel(const QString&)));
    connect(pWebcamNetwork, SIGNAL(error(const QString&)), this, SLOT(slotError(const QString&)));
}

void WebcamEngine::closeEngine()
{
    pWebcamNetwork->networkDisconnect();
}

void WebcamEngine::networkSend(const QString &_strData)
{
    pWebcamNetwork->networkSend(_strData);
}

void WebcamEngine::setUser(const QString &_strNick)
{
    strNick = _strNick;
}

/* from WebcamNetwork to WebcamGui */
void WebcamEngine::slotError(const QString &strError)
{
    emit error(strError);
}

void WebcamEngine::connected()
{
    emit updateText(tr("Connected to server webcam.")+"<br/>"+tr("Please wait ..."));

    QString strCAUTH = "1234567890123456";
    pWebcamNetwork->networkSend(QString("CAUTH %1 3.00.159").arg(strCAUTH));
}

void WebcamEngine::disconnected()
{
    emit updateText(tr("Disconnected from server webcams"));
}

void WebcamEngine::dataKernel(const QByteArray &bData)
{
    if (Settings::instance()->getBool("debug"))
        qDebug() << "CAM byte <- " << bData;

    switch (iCamCmd)
    {
        case 202:
            raw_202b(bData);
            break;
        case 250:
            raw_250b(bData);
            break;
        case 251:
            raw_251b(bData);
            break;
        case 252:
            raw_252b(bData);
            break;
        case 254:
            raw_254b(bData);
            break;
        case 403:
            raw_403b(bData);
            break;
    }

    iCamCmd = 0;
}

/*
202 14283 IMAGE_UPDATE_BIG psotnica2603
*/
void WebcamEngine::raw_202b(const QByteArray &data)
{
    emit updateImage(data);
}

/*
250 4892 OK
A_R_B:1:5/0/#DEUTSCHLAND/0,3/0/#EROTICA/0,3/0/#BDSM_BDSM/0:0::0
aaa__sia:1:3/0/#n_o_g_i/0:15:0123:0

ja31:-
osa1987:1:-2/-2/osa1987/1:0::0
ToWiemTylkoJa:1:2/0/#Relax/0,-2/-2/ToWiemTylkoJa/1:2::13
scc_test:1:2/0/#Quiz/0,2/0/#Relax/0,2/0/#Scrabble/0,4/0/#scc/0:0::0
*/
/* multiline */
void WebcamEngine::raw_250b(const QByteArray &data)
{
    // init data
    QString strData = codec_iso8859_2->toUnicode(data);

    // is empty
    if (strData.isEmpty())
        return;

    QStringList strDataList = strData.split("\n");

    /* clear users */
    emit clearUsers();

    foreach (QString strLine, strDataList)
    {
        QStringList strLineList = strLine.split(":");
        if (strLineList.size() == 6)
        {
            QString strUser = strLineList.at(0);
            int iCamOnOff = strLineList.at(1).toInt(); // 1 = on; 2 = off
            QString strChannelsParams = strLineList.at(2);
            int iSpectators = strLineList.at(3).toInt();
            QString strUdget = strLineList.at(4); // udget (012345)
            int iRank = strLineList.at(5).toInt(); // -500 to 500

            // NO_SUCH_USER / Spectators and Rank unknown
            if ((strLineList.at(3) == "-") && (strLineList.at(5) == "-"))
                iCamOnOff = 0; // off

            QStringList lUserChannels;
            if (!strChannelsParams.isEmpty())
            {
                // 2/0/#Quiz/0,2/0/#Relax/0,2/0/#Scrabble/0,4/0/#scc/0
                QStringList lChannelsParams = strChannelsParams.split(",");
                foreach (QString strChannelParams, lChannelsParams)
                {
                    QStringList lChannelParams = strChannelParams.split("/");
                    if (lChannelParams.size() == 4)
                    {
                        int iChannelCategory = lChannelParams.at(0).toInt();
                        int iUnknown = lChannelParams.at(1).toInt(); // always 0 or -2
                        QString strChannelName = lChannelParams.at(2);
                        int iCamPubPriv = lChannelParams.at(3).toInt(); // 0 = public; 1 = private

                        Q_UNUSED(iChannelCategory);
                        Q_UNUSED(iUnknown);
                        Q_UNUSED(iCamPubPriv);

                        lUserChannels.append(strChannelName);
                    }
                }
            }

            emit updateUser(strUser, iSpectators, iRank, iCamOnOff, strUdget, lUserChannels);
        }
    }

    emit updateText(tr("Select user"));
}

/*
251 57 UPDATE
kriss89:0:5/0/#Pozna./0,2/0/#sex/0,2/0/#Towarzyski/0:0::0

ja31:-
osa1987:1:-2/-2/osa1987/1:0::0
ToWiemTylkoJa:1:2/0/#Relax/0,-2/-2/ToWiemTylkoJa/1:2::13
scc_test:1:2/0/#Quiz/0,2/0/#Relax/0,2/0/#Scrabble/0,4/0/#scc/0:0::0
*/
void WebcamEngine::raw_251b(const QByteArray &data)
{
    QString strLine = codec_iso8859_2->toUnicode(data);

    // is empty
    if (strLine.isEmpty())
        return;

    QStringList strLineList = strLine.split(":");

    // invalid parameters
    if (strLineList.size() != 6)
        return;

    QString strUser = strLineList.at(0);
    int iCamOnOff = strLineList.at(1).toInt(); // 1 = on; 2 = off
    QString strChannelsParams = strLineList.at(2);
    int iSpectators = strLineList.at(3).toInt();
    QString strUdget = strLineList.at(4); // udget (012345)
    int iRank = strLineList.at(5).toInt(); // -500 to 500

    // NO_SUCH_USER / Spectators and Rank unknown
    if ((strLineList.at(3) == "-") && (strLineList.at(5) == "-"))
        iCamOnOff = 0; // off

    QStringList lUserChannels;
    if (!strChannelsParams.isEmpty())
    {
        QStringList lChannelsParams = strChannelsParams.split(",");
        foreach (QString strChannelParams, lChannelsParams)
        {
            QStringList lChannelParams = strChannelParams.split("/");
            if (lChannelParams.size() == 4)
            {
                int iChannelCategory = lChannelParams.at(0).toInt();
                int iUnknown = lChannelParams.at(1).toInt(); // always 0 or -2
                QString strChannelName = lChannelParams.at(2);
                int iCamPubPriv = lChannelParams.at(3).toInt(); // 0 = public; 1 = private

                Q_UNUSED(iChannelCategory);
                Q_UNUSED(iUnknown);
                Q_UNUSED(iCamPubPriv);

                lUserChannels.append(strChannelName);
            }
        }
    }

    emit updateUser(strUser, iSpectators, iRank, iCamOnOff, strUdget, lUserChannels);

    if ((strUser == strNick) && (iCamOnOff == 0))
        emit userError(tr("No such user"));
}

/*
SETSTATUS //panda
*/
void WebcamEngine::raw_252b(const QByteArray &data)
{
    if (data.indexOf("SETSTATUS ") == 0)
    {
        QString strStatus = codec_cp1250->toUnicode(data.mid(10));

        if (Settings::instance()->getBool("debug"))
            qDebug() << "CAM <- SETSTATUS " << strStatus;

        emit updateStatus(strStatus);
    }
}

/*
254 519 USER_COUNT_UPDATE
scc_test 2 0
Aniolek___ 12 0
bluemn 1 0
Bluesky 2 0
*/
/* multi-line */
void WebcamEngine::raw_254b(const QByteArray &data)
{
    QString strData = codec_iso8859_2->toUnicode(data);

    // is empty
    if (strData.isEmpty())
        return;

    QStringList strDataList = strData.split("\n");

    foreach (QString strLine, strDataList)
    {
        QStringList strLineList = strLine.split(" ");
        if (strLineList.size() == 3) // is correct ?
        {
            QString strUser = strLineList.at(0);
            int iSpectators = strLineList.at(1).toInt();
            int iRank = strLineList.at(2).toInt();

            emit updateUserCount(strUser, iSpectators, iRank);
        }
    }
}

/*
Invalid key
*/
void WebcamEngine::raw_403b(const QByteArray &data)
{
    QString strError = data;

    if (Settings::instance()->getBool("debug"))
        qDebug() << "CAM <- " << strError;

    emit error(strError);
}

void WebcamEngine::textKernel(const QString &strData)
{
    if (strData.isEmpty()) return;

    if (Settings::instance()->getBool("debug"))
        qDebug() << "CAM <- " << strData;

    QStringList strDataList = strData.split(" ");
    int cmd = strDataList.at(0).toInt();

    switch (cmd)
    {
        case 200:
            raw_200();
            break;
        case 202:
            raw_202(strDataList);
            break;
        case 211:
            raw_211(strDataList);
            break;
        case 221:
            raw_221();
            break;
        case 231:
            raw_231();
            break;
        case 232:
            raw_232();
            break;
        case 233:
            raw_233();
            break;
        case 250:
            raw_250(strDataList);
            break;
        case 251:
            raw_251(strDataList);
            break;
        case 252:
            raw_252(strDataList);
            break;
        case 253:
            raw_253(strDataList);
            break;
        case 254:
            raw_254(strDataList);
            break;
        case 264:
            raw_264();
            break;
        case 261:
            raw_261();
            break;
        case 262:
            raw_262();
            break;
        case 263:
            raw_263();
            break;
        case 267:
            raw_267();
            break;
        case 268:
            raw_268();
            break;
        case 403:
            raw_403(strDataList);
            break;
        case 405:
            raw_405(strDataList);
            break;
        case 406:
            raw_406();
            break;
        case 408:
            raw_408(strDataList);
            break;
        case 410:
            raw_410();
            break;
        case 411:
            raw_411();
            break;
        case 412:
            raw_412(strDataList);
            break;
        case 413:
            raw_413(strDataList);
            break;
        case 418:
            raw_418();
            break;
        case 501:
            raw_501();
            break;
        case 502:
            raw_502();
            break;
        case 504:
            raw_504();
            break;
        case 508:
            raw_508();
            break;
        case 515:
            raw_515();
            break;
        case 520:
            raw_520();
            break;
        default:
            if (Settings::instance()->getBool("debug"))
                qDebug() << "Unknown CAM RAW:" << strData;
            break;
    }
}

/*
200 0 OK
*/
void WebcamEngine::raw_200()
{
    pWebcamNetwork->networkSend(QString("SUBSCRIBE_BIG * %1").arg(strNick));
}

/*
202 17244 IMAGE_UPDATE_BIG Ekscentryk
*/
void WebcamEngine::raw_202(const QStringList &strDataList)
{
    if (strDataList.size() < 4)
        return;

    QString strUser = strDataList.at(3);

    pWebcamNetwork->setBytesNeed(strDataList.at(1).toInt());
    if (strDataList.at(1).toInt() != 0)
    {
        // re-send
        if (strUser == strNick)
        {
            qint64 iCurrentTime = QDateTime::currentMSecsSinceEpoch();
            pWebcamNetwork->setLastKeepAlive(iCurrentTime);

            pWebcamNetwork->networkSend(QString("KEEPALIVE_BIG %1").arg(strNick));
        }

        pWebcamNetwork->setBText(false);
        if (strUser == strNick)
            iCamCmd = 202;
        else
            iCamCmd = 0;
    }
    else
    {
        if (strUser == strNick)
            emit userError(tr("This user does not send data"));
    }
}

/*
211 19995 Noemi_01@0
211 13584 Noemi_01@1
211 13584 Noemi_01@2
211 13584 Noemi_01@3
211 29 my_dwoje@4
211 17 myszka29brunetka@5
*/
void WebcamEngine::raw_211(const QStringList &strDataList)
{
    if (strDataList.size() < 3)
        return;

    if (strDataList.at(1).toInt() > 0)
    {
        pWebcamNetwork->setBText(false);
        pWebcamNetwork->setBytesNeed(strDataList.at(1).toInt());
        iCamCmd = 211;
    }
}

/*
221 0 UDPUT_OK
*/
void WebcamEngine::raw_221()
{
}

/*
231 0 OK scc_test
*/
void WebcamEngine::raw_231()
{
}

/*
232 0 CMODE 0
*/
void WebcamEngine::raw_232()
{
}

/*
233 0 QUALITY_FACTOR 1
*/
void WebcamEngine::raw_233()
{
}

/*
250 12519 OK
*/
void WebcamEngine::raw_250(const QStringList &strDataList)
{
    if (strDataList.size() < 3)
        return;

    // initial read users status
    pWebcamNetwork->setBText(false);
    pWebcamNetwork->setBytesNeed(strDataList.at(1).toInt());
    iCamCmd = 250;
}

/*
251 52 UPDATE
*/
void WebcamEngine::raw_251(const QStringList &strDataList)
{
    if (strDataList.size() < 3)
        return;

    pWebcamNetwork->setBText(false);
    pWebcamNetwork->setBytesNeed(strDataList.at(1).toInt());
    iCamCmd = 251;
}

/*
252 41 USER_STATUS pati28ash
*/
void WebcamEngine::raw_252(const QStringList &strDataList)
{
    if (strDataList.size() < 4)
        return;

    if (strDataList.at(1).toInt() > 0)
    {
        pWebcamNetwork->setBytesNeed(strDataList.at(1).toInt());
        pWebcamNetwork->setBText(false);

        QString strUser = strDataList.at(3);
        if (strUser == strNick)
            iCamCmd = 252;
        else
            iCamCmd = 0;
    }
}

/*
253 0 USER_VOTES Delikatna 38
*/
void WebcamEngine::raw_253(const QStringList &strDataList)
{
    if (strDataList.size() < 5)
        return;

    QString strUser = strDataList.at(3);
    int iRank = strDataList.at(4).toInt();

    if (strUser == strNick) // is current nick
        emit updateRank(iRank);
}

/*
254 1489 USER_COUNT_UPDATE
*/
void WebcamEngine::raw_254(const QStringList &strDataList)
{
    if (strDataList.size() < 3)
        return;

    pWebcamNetwork->setBText(false);
    pWebcamNetwork->setBytesNeed(strDataList.at(1).toInt());
    iCamCmd = 254;

    // check keepalive
    qint64 iCurrentTime = QDateTime::currentMSecsSinceEpoch();

    if (iCurrentTime - pWebcamNetwork->getLastKeepAlive() > 30000) // 30 sec
    {
        if (!strNick.isEmpty())
            pWebcamNetwork->networkSend(QString("KEEPALIVE_BIG %1").arg(strNick));
    }
}

/*
261 0 OK
*/
void WebcamEngine::raw_261()
{
    /* stopped */
}

/*
264 0 CODE_ACCEPTED ffffffff 2147483647
*/
/* successfully logged in */
void WebcamEngine::raw_264()
{
    emit updateText(tr("Setting mode for viewing only"));
    pWebcamNetwork->networkSend("SENDMODE 0");

    // if mini get user image
    if (bMini)
        pWebcamNetwork->networkSend(QString("SUBSCRIBE_BIG * %1").arg(strNick));
}

/*
262 0 NEW_FAN ~matka_wariatka
*/
void WebcamEngine::raw_262()
{
}

/*
263 0 VOTE_OK alicja17 3
*/
void WebcamEngine::raw_263()
{
    emit voteOk();
}

/*
267 0 SENDMODE=0
267 0 SENDMODE=1
*/
void WebcamEngine::raw_267()
{
}

/*
268 0 OK
*/
void WebcamEngine::raw_268()
{
    // CAUTH ok
    QString strUOKey = Settings::instance()->get("uo_key");
    pWebcamNetwork->networkSend(QString("AUTH %1 3.00.159").arg(strUOKey));
}

/*
403 11 ACCESS_DENIED
Invalid key
*/
void WebcamEngine::raw_403(const QStringList &strDataList)
{
    if (strDataList.size() < 3)
        return;

    pWebcamNetwork->setBText(false);
    pWebcamNetwork->setBytesNeed(strDataList.at(1).toInt());
    iCamCmd = 403;
}

/*
405 0 USER_GONE Restonka
*/
void WebcamEngine::raw_405(const QStringList &strDataList)
{
    if (strDataList.size() < 4)
        return;

    QString strUser = strDataList.at(3);
    if (strUser == strNick)
        emit userError(tr("User gone"));
}

/*
406 0 NO_SUCH_USER_UDGET 0@4
*/
void WebcamEngine::raw_406()
{
}

/*
408 0 NO_SUCH_USER_SUBSCRIBE LOLexx
*/
void WebcamEngine::raw_408(const QStringList &strDataList)
{
    if (strDataList.size() < 4)
        return;

    QString strUser = strDataList.at(3);
    if (strUser == strNick)
        emit userError(tr("The specified user does not have a webcam enabled"));
}

/*
410 0 FAN_GONE Merovingian
*/
void WebcamEngine::raw_410()
{
}

/*
411 0 VOTE_DENIED soneja
*/
void WebcamEngine::raw_411()
{
}

/*
412 0 SUBSCRIBE_FAILED olgusia32
*/
void WebcamEngine::raw_412(const QStringList &strDataList)
{
    if (strDataList.size() < 4)
        return;

    QString strUser = strDataList.at(3);
    if (strUser == strNick)
        emit userError(tr("Failed to retrieve the image from the webcam"));
}

/*
413 0 SUBSCRIBE_DENIED aliina
*/
void WebcamEngine::raw_413(const QStringList &strDataList)
{
    if (strDataList.size() < 4)
        return;

    QString strUser = strDataList.at(3);
    if (strUser == strNick)
        emit userError(tr("Failed to retrieve the image from the webcam"));
}

/*
418 0 QUIT_CZAT
*/
void WebcamEngine::raw_418()
{
    pWebcamNetwork->networkDisconnect();
}

/*
501 0 INVALID_USER_DATA_NUM
*/
/* udget with -1 */
void WebcamEngine::raw_501()
{
}

/*
502 0 SESSION_TIMED_OUT
*/
void WebcamEngine::raw_502()
{
    pWebcamNetwork->networkDisconnect();
}

/*
504 0 UNKNOWN_COMMAND PUT2
*/
void WebcamEngine::raw_504()
{
}

/*
508 0 SESSION_OVERRIDEN
*/
void WebcamEngine::raw_508()
{
}

/*
515 0 USER_DATA_TOO_LARGE
*/
void WebcamEngine::raw_515()
{
}

/*
520 0 INVALID_UOKEY 1q3j0llVg40cu2784j9EVoz8sRdfNl3w
*/
void WebcamEngine::raw_520()
{
    emit error(tr("Invalid authorization key"));
    pWebcamNetwork->networkDisconnect();
}
