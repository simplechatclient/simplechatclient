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

#include <QDateTime>
#include <QDesktopWidget>
#include <QTimer>
#include "core/core.h"
#include "models/channel.h"
#include "models/message.h"
#include "models/settings.h"
#include "moderation_gui.h"

ModerationGui::ModerationGui(const QString &_strCurrentChannel, QWidget *parent) : QDialog(parent), strCurrentChannel(_strCurrentChannel)
{
    ui.setupUi(this);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowTitle(tr("Moderation"));
    // center screen
    move(QApplication::desktop()->screenGeometry(QApplication::desktop()->screenNumber(parent)).center()  - rect().center());

    createGui();
    createSignals();

    // refresh
    QTimer::singleShot(200, this, SLOT(refreshMessages())); // 0.2 sec
}

void ModerationGui::createGui()
{
    ui.pushButton_accept->setIcon(QIcon(":/images/breeze/list-add.svg"));
    ui.pushButton_remove->setIcon(QIcon(":/images/breeze/list-remove.svg"));
    ui.buttonBox->button(QDialogButtonBox::Close)->setIcon(QIcon(":/images/breeze/dialog-close.svg"));

    ui.pushButton_accept->setText(tr("Accept"));
    ui.pushButton_remove->setText(tr("Remove"));
    ui.buttonBox->button(QDialogButtonBox::Close)->setText(tr("Close"));

    ui.label_channel->setText(strCurrentChannel);
}

void ModerationGui::createSignals()
{
    connect(ui.pushButton_accept, SIGNAL(clicked()), this, SLOT(buttonAccept()));
    connect(ui.pushButton_remove, SIGNAL(clicked()), this, SLOT(buttonRemove()));
    connect(ui.buttonBox, SIGNAL(rejected()), this, SLOT(close()));
}

void ModerationGui::refreshMessages()
{
    QList<OnetModerateMessage> lModerateMessages = Channel::instance()->getModerateMessages(strCurrentChannel);
    int listWidgetCount = ui.listWidget_msg->count();
    int moderateMessagesCount = lModerateMessages.size();

    for (int i = listWidgetCount; i < listWidgetCount + (moderateMessagesCount - listWidgetCount); ++i)
    {
        OnetModerateMessage oModerateMessage = lModerateMessages.at(i);

        QString strID = oModerateMessage.id;
        qint64 iTime = oModerateMessage.datetime;
        QString strNick = oModerateMessage.nick;
        QString strMessage = oModerateMessage.message;

        QString strData = QString("[%1] <%2> %3").arg(QDateTime::fromMSecsSinceEpoch(iTime).toString("hh:mm:ss"), strNick, strMessage);

        QListWidgetItem *item = new QListWidgetItem();
        item->setData(ModerationIdRole, strID);
        item->setData(ModerationChannelRole, strCurrentChannel);
        item->setData(ModerationTimeRole, iTime);
        item->setData(ModerationNickRole, strNick);
        item->setData(ModerationMessageRole, strMessage);
        item->setText(strData);

        ui.listWidget_msg->addItem(item);
    }

    // refresh
    QTimer::singleShot(1000*1, this, SLOT(refreshMessages())); // 1 sec
}

void ModerationGui::removeSelected()
{
    if (ui.listWidget_msg->selectedItems().isEmpty())
        return;

    QList<QListWidgetItem*> list = ui.listWidget_msg->selectedItems();

    // remove from list
    foreach (QListWidgetItem *item, list)
    {
        QString strChannel = item->data(ModerationChannelRole).toString();
        QString strID = item->data(ModerationIdRole).toString();

        Channel::instance()->removeModerateMessage(strChannel, strID);

        ui.listWidget_msg->takeItem(ui.listWidget_msg->row(item));
    }
}

void ModerationGui::buttonAccept()
{
    if (ui.listWidget_msg->selectedItems().isEmpty())
        return;

    QList<QListWidgetItem*> list = ui.listWidget_msg->selectedItems();

    // display
    foreach (QListWidgetItem *item, list)
    {
        QString strChannel = item->data(ModerationChannelRole).toString();
        qint64 iTime = item->data(ModerationTimeRole).toLongLong();
        QString strNick = item->data(ModerationNickRole).toString();
        QString strMessage = item->data(ModerationMessageRole).toString();

        QString strSend = QString("MODERMSG %1 - %2 :%3").arg(strNick, strChannel, strMessage);
        Core::instance()->network->send(strSend);

        QString strMe = Settings::instance()->get("nick");
        QString strDisplay = QString("%1 [%2 %3]").arg(strMessage, tr("Moderated by"), strMe);
        Message::instance()->showMessage(strChannel, strDisplay, MessageDefault, strNick, iTime);
    }

    // remove
    removeSelected();
}

void ModerationGui::buttonRemove()
{
    // remove
    removeSelected();
}
