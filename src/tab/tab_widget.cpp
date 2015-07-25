/*
 * Simple Chat Client
 *
 *   Copyright (C) 2009-2015 Piotr Łuczko <piotr.luczko@gmail.com>
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

#include <QLabel>
#include <QSplitter>
#include <QVBoxLayout>
#include "widgets/nicklist_delegate.h"
#include "widgets/nicklist_widget.h"
#include "models/settings.h"
#include "tab_widget.h"

TabWidget::TabWidget(const QString &_strName) : strName(_strName)
{
    createGui();
    setDefaultValues();
}

TabWidget::~TabWidget()
{
    delete pNickListWidget;
    delete pChatView;
    delete topic;
}

void TabWidget::createGui()
{
    topic = new TopicWidget();
    topic->setParent(this);
    topic->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
    topic->show();

    pChatView = new ChatView(strName);
    pChatView->setParent(this);
    pChatView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    pChatView->show();

    users = new QLabel(this);
    users->setAlignment(Qt::AlignCenter);
    users->show();

    pNickListWidget = new NickListWidget(strName);
    pNickListWidget->setParent(this);
    pNickListWidget->setItemDelegate(new NickListDelegate(pNickListWidget));
    pNickListWidget->show();

    QVBoxLayout *leftLayout = new QVBoxLayout();
    leftLayout->setMargin(0);
    leftLayout->addWidget(topic);
    leftLayout->addWidget(pChatView);

    QVBoxLayout *rightLayout = new QVBoxLayout();
    rightLayout->setMargin(0);
    rightLayout->addWidget(users);
    rightLayout->addWidget(pNickListWidget);

    QWidget *leftWidget = new QWidget(this);
    leftWidget->setLayout(leftLayout);
    leftWidget->show();

    QWidget *rightWidget = new QWidget(this);
    rightWidget->setLayout(rightLayout);
    rightWidget->show();

    splitter = new QSplitter(this);
    splitter->addWidget(leftWidget);
    splitter->addWidget(rightWidget);
    splitter->show();

    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(splitter);
    this->setLayout(layout);
}

void TabWidget::setDefaultValues()
{
    // settings
    if (Settings::instance()->getBool("nicklist"))
    {
        users->show();
        pNickListWidget->show();
    }
    else
    {
        users->hide();
        pNickListWidget->hide();
    }

    // channel type
    if (strName.at(0) == '^')
    {
        topic->hide();
    }
    if ((strName.at(0) != '^') && (strName.at(0) != '#'))
    {
        topic->hide();
        users->hide();
        pNickListWidget->hide();
    }
}

void TabWidget::showEvent(QShowEvent *)
{
    if ((strName.at(0) == '^') || (strName.at(0) == '#'))
    {
        if (Settings::instance()->getBool("nicklist") && pNickListWidget->isHidden())
        {
            users->show();
            pNickListWidget->show();
        }
        else if ((Settings::instance()->get("nicklist") == "false") && (!pNickListWidget->isHidden()))
        {
            users->hide();
            pNickListWidget->hide();
        }
    }

    int width = this->width();

    if (pNickListWidget->isVisible())
    {
        if (width > 250)
        {
            QList<int> currentSizes = splitter->sizes();

            currentSizes[0] = width-200;
            currentSizes[1] = 200;

            splitter->setSizes(currentSizes);
        }
    }
    else
    {
        QList<int> currentSizes = splitter->sizes();
        currentSizes[0] = width;
        currentSizes[1] = 0;
        splitter->setSizes(currentSizes);
    }
}
