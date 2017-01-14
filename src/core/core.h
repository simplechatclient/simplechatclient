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

#ifndef CORE_H
#define CORE_H

#include "scc-config.h"
#include <QObject>
#include <QTranslator>
#include "network.h"
class MainWindow;

/**
 * Core
 */
class Core : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(Core)
    static Core *Instance;
public:
    static Core *instance();

    Core();
    virtual ~Core();
    void createAndShowGui();
    void reloadSettings();
    MainWindow *mainWindow();
    void refreshLanguage();

    // network
    Network *network;

public slots:
    void quit();

private:
    MainWindow *window;
    QTranslator qtTranslator;
    QTranslator sccTranslator;

    void init();
    void createSettings();
    // reload config
    void configSettingsValues();
    void configProfileValues();
    // fix settings
    void fixSettings();
};

#endif // CORE_H
