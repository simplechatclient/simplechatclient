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

#ifndef PROFILE_MANAGER_GUI_H
#define PROFILE_MANAGER_GUI_H

#include <QDialog>
#include "ui_profile_manager.h"
class OptionsGui;

class ProfileManagerGui : public QDialog
{
    Q_OBJECT
public:
    ProfileManagerGui(OptionsGui *_pOptionsGui, QWidget *parent = 0);
    void refreshAllLists();
    bool existProfile(const QString &strExistProfile);

private:
    Ui::uiProfileManager ui;
    OptionsGui *pOptionsGui;
    QString path;

    void createGui();
    void setDefaultValues();
    void createSignals();
    void createPath();
    void refreshProfilesList();
    bool removeProfileDirectory(const QString &dirName);

private slots:
    void itemClicked();
    void addProfile();
    void editProfile();
    void removeProfile();
};

#endif // PROFILE_MANAGER_GUI_H
