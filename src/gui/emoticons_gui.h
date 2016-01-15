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

#ifndef EMOTICONS_GUI_H
#define EMOTICONS_GUI_H

#include <QDialog>
#include <QThread>
#include "core/defines.h"
#include "ui_emoticons.h"
class InputLineWidget;

QT_BEGIN_NAMESPACE
class QListWidgetItem;
class QListWidget;
QT_END_NAMESPACE

enum EmoticonsRoles
{
    EmoticonNameRole = Qt::UserRole
};

class EmoticonsThread : public QThread
{
    Q_OBJECT
public:
    EmoticonsThread();
    void setParams(const QString &_strDir, EmoticonsCategory _emoticonsCategory);

private:
    QString strDir;
    EmoticonsCategory emoticonsCategory;

protected:
    void run();

signals:
    void addEmoticon(const QString &strEmoticon, const QByteArray &bData);
    void sortEmoticons();
};

class EmoticonsTabGui : public QWidget
{
    Q_OBJECT
public:
    EmoticonsTabGui(const QString &_strDir, EmoticonsCategory emoticonsCategory, QWidget *parent = 0);
    QListWidget *listWidget;
    EmoticonsThread thread;

public slots:
    void addEmoticon(const QString &strEmoticon, const QByteArray &bData);
    void sortEmoticons();
};

class EmoticonsGui : public QDialog
{
    Q_OBJECT
public:
    EmoticonsGui(InputLineWidget *_pInputLineWidget, QWidget *parent = 0);
    virtual ~EmoticonsGui();

private:
    Ui::uiEmoticons ui;
    InputLineWidget *pInputLineWidget;
    QList<int> lReadedTabIndex;

    void createGui();
    void setDefaultValues();
    void createSignals();

    void createEmoticonsTabs();
    void createEmoticonsEmojiTabs();

protected:
    void resizeEvent(QResizeEvent *);

public slots:
    void tabChanged(int index);
    void buttonInsert();
};

#endif // EMOTICONS_GUI_H
