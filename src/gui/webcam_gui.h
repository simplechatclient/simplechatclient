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

#ifndef WEBCAM_GUI_H
#define WEBCAM_GUI_H

#include <QObject>
class WebcamEngine;
class WebcamMiniGui;
class WebcamStandardGui;

class WebcamGui : public QObject
{
    Q_OBJECT
public:
    WebcamGui(QString _strNick = QString::null, bool _bMini = false);
    virtual ~WebcamGui();

private:
    QString strNick;
    bool bMini;
    WebcamEngine *pWebcamEngine;
    WebcamMiniGui *pWebcamMini;
    WebcamStandardGui *pWebcamStandard;

    void createSignals();

private slots:
    void closeCam();
    void userError(const QString&);
};

#endif // WEBCAM_GUI_H
