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

#ifndef MODERATION_GUI_H
#define MODERATION_GUI_H

#include <QDialog>
#include "ui_moderation.h"

class ModerationGui : public QDialog
{
    Q_OBJECT
public:
    ModerationGui(const QString &_strCurrentChannel, QWidget *parent = 0);

private:
    Ui::uiModeration ui;
    QString strCurrentChannel;
    enum ModerationRoles
    {
        ModerationIdRole = Qt::UserRole,
        ModerationChannelRole = Qt::UserRole+1,
        ModerationTimeRole = Qt::UserRole+2,
        ModerationNickRole = Qt::UserRole+3,
        ModerationMessageRole = Qt::UserRole+4
    };

    void createGui();
    void createSignals();

    void removeSelected();

private slots:
    void refreshMessages();
    void buttonAccept();
    void buttonRemove();
};

#endif // MODERATION_GUI_H
