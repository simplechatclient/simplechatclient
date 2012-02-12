/****************************************************************************
 *                                                                          *
 *   This file is part of Simple Chat Client                                *
 *   Copyright (C) 2012 Piotr Łuczko <piotr.luczko@gmail.com>               *
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

#ifndef CAPTCHA_H
#define CAPTCHA_H

class MainWindow;
class QNetworkAccessManager;
class QNetworkCookieJar;
class QNetworkReply;
#include <QDialog>
#include "ui_captcha.h"

class DlgCaptcha : public QDialog
{
    Q_OBJECT
public:
    DlgCaptcha(MainWindow *, QNetworkCookieJar *, QString *);

private:
    Ui::uiCaptcha ui;
    QString *strCaptcha;
    QNetworkAccessManager *accessManager;
    QNetworkCookieJar *cookieJar;

private:
    void createGui();
    void createSignals();
    void getImg();
    void gotImg(QByteArray);

private slots:
    void networkFinished(QNetworkReply *);
    void buttonOk();
};

#endif // CAPTCHA_H
