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

#include <QBrush>
#include <QPainter>
#include <QPen>
#include "simple_stats_widget.h"

SimpleStatsWidget::SimpleStatsWidget(QWidget *parent) : QWidget(parent)
{
    setBackgroundRole(QPalette::Base);
    setAutoFillBackground(true);

    setMaximumSize(400, 100);
    setMinimumSize(400, 100);

    clearStats();
}

void SimpleStatsWidget::clearStats()
{
    fForceMax = 0;
    lStats.clear();

    update();
}

void SimpleStatsWidget::setMax(qreal f)
{
    fForceMax = f;
}

void SimpleStatsWidget::setStats(QList<int> s)
{
    lStats = s;
    update();
}

QSize SimpleStatsWidget::sizeHint() const
{
    return QSize(400, 100);
}

void SimpleStatsWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    // clear
    painter.setBrush(Qt::white);
    painter.setPen(Qt::white);
    painter.drawRect(0,0,this->sizeHint().width(),this->sizeHint().height());

    // get max
    qreal fMax = 0;
    foreach (int iStat, lStats)
    {
        if (iStat > fMax)
            fMax = iStat;
    }

    // force max
    if (fForceMax != 0)
        fMax = fForceMax;

    // get percentage
    qreal fPercentage = 100;
    if (fMax != 0) fPercentage = (100*100)/fMax;

    // if correct stats
    if (fMax != 0)
    {
        QList<qreal> lStatsF;
        // fix values to max 100
        foreach (int iStat, lStats)
        {
            if (iStat != fMax)
                lStatsF.append(iStat * (fPercentage/100));
            else
                lStatsF.append(100);
        }

        // draw
        for (int i = 0; i < lStatsF.size(); ++i)
        {
            QBrush brush(Qt::green);

            if (lStatsF.value(i, 0) != 100)
                painter.fillRect(5+i*(400/30), 100-lStatsF.value(i, 0), 10, lStatsF.value(i, 0), brush);
            else // draw 100%
                painter.fillRect(5+i*(400/30), 0, 10, 100, brush);
        }
    }

    // draw zero
    for (int i = 0; i < lStats.size(); ++i)
    {
        QBrush brush(Qt::green);

        painter.fillRect(5+i*(400/30), 100-2, 10, 1, brush);
    }

    // draw border
    painter.setBrush(Qt::black);
    painter.setPen(Qt::black);
    painter.drawLine(0,0,0,100); // left
    painter.drawLine(400-1,0,400-1,100); // right
    painter.drawLine(0,0,400,0); // top
    painter.drawLine(0,100-1,400,100-1); // bottom
}
