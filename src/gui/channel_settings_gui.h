/*
 * Simple Chat Client
 *
 *   Copyright (C) 2009-2017 Piotr Łuczko <piotr.luczko@gmail.com>
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

#ifndef CHANNEL_SETTINGS_GUI_H
#define CHANNEL_SETTINGS_GUI_H

#include <QDialog>
#include "ui_channel_settings.h"
class OnetAvatar;
class SimpleStatsWidget;
class SimplePercentageWidget;

class ChannelSettingsGui : public QDialog
{
    Q_OBJECT
public:
    ChannelSettingsGui(const QString &_strChannel, QWidget *parent = 0);
    ~ChannelSettingsGui();

private:
    Ui::uiChannelSettings ui;
    QString strChannel;
    SimplePercentageWidget *pSimplePercentageWidget;
    SimpleStatsWidget *pSimpleStatsWidget;
    OnetAvatar *onetAvatar;
    QString avatarUrl;
    enum BanRoles
    {
        OryginalBanMaskRole = Qt::UserRole
    };

    void createGui();
    void setDefaultValues();
    void createSignals();

    void refreshAll();
    void setSettingsTabsStatus(bool b);
    void setOwner(const QString &strNick);
    void addOp(const QString &strNick);
    void addHalfop(const QString &strNick);
    void addBan(const QString &strNick, const QString &strWho, const QString &strDT, const QString &strIPNick);
    void addInvite(const QString &strNick, const QString &strWho, const QString &strDT);
    void clear();

private slots:
    void changePage(QModelIndex modelIndex);
    void tabChangePage(int index);
    void refreshChannelInfo();
    void refreshChannelStats();
    void ownerChanged();
    void removeChannelClicked();
    void wwwChanged();
    void topicChanged();
    void descChanged();
    void statusPub();
    void statusPriv();
    void categoryChanged(int index);
    void guardianInactive();
    void guardianActive();
    void guardianClicked(int iLevel);
    void passwordChanged();
    void kickRejoinChanged();
    void limitChanged();
    void moderatedInactive();
    void moderatedActive();
    void auditoriumInactive();
    void auditoriumActive();

    void buttonPermissionAdd();
    void buttonPermissionRemove();
    void refreshPermissionList();
    void buttonClose();

    void avatarSelected(const QString &avatarUrl);
    void getAvatarReady(const QByteArray &content, const QString &avatarUrl, OnetAvatar::AvatarType type);
    void refreshAvatar();
};

#endif // CHANNEL_SETTINGS_GUI_H
