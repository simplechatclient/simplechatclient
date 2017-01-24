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

#include <QDesktopWidget>
#include <QMessageBox>
#include <QPushButton>
#include "common/config.h"
#include "common/simple_crypt.h"
#include "models/settings.h"
#include "profile_edit_gui.h"

ProfileEditGui::ProfileEditGui(const QString &_strNick, QWidget *parent) : QDialog(parent), strNick(_strNick)
{
    ui.setupUi(this);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowTitle(tr("Edit profile"));
    // center screen
    move(QApplication::desktop()->screenGeometry(QApplication::desktop()->screenNumber(parent)).center()  - rect().center());

    createGui();
    setDefaultValues();
    createSignals();
}

void ProfileEditGui::createGui()
{
    ui.buttonBox->button(QDialogButtonBox::Ok)->setIcon(QIcon(":/images/breeze/dialog-ok.svg"));
    ui.buttonBox->button(QDialogButtonBox::Cancel)->setIcon(QIcon(":/images/breeze/dialog-cancel.svg"));

    ui.label_nick->setText(tr("Nick:"));
    ui.label_password->setText(tr("Password:"));
    ui.buttonBox->button(QDialogButtonBox::Ok)->setText(tr("Ok"));
    ui.buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Cancel"));
}

void ProfileEditGui::setDefaultValues()
{
    Config *pConfig = new Config(ProfileConfig, strNick);
    QString strPassword = pConfig->get("pass");
    delete pConfig;

    // decrypt pass
    if (!strPassword.isEmpty())
    {
        QString strKey = Settings::instance()->get("unique_id");
        SimpleCrypt *pSimpleCrypt = new SimpleCrypt(strKey);
        strPassword = pSimpleCrypt->decrypt(strPassword);
        delete pSimpleCrypt;
    }

    ui.lineEdit_nick->setText(strNick);
    ui.lineEdit_password->setText(strPassword);
}

void ProfileEditGui::createSignals()
{
    connect(ui.buttonBox, SIGNAL(accepted()), this, SLOT(buttonOk()));
    connect(ui.buttonBox, SIGNAL(rejected()), this, SLOT(close()));
}

void ProfileEditGui::buttonOk()
{
    QString strPassword = ui.lineEdit_password->text();

    // empty
    if (strPassword.isEmpty())
    {
        QMessageBox::critical(0, QString::null, tr("Password is empty!"));
        return;
    }

    // encrypt pass
    QString strKey = Settings::instance()->get("unique_id");
    SimpleCrypt *pSimpleCrypt = new SimpleCrypt(strKey);
    strPassword = pSimpleCrypt->encrypt(strPassword);
    delete pSimpleCrypt;

    Config *pConfig = new Config(ProfileConfig, strNick);
    pConfig->set("pass", strPassword);
    delete pConfig;

    // current nick
    QString strMe = Settings::instance()->get("nick");
    if (strNick == strMe)
        Settings::instance()->set("pass", strPassword);

    // close
    this->close();
}
