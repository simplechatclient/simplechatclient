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
#include <QMessageBox>
#include "core/core.h"
#include "common/config.h"
#include "common/simple_crypt.h"
#include "register_nick_gui.h"
#include "profile_manager_gui.h"
#include "profile_add_gui.h"

ProfileAddGui::ProfileAddGui(ProfileManagerGui *_profileManager, QWidget *parent) : QDialog(parent), profileManager(_profileManager)
{
    ui.setupUi(this);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowTitle(tr("Add profile"));
    // center screen
    move(QApplication::desktop()->screenGeometry(QApplication::desktop()->screenNumber(parent)).center()  - rect().center());

    createGui();
    setDefaultValues();
    createSignals();

    ui.lineEdit_nick->setFocus();
}

void ProfileAddGui::createGui()
{
    ui.pushButton_register_nick->setIcon(QIcon(":/images/breeze/list-add-user.svg"));
    ui.buttonBox->button(QDialogButtonBox::Ok)->setIcon(QIcon(":/images/breeze/dialog-ok.svg"));
    ui.buttonBox->button(QDialogButtonBox::Cancel)->setIcon(QIcon(":/images/breeze/dialog-cancel.svg"));

    ui.radioButton_unregistered_nick->setText(tr("Unregistered nick"));
    ui.radioButton_registered_nick->setText(tr("Registered nick"));
    ui.pushButton_register_nick->setText(tr("Register nick"));
    ui.label_nick->setText(tr("Nick:"));
    ui.label_password->setText(tr("Password:"));
    ui.buttonBox->button(QDialogButtonBox::Ok)->setText(tr("Ok"));
    ui.buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Cancel"));
}

void ProfileAddGui::setDefaultValues()
{
    ui.radioButton_unregistered_nick->setChecked(true);
    hidePass();

    ui.pushButton_register_nick->setVisible(false);
}

void ProfileAddGui::createSignals()
{
    connect(ui.radioButton_unregistered_nick, SIGNAL(clicked()), this, SLOT(hidePass()));
    connect(ui.radioButton_registered_nick, SIGNAL(clicked()), this, SLOT(showPass()));
    connect(ui.pushButton_register_nick, SIGNAL(clicked()), this, SLOT(buttonRegisterNick()));
    connect(ui.buttonBox, SIGNAL(accepted()), this, SLOT(buttonOk()));
    connect(ui.buttonBox, SIGNAL(rejected()), this, SLOT(close()));
}

void ProfileAddGui::setNickPass(const QString &n, const QString &p)
{
    ui.radioButton_registered_nick->setChecked(true);
    showPass();

    ui.lineEdit_nick->setText(n);
    ui.lineEdit_password->setText(p);
}

void ProfileAddGui::hidePass()
{
    ui.label_password->hide();
    ui.lineEdit_password->hide();

    QString strNick = ui.lineEdit_nick->text().trimmed();

    // fix nick
    //if ((!strNick.isEmpty()) && (strNick.at(0) != '~'))
        //ui.lineEdit_nick->setText("~"+strNick);

    ui.lineEdit_nick->setFocus();
}

void ProfileAddGui::showPass()
{
    ui.label_password->show();
    ui.lineEdit_password->show();

    QString strNick = ui.lineEdit_nick->text().trimmed();

    // fix nick
    //if ((!strNick.isEmpty()) && (strNick.at(0) == '~'))
        //ui.lineEdit_nick->setText(strNick.remove(0,1));

    ui.lineEdit_nick->setFocus();
}

void ProfileAddGui::buttonRegisterNick()
{
    RegisterNickGui(this).exec();
}

void ProfileAddGui::buttonOk()
{
    QString strNick = ui.lineEdit_nick->text().trimmed();
    QString strPassword = ui.lineEdit_password->text();

    // empty
    if (strNick.isEmpty())
    {
        QMessageBox::critical(0, QString::null, tr("Nick is empty!"));
        return;
    }

    // check nick
    QString strPass;
    if (ui.radioButton_unregistered_nick->isChecked())
    {
        // fix nick
        //if (strNick.at(0) != '~')
            //strNick = "~"+strNick;

        strPass = QString::null;
    }
    else if (ui.radioButton_registered_nick->isChecked())
    {
        // fix nick
        //if (strNick.at(0) == '~')
            //strNick.remove(0,1);

        strPass = ui.lineEdit_password->text();
    }

    // fix nick
    //if ((strPass.isEmpty()) && (strNick.at(0) != '~'))
        //strNick = '~'+strNick;

    // fix too long nick
    if ((strPass.isEmpty()) && (strNick.size() > 32))
        strNick = strNick.left(32);

    // exist profile
    if (profileManager->existProfile(strNick))
    {
        QMessageBox::critical(0, QString::null, tr("Profile already exists!"));
        return;
    }

    // encrypt pass
    if (!strPass.isEmpty())
    {
        SimpleCrypt *pSimpleCrypt = new SimpleCrypt();
        strPassword = pSimpleCrypt->encrypt(strNick, strPassword);
        delete pSimpleCrypt;
    }

    // save
    Config *pConfig = new Config(ProfileConfig, strNick);
    pConfig->set("nick", strNick);
    pConfig->set("pass", strPassword);
    delete pConfig;

    // refresh
    profileManager->refreshAllLists();

    // close
    this->close();
}
