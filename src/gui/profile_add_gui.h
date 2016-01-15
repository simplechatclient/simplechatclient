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

#ifndef PROFILE_ADD_GUI_H
#define PROFILE_ADD_GUI_H

#include <QDialog>
#include "ui_profile_add.h"
class ProfileManagerGui;

class ProfileAddGui : public QDialog
{
    Q_OBJECT
public:
    ProfileAddGui(ProfileManagerGui *_profileManager, QWidget *parent = 0);
    void setNickPass(const QString &n, const QString &p);

private:
    Ui::uiProfileAdd ui;
    ProfileManagerGui *profileManager;

    void createGui();
    void setDefaultValues();
    void createSignals();

private slots:
    void hidePass();
    void showPass();
    void buttonRegisterNick();
    void buttonOk();
};

#endif // PROFILE_ADD_GUI_H
