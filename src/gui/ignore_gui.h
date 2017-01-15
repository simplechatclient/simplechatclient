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

#ifndef IGNORE_GUI_H
#define IGNORE_GUI_H

#include <QDialog>
#include "ui_ignore.h"

class IgnoreGui : public QDialog
{
    Q_OBJECT
public:
    IgnoreGui(QWidget *parent = 0);

private:
    Ui::uiIgnore ui;

    void createGui();
    void createSignals();

private slots:
    void refresh();
    void buttonAdd();
    void buttonRemove();
};

#endif // IGNORE_GUI_H
