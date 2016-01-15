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

#ifndef WEBCAM_STANDARD_GUI_H
#define WEBCAM_STANDARD_GUI_H

#include <QDialog>
#include "ui_webcam_standard.h"
class SimpleRankWidget;

class WebcamStandardGui : public QDialog
{
    Q_OBJECT
public:
    WebcamStandardGui();
    virtual ~WebcamStandardGui();

public slots:
    void updateImage(const QByteArray &image);
    void updateText(const QString &text);
    void updateRank(int rank);
    void userError(const QString &error);
    void updateStatus(const QString &status);
    void voteOk();
    void error(const QString &error);
    void updateUser(const QString &strNick, int iSpectators, int iRank, int iCamOnOff, const QString &strUdget, const QStringList &lUserChannels);
    void updateUserCount(const QString &strNick, int iSpectators, int iRank);

    void addUser(const QString &strNick, int iSpectators, int iRank, const QString &strUdget, const QStringList &lUserChannels);
    void updateUser(const QString &strNick, int iSpectators, int iRank, const QString &strUdget, const QStringList &lUserChannels);
    void removeUser(const QString &strNick);
    void clearUsers();

private:
    Ui::uiWebcamStandard ui;
    QString strCurrentNick;
    SimpleRankWidget *pSimpleRankWidget;
    enum WebCamRoles
    {
        WebCamSpectatorsRole = Qt::UserRole+1,
        WebCamRankRole = Qt::UserRole+2,
        WebCamUdgetRole = Qt::UserRole+3,
        WebCamUserChannelsRole = Qt::UserRole+4
    };

    void createGui();
    void createSignals();
    bool existUser(const QString &strNick);

private slots:
    void changeUser(QListWidgetItem *item);
    void voteMinus();
    void votePlus();
    void enableVote();
    void buttonClose();
    void channelClicked(QUrl url);

protected:
    virtual void closeEvent(QCloseEvent *);

signals:
    void closeCam();
    void networkSend(const QString&);
    void setUser(const QString &strNewNick);
};

#endif // WEBCAM_STANDARD_GUI_H
