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
#include <QPushButton>
#include "core/core.h"
#include "models/my_profile.h"
#include "models/my_stats.h"
#include "widgets/simple_percentage_widget.h"
#include "widgets/simple_stats_widget.h"
#include "models/utils.h"
#include "my_stats_gui.h"

MyStatsGui::MyStatsGui(QWidget *parent) : QDialog(parent)
{
    ui.setupUi(this);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowTitle(tr("My statistics"));
    // center screen
    move(QApplication::desktop()->screenGeometry(QApplication::desktop()->screenNumber(parent)).center()  - rect().center());

    pSimpleStatsWidget = new SimpleStatsWidget(this);
    ui.verticalLayout_stats->addWidget(pSimpleStatsWidget);
    pSimpleStatsWidget->show();

    pSimplePercentageWidget = new SimplePercentageWidget(this);
    ui.verticalLayout_rank->addWidget(pSimplePercentageWidget);
    pSimplePercentageWidget->show();

    createGui();
    createSignals();

    refresh();
}

MyStatsGui::~MyStatsGui()
{
    delete pSimpleStatsWidget;
}

void MyStatsGui::createGui()
{
    ui.buttonBox->button(QDialogButtonBox::Close)->setIcon(QIcon(":/images/breeze/dialog-close.svg"));

    ui.groupBox_stats->setTitle(tr("Statistics"));
    ui.groupBox_rank->setTitle(tr("Type"));
    ui.label_friends->setText(tr("Added you as a friend:"));
    ui.label_ignored->setText(tr("Added you to ignore:"));
    ui.label_average_time->setText(tr("Average time:"));
    ui.buttonBox->button(QDialogButtonBox::Close)->setText(tr("Close"));
}

void MyStatsGui::createSignals()
{
    connect(ui.buttonBox, SIGNAL(rejected()), this, SLOT(close()));
}

void MyStatsGui::refresh()
{
    QHashIterator <QString, QString> is(MyStats::instance()->getAll());
    while (is.hasNext())
    {
        is.next();
        QString strKey = is.key();
        QString strValue = is.value();

        if (strKey == "histTotal")
        {
            QList<int> lStats;

            qreal fAverageTime = 0;
            for (int i = 0; i < strValue.size(); ++i)
            {
                int iTime = Utils::instance()->timeToPercentage(strValue.at(i));

                // add to list
                lStats.append(iTime);

                // add to average time
                fAverageTime += iTime;
            }

            // average time
            fAverageTime /= strValue.size();
            qreal fAverageTime24 = (fAverageTime/100)*24;
            ui.label_stats_average_time->setText(QString::number(fAverageTime24,'g',3)+" h");

            // simple stats widget
            pSimpleStatsWidget->setMax(100);
            pSimpleStatsWidget->setStats(lStats);
        }
        else if (strKey == "relationsFriend")
        {
            ui.label_stats_friend->setText(strValue);
        }
        else if (strKey == "relationsIgnored")
        {
            ui.label_stats_ignored->setText(strValue);
        }
    }

    QHashIterator <QString, QString> im(MyProfile::instance()->getAll());
    while (im.hasNext())
    {
        im.next();
        QString strKey = im.key();
        QString strValue = im.value();

        if (strKey == "rank")
        {
            QStringList lRank = strValue.split('.');
            double dType = lRank.value(0, 0).toDouble();
            int iType = lRank.value(0, 0).toInt();
            double dRank = strValue.toDouble();

            pSimplePercentageWidget->setRank(dRank-dType);

            ui.label_rank_current->setText(Utils::instance()->convertIntToType(iType));
            ui.label_rank_next->setText(Utils::instance()->convertIntToType(iType + 1));
        }
    }
}
