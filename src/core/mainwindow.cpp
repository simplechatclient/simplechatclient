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

#include <QDesktopWidget>
#include <QMenuBar>
#include <QMessageBox>
#include <QSystemTrayIcon>
#include <QTimer>
#include <QToolBar>
#include <QToolButton>

#include "gui/about_gui.h"
#include "models/autoaway.h"
#include "models/away.h"
#include "models/awaylog.h"
#include "gui/awaylog_gui.h"
#include "models/busy.h"
#include "models/channel.h"
#include "gui/channel_favourites_gui.h"
#include "gui/channel_homes_gui.h"
#include "gui/channel_list_gui.h"
#include "common/config.h"
#include "core.h"
#include "gui/find_nick_gui.h"
#include "gui/friends_gui.h"
#include "gui/ignore_gui.h"
#include "gui/invite_list_gui.h"
#include "models/invite.h"
#include "models/lag.h"
#include "models/message.h"
#include "gui/my_avatar_gui.h"
#include "gui/my_profile_gui.h"
#include "gui/my_stats_gui.h"
#include "models/nick.h"
#include "gui/notes_gui.h"
#include "models/notification.h"
#include "models/offline.h"
#include "gui/offline_list_gui.h"
#include "gui/options_gui.h"
#include "onet/onet_auth.h"
#include "onet/onet_kernel.h"
#include "models/settings.h"
#include "tab/tab_container.h"
#include "tab/tab_manager.h"
#include "models/update.h"
#include "gui/update_gui.h"
#include "gui/webcam_gui.h"
#include "widgets/tool_widget.h"

#ifdef Q_OS_WIN
    #include "common/kamerzysta.h"
#endif

#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    // init
    setWindowTitle("Simple Chat Client");
    setWindowIcon(QIcon(":/images/logo32x32.png"));

    setWindowGeometry();
    createActions();
    createMenus();

    // classes
    pTabM = new TabManager(this);
    pTabC = new TabContainer(pTabM);

    pOnetKernel = new OnetKernel(pTabC);
    pOnetAuth = new OnetAuth();

    // gui
    createGui();

    // refresh colors
    refreshColors();

    // create signals
    createSignals();
}

MainWindow::~MainWindow()
{
    delete pToolWidget;

    Autoaway::instance()->stop();

    delete pOnetAuth;
    delete pOnetKernel;

    QObject::disconnect(pTabM, SIGNAL(currentChanged(int)), this, SLOT(currentTabChanged(int)));
    delete pTabC;
    delete pTabM;

    // hide tray
    trayIcon->hide();

    // delete teray
    delete trayIcon;
    delete trayIconMenu;
}

void MainWindow::setWindowGeometry()
{
    const int x = QApplication::desktop()->availableGeometry().width();
    const int y = QApplication::desktop()->availableGeometry().height();
    const int xo = 800;
    const int yo = 600;

    this->resize(xo,yo);
    this->setGeometry((x-xo)/2,(y-yo)/2,xo,yo);
}

void MainWindow::createGui()
{
    pToolWidget = new ToolWidget(this);
    pToolWidget->show();

    QVBoxLayout *layout = new QVBoxLayout;
    layout->setMargin(0);
    layout->addWidget(pTabM);
    layout->addWidget(pToolWidget);

    QWidget *widget = new QWidget;
    widget->setLayout(layout);
    this->setCentralWidget(widget);
}

void MainWindow::createActions()
{
    // action
    trayAction = new QAction(QIcon(":/images/logo16x16.png"), "Simple Chat Client", this);
    trayAction->setFont(QFont(this->font().family(), -1, 75, false));
    trayAction->setEnabled(false);

    connectAction = new QAction(QIcon(":/images/breeze/network-connect.svg"), tr("&Connect"), this);
    optionsAction = new QAction(QIcon(":/images/options.svg"), tr("Options"), this);

    // onet action
    channelListAction = new QAction(QIcon(":/images/breeze/documentation.svg"), tr("Channel list"), this);
    channelHomesAction = new QAction(QIcon(":/images/breeze/view-group.svg"), tr("My channels"), this);
    channelFavouritesAction = new QAction(QIcon(":/images/breeze/emblem-favorite.svg"), tr("Favorite channels"), this);
    friendsAction = new QAction(QIcon(":/images/breeze/meeting-attending.svg"), tr("Friends"), this);
    ignoreAction = new QAction(QIcon(":/images/breeze/meeting-attending-tentative.svg"), tr("Ignored"), this);
    findNickAction = new QAction(QIcon(":/images/breeze/edit-find-user.svg"), tr("Find nick"), this);

    myStatsAction = new QAction(QIcon(":/images/breeze/office-chart-bar.svg"),tr("My statistics"), this);
    myProfileAction = new QAction(QIcon(":/images/breeze/view-pim-contacts.svg"),tr("My profile"), this);
    myAvatarAction = new QAction(QIcon(":/images/breeze/edit-image-face-show.svg"),tr("My avatar"), this);

    camsAction = new QAction(QIcon(":/images/breeze/camera-web.svg"),tr("Webcams"), this);
    notesAction = new QAction(QIcon(":/images/breeze/story-editor.svg"), tr("Notes"), this);
    aboutAction = new QAction(QIcon(":/images/breeze/help-hint.svg"), tr("About SCC ..."), this);

    minimizeAction = new QAction(QIcon(":/images/breeze/view-close.svg"), tr("Mi&nimize"), this);
    maximizeAction = new QAction(QIcon(":/images/breeze/view-fullscreen.svg"), tr("Ma&ximize"), this);
    restoreAction = new QAction(QIcon(":/images/breeze/view-restore.svg"), tr("&Restore"), this);
    quitAction = new QAction(QIcon(":/images/breeze/application-exit.svg"), tr("&Quit"), this);

    // shortcut
    connectAction->setShortcuts(QKeySequence::New);
    quitAction->setShortcut(tr("Ctrl+Q"));
    optionsAction->setShortcut(tr("Ctrl+O"));

    // onet shortcut
    channelListAction->setShortcut(tr("Ctrl+L"));
    channelHomesAction->setShortcut(tr("Ctrl+M"));
    channelFavouritesAction->setShortcut(tr("Ctrl+U"));
    friendsAction->setShortcut(tr("Ctrl+P"));
    ignoreAction->setShortcut(tr("Ctrl+I"));
    findNickAction->setShortcut(tr("Ctrl+G"));
    camsAction->setShortcut(tr("Ctrl+K"));
}

void MainWindow::createMenus()
{
    // chat
    chatMenu = new QMenu(tr("&Chat"));
    chatMenu->setIcon(QIcon(":/images/breeze/meeting-attending.svg"));
    chatMenu->addAction(channelListAction);
    chatMenu->addAction(findNickAction);
    chatMenu->addAction(camsAction);
    chatMenu->addSeparator();
    chatMenu->addAction(Lag::instance()->lagAction);

    // my
    myMenu = new QMenu(tr("&My"));
    myMenu->setIcon(QIcon(":/images/breeze/user-identity.svg"));
    myMenu->addAction(myStatsAction);
    myMenu->addAction(myProfileAction);
    myMenu->addAction(myAvatarAction);
    myMenu->addAction(channelHomesAction);
    myMenu->addAction(channelFavouritesAction);
    myMenu->addAction(friendsAction);
    myMenu->addAction(ignoreAction);
    myMenu->addAction(notesAction);
    myMenu->addSeparator();
    myMenu->addAction(Busy::instance()->busyAction);
    myMenu->addAction(Away::instance()->awayAction);

    // main menu
    sccMenu = new QMenu(tr("SCC"));
    sccMenu->addAction(connectAction);
    sccMenu->addSeparator();
    sccMenu->addMenu(chatMenu);
    sccMenu->addMenu(myMenu);
    sccMenu->addSeparator();
    sccMenu->addAction(optionsAction);
    sccMenu->addSeparator();
    sccMenu->addAction(aboutAction);
    sccMenu->addAction(quitAction);

    mainToolButton = new QToolButton(this);
    mainToolButton->setIcon(QIcon(":/images/menu.svg"));
    mainToolButton->setText(tr("SCC"));
    mainToolButton->setMenu(sccMenu);
    mainToolButton->setPopupMode(QToolButton::InstantPopup);
    mainToolButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

    // notification
    notificationToolButton = new QToolButton(this);
    notificationToolButton->setIcon(QIcon(":/images/notification.svg"));
    notificationToolButton->setText(tr("N&otification"));
    notificationToolButton->setMenu(Notification::instance()->getNotificationMenu());
    notificationToolButton->setPopupMode(QToolButton::InstantPopup);
    notificationToolButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    notificationToolButton->setVisible(false);

    // toolbar
    toolBar = addToolBar(tr("Navigation bar"));
    toolBar->setIconSize(QSize(22,22));
    toolBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    toolBar->addWidget(mainToolButton);
    toolBar->addAction(connectAction);
    toolBar->addAction(channelListAction);
    toolBar->addAction(channelHomesAction);
    toolBar->addAction(friendsAction);
    toolBar->addAction(camsAction);
    toolBar->addAction(notesAction);
    notificationAction = toolBar->addWidget(notificationToolButton);

    // tray menu
    trayIconMenu = new QMenu();
    trayIconMenu->addAction(trayAction);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(minimizeAction);
    trayIconMenu->addAction(maximizeAction);
    trayIconMenu->addAction(restoreAction);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(quitAction);

    // tray
    trayIcon = new QSystemTrayIcon(QIcon(":/images/logo16x16.png"), this);
    trayIcon->setContextMenu(trayIconMenu);
    trayIcon->setToolTip("Simple Chat Client");
    trayIcon->setVisible(true);
}

void MainWindow::createSignals()
{
    // toolbutton
    connect(toolBar, SIGNAL(orientationChanged(Qt::Orientation)), this, SLOT(toolbarOrientationChanged(Qt::Orientation)));

    // signals buttons
    connect(connectAction, SIGNAL(triggered()), this, SLOT(buttonConnect()));
    connect(optionsAction, SIGNAL(triggered()), this, SLOT(openOptions()));
    connect(aboutAction, SIGNAL(triggered()), this, SLOT(openAbout()));

    // signals onet dialogs
    connect(channelListAction, SIGNAL(triggered()), this, SLOT(openChannelList()));
    connect(channelHomesAction, SIGNAL(triggered()), this, SLOT(openChannelHomes()));
    connect(channelFavouritesAction, SIGNAL(triggered()), this, SLOT(openChannelFavourites()));
    connect(friendsAction, SIGNAL(triggered()), this, SLOT(openFriends()));
    connect(ignoreAction, SIGNAL(triggered()), this, SLOT(openIgnore()));
    connect(myStatsAction, SIGNAL(triggered()), this, SLOT(openMyStats()));
    connect(myProfileAction, SIGNAL(triggered()), this, SLOT(openMyProfile()));
    connect(myAvatarAction, SIGNAL(triggered()), this, SLOT(openMyAvatar()));
    connect(findNickAction, SIGNAL(triggered()), this, SLOT(openFindNick()));
    connect(camsAction, SIGNAL(triggered()), this, SLOT(openCams()));
    connect(notesAction, SIGNAL(triggered()), this, SLOT(openNotes()));

    connect(notificationToolButton->menu(), SIGNAL(aboutToShow()), this, SLOT(stopAnimatedTrayIcon()));
    connect(Awaylog::instance()->awaylogAction, SIGNAL(triggered()), this, SLOT(openAwaylog()));
    connect(Invite::instance()->inviteAction, SIGNAL(triggered()), this, SLOT(openInviteList()));
    connect(Offline::instance()->offlineMessagesAction, SIGNAL(triggered()), this, SLOT(openOfflineMessages()));
    connect(Update::instance()->updateAction, SIGNAL(triggered()), this, SLOT(openUpdate()));

    connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(trayIconActivated(QSystemTrayIcon::ActivationReason)));

    connect(minimizeAction, SIGNAL(triggered()), this, SLOT(hide()));
    connect(maximizeAction, SIGNAL(triggered()), this, SLOT(showMaximized()));
    connect(restoreAction, SIGNAL(triggered()), this, SLOT(show()));
    connect(quitAction, SIGNAL(triggered()), qApp, SLOT(quit()));

    // tool widget
    connect(pToolWidget, SIGNAL(forceScrollToBottom()), pTabC, SLOT(forceScrollToBottom()));

    // signals tab
    connect(pTabM, SIGNAL(tabCloseRequested(int)), this, SLOT(tabCloseRequested(int)));
    connect(pTabM, SIGNAL(currentChanged(int)), this, SLOT(currentTabChanged(int)));

    // signals from network
    connect(Core::instance()->network, SIGNAL(socketStateChanged()), this, SLOT(updateButtons()));
    connect(Core::instance()->network, SIGNAL(kernel(const QString&)), pOnetKernel, SLOT(kernel(const QString&)));
    connect(Core::instance()->network, SIGNAL(authorize(QString,QString)), pOnetAuth, SLOT(authorize(QString,QString)));
    connect(Core::instance()->network, SIGNAL(updateNick(const QString&)), this, SLOT(updateNick(const QString&)));

    // signals from auth
    connect(pOnetAuth, SIGNAL(updateNick(const QString&)), this, SLOT(updateNick(const QString&)));
    connect(pOnetAuth, SIGNAL(authStateChanged()), this, SLOT(updateButtons()));
}

void MainWindow::init()
{
    // focus
    setTabOrder(pTabM, pToolWidget);
    pToolWidget->setFocus();

    // add default tabs
    addDefaultTabs();

    // show welcome
    showWelcome();

    // first run - show options if config not exist
    if (Settings::instance()->getBool("first_run"))
        firstRun();

    // auto connect
    if (Settings::instance()->getBool("auto_connect"))
        buttonConnect();
}

void MainWindow::addDefaultTabs()
{
    // debug
    if (Settings::instance()->getBool("debug"))
        pTabC->addTab(DEBUG_WINDOW);

    // status
    pTabC->addTab(STATUS_WINDOW);
}

void MainWindow::showWelcome()
{
    QString strWelcome = "%Fi:courier%"+tr("Welcome to the Simple Chat Client")+" %Ihehe%";
    Message::instance()->showMessage(STATUS_WINDOW, strWelcome, MessageDefault);
}

void MainWindow::firstRun()
{
    Settings::instance()->setBool("first_run", false);

    Config *pConfig = new Config(SettingsConfig);
    pConfig->set("first_run", "false");
    delete pConfig;

    //QTimer::singleShot(1000*1, this, SLOT(openOptions())); // 1 sec
}

void MainWindow::setTrayMenuVisible(bool visible)
{
    minimizeAction->setEnabled(visible);
    maximizeAction->setEnabled(!isMaximized());
    restoreAction->setEnabled(isMaximized() || !visible);
    setVisible(visible);
}

// refresh colors
void MainWindow::refreshColors()
{
    QString strTabBackgroundColor;
    QString strBackgroundColor;

    if (Settings::instance()->get("background_color").toLower() != "#ffffff")
    {
#ifdef Q_OS_WIN
        QString strMidColor = this->palette().midlight().color().name();
        strTabBackgroundColor = QString("QTabBar::tab { padding: 4px; margin-right: 3px; border: 1px solid %1; border-top-left-radius: 6px; border-top-right-radius: 6px; min-width: 8ex; background-color: %2;}").arg(strMidColor, Settings::instance()->get("background_color"));
#endif
        strBackgroundColor = QString("background-color:%1;").arg(Settings::instance()->get("background_color"));
    }

    QString strDefaultColor;
    if (Settings::instance()->get("default_color").toLower() != "#000000")
        strDefaultColor = QString("color:%1;").arg(Settings::instance()->get("default_color"));

    this->setStyleSheet(QString("*{%1}%2").arg(strDefaultColor+strBackgroundColor, strTabBackgroundColor));

    pTabC->refreshColors();
}

// refresh CSS
void MainWindow::refreshCSS()
{
    pTabC->refreshCSS();
}

// refresh inputline color
void MainWindow::refreshToolWidgetValues()
{
    pToolWidget->setDefaultValues();
}

// buttons
void MainWindow::buttonConnect()
{
    if (!Core::instance()->network->isConnected())
    {
        Settings::instance()->setBool("reconnect", true);
        Core::instance()->network->connect();
    }
    else
    {
        Settings::instance()->setBool("reconnect", false);
        Settings::instance()->setBool("logged", false);
        Core::instance()->network->disconnect();

        if (Settings::instance()->getBool("debug"))
            qDebug() << "Set timerReconnect: stop";
        Core::instance()->network->timerReconnect->stop();
    }
}

void MainWindow::updateButtons()
{
    bool bUpdateMenu = false;

    if ((Settings::instance()->get("socket_state") == "unknown") || (Settings::instance()->getBool("authorizing")))
    {
        bUpdateMenu = false;
        connectAction->setEnabled(false);
    }
    else
    {
        if (Settings::instance()->get("socket_state") == "disconnected")
        {
            bUpdateMenu = true;
            connectAction->setEnabled(true);
            connectAction->setText(tr("&Connect"));
            connectAction->setIconText(tr("&Connect"));
            connectAction->setIcon(QIcon(":/images/breeze/network-connect.svg"));
        }
        else if (Settings::instance()->get("socket_state") == "connected")
        {
            bUpdateMenu = true;
            connectAction->setEnabled(true);
            connectAction->setText(tr("&Disconnect"));
            connectAction->setIconText(tr("&Disconnect"));
            connectAction->setIcon(QIcon(":/images/breeze/network-disconnect.svg"));
        }
    }

    if (bUpdateMenu)
    {
        QString strNick = Settings::instance()->get("nick");

        bool bRegistered = (strNick.at(0) == '~' ? false : true);

        if (bRegistered)
        {
            channelHomesAction->setEnabled(true);
            channelFavouritesAction->setEnabled(true);
            friendsAction->setEnabled(true);
            ignoreAction->setEnabled(true);
            myStatsAction->setEnabled(true);
            myProfileAction->setEnabled(true);
            myAvatarAction->setEnabled(true);
        }
        else
        {
            channelHomesAction->setEnabled(false);
            channelFavouritesAction->setEnabled(false);
            friendsAction->setEnabled(false);
            ignoreAction->setEnabled(false);
            myStatsAction->setEnabled(false);
            myProfileAction->setEnabled(false);
            myAvatarAction->setEnabled(false);
        }
    }
}

void MainWindow::openOptions()
{
    OptionsGui(this).exec();
}

void MainWindow::openChannelList()
{
    if (Core::instance()->network->isConnected() && Settings::instance()->getBool("logged"))
        ChannelListGui(this).exec();
}

void MainWindow::openChannelHomes()
{
    if (Core::instance()->network->isConnected() && Settings::instance()->getBool("logged"))
        ChannelHomesGui(this).exec();
}

void MainWindow::openChannelFavourites()
{
    if (Core::instance()->network->isConnected() && Settings::instance()->getBool("logged"))
        ChannelFavouritesGui(this).exec();
}

void MainWindow::openFriends()
{
    if (Core::instance()->network->isConnected() && Settings::instance()->getBool("logged"))
        FriendsGui(this).exec();
}

void MainWindow::openIgnore()
{
    if (Core::instance()->network->isConnected() && Settings::instance()->getBool("logged"))
        IgnoreGui(this).exec();
}

void MainWindow::openFindNick()
{
    if (Core::instance()->network->isConnected() && Settings::instance()->getBool("logged"))
        FindNickGui(this).exec();
}

void MainWindow::openCams()
{
    if (Settings::instance()->get("webcam") == "system")
    {
#ifdef Q_OS_WIN
        if (Core::instance()->network->isConnected() && Settings::instance()->getBool("logged"))
        {
            QString strMe = Settings::instance()->get("nick");
            (new Kamerzysta(Core::instance()->kamerzystaSocket))->show(strMe);
        }
#else
        if (Core::instance()->network->isConnected() && Settings::instance()->getBool("logged"))
            new WebcamGui();
#endif
    }
    else // internal
    {
        if (Core::instance()->network->isConnected() && Settings::instance()->getBool("logged"))
            new WebcamGui();
    }
}

void MainWindow::openMyStats()
{
    if (Core::instance()->network->isConnected() && Settings::instance()->getBool("logged"))
        MyStatsGui(this).exec();
}

void MainWindow::openMyProfile()
{
    if (Core::instance()->network->isConnected() && Settings::instance()->getBool("logged"))
        MyProfileGui(this).exec();
}

void MainWindow::openMyAvatar()
{
    if (Core::instance()->network->isConnected() && Settings::instance()->getBool("logged"))
        MyAvatarGui(this).exec();
}

void MainWindow::openNotes()
{
    (new NotesGui(this))->show();
}

void MainWindow::openAbout()
{
    AboutGui(this).exec();
}

void MainWindow::openAwaylog()
{
    AwaylogGui(this).exec();
}

void MainWindow::openInviteList()
{
    if (Core::instance()->network->isConnected() && Settings::instance()->getBool("logged"))
        InviteListGui(this).exec();
}

void MainWindow::openOfflineMessages()
{
    if (Core::instance()->network->isConnected() && Settings::instance()->getBool("logged"))
        OfflineListGui(this).exec();
}

void MainWindow::openUpdate()
{
    UpdateGui(this).exec();
}

// start alert
void MainWindow::startAlert()
{
    QApplication::alert(this, 0);
}

// tray
void MainWindow::trayIconActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason)
    {
        case QSystemTrayIcon::Trigger:
        case QSystemTrayIcon::DoubleClick:
            if (this->isVisible())
                this->hide();
            else
                this->show();
            break;
        case QSystemTrayIcon::Context:
            setTrayMenuVisible(this->isVisible());
            break;
        default:
            break;
    }
}

void MainWindow::toolbarOrientationChanged(Qt::Orientation orientation)
{
    if (orientation == Qt::Horizontal)
    {
        toolBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
        mainToolButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
        notificationToolButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    }
    else
    {
        toolBar->setToolButtonStyle(Qt::ToolButtonIconOnly);
        mainToolButton->setToolButtonStyle(Qt::ToolButtonIconOnly);
        notificationToolButton->setToolButtonStyle(Qt::ToolButtonIconOnly);
    }
}

bool MainWindow::inputLineKeyEvent(QKeyEvent *k)
{
    // ctrl+tab pressed in inputline
    // alt+right pressed in inputline
    if (((k->key() == Qt::Key_Tab) && (k->modifiers() == Qt::ControlModifier)) ||
        ((k->key() == Qt::Key_Right) && (k->modifiers() == Qt::AltModifier)))
    {
        int index = pTabM->currentIndex();
        if (pTabM->count()-1 != index)
            pTabM->setCurrentIndex(index+1);
        else
            pTabM->setCurrentIndex(0);
        return true;
    }
    // ctrl+shift+tab pressed in inputline
    // alt+left pressed in inputline
    else if (((k->key() == Qt::Key_Backtab) && (k->modifiers() & (Qt::ControlModifier | Qt::ShiftModifier))) ||
            ((k->key() == Qt::Key_Left) && (k->modifiers() == Qt::AltModifier)))
    {
        int index = pTabM->currentIndex();
        if (index != 0)
            pTabM->setCurrentIndex(index-1);
        else
            pTabM->setCurrentIndex(pTabM->count()-1);
        return true;
    }
    // ctrl+F
    else if ((k->key() == Qt::Key_F) && (k->modifiers() == Qt::ControlModifier))
    {
        QString strChannel = Channel::instance()->getCurrentName();
        Channel::instance()->getChatView(strChannel)->search();
        return true;
    }
    // alt+1
    else if ((k->key() == Qt::Key_1) && (k->modifiers() == Qt::AltModifier))
    {
        if (pTabM->count() >= 1)
            pTabM->setCurrentIndex(0);
        return true;
    }
    // alt+2
    else if ((k->key() == Qt::Key_2) && (k->modifiers() == Qt::AltModifier))
    {
        if (pTabM->count() >= 2)
            pTabM->setCurrentIndex(1);
        return true;
    }
    // alt+3
    else if ((k->key() == Qt::Key_3) && (k->modifiers() == Qt::AltModifier))
    {
        if (pTabM->count() >= 3)
            pTabM->setCurrentIndex(2);
        return true;
    }
    // alt+4
    else if ((k->key() == Qt::Key_4) && (k->modifiers() == Qt::AltModifier))
    {
        if (pTabM->count() >= 4)
            pTabM->setCurrentIndex(3);
        return true;
    }
    // alt+5
    else if ((k->key() == Qt::Key_5) && (k->modifiers() == Qt::AltModifier))
    {
        if (pTabM->count() >= 5)
            pTabM->setCurrentIndex(4);
        return true;
    }
    // alt+6
    else if ((k->key() == Qt::Key_6) && (k->modifiers() == Qt::AltModifier))
    {
        if (pTabM->count() >= 6)
            pTabM->setCurrentIndex(5);
        return true;
    }
    // alt+7
    else if ((k->key() == Qt::Key_7) && (k->modifiers() == Qt::AltModifier))
    {
        if (pTabM->count() >= 7)
            pTabM->setCurrentIndex(6);
        return true;
    }
    // alt+8
    else if ((k->key() == Qt::Key_8) && (k->modifiers() == Qt::AltModifier))
    {
        if (pTabM->count() >= 8)
            pTabM->setCurrentIndex(7);
        return true;
    }
    // alt+9
    else if ((k->key() == Qt::Key_9) && (k->modifiers() == Qt::AltModifier))
    {
        if (pTabM->count() >= 9)
            pTabM->setCurrentIndex(8);
        return true;
    }
    // alt+0
    else if ((k->key() == Qt::Key_0) && (k->modifiers() == Qt::AltModifier))
    {
        if (pTabM->count() >= 10)
            pTabM->setCurrentIndex(9);
        return true;
    }

    return false;
}

int MainWindow::getCurrentTabIndex()
{
    return pTabM->currentIndex();
}

QSystemTrayIcon *MainWindow::getTrayIcon()
{
    return trayIcon;
}

QToolButton *MainWindow::getNotificationToolButton()
{
    return notificationToolButton;
}

QAction *MainWindow::getNotificationAction()
{
    return notificationAction;
}

void MainWindow::refreshToolButtons(const QString &strChannel)
{
    if (strChannel.isEmpty()) return;

    QString strMe = Settings::instance()->get("nick");

    // hide/show settings on non channel
    if (strChannel.at(0) != '#')
        pToolWidget->showChannelSettings(false);
    else
        pToolWidget->showChannelSettings(true);

    // moderation
    QString strModes = Nick::instance()->getModes(strMe, strChannel);
    if (strModes.contains(FLAG_MOD))
        pToolWidget->showModeration(true);
    else
        pToolWidget->showModeration(false);
}

void MainWindow::updateNick(const QString &strNick)
{
    setWindowTitle(QString("%1 - Simple Chat Client").arg(strNick));
}

void MainWindow::updateChannelIcon(int index, const QString &avatar)
{
    pTabM->updateIcon(index, avatar);
}

void MainWindow::insertTextToInputLine(const QString &strText)
{
    pToolWidget->insertTextToInputLine(strText);
}

void MainWindow::toolButtonFix(QWidget *widget)
{
#ifdef Q_OS_WIN
    // workaround for QTBUG-21808
    Q_ASSERT(widget);
    widget->setStyleSheet("QTabBar QToolButton {"
                          "background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #f2f2f2, stop:0.5 #ebebeb, stop:0.51 #dddddd, stop:1 #cfcfcf);"
                          "margin: 1px;"
                          "border: 1px solid #707070;"
                          "border-radius: 2px;"
                          "}"
                          "QTabBar QToolButton:pressed {"
                          "margin: 2px 0 0 2px;"
                          "border: 1px solid #000000;"
                          "}");
#else
    Q_UNUSED(widget);
#endif
}

// part tab
void MainWindow::tabCloseRequested(int index)
{
    pTabC->partTab(index);
}

void MainWindow::setTabColor(const QString &strChannel, MessageCategory eMessageCategory)
{
    if (eMessageCategory == MessageHighlight)
        pTabM->setAlert(strChannel, ChannelHighlight);
    else if (eMessageCategory != MessageDefault)
        pTabM->setAlert(strChannel, ChannelGreen);
    else
        pTabM->setAlert(strChannel, ChannelRed);
}

// change tab
void MainWindow::changeCurrentTab(int index)
{
    pTabM->setCurrentIndex(index);
}

// tab changed
void MainWindow::currentTabChanged(int index)
{
    // change tab color
    pTabM->setColor(index, QColor(Settings::instance()->get("default_color")));

    // clear input line
    pToolWidget->clearInputLine();

    // refresh tool buttons
    QString strChannel = Channel::instance()->getNameFromIndex(index);
    refreshToolButtons(strChannel);
}

void MainWindow::resizeEvent(QResizeEvent *e)
{
    pTabC->resizeMainWindow(e->size());
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (isVisible() && Settings::instance()->getBool("minimize_to_tray"))
    {
        hide();
        event->ignore();
    }
    else
    {
        if (Settings::instance()->getBool("always_quit"))
        {
            qApp->quit();
            return;
        }

        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setWindowIcon(QIcon(":/images/logo16x16.png"));
        msgBox.setWindowTitle(tr("Quit program"));
        msgBox.setText(tr("Are you sure you want to quit?"));

        QCheckBox dontPrompt(tr("Do not prompt again"), &msgBox);
        msgBox.addButton(&dontPrompt, QMessageBox::ActionRole);

        QPushButton *quitButton = msgBox.addButton(tr("Quit"), QMessageBox::ActionRole);
        QPushButton *cancelButton = msgBox.addButton(tr("Cancel"), QMessageBox::ActionRole);

        quitButton->setIcon(QIcon(":/images/breeze/dialog-ok.svg"));
        cancelButton->setIcon(QIcon(":/images/breeze/dialog-cancel.svg"));

        dontPrompt.blockSignals(true); // hack: blocking singals so QMessageBox won't close
        msgBox.exec();

        if (msgBox.clickedButton() == quitButton)
        {
            if (dontPrompt.checkState() == Qt::Checked)
            {
                Config *pConfig = new Config();
                pConfig->set("always_quit", "true");
                Settings::instance()->setBool("always_quit", true);
                delete pConfig;
            }

            qApp->quit();
        }
        else
            event->ignore();
    }
}
