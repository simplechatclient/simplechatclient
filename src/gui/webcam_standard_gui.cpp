/*
 * Simple Chat Client
 *
 *   Copyright (C) 2009-2017 Piotr Łuczko <piotr.luczko@gmail.com>
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

#include <QPushButton>
#include <QTimer>

#include "core/defines.h"
#include "core/core.h"
#include "widgets/simple_rank_widget.h"
#include "widgets/webcam_delegate.h"
#include "webcam_standard_gui.h"

WebcamStandardGui::WebcamStandardGui()
{
    ui.setupUi(this);
    setWindowFlags(Qt::Dialog | Qt::WindowStaysOnTopHint);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowTitle(tr("Webcams"));

    ui.listWidget_nicks->setItemDelegate(new WebcamDelegate());

    pSimpleRankWidget = new SimpleRankWidget(this);
    pSimpleRankWidget->show();
    ui.horizontalLayout_toolbuttons->insertWidget(2, pSimpleRankWidget);

    createGui();
    createSignals();
}

WebcamStandardGui::~WebcamStandardGui()
{
    delete pSimpleRankWidget;
}

void WebcamStandardGui::createGui()
{
    ui.toolButton_vote_minus->setIcon(QIcon(":/images/breeze/list-minus.svg"));
    ui.toolButton_vote_plus->setIcon(QIcon(":/images/breeze/list-add.svg"));
    ui.buttonBox->button(QDialogButtonBox::Close)->setIcon(QIcon(":/images/breeze/dialog-close.svg"));

    ui.label_list_nick->setText(tr("Nick"));
    ui.label_list_spectators->setText(tr("Spectators"));
    ui.label_list_rank->setText(tr("Rank"));
    ui.buttonBox->button(QDialogButtonBox::Close)->setText(tr("Close"));

    //hide desc
    ui.textEdit_desc->clear();
    ui.textEdit_desc->hide();
}

void WebcamStandardGui::createSignals()
{
    connect(ui.listWidget_nicks, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(changeUser(QListWidgetItem*)));
    connect(ui.toolButton_vote_minus, SIGNAL(clicked()), this, SLOT(voteMinus()));
    connect(ui.toolButton_vote_plus, SIGNAL(clicked()), this, SLOT(votePlus()));
    connect(ui.buttonBox, SIGNAL(rejected()), this, SLOT(buttonClose()));
    connect(ui.textBrowser_channels, SIGNAL(anchorClicked(QUrl)), this, SLOT(channelClicked(QUrl)));
}

void WebcamStandardGui::updateImage(const QByteArray &b)
{
    ui.label_img->clear();
    QPixmap pixmap;
    pixmap.loadFromData(b);
    ui.label_img->setPixmap(pixmap);
}

void WebcamStandardGui::updateText(const QString &s)
{
    ui.label_img->setText(s);
}

void WebcamStandardGui::updateRank(int i)
{
    pSimpleRankWidget->setRank(i);
}

void WebcamStandardGui::userError(const QString &s)
{
    // desc
    ui.textEdit_desc->clear();
    ui.textEdit_desc->hide();
    // img
    ui.label_img->setText(s);
    // rank
    pSimpleRankWidget->setRank(0);
    // channels
    ui.textBrowser_channels->clear();
}

void WebcamStandardGui::updateStatus(const QString &s)
{
    ui.textEdit_desc->setText(s);
    ui.textEdit_desc->show();
}

void WebcamStandardGui::voteMinus()
{
    if (!strCurrentNick.isEmpty())
        emit networkSend(QString("VOTE %1 -").arg(strCurrentNick));
}

void WebcamStandardGui::votePlus()
{
    if (!strCurrentNick.isEmpty())
        emit networkSend(QString("VOTE %1 +").arg(strCurrentNick));
}

void WebcamStandardGui::voteOk()
{
    ui.toolButton_vote_minus->setEnabled(false);
    ui.toolButton_vote_plus->setEnabled(false);

    QTimer::singleShot(1000*3, this, SLOT(enableVote())); // 3 sec
}

void WebcamStandardGui::enableVote()
{
    ui.toolButton_vote_minus->setEnabled(true);
    ui.toolButton_vote_plus->setEnabled(true);
}

void WebcamStandardGui::error(const QString &s)
{
    updateText(s + "<br/>"+tr("Disconnected from server webcams"));

    // clear
    ui.listWidget_nicks->clear();
    ui.textEdit_desc->clear();
    ui.textEdit_desc->hide();
    pSimpleRankWidget->setRank(0);
    ui.textBrowser_channels->clear();
}

void WebcamStandardGui::updateUser(const QString &strNick, int iSpectators, int iRank, int iCamOnOff, const QString &strUdget, const QStringList &lUserChannels)
{
    // cam off or private cam
    if ((iCamOnOff == 0) || (lUserChannels.isEmpty()))
        removeUser(strNick);
    else
    {
        if (!existUser(strNick))
            addUser(strNick, iSpectators, iRank, strUdget, lUserChannels);
        else
            updateUser(strNick, iSpectators, iRank, strUdget, lUserChannels);
    }
}

void WebcamStandardGui::updateUserCount(const QString &strNick, int iSpectators, int iRank)
{
    QList<QListWidgetItem*> items = ui.listWidget_nicks->findItems(strNick, Qt::MatchExactly);
    foreach (QListWidgetItem *item, items)
    {
        item->setData(WebCamSpectatorsRole, iSpectators);
        item->setData(WebCamRankRole, iRank);
    }

    if (strNick == strCurrentNick)
        updateRank(iRank);
}

void WebcamStandardGui::addUser(const QString &strNick, int iSpectators, int iRank, const QString &strUdget, const QStringList &lUserChannels)
{
    SortedListWidgetItem *item = new SortedListWidgetItem();
    item->setText(strNick);
    item->setData(WebCamSpectatorsRole, iSpectators);
    item->setData(WebCamRankRole, iRank);
    item->setData(WebCamUdgetRole, strUdget);
    item->setData(WebCamUserChannelsRole, lUserChannels);

    item->setData(SortedListWidgetNicklistRole, false); // is nicklist
    ui.listWidget_nicks->addItem(item);
}

void WebcamStandardGui::updateUser(const QString &strNick, int iSpectators, int iRank, const QString &strUdget, const QStringList &lUserChannels)
{
    QList<QListWidgetItem*> items = ui.listWidget_nicks->findItems(strNick, Qt::MatchExactly);
    foreach (QListWidgetItem *item, items)
    {
        item->setData(WebCamSpectatorsRole, iSpectators);
        item->setData(WebCamRankRole, iRank);
        item->setData(WebCamUdgetRole, strUdget);
        item->setData(WebCamUserChannelsRole, lUserChannels);
    }

    if (strNick == strCurrentNick)
        updateRank(iRank);
}

void WebcamStandardGui::removeUser(const QString &strNick)
{
    QList<QListWidgetItem*> items = ui.listWidget_nicks->findItems(strNick, Qt::MatchExactly);
    foreach (QListWidgetItem *item, items)
        ui.listWidget_nicks->takeItem(ui.listWidget_nicks->row(item));
}

void WebcamStandardGui::clearUsers()
{
    ui.listWidget_nicks->clear();
}

bool WebcamStandardGui::existUser(const QString &strNick)
{
    QList<QListWidgetItem*> items = ui.listWidget_nicks->findItems(strNick, Qt::MatchCaseSensitive);

    if (items.size() != 0)
        return true;
    else
        return false;
}

void WebcamStandardGui::changeUser(QListWidgetItem *item)
{
    // img
    ui.label_img->setText(tr("Downloading image"));

    // clear desc
    ui.textEdit_desc->clear();
    ui.textEdit_desc->hide();

    // clear rank
    pSimpleRankWidget->setRank(0);

    // read nick
    QString strNewNick = item->text();
    QStringList lNewNickChannels = item->data(WebCamUserChannelsRole).toStringList();

    // change user
    if (strCurrentNick.isEmpty())
        emit networkSend(QString("SUBSCRIBE_BIG * %1").arg(strNewNick));
    else
        emit networkSend(QString("UNSUBSCRIBE_BIG %1").arg(strCurrentNick));

    // set nick
    strCurrentNick = strNewNick;

    // display nick
    ui.label_nick->setText(strCurrentNick);

    // set nick
    emit setUser(strCurrentNick);

    // update channels
    QString strNewNickChannels;
    foreach (QString strNewNickChannel, lNewNickChannels)
        strNewNickChannels += QString("<a href=\"%1\" class=\"ChannelColor\">%2</a> ").arg(strNewNickChannel, strNewNickChannel);

    ui.textBrowser_channels->setText(QString("<b>%1:</b><br/>%2").arg(tr("Is on channels"), strNewNickChannels));
}

void WebcamStandardGui::buttonClose()
{
    this->close();
}

void WebcamStandardGui::closeEvent(QCloseEvent *)
{
    emit closeCam();
}

void WebcamStandardGui::channelClicked(QUrl url)
{
    Core::instance()->network->send(QString("JOIN %1").arg(url.toString()));
}
