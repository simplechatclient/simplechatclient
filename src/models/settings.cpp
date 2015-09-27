/*
 * Simple Chat Client
 *
 *   Copyright (C) 2009-2015 Piotr Łuczko <piotr.luczko@gmail.com>
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

Settings * Settings::Instance = 0;

Settings * Settings::instance()
{
    if (!Instance)
    {
        Instance = new Settings();
        Instance->init();
    }

    return Instance;
}

Settings::Settings()
{
}

void Settings::init()
{
    clear();

    set("version", "2.2.0.0");
}

void Settings::clear()
{
    settings.clear();
    bSettings.clear();
}

QString Settings::get(const QString &key)
{
    return settings.value(key, QString::null);
}

bool Settings::getBool(const QString &key)
{
    return bSettings.value(key, false);
}

void Settings::set(const QString &key, const QString &value)
{
    settings[key] = value;
}

void Settings::setBool(const QString &key, bool value)
{
    bSettings[key] = value;
}
