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

#ifndef DLG_IGNORE_H
#define DLG_IGNORE_H

#include <QCloseEvent>
#include <QDialog>
#include <QShowEvent>
#include "dlg_ignore_ad.h"
#include "network.h"
#include "ui_ignore.h"

class DlgIgnore : public QDialog
{
    Q_OBJECT
public:
    DlgIgnore(QWidget *, Network *, QSettings *, QMap <QString, QByteArray> *);
    void add_ignore(QString);
    void clear();

private:
    Ui::uiIgnore ui;
    QWidget *myparent;
    Network *pNetwork;
    QSettings *settings;
    QMap <QString, QByteArray> *mNickAvatar;

private slots:
    void button_add();
    void button_remove();
    void button_ok();
    void button_cancel();

protected:
    virtual void showEvent(QShowEvent *);
    virtual void closeEvent(QCloseEvent *);

};

#endif // DLG_IGNORE_H