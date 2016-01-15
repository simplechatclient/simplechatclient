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

#ifndef REGISTER_NICK_GUI_H
#define REGISTER_NICK_GUI_H

#include <QDialog>
#include "ui_register_nick.h"
class ProfileAddGui;

QT_BEGIN_NAMESPACE
class QNetworkAccessManager;
class QNetworkCookieJar;
class QNetworkReply;
QT_END_NAMESPACE

class RegisterNickGui : public QDialog
{
    Q_OBJECT
public:
    RegisterNickGui(ProfileAddGui *_pProfileAddGui, QWidget *parent = 0);
    virtual ~RegisterNickGui();

private:
    enum RegisterType
    {
        RT_img,
        RT_cookies,
        RT_register
    };

    Ui::uiRegisterNick ui;
    ProfileAddGui *pProfileAddGui;
    QNetworkAccessManager *accessManager;
    QNetworkCookieJar *cookieJar;
    QHash<QString,QString> mCookies;

    void createGui();
    void createSignals();

    void getCookies();
    void gotCookies();
    void getImg();
    void gotImg(const QByteArray &bData);
    void registerNick();
    void parseResult(const QString &strResult);

private slots:
    void networkFinished(QNetworkReply *reply);
    void buttonRefresh();
    void buttonOk();
};

#endif // REGISTER_NICK_GUI_H
