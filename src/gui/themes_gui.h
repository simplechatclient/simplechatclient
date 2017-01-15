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

#ifndef THEMES_GUI_H
#define THEMES_GUI_H

#include <QDialog>
#include "ui_themes.h"

class ThemesGui : public QDialog
{
    Q_OBJECT
public:
    ThemesGui(QWidget *parent = 0);

private:
    Ui::uiThemes ui;

    void createGui();
    void setDefaultValues();
    void createSignals();

private slots:
    void changeTheme(QModelIndex index);
};

#endif // THEMES_GUI_H
