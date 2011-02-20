/****************************************************************************
 *                                                                          *
 *   This file is part of Simple Chat Client                                *
 *   Copyright (C) 2011 Piotr Łuczko <piotr.luczko@gmail.com>               *
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

#include <QDesktopWidget>
#include <QPushButton>
#include <QSettings>
#include "simplestatswidget.h"
#include "dlg_my_stats.h"

DlgMyStats::DlgMyStats(QWidget *parent, QMap<QString,QString> *param1) : QDialog(parent)
{
    ui.setupUi(this);
    setWindowTitle(tr("My statistics"));
    // center screen
    move(QApplication::desktop()->screen()->rect().center() - rect().center());

    mMyStats = param1;

    simpleStatsWidget = new SimpleStatsWidget(this);
    simpleStatsWidget->show();
    ui.verticalLayout_stats->addWidget(simpleStatsWidget);

    ui.buttonBox->button(QDialogButtonBox::Close)->setIcon(QIcon(":/images/oxygen/16x16/dialog-close.png"));

    ui.groupBox_stats->setTitle(tr("Statistics"));
    ui.label_friends->setText(tr("Added you as a friend:"));
    ui.label_ignored->setText(tr("Added you to ignore:"));
    ui.label_average_time->setText(tr("Average time:"));

    QObject::connect(ui.buttonBox, SIGNAL(rejected()), this, SLOT(button_close()));

    refresh();
}

int DlgMyStats::replace_value(QString strValue)
{
    int iValue = 0;

    // ascii to number
    int i = 0;
    for (int c = 97; c <= 122; c++)
    {
        QString strChar = QString(QChar(c));
        if (strChar == strValue)
        {
            iValue = i;
            break;
        }
        i++;
    }

    // percentage
    iValue = (iValue*100)/24;

    return iValue;
}

void DlgMyStats::refresh()
{
    QMap<QString, QString>::const_iterator i = mMyStats->constBegin();
    while (i != mMyStats->constEnd())
    {
        QString strKey = i.key();
        QString strValue = i.value();

        if (strKey == "histTotal")
        {
            QList<int> lStats;

            qreal fAverageTime = 0;
            for (int i = 0; i < strValue.count(); i++)
            {
                int iTime = replace_value(strValue.at(i));

                // add to list
                lStats.append(iTime);

                // add to average time
                fAverageTime += iTime;
            }

            // average time
            fAverageTime /= strValue.count();
            qreal fAverageTime24 = (fAverageTime/100)*24;
            ui.label_stats_average_time->setText(QString::number(fAverageTime24,'g',3)+" h");

            // simple stats widget
            simpleStatsWidget->set_max(100);
            simpleStatsWidget->set_stats(lStats);
        }
        else if (strKey == "relationsFriend")
        {
            ui.label_stats_friend->setText(strValue);
        }
        else if (strKey == "relationsIgnored")
        {
            ui.label_stats_ignored->setText(strValue);
        }

        ++i;
    }
}

void DlgMyStats::button_close()
{
    this->close();
}
