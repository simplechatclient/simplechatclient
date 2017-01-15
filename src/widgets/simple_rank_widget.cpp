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
#include "simple_rank_widget.h"

SimpleRankWidget::SimpleRankWidget(QWidget *parent, int _iRank, int _iWidth, int _iHeight) : QWidget(parent), iWidth(_iWidth), iHeight(_iHeight), iRank(_iRank)
{
    setBackgroundRole(QPalette::Base);
    setAutoFillBackground(true);

    setMaximumSize(iWidth, iHeight);
    setMinimumSize(iWidth, iHeight);
}

void SimpleRankWidget::setRank(int r)
{
    iRank = r;
    update();
}

QSize SimpleRankWidget::sizeHint() const
{
    return QSize(iWidth, iHeight);
}

void SimpleRankWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    // clear
    painter.setBrush(Qt::white);
    painter.setPen(Qt::white);
    painter.drawRect(0,0,this->sizeHint().width(),this->sizeHint().height());

    if (iRank > 500) iRank = 500;
    if (iRank < -500) iRank = -500;

    if (iRank < 0)
    {
        int x = qAbs(iRank)/4;

        painter.setBrush(Qt::red);
        painter.setPen(Qt::red);
        painter.drawRect(this->sizeHint().width()/2 - x, 0, x, this->sizeHint().height());
    }
    else if (iRank > 0)
    {
        int x = iRank/4;

        painter.setBrush(Qt::green);
        painter.setPen(Qt::green);
        painter.drawRect(this->sizeHint().width()/2, 0, x, this->sizeHint().height());
    }

    // zero
    painter.setBrush(Qt::black);
    painter.setPen(Qt::black);
    painter.drawLine(this->sizeHint().width()/2, 0, this->sizeHint().width()/2, this->sizeHint().height());
}
