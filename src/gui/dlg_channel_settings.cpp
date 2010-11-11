/****************************************************************************
 *                                                                          *
 *   This file is part of Simple Chat Client                                *
 *   Copyright (C) 2010 Piotr Łuczko <piotr.luczko@gmail.com>               *
 *                                                                          *
 *   This program is free software: you can redistribute it and/or modify   *
 *   it under the terms of the GNU General Public License as published by   *
 *   the Free Software Foundation, either version 3 of the License, or      *
 *   (at your option) any later version.                                    *
 *                                                                          *
 *   This program is distributed in the hope that it will be useful,        *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *   GNU General Public License for more details.                           *
 *                                                                          *
 *   You should have received a copy of the GNU General Public License      *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 *                                                                          *
 ****************************************************************************/

#include "dlg_channel_settings.h"

DlgChannelSettings::DlgChannelSettings(QWidget *parent, Network *param1) : QDialog(parent)
{
    ui.setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);

    myparent = parent;
    pNetwork = param1;

    ui.pushButton_transfer->setIcon(QIcon(":/images/oxygen/16x16/user-group-new.png"));
    ui.pushButton_remove_channel->setIcon(QIcon(":/images/oxygen/16x16/user-group-delete.png"));
    ui.pushButton_set_email->setIcon(QIcon(":/images/oxygen/16x16/dialog-ok-apply.png"));
    ui.pushButton_set_website->setIcon(QIcon(":/images/oxygen/16x16/dialog-ok-apply.png"));
    ui.pushButton_set_topic->setIcon(QIcon(":/images/oxygen/16x16/dialog-ok-apply.png"));
    ui.pushButton_set_desc->setIcon(QIcon(":/images/oxygen/16x16/dialog-ok-apply.png"));
    ui.pushButton_set_password->setIcon(QIcon(":/images/oxygen/16x16/dialog-ok-apply.png"));
    ui.pushButton_set_limit->setIcon(QIcon(":/images/oxygen/16x16/dialog-ok-apply.png"));
    ui.pushButton_op_add->setIcon(QIcon(":/images/oxygen/16x16/list-add.png"));
    ui.pushButton_op_del->setIcon(QIcon(":/images/oxygen/16x16/list-remove.png"));
    ui.pushButton_halfop_add->setIcon(QIcon(":/images/oxygen/16x16/list-add.png"));
    ui.pushButton_halfop_del->setIcon(QIcon(":/images/oxygen/16x16/list-remove.png"));
    ui.pushButton_ban_add->setIcon(QIcon(":/images/oxygen/16x16/list-add.png"));
    ui.pushButton_ban_del->setIcon(QIcon(":/images/oxygen/16x16/list-remove.png"));
    ui.pushButton_invite_add->setIcon(QIcon(":/images/oxygen/16x16/list-add.png"));
    ui.pushButton_invite_del->setIcon(QIcon(":/images/oxygen/16x16/list-remove.png"));
    ui.buttonBox->button(QDialogButtonBox::Ok)->setIcon(QIcon(":/images/oxygen/16x16/dialog-ok.png"));
    ui.buttonBox->button(QDialogButtonBox::Cancel)->setIcon(QIcon(":/images/oxygen/16x16/dialog-cancel.png"));

    ui.tabWidget->setTabText(0, tr("General"));
    ui.tabWidget->setTabText(1, tr("Permissions"));
    ui.toolBox->setItemText(0, tr("Operators"));
    ui.toolBox->setItemText(1, tr("Half-operators"));
    ui.toolBox->setItemText(2, tr("Banned"));
    ui.toolBox->setItemText(3, tr("Invited"));

    ui.label_channel->setText(tr("Channel:"));
    ui.label_owner->setText(tr("Owner:"));
    ui.pushButton_transfer->setText(tr("Transfer"));
    ui.pushButton_remove_channel->setText(tr("Destroy"));
    ui.label_email->setText(tr("Email:"));
    ui.pushButton_set_email->setText(tr("Apply"));
    ui.label_created->setText(tr("Created:"));
    ui.label_datetime->setText(tr("Date/Time"));
    ui.label_website->setText(tr("Website:"));
    ui.pushButton_set_website->setText(tr("Apply"));
    ui.label_topic->setText(tr("Topic:"));
    ui.pushButton_set_topic->setText(tr("Apply"));
    ui.label_desc->setText(tr("Description:"));
    ui.pushButton_set_desc->setText(tr("Apply"));
    ui.label_status->setText(tr("Status:"));
    ui.radioButton_status_pub->setText(tr("Public"));
    ui.radioButton_status_priv->setText(tr("Private"));
    ui.label_category->setText(tr("Category:"));
    ui.label_guardian->setText(tr("Guardian:"));
    ui.radioButton_guardian_off->setText(tr("Inactive"));
    ui.radioButton_guardian_on->setText(tr("Active"));
    ui.comboBox_guardian_level->setItemText(0, tr("Level 1"));
    ui.comboBox_guardian_level->setItemText(1, tr("Level 2"));
    ui.comboBox_guardian_level->setItemText(2, tr("Level 3"));
    ui.label_password->setText(tr("Password:"));
    ui.pushButton_set_password->setText(tr("Apply"));
    ui.label_limit->setText(tr("Limit:"));
    ui.pushButton_set_limit->setText(tr("Apply"));
    ui.label_moderation->setText(tr("Moderation:"));
    ui.radioButton_moderation_off->setText(tr("Off"));
    ui.radioButton_moderation_on->setText(tr("On"));
    ui.label_auditorium->setText(tr("Auditorium:"));
    ui.radioButton_auditorium_off->setText(tr("Off"));
    ui.radioButton_auditorium_on->setText(tr("On"));
    ui.pushButton_op_add->setText(tr("Add"));
    ui.pushButton_op_del->setText(tr("Remove"));
    ui.pushButton_halfop_add->setText(tr("Add"));
    ui.pushButton_halfop_del->setText(tr("Remove"));
    ui.pushButton_ban_add->setText(tr("Add"));
    ui.pushButton_ban_del->setText(tr("Remove"));
    ui.pushButton_invite_add->setText(tr("Add"));
    ui.pushButton_invite_del->setText(tr("Remove"));

    QObject::connect(ui.pushButton_transfer, SIGNAL(clicked()), this, SLOT(owner_changed()));
    QObject::connect(ui.pushButton_remove_channel, SIGNAL(clicked()), this, SLOT(remove_channel_clicked()));
    QObject::connect(ui.pushButton_set_email, SIGNAL(clicked()), this, SLOT(email_changed()));
    QObject::connect(ui.pushButton_set_website, SIGNAL(clicked()), this, SLOT(www_changed()));
    QObject::connect(ui.pushButton_set_topic, SIGNAL(clicked()), this, SLOT(topic_changed()));
    QObject::connect(ui.pushButton_set_desc, SIGNAL(clicked()), this, SLOT(desc_changed()));
    QObject::connect(ui.radioButton_status_pub, SIGNAL(clicked()), this, SLOT(status_pub()));
    QObject::connect(ui.radioButton_status_priv, SIGNAL(clicked()), this, SLOT(status_priv()));
    QObject::connect(ui.comboBox_category, SIGNAL(activated(int)), this, SLOT(category_changed(int)));
    QObject::connect(ui.radioButton_guardian_off, SIGNAL(clicked()), this, SLOT(guardian_inactive()));
    QObject::connect(ui.radioButton_guardian_on, SIGNAL(clicked()), this, SLOT(guardian_active()));
    QObject::connect(ui.comboBox_guardian_level, SIGNAL(activated(int)), this, SLOT(guardian_clicked(int)));
    QObject::connect(ui.pushButton_set_password, SIGNAL(clicked()), this, SLOT(password_changed()));
    QObject::connect(ui.pushButton_set_limit, SIGNAL(clicked()), this, SLOT(limit_changed()));
    QObject::connect(ui.radioButton_moderation_off, SIGNAL(clicked()), this, SLOT(moderated_inactive()));
    QObject::connect(ui.radioButton_moderation_on, SIGNAL(clicked()), this, SLOT(moderated_active()));
    QObject::connect(ui.radioButton_auditorium_off, SIGNAL(clicked()), this, SLOT(auditorium_inactive()));
    QObject::connect(ui.radioButton_auditorium_on, SIGNAL(clicked()), this, SLOT(auditorium_active()));

    QObject::connect(ui.pushButton_op_add, SIGNAL(clicked()), this, SLOT(button_op_add()));
    QObject::connect(ui.pushButton_op_del, SIGNAL(clicked()), this, SLOT(button_op_del()));
    QObject::connect(ui.pushButton_halfop_add, SIGNAL(clicked()), this, SLOT(button_halfop_add()));
    QObject::connect(ui.pushButton_halfop_del, SIGNAL(clicked()), this, SLOT(button_halfop_del()));
    QObject::connect(ui.pushButton_ban_add, SIGNAL(clicked()), this, SLOT(button_ban_add()));
    QObject::connect(ui.pushButton_ban_del, SIGNAL(clicked()), this, SLOT(button_ban_del()));
    QObject::connect(ui.pushButton_invite_add, SIGNAL(clicked()), this, SLOT(button_invite_add()));
    QObject::connect(ui.pushButton_invite_del, SIGNAL(clicked()), this, SLOT(button_invite_del()));

    QObject::connect(ui.buttonBox, SIGNAL(accepted()), this, SLOT(button_ok()));
    QObject::connect(ui.buttonBox, SIGNAL(rejected()), this, SLOT(button_cancel()));
}

void DlgChannelSettings::set_channel(QString param1)
{
    strChannel = param1;
    pNetwork->send(QString("CS INFO %1").arg(strChannel));
}

void DlgChannelSettings::add_topic(QString strCheckChannel, QString strTopic)
{
    if (strCheckChannel != strChannel) return; // not this channel

    // convert emoticons
    strTopic.replace(QRegExp("%I([a-zA-Z0-9_-]+)%"), "//\\1");

    ui.plainTextEdit_topic->clear();
    ui.plainTextEdit_topic->insertPlainText(strTopic);
}

void DlgChannelSettings::add_pubpriv(QString strCheckChannel, int i)
{
    if (strCheckChannel != strChannel) return; // not this channel

    if (i == 1)
        ui.radioButton_status_priv->setChecked(true);
    else if (i == 0)
        ui.radioButton_status_pub->setChecked(true);
}

void DlgChannelSettings::add_owner(QString strCheckChannel, QString strNick)
{
    if (strCheckChannel != strChannel) return; // not this channel

    ui.label_owner_nick->clear();
    ui.label_owner_nick->setText(strNick);
}

void DlgChannelSettings::add_email(QString strCheckChannel, QString strEmail)
{
    if (strCheckChannel != strChannel) return; // not this channel

    ui.lineEdit_email->setText(strEmail);
}

void DlgChannelSettings::add_cat(QString strCheckChannel, int iCatMajor, int iCatMinor)
{
    if (strCheckChannel != strChannel) return; // not this channel

    Q_UNUSED (iCatMinor);

    if (iCatMajor == 1) // teen
        ui.comboBox_category->setCurrentIndex(0);
    else if (iCatMajor == 2) // towarzyskie
        ui.comboBox_category->setCurrentIndex(1);
    else if (iCatMajor == 3) // erotyczne
        ui.comboBox_category->setCurrentIndex(2);
    else if (iCatMajor == 4) // tematyczne
        ui.comboBox_category->setCurrentIndex(3);
    else if (iCatMajor == 5) // regionalne
        ui.comboBox_category->setCurrentIndex(4);
    else
        ui.comboBox_category->setCurrentIndex(-1);
}

void DlgChannelSettings::add_guardian(QString strCheckChannel, int iGuardianLevel)
{
    if (strCheckChannel != strChannel) return; // not this channel

    if (iGuardianLevel == 0)
    {
        ui.radioButton_guardian_off->setChecked(true);
        ui.comboBox_guardian_level->setCurrentIndex(-1);
    }
    else if (iGuardianLevel == 1)
    {
        ui.radioButton_guardian_on->setChecked(true);
        ui.comboBox_guardian_level->setCurrentIndex(0);
    }
    else if (iGuardianLevel == 2)
    {
        ui.radioButton_guardian_on->setChecked(true);
        ui.comboBox_guardian_level->setCurrentIndex(1);
    }
    else if (iGuardianLevel == 3)
    {
        ui.radioButton_guardian_on->setChecked(true);
        ui.comboBox_guardian_level->setCurrentIndex(2);
    }
}

void DlgChannelSettings::add_moderated(QString strCheckChannel, int iModerated)
{
    if (strCheckChannel != strChannel) return; // not this channel

    if (iModerated == 0)
        ui.radioButton_moderation_off->setChecked(true);
    else if (iModerated == 1)
        ui.radioButton_moderation_on->setChecked(true);
}

void DlgChannelSettings::add_www(QString strCheckChannel, QString strLink)
{
    if (strCheckChannel != strChannel) return; // not this channel

    ui.lineEdit_website->clear();
    ui.lineEdit_website->setText(strLink);
}

void DlgChannelSettings::add_created(QString strCheckChannel, QString strTime)
{
    if (strCheckChannel != strChannel) return; // not this channel

    QDateTime dt = QDateTime::fromTime_t(strTime.toInt());
    QString strDT = dt.toString("dd/MM/yyyy hh:mm:ss");
    ui.label_datetime->clear();
    ui.label_datetime->setText(strDT);
}

void DlgChannelSettings::add_password(QString strCheckChannel, QString strPassword)
{
    if (strCheckChannel != strChannel) return; // not this channel

    ui.lineEdit_password->clear();
    ui.lineEdit_password->setText(strPassword);
}

void DlgChannelSettings::add_limit(QString strCheckChannel, int iLimit)
{
    if (strCheckChannel != strChannel) return; // not this channel

    ui.spinBox_limit->setValue(iLimit);
}

void DlgChannelSettings::add_auditorium(QString strCheckChannel, int iAuditorium)
{
    if (strCheckChannel != strChannel) return; // not this channel

    if (iAuditorium == 0)
        ui.radioButton_auditorium_off->setChecked(true);
    else if (iAuditorium == 1)
        ui.radioButton_auditorium_on->setChecked(true);
}

void DlgChannelSettings::add_description(QString strCheckChannel, QString strDescription)
{
    if (strCheckChannel != strChannel) return; // not this channel

    ui.plainTextEdit_desc->clear();
    ui.plainTextEdit_desc->insertPlainText(strDescription);
}

void DlgChannelSettings::add_op(QString strCheckChannel, QString strNick)
{
    if (strCheckChannel != strChannel) return; // not this channel

    if (exist_item(strNick, ui.tableWidget_op) == false)
    {
        ui.tableWidget_op->insertRow(ui.tableWidget_op->rowCount());
        ui.tableWidget_op->setItem(ui.tableWidget_op->rowCount()-1, 0, new QTableWidgetItem(strNick));
    }
}

void DlgChannelSettings::add_halfop(QString strCheckChannel, QString strNick)
{
    if (strCheckChannel != strChannel) return; // not this channel

    if (exist_item(strNick, ui.tableWidget_halfop) == false)
    {
        ui.tableWidget_halfop->insertRow(ui.tableWidget_halfop->rowCount());
        ui.tableWidget_halfop->setItem(ui.tableWidget_halfop->rowCount()-1, 0, new QTableWidgetItem(strNick));
    }
}

void DlgChannelSettings::add_ban(QString strCheckChannel, QString strNick, QString strWho, QString strDT)
{
    if (strCheckChannel != strChannel) return; // not this channel

    if (exist_item(strNick, ui.tableWidget_ban) == false)
    {
        ui.tableWidget_ban->insertRow(ui.tableWidget_ban->rowCount());
        ui.tableWidget_ban->setItem(ui.tableWidget_ban->rowCount()-1, 0, new QTableWidgetItem(strNick));
        ui.tableWidget_ban->setItem(ui.tableWidget_ban->rowCount()-1, 1, new QTableWidgetItem(strWho));
        ui.tableWidget_ban->setItem(ui.tableWidget_ban->rowCount()-1, 2, new QTableWidgetItem(strDT));
    }
}

void DlgChannelSettings::add_invite(QString strCheckChannel, QString strNick, QString strWho, QString strDT)
{
    if (strCheckChannel != strChannel) return; // not this channel

    if (exist_item(strNick, ui.tableWidget_invite) == false)
    {
        ui.tableWidget_invite->insertRow(ui.tableWidget_invite->rowCount());
        ui.tableWidget_invite->setItem(ui.tableWidget_invite->rowCount()-1, 0, new QTableWidgetItem(strNick));
        ui.tableWidget_invite->setItem(ui.tableWidget_invite->rowCount()-1, 1, new QTableWidgetItem(strWho));
        ui.tableWidget_invite->setItem(ui.tableWidget_invite->rowCount()-1, 2, new QTableWidgetItem(strDT));
    }
}

void DlgChannelSettings::owner_changed()
{
    QString strMsg = "<p style=\"font-weight:bold;\">"+tr("The owner of the channel can be only one!")+"</p><p>"+tr("Enter the nickname of the new owner:")+"</p>";
    bool ok;
    QString strNick = QInputDialog::getText(this, tr("Changing privileges"), strMsg, QLineEdit::Normal, QString::null, &ok);

    if ((ok == true) && (strNick.isEmpty() == false))
        pNetwork->send(QString("CS TRANSFER %1 %2").arg(strChannel).arg(strNick));

    clear();
    ui.label_channel_name->setText(strChannel);
    pNetwork->send(QString("CS INFO %1").arg(strChannel));
}

void DlgChannelSettings::remove_channel_clicked()
{
    QString strMsg = "<p style=\"font-weight:bold;\">"+tr("The removal of the channel operation is irreversible!")+"</p>";
    bool ok;
    QString strText = QInputDialog::getText(this, tr("Changing privileges"), strMsg, QLineEdit::Normal, strChannel, &ok);

    if ((ok == true) && (strText.isEmpty() == false))
        pNetwork->send(QString("CS DROP %1").arg(strText));

    strChannel.clear();
    clear();
    this->hide();
}

void DlgChannelSettings::email_changed()
{
    (new DlgEmail(myparent, strChannel, ui.lineEdit_email->text()))->show();
}

void DlgChannelSettings::www_changed()
{
    pNetwork->send(QString("CS SET %1 WWW %2").arg(strChannel).arg(ui.lineEdit_website->text()));
    pNetwork->send(QString("CS INFO %1").arg(strChannel));
}

void DlgChannelSettings::topic_changed()
{
    QString strTopic = ui.plainTextEdit_topic->toPlainText();
    strTopic.replace(QRegExp("(\r|\n)"), "");
    strTopic.replace(QRegExp("//([a-zA-Z0-9_-]+)"), "%I\\1%");

    pNetwork->send(QString("CS SET %1 TOPIC %2").arg(strChannel).arg(strTopic));
    pNetwork->send(QString("CS INFO %1").arg(strChannel));
}

void DlgChannelSettings::desc_changed()
{
    pNetwork->send(QString("CS SET %1 LONGDESC %2").arg(strChannel).arg(ui.plainTextEdit_desc->toPlainText().replace(QRegExp("(\r|\n)"), "")));
    pNetwork->send(QString("CS INFO %1").arg(strChannel));
}

void DlgChannelSettings::status_pub()
{
    pNetwork->send(QString("CS SET %1 PRIVATE OFF").arg(strChannel));
    pNetwork->send(QString("CS INFO %1").arg(strChannel));
}

void DlgChannelSettings::status_priv()
{
    pNetwork->send(QString("CS SET %1 PRIVATE ON").arg(strChannel));
    pNetwork->send(QString("CS INFO %1").arg(strChannel));
}

void DlgChannelSettings::category_changed(int index)
{
    pNetwork->send(QString("CS SET %1 CATMAJOR %2").arg(strChannel).arg(index+1));
    pNetwork->send(QString("CS INFO %1").arg(strChannel));
}

void DlgChannelSettings::guardian_inactive()
{
    pNetwork->send(QString("CS SET %1 GUARDIAN 0").arg(strChannel));
    pNetwork->send(QString("CS INFO %1").arg(strChannel));
}

void DlgChannelSettings::guardian_active()
{
    if (ui.comboBox_guardian_level->currentIndex() != -1)
        pNetwork->send(QString("CS SET %1 GUARDIAN %2").arg(strChannel).arg(ui.comboBox_guardian_level->currentIndex()));
    else
        pNetwork->send(QString("CS SET %1 GUARDIAN 1").arg(strChannel));
    pNetwork->send(QString("CS INFO %1").arg(strChannel));
}

void DlgChannelSettings::guardian_clicked(int iLevel)
{
    if (ui.radioButton_guardian_on->isChecked() == true)
        pNetwork->send(QString("CS SET %1 GUARDIAN %2").arg(strChannel).arg(iLevel+1));
    pNetwork->send(QString("CS INFO %1").arg(strChannel));
}

void DlgChannelSettings::password_changed()
{
    pNetwork->send(QString("CS SET %1 PASSWORD %2").arg(strChannel).arg(ui.lineEdit_password->text()));
    pNetwork->send(QString("CS INFO %1").arg(strChannel));
}

void DlgChannelSettings::limit_changed()
{
    pNetwork->send(QString("CS SET %1 LIMIT %2").arg(strChannel).arg(ui.spinBox_limit->value()));
    pNetwork->send(QString("CS INFO %1").arg(strChannel));
}

void DlgChannelSettings::moderated_inactive()
{
    pNetwork->send(QString("CS SET %1 MODERATED OFF").arg(strChannel));
    pNetwork->send(QString("CS INFO %1").arg(strChannel));
}

void DlgChannelSettings::moderated_active()
{
    pNetwork->send(QString("CS SET %1 MODERATED ON").arg(strChannel));
    pNetwork->send(QString("CS INFO %1").arg(strChannel));
}

void DlgChannelSettings::auditorium_inactive()
{
    pNetwork->send(QString("CS SET %1 AUDITORIUM OFF").arg(strChannel));
    pNetwork->send(QString("CS INFO %1").arg(strChannel));
}

void DlgChannelSettings::auditorium_active()
{
    pNetwork->send(QString("CS SET %1 AUDITORIUM ON").arg(strChannel));
    pNetwork->send(QString("CS INFO %1").arg(strChannel));
}

void DlgChannelSettings::button_op_add()
{
    bool ok;
    QString strNick = QInputDialog::getText(this, tr("Changing privileges"), tr("Add super-operator:"), QLineEdit::Normal, QString::null, &ok);

    if ((ok == true) && (strNick.isEmpty() == false))
        pNetwork->send(QString("CS OP %1 ADD %2").arg(strChannel).arg(strNick));

    clear();
    ui.label_channel_name->setText(strChannel);
    pNetwork->send(QString("CS INFO %1").arg(strChannel));
}

void DlgChannelSettings::button_op_del()
{
    if (ui.tableWidget_op->selectedItems().isEmpty() == false)
    {
        QString strRemoveNick = ui.tableWidget_op->selectedItems().at(0)->text();
        pNetwork->send(QString("CS OP %1 DEL %2").arg(strChannel).arg(strRemoveNick));
        pNetwork->send(QString("CS INFO %1").arg(strChannel));
        clear();
    }
    else
    {
        bool ok;
        QString strNick = QInputDialog::getText(this, tr("Changing privileges"), tr("Remove the super-operator:"), QLineEdit::Normal, QString::null, &ok);

        if ((ok == true) && (strNick.isEmpty() == false))
            pNetwork->send(QString("CS OP %1 DEL %2").arg(strChannel).arg(strNick));

        clear();
        ui.label_channel_name->setText(strChannel);
        pNetwork->send(QString("CS INFO %1").arg(strChannel));
    }
}

void DlgChannelSettings::button_halfop_add()
{
    bool ok;
    QString strNick = QInputDialog::getText(this, tr("Changing privileges"), tr("Add operator:"), QLineEdit::Normal, QString::null, &ok);

    if ((ok == true) && (strNick.isEmpty() == false))
        pNetwork->send(QString("CS HALFOP %1 ADD %2").arg(strChannel).arg(strNick));

    clear();
    ui.label_channel_name->setText(strChannel);
    pNetwork->send(QString("CS INFO %1").arg(strChannel));
}

void DlgChannelSettings::button_halfop_del()
{
    if (ui.tableWidget_halfop->selectedItems().isEmpty() == false)
    {
        QString strRemoveNick = ui.tableWidget_halfop->selectedItems().at(0)->text();
        pNetwork->send(QString("CS HALFOP %1 DEL %2").arg(strChannel).arg(strRemoveNick));
        pNetwork->send(QString("CS INFO %1").arg(strChannel));
        clear();
    }
    else
    {
        bool ok;
        QString strNick = QInputDialog::getText(this, tr("Changing privileges"), tr("Remove operator:"), QLineEdit::Normal, QString::null, &ok);

        if ((ok == true) && (strNick.isEmpty() == false))
            pNetwork->send(QString("CS HALFOP %1 DEL %2").arg(strChannel).arg(strNick));

        clear();
        ui.label_channel_name->setText(strChannel);
        pNetwork->send(QString("CS INFO %1").arg(strChannel));
    }
}

void DlgChannelSettings::button_ban_add()
{
    bool ok;
    QString strNick = QInputDialog::getText(this, tr("Changing privileges"), tr("Add ban:"), QLineEdit::Normal, QString::null, &ok);

    if ((ok == true) && (strNick.isEmpty() == false))
    {
        if (strNick.contains("*") == true)
        {
            QMessageBox msgBox;
            msgBox.setIcon(QMessageBox::Warning);
            msgBox.setWindowIcon(QIcon(":/images/logo_64.png"));
            msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
            msgBox.setWindowTitle(tr("Warning"));
            msgBox.setText(tr("This ban will ban large numbers of people. Are you sure you want to use this ban?"));
            int iResult = msgBox.exec();

            if (iResult == QMessageBox::Ok)
                pNetwork->send(QString("CS BAN %1 ADD %2").arg(strChannel).arg(strNick));
        }
        else
            pNetwork->send(QString("CS BAN %1 ADD %2").arg(strChannel).arg(strNick));
    }

    clear();
    ui.label_channel_name->setText(strChannel);
    pNetwork->send(QString("CS INFO %1").arg(strChannel));
}

void DlgChannelSettings::button_ban_del()
{
    if (ui.tableWidget_ban->selectedItems().isEmpty() == false)
    {
        QString strRemoveNick = ui.tableWidget_ban->selectedItems().at(0)->text();
        pNetwork->send(QString("CS BAN %1 DEL %2").arg(strChannel).arg(strRemoveNick));
        pNetwork->send(QString("CS INFO %1").arg(strChannel));
        clear();
    }
    else
    {
        bool ok;
        QString strNick = QInputDialog::getText(this, tr("Changing privileges"), tr("Remove ban:"), QLineEdit::Normal, QString::null, &ok);

        if ((ok == true) && (strNick.isEmpty() == false))
            pNetwork->send(QString("CS BAN %1 DEL %2").arg(strChannel).arg(strNick));

        clear();
        ui.label_channel_name->setText(strChannel);
        pNetwork->send(QString("CS INFO %1").arg(strChannel));
    }
}

void DlgChannelSettings::button_invite_add()
{
    bool ok;
    QString strNick = QInputDialog::getText(this, tr("Changing privileges"), tr("Add invitation:"), QLineEdit::Normal, QString::null, &ok);

    if ((ok == true) && (strNick.isEmpty() == false))
        pNetwork->send(QString("CS INVITE %1 ADD %2").arg(strChannel).arg(strNick));

    clear();
    ui.label_channel_name->setText(strChannel);
    pNetwork->send(QString("CS INFO %1").arg(strChannel));
}

void DlgChannelSettings::button_invite_del()
{
    if (ui.tableWidget_invite->selectedItems().isEmpty() == false)
    {
        QString strRemoveNick = ui.tableWidget_invite->selectedItems().at(0)->text();
        pNetwork->send(QString("CS INVITE %1 DEL %2").arg(strChannel).arg(strRemoveNick));
        pNetwork->send(QString("CS INFO %1").arg(strChannel));
        clear();
    }
    else
    {
        bool ok;
        QString strNick = QInputDialog::getText(this, tr("Changing privileges"), tr("Delete invitation:"), QLineEdit::Normal, QString::null, &ok);

        if ((ok == true) && (strNick.isEmpty() == false))
            pNetwork->send(QString("CS INVITE %1 DEL %2").arg(strChannel).arg(strNick));

        clear();
        ui.label_channel_name->setText(strChannel);
        pNetwork->send(QString("CS INFO %1").arg(strChannel));
    }
}

void DlgChannelSettings::button_ok()
{
    strChannel.clear();
    clear();
    this->hide();
}

void DlgChannelSettings::button_cancel()
{
    strChannel.clear();
    clear();
    this->hide();
}

bool DlgChannelSettings::exist_item(QString strItem, QTableWidget *list)
{
    for (int i = 0; i < list->rowCount(); i++)
    {
        if (list->item(0, i)->text() == strItem)
            return true;
    }
    return false;
}

void DlgChannelSettings::clear()
{
    ui.tableWidget_op->clear();
    ui.tableWidget_halfop->clear();
    ui.tableWidget_ban->clear();
    ui.tableWidget_invite->clear();

    ui.tableWidget_op->setRowCount(0);
    ui.tableWidget_halfop->setRowCount(0);
    ui.tableWidget_ban->setRowCount(0);
    ui.tableWidget_invite->setRowCount(0);

    QStringList strlLabels;
    strlLabels << tr("Nick");

    QStringList strlLabels2;
    strlLabels2 << tr("Nick") << tr("Created by") << tr("Date/Time");

    ui.tableWidget_op->setHorizontalHeaderLabels(strlLabels);
    ui.tableWidget_halfop->setHorizontalHeaderLabels(strlLabels);
    ui.tableWidget_ban->setHorizontalHeaderLabels(strlLabels2);
    ui.tableWidget_invite->setHorizontalHeaderLabels(strlLabels2);

    // prevents crash!
    ui.tableWidget_op->setSortingEnabled(false);
    ui.tableWidget_halfop->setSortingEnabled(false);
    ui.tableWidget_ban->setSortingEnabled(false);
    ui.tableWidget_invite->setSortingEnabled(false);

    ui.lineEdit_email->clear();
    ui.lineEdit_website->clear();
    ui.lineEdit_password->clear();
    ui.plainTextEdit_topic->clear();
    ui.plainTextEdit_desc->clear();
    ui.label_channel_name->clear();
    ui.label_owner_nick->clear();
    ui.label_datetime->clear();
    ui.spinBox_limit->setValue(0);
    ui.comboBox_category->setCurrentIndex(-1);
    ui.comboBox_guardian_level->setCurrentIndex(-1);
    ui.radioButton_status_pub->setChecked(false);
    ui.radioButton_status_priv->setChecked(false);
    ui.radioButton_guardian_off->setChecked(false);
    ui.radioButton_guardian_on->setChecked(false);
    ui.radioButton_moderation_off->setChecked(false);
    ui.radioButton_moderation_on->setChecked(false);
    ui.radioButton_auditorium_off->setChecked(false);
    ui.radioButton_auditorium_on->setChecked(false);
}

void DlgChannelSettings::showEvent(QShowEvent *event)
{
    event->accept();
    // center screen
    move(QApplication::desktop()->screen()->rect().center() - rect().center());

    clear();

    ui.label_channel_name->setText(strChannel);
}

void DlgChannelSettings::hideEvent(QHideEvent *event)
{
    event->accept();
    strChannel.clear();
    clear();
}

void DlgChannelSettings::closeEvent(QCloseEvent *event)
{
    event->ignore();
    this->hide();
}
