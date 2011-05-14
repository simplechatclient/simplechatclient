/****************************************************************************
 *                                                                          *
 *   This file is part of Simple Chat Client                                *
 *   Copyright (C) 2011 Piotr Łuczko <piotr.luczko@gmail.com>               *
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

#include <QColorDialog>
#include <QDesktopWidget>
#include <QFileDialog>
#include <QMessageBox>
#include <QProcess>
#include <QSettings>
#include <QShowEvent>
#include <QStyleFactory>
#include "config.h"
#include "core.h"
#include "crypt.h"
#include "dlg_register_nick.h"
#include "notify.h"
#include "dlg_options.h"

DlgOptions::DlgOptions(QWidget *parent) : QDialog(parent)
{
    ui.setupUi(this);
    setWindowTitle(tr("Options"));
    // center screen
    move(QApplication::desktop()->screen()->rect().center() - rect().center());

    create_gui();
    set_default_values();
    create_signals();
}

void DlgOptions::create_gui()
{
    ui.pushButton_register_nick->setIcon(QIcon(":/images/oxygen/16x16/list-add-user.png"));
    ui.pushButton_mainwindow_restore_default->setIcon(QIcon(":/images/oxygen/16x16/edit-undo.png"));
    ui.pushButton_nicklist_restore_default->setIcon(QIcon(":/images/oxygen/16x16/edit-undo.png"));
    ui.pushButton_set_embedded_style->setIcon(QIcon(":/images/oxygen/16x16/dialog-ok-apply.png"));
    ui.pushButton_play_beep->setIcon(QIcon(":/images/oxygen/16x16/media-playback-start.png"));
    ui.pushButton_play_query->setIcon(QIcon(":/images/oxygen/16x16/media-playback-start.png"));
    ui.pushButton_sound_beep_change->setIcon(QIcon(":/images/oxygen/16x16/document-edit.png"));
    ui.pushButton_sound_query_change->setIcon(QIcon(":/images/oxygen/16x16/document-edit.png"));
    ui.pushButton_logs_open_folder->setIcon(QIcon(":/images/oxygen/16x16/folder-txt.png"));
    ui.pushButton_set_background_image->setIcon(QIcon(":/images/oxygen/16x16/insert-image.png"));
    ui.buttonBox->button(QDialogButtonBox::Ok)->setIcon(QIcon(":/images/oxygen/16x16/dialog-ok.png"));
    ui.buttonBox->button(QDialogButtonBox::Cancel)->setIcon(QIcon(":/images/oxygen/16x16/dialog-cancel.png"));

    // page basic
    ui.radioButton_unregistered_nick->setText(tr("Unregistered nick"));
    ui.radioButton_registered_nick->setText(tr("Registered nick"));
    ui.pushButton_register_nick->setText(tr("Register nick"));

    ui.label_nick->setText(tr("Nick:"));
    ui.label_password->setText(tr("Password:"));
    ui.groupBox_skins->setTitle(tr("Skins"));
    ui.radioButton_modern_avatars->setText(tr("Modern"));
    ui.radioButton_modern_no_avatars->setText(tr("Modern without avatars"));
    ui.radioButton_classic->setText(tr("Classic"));
    ui.groupBox_language->setTitle(tr("Language"));

    // page adv
    ui.checkBox_auto_busy->setText(tr("Busy mode after you log in to chat"));
    ui.checkBox_disable_autojoin_favourites->setText(tr("Disable autojoin favourite channels"));
    ui.checkBox_show_zuo->setText(tr("Show ZUO"));
    ui.checkBox_hide_formating->setText(tr("Disable font size, color..."));
    ui.checkBox_hide_join_part->setText(tr("Hide join/part"));
    ui.checkBox_hide_join_part_200->setText(tr("Hide join/part on big channels"));
    ui.checkBox_disable_avatars->setText(tr("Disable avatars"));
    ui.checkBox_disable_emots->setText(tr("Disable emoticons"));
    ui.checkBox_disable_replaces->setText(tr("Disable replaces"));

    // page default font
    ui.groupBox_my_font->setTitle(tr("Default font"));
    ui.label_my_bold->setText(tr("Bold:"));
    ui.comboBox_my_bold->setItemText(0, tr("Off"));
    ui.comboBox_my_bold->setItemText(1, tr("On"));
    ui.label_my_italic->setText(tr("Italic:"));
    ui.comboBox_my_italic->setItemText(0, tr("Off"));
    ui.comboBox_my_italic->setItemText(1, tr("On"));
    ui.label_my_font->setText(tr("Font:"));
    ui.label_my_color->setText(tr("Color:"));
    ui.label_font_size->setText(tr("Font size:"));

    // page colors
    ui.tabWidget->setTabText(0,tr("Main window"));
    ui.tabWidget->setTabText(1,tr("Nicklist"));

    ui.label_background_color->setText(tr("Background color:"));
    ui.label_default_font_color->setText(tr("Default font color:"));
    ui.label_join_font_color->setText(tr("Join color:"));
    ui.label_part_font_color->setText(tr("Part color:"));
    ui.label_quit_font_color->setText(tr("Quit color:"));
    ui.label_kick_font_color->setText(tr("Kick color:"));
    ui.label_mode_font_color->setText(tr("Mode color:"));
    ui.label_notice_font_color->setText(tr("Notice color:"));
    ui.label_info_font_color->setText(tr("Info color:"));
    ui.label_error_font_color->setText(tr("Error color:"));
    ui.label_channel_font_color->setText(tr("Channel color:"));
    ui.pushButton_mainwindow_restore_default->setText(tr("Restore default"));

    ui.label_nicklist_nick_color->setText(tr("Nick color:"));
    ui.label_nicklist_selected_nick_color->setText(tr("Selected nick color:"));
    ui.label_nicklist_busy_nick_color->setText(tr("Busy nick color:"));
    ui.label_nicklist_gradient_1_color->setText(tr("Gradient 1:"));
    ui.label_nicklist_gradient_2_color->setText(tr("Gradient 2:"));
    ui.pushButton_nicklist_restore_default->setText(tr("Restore default"));

    // page embedded styles
    ui.groupBox_embedded_styles->setTitle(tr("Embedded styles"));
    ui.pushButton_set_embedded_style->setText(tr("Set"));

    // page sounds
    ui.groupBox_sounds->setTitle(tr("Sounds"));
    ui.label_sound_beep->setText(tr("Beep"));
    ui.label_sound_query->setText(tr("Query"));
    ui.pushButton_sound_beep_change->setText(tr("Change"));
    ui.pushButton_sound_query_change->setText(tr("Change"));
    ui.checkBox_disable_sounds->setText(tr("Disable sounds"));

    // page logs
    ui.groupBox_logs->setTitle(tr("Logs"));
    ui.label_logs->setText(tr("Default logs folder:"));
    ui.pushButton_logs_open_folder->setText(tr("Open folder"));
    ui.checkBox_save_logs_by_date->setText(tr("Save logs by date"));
    ui.checkBox_disable_logs->setText(tr("Disable logs"));

    // background image
    ui.groupBox_background_image->setTitle(tr("Background image"));
    ui.label_background_image->setText(tr("Default background image:"));
    ui.pushButton_set_background_image->setText(tr("Set"));
    ui.checkBox_disable_background_image->setText(tr("Disable background image"));

    // spell checker
    ui.groupBox_spellchecker->setTitle(tr("Spellchecker"));
    ui.radioButton_spellchecker_active->setText(tr("Active"));
    ui.radioButton_spellchecker_inactive->setText(tr("Inactive"));

    // options list
    QTreeWidgetItem *basic = new QTreeWidgetItem(ui.treeWidget_options);
    basic->setIcon(0, QIcon(":/images/oxygen/16x16/view-media-artist.png"));
    basic->setText(0, tr("Basic"));
    basic->setToolTip(0, tr("Basic"));

    QTreeWidgetItem *adv = new QTreeWidgetItem(ui.treeWidget_options);
    adv->setIcon(0, QIcon(":/images/oxygen/16x16/dialog-warning.png"));
    adv->setText(0, tr("Advanced"));
    adv->setToolTip(0, tr("Advanced"));

    QTreeWidgetItem *default_font = new QTreeWidgetItem(ui.treeWidget_options);
    default_font->setIcon(0, QIcon(":/images/oxygen/16x16/format-text-color.png"));
    default_font->setText(0, tr("Default font"));
    default_font->setToolTip(0, tr("Default font"));

    QTreeWidgetItem *colors = new QTreeWidgetItem(ui.treeWidget_options);
    colors->setIcon(0, QIcon(":/images/oxygen/16x16/view-media-visualization.png"));
    colors->setText(0, tr("Colors"));
    colors->setToolTip(0, tr("Colors"));

    QTreeWidgetItem *embedded_styles = new QTreeWidgetItem(ui.treeWidget_options);
    embedded_styles->setIcon(0, QIcon(":/images/oxygen/16x16/system-switch-user.png"));
    embedded_styles->setText(0, tr("Embedded styles"));
    embedded_styles->setToolTip(0, tr("Embedded styles"));

    QTreeWidgetItem *sounds = new QTreeWidgetItem(ui.treeWidget_options);
    sounds->setIcon(0, QIcon(":/images/oxygen/16x16/media-playback-start.png"));
    sounds->setText(0, tr("Sounds"));
    sounds->setToolTip(0, tr("Sounds"));

    QTreeWidgetItem *logs = new QTreeWidgetItem(ui.treeWidget_options);
    logs->setIcon(0, QIcon(":/images/oxygen/16x16/text-field.png"));
    logs->setText(0, tr("Logs"));
    logs->setToolTip(0, tr("Logs"));

    QTreeWidgetItem *background_image = new QTreeWidgetItem(ui.treeWidget_options);
    background_image->setIcon(0, QIcon(":/images/oxygen/16x16/games-config-background.png"));
    background_image->setText(0, tr("Background image"));
    background_image->setToolTip(0, tr("Background image"));

    QTreeWidgetItem *spell_checker = new QTreeWidgetItem(ui.treeWidget_options);
    spell_checker->setIcon(0, QIcon(":/images/oxygen/16x16/character-set.png"));
    spell_checker->setText(0, tr("Spellchecker"));
    spell_checker->setToolTip(0, tr("Spellchecker"));
}

void DlgOptions::set_default_values()
{
    QSettings settings;

    // open folder command
#ifdef Q_WS_X11
    if (QFile::exists("/usr/bin/nautilus"))
        strOpenFolderCommand = "nautilus";
    else if (QFile::exists("/usr/bin/dolphin"))
        strOpenFolderCommand = "dolphin";
#else
    strOpenFolderCommand = "explorer.exe";
#endif

    // current option
    ui.treeWidget_options->setCurrentItem(ui.treeWidget_options->itemAt(0,0));

    // language
    QStringList strlLanguage;
    strlLanguage << tr("English") << tr("Polish");
    ui.comboBox_language->addItems(strlLanguage);

    // my font
    QStringList strlMyFont;
    strlMyFont << "Arial" << "Times" << "Verdana" << "Tahoma" << "Courier";
    ui.comboBox_my_font->insertItems(0, strlMyFont);

    // my color
    ui.comboBox_my_color->setIconSize(QSize(50,10));

    QStringList comboBoxMyColors;
    comboBoxMyColors << "#000000" << "#623c00" << "#c86c00" << "#ff6500" << "#ff0000" << "#e40f0f" << "#990033" << "#8800ab" << "#ce00ff" << "#0f2ab1" << "#3030ce" << "#006699" << "#1a866e" << "#008100" << "#959595";

    int iComboBoxMyColors = 0;
    foreach (QString strColor, comboBoxMyColors)
    {
        QPixmap pixmap(50,10);
        pixmap.fill(QColor(strColor));
        ui.comboBox_my_color->insertItem(iComboBoxMyColors, pixmap, "");
        iComboBoxMyColors++;
    }

    // font size
    QStringList comboBoxFontSize;
    comboBoxFontSize << "8" << "9" << "10" << "11" << "12" << "14" << "16" << "18" << "20" << "24";

    foreach (QString strFontSize, comboBoxFontSize)
        ui.comboBox_font_size->addItem(strFontSize);

    // embedded styles
    foreach (QString strStyleName, QStyleFactory::keys())
        ui.comboBox_embedded_styles->addItem(strStyleName);

    // sound beep
    ui.lineEdit_sound_beep->setText(settings.value("sound_beep").toString());
    ui.lineEdit_sound_query->setText(settings.value("sound_query").toString());

    // logs
    QString strLogsPath;
#ifdef Q_WS_X11
    strLogsPath = QDir::homePath()+"/.scc/log";
#else
    QSettings mySettings(QSettings::UserScope, "Microsoft", "Windows");
    mySettings.beginGroup("CurrentVersion/Explorer/Shell Folders");
    QString path = mySettings.value("Personal").toString();
    path += "/scc";

    strLogsPath = path+"/log";
    strLogsPath.replace("/", "\\");
#endif
    ui.lineEdit_logs_folder->setText(strLogsPath);

    if (strOpenFolderCommand.isEmpty())
        ui.pushButton_logs_open_folder->setEnabled(false);

    // background image
    ui.lineEdit_background_image->setText(settings.value("background_image").toString());

    // default values
    QString strNick = settings.value("nick").toString();
    QString strPass = settings.value("pass").toString();
    QString strStyle = settings.value("style").toString();
    QString strLanguage = settings.value("language").toString();

    QString strAutoBusy = settings.value("auto_busy").toString();
    QString strDisableAutojoinFavourites = settings.value("disable_autojoin_favourites").toString();
    QString strShowZuo = settings.value("show_zuo").toString();
    QString strHideFormating = settings.value("hide_formating").toString();
    QString strHideJoinPart = settings.value("hide_join_part").toString();
    QString strHideJoinPart200 = settings.value("hide_join_part_200").toString();
    QString strDisableAvatars = settings.value("disable_avatars").toString();
    QString strDisableEmots = settings.value("disable_emots").toString();
    QString strDisableReplaces = settings.value("disable_replaces").toString();

    QString strMyBold = settings.value("my_bold").toString();
    QString strMyItalic = settings.value("my_italic").toString();
    QString strMyFont = settings.value("my_font").toString();
    QString strMyColor = settings.value("my_color").toString();
    QString strFontSize = settings.value("font_size").toString();

    QString strSaveLogsByDate = settings.value("save_logs_by_date").toString();
    QString strDisableLogs = settings.value("disable_logs").toString();
    QString strDisableSounds = settings.value("disable_sounds").toString();
    QString strDisableBackgroundImage = settings.value("disable_background_image").toString();
    QString strSpellchecker = settings.value("spellchecker").toString();

    // decrypt pass
    if (!strPass.isEmpty())
    {
        Crypt *pCrypt = new Crypt();
        strPass = pCrypt->decrypt(strNick, strPass);
        delete pCrypt;
    }

    // set nick staly/tyldowy
    if (strPass.isEmpty())
    {
        ui.radioButton_unregistered_nick->setChecked(true);
        ui.radioButton_registered_nick->setChecked(false);
        ui.label_password->hide();
        ui.lineEdit_password->hide();
    }
    else
    {
        ui.radioButton_unregistered_nick->setChecked(false);
        ui.radioButton_registered_nick->setChecked(true);
    }

    // set nick and pass
    ui.lineEdit_nick->setText(strNick);
    ui.lineEdit_password->setText(strPass);

    // set style
    if (strStyle == "modern")
    {
        if (strDisableAvatars == "off")
            ui.radioButton_modern_avatars->setChecked(true);
        else
            ui.radioButton_modern_no_avatars->setChecked(true);
    }
    else if (strStyle == "classic")
        ui.radioButton_classic->setChecked(true);

    // language
    if (strLanguage == "en")
        ui.comboBox_language->setCurrentIndex(0);
    else if (strLanguage == "pl")
        ui.comboBox_language->setCurrentIndex(1);
    else
        ui.comboBox_language->setCurrentIndex(1);

    // auto busy
    if (strAutoBusy == "on")
        ui.checkBox_auto_busy->setChecked(true);
    else
        ui.checkBox_auto_busy->setChecked(false);

    // auto busy
    if (strDisableAutojoinFavourites == "on")
        ui.checkBox_disable_autojoin_favourites->setChecked(true);
    else
        ui.checkBox_disable_autojoin_favourites->setChecked(false);

    // show zuo
    if (strShowZuo == "on")
        ui.checkBox_show_zuo->setChecked(true);
    else
        ui.checkBox_show_zuo->setChecked(false);

    // hide formating
    if (strHideFormating == "on")
        ui.checkBox_hide_formating->setChecked(true);
    else
        ui.checkBox_hide_formating->setChecked(false);

    // hide join part
    if (strHideJoinPart == "on")
        ui.checkBox_hide_join_part->setChecked(true);
    else
        ui.checkBox_hide_join_part->setChecked(false);

    // hide join part
    if (strHideJoinPart200 == "on")
        ui.checkBox_hide_join_part_200->setChecked(true);
    else
        ui.checkBox_hide_join_part_200->setChecked(false);

    // disable avatars
    if (strDisableAvatars == "on")
        ui.checkBox_disable_avatars->setChecked(true);
    else
        ui.checkBox_disable_avatars->setChecked(false);

    // disable emots
    if (strDisableEmots == "on")
        ui.checkBox_disable_emots->setChecked(true);
    else
        ui.checkBox_disable_emots->setChecked(false);

    // disable replaces
    if (strDisableReplaces == "on")
        ui.checkBox_disable_replaces->setChecked(true);
    else
        ui.checkBox_disable_replaces->setChecked(false);

    // set my bold
    if (strMyBold == "on")
        ui.comboBox_my_bold->setCurrentIndex(1);
    else
        ui.comboBox_my_bold->setCurrentIndex(0);

    // set my italic
    if (strMyItalic == "on")
        ui.comboBox_my_italic->setCurrentIndex(1);
    else
        ui.comboBox_my_italic->setCurrentIndex(0);

    // set my font combobox
    for (int i = 0; i < ui.comboBox_my_font->count(); i++)
    {
        if (ui.comboBox_my_font->itemText(i) == strMyFont)
            ui.comboBox_my_font->setCurrentIndex(i);
    }

    // set my color combobox
    int iMyColor;

    if (strMyColor == "#000000") iMyColor = 0;
    else if (strMyColor == "#623c00") iMyColor = 1;
    else if (strMyColor == "#c86c00") iMyColor = 2;
    else if (strMyColor == "#ff6500") iMyColor = 3;
    else if (strMyColor == "#ff0000") iMyColor = 4;
    else if (strMyColor == "#e40f0f") iMyColor = 5;
    else if (strMyColor == "#990033") iMyColor = 6;
    else if (strMyColor == "#8800ab") iMyColor = 7;
    else if (strMyColor == "#ce00ff") iMyColor = 8;
    else if (strMyColor == "#0f2ab1") iMyColor = 9;
    else if (strMyColor == "#3030ce") iMyColor = 10;
    else if (strMyColor == "#006699") iMyColor = 11;
    else if (strMyColor == "#1a866e") iMyColor = 12;
    else if (strMyColor == "#008100") iMyColor = 13;
    else if (strMyColor == "#959595") iMyColor = 14;
    else
        iMyColor = 0;

    ui.comboBox_my_color->setCurrentIndex(iMyColor);

    // set font size combobox
    int iFontSize;

    if (strFontSize == "8px") iFontSize = 0;
    else if (strFontSize == "9px") iFontSize = 1;
    else if (strFontSize == "10px") iFontSize = 2;
    else if (strFontSize == "11px") iFontSize = 3;
    else if (strFontSize == "12px") iFontSize = 4;
    else if (strFontSize == "14px") iFontSize = 5;
    else if (strFontSize == "16px") iFontSize = 6;
    else if (strFontSize == "18px") iFontSize = 7;
    else if (strFontSize == "20px") iFontSize = 8;
    else if (strFontSize == "24px") iFontSize = 9;
    else
        iFontSize = 0;

    ui.comboBox_font_size->setCurrentIndex(iFontSize);

    // save logs by date
    if (strSaveLogsByDate == "on")
        ui.checkBox_save_logs_by_date->setChecked(true);
    else
        ui.checkBox_save_logs_by_date->setChecked(false);

    // disable logs
    if (strDisableLogs == "on")
        ui.checkBox_disable_logs->setChecked(true);
    else
        ui.checkBox_disable_logs->setChecked(false);

    // disable sounds
    if (strDisableSounds == "on")
        ui.checkBox_disable_sounds->setChecked(true);
    else
        ui.checkBox_disable_sounds->setChecked(false);

    // disable background image
    if (strDisableBackgroundImage == "on")
        ui.checkBox_disable_background_image->setChecked(true);
    else
        ui.checkBox_disable_background_image->setChecked(false);

    // spellchecker
    if (strSpellchecker == "on")
    {
        ui.radioButton_spellchecker_active->setChecked(true);
        ui.radioButton_spellchecker_inactive->setChecked(false);
    }
    else
    {
        ui.radioButton_spellchecker_active->setChecked(false);
        ui.radioButton_spellchecker_inactive->setChecked(true);
    }

    // set mainwindow colors
    set_mainwindow_colors();

    // set nicklist colors
    set_nicklist_colors();

    // disable change nick if connected
    if (settings.value("logged").toString() == "on")
    {
        ui.radioButton_registered_nick->setDisabled(true);
        ui.radioButton_unregistered_nick->setDisabled(true);
        ui.pushButton_register_nick->setDisabled(true);
        ui.groupBox_login_password->setDisabled(true);
        ui.groupBox_skins->setDisabled(true);
        ui.radioButton_modern_avatars->setDisabled(true);
        ui.radioButton_modern_no_avatars->setDisabled(true);
        ui.radioButton_classic->setDisabled(true);
    }
    else
    {
        ui.radioButton_registered_nick->setDisabled(false);
        ui.radioButton_unregistered_nick->setDisabled(false);
        ui.pushButton_register_nick->setDisabled(false);
        ui.groupBox_login_password->setDisabled(false);
        ui.groupBox_skins->setDisabled(false);
        ui.radioButton_modern_avatars->setDisabled(false);
        ui.radioButton_modern_no_avatars->setDisabled(false);
        ui.radioButton_classic->setDisabled(false);
    }
}

void DlgOptions::create_signals()
{
    QObject::connect(ui.treeWidget_options, SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)), this, SLOT(change_page(QTreeWidgetItem*,QTreeWidgetItem*)));
    QObject::connect(ui.radioButton_unregistered_nick, SIGNAL(clicked()), this, SLOT(hide_pass()));
    QObject::connect(ui.radioButton_registered_nick, SIGNAL(clicked()), this, SLOT(show_pass()));
    QObject::connect(ui.pushButton_register_nick, SIGNAL(clicked()), this, SLOT(button_register_nick()));
    QObject::connect(ui.radioButton_modern_avatars, SIGNAL(clicked()), this, SLOT(set_modern_style_avatars()));
    QObject::connect(ui.radioButton_modern_no_avatars, SIGNAL(clicked()), this, SLOT(set_modern_style_no_avatars()));
    QObject::connect(ui.radioButton_classic, SIGNAL(clicked()), this, SLOT(set_classic_style()));
    QObject::connect(ui.comboBox_language, SIGNAL(currentIndexChanged(int)), this, SLOT(language_changed(int)));
    QObject::connect(ui.checkBox_auto_busy, SIGNAL(clicked(bool)), this, SLOT(auto_busy(bool)));
    QObject::connect(ui.checkBox_disable_autojoin_favourites, SIGNAL(clicked(bool)), this, SLOT(disable_autojoin_favourites(bool)));
    QObject::connect(ui.checkBox_show_zuo, SIGNAL(clicked(bool)), this, SLOT(show_zuo(bool)));
    QObject::connect(ui.checkBox_hide_formating, SIGNAL(clicked(bool)), this, SLOT(hide_formating(bool)));
    QObject::connect(ui.checkBox_hide_join_part, SIGNAL(clicked(bool)), this, SLOT(hide_join_part(bool)));
    QObject::connect(ui.checkBox_hide_join_part_200, SIGNAL(clicked(bool)), this, SLOT(hide_join_part_200(bool)));
    QObject::connect(ui.checkBox_disable_avatars, SIGNAL(clicked(bool)), this, SLOT(disable_avatars(bool)));
    QObject::connect(ui.checkBox_disable_emots, SIGNAL(clicked(bool)), this, SLOT(disable_emots(bool)));
    QObject::connect(ui.checkBox_disable_replaces, SIGNAL(clicked(bool)), this, SLOT(disable_replaces(bool)));
    QObject::connect(ui.comboBox_my_bold, SIGNAL(currentIndexChanged(int)), this, SLOT(set_my_bold(int)));
    QObject::connect(ui.comboBox_my_italic, SIGNAL(currentIndexChanged(int)), this, SLOT(set_my_italic(int)));
    QObject::connect(ui.comboBox_my_font, SIGNAL(currentIndexChanged(QString)), this, SLOT(set_my_font(QString)));
    QObject::connect(ui.comboBox_my_color, SIGNAL(currentIndexChanged(int)), this, SLOT(set_my_color(int)));
    QObject::connect(ui.comboBox_font_size, SIGNAL(currentIndexChanged(QString)), this, SLOT(set_font_size(QString)));
    QObject::connect(ui.pushButton_background_color, SIGNAL(clicked()), this, SLOT(set_background_color()));
    QObject::connect(ui.pushButton_default_font_color, SIGNAL(clicked()), this, SLOT(set_default_font_color()));
    QObject::connect(ui.pushButton_join_font_color, SIGNAL(clicked()), this, SLOT(set_join_font_color()));
    QObject::connect(ui.pushButton_part_font_color, SIGNAL(clicked()), this, SLOT(set_part_font_color()));
    QObject::connect(ui.pushButton_quit_font_color, SIGNAL(clicked()), this, SLOT(set_quit_font_color()));
    QObject::connect(ui.pushButton_kick_font_color, SIGNAL(clicked()), this, SLOT(set_kick_font_color()));
    QObject::connect(ui.pushButton_mode_font_color, SIGNAL(clicked()), this, SLOT(set_mode_font_color()));
    QObject::connect(ui.pushButton_notice_font_color, SIGNAL(clicked()), this, SLOT(set_notice_font_color()));
    QObject::connect(ui.pushButton_info_font_color, SIGNAL(clicked()), this, SLOT(set_info_font_color()));
    QObject::connect(ui.pushButton_error_font_color, SIGNAL(clicked()), this, SLOT(set_error_font_color()));
    QObject::connect(ui.pushButton_channel_font_color, SIGNAL(clicked()), this, SLOT(set_channel_font_color()));
    QObject::connect(ui.pushButton_mainwindow_restore_default, SIGNAL(clicked()), this, SLOT(mainwindow_restore_default()));
    QObject::connect(ui.pushButton_nicklist_nick_color, SIGNAL(clicked()), this, SLOT(set_nicklist_nick_color()));
    QObject::connect(ui.pushButton_nicklist_selected_nick_color, SIGNAL(clicked()), this, SLOT(set_nicklist_selected_nick_color()));
    QObject::connect(ui.pushButton_nicklist_busy_nick_color, SIGNAL(clicked()), this, SLOT(set_nicklist_busy_nick_color()));
    QObject::connect(ui.pushButton_nicklist_gradient_1_color, SIGNAL(clicked()), this, SLOT(set_nicklist_gradient_1_color()));
    QObject::connect(ui.pushButton_nicklist_gradient_2_color, SIGNAL(clicked()), this, SLOT(set_nicklist_gradient_2_color()));
    QObject::connect(ui.pushButton_nicklist_restore_default, SIGNAL(clicked()), this, SLOT(nicklist_restore_default()));
    QObject::connect(ui.pushButton_set_embedded_style, SIGNAL(clicked()), this, SLOT(set_embedded_style()));
    QObject::connect(ui.pushButton_play_beep, SIGNAL(clicked()), this, SLOT(try_play_beep()));
    QObject::connect(ui.pushButton_play_query, SIGNAL(clicked()), this, SLOT(try_play_query()));
    QObject::connect(ui.pushButton_sound_beep_change, SIGNAL(clicked()), this, SLOT(set_sound_beep()));
    QObject::connect(ui.pushButton_sound_query_change, SIGNAL(clicked()), this, SLOT(set_sound_query()));
    QObject::connect(ui.checkBox_disable_sounds, SIGNAL(clicked(bool)), this, SLOT(disable_sounds(bool)));
    QObject::connect(ui.pushButton_logs_open_folder, SIGNAL(clicked()), this, SLOT(open_logs_folder()));
    QObject::connect(ui.checkBox_save_logs_by_date, SIGNAL(clicked(bool)), this, SLOT(set_save_logs_by_date(bool)));
    QObject::connect(ui.checkBox_disable_logs, SIGNAL(clicked(bool)), this, SLOT(disable_logs(bool)));
    QObject::connect(ui.pushButton_set_background_image, SIGNAL(clicked()), this, SLOT(set_background_image()));
    QObject::connect(ui.checkBox_disable_background_image, SIGNAL(clicked(bool)), this, SLOT(disable_background_image(bool)));
    QObject::connect(ui.radioButton_spellchecker_active, SIGNAL(clicked()), this, SLOT(set_spellchecker()));
    QObject::connect(ui.radioButton_spellchecker_inactive, SIGNAL(clicked()), this, SLOT(set_spellchecker()));
    QObject::connect(ui.buttonBox, SIGNAL(accepted()), this, SLOT(button_ok()));
    QObject::connect(ui.buttonBox, SIGNAL(rejected()), this, SLOT(button_cancel()));
}

void DlgOptions::change_page(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
    if (!current)
        current = previous;

    ui.stackedWidget->setCurrentIndex(ui.treeWidget_options->currentIndex().row());
}

void DlgOptions::hide_pass()
{
    ui.label_password->hide();
    ui.lineEdit_password->hide();

    // fix nick
    if (ui.lineEdit_nick->text()[0] != '~')
        ui.lineEdit_nick->setText("~"+ui.lineEdit_nick->text());
}

void DlgOptions::show_pass()
{
    ui.label_password->show();
    ui.lineEdit_password->show();

    // fix nick
    if (ui.lineEdit_nick->text()[0] == '~')
        ui.lineEdit_nick->setText(ui.lineEdit_nick->text().right(ui.lineEdit_nick->text().length()-1));
}

void DlgOptions::button_register_nick()
{
    DlgRegisterNick(Core::instance()->sccWindow(), this).exec();
}

void DlgOptions::set_modern_style_avatars()
{
    // save settings
    save_settings();

    // save style
    QSettings settings;
    Config *pConfig = new Config();
    pConfig->set_value("style", "modern");
    settings.setValue("style", "modern");
    pConfig->set_value("disable_avatars", "off");
    settings.setValue("disable_avatars", "off");
    delete pConfig;

    QMessageBox::information(0, "", tr("Restart program to apply the changes."));
}

void DlgOptions::set_modern_style_no_avatars()
{
    // save settings
    save_settings();

    // save style
    QSettings settings;
    Config *pConfig = new Config();
    pConfig->set_value("style", "modern");
    settings.setValue("style", "modern");
    pConfig->set_value("disable_avatars", "on");
    settings.setValue("disable_avatars", "on");
    delete pConfig;

    QMessageBox::information(0, "", tr("Restart program to apply the changes."));
}

void DlgOptions::set_classic_style()
{
    // save settings
    save_settings();

    // save style
    QSettings settings;
    Config *pConfig = new Config();
    pConfig->set_value("style", "classic");
    settings.setValue("style", "classic");
    pConfig->set_value("disable_avatars", "on");
    settings.setValue("disable_avatars", "on");
    delete pConfig;

    QMessageBox::information(0, "", tr("Restart program to apply the changes."));
}

void DlgOptions::language_changed(int index)
{
    QSettings settings;
    Config *pConfig = new Config();
    if (index == 0) // english
    {
        pConfig->set_value("language", "en");
        settings.setValue("language", "en");
    }
    else if (index == 1) // polish
    {
        pConfig->set_value("language", "pl");
        settings.setValue("language", "pl");
    }
    else // polish
    {
        pConfig->set_value("language", "pl");
        settings.setValue("language", "pl");
    }
    delete pConfig;

    QMessageBox::information(0, "", tr("Restart program to apply the changes."));
}

void DlgOptions::auto_busy(bool bValue)
{
    QString strValue = (bValue ? "on" : "off");

    QSettings settings;
    Config *pConfig = new Config();
    pConfig->set_value("auto_busy", strValue);
    settings.setValue("auto_busy", strValue);
    delete pConfig;
}

void DlgOptions::disable_autojoin_favourites(bool bValue)
{
    QString strValue = (bValue ? "on" : "off");

    QSettings settings;
    Config *pConfig = new Config();
    pConfig->set_value("disable_autojoin_favourites", strValue);
    settings.setValue("disable_autojoin_favourites", strValue);
    delete pConfig;
}

void DlgOptions::show_zuo(bool bValue)
{
    QString strValue = (bValue ? "on" : "off");

    QSettings settings;
    Config *pConfig = new Config();
    pConfig->set_value("show_zuo", strValue);
    settings.setValue("show_zuo", strValue);
    delete pConfig;
}

void DlgOptions::hide_formating(bool bValue)
{
    QString strValue = (bValue ? "on" : "off");

    QSettings settings;
    Config *pConfig = new Config();
    pConfig->set_value("hide_formating", strValue);
    settings.setValue("hide_formating", strValue);
    delete pConfig;
}

void DlgOptions::hide_join_part(bool bValue)
{
    QString strValue = (bValue ? "on" : "off");

    QSettings settings;
    Config *pConfig = new Config();
    pConfig->set_value("hide_join_part", strValue);
    settings.setValue("hide_join_part", strValue);
    delete pConfig;
}

void DlgOptions::hide_join_part_200(bool bValue)
{
    QString strValue = (bValue ? "on" : "off");

    QSettings settings;
    Config *pConfig = new Config();
    pConfig->set_value("hide_join_part_200", strValue);
    settings.setValue("hide_join_part_200", strValue);
    delete pConfig;
}

void DlgOptions::disable_avatars(bool bValue)
{
    QString strValue = (bValue ? "on" : "off");

    QSettings settings;
    Config *pConfig = new Config();
    pConfig->set_value("disable_avatars", strValue);
    settings.setValue("disable_avatars", strValue);
    delete pConfig;
}

void DlgOptions::disable_emots(bool bValue)
{
    QString strValue = (bValue ? "on" : "off");

    QSettings settings;
    Config *pConfig = new Config();
    pConfig->set_value("disable_emots", strValue);
    settings.setValue("disable_emots", strValue);
    delete pConfig;
}

void DlgOptions::disable_replaces(bool bValue)
{
    QString strValue = (bValue ? "on" : "off");

    QSettings settings;
    Config *pConfig = new Config();
    pConfig->set_value("disable_replaces", strValue);
    settings.setValue("disable_replaces", strValue);
    delete pConfig;
}

void DlgOptions::set_my_bold(int index)
{
    QString strValue = (index == 0 ? "off" : "on");

    QSettings settings;
    Config *pConfig = new Config();
    pConfig->set_value("my_bold", strValue);
    settings.setValue("my_bold", strValue);
    delete pConfig;
}

void DlgOptions::set_my_italic(int index)
{
    QString strValue = (index == 0 ? "off" : "on");

    QSettings settings;
    Config *pConfig = new Config();
    pConfig->set_value("my_italic", strValue);
    settings.setValue("my_italic", strValue);
    delete pConfig;
}

void DlgOptions::set_my_font(QString strFont)
{
    QSettings settings;
    Config *pConfig = new Config();
    pConfig->set_value("my_font", strFont);
    settings.setValue("my_font", strFont);
    delete pConfig;
}

void DlgOptions::set_my_color(int index)
{
    QString strMyColor;
    if (index == 0) strMyColor = "#000000";
    else if (index == 1) strMyColor = "#623c00";
    else if (index == 2) strMyColor = "#c86c00";
    else if (index == 3) strMyColor = "#ff6500";
    else if (index == 4) strMyColor = "#ff0000";
    else if (index == 5) strMyColor = "#e40f0f";
    else if (index == 6) strMyColor = "#990033";
    else if (index == 7) strMyColor = "#8800ab";
    else if (index == 8) strMyColor = "#ce00ff";
    else if (index == 9) strMyColor = "#0f2ab1";
    else if (index == 10) strMyColor = "#3030ce";
    else if (index == 11) strMyColor = "#006699";
    else if (index == 12) strMyColor = "#1a866e";
    else if (index == 13) strMyColor = "#008100";
    else if (index == 14) strMyColor = "#959595";
    else strMyColor = "#000000";

    QSettings settings;
    Config *pConfig = new Config();
    pConfig->set_value("my_color", strMyColor);
    settings.setValue("my_color", strMyColor);
    delete pConfig;
}

void DlgOptions::set_font_size(QString strFontSize)
{
    QSettings settings;
    Config *pConfig = new Config();
    pConfig->set_value("font_size", strFontSize+"px");
    settings.setValue("font_size", strFontSize+"px");
    delete pConfig;
}

void DlgOptions::set_background_color()
{
    set_color("background_color");
}

void DlgOptions::set_default_font_color()
{
    set_color("default_font_color");
}

void DlgOptions::set_join_font_color()
{
    set_color("font_color_level_1");
}

void DlgOptions::set_part_font_color()
{
    set_color("font_color_level_2");
}

void DlgOptions::set_quit_font_color()
{
    set_color("font_color_level_3");
}

void DlgOptions::set_kick_font_color()
{
    set_color("font_color_level_4");
}

void DlgOptions::set_mode_font_color()
{
    set_color("font_color_level_5");
}

void DlgOptions::set_notice_font_color()
{
    set_color("font_color_level_6");
}

void DlgOptions::set_info_font_color()
{
    set_color("font_color_level_7");
}

void DlgOptions::set_error_font_color()
{
    set_color("font_color_level_9");
}

void DlgOptions::set_channel_font_color()
{
    set_color("channel_font_color");
}

void DlgOptions::mainwindow_restore_default()
{
    QSettings settings;
    Config *pConfig = new Config();

    pConfig->set_value("background_color", "#ffffff");
    settings.setValue("background_color", "#ffffff");

    pConfig->set_value("default_font_color", "#000000");
    settings.setValue("default_font_color", "#000000");

    pConfig->set_value("font_color_level_1", "#009300");
    settings.setValue("font_color_level_1", "#009300");

    pConfig->set_value("font_color_level_2", "#4733FF");
    settings.setValue("font_color_level_2", "#4733FF");

    pConfig->set_value("font_color_level_3", "#00007F");
    settings.setValue("font_color_level_3", "#00007F");

    pConfig->set_value("font_color_level_4", "#00007F");
    settings.setValue("font_color_level_4", "#00007F");

    pConfig->set_value("font_color_level_5", "#009300");
    settings.setValue("font_color_level_5", "#009300");

    pConfig->set_value("font_color_level_6", "#0066FF");
    settings.setValue("font_color_level_6", "#0066FF");

    pConfig->set_value("font_color_level_7", "#666666");
    settings.setValue("font_color_level_7", "#666666");

    pConfig->set_value("font_color_level_9", "#ff0000");
    settings.setValue("font_color_level_9", "#ff0000");

    pConfig->set_value("channel_font_color", "#0000ff");
    settings.setValue("channel_font_color", "#0000ff");

    delete pConfig;

    // set
    set_mainwindow_colors();

    // refresh tabs
    Core::instance()->refresh_colors();
}

void DlgOptions::set_nicklist_nick_color()
{
    set_color("nicklist_nick_color");
}

void DlgOptions::set_nicklist_selected_nick_color()
{
    set_color("nicklist_selected_nick_color");
}

void DlgOptions::set_nicklist_busy_nick_color()
{
    set_color("nicklist_busy_nick_color");
}

void DlgOptions::set_nicklist_gradient_1_color()
{
    set_color("nicklist_gradient_1_color");
}

void DlgOptions::set_nicklist_gradient_2_color()
{
    set_color("nicklist_gradient_2_color");
}

void DlgOptions::nicklist_restore_default()
{
    QSettings settings;
    Config *pConfig = new Config();

    pConfig->set_value("nicklist_nick_color", "#333333");
    settings.setValue("nicklist_nick_color", "#333333");

    pConfig->set_value("nicklist_selected_nick_color", "#ffffff");
    settings.setValue("nicklist_selected_nick_color", "#ffffff");

    pConfig->set_value("nicklist_busy_nick_color", "#a0a0a4");
    settings.setValue("nicklist_busy_nick_color", "#a0a0a4");

    pConfig->set_value("nicklist_gradient_1_color", "#77d5f7");
    settings.setValue("nicklist_gradient_1_color", "#77d5f7");

    pConfig->set_value("nicklist_gradient_2_color", "#1b86b7");
    settings.setValue("nicklist_gradient_2_color", "#1b86b7");

    delete pConfig;

    // set
    set_nicklist_colors();
}

void DlgOptions::set_embedded_style()
{
    QStyle *style = QStyleFactory::create(ui.comboBox_embedded_styles->currentText());
    QApplication::setStyle(style);
}

void DlgOptions::try_play_beep()
{
    Notify::instance()->play(Beep);
}

void DlgOptions::try_play_query()
{
    Notify::instance()->play(Query);
}

void DlgOptions::set_sound_beep()
{
    QString selectedFilter;
    QString fileName = QFileDialog::getOpenFileName(this,
                                     tr("Select Audio File"),
                                     ui.lineEdit_sound_beep->text(),
                                     tr("All Files (*);;Mp3 Files (*.mp3);;Wave Files (*.wav)"),
                                     &selectedFilter,
                                     0);
    if (!fileName.isEmpty())
    {
        QSettings settings;
        Config *pConfig = new Config();

        pConfig->set_value("sound_beep", fileName);
        settings.setValue("sound_beep", fileName);

        delete pConfig;
        ui.lineEdit_sound_beep->setText(fileName);
    }
}

void DlgOptions::set_sound_query()
{
    QString selectedFilter;
    QString fileName = QFileDialog::getOpenFileName(this,
                                     tr("Select Audio File"),
                                     ui.lineEdit_sound_query->text(),
                                     tr("All Files (*);;Mp3 Files (*.mp3);;Wave Files (*.wav)"),
                                     &selectedFilter,
                                     0);
    if (!fileName.isEmpty())
    {
        QSettings settings;
        Config *pConfig = new Config();

        pConfig->set_value("sound_query", fileName);
        settings.setValue("sound_query", fileName);

        delete pConfig;
        ui.lineEdit_sound_query->setText(fileName);
    }
}

void DlgOptions::disable_sounds(bool bValue)
{
    QString strValue = (bValue ? "on" : "off");

    QSettings settings;
    Config *pConfig = new Config();
    pConfig->set_value("disable_sounds", strValue);
    settings.setValue("disable_sounds", strValue);
    delete pConfig;
}

void DlgOptions::open_logs_folder()
{
    QString strLogsPath = ui.lineEdit_logs_folder->text();

    QProcess pProcess;
    pProcess.execute(strOpenFolderCommand+" "+strLogsPath);
}

void DlgOptions::set_save_logs_by_date(bool bValue)
{
    QString strValue = (bValue ? "on" : "off");

    QSettings settings;
    Config *pConfig = new Config();
    pConfig->set_value("save_logs_by_date", strValue);
    settings.setValue("save_logs_by_date", strValue);
    delete pConfig;
}

void DlgOptions::disable_logs(bool bValue)
{
    QString strValue = (bValue ? "on" : "off");

    QSettings settings;
    Config *pConfig = new Config();
    pConfig->set_value("disable_logs", strValue);
    settings.setValue("disable_logs", strValue);
    delete pConfig;
}

void DlgOptions::set_background_image()
{
    QString selectedFilter;
    QString fileName = QFileDialog::getOpenFileName(this,
                                     tr("Select Image File"),
                                     ui.lineEdit_background_image->text(),
                                     tr("All Files (*);;JPG Files (*.jpg);;PNG Files (*.png);;Bitmap Files (*.bmp)"),
                                     &selectedFilter,
                                     0);
    if (!fileName.isEmpty())
    {
        QSettings settings;
        Config *pConfig = new Config();

        pConfig->set_value("background_image", fileName);
        settings.setValue("background_image", fileName);

        delete pConfig;
        ui.lineEdit_background_image->setText(fileName);

        // refresh
        Core::instance()->refresh_background_image();
    }
}

void DlgOptions::disable_background_image(bool bValue)
{
    QString strValue = (bValue ? "on" : "off");

    QSettings settings;
    Config *pConfig = new Config();
    pConfig->set_value("disable_background_image", strValue);
    settings.setValue("disable_background_image", strValue);
    delete pConfig;

    // refresh
    Core::instance()->refresh_background_image();
}

void DlgOptions::set_spellchecker()
{
    QString strValue = (ui.radioButton_spellchecker_active->isChecked() ? "on" : "off");

    QSettings settings;
    Config *pConfig = new Config();
    pConfig->set_value("spellchecker", strValue);
    settings.setValue("spellchecker", strValue);
    delete pConfig;
}

void DlgOptions::button_cancel()
{
    this->close();
}

void DlgOptions::button_ok()
{
    // save
    save_settings();

    this->close();
}

void DlgOptions::save_settings()
{
    // get values
    QString strNick = ui.lineEdit_nick->text();

    // check nick
    QString strPass;
    if (ui.radioButton_unregistered_nick->isChecked())
    {
        // fix nick
        if (strNick[0] != '~')
            strNick = "~"+strNick;

        strPass = "";
    }
    else if (ui.radioButton_registered_nick->isChecked())
    {
        // fix nick
        if (strNick[0] == '~')
            strNick = strNick.right(strNick.length()-1);

        strPass = ui.lineEdit_password->text();
    }

    // encrypt pass
    if (!strPass.isEmpty())
    {
        Crypt *pCrypt = new Crypt();
        strPass = pCrypt->encrypt(strNick, strPass);
        delete pCrypt;
    }

    // save values
    QSettings settings;
    settings.setValue("nick", strNick);
    settings.setValue("pass", strPass);

    Config *pConfig = new Config();
    pConfig->set_value("nick", strNick);
    pConfig->set_value("pass", strPass);
    delete pConfig;
}

void DlgOptions::set_color(QString strKey)
{
    // get current value
    QSettings settings;
    QString strDefaultColor = settings.value(strKey).toString();

    // color dialog
    QColor cColor = QColorDialog::getColor(QColor(strDefaultColor), this);

    if (cColor.isValid())
    {
        // color to name
        QString strColor = cColor.name();

        // save
        Config *pConfig = new Config();
        pConfig->set_value(strKey, strColor);
        settings.setValue(strKey, strColor);
        delete pConfig;

        // save to pushbutton
        QPixmap color(50,15);
        color.fill(QColor(cColor));

        if (strKey == "background_color")
            ui.pushButton_background_color->setIcon(QIcon(color));
        else if (strKey == "background_color")
            ui.pushButton_background_color->setIcon(QIcon(color));
        else if (strKey == "default_font_color")
            ui.pushButton_default_font_color->setIcon(QIcon(color));
        else if (strKey == "font_color_level_1")
            ui.pushButton_join_font_color->setIcon(QIcon(color));
        else if (strKey == "font_color_level_2")
            ui.pushButton_part_font_color->setIcon(QIcon(color));
        else if (strKey == "font_color_level_3")
            ui.pushButton_quit_font_color->setIcon(QIcon(color));
        else if (strKey == "font_color_level_4")
            ui.pushButton_kick_font_color->setIcon(QIcon(color));
        else if (strKey == "font_color_level_5")
            ui.pushButton_mode_font_color->setIcon(QIcon(color));
        else if (strKey == "font_color_level_6")
            ui.pushButton_notice_font_color->setIcon(QIcon(color));
        else if (strKey == "font_color_level_7")
            ui.pushButton_info_font_color->setIcon(QIcon(color));
        else if (strKey == "font_color_level_9")
            ui.pushButton_error_font_color->setIcon(QIcon(color));
        else if (strKey == "channel_font_color")
            ui.pushButton_channel_font_color->setIcon(QIcon(color));
        else if (strKey == "nicklist_nick_color")
            ui.pushButton_nicklist_nick_color->setIcon(QIcon(color));
        else if (strKey == "nicklist_selected_nick_color")
            ui.pushButton_nicklist_selected_nick_color->setIcon(QIcon(color));
        else if (strKey == "nicklist_busy_nick_color")
            ui.pushButton_nicklist_busy_nick_color->setIcon(QIcon(color));
        else if (strKey == "nicklist_gradient_1_color")
            ui.pushButton_nicklist_gradient_1_color->setIcon(QIcon(color));
        else if (strKey == "nicklist_gradient_2_color")
            ui.pushButton_nicklist_gradient_2_color->setIcon(QIcon(color));

        // refresh
        Core::instance()->refresh_colors();
    }
}

void DlgOptions::set_mainwindow_colors()
{
    QSettings settings;
    QString strBackgroundColor = settings.value("background_color").toString();
    QString strDefaultFontColor = settings.value("default_font_color").toString();
    QString strJoinFontColor = settings.value("font_color_level_1").toString();
    QString strPartFontColor = settings.value("font_color_level_2").toString();
    QString strQuitFontColor = settings.value("font_color_level_3").toString();
    QString strKickFontColor = settings.value("font_color_level_4").toString();
    QString strModeFontColor = settings.value("font_color_level_5").toString();
    QString strNoticeFontColor = settings.value("font_color_level_6").toString();
    QString strInfoFontColor = settings.value("font_color_level_7").toString();
    QString strErrorFontColor = settings.value("font_color_level_9").toString();
    QString strChannelFontColor = settings.value("channel_font_color").toString();

    // set background color
    QPixmap bcolor(50,15);
    bcolor.fill(QColor(strBackgroundColor));
    ui.pushButton_background_color->setIcon(QIcon(bcolor));

    // set default font color
    QPixmap dfcolor(50,15);
    dfcolor.fill(QColor(strDefaultFontColor));
    ui.pushButton_default_font_color->setIcon(QIcon(dfcolor));

    // set join font color
    QPixmap jfcolor(50,15);
    jfcolor.fill(QColor(strJoinFontColor));
    ui.pushButton_join_font_color->setIcon(QIcon(jfcolor));

    // set part font color
    QPixmap pfcolor(50,15);
    pfcolor.fill(QColor(strPartFontColor));
    ui.pushButton_part_font_color->setIcon(QIcon(pfcolor));

    // set quit font color
    QPixmap qfcolor(50,15);
    qfcolor.fill(QColor(strQuitFontColor));
    ui.pushButton_quit_font_color->setIcon(QIcon(qfcolor));

    // set kick font color
    QPixmap kfcolor(50,15);
    kfcolor.fill(QColor(strKickFontColor));
    ui.pushButton_kick_font_color->setIcon(QIcon(kfcolor));

    // set mode font color
    QPixmap mfcolor(50,15);
    mfcolor.fill(QColor(strModeFontColor));
    ui.pushButton_mode_font_color->setIcon(QIcon(mfcolor));

    // set notice font color
    QPixmap nfcolor(50,15);
    nfcolor.fill(QColor(strNoticeFontColor));
    ui.pushButton_notice_font_color->setIcon(QIcon(nfcolor));

    // set info font color
    QPixmap ifcolor(50,15);
    ifcolor.fill(QColor(strInfoFontColor));
    ui.pushButton_info_font_color->setIcon(QIcon(ifcolor));

    // set error font color
    QPixmap efcolor(50,15);
    efcolor.fill(QColor(strErrorFontColor));
    ui.pushButton_error_font_color->setIcon(QIcon(efcolor));

    // set channel font color
    QPixmap cfcolor(50,15);
    cfcolor.fill(QColor(strChannelFontColor));
    ui.pushButton_channel_font_color->setIcon(QIcon(cfcolor));
}

void DlgOptions::set_nicklist_colors()
{
    QSettings settings;
    QString strNicklistNickColor = settings.value("nicklist_nick_color").toString();
    QString strNicklistSelectedNickColor = settings.value("nicklist_selected_nick_color").toString();
    QString strNicklistBusyNickColor = settings.value("nicklist_busy_nick_color").toString();
    QString strNicklistGradient1Color = settings.value("nicklist_gradient_1_color").toString();
    QString strNicklistGradient2Color = settings.value("nicklist_gradient_2_color").toString();

    // set nicklist nick color
    QPixmap nncolor(50,15);
    nncolor.fill(QColor(strNicklistNickColor));
    ui.pushButton_nicklist_nick_color->setIcon(QIcon(nncolor));

    // set nicklist selected nick color
    QPixmap nsncolor(50,15);
    nsncolor.fill(QColor(strNicklistSelectedNickColor));
    ui.pushButton_nicklist_selected_nick_color->setIcon(QIcon(nsncolor));

    // set nicklist busy nick color
    QPixmap nbncolor(50,15);
    nbncolor.fill(QColor(strNicklistBusyNickColor));
    ui.pushButton_nicklist_busy_nick_color->setIcon(QIcon(nbncolor));

    // set nicklist gradient 1 color
    QPixmap ng1color(50,15);
    ng1color.fill(QColor(strNicklistGradient1Color));
    ui.pushButton_nicklist_gradient_1_color->setIcon(QIcon(ng1color));

    // set nicklist gradient 2 color
    QPixmap ng2color(50,15);
    ng2color.fill(QColor(strNicklistGradient2Color));
    ui.pushButton_nicklist_gradient_2_color->setIcon(QIcon(ng2color));
}
