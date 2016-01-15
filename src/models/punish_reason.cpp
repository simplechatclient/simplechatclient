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

#include "common/config.h"
#include "punish_reason.h"

PunishReason * PunishReason::Instance = 0;

PunishReason * PunishReason::instance()
{
    if (!Instance)
    {
        Instance = new PunishReason();
        Instance->init();
    }

    return Instance;
}

PunishReason::PunishReason()
{
}

void PunishReason::init()
{
    lPunishReason.clear();
    read();
}

void PunishReason::add(const QString &strPunishReason)
{
    QString strPunishReasonClean = fix(strPunishReason);

    if (!lPunishReason.contains(strPunishReasonClean))
    {
        lPunishReason.append(strPunishReasonClean);
        save();
    }
}

void PunishReason::remove(const QString &strPunishReason)
{
    QString strPunishReasonClean = fix(strPunishReason);

    if (lPunishReason.contains(strPunishReasonClean))
    {
        lPunishReason.removeAll(strPunishReasonClean);
        save();
    }
}

void PunishReason::replace(const QString &strOldPunishReason, const QString &strNewPunishReason)
{
    this->remove(strOldPunishReason);
    this->add(strNewPunishReason);
}

bool PunishReason::contains(const QString &strPunishReason)
{
    QString strPunishReasonClean = fix(strPunishReason);
    return lPunishReason.contains(strPunishReasonClean);
}

void PunishReason::clear()
{
    lPunishReason.clear();
}

QStringList PunishReason::get()
{
    return lPunishReason;
}

void PunishReason::read()
{
    QString strPunishReason;

    Config *pConfig = new Config();
    strPunishReason = pConfig->get("punish_reason");
    delete pConfig;

    lPunishReason = strPunishReason.split(";", QString::SkipEmptyParts);
}

void PunishReason::save()
{
    QString strPunishReason;

    foreach (const QString &strNewPunishReason, lPunishReason)
        strPunishReason.append(strNewPunishReason+";");

    Config *pConfig = new Config();
    pConfig->set("punish_reason", strPunishReason);
    delete pConfig;
}

QString PunishReason::fix(const QString &strPunishReason)
{
    QString strPunishReasonFixed = strPunishReason;
    return strPunishReasonFixed.remove(QRegExp("[&<>\"'\\;]"));
}
