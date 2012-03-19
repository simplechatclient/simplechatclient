/****************************************************************************
 *                                                                          *
 *   This file is part of Simple Chat Client                                *
 *   Copyright (C) 2012 Piotr Łuczko <piotr.luczko@gmail.com>               *
 *                                                                          *
 *   This program is free software: you can redistribute it and/or modify   *
 *   it under the terms of the GNU General Public License as published by   *
 *   the Free Software Foundation, either version 3 of the License, or      *
 *   (at your option) any later version.                                    *
 *                                                                          *
 *   This program is distributed in the hope that it will be useful,        *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *   GNU General Public License for more details.                           *
 *                                                                          *
 *   You should have received a copy of the GNU General Public License      *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 *                                                                          *
 ****************************************************************************/

#include <QCoreApplication>
#include <QDate>
#include <QDir>
#include <QFile>
#include <QTextStream>
#include "core.h"
#include "log.h"

#ifdef Q_WS_WIN
    #include <QDesktopServices>
#endif

void convert(QString &strData)
{
    strData.remove(QRegExp("%C([a-zA-Z0-9]+)%"));
    strData.remove(QRegExp("%F([a-zA-Z0-9:]+)%"));
    strData.replace(QRegExp("%I([a-zA-Z0-9_-]+)%"),"<\\1>");
}

void Log::save(const QString &strChannel, const QString &strData)
{
    QString path;

#ifdef Q_WS_WIN
    path = QFileInfo(QDesktopServices::storageLocation(QDesktopServices::DocumentsLocation)).absoluteFilePath();
    path += "/scc";
#else
    path = QDir::homePath()+"/.scc";
#endif

    QString strCurrentProfile = Core::instance()->settings.value("current_profile");
    path += "/profiles/"+strCurrentProfile+"/log";

    // save logs by date
    bool bSaveLogsByDate = Core::instance()->settings.value("save_logs_by_date") == "true" ? true : false;
    if (bSaveLogsByDate)
    {
        QString strDate = QDate().currentDate().toString("yyyy-MM");
        path += "/"+strDate;
    }

    // create dir if not exist
    if (!QDir().exists(path))
        QDir().mkpath(path);

    QString strFileName = strChannel;

    // fixed priv name
    if (strFileName[0] == '^')
        strFileName = Core::instance()->convertPrivName(strFileName);

    QFile f(path+"/"+strFileName+".txt");
    if (f.open(QIODevice::Append))
    {
        // convert
        QString strSaveData = strData; 
        convert(strSaveData);

        // save
        QTextStream out(&f);
        out << strSaveData << "\r\n";

        f.close();
    }
}
