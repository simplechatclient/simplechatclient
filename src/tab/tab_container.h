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

#ifndef TAB_CONTAINER_H
#define TAB_CONTAINER_H

#include "core/defines.h"
#include <QObject>
class TabManager;

class TabContainer : public QObject
{
    Q_OBJECT
public:
    TabContainer(TabManager *_pTabM);
    virtual ~TabContainer();
    void addTab(const QString &strChannel);
    void removeTab(const QString &strChannel);
    void renameTab(const QString &strChannel, const QString &strNewName);
    void partTab(int index);
    void resizeMainWindow(QSize s);

public slots:
    void refreshColors();
    void refreshCSS();
    void forceScrollToBottom();

private:
    // params
    TabManager *pTabM;
};

#endif // TAB_CONTAINER_H
