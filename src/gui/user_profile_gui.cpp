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

#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QTextEdit>
#include <QToolButton>
#include <QVBoxLayout>

#include <QDesktopWidget>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QTimer>
#include "common/convert.h"
#include "core/core.h"
#include "user_avatar_gui.h"
#include "models/avatar.h"
#include "models/user_profile.h"
#include "models/utils.h"
#include "user_profile_gui.h"

UserProfileGui::UserProfileGui(const QString &_strNick, QWidget *parent) : QDialog(parent), strNick(_strNick)
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowTitle(tr("Profile"));

    createGui();
    createSignals();

    // max height
    setMinimumSize(285, sizeHint().height());
    setMaximumSize(285, sizeHint().height());

    // center screen
    move(QApplication::desktop()->screenGeometry(QApplication::desktop()->screenNumber(parent)).center()  - rect().center());

    accessManager = new QNetworkAccessManager;
    connect(accessManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(avatarFinished(QNetworkReply*)));

    // set nick
    UserProfile::instance()->setNick(strNick);

    //set label
    label_nick->setText(strNick);

    // get new user info
    Core::instance()->network->send(QString("NS INFO %1").arg(strNick));

    // refresh
    QTimer::singleShot(200, this, SLOT(refreshUserInfo())); // 0.2 sec
}

UserProfileGui::~UserProfileGui()
{
    accessManager->deleteLater();
}

void UserProfileGui::createGui()
{
    toolButton_zoom = new QToolButton();
    toolButton_zoom->setEnabled(false);
    toolLayout = new QVBoxLayout();
    toolLayout->setMargin(0);
    toolLayout->addStretch();
    toolLayout->addWidget(toolButton_zoom);
    toolWidget = new QWidget(this);
    toolWidget->setLayout(toolLayout);

    label_avatar = new QLabel();
    label_avatar->setMinimumSize(50,50);
    label_avatar->setWordWrap(true);
    label_avatar->setAlignment(Qt::AlignCenter);
    textEdit_desc = new QTextEdit();
    textEdit_desc->setMinimumSize(190,70);
    textEdit_desc->setMaximumSize(190,70);
    textEdit_desc->setReadOnly(true);
    topLayout = new QHBoxLayout();
    topLayout->setMargin(0);
    topLayout->addWidget(toolWidget);
    topLayout->addWidget(label_avatar);
    topLayout->addWidget(textEdit_desc);
    topWidget = new QWidget(this);
    topWidget->setLayout(topLayout);

    label_age = new QLabel();
    lineEdit_age = new QLineEdit();
    lineEdit_age->setAlignment(Qt::AlignCenter);
    lineEdit_age->setReadOnly(true);
    label_age->setBuddy(label_age);
    label_sex = new QLabel();
    lineEdit_sex = new QLineEdit();
    lineEdit_sex->setAlignment(Qt::AlignCenter);
    lineEdit_sex->setReadOnly(true);
    label_sex->setBuddy(label_sex);
    agesexLayout = new QHBoxLayout();
    agesexLayout->setMargin(0);
    agesexLayout->addWidget(label_age);
    agesexLayout->addWidget(lineEdit_age);
    agesexLayout->addWidget(label_sex);
    agesexLayout->addWidget(lineEdit_sex);
    agesexWidget = new QWidget(this);
    agesexWidget->setLayout(agesexLayout);

    lineEdit_birthdate = new QLineEdit();
    lineEdit_birthdate->setAlignment(Qt::AlignCenter);
    lineEdit_birthdate->setReadOnly(true);
    lineEdit_city = new QLineEdit();
    lineEdit_city->setAlignment(Qt::AlignCenter);
    lineEdit_city->setReadOnly(true);
    lineEdit_country = new QLineEdit();
    lineEdit_country->setAlignment(Qt::AlignCenter);
    lineEdit_country->setReadOnly(true);
    plainTextEdit_hobby = new QPlainTextEdit();
    plainTextEdit_hobby->setReadOnly(true);
    plainTextEdit_hobby->setMaximumHeight(50);
    plainTextEdit_hobby->setMaximumHeight(50);
    lineEdit_type = new QLineEdit();
    lineEdit_type->setAlignment(Qt::AlignCenter);
    lineEdit_type->setReadOnly(true);
    label_website_link = new QLabel();
    label_website_link->setOpenExternalLinks(true);
    moreLayout = new QFormLayout();
    moreLayout->setMargin(0);
    moreLayout->addRow(tr("Birthdate:"), lineEdit_birthdate);
    moreLayout->addRow(tr("City:"), lineEdit_city);
    moreLayout->addRow(tr("Country:"), lineEdit_country);
    moreLayout->addRow(tr("Hobby:"), plainTextEdit_hobby);
    moreLayout->addRow(tr("Type:"), lineEdit_type);
    moreLayout->addRow(tr("Website:"), label_website_link);
    moreWidget = new QWidget(this);
    moreWidget->setLayout(moreLayout);

    pushButton_more = new QPushButton();
    pushButton_close = new QPushButton();
    pushButtonLayout = new QHBoxLayout();
    pushButtonLayout->setMargin(0);
    pushButtonLayout->addWidget(pushButton_more);
    pushButtonLayout->addStretch();
    pushButtonLayout->addWidget(pushButton_close);
    pushButtonWidget = new QWidget(this);
    pushButtonWidget->setLayout(pushButtonLayout);

    label_nick = new QLabel();
    label_nick->setAlignment(Qt::AlignCenter);
    label_nick->setStyleSheet("font-weight:bold;");
    mainLayout = new QVBoxLayout();
    mainLayout->setMargin(5);
    mainLayout->setSizeConstraint(QLayout::SetMinimumSize);
    mainLayout->addWidget(label_nick);
    mainLayout->addWidget(topWidget);
    mainLayout->addWidget(agesexWidget);
    mainLayout->addWidget(moreWidget);
    mainLayout->addWidget(pushButtonWidget);
    this->setLayout(mainLayout);

    toolButton_zoom->setIcon(QIcon(":/images/breeze/zoom-in.svg"));
    pushButton_more->setIcon(QIcon(":/images/breeze/go-down.svg"));
    pushButton_close->setIcon(QIcon(":/images/breeze/dialog-close.svg"));

    label_sex->setText(tr("Sex:"));
    label_age->setText(tr("Age:"));
    pushButton_more->setText(tr("More..."));
    pushButton_close->setText(tr("Close"));

    moreWidget->setVisible(false);
}

void UserProfileGui::createSignals()
{
    connect(toolButton_zoom, SIGNAL(clicked()), this, SLOT(buttonZoom()));
    connect(pushButton_more, SIGNAL(clicked()), this, SLOT(buttonMore()));
    connect(pushButton_close, SIGNAL(clicked()), this, SLOT(buttonClose()));
}

void UserProfileGui::refreshUserInfo()
{
    if (UserProfile::instance()->getStatus() != StatusCompleted)
    {
        QTimer::singleShot(200, this, SLOT(refreshUserInfo())); // 0.2 sec
        return;
    }

    QHashIterator<QString, QString> it(UserProfile::instance()->getAll());
    while (it.hasNext())
    {
        it.next();
        QString strKey = it.key();
        QString strValue = it.value();

        if (strKey == "avatar")
        {
            if (!strValue.isEmpty())
                showAvatar(strValue);
            else
                label_avatar->setPixmap(QPixmap(Avatar::instance()->getEmptyRegisteredUserAvatar()));
        }
        else if (strKey == "birthdate")
        {
            lineEdit_age->setText(Utils::instance()->convertDateToAge(strValue));
            lineEdit_birthdate->setText(strValue);
        }
        else if (strKey == "city")
            lineEdit_city->setText(strValue);
        else if (strKey == "country")
            lineEdit_country->setText(Utils::instance()->convertCodeToCountry(strValue));
        else if (strKey == "longDesc")
            plainTextEdit_hobby->setPlainText(strValue);
        else if (strKey == "sex")
            lineEdit_sex->setText(Utils::instance()->convertCharToSex(strValue));
        else if (strKey == "shortDesc")
            textEdit_desc->setHtml(convertDesc(strValue));
        else if (strKey == "type")
            lineEdit_type->setText(Utils::instance()->convertIntToType(strValue.toInt()));
        else if (strKey == "www")
        {
            QString strShortLink = strValue;
            if (strShortLink.size() > 30) strShortLink = strShortLink.left(15)+"..."+strShortLink.right(15);

            label_website_link->setText(QString("<a href=\"%1\">%2</a>").arg(strValue, strShortLink));
        }
    }
}

void UserProfileGui::avatarFinished(QNetworkReply *reply)
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
        // display
        avatar.loadFromData(bData);
        label_avatar->setPixmap(avatar.scaled(QSize(50,50), Qt::KeepAspectRatio, Qt::SmoothTransformation));

        // enable zoom
        toolButton_zoom->setEnabled(true);
    }
}

void UserProfileGui::buttonZoom()
{
    UserAvatarGui(avatar).exec();
}

void UserProfileGui::buttonMore()
{
    if (moreWidget->isVisible())
    {
        moreWidget->setVisible(false);
        pushButton_more->setIcon(QIcon(":/images/breeze/go-down.svg"));
        pushButton_more->setText(tr("More..."));
    }
    else
    {
        moreWidget->setVisible(true);
        pushButton_more->setIcon(QIcon(":/images/breeze/go-up.svg"));
        pushButton_more->setText(tr("Less..."));
    }

    setMinimumSize(285, sizeHint().height());
    setMaximumSize(285, sizeHint().height());
}

void UserProfileGui::buttonClose()
{
    UserProfile::instance()->clear();

    close();
}

QString UserProfileGui::convertDesc(QString strContent)
{
    // convert
    Convert::fixHtmlChars(strContent);
    Convert::convertText(strContent);

    return QString("<html><body style=\"background-color:white;font-size:12px;font-family:Verdana;\">%1</body></html>").arg(strContent);
}

void UserProfileGui::showAvatar(const QString &strUrl)
{
    if (!strUrl.contains(",")) return; // wrong url

    // change url
    QStringList lUrl = strUrl.split(",");
    lUrl[1] = "10"; // 10 = zoom; 0 = get real image
    //lUrl[3] = "0.jpg"; // 0 = get real image
    QString strNewUrl = lUrl.join(",");

    // get url
    accessManager->get(QNetworkRequest(QUrl(strNewUrl)));
}
