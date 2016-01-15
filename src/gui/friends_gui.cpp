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
#include <QMenu>
#include <QTimer>
#include "core/core.h"
#include "models/channel.h"
#include "models/friends.h"
#include "friends_gui.h"

FriendsGui::FriendsGui(QWidget *parent) : QDialog(parent)
{
    ui.setupUi(this);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowTitle(tr("Friends list"));
    // center screen
    move(QApplication::desktop()->screenGeometry(QApplication::desktop()->screenNumber(parent)).center()  - rect().center());

    createGui();
    createSignals();

    refresh();
}

void FriendsGui::createGui()
{
    ui.toolButton_options->setEnabled(false);

    ui.pushButton_add->setIcon(QIcon(":/images/breeze/list-add-user.svg"));
    ui.pushButton_remove->setIcon(QIcon(":/images/breeze/list-remove-user.svg"));
    ui.toolButton_options->setIcon(QIcon(":/images/breeze/applications-system.svg"));
    ui.buttonBox->button(QDialogButtonBox::Close)->setIcon(QIcon(":/images/breeze/dialog-close.svg"));

    ui.pushButton_add->setText(tr("Add"));
    ui.pushButton_remove->setText(tr("Remove"));
    ui.toolButton_options->setText(tr("Options"));
    ui.buttonBox->button(QDialogButtonBox::Close)->setText(tr("Close"));

    QMenu *optionsMenu = new QMenu(this);
    optionsMenu->addAction(QIcon(":/images/breeze/list-add-user.svg"), tr("Priv"), this, SLOT(priv()));
    optionsMenu->addAction(QIcon(":/images/breeze/user-properties.svg"), tr("Whois"), this, SLOT(whois()));

    QMenu *mInvite = new QMenu(tr("Invite"));
    mInvite->setIcon(QIcon(":/images/breeze/legalmoves.svg"));

    QList<CaseIgnoreString> lChannelsCleared = Channel::instance()->getListClearedSorted();
    for (int i = 0; i < lChannelsCleared.size(); ++i)
    {
        QString strOpenChannel = lChannelsCleared.at(i);
        if (strOpenChannel.at(0) == '^')
            strOpenChannel = Channel::instance()->getAlternativeName(strOpenChannel);

        openChannelsActs[i] = new QAction(this);
        openChannelsActs[i]->setIcon(QIcon(":/images/breeze/irc-join-channel.svg"));
        openChannelsActs[i]->setVisible(false);
        openChannelsActs[i]->setText(strOpenChannel);
        openChannelsActs[i]->setData(lChannelsCleared.at(i));
        openChannelsActs[i]->setVisible(true);

        connect(openChannelsActs[i], SIGNAL(triggered()), this, SLOT(invite()));
        mInvite->addAction(openChannelsActs[i]);
    }
    optionsMenu->addMenu(mInvite);

    ui.toolButton_options->setMenu(optionsMenu);
}

void FriendsGui::createSignals()
{
    connect(ui.tabWidget, SIGNAL(currentChanged(int)), this, SLOT(tabChanged(int)));
    connect(ui.listWidget_online, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(itemClicked(QListWidgetItem*)));
    connect(ui.pushButton_add, SIGNAL(clicked()), this, SLOT(buttonAdd()));
    connect(ui.pushButton_remove, SIGNAL(clicked()), this, SLOT(buttonRemove()));
    connect(ui.buttonBox, SIGNAL(rejected()), this, SLOT(close()));
}

void FriendsGui::refresh()
{
    ui.listWidget_online->clear();
    ui.listWidget_offline->clear();

    QHashIterator <QString, bool> it(Friends::instance()->getAll());
    while (it.hasNext())
    {
        it.next();
        SortedListWidgetItem *item = new SortedListWidgetItem();
        item->setData(SortedListWidgetNicklistRole, false); // is nicklist
        item->setText(it.key());
        item->setIcon(QIcon(":/images/breeze/meeting-attending.svg"));

        if (it.value())
            ui.listWidget_online->addItem(item);
        else
            ui.listWidget_offline->addItem(item);
    }
}

void FriendsGui::tabChanged(int index)
{
    // online - show options
    if ((index == 0) && (ui.toolButton_options->isHidden()))
        ui.toolButton_options->setHidden(false);
    // offline - hide options
    else if ((index == 1) && (!ui.toolButton_options->isHidden()))
        ui.toolButton_options->setHidden(true);
}

void FriendsGui::itemClicked(QListWidgetItem *)
{
    if (!ui.toolButton_options->isEnabled())
        ui.toolButton_options->setEnabled(true);
}

void FriendsGui::buttonAdd()
{
    bool ok;
    QString strText = QInputDialog::getText(this, tr("Changing your friends list"), tr("Enter a nickname to be added:"), QLineEdit::Normal, QString::null, &ok);
    strText = strText.trimmed();

    if ((ok) && (!strText.isEmpty()))
    {
        Core::instance()->network->send(QString("NS FRIENDS ADD %1").arg(strText));
        QTimer::singleShot(1000*4, this, SLOT(refresh())); // 4 sec
    }
}

void FriendsGui::buttonRemove()
{
    QString strSelected;
    if (ui.tabWidget->currentIndex() == 0)
    {
        if (ui.listWidget_online->selectedItems().size() != 0)
            strSelected = ui.listWidget_online->selectedItems().at(0)->text();
    }
    else if (ui.tabWidget->currentIndex() == 1)
    {
        if (ui.listWidget_offline->selectedItems().size() != 0)
            strSelected = ui.listWidget_offline->selectedItems().at(0)->text();
    }

    bool ok;
    QString strText = QInputDialog::getText(this, tr("Changing your friends list"), tr("Enter a nickname for removal:"), QLineEdit::Normal, strSelected, &ok);
    strText = strText.trimmed();

    if ((ok) && (!strText.isEmpty()))
    {
        Core::instance()->network->send(QString("NS FRIENDS DEL %1").arg(strText));
        QTimer::singleShot(1000*4, this, SLOT(refresh())); // 4 sec
    }
}

void FriendsGui::priv()
{
    QString strSelected;
    if (ui.tabWidget->currentIndex() == 0)
    {
        if (ui.listWidget_online->selectedItems().size() != 0)
            strSelected = ui.listWidget_online->selectedItems().at(0)->text();
    }
    else if (ui.tabWidget->currentIndex() == 1)
    {
        if (ui.listWidget_offline->selectedItems().size() != 0)
            strSelected = ui.listWidget_offline->selectedItems().at(0)->text();
    }

    if (!strSelected.isEmpty())
        Core::instance()->network->send(QString("PRIV %1").arg(strSelected));
}

void FriendsGui::whois()
{
    QString strSelected;
    if (ui.tabWidget->currentIndex() == 0)
    {
        if (ui.listWidget_online->selectedItems().size() != 0)
            strSelected = ui.listWidget_online->selectedItems().at(0)->text();
    }
    else if (ui.tabWidget->currentIndex() == 1)
    {
        if (ui.listWidget_offline->selectedItems().size() != 0)
            strSelected = ui.listWidget_offline->selectedItems().at(0)->text();
    }

    if (!strSelected.isEmpty())
        Core::instance()->network->send(QString("WHOIS %1 :%1").arg(strSelected));
}

void FriendsGui::invite()
{
    QString strSelected;
    if (ui.tabWidget->currentIndex() == 0)
    {
        if (ui.listWidget_online->selectedItems().size() != 0)
            strSelected = ui.listWidget_online->selectedItems().at(0)->text();
    }
    else if (ui.tabWidget->currentIndex() == 1)
    {
        if (ui.listWidget_offline->selectedItems().size() != 0)
            strSelected = ui.listWidget_offline->selectedItems().at(0)->text();
    }

    if (!strSelected.isEmpty())
    {
        QAction *action = qobject_cast<QAction *>(sender());
        if (action)
        {
            QString strInviteChannel = action->data().toString();
            Core::instance()->network->send(QString("INVITE %1 %2").arg(strSelected, strInviteChannel));
        }
    }
}
