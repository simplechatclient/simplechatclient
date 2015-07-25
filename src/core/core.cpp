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

#include <QCoreApplication>
#include <QFile>
#include <QTcpSocket>
#include "common/config.h"
#include "mainwindow.h"
#include "models/settings.h"
#include "models/themes.h"
#include "models/update.h"
#include "core.h"

Core * Core::Instance = 0;

Core * Core::instance()
{
    if (!Instance)
    {
        Instance = new Core();
        Instance->init();
    }

    return Instance;
}

Core::Core() : window(0)
{
}

Core::~Core()
{
    delete window;
    window = 0;

    kamerzystaSocket->deleteLater();

    // close network
    Settings::instance()->setBool("reconnect", false);
    network->disconnect();
    network->quit();
    network->wait();
    network->deleteLater();
    network->QObject::disconnect();
    delete network;
}

void Core::init()
{
    // create settings
    createSettings();
    reloadSettings();

    // language
    refreshLanguage();

    // kamerzysta
    kamerzystaSocket = new QTcpSocket();
    kamerzystaSocket->setSocketOption(QAbstractSocket::LowDelayOption, 1);
    kamerzystaSocket->setSocketOption(QAbstractSocket::KeepAliveOption, 0);

    // network
    QString strServer = "czat-app.onet.pl";
    int iPort = 5015;
    network = new Network(strServer, iPort);
    network->start(QThread::InheritPriority);
}

void Core::createAndShowGui()
{
    window = new MainWindow(0);
    window->show();
    window->init();

    // updates
    Update::instance()->checkUpdate();
}

void Core::createSettings()
{
    // default settings
    Settings::instance()->set("available_version", "0.0.0.0");
    Settings::instance()->set("whats_new", QString::null);
    Settings::instance()->set("motd", QString::null);
    Settings::instance()->set("version_status", UPDATE_STATUS_UNKNOWN);

    Settings::instance()->setBool("logged", false);
    Settings::instance()->setBool("busy", false);
    Settings::instance()->setBool("away", false);
    Settings::instance()->setBool("override", false);
    Settings::instance()->setBool("ignore_raw_141", false);
    Settings::instance()->setBool("age_check", true);
    Settings::instance()->setBool("first_run", true);
    Settings::instance()->set("uo_key", QString::null);
    Settings::instance()->set("uo_nick", QString::null);
    Settings::instance()->set("last_active", "0");
    Settings::instance()->setBool("reconnect", true);
    Settings::instance()->set("socket_state", "disconnected");
    Settings::instance()->setBool("authorizing", false);

    QStringList constCookies;
    constCookies << "onet_ubi" << "onet_cid" << "onet_sid" << "onet_uid" << "onetzuo_ticket" << "onet_uoi" << "onet_sgn";

    foreach (const QString &constCookie, constCookies)
        Settings::instance()->set(constCookie, QString::null);
}

void Core::reloadSettings()
{
    // read config settings
    configSettingsValues();

    // read config profile
    configProfileValues();

    // check settings
    fixSettings();

    // themes
    Themes::instance()->refreshCurrent();
}

void Core::configSettingsValues()
{
    // config values
    Config *pConfig = new Config(SettingsConfig);
    QHash<QString,QString> lConfigValues = pConfig->read();
    delete pConfig;

    // set settings
    QHashIterator <QString, QString> it(lConfigValues);
    while (it.hasNext())
    {
        it.next();
        if (it.value() == "true" || it.value() == "false")
            Settings::instance()->setBool(it.key(), it.value() == "true");
        else
            Settings::instance()->set(it.key(), it.value());
    }
}

void Core::configProfileValues()
{
    // config profile values
    Config *pConfigProfile = new Config();
    QHash<QString,QString> lConfigProfileValues = pConfigProfile->read();
    delete pConfigProfile;

    // set profile settings
    QHashIterator <QString, QString> it(lConfigProfileValues);
    while (it.hasNext())
    {
        it.next();
        if (it.value() == "true" || it.value() == "false")
            Settings::instance()->setBool(it.key(), it.value() == "true");
        else
            Settings::instance()->set(it.key(), it.value());
    }
}

void Core::fixSettings()
{
    QString path;
#ifdef Q_OS_WIN
    path = QCoreApplication::applicationDirPath();
#else
    path = SCC_DATA_DIR;
#endif

    Config *pConfig = new Config();

    if (!QFile::exists(Settings::instance()->get("sound_beep")))
    {
        QString strSoundBeep = path+"/sounds/beep.wav";

        pConfig->set("sound_beep", strSoundBeep);
        Settings::instance()->set("sound_beep", strSoundBeep);
    }
    if (!QFile::exists(Settings::instance()->get("sound_query")))
    {
        QString strSoundQuery = path+"/sounds/query.wav";

        pConfig->set("sound_query", strSoundQuery);
        Settings::instance()->set("sound_query", strSoundQuery);
    }
    if (!QFile::exists(Settings::instance()->get("background_image")))
    {
        QString strBackgroundImage = path+"/images/wallpaper/default.jpg";

        pConfig->set("background_image", strBackgroundImage);
        Settings::instance()->set("background_image", strBackgroundImage);
    }
    if ((Settings::instance()->get("themes") != "Standard") &&
        (Settings::instance()->get("themes") != "Alhena") &&
        (Settings::instance()->get("themes") != "Origin") &&
        (Settings::instance()->get("themes") != "Adara"))
    {
        pConfig->set("themes", "Standard");
        Settings::instance()->set("themes", "Standard");
    }

    delete pConfig;
}

MainWindow *Core::mainWindow()
{
    return window;
}

void Core::quit()
{
    if (!Instance)
        return;

    delete Instance;
    Instance = 0;
}

void Core::refreshLanguage()
{
    // get language
    QString strLanguage = Settings::instance()->get("language");

    // set translate
    QString path;
#ifdef Q_OS_WIN
    path = QCoreApplication::applicationDirPath();
#else
    path = SCC_DATA_DIR;
#endif

    // load translate
    if (qtTranslator.load(QString("%1/translations/qt_%2").arg(path, strLanguage)))
        qApp->installTranslator(&qtTranslator);

    if (sccTranslator.load(QString("%1/translations/scc_%2").arg(path, strLanguage)))
        qApp->installTranslator(&sccTranslator);
}
