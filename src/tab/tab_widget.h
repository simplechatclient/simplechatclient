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

#ifndef TAB_WIDGET_H
#define TAB_WIDGET_H

#include "widgets/chat_view.h"
#include "widgets/topic_widget.h"
#include <QWidget>
class NickListWidget;

QT_BEGIN_NAMESPACE
class QLabel;
class QSplitter;
QT_END_NAMESPACE

/**
 * Main widget
 */
class TabWidget : public QWidget
{
    Q_OBJECT
public:
    TabWidget(const QString &_strName);
    virtual ~TabWidget();

    TopicWidget *topic;
    ChatView *pChatView;
    QLabel *users;
    NickListWidget *pNickListWidget;
    QSplitter *splitter;

private:
    QString strName;

    void createGui();
    void setDefaultValues();

protected:
    void showEvent(QShowEvent *);
};

#endif // TAB_WIDGET_H
