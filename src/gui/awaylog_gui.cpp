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

#include <QClipboard>
#include <QDesktopWidget>
#include <QPushButton>
#include "models/awaylog.h"
#include "core/core.h"
#include "awaylog_gui.h"

AwaylogGui::AwaylogGui(QWidget *parent) : QDialog(parent)
{
    ui.setupUi(this);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowTitle(tr("Awaylog"));
    // center screen
    move(QApplication::desktop()->screenGeometry(QApplication::desktop()->screenNumber(parent)).center()  - rect().center());

    createGui();
    createSignals();

    refresh();
}

void AwaylogGui::createGui()
{
    ui.pushButton_copy->setEnabled(false);

    ui.pushButton_clear->setIcon(QIcon(":/images/breeze/document-revert.svg"));
    ui.pushButton_copy->setIcon(QIcon(":/images/breeze/edit-copy.svg"));
    ui.pushButton_close->setIcon(QIcon(":/images/breeze/dialog-close.svg"));

    ui.pushButton_clear->setText(tr("Clear"));
    ui.pushButton_copy->setText(tr("Copy"));
    ui.pushButton_close->setText(tr("Close"));
}

void AwaylogGui::createSignals()
{
    connect(ui.listWidget, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(itemClicked(QListWidgetItem*)));
    connect(ui.pushButton_clear, SIGNAL(clicked()), this, SLOT(buttonClear()));
    connect(ui.pushButton_copy, SIGNAL(clicked()), this, SLOT(buttonCopy()));
    connect(ui.pushButton_close, SIGNAL(clicked()), this, SLOT(close()));
}

void AwaylogGui::refresh()
{
    QList<QString> list = Awaylog::instance()->get();
    foreach (const QString &strAwaylog, list)
        ui.listWidget->addItem(strAwaylog);
}

void AwaylogGui::buttonClear()
{
    Awaylog::instance()->clear();
    ui.listWidget->clear();
}

void AwaylogGui::buttonCopy()
{
    if (ui.listWidget->selectedItems().size() == 0)
        return;

    QString strText = ui.listWidget->selectedItems().at(0)->text();

    QApplication::clipboard()->setText(strText, QClipboard::Selection);
    QApplication::clipboard()->setText(strText, QClipboard::Clipboard);
}

void AwaylogGui::itemClicked(QListWidgetItem *)
{
    if (!ui.pushButton_copy->isEnabled())
        ui.pushButton_copy->setEnabled(true);
}
