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

#include <QComboBox>
#include <QDateTime>
#include <QHBoxLayout>
#include <QMenu>
#include <QToolButton>
#include "models/channel.h"
#include "core/core.h"
#include "common/commands.h"
#include "common/config.h"
#include "common/convert.h"
#include "gui/channel_settings_gui.h"
#include "gui/emoticons_gui.h"
#include "models/message.h"
#include "core/mainwindow.h"
#include "gui/moderation_gui.h"
#include "inputline_widget.h"
#include "common/replace.h"
#include "models/settings.h"
#include "models/utils.h"
#include "tool_widget.h"

ToolWidget::ToolWidget(QWidget *parent) : QWidget(parent), strCurrentColor("#000000")
{
    showFontButtons = new QToolButton(this);
    showFontButtons->setIcon(QIcon(":/images/draw-brush.svg"));
    showFontButtons->setToolTip(tr("Show/Hide font buttons"));
    showFontButtons->show();

    separator1 = new QFrame(this);
    separator1->setFrameShape(QFrame::VLine);
    separator1->setFrameShadow(QFrame::Sunken);
    separator1->show();

    bold = new QToolButton(this);
    bold->setIcon(QIcon(":/images/breeze/format-text-bold.svg"));
    bold->setToolTip(tr("Bold"));
    bold->setCheckable(true);
    bold->setAutoRaise(true);
    bold->show();
    bMyBold = false;

    italic = new QToolButton(this);
    italic->setIcon(QIcon(":/images/breeze/format-text-italic.svg"));
    italic->setToolTip(tr("Italic"));
    italic->setCheckable(true);
    italic->setAutoRaise(true);
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

    fontfamily = new QToolButton(this);
    fontfamily->setToolTip(tr("Font family"));
    fontfamily->setFont(QFont("Verdana", -1, -1, false));
    fontfamily->setText("Verdana");
    fontfamily->setPopupMode(QToolButton::InstantPopup);
    fontfamily->setMenu(fontMenu);
    fontfamily->setArrowType(Qt::NoArrow);
    fontfamily->show();

    color = new QComboBox(this);
    color->setToolTip(tr("Font color"));
    color->setIconSize(QSize(20,10));

    QList<QString> comboBoxColors = Utils::instance()->getColors();
    foreach (const QString &strColor, comboBoxColors)
    {
        QPixmap pixmap(20,10);
        pixmap.fill(QColor("#"+strColor));
        color->addItem(QIcon(pixmap), QString::null);
    }
    color->show();

    size8Act = new QAction("8", this);
    QFont size8Font(size8Act->font());
    size8Font.setPixelSize(8);
    size8Act->setFont(size8Font);

    size9Act = new QAction("9", this);
    QFont size9Font(size9Act->font());
    size9Font.setPixelSize(9);
    size9Act->setFont(size9Font);

    size10Act = new QAction("10", this);
    QFont size10Font(size10Act->font());
    size10Font.setPixelSize(10);
    size10Act->setFont(size10Font);

    size11Act = new QAction("11", this);
    QFont size11Font(size11Act->font());
    size11Font.setPixelSize(11);
    size11Act->setFont(size11Font);

    size12Act = new QAction("12", this);
    QFont size12Font(size12Act->font());
    size12Font.setPixelSize(12);
    size12Act->setFont(size12Font);

    size14Act = new QAction("14", this);
    QFont size14Font = size14Act->font();
    size14Font.setPixelSize(14);
    size14Act->setFont(size14Font);

    size16Act = new QAction("16", this);
    QFont size16Font(size16Act->font());
    size16Font.setPixelSize(16);
    size16Act->setFont(size16Font);

    size18Act = new QAction("18", this);
    QFont size18Font(size18Act->font());
    size18Font.setPixelSize(18);
    size18Act->setFont(size18Font);

    size20Act = new QAction("20", this);
    QFont size20Font(size20Act->font());
    size20Font.setPixelSize(20);
    size20Act->setFont(size20Font);

    size24Act = new QAction("24", this);
    QFont size24Font(size24Act->font());
    size24Font.setPixelSize(24);
    size24Act->setFont(size24Font);

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

    size = new QToolButton(this);
    size->setIcon(QIcon(":/images/breeze/format-font-size-more.svg"));
    size->setToolTip(tr("Font size"));
    size->setFont(QFont("Times New Roman", -1, -1, false));
    size->setPopupMode(QToolButton::InstantPopup);
    size->setMenu(sizeMenu);
    size->setArrowType(Qt::NoArrow);
    size->show();

    separator2 = new QFrame(this);
    separator2->setFrameShape(QFrame::VLine);
    separator2->setFrameShadow(QFrame::Sunken);
    separator2->show();

    emoticons = new QToolButton(this);
    emoticons->setIcon(QIcon(":/images/smile.svg"));
    emoticons->setToolTip(tr("Emoticons"));
    emoticons->show();

    channel_settings = new QToolButton(this);
    channel_settings->setIcon(QIcon(":/images/settings.svg"));
    channel_settings->setToolTip(tr("Channel settings"));
    channel_settings->show();

    moderation = new QToolButton(this);
    moderation->setIcon(QIcon(":/images/moderation.svg"));
    moderation->setToolTip(tr("Moderation"));
    moderation->show();

    pInputLine = new InputLineWidget(this);
    pInputLine->setMinimumWidth(350);
    pInputLine->setMaximumHeight(25);
    pInputLine->setAcceptRichText(false);
    pInputLine->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    pInputLine->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    pInputLine->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    pInputLine->show();

    sendButton = new QToolButton(this);
    sendButton->setIcon(QIcon(":/images/breeze/key-enter.svg"));
    sendButton->setToolTip(tr("Send"));
    sendButton->show();

    moderSendButton = new QToolButton(this);
    moderSendButton->setIcon(QIcon(":/images/breeze/meeting-observer.svg"));
    moderSendButton->setToolTip(tr("Send to moderators"));
    moderSendButton->show();

    QHBoxLayout *layout = new QHBoxLayout;
    layout->setMargin(0);
    layout->setAlignment(Qt::AlignLeft);
    layout->addWidget(pInputLine);
    layout->addWidget(sendButton);
    layout->addWidget(emoticons);

    layout->addWidget(showFontButtons);
    layout->addWidget(separator1);
    layout->addWidget(bold);
    layout->addWidget(italic);
    layout->addWidget(fontfamily);
    layout->addWidget(color);
    layout->addWidget(size);
    layout->addWidget(separator2);

    layout->addWidget(channel_settings);
    layout->addWidget(moderation);
    layout->addWidget(moderSendButton);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->setMargin(0);
    mainLayout->addLayout(layout);
    this->setLayout(mainLayout);

    // set default values
    setDefaultValues();

    // focus
    setFocusPolicy(Qt::StrongFocus);
    setFocusProxy(pInputLine);

    // signals
    connect(showFontButtons, SIGNAL(clicked()), this, SLOT(showFontButtonsClicked()));

    connect(bold, SIGNAL(clicked()), this, SLOT(boldClicked()));
    connect(italic, SIGNAL(clicked()), this, SLOT(italicClicked()));

    connect(arialAct, SIGNAL(triggered()), this, SLOT(arialTriggered()));
    connect(timesAct, SIGNAL(triggered()), this, SLOT(timesTriggered()));
    connect(verdanaAct, SIGNAL(triggered()), this, SLOT(verdanaTriggered()));
    connect(tahomaAct, SIGNAL(triggered()), this, SLOT(tahomaTriggered()));
    connect(courierAct, SIGNAL(triggered()), this, SLOT(courierTriggered()));

    connect(size8Act, SIGNAL(triggered()), this, SLOT(size8Triggered()));
    connect(size9Act, SIGNAL(triggered()), this, SLOT(size9Triggered()));
    connect(size10Act, SIGNAL(triggered()), this, SLOT(size10Triggered()));
    connect(size11Act, SIGNAL(triggered()), this, SLOT(size11Triggered()));
    connect(size12Act, SIGNAL(triggered()), this, SLOT(size12Triggered()));
    connect(size14Act, SIGNAL(triggered()), this, SLOT(size14Triggered()));
    connect(size16Act, SIGNAL(triggered()), this, SLOT(size16Triggered()));
    connect(size18Act, SIGNAL(triggered()), this, SLOT(size18Triggered()));
    connect(size20Act, SIGNAL(triggered()), this, SLOT(size20Triggered()));
    connect(size24Act, SIGNAL(triggered()), this, SLOT(size24Triggered()));

    connect(color, SIGNAL(activated(int)), this, SLOT(colorClicked(int)));
    connect(emoticons, SIGNAL(clicked()), this, SLOT(emoticonsClicked()));
    connect(channel_settings, SIGNAL(clicked()), this, SLOT(channelSettingsClicked()));
    connect(moderation, SIGNAL(clicked()), this, SLOT(moderationClicked()));

    connect(sendButton, SIGNAL(clicked()), this, SLOT(inputlineReturnPressed()));
    connect(pInputLine, SIGNAL(returnPressed()), this, SLOT(inputlineReturnPressed()));
    connect(moderSendButton, SIGNAL(clicked()), this, SLOT(moderButtonClicked()));

    connect(pInputLine->document()->documentLayout(), SIGNAL(documentSizeChanged(const QSizeF &)), this, SLOT(documentSizeChanged(const QSizeF &)));
}

ToolWidget::~ToolWidget()
{
    disconnect(pInputLine->document()->documentLayout(), SIGNAL(documentSizeChanged(const QSizeF &)), this, SLOT(documentSizeChanged(const QSizeF &)));
    delete pInputLine;
}

void ToolWidget::setDefaultValues()
{
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
    if (Settings::instance()->getBool("my_bold"))
    {
        bold->setChecked(true);
        bMyBold = true;
        strMyFontWeight = "bold";
    }
    else
    {
        bold->setChecked(false);
        bMyBold = false;
        strMyFontWeight = QString::null;
    }

    // set default italic
    if (Settings::instance()->getBool("my_italic"))
    {
        italic->setChecked(true);
        bMyItalic = true;
        strMyFontStyle = "italic";
    }
    else
    {
        italic->setChecked(false);
        bMyItalic = false;
        strMyFontStyle = QString::null;
    }

    // set default font
    QString strMyFont = Settings::instance()->get("my_font");
    fontfamily->setText(strMyFont);
    strMyFontFamily = strMyFont;

    // set default color
    QString strMyColor = Settings::instance()->get("my_color");
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

    // input widget
    QString strFontHeight = Settings::instance()->get("font_size");
    strFontHeight.remove("px");
    int iFontHeight = strFontHeight.toInt();

    pInputLine->setFont(QFont(strMyFontFamily, iFontHeight, (bMyBold ? QFont::Bold : QFont::Normal), bMyItalic));

    // refresh input line text color
    if ((Settings::instance()->get("background_color") == "#000000") && (strCurrentColor == "#000000"))
        pInputLine->setStyleSheet(QString("QTextEdit { color:#ffffff; font-size: %1px;}").arg(strFontHeight));
    else
        pInputLine->setStyleSheet(QString("QTextEdit { color:%1; font-size: %2px;}").arg(strCurrentColor, strFontHeight));

    // moderation default hidden
    showModeration(false);
}

void ToolWidget::clearInputLine()
{
    pInputLine->clear();
}

void ToolWidget::insertTextToInputLine(const QString &strText)
{
    pInputLine->insertText(strText);
}

void ToolWidget::documentSizeChanged(const QSizeF & newSize)
{
    if (newSize.height() < 200)
    {
        pInputLine->setMinimumHeight(newSize.height());
        pInputLine->setMaximumHeight(newSize.height());
        pInputLine->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    }
    else
    {
        pInputLine->setMinimumHeight(200);
        pInputLine->setMaximumHeight(200);
        pInputLine->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    }

    emit forceScrollToBottom();
}

void ToolWidget::showModeration(bool bShow)
{
    if (bShow)
    {
        moderation->show();
        moderSendButton->show();
    }
    else
    {
        moderation->hide();
        moderSendButton->hide();
    }
}

void ToolWidget::showChannelSettings(bool bShow)
{
    if (bShow)
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
        if (channel_settings->isVisible())
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
        if (channel_settings->isVisible())
            separator2->show();
    }
}

void ToolWidget::boldClicked()
{
    if (bMyBold)
    {
        bold->setChecked(false);
        bMyBold = false;
        strMyFontWeight = QString::null;
    }
    else
    {
        bold->setChecked(true);
        bMyBold = true;
        strMyFontWeight = "bold";
    }

    Settings::instance()->setBool("my_bold", bMyBold);

    Config *pConfig = new Config();
    pConfig->set("my_bold", bMyBold ? "true" : "false");
    delete pConfig;

    // refresh font
    QFont fInputLine = pInputLine->font();
    fInputLine.setBold(bMyBold);
    pInputLine->setFont(fInputLine);
}

void ToolWidget::italicClicked()
{
    if (bMyItalic)
    {
        italic->setChecked(false);
        bMyItalic = false;
        strMyFontStyle = QString::null;
    }
    else
    {
        italic->setChecked(true);
        bMyItalic = true;
        strMyFontStyle = "italic";
    }

    Settings::instance()->setBool("my_italic", bMyItalic);

    Config *pConfig = new Config();
    pConfig->set("my_italic", bMyItalic ? "true" : "false");
    delete pConfig;

    // refresh font
    QFont fInputLine = pInputLine->font();
    fInputLine.setItalic(bMyItalic);
    pInputLine->setFont(fInputLine);
}

void ToolWidget::arialTriggered()
{
    fontfamily->setText("Arial");
    strMyFontFamily = "Arial";
    Settings::instance()->set("my_font", strMyFontFamily);
    Config *pConfig = new Config();
    pConfig->set("my_font", strMyFontFamily);
    delete pConfig;

    // refresh font
    QFont fInputLine = pInputLine->font();
    fInputLine.setFamily(strMyFontFamily);
    pInputLine->setFont(fInputLine);
}

void ToolWidget::timesTriggered()
{
    fontfamily->setText("Times");
    strMyFontFamily = "Times";
    Settings::instance()->set("my_font", strMyFontFamily);
    Config *pConfig = new Config();
    pConfig->set("my_font", strMyFontFamily);
    delete pConfig;

    // refresh font
    QFont fInputLine = pInputLine->font();
    fInputLine.setFamily(strMyFontFamily);
    pInputLine->setFont(fInputLine);
}

void ToolWidget::verdanaTriggered()
{
    fontfamily->setText("Verdana");
    strMyFontFamily = "Verdana";
    Settings::instance()->set("my_font", strMyFontFamily);
    Config *pConfig = new Config();
    pConfig->set("my_font", strMyFontFamily);
    delete pConfig;

    // refresh font
    QFont fInputLine = pInputLine->font();
    fInputLine.setFamily(strMyFontFamily);
    pInputLine->setFont(fInputLine);
}

void ToolWidget::tahomaTriggered()
{
    fontfamily->setText("Tahoma");
    strMyFontFamily = "Tahoma";
    Settings::instance()->set("my_font", strMyFontFamily);
    Config *pConfig = new Config();
    pConfig->set("my_font", strMyFontFamily);
    delete pConfig;

    // refresh font
    QFont fInputLine = pInputLine->font();
    fInputLine.setFamily(strMyFontFamily);
    pInputLine->setFont(fInputLine);
}

void ToolWidget::courierTriggered()
{
    fontfamily->setText("Courier");
    strMyFontFamily = "Courier";
    Settings::instance()->set("my_font", strMyFontFamily);
    Config *pConfig = new Config();
    pConfig->set("my_font", strMyFontFamily);
    delete pConfig;

    // refresh font
    QFont fInputLine = pInputLine->font();
    fInputLine.setFamily(strMyFontFamily);
    pInputLine->setFont(fInputLine);
}

void ToolWidget::size8Triggered()
{
    QString strValue = "8px";

    Settings::instance()->set("font_size", strValue);
    Config *pConfig = new Config();
    pConfig->set("font_size", strValue);
    delete pConfig;

    Core::instance()->mainWindow()->refreshCSS();

    // refresh font
    pInputLine->setStyleSheet(QString("QTextEdit { color:%1; font-size: 8px;}").arg(strCurrentColor));
}

void ToolWidget::size9Triggered()
{
    QString strValue = "9px";

    Settings::instance()->set("font_size", strValue);
    Config *pConfig = new Config();
    pConfig->set("font_size", strValue);
    delete pConfig;

    Core::instance()->mainWindow()->refreshCSS();

    // refresh font
    pInputLine->setStyleSheet(QString("QTextEdit { color:%1; font-size: 9px;}").arg(strCurrentColor));
}

void ToolWidget::size10Triggered()
{
    QString strValue = "10px";

    Settings::instance()->set("font_size", strValue);
    Config *pConfig = new Config();
    pConfig->set("font_size", strValue);
    delete pConfig;

    Core::instance()->mainWindow()->refreshCSS();

    // refresh font
    pInputLine->setStyleSheet(QString("QTextEdit { color:%1; font-size: 10px;}").arg(strCurrentColor));
}

void ToolWidget::size11Triggered()
{
    QString strValue = "11px";

    Settings::instance()->set("font_size", strValue);
    Config *pConfig = new Config();
    pConfig->set("font_size", strValue);
    delete pConfig;

    Core::instance()->mainWindow()->refreshCSS();

    // refresh font
    pInputLine->setStyleSheet(QString("QTextEdit { color:%1; font-size: 11px;}").arg(strCurrentColor));
}

void ToolWidget::size12Triggered()
{
    QString strValue = "12px";

    Settings::instance()->set("font_size", strValue);
    Config *pConfig = new Config();
    pConfig->set("font_size", strValue);
    delete pConfig;

    Core::instance()->mainWindow()->refreshCSS();

    // refresh font
    pInputLine->setStyleSheet(QString("QTextEdit { color:%1; font-size: 12px;}").arg(strCurrentColor));
}

void ToolWidget::size14Triggered()
{
    QString strValue = "14px";

    Settings::instance()->set("font_size", strValue);
    Config *pConfig = new Config();
    pConfig->set("font_size", strValue);
    delete pConfig;

    Core::instance()->mainWindow()->refreshCSS();

    // refresh font
    pInputLine->setStyleSheet(QString("QTextEdit { color:%1; font-size: 14px;}").arg(strCurrentColor));
}

void ToolWidget::size16Triggered()
{
    QString strValue = "16px";

    Settings::instance()->set("font_size", strValue);
    Config *pConfig = new Config();
    pConfig->set("font_size", strValue);
    delete pConfig;

    Core::instance()->mainWindow()->refreshCSS();

    // refresh font
    pInputLine->setStyleSheet(QString("QTextEdit { color:%1; font-size: 16px;}").arg(strCurrentColor));
}

void ToolWidget::size18Triggered()
{
    QString strValue = "18px";

    Settings::instance()->set("font_size", strValue);
    Config *pConfig = new Config();
    pConfig->set("font_size", strValue);
    delete pConfig;

    Core::instance()->mainWindow()->refreshCSS();

    // refresh font
    pInputLine->setStyleSheet(QString("QTextEdit { color:%1; font-size: 18px;}").arg(strCurrentColor));
}

void ToolWidget::size20Triggered()
{
    QString strValue = "20px";

    Settings::instance()->set("font_size", strValue);
    Config *pConfig = new Config();
    pConfig->set("font_size", strValue);
    delete pConfig;

    Core::instance()->mainWindow()->refreshCSS();

    // refresh font
    pInputLine->setStyleSheet(QString("QTextEdit { color:%1; font-size: 20px;}").arg(strCurrentColor));
}

void ToolWidget::size24Triggered()
{
    QString strValue = "24px";

    Settings::instance()->set("font_size", strValue);
    Config *pConfig = new Config();
    pConfig->set("font_size", strValue);
    delete pConfig;

    Core::instance()->mainWindow()->refreshCSS();

    // refresh font
    pInputLine->setStyleSheet(QString("QTextEdit { color:%1; font-size: 24px;}").arg(strCurrentColor));
}

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

    Settings::instance()->set("my_color", strCurrentColor);
    Config *pConfig = new Config();
    pConfig->set("my_color", strCurrentColor);
    delete pConfig;

    QString strFontSize = Settings::instance()->get("font_size");

    // refresh input line text color
    if ((Settings::instance()->get("background_color") == "#000000") && (strCurrentColor == "#000000"))
        pInputLine->setStyleSheet(QString("QTextEdit { color:#ffffff; font-size: %1; }").arg(strFontSize));
    else
        pInputLine->setStyleSheet(QString("QTextEdit { color:%1; font-size: %2; }").arg(strCurrentColor, strFontSize));
}

void ToolWidget::emoticonsClicked()
{
    EmoticonsGui(pInputLine).exec();
}

void ToolWidget::channelSettingsClicked()
{
    if (Core::instance()->network->isConnected())
    {
        QString strChannel = Channel::instance()->getCurrentName();

        if (!strChannel.isEmpty())
            ChannelSettingsGui(strChannel).exec();
    }
}

void ToolWidget::moderationClicked()
{
    QString strChannel = Channel::instance()->getCurrentName();

    if (!strChannel.isEmpty())
        ModerationGui(strChannel).exec();
}

void ToolWidget::inputlineReturnPressed()
{
    // update last active
    qint64 iCurrentTime = QDateTime::currentMSecsSinceEpoch();
    Settings::instance()->set("last_active", QString::number(iCurrentTime));

    // disable away
    bool bAway = Settings::instance()->getBool("away");
    if (bAway)
        Core::instance()->network->send("AWAY :");

    // text
    QString strText = pInputLine->toPlainText().trimmed();
    bool bClearLine = pasteMultiLine(strText, false);
    if (bClearLine)
        pInputLine->clear();
}

void ToolWidget::moderButtonClicked()
{
    // update last active
    qint64 iCurrentTime = QDateTime::currentMSecsSinceEpoch();
    Settings::instance()->set("last_active", QString::number(iCurrentTime));

    // disable away
    bool bAway = Settings::instance()->getBool("away");
    if (bAway)
        Core::instance()->network->send("AWAY :");

    // text
    QString strText = pInputLine->toPlainText().trimmed();
    pasteMultiLine(strText, true);
    pInputLine->clear();
}

bool ToolWidget::pasteMultiLine(const QString &strText, bool bModeration)
{
    QStringList list = strText.split(QRegExp("(\n|\r)"));
    int len = 396;
    bool bClearLine = true;

    foreach (QString line, list)
    {
        if (line.size() > len)
        {
            while (line.size() > len)
            {
                // find last space and cut
                int pos = line.lastIndexOf(' ', len);
                if (pos != -1)
                    pos++; // remove space
                else
                    pos = len; // no cut

                QString short_line = line.left(pos);
                bClearLine = sendMessage(short_line, bModeration);
                line.remove(0, pos);
            }
        }
        if ((line.size() < len) && (line.size() != 0))
            bClearLine = sendMessage(line, bModeration);
    }

    return bClearLine;
}

bool ToolWidget::sendMessage(QString strText, bool bModeration)
{
    QString strChannel = Channel::instance()->getCurrentName();

    // empty text or channel
    if ((strText.isEmpty()) || (strChannel.isEmpty()))
        return true;

    QString strMe = Settings::instance()->get("nick");
    QString strCommand;
    QString strTextOriginal;

    // is command
    if ((strText.at(0) == '/') && (strText.at(1) != '/'))
    {
        strText.remove(0,1);
        strTextOriginal = strText;

        QStringList lText = strText.split(" ");
        if (lText.size() != 0)
            strCommand = lText.value(0, QString::null);

        if (!strCommand.isEmpty())
        {
            strCommand = strCommand.toLower();

            Commands *pCommands = new Commands(strChannel, strText);
            strText = pCommands->execute();
            delete pCommands;
        }

        // is empty
        if (strText.isEmpty())
            return true;
    }

    if ((strCommand == "amsg") || (strCommand == "all"))
    {
        Convert::simpleReverseConvert(strText);
        Replace::replaceEmots(strText);
        Convert::createText(strText);

        QList<CaseIgnoreString> lChannelsCleared = Channel::instance()->getListClearedSorted();
        foreach (const QString &strChannel, lChannelsCleared)
        {
            Core::instance()->network->send(QString("PRIVMSG %1 :%2").arg(strChannel, strText));
            Message::instance()->showMessage(strChannel, strText, MessageDefault, strMe);
        }

        return true;
    }
    else if (strCommand == "me")
    {
        if ((strChannel != DEBUG_WINDOW) && (strChannel != STATUS_WINDOW))
        {
            Core::instance()->network->send(strText);

            if (strTextOriginal.length() > 3) strTextOriginal.remove(0,3);

            Convert::simpleReverseConvert(strTextOriginal);
            Replace::replaceEmots(strTextOriginal);
            Convert::createText(strTextOriginal);

            QString strDisplay = QString("%1ACTION %2%3").arg(QString(QByteArray("\x01")), strTextOriginal, QString(QByteArray("\x01")));
            Message::instance()->showMessage(strChannel, strDisplay, MessageMe, strMe);
        }
        return true;
    }
    else if ((strCommand == "mp3") || (strCommand == "winamp"))
    {
        if ((strChannel != DEBUG_WINDOW) && (strChannel != STATUS_WINDOW))
        {
            pInputLine->clear();
            pInputLine->insertPlainText(strText);
        }
        return false;
    }
    else if (strCommand == "raw")
    {
        Core::instance()->network->send(strText);
        return true;
    }
    else if (!strCommand.isEmpty())
    {
        Core::instance()->network->send(strText);
        return true;
    }

    // not status
    if ((strChannel == DEBUG_WINDOW) || (strChannel == STATUS_WINDOW))
        return true;

    // convert
    Convert::simpleReverseConvert(strText);
    Replace::replaceEmots(strText);
    Convert::createText(strText);

    // moder notice
    if (bModeration)
    {
        Core::instance()->network->send(QString("MODERNOTICE %1 :%2").arg(strChannel, strText));
        Message::instance()->showMessage(strChannel, strText, MessageModerNotice, strMe);
    }
    // standard text
    else
    {
        bool bOffline = Channel::instance()->getOffline(strChannel);
        QString strNick = Channel::instance()->getAlternativeName(strChannel);

        if ((bOffline) && (!strNick.isEmpty()))
            Core::instance()->network->send(QString("NS OFFLINE MSG %1 %2").arg(strNick, strText));
        else
            Core::instance()->network->send(QString("PRIVMSG %1 :%2").arg(strChannel, strText));

        Message::instance()->showMessage(strChannel, strText, MessageDefault, strMe);
    }

    return true;
}
