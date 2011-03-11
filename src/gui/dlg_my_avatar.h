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

#ifndef DLG_MY_AVATAR_H
#define DLG_MY_AVATAR_H

class Network;
#include <QDialog>
#include <QNetworkAccessManager>
#include <QNetworkCookieJar>
#include "ui_my_avatar.h"

class DlgMyAvatar : public QDialog
{
    Q_OBJECT
public:
    DlgMyAvatar(QWidget *, Network *, QMap <QString, QByteArray> *);
    ~DlgMyAvatar();

private:
    Ui::uiMyAvatar ui;
    QWidget *myparent;
    Network *pNetwork;
    QMap <QString, QByteArray> *mNickAvatar;
    QNetworkAccessManager *accessManager;
    QNetworkCookieJar *cookieJar;
    // collections
    bool bReadedCollections;
    QMap<QString, int> mCollections; // name, id
    QMap< int, QMap<QString, QByteArray> > mCollectionAvatars; // id, map <link, avatar data>
    // my avatars
    QMap<QString, QString> mMyAvatarsID; // link, id
    QMap<QString, QByteArray> mMyAvatars; // link, avatar data

    void get_cookies();
    QString network_request(QString, QString);
    QByteArray get_avatar(QString);
    // my avatars
    void display_my_avatars();
    void load_my_avatars();
    void draw_my_avatars();
    // collections
    void get_collections();
    void draw_collections();
    // avatars from collect
    void get_avatars_from_collect(int);
    void draw_avatars_from_collect(int);

private slots:
    void refresh_avatar();
    void tab_changed(int);
    void collection_changed(QString);
    void button_apply_avatar();
    void button_remove_avatar();
    void button_set_empty_avatar();
    void button_apply_collection_avatar();
    void button_close();
};

#endif // DLG_MY_AVATAR_H
