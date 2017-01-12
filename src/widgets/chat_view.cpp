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

#include <QContextMenuEvent>
#include <QDesktopServices>
#include <QFile>
#include <QInputDialog>
#include <QMenu>
#include <QTimer>
#include <QUrl>
#include <QtWebKitWidgets/QWebFrame>
#include <QtWebKit/QWebElement>
#include "models/channel.h"
#include "core/core.h"
#include "models/channel_favourites.h"
#include "gui/channel_settings_gui.h"
#include "gui/find_text_gui.h"
#include "models/friends.h"
#include "common/html_messages_renderer.h"
#include "models/ignore.h"
#include "common/log.h"
#include "core/mainwindow.h"
#include "gui/moderation_gui.h"
#include "models/notes.h"
#include "models/nick.h"
#include "models/punish_reason.h"
#include "models/settings.h"
#include "gui/user_profile_gui.h"
#include "gui/webbrowser_gui.h"
#include "gui/webcam_gui.h"
#include "chat_view.h"

#ifdef Q_OS_WIN
    #include <QApplication>
    #include <QDir>
    #include <QSettings>
    #include "common/kamerzysta.h"
#endif

ChatView::ChatView(const QString &_strChatViewChannel) : strChatViewChannel(_strChatViewChannel), bScrollToBottom(true)
{
    setFocusPolicy(Qt::NoFocus);

    settings()->setAttribute(QWebSettings::JavascriptEnabled, true);
    settings()->setAttribute(QWebSettings::LocalContentCanAccessFileUrls, false);
    settings()->setAttribute(QWebSettings::PrintElementBackgrounds, false);
    //settings()->setAttribute(QWebSettings::NotificationsEnabled, false);
    settings()->setObjectCacheCapacities(0, 0, 0);
    settings()->setMaximumPagesInCache(0);
    settings()->setOfflineWebApplicationCacheQuota(0);
    settings()->clearIconDatabase();
    settings()->clearMemoryCaches();

    connect(this->page()->mainFrame(), SIGNAL(contentsSizeChanged(const QSize &)), this, SLOT(autoScrollToBottom()));

    createBody();
    refreshCSS();
}

ChatView::~ChatView()
{
    disconnect(this->page()->mainFrame(), SIGNAL(contentsSizeChanged(const QSize &)), this, SLOT(autoScrollToBottom()));
}

void ChatView::createBody()
{
    QString path;
#ifdef Q_OS_WIN
    path = QCoreApplication::applicationDirPath();
#else
    path = SCC_DATA_DIR;
#endif

    QString jsCode = "function appendMessage(html) { "
                         "var elements = document.getElementById('Chat').getElementsByTagName('article'); "
                         "if (elements.length > 350) { "
                             "var chatElement = document.getElementById('Chat'); "
                             "chatElement.removeChild(chatElement.firstChild); "
                         "} "
                         "var chatElement = document.getElementById('Chat'); "
                         "var n = document.createElement('article'); "
                         "n.innerHTML = html; "
                         "chatElement.appendChild(n); "
                     "} "

                     "function clearMessages() { "
                         "var chatElement = document.getElementById('Chat'); "
                         "chatElement.innerHTML = ''; "
                     "}"

                    "function imgError(image) {"
                        "image.onerror = '';"
                        //"image.src = 'qrc:/images/not_image.png';"
                        "image.style.display = 'none';"
                        "return true;"
                    "}"

                    "function reloadImage(image_src) {"
                        "var images = document.getElementsByTagName('img');"
                        "for (var i = 0; i < images.length; i++) {"
                            "if (images[i].src == image_src) {"
                                "images[i].style.display = 'block';"
                                "images[i].src = images[i].src;"
                            "}"
                        "}"
                    "}";

    QString strMainHtml = "<html>"
                              "<head>"
                                  "<meta http-equiv=\"content-type\" content=\"text/html; charset=utf-8\" />"
                                  "<style type=\"text/css\"></style>"
                              "</head>"
                              "<body>"
                                  "<main id=\"Chat\"></main>"
                              "</body>"
                          "</html>";
    this->setHtml(strMainHtml);
    this->page()->mainFrame()->evaluateJavaScript(jsCode);
}

void ChatView::reloadCacheImage(const QString &strImage)
{
    QString strFindImageSrc = "file://";
#ifdef Q_OS_WIN
    strFindImageSrc += "/";
#endif

    strFindImageSrc += strImage;

    // convert to encoded url
    strFindImageSrc = QUrl(strFindImageSrc).toEncoded();

    this->page()->mainFrame()->evaluateJavaScript("reloadImage('"+strFindImageSrc+"')");
}

void ChatView::refreshCSS()
{
    QString strHeadCSS = HtmlMessagesRenderer::instance()->headCSS();
    QString strBodyCSS = HtmlMessagesRenderer::instance()->bodyCSS();

    QWebElement head = this->page()->mainFrame()->findFirstElement("head");
    QWebElement headStyle = head.findFirst("style");
    headStyle.setPlainText(strHeadCSS);

    QWebElement body = this->page()->mainFrame()->findFirstElement("body");
    body.setAttribute("style", strBodyCSS);
}

void ChatView::clearMessages()
{
    this->page()->mainFrame()->evaluateJavaScript("clearMessages()");

    bScrollToBottom = true;
}

void ChatView::displayMessage(const QString &strData, MessageCategory eMessageCategory, qint64 iTime, QString strNick)
{
    QString strContent;

    if (strChatViewChannel == DEBUG_WINDOW)
        strContent = HtmlMessagesRenderer::instance()->rendererDebug(strData, iTime);
    else
        strContent = HtmlMessagesRenderer::instance()->renderer(strData, eMessageCategory, iTime, strNick, strChatViewChannel);

    // append
    this->page()->mainFrame()->evaluateJavaScript("appendMessage(\'"+strContent+"\')");

    // html logs
    if (Settings::instance()->get("logs_format") == "html")
        Log::save(strChatViewChannel, strContent, Log::Html);
}

void ChatView::channel()
{
    Core::instance()->mainWindow()->insertTextToInputLine(strChannel+" ");
}

void ChatView::joinChannel()
{
    Core::instance()->network->send(QString("JOIN %1").arg(strChannel));
}

void ChatView::addChannelToFavourites()
{
    Core::instance()->network->send(QString("PRIVMSG NickServ AJOIN ADD %1").arg(strChannel));
}

void ChatView::removeChannelFromFavourites()
{
    Core::instance()->network->send(QString("PRIVMSG NickServ AJOIN DEL %1").arg(strChannel));
}

void ChatView::addCurrentChannelToFavourites()
{
    Core::instance()->network->send(QString("PRIVMSG NickServ AJOIN ADD %1").arg(strChatViewChannel));
}

void ChatView::removeCurrentChannelFromFavourites()
{
    Core::instance()->network->send(QString("PRIVMSG NickServ AJOIN DEL %1").arg(strChatViewChannel));
}

void ChatView::openCurrentChannelModeration()
{
    ModerationGui(strChatViewChannel).exec();
}

void ChatView::openCurrentChannelSettings()
{
    ChannelSettingsGui(strChatViewChannel).exec();
}

void ChatView::nick()
{
    Core::instance()->mainWindow()->insertTextToInputLine(strNick+" ");
}

void ChatView::priv()
{
    Core::instance()->network->send(QString("PRIV %1").arg(strNick));
}

void ChatView::whois()
{
    Core::instance()->network->send(QString("WHOIS %1 :%1").arg(strNick));
}

void ChatView::profile()
{
    UserProfileGui(strNick).exec();
}

void ChatView::cam()
{
    new WebcamGui(strNick, true);
}

void ChatView::kamerzysta()
{
#ifdef Q_OS_WIN
    (new Kamerzysta(Core::instance()->kamerzystaSocket))->show(strNick);
#endif
}

void ChatView::friendsAdd()
{
    Core::instance()->network->send(QString("WATCH +%1").arg(strNick));
}

void ChatView::friendsDel()
{
    Core::instance()->network->send(QString("WATCH -%1").arg(strNick));
}

void ChatView::ignoreAdd()
{
    Core::instance()->network->send(QString("SILENCE +%1!*@*").arg(strNick));
}

void ChatView::ignoreDel()
{
    Core::instance()->network->send(QString("SILENCE -%1!*@*").arg(strNick));
}

void ChatView::kick()
{
    Core::instance()->network->send(QString("KICK %1 %2 :%3").arg(strChatViewChannel, strNick, tr("No reason")));
}

void ChatView::kickWithReason()
{
    bool ok;
    QString strText = QInputDialog::getText(this, tr("Kick From Channel"), tr("Reason for kicking:"), QLineEdit::Normal, tr("No reason"), &ok);
    strText = strText.trimmed();

    if ((ok) && (!strText.isEmpty()))
    {
        Core::instance()->network->send(QString("KICK %1 %2 :%3").arg(strChatViewChannel, strNick, strText));
    }
}

void ChatView::kickWithSelectedReason()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (action)
    {
        QString strPunishReason = action->data().toString();
        Core::instance()->network->send(QString("KICK %1 %2 :%3").arg(strChatViewChannel, strNick, strPunishReason));
    }
}

void ChatView::ban()
{
    Core::instance()->network->send(QString("CS BAN %1 ADD %2").arg(strChatViewChannel, strNick));
}

void ChatView::kbanWithReason()
{
    bool ok;
    QString strText = QInputDialog::getText(this, tr("Kick & Ban"), tr("Reason for kicking:"), QLineEdit::Normal, tr("No reason"), &ok);
    strText = strText.trimmed();

    if ((ok) && (!strText.isEmpty()))
    {
        Core::instance()->network->send(QString("CS BAN %1 ADD %2").arg(strChatViewChannel, strNick));
        Core::instance()->network->send(QString("KICK %1 %2 :%3").arg(strChatViewChannel, strNick, strText));
    }
}

void ChatView::kbanWithSelectedReason()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (action)
    {
        QString strPunishReason = action->data().toString();

        Core::instance()->network->send(QString("CS BAN %1 ADD %2").arg(strChatViewChannel, strNick));
        Core::instance()->network->send(QString("KICK %1 %2 :%3").arg(strChatViewChannel, strNick, strPunishReason));
    }
}

void ChatView::ipban()
{
    Core::instance()->network->send(QString("CS BANIP %1 ADD %2").arg(strChatViewChannel, strNick));
}

void ChatView::opAdd()
{
    Core::instance()->network->send(QString("CS OP %1 ADD %2").arg(strChatViewChannel, strNick));
}

void ChatView::opDel()
{
    Core::instance()->network->send(QString("CS OP %1 DEL %2").arg(strChatViewChannel, strNick));
}

void ChatView::halfopAdd()
{
    Core::instance()->network->send(QString("CS HALFOP %1 ADD %2").arg(strChatViewChannel, strNick));
}

void ChatView::halfopDel()
{
    Core::instance()->network->send(QString("CS HALFOP %1 DEL %2").arg(strChatViewChannel, strNick));
}

void ChatView::moderatorAdd()
{
    Core::instance()->network->send(QString("CS MODERATOR %1 ADD %2").arg(strChatViewChannel, strNick));
}

void ChatView::moderatorDel()
{
    Core::instance()->network->send(QString("CS MODERATOR %1 DEL %2").arg(strChatViewChannel, strNick));
}

void ChatView::voiceAdd()
{
    Core::instance()->network->send(QString("CS VOICE %1 ADD %2").arg(strChatViewChannel, strNick));
}

void ChatView::voiceDel()
{
    Core::instance()->network->send(QString("CS VOICE %1 DEL %2").arg(strChatViewChannel, strNick));
}

void ChatView::invite()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (action)
    {
        QString strInviteChannel = action->data().toString();
        Core::instance()->network->send(QString("INVITE %1 %2").arg(strNick, strInviteChannel));
    }
}

void ChatView::watchVideo()
{
    QString strYoutubeLink;

    QRegExp exYoutube_1("youtube.com/watch\\?.*v=([a-zA-Z0-9_-]{11})");
    QRegExp exYoutube_2("youtube.com/v/([a-zA-Z0-9_-]{11})");
    QRegExp exYoutube_3("youtu.be/([a-zA-Z0-9_-]{11})");

    if (exYoutube_1.indexIn(strWebsite) > -1)
        strYoutubeLink = "http://www.youtube.com/embed/"+exYoutube_1.cap(1);
    if (exYoutube_2.indexIn(strWebsite) > -1)
        strYoutubeLink = "http://www.youtube.com/embed/"+exYoutube_2.cap(1);
    if (exYoutube_3.indexIn(strWebsite) > -1)
        strYoutubeLink = "http://www.youtube.com/embed/"+exYoutube_3.cap(1);

    if (QUrl(strYoutubeLink).isValid())
        (new WebBrowserGui(QUrl(strYoutubeLink)))->show();
}

void ChatView::openWebbrowser()
{
    QDesktopServices::openUrl(QUrl::fromUserInput(strWebsite));
}

void ChatView::sendToNotes()
{
    QString strNotesContent = Notes::instance()->get();
    strNotesContent += this->selectedText()+"\r\n";
    Notes::instance()->set(strNotesContent);
}

void ChatView::search()
{
    FindTextGui(this).exec();
}

void ChatView::clear()
{
    clearMessages();
}

void ChatView::menuChannel(QContextMenuEvent *event)
{
    QMenu menu;
    menu.addAction(QIcon(":/images/breeze/irc-join-channel.svg"), strChannel, this, SLOT(channel()));
    menu.addSeparator();
    menu.addAction(QIcon(":/images/breeze/legalmoves.svg"), tr("Join channel"), this, SLOT(joinChannel()));
//    if (ChannelFavourites::instance()->contains(strChannel))
//    else
        menu.addAction(QIcon(":/images/breeze/emblem-favorite.svg"), tr("Add channel to favourites"), this, SLOT(addChannelToFavourites()));
        menu.addAction(QIcon(":/images/breeze/emblem-favorite.svg"), tr("Remove channel from favourites"), this, SLOT(removeChannelFromFavourites()));

    menu.exec(event->globalPos());
}

void ChatView::menuNick(QContextMenuEvent *event)
{
    QString strMe = Settings::instance()->get("nick");
    QString strSelfModes = Nick::instance()->getModes(strMe, strChatViewChannel);
    int iSelfMaxModes = Nick::instance()->getMaxModes(strMe, strChatViewChannel);
    QString strNickModes = Nick::instance()->getModes(strNick, strChatViewChannel);
    QList<QString> lPunishReasons = PunishReason::instance()->get();

#ifdef Q_OS_WIN
    QSettings winSettings(QSettings::UserScope, "Onet.pl", "InstalledApps");
    winSettings.beginGroup("Kamerzysta");
    QDir dir;
    bool bKamerzystaExists = dir.exists(winSettings.value("DataPath").toString());
#endif

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
    if (Friends::instance()->contains(strNick))
        friends->addAction(QIcon(":/images/breeze/list-remove.svg"), tr("Remove from friends"), this, SLOT(friendsDel()));
    else
        friends->addAction(QIcon(":/images/breeze/list-add.svg"), tr("Add to friends"), this, SLOT(friendsAdd()));

    QMenu *ignore = new QMenu(tr("Ignore list"));
    ignore->setIcon(QIcon(":/images/breeze/meeting-attending-tentative.svg"));
    if (Ignore::instance()->contains(strNick))
        ignore->addAction(QIcon(":/images/breeze/list-remove.svg"), tr("Remove from Ignore list"), this, SLOT(ignoreDel()));
    else
        ignore->addAction(QIcon(":/images/breeze/list-add.svg"), tr("Add to Ignore list"), this, SLOT(ignoreAdd()));

    QMenu *privilege = new QMenu(tr("Actions"));
    privilege->setIcon(QIcon(":/images/breeze/irc-operator.svg"));

    if ((strNickModes.contains(FLAG_OP)) && ((iSelfMaxModes >= FLAG_OWNER_INT) || (strNick == strMe)))
        privilege->addAction(QIcon(":/images/op.png"), tr("Take super operator status"), this, SLOT(opDel()));
    else if ((!strNickModes.contains(FLAG_OP)) && (iSelfMaxModes >= FLAG_OWNER_INT))
        privilege->addAction(QIcon(":/images/op.png"), tr("Give super operator status"), this, SLOT(opAdd()));

    if ((strNickModes.contains(FLAG_HALFOP)) && ((iSelfMaxModes >= FLAG_OP_INT) || (strNick == strMe)))
        privilege->addAction(QIcon(":/images/halfop.png"), tr("Take operator status"), this, SLOT(halfopDel()));
    else if ((!strNickModes.contains(FLAG_HALFOP)) && (iSelfMaxModes >= FLAG_OP_INT))
        privilege->addAction(QIcon(":/images/halfop.png"), tr("Give operator status"), this, SLOT(halfopAdd()));

    if ((strNickModes.contains(FLAG_MOD)) && ((iSelfMaxModes >= FLAG_HALFOP_INT) || (strNick == strMe)))
        privilege->addAction(QIcon(":/images/mod.png"), tr("Take moderator status"), this, SLOT(moderatorDel()));
    else if ((!strNickModes.contains(FLAG_MOD)) && (iSelfMaxModes >= FLAG_HALFOP_INT))
        privilege->addAction(QIcon(":/images/mod.png"), tr("Give moderator status"), this, SLOT(moderatorAdd()));

    if ((strNickModes.contains(FLAG_VOICE)) && ((iSelfMaxModes >= FLAG_HALFOP_INT) || (strNick == strMe)))
        privilege->addAction(QIcon(":/images/voice.png"), tr("Take guest status"), this, SLOT(voiceDel()));
    else if ((!strNickModes.contains(FLAG_VOICE)) && (iSelfMaxModes >= FLAG_HALFOP_INT))
        privilege->addAction(QIcon(":/images/voice.png"), tr("Give guest status"), this, SLOT(voiceAdd()));

    QMenu menu;
    menu.addAction(QIcon(":/images/breeze/user-identity.svg"), strNick, this, SLOT(nick()));
    menu.addSeparator();
    //menu.addAction(QIcon(":/images/breeze/list-add-user.svg"), tr("Priv"), this, SLOT(priv()));
    menu.addAction(QIcon(":/images/breeze/user-properties.svg"), tr("Whois"), this, SLOT(whois()));
//    if (strNick.at(0) != '~')
//    {
//        menu.addAction(QIcon(":/images/breeze/view-pim-contacts.svg"), tr("Profile"), this, SLOT(profile()));
//        if ((strNickModes.contains(FLAG_CAM_PUB)) || (strNickModes.contains(FLAG_CAM_PRIV)))
//        {
//            if (Settings::instance()->get("webcam") == "system")
//            {
//#ifdef Q_OS_WIN
//                if (bKamerzystaExists)
//                    menu.addAction(QIcon(":/images/breeze/camera-web.svg"), tr("Webcam"), this, SLOT(kamerzysta()));
//                else
//                    menu.addAction(QIcon(":/images/breeze/camera-web.svg"), tr("Webcam internal"), this, SLOT(cam()));
//#else
//                menu.addAction(QIcon(":/images/breeze/camera-web.svg"), tr("Webcam internal"), this, SLOT(cam()));
//#endif
//            }
//            else // internal
//            {
//                menu.addAction(QIcon(":/images/breeze/camera-web.svg"), tr("Webcam internal"), this, SLOT(cam()));
//            }
//        }
//    }
    menu.addMenu(mInvite);
//    if (strSelfModes.contains(FLAG_REGISTERED))
//    {
        menu.addMenu(friends);
        menu.addMenu(ignore);
//    }
//    if (iSelfMaxModes >= FLAG_HALFOP_INT)
//    {
//        menu.addSeparator();
//        menu.addAction(QIcon(":/images/breeze/im-kick-user.svg"), tr("Kick From Channel"), this, SLOT(kick()));
//        menu.addMenu(mKick);
//        menu.addAction(QIcon(":/images/breeze/im-ban-user.svg"), tr("Ban From Channel"), this, SLOT(ban()));
//        menu.addMenu(mKickAndBan);
//        menu.addAction(QIcon(":/images/breeze/im-user-busy.svg"), tr("IP Ban"), this, SLOT(ipban()));
//    }
//    if (!privilege->isEmpty())
//    {
//        menu.addSeparator();
//        menu.addMenu(privilege);
//    }

    menu.exec(event->globalPos());
}

void ChatView::menuStandard(QContextMenuEvent *event)
{
    QString strMe = Settings::instance()->get("nick");
    QString strSelfModes = Nick::instance()->getModes(strMe, strChatViewChannel);

    QMenu menu;

    if (!this->selectedText().isEmpty())
    {
        QAction *copy = this->pageAction(QWebPage::Copy);
        copy->setIcon(QIcon(":/images/breeze/edit-copy.svg"));
        menu.addAction(copy);
    }

    QAction *selectAll = this->pageAction(QWebPage::SelectAll);
    selectAll->setIcon(QIcon(":/images/breeze/edit-select-all.svg"));
    menu.addAction(selectAll);

    if (!this->selectedText().isEmpty())
    {
        QAction *sendToNotes = new QAction(QIcon(":/images/breeze/story-editor.svg"), tr("Send to notes"), &menu);
        connect(sendToNotes, SIGNAL(triggered()), this, SLOT(sendToNotes()));
        menu.addAction(sendToNotes);
    }

    menu.addSeparator();

    QAction *search = new QAction(QIcon(":/images/breeze/edit-find.svg"), tr("Find..."), &menu);
    connect(search, SIGNAL(triggered()), this, SLOT(search()));
    menu.addAction(search);

    QAction *clear = new QAction(QIcon(":/images/breeze/draw-eraser.svg"), tr("Clear"), &menu);
    connect(clear, SIGNAL(triggered()), this, SLOT(clear()));
    menu.addAction(clear);

    if (strChatViewChannel.at(0) == '#')
    {
        QMenu *channel = new QMenu(strChatViewChannel, &menu);
        channel->setIcon(QIcon(":/images/breeze/irc-operator.svg"));
//        if (ChannelFavourites::instance()->contains(strChatViewChannel))
//        else
            channel->addAction(QIcon(":/images/breeze/emblem-favorite.svg"), tr("Add channel to favourites"), this, SLOT(addCurrentChannelToFavourites()));
            channel->addAction(QIcon(":/images/breeze/emblem-favorite.svg"), tr("Remove channel from favourites"), this, SLOT(removeCurrentChannelFromFavourites()));
//        if (strSelfModes.contains(FLAG_MOD))
//            channel->addAction(QIcon(":/images/moderation.svg"), tr("Moderation"), this, SLOT(openCurrentChannelModeration()));
//        channel->addAction(QIcon(":/images/settings.svg"), tr("Channel settings"), this, SLOT(openCurrentChannelSettings()));

        menu.addSeparator();
        menu.addMenu(channel);
    }

    menu.exec(event->globalPos());
}

void ChatView::mousePressEvent(QMouseEvent *event)
{
    if (event->button() != Qt::LeftButton)
        return QWebView::mousePressEvent(event);

    QWebHitTestResult r = page()->mainFrame()->hitTestContent(event->pos());

    if (r.linkUrl().isEmpty() == false)
    {
        QString strWord = r.linkUrl().toString();
        QString strCategory = r.linkElement().attribute("name");

        // website
        if (strCategory == "website")
        {
            strWebsite = strWord;
            openWebbrowser();
            return;
        }

        // youtube
        if (strCategory == "youtube")
        {
            strWebsite = strWord;
            watchVideo();
            return;
        }
    }

    QWebView::mousePressEvent(event);
}

void ChatView::menuWebsite(QContextMenuEvent *event)
{
    QString strShortLink = strWebsite;
    if (strShortLink.size() > 40) strShortLink = strShortLink.left(20)+"..."+strShortLink.right(20);

    QMenu menu;

    QAction *websiteLinkAct = new QAction(strShortLink, this);
    websiteLinkAct->setIcon(QIcon(":/images/breeze/preferences-web-browser-shortcuts.svg"));
    websiteLinkAct->setDisabled(true);

    menu.addAction(websiteLinkAct);
    menu.addSeparator();
    menu.addAction(QIcon(":/images/breeze/applications-internet.svg"), tr("Open link"), this, SLOT(openWebbrowser()));

//    QRegExp exYoutube_1("youtube.com/watch\\?.*v=([a-zA-Z0-9_-]{11})");
//    QRegExp exYoutube_2("youtube.com/v/([a-zA-Z0-9_-]{11})");
//    QRegExp exYoutube_3("youtu.be/([a-zA-Z0-9_-]{11})");

//    if ((strWebsite.contains(exYoutube_1)) || (strWebsite.contains(exYoutube_2)) || (strWebsite.contains(exYoutube_3)))
//        menu.addAction(QIcon(":/images/breeze/media-playback-start.svg"), tr("Watch video"), this, SLOT(watchVideo()));

    QAction *copyLinkToClipboard = this->pageAction(QWebPage::CopyLinkToClipboard);
    copyLinkToClipboard->setIcon(QIcon(":/images/breeze/edit-copy.svg"));
    menu.addAction(copyLinkToClipboard);

    menu.exec(event->globalPos());
}

void ChatView::contextMenuEvent(QContextMenuEvent *event)
{
    QWebHitTestResult r = page()->mainFrame()->hitTestContent(event->pos());

    if (r.linkUrl().isEmpty() == false)
    {
        QString strCategory = r.linkElement().attribute("name");

        // website
        if (strCategory == "website")
        {
            strWebsite = r.linkUrl().toString();
            menuWebsite(event);
            return;
        }

        // channel
        if (strCategory == "channel")
        {
            strChannel = r.linkText();
            menuChannel(event);
            return;
        }

        // nick
        if (strCategory == "nick")
        {
            strNick = r.linkText();
            menuNick(event);
            return;
        }
    }

    menuStandard(event);
}

bool ChatView::copySelectedText()
{
    if (page()->hasSelection())
    {
        page()->triggerAction(QWebPage::Copy);
        return true;
    }
    else
        return false;
}
void ChatView::forceScrollToBottom()
{
    bScrollToBottom = true;

    this->scrollToBottom();
}

void ChatView::autoScrollToBottom()
{
    if (bScrollToBottom)
        this->scrollToBottom();
}

void ChatView::scrollToBottom()
{
    page()->mainFrame()->setScrollBarValue(Qt::Vertical, page()->mainFrame()->scrollBarMaximum(Qt::Vertical));
}

void ChatView::resizeEvent(QResizeEvent *event)
{
    QWebView::resizeEvent(event);

    forceScrollToBottom();
}

void ChatView::wheelEvent(QWheelEvent *event)
{
    // event
    QWebView::wheelEvent(event);

    // scroll
    bScrollToBottom = (this->page()->mainFrame()->scrollBarValue(Qt::Vertical) >= this->page()->mainFrame()->scrollBarMaximum(Qt::Vertical));
}
