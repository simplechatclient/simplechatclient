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
#include <QDir>
#include <QMessageBox>
#include "core/core.h"
#include "options_gui.h"
#include "profile_add_gui.h"
#include "profile_edit_gui.h"
#include "models/profile_manager.h"
#include "models/settings.h"
#include "profile_manager_gui.h"

#ifdef Q_OS_WIN
    #include <QStandardPaths>
#endif

ProfileManagerGui::ProfileManagerGui(OptionsGui *_pOptionsGui, QWidget *parent) : QDialog(parent), pOptionsGui(_pOptionsGui)
{
    ui.setupUi(this);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowTitle(tr("Profile manager"));
    // center screen
    move(QApplication::desktop()->screenGeometry(QApplication::desktop()->screenNumber(parent)).center()  - rect().center());

    createGui();
    setDefaultValues();
    createSignals();
}

void ProfileManagerGui::createGui()
{
    ui.pushButton_add->setIcon(QIcon(":/images/breeze/user-group-new.svg"));
    ui.pushButton_edit->setIcon(QIcon(":/images/breeze/user-group-properties.svg"));
    ui.pushButton_remove->setIcon(QIcon(":/images/breeze/user-group-delete.svg"));
    ui.buttonBox->button(QDialogButtonBox::Close)->setIcon(QIcon(":/images/breeze/dialog-close.svg"));

    ui.pushButton_add->setText(tr("Add"));
    ui.pushButton_edit->setText(tr("Edit"));
    ui.pushButton_remove->setText(tr("Remove"));
    ui.buttonBox->button(QDialogButtonBox::Close)->setText(tr("Close"));

    ui.pushButton_edit->setEnabled(false);
    ui.pushButton_remove->setEnabled(false);
}

void ProfileManagerGui::setDefaultValues()
{
    // create path
    createPath();

    // refresh list
    refreshProfilesList();
}

void ProfileManagerGui::createSignals()
{
    connect(ui.listWidget, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(itemClicked()));
    connect(ui.pushButton_add, SIGNAL(clicked()), this, SLOT(addProfile()));
    connect(ui.pushButton_edit, SIGNAL(clicked()), this, SLOT(editProfile()));
    connect(ui.pushButton_remove, SIGNAL(clicked()), this, SLOT(removeProfile()));
    connect(ui.buttonBox, SIGNAL(rejected()), this, SLOT(close()));
}

void ProfileManagerGui::refreshAllLists()
{
    refreshProfilesList(); // here
    pOptionsGui->refreshProfilesList(); // in options
}

bool ProfileManagerGui::existProfile(const QString &strExistProfile)
{
    QDir dir(path);
    dir.setSorting(QDir::Name | QDir::IgnoreCase);

    QFileInfoList list = dir.entryInfoList(QStringList("*"), QDir::Dirs | QDir::NoDotAndDotDot | QDir::NoSymLinks);
    foreach (const QFileInfo &info, list)
    {
        QString profileName = info.fileName();

        if (profileName == strExistProfile)
            return true;
    }
    return false;
}

void ProfileManagerGui::createPath()
{
#ifdef Q_OS_WIN
    path = QFileInfo(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)).absoluteFilePath();
    path += "/scc/";
#else
    path = QDir::homePath()+"/.scc/";
#endif

    path += "profiles/";

    // create dir if not exist
    if (!QDir().exists(path))
        QDir().mkpath(path);
}

void ProfileManagerGui::refreshProfilesList()
{
    // clear
    ui.listWidget->clear();

    QDir dir(path);
    dir.setSorting(QDir::Name | QDir::IgnoreCase);

    QFileInfoList list = dir.entryInfoList(QStringList("*"), QDir::Dirs | QDir::NoDotAndDotDot | QDir::NoSymLinks);
    foreach (const QFileInfo &info, list)
    {
        QString profileName = info.fileName();
        ui.listWidget->addItem(profileName);
    }
}

void ProfileManagerGui::itemClicked()
{
    if (ui.listWidget->selectedItems().size() == 0)
        return;

    QString strNick = ui.listWidget->selectedItems().at(0)->text();

    if (strNick.at(0) != '~')
        ui.pushButton_edit->setEnabled(true);
    else
        ui.pushButton_edit->setEnabled(false);

    ui.pushButton_remove->setEnabled(true);
}

void ProfileManagerGui::addProfile()
{
    ProfileAddGui(this).exec();
}

void ProfileManagerGui::editProfile()
{
    if (ui.listWidget->selectedItems().size() == 0)
        return;

    QString strNick = ui.listWidget->selectedItems().at(0)->text();
    ProfileEditGui(strNick).exec();
}

void ProfileManagerGui::removeProfile()
{
    if (ui.listWidget->count() <= 1)
    {
        QMessageBox::critical(0, QString::null, tr("Cannot remove last profile"));
        return;
    }

    if (ui.listWidget->selectedItems().size() == 0)
        return;

    QString profileName = ui.listWidget->selectedItems().at(0)->text();

    if (QDir().exists(path+"/"+profileName))
    {
        ProfileManager::instance()->removeProfileDirectory(path+"/"+profileName);

        refreshAllLists();

        if (profileName == Settings::instance()->get("current_profile"))
            pOptionsGui->setCurrentProfile(0);
    }
}
