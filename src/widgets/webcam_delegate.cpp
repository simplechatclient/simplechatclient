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

#include <QBrush>
#include <QPainter>
#include "webcam_delegate.h"

WebcamDelegate::WebcamDelegate(QObject *parent)
{
    Q_UNUSED (parent);
}

void WebcamDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QString nick = index.data(Qt::DisplayRole).toString();
    int spectators = index.data(WebCamSpectatorsRole).toInt();
    int rank = index.data(WebCamRankRole).toInt();

/*
    // state
    if (option.state & QStyle::State_MouseOver)
        painter->fillRect(option.rect, option.palette.highlight());
*/

    // draw nick
    painter->drawText(option.rect, nick);

    // x y
    int x = option.rect.right();
    int y = option.rect.top() + (option.rect.height() / 2) + (option.font.pointSize() / 2);

    // spectators
    painter->drawText(x-80,y,QString::number(spectators));

    // rank
    x -= 50;
    y = option.rect.top();
    int w = 50;
    int half_w = x+w/2;
    int h = option.rect.height();
    int new_w;

    QBrush bRed(Qt::red);
    QBrush bGreen(Qt::green);
    QBrush bBlack(Qt::black);
    QBrush bWhite(Qt::white);

    // rank background
    painter->fillRect(x,y,w-1,h, bBlack);
    painter->fillRect(x+1,y+1,w-3,h-1, bWhite);

    // correct rank
    if (rank > 500) rank = 500;
    if (rank < -500) rank = -500;

    if (rank < 0)
    {
        new_w = qAbs(rank)/20;
        painter->fillRect(half_w - new_w, y+1, new_w, h, bRed);
    }
    else if (rank > 0)
    {
        new_w = rank/20;
        painter->fillRect(half_w, y+1, new_w, h, bGreen);
    }

    // zero
    painter->fillRect(half_w, y, 1, h, bBlack);
}

QSize WebcamDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED (option);
    Q_UNUSED (index);

    return QSize(270,16);
}
