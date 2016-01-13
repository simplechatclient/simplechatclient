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

#include <QDir>
#include "emoticons.h"

#ifdef Q_OS_WIN
    #include <QCoreApplication>
#else
    #include "scc-config.h"
#endif

Emoticons * Emoticons::Instance = 0;

Emoticons * Emoticons::instance()
{
    if (!Instance)
    {
        Instance = new Emoticons();
    }

    return Instance;
}

Emoticons::Emoticons()
{
}

QList<CaseIgnoreString> Emoticons::listEmoticons()
{
    if (lEmoticonsList.size() != 0)
        return lEmoticonsList;

    QString path;
#ifdef Q_OS_WIN
    path = QCoreApplication::applicationDirPath();
#else
    path = SCC_DATA_DIR;
#endif

    QDir dAllEmoticonsDirs = path+"/emoticons/";
    QStringList lDirs = dAllEmoticonsDirs.entryList(QStringList("*"), QDir::Dirs | QDir::NoDotAndDotDot | QDir::NoSymLinks);

    QStringList lSupportedEmoticons;
    lSupportedEmoticons << "*.gif" << "*.jpg" << "*.jpeg" << "*.png" << "*.bmp";

    foreach (const QString &strDir, lDirs)
    {
        QDir dEmoticonDir = QString("%1/emoticons/%2/").arg(path, strDir);
        QFileInfoList lEmoticons = dEmoticonDir.entryInfoList(lSupportedEmoticons, QDir::Files | QDir::NoDotAndDotDot | QDir::NoSymLinks);

        foreach (const QFileInfo &fEmoticon, lEmoticons)
            lEmoticonsList.append("//"+fEmoticon.baseName());
    }

    // sort
    qStableSort(lEmoticonsList.begin(), lEmoticonsList.end());

    return lEmoticonsList;
}

QList<CaseIgnoreString> Emoticons::listEmoticonsEmoji()
{
    if (lEmoticonsEmojiList.size() != 0)
        return lEmoticonsEmojiList;

    QString path;
#ifdef Q_OS_WIN
    path = QCoreApplication::applicationDirPath();
#else
    path = SCC_DATA_DIR;
#endif

    QDir dAllEmoticonsDirs = path+"/emoticons_emoji/";
    QStringList lDirs = dAllEmoticonsDirs.entryList(QStringList("*"), QDir::Dirs | QDir::NoDotAndDotDot | QDir::NoSymLinks);

    QStringList lSupportedEmoticons;
    lSupportedEmoticons << "*.gif" << "*.jpg" << "*.jpeg" << "*.png" << "*.bmp";

    foreach (const QString &strDir, lDirs)
    {
        QDir dEmoticonDir = QString("%1/emoticons_emoji/%2/").arg(path, strDir);
        QFileInfoList lEmoticons = dEmoticonDir.entryInfoList(lSupportedEmoticons, QDir::Files | QDir::NoDotAndDotDot | QDir::NoSymLinks);

        foreach (const QFileInfo &fEmoticon, lEmoticons)
            lEmoticonsEmojiList.append(":"+fEmoticon.baseName()+":");
    }

    // sort
    qStableSort(lEmoticonsEmojiList.begin(), lEmoticonsEmojiList.end());

    return lEmoticonsEmojiList;
}
