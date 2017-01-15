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

#include <QPainter>
#include "simple_percentage_widget.h"

SimplePercentageWidget::SimplePercentageWidget(QWidget *parent, int _iWidth, int _iHeight, double _iRank) : QWidget(parent), iWidth(_iWidth), iHeight(_iHeight), iRank(_iRank)
{
    setBackgroundRole(QPalette::Base);
    setAutoFillBackground(true);

    setMaximumSize(iWidth, iHeight);
    setMinimumSize(iWidth, iHeight);
}

void SimplePercentageWidget::setRank(double _iRank)
{
    iRank = _iRank;
}

QSize SimplePercentageWidget::sizeHint() const
{
    return QSize(iWidth, iHeight);
}

void SimplePercentageWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    // clear
    painter.setBrush(Qt::white);
    painter.setPen(Qt::white);
    painter.drawRect(0,0,this->sizeHint().width(),this->sizeHint().height());

    int x = (int)this->sizeHint().width()*iRank;

    // min max
    if (x < 0)
        x = 0;
    else if (x > iWidth)
        x = iWidth;

    painter.setBrush(Qt::green);
    painter.setPen(Qt::green);
    painter.drawRect(0,0,x, this->sizeHint().height());
}
