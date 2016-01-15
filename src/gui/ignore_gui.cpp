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
#include <QInputDialog>
#include <QTimer>
#include "core/core.h"
#include "models/ignore.h"
#include "ignore_gui.h"

IgnoreGui::IgnoreGui(QWidget *parent) : QDialog(parent)
{
    ui.setupUi(this);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowTitle(tr("Ignore list"));
    // center screen
    move(QApplication::desktop()->screenGeometry(QApplication::desktop()->screenNumber(parent)).center()  - rect().center());

    createGui();
    createSignals();

    refresh();
}

void IgnoreGui::createGui()
{
    ui.pushButton_add->setIcon(QIcon(":/images/breeze/list-add-user.svg"));
    ui.pushButton_remove->setIcon(QIcon(":/images/breeze/list-remove-user.svg"));
    ui.buttonBox->button(QDialogButtonBox::Close)->setIcon(QIcon(":/images/breeze/dialog-close.svg"));

    ui.pushButton_add->setText(tr("Add"));
    ui.pushButton_remove->setText(tr("Remove"));
    ui.buttonBox->button(QDialogButtonBox::Close)->setText(tr("Close"));
}

void IgnoreGui::createSignals()
{
    connect(ui.pushButton_add, SIGNAL(clicked()), this, SLOT(buttonAdd()));
    connect(ui.pushButton_remove, SIGNAL(clicked()), this, SLOT(buttonRemove()));
    connect(ui.buttonBox, SIGNAL(rejected()), this, SLOT(close()));
}

void IgnoreGui::refresh()
{
    ui.listWidget_nicks->clear();

    QList<QString> ignoreList = Ignore::instance()->getAll();
    foreach (const QString &strNick, ignoreList)
    {
        SortedListWidgetItem *item = new SortedListWidgetItem();
        item->setData(SortedListWidgetNicklistRole, false); // is nicklist
        item->setText(strNick);
        item->setIcon(QIcon(":/images/breeze/meeting-attending-tentative.svg"));

        ui.listWidget_nicks->addItem(item);
    }
}

void IgnoreGui::buttonAdd()
{
    bool ok;
    QString strText = QInputDialog::getText(this, tr("Change your ignore list"), tr("Enter a nickname to be added:"), QLineEdit::Normal, QString::null, &ok);
    strText = strText.trimmed();

    if ((ok) && (!strText.isEmpty()))
    {
        Core::instance()->network->send(QString("NS IGNORE ADD %1").arg(strText));
        QTimer::singleShot(1000*4, this, SLOT(refresh())); // 4 sec
    }
}

void IgnoreGui::buttonRemove()
{
    QString strSelected;
    if (ui.listWidget_nicks->selectedItems().size() != 0)
        strSelected = ui.listWidget_nicks->selectedItems().at(0)->text();

    bool ok;
    QString strText = QInputDialog::getText(this, tr("Change your ignore list"), tr("Enter a nickname for removal:"), QLineEdit::Normal, strSelected, &ok);
    strText = strText.trimmed();

    if ((ok) && (!strText.isEmpty()))
    {
        Core::instance()->network->send(QString("NS IGNORE DEL %1").arg(strText));
        QTimer::singleShot(1000*4, this, SLOT(refresh())); // 4 sec
    }
}
