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

#ifndef SIMPLE_RANK_WIDGET_H
#define SIMPLE_RANK_WIDGET_H

#include <QWidget>

class SimpleRankWidget : public QWidget
{
    Q_OBJECT
public:
    SimpleRankWidget(QWidget *parent = 0, int _iRank = 0, int _iWidth = 250, int _iHeight = 15);
    void setRank(int);

    QSize sizeHint() const;

private:
    int iWidth;
    int iHeight;
    int iRank;

protected:
     void paintEvent(QPaintEvent *);
};

#endif // SIMPLE_RANK_WIDGET_H
