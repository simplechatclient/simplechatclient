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

#ifndef TOOLBUTTONMENU_H
#define TOOLBUTTONMENU_H

#include <QMenu>

QT_BEGIN_NAMESPACE
class QToolButton;
class QWidget;
QT_END_NAMESPACE

class ToolButtonMenu : public QMenu
{
    Q_OBJECT
public:
    explicit ToolButtonMenu(QToolButton* button, QWidget* parent = 0);
    void showEvent(QShowEvent* event);
private:
    QToolButton* b;
};

#endif // TOOLBUTTONMENU_H
