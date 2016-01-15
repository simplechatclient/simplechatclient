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
#include <QCompleter>
#include "core/core.h"
#include "models/channel_list.h"
#include "join_channel_gui.h"

JoinChannelGui::JoinChannelGui(QWidget *parent) : QDialog(parent)
{
    ui.setupUi(this);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowTitle(tr("Join channel"));
    // center screen
    move(QApplication::desktop()->screenGeometry(QApplication::desktop()->screenNumber(parent)).center()  - rect().center());

    createGui();
    createSignals();

    createCompleter();
}

void JoinChannelGui::createGui()
{
    ui.pushButton_join->setIcon(QIcon(":/images/breeze/irc-join-channel.svg"));
    ui.buttonBox->button(QDialogButtonBox::Close)->setIcon(QIcon(":/images/breeze/dialog-close.svg"));

    ui.pushButton_join->setText(tr("Join"));
    ui.label_channel_name->setText(tr("Channel name:"));
    ui.buttonBox->button(QDialogButtonBox::Close)->setText(tr("Close"));
}

void JoinChannelGui::createSignals()
{
    connect(ui.pushButton_join, SIGNAL(clicked()), this, SLOT(buttonJoin()));
    connect(ui.buttonBox, SIGNAL(rejected()), this, SLOT(close()));
}

void JoinChannelGui::createCompleter()
{
    QStringList lChannelList;

    QList<OnetChannelList> list = ChannelList::instance()->getAll();
    foreach (const OnetChannelList &oChannelList, list)
    {
        QString strName = oChannelList.name;
        if (strName.at(0) == '#') strName.remove(0,1);

        lChannelList.append(strName);
    }

    QList<CaseIgnoreString> lChannelsCaseIgnore;

    foreach (const QString &strChannel, lChannelList)
        lChannelsCaseIgnore.append(strChannel);

    // clear
    lChannelList.clear();

    // sort
    qStableSort(lChannelsCaseIgnore.begin(), lChannelsCaseIgnore.end());

    // create sorted list
    foreach (const QString &strChannel, lChannelsCaseIgnore)
        lChannelList.append(strChannel);

    // clear old list
    lChannelsCaseIgnore.clear();

    QCompleter *completer = new QCompleter(lChannelList, this);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    ui.lineEdit_channel->setCompleter(completer);
}

void JoinChannelGui::buttonJoin()
{
    QString strChannel = ui.lineEdit_channel->text().trimmed();

    if (strChannel.isEmpty())
        return;

    if ((strChannel.at(0) != '#') && (strChannel.at(0) != '^'))
        strChannel = "#"+strChannel;

    Core::instance()->network->send(QString("JOIN %1").arg(strChannel));

    this->close();
}
