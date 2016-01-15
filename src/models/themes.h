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

#ifndef THEMES_H
#define THEMES_H

#include "core/defines.h"
#include <QObject>

class Themes : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(Themes)
    static Themes *Instance;
public:
    static Themes *instance();

    Themes();
    void refreshCurrent();
    QList<QString> getAll();
    bool isCurrentWithAvatar();
    bool isCurrentWithChannelAvatar();
    bool isCurrentWithNicklistAvatar();

private:
    QList<Theme> lThemes;
    Theme current;

    void init();
    void add(const QString &name, ThemeFlags::Flags flags = ThemeFlags::NoFlags);
    Theme get();
};

#endif // THEMES_H
