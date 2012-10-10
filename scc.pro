# -------------------------------------------------
# Project created by QtCreator 2009-08-13T02:50:42
# -------------------------------------------------
CONFIG += qtopia
QT += network \
    svg \
    xml
TARGET = scc
TEMPLATE = app
RESOURCES += scc.qrc
SOURCES += src/common/commands.cpp \
    src/common/config.cpp \
    src/common/convert.cpp \
    src/common/html_messages_renderer.cpp \
    src/common/kamerzysta.cpp \
    src/common/log.cpp \
    src/common/replace.cpp \
    src/common/simple_crypt.cpp \
    src/common/webcam_network.cpp \
    src/common/webcam_engine.cpp \
    src/common/winamp.cpp \
    src/common/xml.cpp \
    src/core/core.cpp \
    src/core/main.cpp \
    src/core/mainwindow.cpp \
    src/core/network.cpp \
    src/debug/debug_block.cpp \
    src/debug/linux_debug.cpp \
    src/gui/about.cpp \
    src/gui/avatar_edit.cpp \
    src/gui/awaylog.cpp \
    src/gui/captcha.cpp \
    src/gui/channel_favourites.cpp \
    src/gui/channel_homes.cpp \
    src/gui/channel_key.cpp \
    src/gui/channel_list.cpp \
    src/gui/channel_settings.cpp \
    src/gui/emoticons.cpp \
    src/gui/find_nick.cpp \
    src/gui/find_text.cpp \
    src/gui/friends.cpp \
    src/gui/ignore.cpp \
    src/gui/invite.cpp \
    src/gui/invite_list.cpp \
    src/gui/moderation.cpp \
    src/gui/my_avatar.cpp \
    src/gui/my_profile.cpp \
    src/gui/my_stats.cpp \
    src/gui/notes.cpp \
    src/gui/offline_messages.cpp \
    src/gui/options.cpp \
    src/gui/profile_add.cpp \
    src/gui/profile_edit.cpp \
    src/gui/profile_manager.cpp \
    src/gui/register_nick.cpp \
    src/gui/update.cpp \
    src/gui/user_avatar.cpp \
    src/gui/user_profile.cpp \
    src/gui/webbrowser.cpp \
    src/gui/webcam.cpp \
    src/gui/webcam_mini.cpp \
    src/gui/webcam_standard.cpp \
    src/models/channel_favourites_model.cpp \
    src/models/channel_homes_model.cpp \
    src/models/channel_list_model.cpp \
    src/models/channel_settings_model.cpp \
    src/models/find_nick_model.cpp \
    src/models/friends_model.cpp \
    src/models/ignore_model.cpp \
    src/models/moderation_model.cpp \
    src/models/my_profile_model.cpp \
    src/models/my_stats_model.cpp \\
    src/models/settings.cpp \
    src/models/themes_model.cpp \
    src/models/user_profile_model.cpp \
    src/onet/avatar_client.cpp \
    src/onet/my_avatar_model.cpp \
    src/onet/onet_auth.cpp \
    src/onet/onet_kernel.cpp \
    src/onet/onet_utils.cpp \	
    src/singleton/avatar.cpp \
    src/singleton/autoaway.cpp \
    src/singleton/away.cpp \
    src/singleton/awaylog_model.cpp \
    src/singleton/busy.cpp \
    src/singleton/channel.cpp \
    src/singleton/highlight.cpp \
    src/singleton/invite_model.cpp \
    src/singleton/lag.cpp \
    src/singleton/message.cpp \
    src/singleton/nicklist.cpp \
    src/singleton/notes_model.cpp \
    src/singleton/notification.cpp \
    src/singleton/offline.cpp \
    src/singleton/profile_manager_model.cpp \
    src/singleton/punish_reason.cpp \
    src/singleton/sound_notify.cpp \
    src/singleton/tray.cpp \
    src/singleton/updates.cpp \
    src/singleton/utils.cpp \
    src/tab/tab_container.cpp \
    src/tab/tab_manager.cpp \
    src/tab/tab_widget.cpp \
    src/widgets/avatar_edit_scene.cpp \
    src/widgets/avatar_list_widget.cpp \
    src/widgets/chat_view.cpp \
    src/widgets/inputline_widget.cpp \
    src/widgets/nicklist_delegate.cpp \
    src/widgets/nicklist_widget.cpp \
    src/widgets/simple_percentage_widget.cpp \
    src/widgets/simple_rank_widget.cpp \
    src/widgets/simple_stats_widget.cpp \
    src/widgets/tool_widget.cpp \
    src/widgets/webcam_delegate.cpp
HEADERS += src/common/commands.h \
    src/common/config.h \
    src/common/convert.h \
    src/common/log.h \
    src/common/html_messages_renderer.h \
    src/common/kamerzysta.h \
    src/common/replace.h \
    src/common/simple_crypt.h \
    src/common/webcam_engine.h \
    src/common/webcam_network.h \
    src/common/winamp.h \
    src/common/xml.h \
    src/core/core.h \
    src/core/defines.h \
    src/core/main.h \
    src/core/mainwindow.h \
    src/core/network.h \
    src/debug/debug_block.h \
    src/debug/linux_debug.h \
    src/gui/about.h \
    src/gui/avatar_edit.h \
    src/gui/awaylog.h \
    src/gui/captcha.h \
    src/gui/channel_favourites.h \
    src/gui/channel_homes.h \
    src/gui/channel_key.h \
    src/gui/channel_list.h \
    src/gui/channel_settings.h \
    src/gui/emoticons.h \
    src/gui/find_nick.h \
    src/gui/find_text.h \
    src/gui/friends.h \
    src/gui/ignore.h \
    src/gui/invite.h \
    src/gui/invite_list.h \
    src/gui/moderation.h \
    src/gui/my_avatar.h \
    src/gui/my_profile.h \
    src/gui/my_stats.h \
    src/gui/notes.h \
    src/gui/offline_messages.h \
    src/gui/options.h \
    src/gui/profile_add.h \
    src/gui/profile_edit.h \
    src/gui/profile_manager.h \
    src/gui/register_nick.h \
    src/gui/update.h \
    src/gui/user_avatar.h \
    src/gui/user_profile.h \
    src/gui/webbrowser.h \
    src/gui/webcam.h \
    src/gui/webcam_mini.h \
    src/gui/webcam_standard.h \
    src/models/channel_favourites_model.h \
    src/models/channel_homes_model.h \
    src/models/channel_list_model.h \
    src/models/channel_settings_model.h \
    src/models/find_nick_model.h \
    src/models/friends_model.h \
    src/models/ignore_model.h \
    src/models/moderation_model.h \
    src/models/my_profile_model.h \
    src/models/my_stats_model.h \
    src/models/settings.h \
    src/models/themes_model.h \
    src/models/user_profile_model.h \
    src/onet/avatar_client.h \
    src/onet/my_avatar_model.h \
    src/onet/onet_auth.h \
    src/onet/onet_kernel.h \
    src/onet/onet_utils.h \
    src/singleton/avatar.h \
    src/singleton/autoaway.h \
    src/singleton/away.h \
    src/singleton/awaylog_model.h \
    src/singleton/busy.h \
    src/singleton/channel.h \
    src/singleton/highlight.h \
    src/singleton/invite_model.h \
    src/singleton/lag.h \
    src/singleton/message.h \
    src/singleton/nicklist.h \
    src/singleton/notes_model.h \
    src/singleton/notification.h \
    src/singleton/offline.h \
    src/singleton/profile_manager_model.h \
    src/singleton/punish_reason.h \
    src/singleton/sound_notify.h \
    src/singleton/tray.h \
    src/singleton/updates.h \
    src/singleton/utils.h \
    src/tab/tab_container.h \
    src/tab/tab_manager.h \
    src/tab/tab_widget.h \
    src/widgets/avatar_edit_scene.h \
    src/widgets/avatar_list_widget.h \
    src/widgets/chat_view.h \
    src/widgets/inputline_widget.h \
    src/widgets/nicklist_delegate.h \
    src/widgets/nicklist_widget.h \
    src/widgets/simple_percentage_widget.h \
    src/widgets/simple_rank_widget.h \
    src/widgets/simple_stats_widget.h \
    src/widgets/tool_widget.h \
    src/widgets/webcam_delegate.h
FORMS += ui/about.ui \
    ui/avatar_edit.ui \
    ui/avatar_list_widget.ui \
    ui/awaylog.ui \
    ui/captcha.ui \
    ui/channel_favourites.ui \
    ui/channel_homes.ui \
    ui/channel_key.ui \
    ui/channel_list.ui \
    ui/channel_settings.ui \
    ui/emoticons.ui \
    ui/find_nick.ui \
    ui/find_text.ui \
    ui/friends.ui \
    ui/ignore.ui \
    ui/invite.ui \
    ui/invite_list.ui \
    ui/moderation.ui \
    ui/my_avatar.ui \
    ui/my_profile.ui \
    ui/my_stats.ui \
    ui/notes.ui \
    ui/offline_messages.ui \
    ui/options.ui \
    ui/profile_add.ui \
    ui/profile_edit.ui \
    ui/profile_manager.ui \
    ui/register_nick.ui \
    ui/update.ui \
    ui/user_avatar.ui \
    ui/webbrowser.ui \
    ui/webcam_mini.ui \
    ui/webcam_standard.ui
