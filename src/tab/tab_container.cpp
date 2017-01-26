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

#include <QSplitter>
#include "models/avatar.h"
#include "core/core.h"
#include "models/channel.h"
#include "common/log.h"
#include "models/nick.h"
#include "widgets/nicklist_widget.h"
#include "models/settings.h"
#include "tab_manager.h"
#include "tab_container.h"

TabContainer::TabContainer(TabManager *_pTabM) : pTabM(_pTabM)
{
}

TabContainer::~TabContainer()
{
    Channel::instance()->removeAll();
}

void TabContainer::addTab(const QString &strChannel)
{
    if ((strChannel.isEmpty()) || (Channel::instance()->contains(strChannel)))
        return;

    Channel::instance()->add(strChannel);

    // create tab
    pTabM->addTab(Channel::instance()->getTw(strChannel), strChannel);

    int index = pTabM->count()-1;
    pTabM->setCurrentIndex(index);

    QString strDefaultAvatar;

    // if priv
    if (strChannel.at(0) == '^')
    {
        QString strAlternativeName = Channel::instance()->getAlternativeName(strChannel);

        if (!strAlternativeName.isEmpty())
        {
            // new text
            pTabM->setTabText(index, strAlternativeName);

            // new avatar
            QString strUserAvatar = Nick::instance()->getAvatar(strAlternativeName);
            if (strUserAvatar.isEmpty())
            {
                if (strAlternativeName.at(0) == '~')
                    strDefaultAvatar = Avatar::instance()->getEmptyUnregisteredUserAvatar();
                else
                    strDefaultAvatar = Avatar::instance()->getEmptyRegisteredUserAvatar();
            }
            else
                strDefaultAvatar = Avatar::instance()->getAvatarPath(strUserAvatar);
        }
        else
        {
            if (strChannel.at(1) == '~')
                strDefaultAvatar = Avatar::instance()->getEmptyUnregisteredUserAvatar();
            else
                strDefaultAvatar = Avatar::instance()->getEmptyRegisteredUserAvatar();
        }
    }
    else if (strChannel.startsWith('#'))
    {
        strDefaultAvatar = ":/images/channel_avatar.png";

    } else {

        strDefaultAvatar = Avatar::instance()->getEmptyUnregisteredUserAvatar();
    }

    // set default avatar
    if (pTabM->tabIcon(index).isNull())
        pTabM->setTabIcon(index, QIcon(strDefaultAvatar));
}

void TabContainer::removeTab(const QString &strChannel)
{
    if ((!Channel::instance()->contains(strChannel)) || (strChannel == DEBUG_WINDOW) || (strChannel == STATUS_WINDOW))
        return;

    Channel::instance()->remove(strChannel);
}

void TabContainer::renameTab(const QString &strChannel, const QString &strNewName)
{
    int index = Channel::instance()->getIndexFromName(strChannel);

    if (index >= 0 && index <= pTabM->count())
    {
        if ((pTabM->tabText(index).size() != 0) && (pTabM->tabText(index).at(0) == '^'))
        {
            // new text
            pTabM->setTabText(index, strNewName);

            // new avatar
            QString strAvatar = Nick::instance()->getAvatar(strNewName);
            if (strAvatar.isEmpty())
            {
                if (strNewName.at(0) == '~')
                    strAvatar = Avatar::instance()->getEmptyUnregisteredUserAvatar();
                else
                    strAvatar = Avatar::instance()->getEmptyRegisteredUserAvatar();
            }
            else
                strAvatar = Avatar::instance()->getAvatarPath(strAvatar);

            pTabM->setTabIcon(index, QIcon(strAvatar));

            // log
            Log::logOpened(strChannel);
        }
    }
}

void TabContainer::partTab(int index)
{
    QString strChannel = Channel::instance()->getNameFromIndex(index);

    if (!strChannel.isEmpty())
    {
        if (Core::instance()->network->isConnected()) {
            if (strChannel.startsWith("#")) {
                Core::instance()->network->send(QString("PART %1").arg(strChannel));
            } else {
                removeTab(strChannel);
            }
        } else {
            removeTab(strChannel);
        }
    }
}

void TabContainer::refreshColors()
{
    QList<QString> lChannels = Channel::instance()->getList();

    foreach (const QString &strChannel, lChannels)
    {
        // update tab name color
        int index = Channel::instance()->getIndexFromName(strChannel);
        if (index >= 0 && index <= pTabM->count())
            pTabM->setColor(index, QColor(Settings::instance()->get("default_color")));
    }
}

void TabContainer::refreshCSS()
{
    QList<QString> lChannels = Channel::instance()->getList();

    foreach (const QString &strChannel, lChannels)
    {
        Channel::instance()->getChatView(strChannel)->refreshCSS();
    }
}

void TabContainer::forceScrollToBottom()
{
    QList<QString> lChannels = Channel::instance()->getList();

    foreach (const QString &strChannel, lChannels)
    {
        Channel::instance()->getChatView(strChannel)->forceScrollToBottom();
    }
}

void TabContainer::resizeMainWindow(QSize s)
{
    QList<QString> lChannels = Channel::instance()->getList();

    foreach (const QString &strChannel, lChannels)
    {
        if (!Channel::instance()->getNickListWidget(strChannel)->isHidden())
        {
            int width = s.width();
            if (width > 250)
            {
                QList<int> currentSizes = Channel::instance()->getSplitter(strChannel)->sizes();

                currentSizes[0] = width-200;
                currentSizes[1] = 200;

                Channel::instance()->getSplitter(strChannel)->setSizes(currentSizes);
            }
        }
    }
}
