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
#include <QDate>
#include <QDir>
#include <QFile>
#include <QTextStream>
#include "models/channel.h"
#include "convert.h"
#include "html_messages_renderer.h"
#include "models/settings.h"
#include "log.h"

#ifdef Q_OS_WIN
    #include <QStandardPaths>
#else
    #include "scc-config.h"
#endif

void Log::logOpened(const QString &strChannel)
{
    QString strData = "--- Log opened "+QDateTime::currentDateTime().toString(Qt::TextDate);

    QString strLogsFormat = Settings::instance()->get("logs_format");
    if (strLogsFormat == "html")
    {
        strData = "<div><strong>"+strData+"</strong></div>";
        Log::save(strChannel, strData, Log::Html);
    }
    else
    {
        Log::save(strChannel, strData, Log::Txt);
    }
}

void Log::logClosed(const QString &strChannel)
{
    QString strData = "--- Log closed "+QDateTime::currentDateTime().toString(Qt::TextDate);

    QString strLogsFormat = Settings::instance()->get("logs_format");
    if (strLogsFormat == "html")
    {
        strData = "<div><strong>"+strData+"</strong></div>";
        Log::save(strChannel, strData, Log::Html);
    }
    else
    {
        Log::save(strChannel, strData, Log::Txt);
    }
}

void Log::save(const QString &strChannel, const QString &strMessage, Log::LogsFormat currentLogFormat)
{
    if (!Settings::instance()->getBool("logs")) return;
    if (strChannel.isEmpty() || strMessage.isEmpty()) return;

    QString strCurrentProfile = Settings::instance()->get("current_profile");
    QString path;
#ifdef Q_OS_WIN
    path = QFileInfo(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)).absoluteFilePath();
    path += "/scc/";
#else
    path = QDir::homePath()+"/.scc/";
#endif

    path += "profiles/"+strCurrentProfile+"/log/";

    // save logs by date
    bool bLogsByDate = Settings::instance()->getBool("logs_by_date");
    if (bLogsByDate)
    {
        QString strDateY = QDate().currentDate().toString("yyyy");
        QString strDateM = QDate().currentDate().toString("MM");
        QString strDateD = QDate().currentDate().toString("dd");
        path += strDateY+"/"+strDateM+"/"+strDateD+"/";
    }

    // create dir if not exist
    if (!QDir().exists(path))
        QDir().mkpath(path);

    QString strFileName = strChannel;

    // fixed priv name
    if (strFileName.at(0) == '^')
        strFileName = Channel::instance()->getAlternativeName(strFileName);

    // convert
    QString strSaveMessage = strMessage;

    // file
    QString strFileExtension = (currentLogFormat == Log::Html ? "html" : "txt");
    QFile f(path+strFileName+"."+strFileExtension);

    if (currentLogFormat == Log::Html)
    {
        if (!f.exists())
        {
            QString strHeadCSS = HtmlMessagesRenderer::instance()->headCSS();
            QString strBodyCSS = HtmlMessagesRenderer::instance()->bodyCSS();

            strSaveMessage = "<!DOCTYPE html><html><head><title>"+strFileName+"</title><meta http-equiv=\"content-type\" content=\"text/html; charset=utf-8\" /><style type=\"text/css\">"+strHeadCSS+"</style></head><body style=\""+strBodyCSS+"\">"+strSaveMessage;
        }

        QString path;
#ifdef Q_OS_WIN
        path = QCoreApplication::applicationDirPath();
#else
        path = SCC_DATA_DIR;
#endif

        strSaveMessage = strSaveMessage.replace("qrc:/images/breeze", "file://"+path+"/images");
        strSaveMessage = strSaveMessage.replace("qrc:", "file://"+path);
    }

    if (f.open(QIODevice::Append))
    {

        if (strChannel != DEBUG_WINDOW)
            Convert::simpleConvert(strSaveMessage);

        // save
        QTextStream out(&f);
        out << strSaveMessage << "\r\n";

        f.close();
    }
}
