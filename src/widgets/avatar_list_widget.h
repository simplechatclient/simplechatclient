/*
 * Simple Chat Client
 *
 *   Copyright (C) 2012-2016 Dariusz Markowicz <dmarkowicz@alari.pl>
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

#ifndef AVATAR_LIST_WIDGET_H
#define AVATAR_LIST_WIDGET_H

#include <QWidget>

#include "onet/onet_avatar.h"
#include "models/my_avatar.h"

#include "ui_avatar_list_widget.h"

class AvatarListWidget : public QWidget
{
    Q_OBJECT
    
public:
    typedef QMap<QString, int> AvatarCollections;

    AvatarListWidget(QWidget *parent = 0);

    void initialize(OnetAvatar *_onetAvatar);
    bool isInitialized() const;

signals:
    void avatarSelected(const QString &avatarUrl);

private:
    Ui::AvatarListWidget ui;

    OnetAvatar *onetAvatar;
    AvatarCollections collections; // name, id
    enum AvatarRoles
    {
        AvatarUrlRole = Qt::UserRole
    };

    void createGui();
    void createSignals();

    void drawMyAvatar(const QPixmap &pixmap, const QString &avatarUrl);
    void drawAvatarFromCollection(const QPixmap &pixmap, const QString &avatarUrl);

    class MyAvatarListWidgetItem : public QListWidgetItem
    {
    public:
        MyAvatarListWidgetItem(const MyAvatar &avatar) : avatar(avatar) {}
        MyAvatar avatar;
    };

private slots:
    void getCollectionsReady(const QByteArray &content);
    void getCollectionAvatarsReady(const QByteArray &content);
    void getMyAvatarsReady(const QByteArray &content);
    void uploadImageReady(const QByteArray &content, const QString &fileName);
    void updatePhotoReady(const QByteArray &content);
    void addPhotoReady(const QByteArray &content);
    void deletePhotoReady(const QByteArray &content);
    void getAvatarReady(const QByteArray &content, const QString &avatarUrl, OnetAvatar::AvatarType type);

    void tabChanged(int index);
    void collectionChanged(QString strName);
    void addAvatarClicked();
    void editAvatarClicked();
    void setMyAvatarClicked();
    void removeAvatarClicked();
    void setEmptyAvatarClicked();
    void setCollectionAvatarClicked();
};

#endif // AVATAR_LIST_WIDGET_H
