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

#include "webcam_mini_gui.h"
#include "webcam_standard_gui.h"
#include "common/webcam_engine.h"
#include "webcam_gui.h"

WebcamGui::WebcamGui(QString _strNick, bool _bMini) : strNick(_strNick), bMini(_bMini)
{
    if (bMini)
    {
        pWebcamMini = new WebcamMiniGui(strNick);
        pWebcamMini->show();
    }
    else
    {
        pWebcamStandard = new WebcamStandardGui();
        pWebcamStandard->show();
    }

    pWebcamEngine = new WebcamEngine(strNick, bMini);

    createSignals();
}

WebcamGui::~WebcamGui()
{
    delete pWebcamEngine;

    if (bMini)
        delete pWebcamMini;
    else
        delete pWebcamStandard;
}

void WebcamGui::createSignals()
{
    if (bMini)
    {
        connect(pWebcamEngine, SIGNAL(updateImage(const QByteArray&)), pWebcamMini, SLOT(updateImage(const QByteArray&)));
        connect(pWebcamEngine, SIGNAL(updateText(const QString&)), pWebcamMini, SLOT(updateText(const QString&)));
        connect(pWebcamEngine, SIGNAL(userError(const QString&)), this, SLOT(userError(const QString&)));
        connect(pWebcamEngine, SIGNAL(error(const QString&)), pWebcamMini, SLOT(error(const QString&)));
        connect(pWebcamEngine, SIGNAL(updateUserCount(const QString&,int,int)), pWebcamMini, SLOT(updateUserCount(const QString&,int,int)));

        connect(pWebcamMini, SIGNAL(closeCam()), this, SLOT(closeCam()));
    }
    else
    {
        connect(pWebcamEngine, SIGNAL(updateImage(const QByteArray&)), pWebcamStandard, SLOT(updateImage(const QByteArray&)));
        connect(pWebcamEngine, SIGNAL(updateText(const QString&)), pWebcamStandard, SLOT(updateText(const QString&)));
        connect(pWebcamEngine, SIGNAL(updateRank(int)), pWebcamStandard, SLOT(updateRank(int)));
        connect(pWebcamEngine, SIGNAL(updateStatus(const QString&)), pWebcamStandard, SLOT(updateStatus(const QString&)));
        connect(pWebcamEngine, SIGNAL(voteOk()), pWebcamStandard, SLOT(voteOk()));
        connect(pWebcamEngine, SIGNAL(userError(const QString&)), pWebcamStandard, SLOT(userError(const QString&)));
        connect(pWebcamEngine, SIGNAL(error(const QString&)), pWebcamStandard, SLOT(error(const QString&)));
        connect(pWebcamEngine, SIGNAL(updateUser(const QString&,int,int,int,const QString&, QStringList)), pWebcamStandard, SLOT(updateUser(const QString&,int,int,int,const QString&, const QStringList&)));
        connect(pWebcamEngine, SIGNAL(updateUserCount(const QString&,int,int)), pWebcamStandard, SLOT(updateUserCount(const QString&,int,int)));
        connect(pWebcamEngine, SIGNAL(clearUsers()), pWebcamStandard, SLOT(clearUsers()));

        connect(pWebcamStandard, SIGNAL(closeCam()), this, SLOT(closeCam()));
        connect(pWebcamStandard, SIGNAL(networkSend(const QString&)), pWebcamEngine, SLOT(networkSend(const QString&)));
        connect(pWebcamStandard, SIGNAL(setUser(const QString&)), pWebcamEngine, SLOT(setUser(const QString&)));
    }
}

void WebcamGui::closeCam()
{
    pWebcamEngine->closeEngine();
    this->deleteLater();
}

void WebcamGui::userError(const QString &s)
{
    /* mini */
    pWebcamMini->updateText(s);
    pWebcamEngine->closeEngine();
}
