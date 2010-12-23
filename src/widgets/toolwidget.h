/****************************************************************************
 *                                                                          *
 *   This file is part of Simple Chat Client                                *
 *   Copyright (C) 2010 Piotr Łuczko <piotr.luczko@gmail.com>               *
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

#ifndef TOOLWIDGET_H
#define TOOLWIDGET_H

#include <QComboBox>
#include <QHBoxLayout>
#include <QMenu>
#include <QPushButton>
#include <QSettings>
#include <QWidget>
#include "dlg_channel_settings.h"
#include "dlg_emoticons.h"
#include "dlg_moderation.h"


class ToolWidget : public QWidget
{
    Q_OBJECT
public:
    ToolWidget(QWidget *, Network *, InputWidget *, DlgChannelSettings *, DlgModeration *);
    void set_moderation(bool);
    void set_channel_settings(bool);
    void change_scroll(bool); // set scroll after change tab page

private:
    // params
    QWidget *myparent;
    Network *pNetwork;
    InputWidget *pInputWidget;
    DlgChannelSettings *dlgchannel_settings;
    DlgModeration *dlgmoderation;
    // other
    bool bMyBold;
    bool bMyItalic;
    QString strMyFontWeight;
    QString strMyFontStyle;
    QString strMyFontFamily;
    QString strCurrentColor;
    bool bScroll;

    QHBoxLayout *toolLayout;
    QPushButton *bold;
    QPushButton *italic;
    QPushButton *fontfamily;
    QComboBox *color;
    QPushButton *emoticons;
    QFrame *separator1;
    QPushButton *channel_settings;
    QPushButton *moderation;
    QPushButton *clear;
    QPushButton *scroll;
    //QFrame *separator2;

    QMenu *fontMenu;
    QAction *arialAct;
    QAction *timesAct;
    QAction *verdanaAct;
    QAction *tahomaAct;
    QAction *courierAct;

    QPushButton *size;
    QMenu *sizeMenu;
    QAction *size8Act;
    QAction *size9Act;
    QAction *size10Act;
    QAction *size11Act;
    QAction *size12Act;
    QAction *size14Act;
    QAction *size16Act;
    QAction *size18Act;
    QAction *size20Act;

    void set_default();

private slots:
    void bold_clicked();
    void italic_clicked();
    void arial_triggered();
    void times_triggered();
    void verdana_triggered();
    void tahoma_triggered();
    void courier_triggered();

    void size8_triggered();
    void size9_triggered();
    void size10_triggered();
    void size11_triggered();
    void size12_triggered();
    void size14_triggered();
    void size16_triggered();
    void size18_triggered();
    void size20_triggered();

    void color_clicked(int);
    void emoticons_clicked();
    void channel_settings_clicked();
    void moderation_clicked();
    void clear_clicked();
    void scroll_clicked();

signals:
    void change_font_size(QString);
    void clear_content(QString);
    void set_scroll(QString, bool);

};

#endif // TOOLWIDGET_H