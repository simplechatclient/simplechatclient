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

#ifndef NICKLIST_DELEGATE_H
#define NICKLIST_DELEGATE_H

#include <QAbstractItemDelegate>

class NickListDelegate : public QAbstractItemDelegate
{
    Q_OBJECT
public:
    NickListDelegate(QObject *parent = 0);
    virtual ~NickListDelegate();

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;

private:
    enum NickListRoles
    {
        NickListModesRole = Qt::UserRole+20,
        NickListAvatarUrlRole = Qt::UserRole+21,
        NickListSexRole = Qt::UserRole+22,
        NickListChannelRole = Qt::UserRole+23
    };
};

#endif // NICKLIST_DELEGATE_H
