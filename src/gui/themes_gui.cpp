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
#include <QPushButton>
#include "common/config.h"
#include "models/settings.h"
#include "models/themes.h"
#include "themes_gui.h"

ThemesGui::ThemesGui(QWidget *parent) : QDialog(parent)
{
    ui.setupUi(this);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowTitle(tr("Themes"));
    // center screen
    move(QApplication::desktop()->screenGeometry(QApplication::desktop()->screenNumber(parent)).center()  - rect().center());

    createGui();
    setDefaultValues();
    createSignals();
}

void ThemesGui::createGui()
{
    ui.buttonBox->button(QDialogButtonBox::Close)->setIcon(QIcon(":/images/breeze/dialog-close.svg"));

    ui.label_view->setText(tr("View"));
    ui.buttonBox->button(QDialogButtonBox::Close)->setText(tr("Close"));

    // themes
    QStringList lThemes = Themes::instance()->getAll();
    ui.listWidget->addItems(lThemes);
}

void ThemesGui::setDefaultValues()
{
    QString strCurrentTheme = Settings::instance()->get("themes");

    if (strCurrentTheme == "Standard")
    {
        ui.listWidget->setCurrentRow(0);
        ui.label->setPixmap(QPixmap(":/images/theme_standard.png"));
    }
    else if (strCurrentTheme == "Alhena")
    {
        ui.listWidget->setCurrentRow(1);
        ui.label->setPixmap(QPixmap(":/images/theme_alhena.png"));
    }
    else if (strCurrentTheme == "Origin")
    {
        ui.listWidget->setCurrentRow(2);
        ui.label->setPixmap(QPixmap(":/images/theme_origin.png"));
    }
    else if (strCurrentTheme == "Adara")
    {
        ui.listWidget->setCurrentRow(3);
        ui.label->setPixmap(QPixmap(":/images/theme_adara.png"));
    }
    else
    {
        ui.listWidget->setCurrentRow(0);
        ui.label->setPixmap(QPixmap(":/images/theme_standard.png"));
    }
}

void ThemesGui::createSignals()
{
    connect(ui.listWidget, SIGNAL(clicked(QModelIndex)), this, SLOT(changeTheme(QModelIndex)));
    connect(ui.buttonBox, SIGNAL(rejected()), this, SLOT(close()));
}

void ThemesGui::changeTheme(QModelIndex index)
{
    QString strTheme = "Standard";

    switch (index.row())
    {
        case 0:
            strTheme = "Standard";
            ui.label->setPixmap(QPixmap(":/images/theme_standard.png"));
            break;
        case 1:
            strTheme = "Alhena";
            ui.label->setPixmap(QPixmap(":/images/theme_alhena.png"));
            break;
        case 2:
            strTheme = "Origin";
            ui.label->setPixmap(QPixmap(":/images/theme_origin.png"));
            break;
        case 3:
            strTheme = "Adara";
            ui.label->setPixmap(QPixmap(":/images/theme_adara.png"));
            break;
    }

    // save
    Settings::instance()->set("themes", strTheme);
    Config *pConfig = new Config();
    pConfig->set("themes", strTheme);
    delete pConfig;

    // refresh
    Themes::instance()->refreshCurrent();
}
