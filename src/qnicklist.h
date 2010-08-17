/****************************************************************************
 *                                                                          *
 *   This file is part of Simple Chat Client                                *
 *   Copyright (C) 2010 Piotr Łuczko <piotr.luczko@gmail.com>               *
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

#ifndef QNICKLIST_H
#define QNICKLIST_H

#include <QNetworkAccessManager>
#include <QNetworkReply>

#include <QContextMenuEvent>
#ifdef Q_WS_X11
#include <QDebug>
#endif
#include <QListWidget>
#include <QMenu>
#include <QQueue>
#include <QSettings>
#include "dlg_cam.h"
#include "dlg_kick.h"
#include "network.h"

struct NickStatus
{
    QString nick;
    QString status;
};
typedef QQueue <NickStatus> sNickStatus;


class Nicklist : public QListWidget
{
    Q_OBJECT
public:
    Nicklist(QWidget *, Network *, QSettings *, QString, QMap <QString, QByteArray> *);
    ~Nicklist();
    void set_open_channels(QStringList);
    void nicklist_add(QString, QString, int, sNickStatus *);
    void nicklist_remove(QString, sNickStatus *);
    bool nicklist_exist(QString, sNickStatus *);
    void nicklist_clear(sNickStatus *);
    QStringList nicklist_get(sNickStatus *);
    void nicklist_refresh(sNickStatus *);
    void nicklist_refresh_avatars();
    void update_avatar(QString, QByteArray);

private:
    QWidget *myparent;
    Network *pNetwork;
    QSettings *settings;
    QString strChannel;
    QStringList strOpenChannels;
    enum { maxOpenChannels = 50 };
    QAction *openChannelsActs[maxOpenChannels];
    QMap <QString, QByteArray> *mNickAvatar;

    void nicklist_sort(sNickStatus *);
    void nicklist_quicksort(QString, sNickStatus *, sNickStatus *);

private slots:
    void priv();
    void whois();
    void cam();
    void friends_add();
    void friends_del();
    void ignore_add();
    void ignore_del();
    void kick();
    void ban();
    void kban();
    void op_add();
    void op_del();
    void halfop_add();
    void halfop_del();
    void moderator_add();
    void moderator_del();
    void voice_add();
    void voice_del();
    void invite();

protected:
    virtual void contextMenuEvent(QContextMenuEvent *);

};

#endif // QNICKLIST_H
