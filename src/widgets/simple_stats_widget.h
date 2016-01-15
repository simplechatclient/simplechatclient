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

#ifndef SIMPLE_STATS_WIDGET_H
#define SIMPLE_STATS_WIDGET_H

#include <QWidget>

class SimpleStatsWidget : public QWidget
{
    Q_OBJECT
public:
    SimpleStatsWidget(QWidget *parent = 0);
    void clearStats();
    void setMax(qreal);
    void setStats(QList<int>);

    QSize sizeHint() const;

private:
    QList<int> lStats;
    qreal fForceMax;

protected:
    void paintEvent(QPaintEvent *);
};

#endif // SIMPLE_STATS_WIDGET_H
