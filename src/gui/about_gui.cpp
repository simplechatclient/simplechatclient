/*
 * Simple Chat Client
 *
 *   Copyright (C) 2009-2015 Piotr Łuczko <piotr.luczko@gmail.com>
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
#include "core/defines.h"
#include "models/settings.h"
#include "about_gui.h"

AboutGui::AboutGui(QWidget *parent) : QDialog(parent)
{
    ui.setupUi(this);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowTitle(tr("About..."));
    // center screen
    move(QApplication::desktop()->screenGeometry(QApplication::desktop()->screenNumber(parent)).center()  - rect().center());

    createGui();
    setDefaultValues();
    createSignals();
}

void AboutGui::createGui()
{
    ui.buttonBox->button(QDialogButtonBox::Ok)->setIcon(QIcon(":/images/breeze/dialog-ok.svg"));
    ui.buttonBox->button(QDialogButtonBox::Ok)->setText(tr("Ok"));
    ui.tabWidget->setTabText(0, tr("&About"));
    ui.tabWidget->setTabText(1, tr("A&uthors"));
    ui.tabWidget->setTabText(2, tr("T&hanks to"));
}

void AboutGui::setDefaultValues()
{
    QString strVersionStatus = Settings::instance()->get("version_status");
    QString strCurrentVersion = Settings::instance()->get("version");

    QString strVersion;
    if (strVersionStatus == UPDATE_STATUS_UPTODATE)
        strVersion = QString("%1 (%2)").arg(tr("Up-to-date version"), strCurrentVersion);
    else if (strVersionStatus == UPDATE_STATUS_BETA)
        strVersion = QString("%1 (%2)").arg(tr("Unstable version"), strCurrentVersion);
    else if (strVersionStatus == UPDATE_STATUS_OUTOFDATE)
         strVersion = QString("<span style=\"color:#ff0000\">%1</span>").arg(tr("A new version is available"));
    else
        strVersion = QString("%1 %2").arg(tr("Version"), strCurrentVersion);

    QString strTitle = QString("<p style=\"font-size:16px;\"><b>Simple Chat Client</b></p>");
    strTitle += "<p>"+strVersion+"</p>";

    QString strAbout = "<p>"+tr("Simple chat client")+"</p>";
    strAbout += "<p>&copy; 2016 by the Simple Chat Client Project</p>";
    strAbout += "<p><a style=\"text-decoration:none;\" href=\"http://simplechatclien.sf.net/\">"+tr("Home page")+"</a></p>";
    strAbout += "<p>"+tr("License")+" <a style=\"text-decoration:none;\" href=\"http://www.fsf.org/licensing/licenses/gpl.html\">GPL v3</a></p>";

    QString strAuthors = "<dl>";
    strAuthors += "<dt><b>"+tr("Piotr Luczko")+"</b></dt>";
    strAuthors += "<dd><a href=\"mailto:piotr.luczko@gmail.com\">piotr.luczko@gmail.com</a></dd>";
    strAuthors += "<dd>"+tr("Project Founder")+", "+tr("Lead Developer")+"</dd>";
    strAuthors += "<dt><b>Dariusz Markowicz</b></dt>";
    strAuthors += "<dd><a href=\"mailto:dmarkowicz@alari.pl\">dmarkowicz@alari.pl</a></dd>";
    strAuthors += "<dd>"+tr("Developer")+"</dd>";
    strAuthors += "</dl>";

    QString strThanksTo = "<p>"+tr("Thanks to <b>ovo_</b> for creating graphics and to all persons who actively support the development of the project.")+"</p>";

    ui.label_logo->setPixmap(QPixmap(":/images/logo64x64.png"));
    ui.label_version->setText(strTitle);
    ui.label_about->setText(strAbout);
    ui.label_authors->setText(strAuthors);
    ui.label_thanks_to->setText(strThanksTo);
}

void AboutGui::createSignals()
{
    connect(ui.buttonBox, SIGNAL(accepted()), this, SLOT(close()));
}
