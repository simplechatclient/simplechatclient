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
#include "models/avatar.h"
#include "models/channel.h"
#include "core/core.h"
#include "models/channel_favourites.h"
#include "channel_favourites_gui.h"

ChannelFavouritesGui::ChannelFavouritesGui(QWidget *parent) : QDialog(parent)
{
    ui.setupUi(this);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowTitle(tr("Favorite channels"));
    // center screen
    move(QApplication::desktop()->screenGeometry(QApplication::desktop()->screenNumber(parent)).center()  - rect().center());

    createGui();
    createSignals();

    refresh();
}

void ChannelFavouritesGui::createGui()
{
    ui.toolButton_options->setEnabled(false);

    ui.pushButton_add->setIcon(QIcon(":/images/breeze/irc-join-channel.svg"));
    ui.pushButton_remove->setIcon(QIcon(":/images/breeze/irc-close-channel.svg"));
    ui.toolButton_options->setIcon(QIcon(":/images/breeze/applications-system.svg"));
    ui.buttonBox->button(QDialogButtonBox::Close)->setIcon(QIcon(":/images/breeze/dialog-close.svg"));

    ui.pushButton_add->setText(tr("Add"));
    ui.pushButton_remove->setText(tr("Remove"));
    ui.toolButton_options->setText(tr("Options"));
    ui.buttonBox->button(QDialogButtonBox::Close)->setText(tr("Close"));

    QMenu *optionsMenu = new QMenu(this);
    optionsMenu->addAction(QIcon(":/images/breeze/legalmoves.svg"), tr("Join"), this, SLOT(join()));

    ui.toolButton_options->setMenu(optionsMenu);
}

void ChannelFavouritesGui::createSignals()
{
    connect(ui.pushButton_add, SIGNAL(clicked()), this, SLOT(buttonAdd()));
    connect(ui.pushButton_remove, SIGNAL(clicked()), this, SLOT(buttonRemove()));
    connect(ui.buttonBox, SIGNAL(rejected()), this, SLOT(close()));
    connect(ui.listWidget_channels, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(itemClicked(QListWidgetItem*)));
}

void ChannelFavouritesGui::refresh()
{
    ui.listWidget_channels->clear();

    QHashIterator<QString, OnetChannelFavourites> it(ChannelFavourites::instance()->getAll());
    while (it.hasNext())
    {
        it.next();
        QString strChannel = it.key();
        OnetChannelFavourites onetChannelFavourites = it.value();

        QString strAvatar = onetChannelFavourites.avatar;
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

void ChannelFavouritesGui::itemClicked(QListWidgetItem *)
{
    if (!ui.toolButton_options->isEnabled())
        ui.toolButton_options->setEnabled(true);
}

void ChannelFavouritesGui::buttonAdd()
{
    bool ok;
    QString strText = QInputDialog::getText(this, tr("Change your favorite channels"), tr("Enter the name of the new channel to add to favorites:"), QLineEdit::Normal, QString::null, &ok);
    strText = strText.trimmed();

    if ((ok) && (!strText.isEmpty()))
    {
        Core::instance()->network->send(QString("NS FAVOURITES ADD %1").arg(strText));
        QTimer::singleShot(1000*4, this, SLOT(refresh())); // 4 sec
    }
}

void ChannelFavouritesGui::buttonRemove()
{
    QString strSelected;
    if (ui.listWidget_channels->selectedItems().size() != 0)
        strSelected = ui.listWidget_channels->selectedItems().at(0)->text();

    bool ok;
    QString strText = QInputDialog::getText(this, tr("Change your favorite channels"), tr("Enter the name of the channel to remove from the favorites:"), QLineEdit::Normal, strSelected, &ok);
    strText = strText.trimmed();

    if ((ok) && (!strText.isEmpty()))
    {
        Core::instance()->network->send(QString("NS FAVOURITES DEL %1").arg(strText));
        QTimer::singleShot(1000*4, this, SLOT(refresh())); // 4 sec
    }
}

void ChannelFavouritesGui::join()
{
    if (ui.listWidget_channels->selectedItems().size() == 0)
        return;

    QString strChannel = ui.listWidget_channels->selectedItems().at(0)->text();
    Core::instance()->network->send(QString("JOIN %1").arg(strChannel));
}
