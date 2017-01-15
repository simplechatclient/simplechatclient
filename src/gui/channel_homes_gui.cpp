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
#include <QInputDialog>
#include <QMenu>
#include <QTimer>
#include "models/avatar.h"
#include "models/channel.h"
#include "core/core.h"
#include "channel_settings_gui.h"
#include "models/channel_homes.h"
#include "channel_homes_gui.h"

ChannelHomesGui::ChannelHomesGui(QWidget *parent) : QDialog(parent)
{
    ui.setupUi(this);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowTitle(tr("My channels"));
    // center screen
    move(QApplication::desktop()->screenGeometry(QApplication::desktop()->screenNumber(parent)).center()  - rect().center());

    createGui();
    createSignals();

    refresh();
}

void ChannelHomesGui::createGui()
{
    ui.toolButton_options->setEnabled(false);

    ui.pushButton_create->setIcon(QIcon(":/images/breeze/irc-join-channel.svg"));
    ui.pushButton_remove->setIcon(QIcon(":/images/breeze/irc-close-channel.svg"));
    ui.toolButton_options->setIcon(QIcon(":/images/breeze/applications-system.svg"));
    ui.buttonBox->button(QDialogButtonBox::Close)->setIcon(QIcon(":/images/breeze/dialog-close.svg"));

    ui.pushButton_create->setText(tr("Create"));
    ui.pushButton_remove->setText(tr("Remove"));
    ui.toolButton_options->setText(tr("Options"));
    ui.buttonBox->button(QDialogButtonBox::Close)->setText(tr("Close"));

    QMenu *optionsMenu = new QMenu(this);
    optionsMenu->addAction(QIcon(":/images/breeze/legalmoves.svg"), tr("Join"), this, SLOT(join()));
    optionsMenu->addAction(QIcon(":/images/settings.svg"), tr("Settings"), this, SLOT(settings()));

    ui.toolButton_options->setMenu(optionsMenu);
}

void ChannelHomesGui::createSignals()
{
    connect(ui.listWidget_channels, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(itemClicked(QListWidgetItem*)));
    connect(ui.pushButton_create, SIGNAL(clicked()), this, SLOT(buttonCreate()));
    connect(ui.pushButton_remove, SIGNAL(clicked()), this, SLOT(buttonRemove()));
    connect(ui.buttonBox, SIGNAL(rejected()), this, SLOT(close()));
}

void ChannelHomesGui::refresh()
{
    // ready
    if (ChannelHomes::instance()->getStatus() != StatusCompleted)
    {
        QTimer::singleShot(200, this, SLOT(refresh())); // 0.2 sec
        return;
    }

    ui.listWidget_channels->clear();

    QHashIterator<QString, OnetChannelHomes> it(ChannelHomes::instance()->get());
    while (it.hasNext())
    {
        it.next();
        QString strChannel = it.key();
        OnetChannelHomes oChannelHomes = it.value();

        QString strAvatar = oChannelHomes.avatar;
        if (!strAvatar.isEmpty())
        {
            strAvatar = Avatar::instance()->getAvatarPath(strAvatar);
        }
        else
        {
            strAvatar = ":/images/channel_avatar.png";
            Core::instance()->network->send(QString("CS INFO %1 i").arg(strChannel));
        }

        ui.listWidget_channels->addItem(new QListWidgetItem(QIcon(strAvatar), strChannel));
    }
}

void ChannelHomesGui::itemClicked(QListWidgetItem *)
{
    if (!ui.toolButton_options->isEnabled())
        ui.toolButton_options->setEnabled(true);
}

void ChannelHomesGui::buttonCreate()
{
    bool ok;
    QString strText = QInputDialog::getText(this, tr("Changing channels"), tr("Enter the name of the new channel:"), QLineEdit::Normal, QString::null, &ok);
    strText = strText.trimmed();

    if ((ok) && (!strText.isEmpty()))
    {
        ChannelHomes::instance()->clear();

        Core::instance()->network->send(QString("CS REGISTER %1").arg(strText));

        QTimer::singleShot(200, this, SLOT(refresh())); // 0.2 sec
    }
}

void ChannelHomesGui::buttonRemove()
{
    bool ok;
    QString strText = QInputDialog::getText(this, tr("Changing channels"), "<p><b>"+tr("The removal of the channel operation is irreversible!")+"</b></p><p>"+tr("Enter the name of the channel to remove:")+"</p>", QLineEdit::Normal, QString::null, &ok);
    strText = strText.trimmed();

    if ((ok) && (!strText.isEmpty()))
    {
        ChannelHomes::instance()->clear();

        Core::instance()->network->send(QString("CS DROP %1").arg(strText));

        QTimer::singleShot(200, this, SLOT(refresh())); // 0.2 sec
    }
}

void ChannelHomesGui::join()
{
    if (ui.listWidget_channels->selectedItems().size() == 0)
        return;

    QString strChannel = ui.listWidget_channels->selectedItems().at(0)->text();
    Core::instance()->network->send(QString("JOIN %1").arg(strChannel));
}

void ChannelHomesGui::settings()
{
    if (ui.listWidget_channels->selectedItems().size() == 0)
        return;

    QString strChannel = ui.listWidget_channels->selectedItems().at(0)->text();
    ChannelSettingsGui(strChannel).exec();
}
