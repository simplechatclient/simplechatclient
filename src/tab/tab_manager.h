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

#ifndef TAB_MANAGER_H
#define TAB_MANAGER_H

#include "core/defines.h"
#include <QTabWidget>

QT_BEGIN_NAMESPACE
class QMenu;
class QPushButton;
class QTabBar;
class QToolButton;
QT_END_NAMESPACE

/**
 * TabBar with highlight support
 */
class TabManager : public QTabWidget
{
    Q_OBJECT
public:
    TabManager(QWidget *parent = 0);
    void setAlert(const QString &channel, ChannelColor c);
    void setColor(int index, QColor color);
    void updateIcon(int index, const QString &avatar);

private:
    QTabBar *tab;
    QMenu *mChannelsList;
    QToolButton *bChannelsList;
    QPushButton *openChannelsButtons[MAX_OPEN_CHANNELS];
    QColor cRed;
    QColor cGreen;
    QColor cHighlight;

    void hideCloseButton(int index);

private slots:
    void tabMovedSlot(int from, int to);
    void joinChannelClicked();
    void channelsListClicked();
    void switchChannel();

protected:
    virtual void tabInserted(int index);
};

#endif // TAB_MANAGER_H
