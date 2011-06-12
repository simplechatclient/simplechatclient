/****************************************************************************
 *                                                                          *
 *   This file is part of Simple Chat Client                                *
 *   Copyright (C) 2011 Piotr Łuczko <piotr.luczko@gmail.com>               *
 *                                                                          *
 *   This program is free software: you can redistribute it and/or modify   *
 *   it under the terms of the GNU General Public License as published by   *
 *   the Free Software Foundation, either version 3 of the License, or      *
 *   (at your option) any later version.                                    *
 *                                                                          *
 *   This program is distributed in the hope that it will be useful,        *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *   GNU General Public License for more details.                           *
 *                                                                          *
 *   You should have received a copy of the GNU General Public License      *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 *                                                                          *
 ****************************************************************************/

#ifndef INPUTLINEDOCKWIDGET_H
#define INPUTLINEDOCKWIDGET_H

#include "defines.h"
class DlgChannelSettings;
class DlgModeration;
class InputWidget;
class Network;
class ToolWidget;
#include <QWidget>

class InputLineDockWidget : public QWidget
{
    Q_OBJECT
public:
    InputLineDockWidget(QWidget *, Network *, DlgChannelSettings *, DlgModeration *);
    virtual ~InputLineDockWidget();
    void enable_moderation();
    void disable_moderation();
    void hide_toolwidget();
    void show_toolwidget();
    void hide_channel_settings();
    void show_channel_settings();
    void set_active(QString);
    QString get_active();
    void update_nick_list();

public slots:
    void slot_update_nick(QString);
    void slot_display_message(QString&, QString&, MessageCategory);
    void slot_show_msg(QString&, QString&, MessageCategory);
    void slot_show_hide_toolwidget();
    void slot_change_font_size(QString);
    void slot_clear_content(QString);
    void slot_ctrl_tab_pressed();
    void slot_ctrl_shift_tab_pressed();

private:
    // params
    Network *pNetwork;
    DlgChannelSettings *pDlgChannelSettings;
    DlgModeration *pDlgModeration;
    // other
    InputWidget *pInputWidget;
    ToolWidget *pToolWidget;

signals:
    void display_message(QString&,QString&,MessageCategory);
    void show_msg(QString&,QString&,MessageCategory);
    void change_font_size(QString);
    void clear_content(QString);
    void ctrlTabPressed();
    void ctrlShiftTabPressed();
};

#endif // INPUTLINEDOCKWIDGET_H
