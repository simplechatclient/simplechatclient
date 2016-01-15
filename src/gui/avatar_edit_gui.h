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

#ifndef AVATAR_EDIT_GUI_H
#define AVATAR_EDIT_GUI_H

#include <QDialog>

#include "onet/onet_avatar.h"
#include "widgets/avatar_edit_scene.h"
#include "models/my_avatar.h"
#include "ui_avatar_edit.h"

class AvatarEditGui : public QDialog
{
    Q_OBJECT
public:
    AvatarEditGui(QWidget *parent, MyAvatar avatar, OnetAvatar *onetAvatar);
    ~AvatarEditGui();
    MyAvatar getAvatar() const;

public slots:
    void getAvatarReady(const QByteArray &content, const QString &avatarUrl, OnetAvatar::AvatarType type);
    void cropChanged(const QRect &crop);

private:
    Ui::uiAvatarEdit ui;
    MyAvatar avatar;
    AvatarEditScene *editScene;
    QGraphicsScene *previewScene;
    QGraphicsPixmapItem *cropItem;

    OnetAvatar *onetAvatar;

    QRect crop;
    int angle;
    QPixmap photo;

    void createGui();
    void setDefaultValues();
    void createSignals();

private slots:
    void rotateLeftClicked();
    void rotateRightClicked();
    void okClicked();

};

#endif // AVATAR_EDIT_GUI_H
