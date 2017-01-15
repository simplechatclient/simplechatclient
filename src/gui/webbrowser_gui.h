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

#ifndef WEBBROWSER_GUI_H
#define WEBBROWSER_GUI_H

#include <QDialog>
#include "ui_webbrowser.h"

QT_BEGIN_NAMESPACE
class QUrl;
QT_END_NAMESPACE

class WebBrowserGui : public QDialog
{
    Q_OBJECT
public:
    WebBrowserGui(const QUrl &url, QWidget *parent = 0);
    virtual ~WebBrowserGui();

private:
    Ui::uiWebBrowser *ui;

private slots:
    void loadFinished();
};

#endif // WEBBROWSER_GUI_H
