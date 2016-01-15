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

#ifndef FRIENDS_GUI_H
#define FRIENDS_GUI_H

#include <QDialog>
#include "ui_friends.h"
#include "core/defines.h"

class FriendsGui : public QDialog
{
    Q_OBJECT
public:
    FriendsGui(QWidget *parent = 0);

private:
    Ui::uiFriends ui;
    QAction *openChannelsActs[MAX_OPEN_CHANNELS];

    void createGui();
    void createSignals();

private slots:
    void refresh();
    void tabChanged(int index);
    void itemClicked(QListWidgetItem*);
    void buttonAdd();
    void buttonRemove();
    void priv();
    void whois();
    void invite();
};

#endif // FRIENDS_GUI_H
