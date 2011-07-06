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

#ifndef TAB_CONTAINER_H
#define TAB_CONTAINER_H

#include "defines.h"
class DlgChannelSettings;
class DlgModeration;
class DlgUserProfile;
class Network;
class TabManager;
class TabWidget;
#include <QMap>
#include <QObject>

/**
 * Class for displaying messages in tabs or change main window settings
 */
class TabContainer : public QObject
{
    Q_OBJECT
public:
    TabContainer(Network *, TabManager *);
    virtual ~TabContainer();
    void setDlg(DlgUserProfile *);
    bool existTab(QString);
    void addTab(QString);
    void removeTab(QString);
    bool renameTab(QString, QString);
    void partTab(int);
    void showMsg(QString &, QString &, QString &, MessageCategory);
    void showMsg(QString &, QString &, MessageCategory);
    void showMsgAll(QString &, MessageCategory);
    void showMsgActive(QString &, MessageCategory);
    void setTopic(QString &, QString &);
    void authorTopic(QString &, QString &);

public slots:
    void slotUpdateNickAvatar(QString);
    void slotUpdateChannelAvatar(QString);
    void slotShowMsg(QString &, QString &, MessageCategory);
    void slotShowMsgActive(QString &, MessageCategory);
    void slotShowMsgAll(QString &, MessageCategory);
    void slotDisplayMessage(QString &, QString &, MessageCategory);
    void slotChangeFontSize(QString);
    void slotClearContent(QString);
    void refreshColors();
    void refreshBackgroundImage();

private:
    // params
    Network *pNetwork;
    TabManager *pTabM;
    DlgChannelSettings *pDlgChannelSettings;
    DlgModeration *pDlgModeration;
    DlgUserProfile *pDlgUserProfile;
    // other
    QList<TabWidget*> tw;

    int getIndex(QString);

signals:
    void clearChannelAllNickAvatars(QString);
    void clearNicklist(QString);
    void updateNickAvatar(QString);
    void updateAwaylogStatus();
};

#endif // TAB_CONTAINER_H
