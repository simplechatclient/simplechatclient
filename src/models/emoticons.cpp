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

void Emoticons::createLists()
{
    createEmoticonsList();
    createEmoticonsSimpleLists();
}

void Emoticons::createEmoticonsList()
{
    QString path;
#ifdef Q_OS_WIN
    path = QCoreApplication::applicationDirPath();
#else
    path = SCC_DATA_DIR;
#endif

    QStringList lSupportedEmoticons;
    lSupportedEmoticons << "*.gif" << "*.jpg" << "*.jpeg" << "*.png" << "*.bmp";

    // create emoticons standard list

    QDir dEmoticonsStandardDirs = path+"/emoticons/";
    QStringList lEmoticonsSandardDirs = dEmoticonsStandardDirs.entryList(QStringList("*"), QDir::Dirs | QDir::NoDotAndDotDot | QDir::NoSymLinks, QDir::Name | QDir::IgnoreCase | QDir::LocaleAware);

    foreach (const QString &strEmoticonStandardDir, lEmoticonsSandardDirs)
    {
        QDir dEmoticonStandardDir = QString("%1/emoticons/%2/").arg(path, strEmoticonStandardDir);
        QFileInfoList lEmoticonsStandard = dEmoticonStandardDir.entryInfoList(lSupportedEmoticons, QDir::Files | QDir::NoDotAndDotDot | QDir::NoSymLinks, QDir::Name | QDir::IgnoreCase | QDir::LocaleAware);

        foreach (const QFileInfo &fEmoticon, lEmoticonsStandard) {

            Emoticon newEmoticon;
            newEmoticon.category = EmoticonStandard;
            newEmoticon.path = fEmoticon.absoluteFilePath();
            newEmoticon.name = fEmoticon.baseName();
            newEmoticon.nameWithPrefix = "//"+fEmoticon.baseName();
            newEmoticon.dir = fEmoticon.dir().dirName();
            newEmoticon.dirAbsolute = fEmoticon.absoluteDir().absolutePath();

            lEmoticonsList.append(newEmoticon);
        }

        EmoticonCategories newEmoticonCategories;
        newEmoticonCategories.name = strEmoticonStandardDir;
        newEmoticonCategories.category = EmoticonStandard;
        newEmoticonCategories.imagePath = dEmoticonStandardDir.absolutePath()+"/"+lEmoticonsStandard.first().baseName();
        newEmoticonCategories.dir = dEmoticonStandardDir.absolutePath();
        lEmoticonsCategoryList.append(newEmoticonCategories);
    }

    // create emoticons emoji list

    QDir dEmoticonsEmojiDirs = path+"/emoticons_emoji/";
    QStringList lEmoticonsEmojiDirs = dEmoticonsEmojiDirs.entryList(QStringList("*"), QDir::Dirs | QDir::NoDotAndDotDot | QDir::NoSymLinks, QDir::Name | QDir::IgnoreCase | QDir::LocaleAware);

    foreach (const QString &strEmoticonEmojiDir, lEmoticonsEmojiDirs)
    {
        QDir dEmoticonEmojiDir = QString("%1/emoticons_emoji/%2/").arg(path, strEmoticonEmojiDir);
        QFileInfoList lEmoticonsEmoji = dEmoticonEmojiDir.entryInfoList(lSupportedEmoticons, QDir::Files | QDir::NoDotAndDotDot | QDir::NoSymLinks, QDir::Name | QDir::IgnoreCase | QDir::LocaleAware);

        foreach (const QFileInfo &fEmoticon, lEmoticonsEmoji) {

            Emoticon newEmoticon;
            newEmoticon.category = EmoticonEmoji;
            newEmoticon.path = fEmoticon.absoluteFilePath();
            newEmoticon.name = fEmoticon.baseName();
            newEmoticon.nameWithPrefix = ":"+fEmoticon.baseName()+":";
            newEmoticon.dir = fEmoticon.dir().dirName();
            newEmoticon.dirAbsolute = fEmoticon.absoluteDir().absolutePath();

            lEmoticonsList.append(newEmoticon);
        }

        EmoticonCategories newEmoticonCategories;
        newEmoticonCategories.name = strEmoticonEmojiDir;
        newEmoticonCategories.category = EmoticonEmoji;
        newEmoticonCategories.imagePath = dEmoticonEmojiDir.absolutePath()+"/"+lEmoticonsEmoji.first().baseName();
        newEmoticonCategories.dir = dEmoticonEmojiDir.absolutePath();
        lEmoticonsCategoryList.append(newEmoticonCategories);
    }
}

void Emoticons::createEmoticonsSimpleLists()
{
    foreach (Emoticon eEmoticon, lEmoticonsList) {
        if (eEmoticon.category == EmoticonStandard) {
            lEmoticonsStandardList.append(eEmoticon.nameWithPrefix);
        } else if (eEmoticon.category == EmoticonEmoji) {
            lEmoticonsEmojiList.append(eEmoticon.nameWithPrefix);
        }
    }

    // sort
    qStableSort(lEmoticonsStandardList.begin(), lEmoticonsStandardList.end());

    // sort
    qStableSort(lEmoticonsEmojiList.begin(), lEmoticonsEmojiList.end());
}

QList<EmoticonCategories> Emoticons::listCategories()
{
    if (lEmoticonsList.size() == 0) {
        createLists();
    }

    return lEmoticonsCategoryList;
}

QList<CaseIgnoreString> Emoticons::listEmoticons(EmoticonCategory category)
{
    if (lEmoticonsList.size() == 0) {
        createLists();
    }

    if (category == EmoticonEmoji) {
        return lEmoticonsEmojiList;
    } else {
        return lEmoticonsStandardList;
    }
}

QList<Emoticon> Emoticons::listEmoticonsFromPath(const QString &path)
{
    QList<Emoticon> list;

    foreach (Emoticon eEmoticon, lEmoticonsList) {
        if (eEmoticon.dirAbsolute == path) {
            list.append(eEmoticon);
        }
    }

    return list;
}
