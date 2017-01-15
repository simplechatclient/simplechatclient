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

#include <QDesktopWidget>
#include <QMenu>
#include <QTimer>
#include "core/core.h"
#include "models/channel.h"
#include "core/defines.h"
#include "models/find_nick.h"
#include "find_nick_gui.h"

FindNickGui::FindNickGui(QWidget *parent) : QDialog(parent)
{
    ui.setupUi(this);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowTitle(tr("Find nick"));
    // center screen
    move(QApplication::desktop()->screenGeometry(QApplication::desktop()->screenNumber(parent)).center()  - rect().center());

    createGui();
    createSignals();
}

void FindNickGui::createGui()
{
    ui.toolButton_options->setEnabled(false);

    ui.pushButton_search->setIcon(QIcon(":/images/breeze/edit-find.svg"));
    ui.pushButton_clear->setIcon(QIcon(":/images/breeze/draw-eraser.svg"));
    ui.toolButton_options->setIcon(QIcon(":/images/breeze/applications-system.svg"));
    ui.buttonBox->button(QDialogButtonBox::Close)->setIcon(QIcon(":/images/breeze/dialog-close.svg"));

    ui.label_find_nick->setText(tr("Find nick:"));
    ui.pushButton_search->setText(tr("Search"));
    ui.pushButton_clear->setText(tr("Clear"));

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

    //focus
    ui.lineEdit_search->setFocus();
}

void FindNickGui::createSignals()
{
    connect(ui.listWidget_nicks, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(itemClicked(QListWidgetItem*)));
    connect(ui.pushButton_search, SIGNAL(clicked()), this, SLOT(buttonFind()));
    connect(ui.pushButton_clear, SIGNAL(clicked()), this, SLOT(buttonClear()));
    connect(ui.buttonBox, SIGNAL(rejected()), this, SLOT(close()));
}

void FindNickGui::buttonFind()
{
    ui.lineEdit_search->setFocus();

    FindNick::instance()->clear();

    QString search = ui.lineEdit_search->text().trimmed();
    if (!search.isEmpty())
    {
        Core::instance()->network->send(QString("NS LIST %1 20").arg(search));
        refreshList();
    }
    else
        ui.listWidget_nicks->clear();
}

void FindNickGui::buttonClear()
{
    ui.lineEdit_search->setFocus();

    ui.lineEdit_search->clear();
    ui.listWidget_nicks->clear();
    ui.listWidget_nicks->setEnabled(false);
    ui.toolButton_options->setEnabled(false);
}

void FindNickGui::refreshList()
{
    // ready
    if (FindNick::instance()->getStatus() != StatusCompleted)
    {
        QTimer::singleShot(200, this, SLOT(refreshList())); // 0.2 sec
        return;
    }

    ui.listWidget_nicks->clear();

    QList<QString> list = FindNick::instance()->get();
    foreach (const QString &nick, list)
    {
        SortedListWidgetItem *item = new SortedListWidgetItem();
        item->setData(SortedListWidgetNicklistRole, false); // is nicklist
        item->setText(nick);

        ui.listWidget_nicks->addItem(item);
    }

    if (!list.isEmpty())
        ui.listWidget_nicks->setEnabled(true);
    else
        ui.listWidget_nicks->setEnabled(false);

    ui.toolButton_options->setEnabled(false);
}

void FindNickGui::itemClicked(QListWidgetItem *)
{
    if (!ui.toolButton_options->isEnabled())
        ui.toolButton_options->setEnabled(true);
}

void FindNickGui::priv()
{
    if (ui.listWidget_nicks->selectedItems().size() == 0)
        return;

    QString strNick = ui.listWidget_nicks->selectedItems().at(0)->text();

    Core::instance()->network->send(QString("PRIV %1").arg(strNick));
}

void FindNickGui::whois()
{
    if (ui.listWidget_nicks->selectedItems().size() == 0)
        return;

    QString strNick = ui.listWidget_nicks->selectedItems().at(0)->text();

    Core::instance()->network->send(QString("WHOIS %1 :%1").arg(strNick));
}

void FindNickGui::invite()
{
    if (ui.listWidget_nicks->selectedItems().size() == 0)
        return;

    QString strNick = ui.listWidget_nicks->selectedItems().at(0)->text();

    QAction *action = qobject_cast<QAction *>(sender());
    if (action)
    {
        QString strInviteChannel = action->data().toString();
        Core::instance()->network->send(QString("INVITE %1 %2").arg(strNick, strInviteChannel));
    }
}
