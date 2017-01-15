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

#include <QHBoxLayout>
#include <QMenu>
#include <QLabel>
#include <QPushButton>
#include <QTabBar>
#include <QToolButton>
#include <QWidgetAction>
#include "models/channel.h"
#include "core/core.h"
#include "core/defines.h"
#include "gui/join_channel_gui.h"
#include "core/mainwindow.h"
#include "tab_manager.h"

TabManager::TabManager(QWidget *parent) : QTabWidget(parent)
{
    tab = tabBar();
    setTabsClosable(true);
    setMovable(true);
    setIconSize(QSize(22,22));

    cRed = QColor(255, 0, 0, 255);
    cGreen = QColor(0, 147, 0, 255);
    cHighlight = QColor(138, 0, 184, 255);

    QToolButton *bJoinChannel = new QToolButton(this);
    bJoinChannel->setAutoRaise(true);
    bJoinChannel->setIcon(QIcon(":/images/breeze/tab-new-background.svg"));
    bJoinChannel->setIconSize(QSize(22,22));
    bJoinChannel->setToolTip(tr("Join channel"));
    bJoinChannel->setFocusPolicy(Qt::NoFocus);
    bJoinChannel->show();

    bChannelsList = new QToolButton(this);
    bChannelsList->setAutoRaise(true);
    bChannelsList->setIcon(QIcon(":/images/breeze/tab-detach.svg"));
    bChannelsList->setIconSize(QSize(22,22));
    bChannelsList->setToolTip(tr("Channels list"));
    bChannelsList->setFocusPolicy(Qt::NoFocus);

    QHBoxLayout *cornerLayout = new QHBoxLayout();
    cornerLayout->setMargin(0);
    cornerLayout->addWidget(bJoinChannel);
    cornerLayout->addWidget(bChannelsList);

    QWidget *cornerWidget = new QWidget(this);
    cornerWidget->setLayout(cornerLayout);
    cornerWidget->show();

    this->setCornerWidget(cornerWidget, Qt::TopRightCorner);
    Core::instance()->mainWindow()->toolButtonFix(this);

    mChannelsList = new QMenu(tr("Channels list"));
    mChannelsList->hide();

    connect(tab, SIGNAL(tabMoved(int,int)), this, SLOT(tabMovedSlot(int, int)));
    connect(bJoinChannel, SIGNAL(clicked()), this, SLOT(joinChannelClicked()));
    connect(bChannelsList, SIGNAL(clicked()), this, SLOT(channelsListClicked()));
}

void TabManager::setAlert(const QString &channel, ChannelColor c)
{
    QColor color;
    int colorPriority = 0;

    if (c == ChannelGreen)
    {
        color = cGreen;
        colorPriority = 1;
    }
    else if (c == ChannelRed)
    {
        color = cRed;
        colorPriority = 2;
    }
    else if (c == ChannelHighlight)
    {
        color = cHighlight;
        colorPriority = 3;
    }

    int index = Channel::instance()->getIndexFromName(channel);
    if ((index < 0) || (index >= count()))
        return;

    QColor currentColor = tab->tabTextColor(index);
    int currentPriority = 0;

    if (currentColor == cGreen)
        currentPriority = 1;
    else if (currentColor == cRed)
        currentPriority = 2;
    else if (currentColor == cHighlight)
        currentPriority = 3;

    if (colorPriority > currentPriority)
    {
        if (index != currentIndex())
            tab->setTabTextColor(index, color);
    }
}

void TabManager::setColor(int index, QColor color)
{
    if ((index >= 0) && (index < count()))
        tab->setTabTextColor(index, color);
}

void TabManager::updateIcon(int index, const QString &avatar)
{
    if ((index >= 0) && (index < count()))
    {
        setTabIcon(index, QIcon(avatar));
    }
}

void TabManager::hideCloseButton(int index)
{
    if ((index >= 0) && (index < count()))
        tab->setTabButton(index, QTabBar::RightSide, 0);
}

void TabManager::tabInserted(int index)
{
    if (tabText(index) == STATUS_WINDOW)
    {
        // hide close on status
        hideCloseButton(index);
        // default avatar
        tab->setTabIcon(index, QIcon(":/images/status_avatar.png"));
    }
    else if (tabText(index) == DEBUG_WINDOW)
    {
        // hide close on debug
        hideCloseButton(index);
        // default avatar
        tab->setTabIcon(index, QIcon(":/images/debug_avatar.png"));
    }
}

void TabManager::tabMovedSlot(int from, int to)
{
    Channel::instance()->move(from, to);
}

void TabManager::joinChannelClicked()
{
    JoinChannelGui(this).exec();
}

void TabManager::switchChannel()
{
    QPushButton *button = qobject_cast<QPushButton *>(sender());
    if (button)
    {
        int index = button->property("index").toInt();
        if ((index >= 0) && (index < count()))
            tab->setCurrentIndex(index);
    }
    mChannelsList->hide();
}

void TabManager::channelsListClicked()
{
    mChannelsList->clear();

    int iChannelsCount = tab->count();
    for (int i = 0; i < iChannelsCount; ++i)
    {
        QString strChannel = Channel::instance()->getNameFromIndex(i);
        if (strChannel.at(0) == '^')
            strChannel = Channel::instance()->getAlternativeName(strChannel);

        openChannelsButtons[i] = new QPushButton(strChannel);
        openChannelsButtons[i]->setFlat(true);
        openChannelsButtons[i]->setIcon(tab->tabIcon(i));
        openChannelsButtons[i]->setIconSize(QSize(22,22));
        openChannelsButtons[i]->setProperty("index", i);

        QColor currentColor = tab->tabTextColor(i);
        if (currentColor == cRed)
            openChannelsButtons[i]->setStyleSheet("color: rgb(255, 0, 0); text-align: left;");
        else if (currentColor == cGreen)
            openChannelsButtons[i]->setStyleSheet("color: rgb(0, 147, 0); text-align: left;");
        else if (currentColor == cHighlight)
            openChannelsButtons[i]->setStyleSheet("color: rgb(138, 0, 184); text-align: left;");
        else
            openChannelsButtons[i]->setStyleSheet("text-align: left;");

        QWidgetAction *openChannelsActs = new QWidgetAction(this);
        openChannelsActs->setDefaultWidget(openChannelsButtons[i]);

        connect(openChannelsButtons[i], SIGNAL(clicked()), this, SLOT(switchChannel()));
        mChannelsList->addAction(openChannelsActs);
    }

    mChannelsList->show();
    mChannelsList->exec(bChannelsList->mapToGlobal(QPoint(-mChannelsList->sizeHint().width()+bChannelsList->width(), bChannelsList->height())));
}
