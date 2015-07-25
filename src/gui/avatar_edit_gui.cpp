/*
 * Simple Chat Client
 *
 *   Copyright (C) 2012-2015 Dariusz Markowicz <dmarkowicz@alari.pl>
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

#include <QDesktopWidget>
#include <QPushButton>
#include <QDebug>
#include <QGraphicsPixmapItem>

#include "onet/onet_avatar.h"
#include "widgets/avatar_edit_scene.h"
#include "models/my_avatar.h"
#include "models/settings.h"

#include "avatar_edit_gui.h"

AvatarEditGui::AvatarEditGui(QWidget *parent, MyAvatar avatar, OnetAvatar *_onetAvatar) :
    QDialog(parent), avatar(avatar), onetAvatar(_onetAvatar)
{
    //avatar.debug("AvatarEditGui");

    ui.setupUi(this);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowTitle(tr("Edit Avatar"));
    // center screen
    move(QApplication::desktop()->screenGeometry(QApplication::desktop()->screenNumber(parent)).center()  - rect().center());

    Q_ASSERT(onetAvatar);

    angle = avatar.angle();

    createGui();
    setDefaultValues();
    createSignals();

    connect(onetAvatar, SIGNAL(getAvatarReady(const QByteArray &,const QString &,OnetAvatar::AvatarType)), this, SLOT(getAvatarReady(const QByteArray &,const QString &,OnetAvatar::AvatarType)));
    connect(editScene, SIGNAL(cropChanged(const QRect &)), this, SLOT(cropChanged(const QRect &)));

    onetAvatar->requestGetAvatar(avatar.getRawUrl(), OnetAvatar::AT_myRaw);
}

AvatarEditGui::~AvatarEditGui()
{
    delete editScene;
    delete previewScene;
}

void AvatarEditGui::createGui()
{
    ui.buttonBox->button(QDialogButtonBox::Ok)->setIcon(QIcon(":/images/breeze/dialog-ok.svg"));
    ui.buttonBox->button(QDialogButtonBox::Cancel)->setIcon(QIcon(":/images/breeze/dialog-cancel.svg"));
    ui.toolButton_rotate_left->setIcon(QIcon(":/images/breeze/object_rotate_left.svg"));
    ui.toolButton_rotate_right->setIcon(QIcon(":/images/breeze/object_rotate_right.svg"));

    ui.label_fileName_title->setText(tr("Filename:"));
    ui.label_size_title->setText(tr("Size:"));
    ui.buttonBox->button(QDialogButtonBox::Ok)->setText(tr("Ok"));
    ui.buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Cancel"));

    editScene = new AvatarEditScene(this);
    ui.graphicsView_edit->setScene(editScene);
    previewScene = new QGraphicsScene(this);
    cropItem = previewScene->addPixmap(QPixmap());
    ui.graphicsView_preview->setScene(previewScene);
}

void AvatarEditGui::setDefaultValues()
{
    ui.label_fileName->setText(avatar.desc());
    ui.label_size->setText(QString("%1x%2 px").arg(QString::number(avatar.width()), QString::number(avatar.height())));
}

void AvatarEditGui::createSignals()
{
    connect(ui.buttonBox, SIGNAL(accepted()), this, SLOT(okClicked()));
    connect(ui.buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    connect(ui.toolButton_rotate_left, SIGNAL(clicked()), this, SLOT(rotateLeftClicked()));
    connect(ui.toolButton_rotate_right, SIGNAL(clicked()), this, SLOT(rotateRightClicked()));
}

void AvatarEditGui::rotateLeftClicked()
{
    avatar.setCrop(MyAvatar::scaledCropToString(crop, photo.size(), avatar.size(), avatar.angle(), true));
    avatar.rotateLeft();
    onetAvatar->requestGetAvatar(avatar.getRawUrl(), OnetAvatar::AT_myRaw);
}

void AvatarEditGui::rotateRightClicked()
{
    avatar.setCrop(MyAvatar::scaledCropToString(crop, photo.size(), avatar.size(), avatar.angle(), true));
    avatar.rotateRight();
    onetAvatar->requestGetAvatar(avatar.getRawUrl(), OnetAvatar::AT_myRaw);
}

void AvatarEditGui::getAvatarReady(const QByteArray &content, const QString &avatarUrl, OnetAvatar::AvatarType type)
{
    if (type != OnetAvatar::AT_myRaw)
        return;

    if (!photo.loadFromData(content))
    {
        if (Settings::instance()->getBool("debug"))
            qWarning() << "Unable to load image from " << avatarUrl;
        return;
    }

    if (avatar.crop().isEmpty())
    {
        int cropSize = qMin(photo.width(), photo.height());
        crop.setRect(0, 0, cropSize, cropSize);
    }
    else
    {
        crop = MyAvatar::stringToScaledCrop(avatar.crop(), photo.size(), avatar.size(), avatar.angle(), angle != avatar.angle());
    }
    angle = avatar.angle();

    editScene->setPhoto(photo, crop);
    previewScene->setSceneRect(photo.rect());
    cropItem->setPixmap(photo);
    ui.graphicsView_preview->fitInView(crop);
}

void AvatarEditGui::cropChanged(const QRect &crop)
{
    this->crop = crop;
    ui.graphicsView_preview->fitInView(crop);
}

MyAvatar AvatarEditGui::getAvatar() const
{
    return avatar;
}

void AvatarEditGui::okClicked()
{
    avatar.setCrop(MyAvatar::scaledCropToString(crop, photo.size(), avatar.size(), avatar.angle()));
    accept();
}

