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

#include <QDateTime>
#include <QDesktopWidget>
#include <QTimer>
#include "core/core.h"
#include "models/offline.h"
#include "models/settings.h"
#include "offline_message_gui.h"

OfflineMessageGui::OfflineMessageGui(const QString &_strNick, QWidget *parent) : QDialog(parent), strNick(_strNick), messagesQuoted(false), messagesReplied(false), messagesQuotedToSender(false)
{
    ui.setupUi(this);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowTitle(QString(tr("Offline messages from %1")).arg(strNick));
    // center screen
    move(QApplication::desktop()->screenGeometry(QApplication::desktop()->screenNumber(parent)).center()  - rect().center());

    createGui();
    createSignals();

    refreshMessages();
}

void OfflineMessageGui::createGui()
{
    ui.pushButton_reply->setIcon(QIcon(":/images/breeze/mail-send.svg"));

    ui.pushButton_reply->setText(tr("Reply"));
}

void OfflineMessageGui::createSignals()
{
    connect(ui.pushButton_reply, SIGNAL(clicked()), this, SLOT(buttonReply()));
}

void OfflineMessageGui::refreshMessages()
{
    QList<OnetOfflineMessage> lOfflineMessages = Offline::instance()->getMessagesReverted(strNick);

    int listWidgetCount = ui.listWidget->count();
    int moderateMessagesCount = lOfflineMessages.size();

    for (int i = listWidgetCount; i < listWidgetCount + (moderateMessagesCount - listWidgetCount); ++i)
    {
        OnetOfflineMessage oOfflineMessage = lOfflineMessages.at(i);

        qint64 iTime = oOfflineMessage.datetime;
        QString strType = oOfflineMessage.type;
        QString strMessage = oOfflineMessage.message;

        QDateTime dDateTime = QDateTime::fromMSecsSinceEpoch(iTime);
        QString strDate = dDateTime.toString("dd MMM yyyy");
        QString strTime = dDateTime.toString("[hh:mm:ss]");

        QListWidgetItem *item = new QListWidgetItem();
        item->setToolTip(strDate);

        if (strType == "quote")
        {
            if (!messagesQuoted)
            {
                messagesQuoted = true;

                QString strDisplay = QString(tr("You sent offline messages to %1:").arg(strNick));
                ui.listWidget->addItem(strDisplay);
            }

            QString strMe = Settings::instance()->get("nick");

            strMessage = QString("%1 <%2> %3").arg(strTime, strMe, strMessage);
            item->setText(strMessage);

            ui.listWidget->addItem(item);
        }
        else if (strType == "reply")
        {
            if (!messagesReplied)
            {
                messagesReplied = true;

                QString strDisplay = QString(tr("User %1 replied:").arg(strNick));
                ui.listWidget->addItem(strDisplay);
            }
            strMessage = QString("%1 <%2> %3").arg(strTime, strNick, strMessage);
            item->setText(strMessage);

            ui.listWidget->addItem(item);
        }
        else if (strType == "msg")
        {
            strMessage = QString("%1 <%2> %3").arg(strTime, strNick, strMessage);
            item->setText(strMessage);

            ui.listWidget->addItem(item);
        }
    }

    // refresh
    QTimer::singleShot(1000*1, this, SLOT(refreshMessages())); // 1 sec
}

void OfflineMessageGui::buttonReply()
{
    QString strMessage = ui.lineEdit->text().trimmed();
    if (strMessage.isEmpty())
        return;

    // quote
    if (!messagesQuotedToSender)
    {
        messagesQuotedToSender = true;

        Core::instance()->network->send(QString("NS OFFLINE QUOTE %1").arg(strNick));
    }

    // reply
    Core::instance()->network->send(QString("NS OFFLINE REPLY %1 %2").arg(strNick, strMessage));

    // clear
    ui.lineEdit->clear();

    // show
    QString strMe = Settings::instance()->get("nick");
    QString strDT = QDateTime::currentDateTime().toString("[hh:mm:ss]");
    strMessage = QString("%1 <%2> %3").arg(strDT, strMe, strMessage);
    ui.listWidget->addItem(strMessage);
}
