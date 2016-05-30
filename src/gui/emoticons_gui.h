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

#ifndef EMOTICONS_GUI_H
#define EMOTICONS_GUI_H

#include <QDialog>
#include <QThread>
#include "core/defines.h"
#include "ui_emoticons.h"
class InputLineWidget;

QT_BEGIN_NAMESPACE
class QListWidgetItem;
class QListWidget;
QT_END_NAMESPACE

enum EmoticonsRoles
{
    EmoticonDirectoryRole = Qt::UserRole,
};

class EmoticonsGui : public QDialog
{
    Q_OBJECT
public:
    EmoticonsGui(InputLineWidget *_pInputLineWidget, QWidget *parent = 0);

private:
    Ui::uiEmoticons ui;
    InputLineWidget *pInputLineWidget;

    void createGui();
    void setDefaultValues();
    void createSignals();

    void createCategoriesList();
    void displayEmoticons(const QString &path);

public slots:
    void categoryChanged(QListWidgetItem *item);
    void buttonInsert();
};

#endif // EMOTICONS_GUI_H
