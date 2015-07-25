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

#include <QtWidgets/QApplication>
#include <QTextCodec>
#include "core.h"
#include "models/settings.h"

#include <iostream>

#ifdef Q_OS_WIN
    #include <windows.h>
#endif

void displayDebug()
{
    std::cout << "Debug enabled\n\n";
}

void displayVersion()
{
    std::cout << "Simple Chat Client " << qPrintable(Settings::instance()->get("version")) << "\n\n"
                 "License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>\n"
                 "This is free software: you are free to change and redistribute it.\n\n";
}

void displayOptions()
{
    std::cout << "Options:\n"
           "  --help                     Display this information\n"
           "  --version                  Display version information\n"
           "  --debug                    Enable debug output\n\n";
}

void enableDEP()
{
#ifdef Q_OS_WIN
    /* Call SetProcessDEPPolicy to permanently enable DEP.
       The function will not resolve on earlier versions of Windows,
       and failure is not dangerous. */
    HMODULE hMod = GetModuleHandleA("Kernel32.dll");
    if (hMod)
    {
        typedef BOOL (WINAPI *PSETDEP)(DWORD);
        PSETDEP setdeppolicy = (PSETDEP)GetProcAddress(hMod, "SetProcessDEPPolicy");
        if (setdeppolicy) setdeppolicy(1); /* PROCESS_DEP_ENABLE */
    }
#endif
}

int main(int argc, char *argv[])
{
    // enable DEP
    enableDEP();

    Q_INIT_RESOURCE(scc);

    // app
    QApplication app(argc, argv);
    // minimize to tray
    QApplication::setQuitOnLastWindowClosed(false);

    // set codec
    QLocale::setDefault(QLocale::Polish);
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));

    // set organization
    QCoreApplication::setOrganizationName("scc");
    QCoreApplication::setOrganizationDomain("simplechatclient.github.io");
    QCoreApplication::setApplicationName("scc");

    // debug off
    Settings::instance()->setBool("debug", false);

    // args
    foreach (QString param, app.arguments())
    {
        if (param == "--debug")
        {
            displayDebug();
            Settings::instance()->setBool("debug", true);
        }
        else if (param == "--version")
        {
            displayVersion();
            app.deleteLater();
            return 0;
        }
        else if (param == "--help")
        {
            displayVersion();
            displayOptions();
            app.deleteLater();
            return 0;
        }
    }

    // core
    Core::instance()->createAndShowGui();

    int ret = app.exec();
    Core::instance()->quit();
    app.deleteLater();
    return ret;
}
