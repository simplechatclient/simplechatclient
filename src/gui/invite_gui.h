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

#ifndef INVITE_GUI_H
#define INVITE_GUI_H

#include <QDialog>
#include "ui_invite.h"

class InviteGui : public QDialog
{
    Q_OBJECT
public:
    InviteGui(const QString &_strId, qint64 _iDateTime, const QString &_strNick, const QString &_strChannel, QWidget *parent = 0);

private:
    Ui::uiInvite ui;
    QString strId;
    qint64 iDateTime;
    QString strNick;
    QString strChannel;

    void createGui();
    void createSignals();

private slots:
    void buttonAccept();
    void buttonReject();
    void buttonIgnore();
    void whois();
};

#endif // INVITE_GUI_H
