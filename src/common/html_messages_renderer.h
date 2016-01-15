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

#ifndef HTML_MESSAGES_RENDERER_H
#define HTML_MESSAGES_RENDERER_H

#include <QObject>
#include "core/defines.h"

class HtmlMessagesRenderer : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(HtmlMessagesRenderer)
    static HtmlMessagesRenderer *Instance;
public:
    static HtmlMessagesRenderer *instance();

    HtmlMessagesRenderer();

    QString renderer(QString strData, MessageCategory eMessageCategory, qint64 iTime, QString strNick = QString::null, QString strChannel = QString::null);
    QString rendererDebug(QString strData, qint64 iTime);
    QString headCSS();
    QString bodyCSS();

private:
    void fixContextMenu(QString &strData, MessageCategory eMessageCategory, const QString &strChannel);
};

#endif // HTML_MESSAGES_RENDERER_H
