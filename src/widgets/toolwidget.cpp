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

#include <QComboBox>
#include <QHBoxLayout>
#include <QMenu>
#include <QPushButton>
#include <QSettings>
#include "core.h"
#include "dlg_channel_settings.h"
#include "dlg_emoticons.h"
#include "dlg_moderation.h"
#include "inputwidget.h"
#include "network.h"
#include "toolwidget.h"

ToolWidget::ToolWidget(QWidget *parent, Network *param1, InputWidget *param2, DlgChannelSettings *param3, DlgModeration *param4) : QWidget(parent)
{
    pNetwork = param1;
    pInputWidget = param2;
    pDlgChannelSettings = param3;
    pDlgModeration = param4;

    strCurrentColor = "#000000";

    showFontButtons = new QPushButton(QIcon(":/images/oxygen/16x16/format-text-color.png"), "", this);
    showFontButtons->setToolTip(tr("Show/Hide font buttons"));
    showFontButtons->setMaximumWidth(25);
    showFontButtons->setMaximumHeight(25);
    showFontButtons->show();

    separator1 = new QFrame(this);
    separator1->setFrameShape(QFrame::VLine);
    separator1->setFrameShadow(QFrame::Sunken);
    separator1->show();

    bold = new QPushButton(QIcon(":/images/oxygen/16x16/format-text-bold.png"), "", this);
    bold->setToolTip(tr("Bold"));
    bold->setFlat(true);
    bold->setCheckable(true);
    bold->setMaximumWidth(25);
    bold->setMaximumHeight(25);
    bold->show();
    bMyBold = false;

    italic = new QPushButton(QIcon(":/images/oxygen/16x16/format-text-italic.png"), "", this);
    italic->setToolTip(tr("Italic"));
    italic->setFlat(true);
    italic->setCheckable(true);
    italic->setMaximumWidth(25);
    italic->setMaximumHeight(25);
    italic->show();
    bMyItalic = false;

    arialAct = new QAction("Arial", this);
    arialAct->setFont(QFont("Arial", -1, -1, false));
    timesAct = new QAction("Times", this);
    timesAct->setFont(QFont("Times New Roman", -1, -1, false));
    verdanaAct = new QAction("Verdana", this);
    verdanaAct->setFont(QFont("Verdana", -1, -1, false));
    tahomaAct = new QAction("Tahoma", this);
    tahomaAct->setFont(QFont("Tahoma", -1, -1, false));
    courierAct = new QAction("Courier", this);
    courierAct->setFont(QFont("Courier New", -1, -1, false));

    fontMenu = new QMenu(this);
    fontMenu->addAction(arialAct);
    fontMenu->addAction(timesAct);
    fontMenu->addAction(verdanaAct);
    fontMenu->addAction(tahomaAct);
    fontMenu->addAction(courierAct);

    fontfamily = new QPushButton(this);
    fontfamily->setToolTip(tr("Font family"));
    fontfamily->setFont(QFont("Verdana", -1, -1, false));
    fontfamily->setText("Verdana");
    fontfamily->setMaximumWidth(250);
    fontfamily->setMaximumHeight(25);
    fontfamily->setMenu(fontMenu);
    fontfamily->show();

    color = new QComboBox(this);
    color->setToolTip(tr("Font color"));
    color->setIconSize(QSize(20,10));

    QStringList comboBoxColors;
    comboBoxColors << "#000000" << "#623c00" << "#c86c00" << "#ff6500" << "#ff0000" << "#e40f0f" << "#990033" << "#8800ab" << "#ce00ff" << "#0f2ab1" << "#3030ce" << "#006699" << "#1a866e" << "#008100" << "#959595";

    int iComboBoxColors = 0;
    foreach (QString strColor, comboBoxColors)
    {
        QPixmap pixmap(20,10);
        pixmap.fill(QColor(strColor));
        color->insertItem(iComboBoxColors, pixmap, "");
        iComboBoxColors++;
    }
    color->show();

    size8Act = new QAction("8", this);
    size8Act->setFont(QFont("Verdana", 8, -1, false));
    size9Act = new QAction("9", this);
    size9Act->setFont(QFont("Verdana", 9, -1, false));
    size10Act = new QAction("10", this);
    size10Act->setFont(QFont("Verdana", 10, -1, false));
    size11Act = new QAction("11", this);
    size11Act->setFont(QFont("Verdana", 11, -1, false));
    size12Act = new QAction("12", this);
    size12Act->setFont(QFont("Verdana", 12, -1, false));
    size14Act = new QAction("14", this);
    size14Act->setFont(QFont("Verdana", 14, -1, false));
    size16Act = new QAction("16", this);
    size16Act->setFont(QFont("Verdana", 16, -1, false));
    size18Act = new QAction("18", this);
    size18Act->setFont(QFont("Verdana", 18, -1, false));
    size20Act = new QAction("20", this);
    size20Act->setFont(QFont("Verdana", 20, -1, false));
    size24Act = new QAction("24", this);
    size24Act->setFont(QFont("Verdana", 24, -1, false));

    sizeMenu = new QMenu(this);
    sizeMenu->addAction(size8Act);
    sizeMenu->addAction(size9Act);
    sizeMenu->addAction(size10Act);
    sizeMenu->addAction(size11Act);
    sizeMenu->addAction(size12Act);
    sizeMenu->addAction(size14Act);
    sizeMenu->addAction(size16Act);
    sizeMenu->addAction(size18Act);
    sizeMenu->addAction(size20Act);
    sizeMenu->addAction(size24Act);

    size = new QPushButton(QIcon(":/images/oxygen/16x16/format-font-size-more.png"), "", this);
    size->setToolTip(tr("Font size"));
    size->setFont(QFont("Times New Roman", -1, -1, false));
    size->setMaximumHeight(25);
    size->setMenu(sizeMenu);
    size->show();

    separator2 = new QFrame(this);
    separator2->setFrameShape(QFrame::VLine);
    separator2->setFrameShadow(QFrame::Sunken);
    separator2->show();

    emoticons = new QPushButton(QIcon(":/images/oxygen/16x16/face-smile.png"), "", this);
    emoticons->setToolTip(tr("Emoticons"));
    emoticons->setMaximumWidth(25);
    emoticons->setMaximumHeight(25);
    emoticons->show();

    channel_settings = new QPushButton(QIcon(":/images/oxygen/16x16/configure.png"), "", this);
    channel_settings->setToolTip(tr("Channel settings"));
    channel_settings->setMaximumWidth(25);
    channel_settings->setMaximumHeight(25);
    channel_settings->show();

    moderation = new QPushButton(QIcon(":/images/oxygen/16x16/go-last.png"), tr("Moderation"), this);
    moderation->setToolTip(tr("Moderation"));
    moderation->show();

    clear = new QPushButton(QIcon(":/images/oxygen/16x16/draw-eraser.png"), "", this);
    clear->setToolTip(tr("Clear"));
    clear->setMaximumWidth(25);
    clear->setMaximumHeight(25);
    clear->show();

    toolLayout = new QHBoxLayout();
    toolLayout->setMargin(0);
    toolLayout->setAlignment(Qt::AlignLeft);
    toolLayout->addWidget(showFontButtons);
    toolLayout->addWidget(separator1);
    toolLayout->addWidget(bold);
    toolLayout->addWidget(italic);
    toolLayout->addWidget(fontfamily);
    toolLayout->addWidget(color);
    toolLayout->addWidget(size);
    toolLayout->addWidget(separator2);
    toolLayout->addWidget(emoticons);
    toolLayout->addWidget(channel_settings);
    toolLayout->addWidget(moderation);
    toolLayout->addWidget(clear);
    setLayout(toolLayout);

    // set default
    setDefault();

    // signals
    QObject::connect(showFontButtons, SIGNAL(clicked()), this, SLOT(showFontButtonsClicked()));

    QObject::connect(bold, SIGNAL(clicked()), this, SLOT(boldClicked()));
    QObject::connect(italic, SIGNAL(clicked()), this, SLOT(italicClicked()));

    QObject::connect(arialAct, SIGNAL(triggered()), this, SLOT(arialTriggered()));
    QObject::connect(timesAct, SIGNAL(triggered()), this, SLOT(timesTriggered()));
    QObject::connect(verdanaAct, SIGNAL(triggered()), this, SLOT(verdanaTriggered()));
    QObject::connect(tahomaAct, SIGNAL(triggered()), this, SLOT(tahomaTriggered()));
    QObject::connect(courierAct, SIGNAL(triggered()), this, SLOT(courierTriggered()));

    QObject::connect(size8Act, SIGNAL(triggered()), this, SLOT(size8Triggered()));
    QObject::connect(size9Act, SIGNAL(triggered()), this, SLOT(size9Triggered()));
    QObject::connect(size10Act, SIGNAL(triggered()), this, SLOT(size10Triggered()));
    QObject::connect(size11Act, SIGNAL(triggered()), this, SLOT(size11Triggered()));
    QObject::connect(size12Act, SIGNAL(triggered()), this, SLOT(size12Triggered()));
    QObject::connect(size14Act, SIGNAL(triggered()), this, SLOT(size14Triggered()));
    QObject::connect(size16Act, SIGNAL(triggered()), this, SLOT(size16Triggered()));
    QObject::connect(size18Act, SIGNAL(triggered()), this, SLOT(size18Triggered()));
    QObject::connect(size20Act, SIGNAL(triggered()), this, SLOT(size20Triggered()));
    QObject::connect(size24Act, SIGNAL(triggered()), this, SLOT(size24Triggered()));

    QObject::connect(color, SIGNAL(currentIndexChanged(int)), this, SLOT(colorClicked(int)));
    QObject::connect(emoticons, SIGNAL(clicked()), this, SLOT(emoticonsClicked()));
    QObject::connect(channel_settings, SIGNAL(clicked()), this, SLOT(channelSettingsClicked()));
    QObject::connect(moderation, SIGNAL(clicked()), this, SLOT(moderationClicked()));
    QObject::connect(clear, SIGNAL(clicked()), this, SLOT(clearClicked()));
}

void ToolWidget::setDefault()
{
    QSettings settings;

    // font buttons
    bShowFontButtons = false;
    separator1->hide();
    bold->hide();
    italic->hide();
    fontfamily->hide();
    color->hide();
    size->hide();
    separator2->hide();

    // set default bold
    if (settings.value("my_bold").toString() == "on")
    {
        bold->setChecked(true);
        bMyBold = true;
        strMyFontWeight = "bold";
    }
    else
    {
        bold->setChecked(false);
        bMyBold = false;
        strMyFontWeight = "";
    }

    // set default italic
    if (settings.value("my_italic").toString() == "on")
    {
        italic->setChecked(true);
        bMyItalic = true;
        strMyFontStyle = "italic";
    }
    else
    {
        italic->setChecked(false);
        bMyItalic = false;
        strMyFontStyle = "";
    }

    // refresh bold italic
    int iWeight = (bMyBold ? 75 : 50);
    pInputWidget->setFont(QFont(strMyFontFamily, -1, iWeight, bMyItalic));

    // set default font
    QString strMyFont = settings.value("my_font").toString();
    fontfamily->setText(strMyFont);
    strMyFontFamily = strMyFont;

    // set default color
    QString strMyColor = settings.value("my_color").toString();
    strCurrentColor = strMyColor;

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

    color->setCurrentIndex(iMyColor);

    // moderation default hidden
    moderation->hide();
}

void ToolWidget::setModeration(bool bEnable)
{
    if (bEnable)
        moderation->show();
    else
        moderation->hide();
}

void ToolWidget::setChannelSettings(bool bEnable)
{
    if (bEnable)
        channel_settings->show();
    else
        channel_settings->hide();
}

void ToolWidget::showFontButtonsClicked()
{
    if (bShowFontButtons)
    {
        bShowFontButtons = false;
        separator1->hide();
        bold->hide();
        italic->hide();
        fontfamily->hide();
        color->hide();
        size->hide();
        separator2->hide();
    }
    else
    {
        bShowFontButtons = true;
        separator1->show();
        bold->show();
        italic->show();
        fontfamily->show();
        color->show();
        size->show();
        separator2->show();
    }
}

void ToolWidget::boldClicked()
{
    QSettings settings;
    if (bMyBold)
    {
        bold->setChecked(false);
        bMyBold = false;
        strMyFontWeight = "";
        settings.setValue("my_bold", "off");
    }
    else
    {
        bold->setChecked(true);
        bMyBold = true;
        strMyFontWeight = "bold";
        settings.setValue("my_bold", "on");
    }

    int iWeight = (bMyBold ? 75 : 50);
    pInputWidget->setFont(QFont(strMyFontFamily, -1, iWeight, bMyItalic));
}

void ToolWidget::italicClicked()
{
    QSettings settings;
    if (bMyItalic)
    {
        italic->setChecked(false);
        bMyItalic = false;
        strMyFontStyle = "";
        settings.setValue("my_italic", "off");
    }
    else
    {
        italic->setChecked(true);
        bMyItalic = true;
        strMyFontStyle = "italic";
        settings.setValue("my_italic", "on");
    }

    int iWeight = (bMyBold ? 75 : 50);
    pInputWidget->setFont(QFont(strMyFontFamily, -1, iWeight, bMyItalic));
}

void ToolWidget::arialTriggered()
{
    fontfamily->setText("Arial");
    strMyFontFamily = "Arial";
    QSettings settings;
    settings.setValue("my_font", strMyFontFamily);

    int iWeight = (bMyBold ? 75 : 50);
    pInputWidget->setFont(QFont(strMyFontFamily, -1, iWeight, bMyItalic));
}

void ToolWidget::timesTriggered()
{
    fontfamily->setText("Times");
    strMyFontFamily = "Times";
    QSettings settings;
    settings.setValue("my_font", strMyFontFamily);

    int iWeight = (bMyBold ? 75 : 50);
    pInputWidget->setFont(QFont(strMyFontFamily, -1, iWeight, bMyItalic));
}

void ToolWidget::verdanaTriggered()
{
    fontfamily->setText("Verdana");
    strMyFontFamily = "Verdana";
    QSettings settings;
    settings.setValue("my_font", strMyFontFamily);

    int iWeight = (bMyBold ? 75 : 50);
    pInputWidget->setFont(QFont(strMyFontFamily, -1, iWeight, bMyItalic));
}

void ToolWidget::tahomaTriggered()
{
    fontfamily->setText("Tahoma");
    strMyFontFamily = "Tahoma";
    QSettings settings;
    settings.setValue("my_font", strMyFontFamily);

    int iWeight = (bMyBold ? 75 : 50);
    pInputWidget->setFont(QFont(strMyFontFamily, -1, iWeight, bMyItalic));
}

void ToolWidget::courierTriggered()
{
    fontfamily->setText("Courier");
    strMyFontFamily = "Courier";
    QSettings settings;
    settings.setValue("my_font", strMyFontFamily);

    int iWeight = (bMyBold ? 75 : 50);
    pInputWidget->setFont(QFont(strMyFontFamily, -1, iWeight, bMyItalic));
}

void ToolWidget::size8Triggered()
{
    emit changeFontSize("8px");
}

void ToolWidget::size9Triggered()
{
    emit changeFontSize("9px");
}

void ToolWidget::size10Triggered()
{
    emit changeFontSize("10px");
}

void ToolWidget::size11Triggered()
{
    emit changeFontSize("11px");
}

void ToolWidget::size12Triggered()
{
    emit changeFontSize("12px");
}

void ToolWidget::size14Triggered()
{
    emit changeFontSize("14px");
}

void ToolWidget::size16Triggered()
{
    emit changeFontSize("16px");
}

void ToolWidget::size18Triggered()
{
    emit changeFontSize("18px");
}

void ToolWidget::size20Triggered()
{
    emit changeFontSize("20px");
}

void ToolWidget::size24Triggered()
{
    emit changeFontSize("24px");
}

// color

void ToolWidget::colorClicked(int index)
{
    if (index == 0) strCurrentColor = "#000000";
    else if (index == 1) strCurrentColor = "#623c00";
    else if (index == 2) strCurrentColor = "#c86c00";
    else if (index == 3) strCurrentColor = "#ff6500";
    else if (index == 4) strCurrentColor = "#ff0000";
    else if (index == 5) strCurrentColor = "#e40f0f";
    else if (index == 6) strCurrentColor = "#990033";
    else if (index == 7) strCurrentColor = "#8800ab";
    else if (index == 8) strCurrentColor = "#ce00ff";
    else if (index == 9) strCurrentColor = "#0f2ab1";
    else if (index == 10) strCurrentColor = "#3030ce";
    else if (index == 11) strCurrentColor = "#006699";
    else if (index == 12) strCurrentColor = "#1a866e";
    else if (index == 13) strCurrentColor = "#008100";
    else if (index == 14) strCurrentColor = "#959595";
    else strCurrentColor = "#000000";

    QSettings settings;
    settings.setValue("my_color", strCurrentColor);

    pInputWidget->setColor(strCurrentColor);
}

// emoticons

void ToolWidget::emoticonsClicked()
{
    DlgEmoticons(Core::instance()->sccWindow(), pInputWidget).exec();
}

// channel settings

void ToolWidget::channelSettingsClicked()
{
    if (pNetwork->isConnected())
    {
        QString strChannel = pInputWidget->getActive();
        if (strChannel != "Status")
        {
            pDlgChannelSettings->set_channel(strChannel);
            pDlgChannelSettings->show();
        }
    }
}

// moderation

void ToolWidget::moderationClicked()
{
    QString strChannel = pInputWidget->getActive();
    pDlgModeration->set_current_channel(strChannel);
    pDlgModeration->show();
}

// clear

void ToolWidget::clearClicked()
{
    QString strChannel = pInputWidget->getActive();
    emit clearContent(strChannel);
}
