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

#include <QDate>
#include <QDesktopWidget>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QPushButton>
#include "core/core.h"
#include "common/convert.h"
#include "models/avatar.h"
#include "models/my_profile.h"
#include "models/utils.h"
#include "my_profile_gui.h"

MyProfileGui::MyProfileGui(QWidget *parent) : QDialog(parent)
{
    ui.setupUi(this);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowTitle(tr("My profile"));
    // center screen
    move(QApplication::desktop()->screenGeometry(QApplication::desktop()->screenNumber(parent)).center()  - rect().center());

    createGui();
    setDefaultValues();
    createSignals();

    accessManager = new QNetworkAccessManager;
    connect(accessManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(avatarFinished(QNetworkReply*)));

    refresh();
}

MyProfileGui::~MyProfileGui()
{
    accessManager->deleteLater();
}

void MyProfileGui::createGui()
{
    ui.toolButton_bold->setIcon(QIcon(":/images/breeze/format-text-bold.svg"));
    ui.toolButton_italic->setIcon(QIcon(":/images/breeze/format-text-italic.svg"));
    ui.buttonBox->button(QDialogButtonBox::Ok)->setIcon(QIcon(":/images/breeze/dialog-ok.svg"));
    ui.buttonBox->button(QDialogButtonBox::Cancel)->setIcon(QIcon(":/images/breeze/dialog-cancel.svg"));

    ui.label_ltype->setText(tr("Type:"));
    ui.label_sex->setText(tr("Sex:"));
    ui.label_birthdate->setText(tr("Birthdate:"));
    ui.label_city->setText(tr("City:"));
    ui.label_country->setText(tr("Country:"));
    ui.label_hobby->setText(tr("Hobby:"));
    ui.label_www->setText(tr("Website:"));
    ui.buttonBox->button(QDialogButtonBox::Ok)->setText(tr("Ok"));
    ui.buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Cancel"));
}

void MyProfileGui::setDefaultValues()
{
    // font
    QList<QString> lFonts = Utils::instance()->getFonts();
    foreach (const QString &strFont, lFonts)
        ui.comboBox_font->addItem(strFont);

    // color
    ui.comboBox_color->setToolTip(tr("Font color"));
    ui.comboBox_color->setIconSize(QSize(50,10));

    QList<QString> lColors = Utils::instance()->getColors();
    foreach (const QString &strColor, lColors)
    {
        QPixmap pixmap(50,10);
        pixmap.fill(QColor("#"+strColor));
        ui.comboBox_color->addItem(QIcon(pixmap), QString::null);
    }

    // sex
    QList<QString> lSex = Utils::instance()->getSex();
    foreach (const QString &strSex, lSex)
        ui.comboBox_sex->addItem(strSex);

    // days
    QList<QString> lDays = Utils::instance()->getDays();
    foreach (const QString &strDay, lDays)
        ui.comboBox_day->addItem(strDay);

    // months
    QList<QString> lMonths = Utils::instance()->getMonths();
    foreach (const QString &strMonth, lMonths)
        ui.comboBox_month->addItem(strMonth);

    // years
    QList<QString> lYears = Utils::instance()->getYears();
    foreach (const QString &strYear, lYears)
        ui.comboBox_year->addItem(strYear);

    // country
    QString strCountries = Utils::instance()->getCountries();
    QStringList lCountries = strCountries.split(",");
    lCountries.insert(0, QString::null);
    foreach (const QString &strCountry, lCountries)
        ui.comboBox_country->addItem(strCountry);
}

void MyProfileGui::createSignals()
{
    connect(ui.buttonBox, SIGNAL(accepted()), this, SLOT(buttonOk()));
    connect(ui.buttonBox, SIGNAL(rejected()), this, SLOT(close()));
}

void MyProfileGui::avatarFinished(QNetworkReply *reply)
{
    reply->deleteLater();

    // if errors
    if (reply->error())
        return;

    QVariant possibleRedirectUrl = reply->attribute(QNetworkRequest::RedirectionTargetAttribute);
    if (!possibleRedirectUrl.toUrl().isEmpty())
    {
        accessManager->get(QNetworkRequest(possibleRedirectUrl.toUrl()));
        return;
    }

    QByteArray bData = reply->readAll();

    // show avatar
    if (!bData.isEmpty())
    {
        QPixmap avatar;
        avatar.loadFromData(bData);
        ui.label_avatar->setPixmap(avatar.scaled(QSize(50,50), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
    else
        ui.label_avatar->setPixmap(Avatar::instance()->getEmptyRegisteredUserAvatar());
}

void MyProfileGui::refresh()
{
    QHashIterator <QString, QString> it(MyProfile::instance()->getAll());
    while (it.hasNext())
    {
        it.next();
        QString strKey = it.key();
        QString strValue = it.value();

        if (strKey == "avatar")
        {
            if (!strValue.isEmpty())
                accessManager->get(QNetworkRequest(QUrl(strValue)));
            else
                ui.label_avatar->setPixmap(QPixmap(Avatar::instance()->getEmptyRegisteredUserAvatar()));
        }
        else if (strKey == "birthdate")
        {
            if (!strValue.isEmpty())
            {
                QStringList lDate = strValue.split("-");
                QString strYear = lDate.value(0, 0);
                QString strMonth = lDate.value(1, 0);
                QString strDay = lDate.value(2, 0);

                strMonth = Utils::instance()->convertIntToMonth(strMonth);

                ui.comboBox_day->setCurrentIndex(getIndex(ui.comboBox_day, strDay));
                ui.comboBox_month->setCurrentIndex(getIndex(ui.comboBox_month, strMonth));
                ui.comboBox_year->setCurrentIndex(getIndex(ui.comboBox_year, strYear));
            }
            else
            {
                ui.comboBox_day->setCurrentIndex(0);
                ui.comboBox_month->setCurrentIndex(0);
                ui.comboBox_year->setCurrentIndex(0);
            }
        }
        else if (strKey == "city")
            ui.lineEdit_city->setText(strValue);
        else if (strKey == "country")
            ui.comboBox_country->setCurrentIndex(getIndex(ui.comboBox_country, Utils::instance()->convertCodeToCountry(strValue)));
        else if (strKey == "longDesc")
            ui.plainTextEdit_hobby->setPlainText(strValue);
        else if (strKey == "sex")
        {
            int iSex = Utils::instance()->convertSexToInt(strValue);
            ui.comboBox_sex->setCurrentIndex(iSex);
        }
        else if (strKey == "shortDesc")
            ui.plainTextEdit_desc->setPlainText(convertTextToDesc(strValue));
        else if (strKey == "type")
            ui.label_type->setText(Utils::instance()->convertIntToType(strValue.toInt()));
        else if (strKey == "www")
            ui.lineEdit_www->setText(strValue);
    }
}

int MyProfileGui::getIndex(QComboBox *comboBox, const QString &strFindText)
{
    return comboBox->findText(strFindText);
}

QString MyProfileGui::convertTextToDesc(QString strContent)
{
    // convert font
    bool bBold = Convert::isBold(strContent);
    bool bItalic = Convert::isItalic(strContent);
    QString strFont = Convert::getFont(strContent);
    int iColor = Convert::getColor(strContent);

    if (bBold) ui.toolButton_bold->setChecked(true);
    else if (bItalic) ui.toolButton_italic->setChecked(true);

    if (strFont == "arial") ui.comboBox_font->setCurrentIndex(0);
    else if (strFont == "times") ui.comboBox_font->setCurrentIndex(1);
    else if (strFont == "verdana") ui.comboBox_font->setCurrentIndex(2);
    else if (strFont == "tahoma") ui.comboBox_font->setCurrentIndex(3);
    else if (strFont == "courier") ui.comboBox_font->setCurrentIndex(4);
    else ui.comboBox_font->setCurrentIndex(2);

    ui.comboBox_color->setCurrentIndex(iColor);

    // default #000000
    if (ui.comboBox_color->currentIndex() == -1)
        ui.comboBox_color->setCurrentIndex(0);

    // default verdana
    if (ui.comboBox_font->currentIndex() == -1)
        ui.comboBox_font->setCurrentIndex(2);

    // convert emoticons
    Convert::simpleConvert(strContent);

    return strContent;
}

QString MyProfileGui::convertDescToText(QString strContent)
{
    strContent.remove(QRegExp("(\r|\n)"));
    Convert::simpleReverseConvert(strContent);

    bool bBold = false;
    bool bItalic = false;
    QString strFontName;
    QString strFontColor;
    QString strFontWeight;

    // bold
    if (ui.toolButton_bold->isChecked()) bBold = true;

    // italic
    if (ui.toolButton_italic->isChecked()) bItalic = true;

    // font name
    strFontName = ui.comboBox_font->currentText().toLower();

    // font color
    QList<QString> lFontColors = Utils::instance()->getColors();
    if (ui.comboBox_color->currentIndex() != -1)
        strFontColor = "#"+lFontColors.at(ui.comboBox_color->currentIndex());

    // set topic
    if (bBold) strFontWeight += "b";
    if (bItalic) strFontWeight += "i";

    if (strFontName == "verdana")
        strFontName = QString::null;
    if ((strFontColor != "#000000") && (!strFontColor.isEmpty()))
        strContent = "%C"+strFontColor.right(6)+"%"+strContent;
    if (!strFontName.isEmpty())
        strFontName = ":"+strFontName;
    if ((!strFontWeight.isEmpty()) || (!strFontName.isEmpty()))
        strContent = "%F"+strFontWeight+strFontName+"%"+strContent;

    return strContent;
}

void MyProfileGui::buttonOk()
{
    // set desc - shortDesc
    QString strDesc = convertDescToText(ui.plainTextEdit_desc->toPlainText());
    if (strDesc != MyProfile::instance()->get("shortDesc"))
        Core::instance()->network->send(QString("NS SET shortDesc %1").arg(strDesc));

    // set sex
    int iSex = ui.comboBox_sex->currentIndex();
    QString strSex = Utils::instance()->convertIntToSexChar(iSex);
    if (strSex != MyProfile::instance()->get("sex"))
        Core::instance()->network->send(QString("NS SET sex %1").arg(strSex));

    // set birthdate
    QString strDay = ui.comboBox_day->currentText();
    QString strMonth = Utils::instance()->convertMonthToInt(ui.comboBox_month->currentText());
    QString strYear = ui.comboBox_year->currentText();
    QString strBirthdate;
    if ((!strDay.isEmpty()) && (!strMonth.isEmpty()) && (!strYear.isEmpty()))
        strBirthdate = QString("%1-%2-%3").arg(strYear, strMonth, strDay);
    if (strBirthdate != MyProfile::instance()->get("birthdate"))
        Core::instance()->network->send(QString("NS SET birthdate %1").arg(strBirthdate));

    // set city
    QString strCity = ui.lineEdit_city->text().trimmed();
    if (strCity != MyProfile::instance()->get("city"))
        Core::instance()->network->send(QString("NS SET city %1").arg(strCity));

    // set country
    QString strCountry = ui.comboBox_country->currentText();
    QString strCountryCode = Utils::instance()->convertCountryToCode(strCountry);
    if (strCountryCode != MyProfile::instance()->get("country"))
        Core::instance()->network->send(QString("NS SET country %1").arg(strCountryCode));

    // set hobby - longDesc
    QString strHobby = ui.plainTextEdit_hobby->toPlainText();
    if (strHobby != MyProfile::instance()->get("longDesc"))
        Core::instance()->network->send(QString("NS SET longDesc %1").arg(strHobby));

    // set www
    QString strWWW = ui.lineEdit_www->text().trimmed();
    if (strWWW != MyProfile::instance()->get("www"))
        Core::instance()->network->send(QString("NS SET www %1").arg(strWWW));

    // close
    this->close();
}
