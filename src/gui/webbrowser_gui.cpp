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

#include <QDesktopWidget>
#include <QPushButton>
#include <QUrl>
#include "webbrowser_gui.h"

WebBrowserGui::WebBrowserGui(const QUrl &url, QWidget *parent) : QDialog(parent), ui(new Ui::uiWebBrowser)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose); // require by show method - prevent hangup!
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowTitle("YouTube");
    // center screen
    move(QApplication::desktop()->screenGeometry(QApplication::desktop()->screenNumber(parent)).center()  - rect().center());

    ui->webView->settings()->setAttribute(QWebSettings::PluginsEnabled, true);
    ui->webView->settings()->setAttribute(QWebSettings::PrivateBrowsingEnabled, true);

    ui->buttonBox->button(QDialogButtonBox::Close)->setIcon(QIcon(":/images/breeze/dialog-close.svg"));
    ui->buttonBox->button(QDialogButtonBox::Close)->setText(tr("Close"));

    // signals
    connect(ui->webView, SIGNAL(loadFinished(bool)), this, SLOT(loadFinished()));
    connect(ui->buttonBox, SIGNAL(rejected()), this, SLOT(close()));

    // load
    ui->webView->load(url);
}

WebBrowserGui::~WebBrowserGui()
{
    ui->webView->stop();
    ui->webView->disconnect();
    delete ui->webView->page();
    delete ui->webView;
    delete ui;
}

void WebBrowserGui::loadFinished()
{
    setWindowTitle(ui->webView->title());
}

