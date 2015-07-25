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

#include <QDebug>
#include <QDir>
#include <QFile>
#include <QTextStream>
#include "settings.h"
#include "notes.h"

#ifdef Q_OS_WIN
    #include <QStandardPaths>
#endif

Notes * Notes::Instance = 0;

Notes * Notes::instance()
{
    if (!Instance)
    {
        Instance = new Notes();
        Instance->init();
    }

    return Instance;
}

Notes::Notes()
{
}

void Notes::init()
{
    refreshPath();
}

void Notes::refreshPath()
{
    QString strCurrentProfile = Settings::instance()->get("current_profile");
    QString path;
#ifdef Q_OS_WIN
    path = QFileInfo(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)).absoluteFilePath();
    path += "/scc/";
#else
    path = QDir::homePath()+"/.scc/";
#endif

    path += "profiles/"+strCurrentProfile+"/";

    // create dir if not exist
    if (!QDir().exists(path))
        QDir().mkpath(path);

    strNotesFile = path+"notes.txt";
    strNotesUserProfile = strCurrentProfile;
}

void Notes::read()
{
    if (Settings::instance()->get("current_profile") != strNotesUserProfile)
        refreshPath();

    strNotesContent.clear(); // clear before read

    QFile f(strNotesFile);
    if (f.exists())
    {
        if (f.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            // read content
            QTextStream in(&f);
            strNotesContent = in.readAll();
            f.close();
        }
        else
        {
            if (Settings::instance()->getBool("debug"))
                qWarning() << tr("Error: Cannot read notes file!");
        }
    }
}

void Notes::save()
{
    if (Settings::instance()->get("current_profile") != strNotesUserProfile)
        refreshPath();

    QFile f(strNotesFile);
    if (f.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text))
    {
        QTextStream out(&f);
        out << strNotesContent;
        f.close();
    }
    else
    {
        if (Settings::instance()->getBool("debug"))
            qWarning() << tr("Error: Cannot save notes file!");
    }
}

QString Notes::get()
{
    read();
    return strNotesContent;
}

void Notes::set(const QString &strContent)
{
    strNotesContent = strContent;
    save();
}
