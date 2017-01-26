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

#include <QContextMenuEvent>
#include <QInputDialog>
#include <QListWidgetItem>
#include <QMenu>
#include "models/channel.h"
#include "core/core.h"
#include "core/defines.h"
#include "models/friends.h"
#include "models/ignore.h"
#include "core/mainwindow.h"
#include "models/nick.h"
#include "models/punish_reason.h"
#include "models/settings.h"
#include "gui/user_profile_gui.h"
#include "nicklist_widget.h"

NickListWidget::NickListWidget(const QString &_strChannel) : strChannel(_strChannel), strSelectedNick(QString::null)
{
    setSortingEnabled(true);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

void NickListWidget::addUser(const QString &strNick, const QString &strModes, int iMaxModes, const QString &strAvatar, QChar &cSex)
{
    if (existUser(strNick)) return;

    SortedListWidgetItem *item = new SortedListWidgetItem();
    item->setText(strNick);
    item->setData(SortedListWidgetNicklistRole, true); // is nicklist
    item->setData(SortedListWidgetStatusRole, iMaxModes); // max modes
    item->setData(NickListModesRole, strModes); // modes
    item->setData(NickListAvatarUrlRole, strAvatar); // avatar url
    item->setData(NickListSexRole, cSex); // sex
    item->setData(NickListChannelRole, strChannel); // channel

    this->addItem(item);
}

void NickListWidget::delUser(const QString &strNick)
{
    QList<QListWidgetItem*> items = this->findItems(strNick, Qt::MatchExactly);
    foreach (QListWidgetItem *item, items)
        this->takeItem(this->row(item));
}

void NickListWidget::renameUser(const QString &strNick, const QString &strNewNick)
{
    QList<QListWidgetItem*> items = this->findItems(strNick, Qt::MatchExactly);
    foreach (QListWidgetItem *item, items)
        item->setText(strNewNick);
}

void NickListWidget::setUserAvatar(const QString &strNick, const QString &strValue)
{
    QList<QListWidgetItem*> items = this->findItems(strNick, Qt::MatchExactly);
    foreach (QListWidgetItem *item, items)
        item->setData(NickListAvatarUrlRole, strValue);
}

void NickListWidget::setUserSex(const QString &strNick, const QChar &cValue)
{
    QList<QListWidgetItem*> items = this->findItems(strNick, Qt::MatchExactly);
    foreach (QListWidgetItem *item, items)
        item->setData(NickListSexRole, cValue);
}

void NickListWidget::setUserModes(const QString &strNick, const QString &strModes, int iMaxModes)
{
    QList<QListWidgetItem*> items = this->findItems(strNick, Qt::MatchExactly);
    foreach (QListWidgetItem *item, items)
    {
        item->setData(SortedListWidgetStatusRole, iMaxModes);
        item->setData(NickListModesRole, strModes);
    }
}

bool NickListWidget::existUser(const QString &strNick)
{
    if (this->findItems(strNick, Qt::MatchExactly).isEmpty())
        return false;
    else
        return true;
}

void NickListWidget::nick()
{
    if (strSelectedNick.isEmpty()) return;

    Core::instance()->mainWindow()->insertTextToInputLine(strSelectedNick+" ");
}

void NickListWidget::priv()
{
    if (strSelectedNick.isEmpty()) return;

    Core::instance()->mainWindow()->tabAddRequested(strSelectedNick);
}

void NickListWidget::whois()
{
    if (strSelectedNick.isEmpty()) return;

    Core::instance()->network->send(QString("WHOIS %1 :%1").arg(strSelectedNick));
}

void NickListWidget::profile()
{
    if (strSelectedNick.isEmpty()) return;

    UserProfileGui(strSelectedNick).exec();
}

void NickListWidget::cam()
{
    if (strSelectedNick.isEmpty()) return;

    // TODO cams
}

void NickListWidget::kamerzysta()
{
    if (strSelectedNick.isEmpty()) return;

    // TODO cams
}

void NickListWidget::friendsAdd()
{
    if (strSelectedNick.isEmpty()) return;

    Core::instance()->network->send(QString("WATCH +%1").arg(strSelectedNick));
}

void NickListWidget::friendsDel()
{
    if (strSelectedNick.isEmpty()) return;

    Core::instance()->network->send(QString("WATCH -%1").arg(strSelectedNick));
}

void NickListWidget::ignoreAdd()
{
    if (strSelectedNick.isEmpty()) return;

    Core::instance()->network->send(QString("SILENCE +%1!*@*").arg(strSelectedNick));
}

void NickListWidget::ignoreDel()
{
    if (strSelectedNick.isEmpty()) return;

    Core::instance()->network->send(QString("SILENCE -%1!*@*").arg(strSelectedNick));
}

void NickListWidget::kick()
{
    if (strSelectedNick.isEmpty()) return;

    Core::instance()->network->send(QString("KICK %1 %2 :%3").arg(strChannel, strSelectedNick, tr("No reason")));
}

void NickListWidget::kickWithReason()
{
    if (strSelectedNick.isEmpty()) return;

    bool ok;
    QString strText = QInputDialog::getText(this, tr("Kick From Channel"), tr("Reason for kicking:"), QLineEdit::Normal, tr("No reason"), &ok);
    strText = strText.trimmed();

    if ((ok) && (!strText.isEmpty()))
    {
        Core::instance()->network->send(QString("KICK %1 %2 :%3").arg(strChannel, strSelectedNick, strText));
    }
}

void NickListWidget::kickWithSelectedReason()
{
    if (strSelectedNick.isEmpty()) return;

    QAction *action = qobject_cast<QAction *>(sender());
    if (action)
    {
        QString strPunishReason = action->data().toString();
        Core::instance()->network->send(QString("KICK %1 %2 :%3").arg(strChannel, strSelectedNick, strPunishReason));
    }
}

void NickListWidget::ban()
{
    if (strSelectedNick.isEmpty()) return;

    Core::instance()->network->send(QString("CS BAN %1 ADD %2").arg(strChannel, strSelectedNick));
}

void NickListWidget::kbanWithReason()
{
    if (strSelectedNick.isEmpty()) return;

    bool ok;
    QString strText = QInputDialog::getText(this, tr("Kick & Ban"), tr("Reason for kicking:"), QLineEdit::Normal, tr("No reason"), &ok);
    strText = strText.trimmed();

    if ((ok) && (!strText.isEmpty()))
    {
        Core::instance()->network->send(QString("CS BAN %1 ADD %2").arg(strChannel, strSelectedNick));
        Core::instance()->network->send(QString("KICK %1 %2 :%3").arg(strChannel, strSelectedNick, strText));
    }
}

void NickListWidget::kbanWithSelectedReason()
{
    if (strSelectedNick.isEmpty()) return;

    QAction *action = qobject_cast<QAction *>(sender());
    if (action)
    {
        QString strPunishReason = action->data().toString();

        Core::instance()->network->send(QString("CS BAN %1 ADD %2").arg(strChannel, strSelectedNick));
        Core::instance()->network->send(QString("KICK %1 %2 :%3").arg(strChannel, strSelectedNick, strPunishReason));
    }
}

void NickListWidget::ipban()
{
    if (strSelectedNick.isEmpty()) return;

    Core::instance()->network->send(QString("CS BANIP %1 ADD %2").arg(strChannel, strSelectedNick));
}

void NickListWidget::opAdd()
{
    if (strSelectedNick.isEmpty()) return;

    Core::instance()->network->send(QString("CS OP %1 ADD %2").arg(strChannel, strSelectedNick));
}

void NickListWidget::opDel()
{
    if (strSelectedNick.isEmpty()) return;

    Core::instance()->network->send(QString("CS OP %1 DEL %2").arg(strChannel, strSelectedNick));
}

void NickListWidget::halfopAdd()
{
    if (strSelectedNick.isEmpty()) return;

    Core::instance()->network->send(QString("CS HALFOP %1 ADD %2").arg(strChannel, strSelectedNick));
}

void NickListWidget::halfopDel()
{
    if (strSelectedNick.isEmpty()) return;

    Core::instance()->network->send(QString("CS HALFOP %1 DEL %2").arg(strChannel, strSelectedNick));
}

void NickListWidget::moderatorAdd()
{
    if (strSelectedNick.isEmpty()) return;

    Core::instance()->network->send(QString("CS MODERATOR %1 ADD %2").arg(strChannel, strSelectedNick));
}

void NickListWidget::moderatorDel()
{
    if (strSelectedNick.isEmpty()) return;

    Core::instance()->network->send(QString("CS MODERATOR %1 DEL %2").arg(strChannel, strSelectedNick));
}

void NickListWidget::voiceAdd()
{
    if (strSelectedNick.isEmpty()) return;

    Core::instance()->network->send(QString("CS VOICE %1 ADD %2").arg(strChannel, strSelectedNick));
}

void NickListWidget::voiceDel()
{
    if (strSelectedNick.isEmpty()) return;

    Core::instance()->network->send(QString("CS VOICE %1 DEL %2").arg(strChannel, strSelectedNick));
}

void NickListWidget::invite()
{
    if (strSelectedNick.isEmpty()) return;

    QAction *action = qobject_cast<QAction *>(sender());
    if (action)
    {
        QString strInviteChannel = action->data().toString();
        Core::instance()->network->send(QString("INVITE %1 %2").arg(strSelectedNick, strInviteChannel));
    }
}

void NickListWidget::contextMenuEvent(QContextMenuEvent *e)
{
    if (this->selectedItems().size() == 0) return;

    strSelectedNick = this->selectedItems().at(0)->text();

    QString strMe = Settings::instance()->get("nick");
    QString strSelfModes = Nick::instance()->getModes(strMe, strChannel);
    int iSelfMaxModes = Nick::instance()->getMaxModes(strMe, strChannel);
    QString strNickModes = Nick::instance()->getModes(strSelectedNick, strChannel);
    QList<QString> lPunishReasons = PunishReason::instance()->get();

    QMenu *mInvite = new QMenu(tr("Invite"));
    mInvite->setIcon(QIcon(":/images/breeze/legalmoves.svg"));
    QList<CaseIgnoreString> lChannelsCleared = Channel::instance()->getListClearedSorted();
    for (int i = 0; i < lChannelsCleared.size(); ++i)
    {
        QString strOpenChannel = lChannelsCleared.at(i);
        if (strOpenChannel.at(0) == '^')
            strOpenChannel = Channel::instance()->getAlternativeName(strOpenChannel);

        openChannelsActs[i] = new QAction(this);
        openChannelsActs[i]->setIcon(QIcon(":/images/breeze/irc-join-channel.svg"));
        openChannelsActs[i]->setVisible(false);
        openChannelsActs[i]->setText(strOpenChannel);
        openChannelsActs[i]->setData(lChannelsCleared.at(i));
        openChannelsActs[i]->setVisible(true);

        connect(openChannelsActs[i], SIGNAL(triggered()), this, SLOT(invite()));
        mInvite->addAction(openChannelsActs[i]);
    }

    QMenu *mKick = new QMenu(tr("Kick From Channel"));
    mKick->setIcon(QIcon(":/images/breeze/im-kick-user.svg"));
    mKick->addAction(QIcon(":/images/breeze/view-conversation-balloon.svg"), tr("(reason)"), this, SLOT(kickWithReason()));
    if (!lPunishReasons.isEmpty()) mKick->addSeparator();
    for (int i = 0; i < lPunishReasons.size(); ++i)
    {
        QString strPunishReasons = lPunishReasons.at(i);

        kickReasonAct[i] = new QAction(this);
        kickReasonAct[i]->setIcon(QIcon(":/images/breeze/view-conversation-balloon.svg"));
        kickReasonAct[i]->setVisible(false);
        kickReasonAct[i]->setText(strPunishReasons);
        kickReasonAct[i]->setData(lPunishReasons.at(i));
        kickReasonAct[i]->setVisible(true);

        connect(kickReasonAct[i], SIGNAL(triggered()), this, SLOT(kickWithSelectedReason()));
        mKick->addAction(kickReasonAct[i]);
    }

    QMenu *mKickAndBan = new QMenu(tr("Kick & Ban"));
    mKickAndBan->setIcon(QIcon(":/images/breeze/im-ban-kick-user.svg"));
    mKickAndBan->addAction(QIcon(":/images/breeze/view-conversation-balloon.svg"), tr("(reason)"), this, SLOT(kbanWithReason()));
    if (!lPunishReasons.isEmpty()) mKickAndBan->addSeparator();
    for (int i = 0; i < lPunishReasons.size(); ++i)
    {
        QString strPunishReasons = lPunishReasons.at(i);

        kbanReasonAct[i] = new QAction(this);
        kbanReasonAct[i]->setIcon(QIcon(":/images/breeze/view-conversation-balloon.svg"));
        kbanReasonAct[i]->setVisible(false);
        kbanReasonAct[i]->setText(strPunishReasons);
        kbanReasonAct[i]->setData(lPunishReasons.at(i));
        kbanReasonAct[i]->setVisible(true);

        connect(kbanReasonAct[i], SIGNAL(triggered()), this, SLOT(kbanWithSelectedReason()));
        mKickAndBan->addAction(kbanReasonAct[i]);
    }

    QMenu *friends = new QMenu(tr("Friends list"));
    friends->setIcon(QIcon(":/images/breeze/meeting-attending.svg"));
    if (Friends::instance()->contains(strSelectedNick))
        friends->addAction(QIcon(":/images/breeze/list-remove.svg"), tr("Remove from friends"), this, SLOT(friendsDel()));
    else
        friends->addAction(QIcon(":/images/breeze/list-add.svg"), tr("Add to friends"), this, SLOT(friendsAdd()));

    QMenu *ignore = new QMenu(tr("Ignore list"));
    ignore->setIcon(QIcon(":/images/breeze/meeting-attending-tentative.svg"));
    if (Ignore::instance()->contains(strSelectedNick))
        ignore->addAction(QIcon(":/images/breeze/list-remove.svg"), tr("Remove from Ignore list"), this, SLOT(ignoreDel()));
    else
        ignore->addAction(QIcon(":/images/breeze/list-add.svg"), tr("Add to Ignore list"), this, SLOT(ignoreAdd()));

    QMenu *privilege = new QMenu(tr("Actions"));
    privilege->setIcon(QIcon(":/images/breeze/irc-operator.svg"));

    if ((strNickModes.contains(FLAG_OP)) && ((iSelfMaxModes >= FLAG_OWNER_INT) || (strSelectedNick == strMe)))
        privilege->addAction(QIcon(":/images/op.png"), tr("Take super operator status"), this, SLOT(opDel()));
    else if ((!strNickModes.contains(FLAG_OP)) && (iSelfMaxModes >= FLAG_OWNER_INT))
        privilege->addAction(QIcon(":/images/op.png"), tr("Give super operator status"), this, SLOT(opAdd()));

    if ((strNickModes.contains(FLAG_HALFOP)) && ((iSelfMaxModes >= FLAG_OP_INT) || (strSelectedNick == strMe)))
        privilege->addAction(QIcon(":/images/halfop.png"), tr("Take operator status"), this, SLOT(halfopDel()));
    else if ((!strNickModes.contains(FLAG_HALFOP)) && (iSelfMaxModes >= FLAG_OP_INT))
        privilege->addAction(QIcon(":/images/halfop.png"), tr("Give operator status"), this, SLOT(halfopAdd()));

    if ((strNickModes.contains(FLAG_MOD)) && ((iSelfMaxModes >= FLAG_HALFOP_INT) || (strSelectedNick == strMe)))
        privilege->addAction(QIcon(":/images/mod.png"), tr("Take moderator status"), this, SLOT(moderatorDel()));
    else if ((!strNickModes.contains(FLAG_MOD)) && (iSelfMaxModes >= FLAG_HALFOP_INT))
        privilege->addAction(QIcon(":/images/mod.png"), tr("Give moderator status"), this, SLOT(moderatorAdd()));

    if ((strNickModes.contains(FLAG_VOICE)) && ((iSelfMaxModes >= FLAG_HALFOP_INT) || (strSelectedNick == strMe)))
        privilege->addAction(QIcon(":/images/voice.png"), tr("Take guest status"), this, SLOT(voiceDel()));
    else if ((!strNickModes.contains(FLAG_VOICE)) && (iSelfMaxModes >= FLAG_HALFOP_INT))
        privilege->addAction(QIcon(":/images/voice.png"), tr("Give guest status"), this, SLOT(voiceAdd()));

    QMenu *menu = new QMenu(strSelectedNick);
    menu->addAction(QIcon(":/images/breeze/user-identity.svg"), strSelectedNick, this, SLOT(nick()));
    menu->addSeparator();
    menu->addAction(QIcon(":/images/breeze/list-add-user.svg"), tr("Priv"), this, SLOT(priv()));
    menu->addAction(QIcon(":/images/breeze/user-properties.svg"), tr("Whois"), this, SLOT(whois()));
//    if (strSelectedNick.at(0) != '~')
//    {
//        menu->addAction(QIcon(":/images/breeze/view-pim-contacts.svg"), tr("Profile"), this, SLOT(profile()));
//    }
    menu->addMenu(mInvite);
//    if (strSelfModes.contains(FLAG_REGISTERED))
//    {
        menu->addMenu(friends);
        menu->addMenu(ignore);
//    }
//    if (iSelfMaxModes >= FLAG_HALFOP_INT)
//    {
//        menu->addSeparator();
//        menu->addAction(QIcon(":/images/breeze/im-kick-user.svg"), tr("Kick From Channel"), this, SLOT(kick()));
//        menu->addMenu(mKick);
//        menu->addAction(QIcon(":/images/breeze/im-ban-user.svg"), tr("Ban From Channel"), this, SLOT(ban()));
//        menu->addMenu(mKickAndBan);
//        menu->addAction(QIcon(":/images/breeze/im-user-busy.svg"), tr("IP Ban"), this, SLOT(ipban()));
//    }
//    if (!privilege->isEmpty())
//    {
//        menu->addSeparator();
//        menu->addMenu(privilege);
//    }

    menu->popup(e->globalPos());
}
