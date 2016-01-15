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

#ifndef OPTIONS_GUI_H
#define OPTIONS_GUI_H

#include <QDialog>
#include "ui_options.h"

class OptionsGui : public QDialog
{
    Q_OBJECT
public:
    OptionsGui(QWidget *parent = 0);
    void setCurrentProfile(int row);
    void refreshProfilesList();

private:
    Ui::uiOptions ui;

    void createGui();
    void setDefaultValues();
    void createSignals();

    void setColor(const QString &strKey);
    void setMainwindowColors();
    void setNicklistColors();

    QString reverseColor(QString strColor);

private slots:
    void changePage(QModelIndex index);
    void currentProfileChanged(int row);

    void buttonProfiles();
    void buttonThemes();
    void languageChanged(int index);

    void highlightAdd();
    void highlightEdit();
    void highlightRemove();

    void punishReasonAdd();
    void punishReasonEdit();
    void punishReasonRemove();

    void reverseColors();
    void restoreDefaultColors();
    QStringList getColors();
    void setBackgroundColor();
    void setDefaultColor();
    void setTimeColor();
    void setJoinColor();
    void setPartColor();
    void setQuitColor();
    void setKickColor();
    void setModeColor();
    void setNoticeColor();
    void setInfoColor();
    void setMeColor();
    void setErrorColor();
    void setChannelColor();
    void setNicklistNickColor();
    void setNicklistSelectedNickColor();
    void setNicklistBusyNickColor();
    void setNicklistGradient1Color();
    void setNicklistGradient2Color();

    void tryPlayBeep();
    void tryPlayQuery();
    void setSoundBeep();
    void setSoundQuery();
    void setLogsByDate(bool bValue);
    void setSound(bool bValue);

    void openLogsFolder();
    void setLogs(bool bValue);
    void setLogsFormatHtml();
    void setLogsFormatTxt();

    void setBackgroundImage();
    void setShowBackgroundImage(bool bValue);

    void setFontFormating(bool bValue);
    void setShowJoinPart(bool bValue);
    void setShowJoinPartBigChannel(bool bValue);
    void setEmoticons(bool bValue);
    void setReplaceEmoticons(bool bValue);
    void setNicklist(bool bValue);
    void setImgThumbs(bool bValue);
    void setTimeFormat(int index);

    void setWinamp();

    void setMprisFormat();
    void setMprisPlayer(int index);

    void setTrayMessage(bool);

    void setWebcamSystem();
    void setWebcamInternal();

    void setUpdates(bool);

    void setAutoConnect(bool bValue);
    void setAutoBusy(bool bValue);
    void setAutoAway(bool bValue);
    void setAutojoinFavourites(bool bValue);
    void setMinimizeToTray(bool bValue);
    void setZuoAndIp(bool bValue);
};

#endif // OPTIONS_GUI_H
