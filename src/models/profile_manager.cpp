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

#include <QDir>
#include "common/config.h"
#include "settings.h"
#include "profile_manager.h"

#ifdef Q_OS_WIN
    #include <QStandardPaths>
#endif

ProfileManager * ProfileManager::Instance = 0;

ProfileManager * ProfileManager::instance()
{
    if (!Instance)
    {
        Instance = new ProfileManager();
        Instance->init();
    }

    return Instance;
}

ProfileManager::ProfileManager()
{
}

void ProfileManager::init()
{
#ifdef Q_OS_WIN
    path = QFileInfo(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)).absoluteFilePath();
    path += "/scc/";
#else
    path = QDir::homePath()+"/.scc/";
#endif

    path += "profiles/";

    // create dir if not exist
    if (!QDir().exists(path))
        QDir().mkpath(path);
}

bool ProfileManager::removeProfileDirectory(const QString &strDir)
{
    bool result = true;
    QDir dir(strDir);

    if (dir.exists(strDir))
    {
        QFileInfoList listInfo = dir.entryInfoList(QDir::NoDotAndDotDot | QDir::System | QDir::Hidden  | QDir::AllDirs | QDir::Files, QDir::DirsFirst);
        foreach(const QFileInfo &info, listInfo)
        {
            if (info.isDir())
                result = removeProfileDirectory(info.absoluteFilePath());
            else
                result = QFile::remove(info.absoluteFilePath());

            if (!result)
                return result;
        }
        result = dir.rmdir(strDir);
    }

    return result;
}

void ProfileManager::renameProfile(const QString &strProfile, const QString &strNewProfile)
{
    // current profile
    Config *pConfig = new Config(SettingsConfig);
    pConfig->set("current_profile", strNewProfile);
    delete pConfig;
    Settings::instance()->set("current_profile", strNewProfile);

    // rename profile dir
    QDir().rename(path+"/"+strProfile, path+"/"+strNewProfile);

    // update config
    Config *pProfileConfig = new Config();
    pProfileConfig->set("nick", strNewProfile);
    delete pProfileConfig;
    Settings::instance()->set("nick", strNewProfile);
}
