/****************************************************************************
 *                                                                          *
 *   This file is part of Simple Chat Client                                *
 *   Copyright (C) 2010 Piotr �uczko <piotr.luczko@gmail.com>               *
 *                                                                          *
 *   This program is free software: you can redistribute it and/or modify   *
 *   it under the terms of the GNU General Public License as published by   *
 *   the Free Software Foundation, either version 3 of the License, or      *
 *   (at your option) any later version.                                    *
 *                                                                          *
 *   This program is distributed in the hope that it will be useful,        *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *   GNU General Public License for more details.                           *
 *                                                                          *
 *   You should have received a copy of the GNU General Public License      *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 *                                                                          *
 ****************************************************************************/

#ifndef TAB_WIDGET_H
#define TAB_WIDGET_H

#include <QAction>
#include <QtAlgorithms>
#include <QDateTime>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QIcon>
#include <QKeyEvent>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QMap>
#include <QMenu>
#include <QMutex>
#include <QPushButton>
#include <QQueue>
#include <QRgb>
#include <QSettings>
#include <QSplitter>
#include <QTcpSocket>
#include <QTextEdit>
#include <QTimer>
#include <QWidget>
#include "commands.h"
#include "config.h"
#include "dlg_channel_settings.h"
#include "dlg_moderation.h"
#include "log.h"
#include "qinputline.h"
#include "qnicklist.h"
#include "qnotify.h"

class tab_widget : public QWidget
{
    Q_OBJECT
public:
    tab_widget(QString, QWidget *, QTcpSocket *, QSettings *, dlg_channel_settings *, dlg_moderation *);
    ~tab_widget();
    QString get_name() { return strName; }
    QString convert_emots(QString);
    QString replace_emots(QString);
    void display_msg(QString, QString, int);
    void display_msg(QString, int);
    void set_topic(QString);
    void enable_topic();
    void disable_topic();
    void enable_moderation();
    void disable_moderation();
    void author_topic(QString);
    void set_link(QString);
    void add_user(QString, QString);
    void del_user(QString);
    void change_flag(QString, QString);
    void update_nick(QString);
    void clear_nicklist();
    void set_open_channels(QStringList);
    bool nicklist_exist(QString);

private:
    QLineEdit *topic;
    QPushButton *topicButton;
    QLabel *topicDetails;
    QLabel *nickCount;
    QLabel *webLink;
    qnicklist *nick_list;
    QTextEdit *textEdit;
    QLabel *nickLabel;
    qinputline *inputline;
    QWidget *toolWidget;
    QPushButton *sendButton;
    QPushButton *moderSendButton;

    QHBoxLayout *toolBox;
    QPushButton *bold;
    QPushButton *italic;
    QPushButton *fontfamily;
    QComboBox *color;
    QPushButton *channel_settings;
    QPushButton *moderation;

    QMenu *fontMenu;
    QAction *arialAct;
    QAction *timesAct;
    QAction *verdanaAct;
    QAction *tahomaAct;
    QAction *courierAct;

    QPushButton *size;
    QMenu *sizeMenu;
    QAction *size8Act;
    QAction *size9Act;
    QAction *size10Act;
    QAction *size11Act;
    QAction *size12Act;
    QAction *size14Act;
    QAction *size18Act;
    QAction *size20Act;

    QSplitter *splitter;
    QGridLayout *leftLayout;
    QGridLayout *rightLayout;
    QWidget *leftWidget;
    QWidget *rightWidget;
    QGridLayout *mainLayout;
    dlg_channel_settings *dlgchannel_settings;
    dlg_moderation *dlgmoderation;
    qnotify *notify;

    QString strName;
    QTcpSocket *socket;
    QString strLast_msg;
    QSettings *settings;
    QMutex mutex_nicklist;
    QMutex mutex_notify;
    QTimer *timer;
    QMap<QString, QString> nick_flag;
    QMap<QString, QString> nick_suffix;
    QHash <QString, QString> nicklist;
    QQueue <QString> new_nicklist1;
    QQueue <QString> new_nicklist2;
    bool bBold;
    bool bItalic;
    int iNickCount;
    bool bCursorPositionChanged;
    QString strCurrentColor;

    void nicklist_add(QString, QString);
    void nicklist_remove(QString);
    void nicklist_clear();
    void nicklist_refresh();
    void nicklist_sort();
    void nicklist_quicksort(QString);

    void display_message(QString, int);
    void send(QString);

private slots:
    void bold_clicked();
    void italic_clicked();
    void arial_triggered();
    void times_triggered();
    void verdana_triggered();
    void tahoma_triggered();
    void courier_triggered();

    void size8_triggered();
    void size9_triggered();
    void size10_triggered();
    void size11_triggered();
    void size12_triggered();
    void size14_triggered();
    void size18_triggered();
    void size20_triggered();

    void color_clicked(int);

    void channel_settings_clicked();
    void moderation_clicked();
    void inputline_return_pressed();
    void moder_button_clicked();
    void topic_return_pressed();

    void cursor_position_changed();
    void timer_timeout();

protected:
    virtual void keyPressEvent(QKeyEvent*);

};

#endif // TAB_WIDGET_H
