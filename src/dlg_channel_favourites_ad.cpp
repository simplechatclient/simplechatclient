/****************************************************************************
 *                                                                          *
 *   This file is part of Simple Chat Client                                *
 *   Copyright (C) 2010 Piotr �uczko <piotr.luczko@gmail.com>               *
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

#include "dlg_channel_favourites_ad.h"

DlgChannelFavouritesAd::DlgChannelFavouritesAd(Network *param1, QSettings *param2, TabContainer *param3, QString param4, QString param5)
{
    ui.setupUi(this);

    pNetwork = param1;
    settings = param2;
    tabc = param3;
    strStatus = param4;
    ui.lineEdit->setText(param5);

    if (strStatus == "add")
        ui.label->setText("Podaj nazw� nowego kana�u do ulubionych:");
    else
        ui.label->setText("Podaj nazw� kana�u do usuni�cia z ulubionych:");

    QObject::connect(ui.buttonBox, SIGNAL(accepted()), this, SLOT(button_ok()));
    QObject::connect(ui.buttonBox, SIGNAL(rejected()), this, SLOT(button_cancel()));
}

void DlgChannelFavouritesAd::button_ok()
{
    QString strChannel = ui.lineEdit->text();

    if (strChannel.isEmpty() == false)
    {
        if (strStatus == "add")
            pNetwork->send(QString("NS FAVOURITES ADD %1").arg(strChannel));
        else
            pNetwork->send(QString("NS FAVOURITES DEL %1").arg(strChannel));
    }

    pNetwork->send("NS FAVOURITES");
    ui.buttonBox->QObject::disconnect();
    this->close();
}

void DlgChannelFavouritesAd::button_cancel()
{
    pNetwork->send("NS FAVOURITES");
    ui.buttonBox->QObject::disconnect();
    this->close();
}
