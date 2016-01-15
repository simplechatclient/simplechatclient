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

#include <QDesktopWidget>
#include <QPushButton>
#include "user_avatar_gui.h"

UserAvatarGui::UserAvatarGui(const QPixmap &_avatar, QWidget *parent) : QDialog(parent), avatar(_avatar)
{
    ui.setupUi(this);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowTitle(tr("Avatar"));
    // center screen
    move(QApplication::desktop()->screenGeometry(QApplication::desktop()->screenNumber(parent)).center()  - rect().center());

    createGui();
    setDefaultValues();
    createSignals();
}

void UserAvatarGui::createGui()
{
    ui.toolButton_zoom_out->setIcon(QIcon(":/images/breeze/zoom-out.svg"));
    ui.toolButton_zoom_in->setIcon(QIcon(":/images/breeze/zoom-in.svg"));
    ui.buttonBox->button(QDialogButtonBox::Close)->setIcon(QIcon(":/images/breeze/dialog-close.svg"));
    ui.buttonBox->button(QDialogButtonBox::Close)->setText(tr("Close"));
}

void UserAvatarGui::setDefaultValues()
{
    // slider value 5
    ui.label_avatar->setPixmap(avatar.scaled(QSize(250,250), Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

void UserAvatarGui::createSignals()
{
    connect(ui.toolButton_zoom_out, SIGNAL(clicked()), this, SLOT(buttonZoomOut()));
    connect(ui.horizontalSlider, SIGNAL(valueChanged(int)), this, SLOT(sliderValueChanged(int)));
    connect(ui.toolButton_zoom_in, SIGNAL(clicked()), this, SLOT(buttonZoomIn()));
    connect(ui.buttonBox, SIGNAL(rejected()), this, SLOT(close()));
}

void UserAvatarGui::buttonZoomOut()
{
    int value = ui.horizontalSlider->value();
    value--;

    if (value >= ui.horizontalSlider->minimum())
        ui.horizontalSlider->setValue(value);
}

void UserAvatarGui::sliderValueChanged(int iValue)
{
    int iSize = 50;
    iSize *= iValue;

    ui.label_avatar->setPixmap(avatar.scaled(QSize(iSize,iSize), Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

void UserAvatarGui::buttonZoomIn()
{
    int value = ui.horizontalSlider->value();
    value++;

    if (value <= ui.horizontalSlider->maximum())
        ui.horizontalSlider->setValue(value);
}
