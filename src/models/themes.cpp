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

#include "settings.h"
#include "themes.h"

Themes * Themes::Instance = 0;

Themes * Themes::instance()
{
    if (!Instance)
    {
        Instance = new Themes();
        Instance->init();
    }

    return Instance;
}

Themes::Themes()
{
    add("Standard", ThemeFlags::Default);
    add("Alhena");
    add("Origin", ThemeFlags::NicklistAvatar);
    add("Adara", ThemeFlags::ChannelAvatar);
}

void Themes::init()
{
    refreshCurrent();
}

void Themes::add(const QString &name, ThemeFlags::Flags flags)
{
    Theme newTheme;
    newTheme.name = name;
    newTheme.flags = flags;

    lThemes.append(newTheme);
}

Theme Themes::get()
{
    QString currentName = Settings::instance()->get("themes");

    foreach (const Theme &_Theme, lThemes)
    {
        if (_Theme.name == currentName)
            return _Theme;
    }

    foreach (const Theme &_Theme, lThemes)
    {
        if (_Theme.flags & ThemeFlags::Default)
            return _Theme;
    }

	return lThemes.at(0);
}

void Themes::refreshCurrent()
{
    current = get();
}

QList<QString> Themes::getAll()
{
    QList<QString> lThemesName;

    foreach (const Theme &_Theme, lThemes)
    {
        lThemesName.append(_Theme.name);
    }

    return lThemesName;
}

bool Themes::isCurrentWithAvatar()
{
    if (current.flags & (ThemeFlags::ChannelAvatar | ThemeFlags::NicklistAvatar))
        return true;
    else
        return false;
}

bool Themes::isCurrentWithChannelAvatar()
{
    return current.flags & ThemeFlags::ChannelAvatar;
}

bool Themes::isCurrentWithNicklistAvatar()
{
    return current.flags & ThemeFlags::NicklistAvatar;
}
