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

#include <QDesktopWidget>
#include "webcam_mini_gui.h"

WebcamMiniGui::WebcamMiniGui(QString nick) : nick(nick)
{
    ui.setupUi(this);
    setWindowFlags(Qt::Dialog | Qt::WindowStaysOnTopHint);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowTitle(nick);
    // move
    int x = QApplication::desktop()->screenGeometry(QApplication::desktop()->screenNumber(this)).topRight().x() - rect().width() - 50;
    int y = QApplication::desktop()->screenGeometry(QApplication::desktop()->screenNumber(this)).topRight().y() + 30;
    move(x,y);
}

void WebcamMiniGui::updateImage(const QByteArray &data)
{
    QPixmap pixmap;
    pixmap.loadFromData(data);
    ui.label_img->setPixmap(pixmap);
}

void WebcamMiniGui::updateText(const QString &text)
{
    ui.label_img->setText(text);
}

void WebcamMiniGui::error(const QString &s)
{
    updateText(s + "<br/>"+tr("Disconnected from server webcams"));
}

void WebcamMiniGui::closeEvent(QCloseEvent *)
{
    emit closeCam();
}

void WebcamMiniGui::updateUserCount(const QString &strNick, int iSpectators, int iRank)
{
    Q_UNUSED(iRank);

    if (strNick == nick)
        setWindowTitle(QString("%1 (%2)").arg(nick, QString::number(iSpectators)));
}
