
set (scc_UIS
	../ui/about.ui
	../ui/avatar_edit.ui
	../ui/avatar_list_widget.ui
	../ui/awaylog.ui
	../ui/captcha.ui
	../ui/channel_favourites.ui
	../ui/channel_homes.ui
	../ui/channel_key.ui
	../ui/channel_list.ui
	../ui/channel_settings.ui
	../ui/emoticons.ui
	../ui/find_nick.ui
	../ui/find_text.ui
	../ui/friends.ui
	../ui/ignore.ui
	../ui/invite.ui
	../ui/invite_list.ui
	../ui/join_channel.ui
	../ui/moderation.ui
	../ui/my_avatar.ui
	../ui/my_profile.ui
	../ui/my_stats.ui
	../ui/notes.ui
	../ui/offline_list.ui
	../ui/offline_message.ui
	../ui/options.ui
	../ui/profile_add.ui
	../ui/profile_edit.ui
	../ui/profile_manager.ui
	../ui/register_nick.ui
	../ui/themes.ui
	../ui/update.ui
	../ui/user_avatar.ui
	../ui/webbrowser.ui
)

set (scc_SRCS
	common/commands.cpp
	common/config.cpp
	common/convert.cpp
	common/html_messages_renderer.cpp
	common/log.cpp
	common/replace.cpp
	common/simple_crypt.cpp
	common/xml.cpp

	core/core.cpp
	core/main.cpp
	core/mainwindow.cpp
	core/network.cpp

	gui/about_gui.cpp
	gui/avatar_edit_gui.cpp
	gui/awaylog_gui.cpp
	gui/captcha_gui.cpp
	gui/channel_favourites_gui.cpp
	gui/channel_homes_gui.cpp
	gui/channel_key_gui.cpp
	gui/channel_list_gui.cpp
	gui/channel_settings_gui.cpp
	gui/emoticons_gui.cpp
	gui/find_nick_gui.cpp
	gui/find_text_gui.cpp
	gui/friends_gui.cpp
	gui/ignore_gui.cpp
	gui/invite_gui.cpp
	gui/invite_list_gui.cpp
	gui/join_channel_gui.cpp
	gui/moderation_gui.cpp
	gui/my_avatar_gui.cpp
	gui/my_profile_gui.cpp
	gui/my_stats_gui.cpp
	gui/notes_gui.cpp
	gui/offline_list_gui.cpp
	gui/offline_message_gui.cpp
	gui/options_gui.cpp
	gui/profile_add_gui.cpp
	gui/profile_edit_gui.cpp
	gui/profile_manager_gui.cpp
	gui/register_nick_gui.cpp
	gui/themes_gui.cpp
	gui/update_gui.cpp
	gui/user_avatar_gui.cpp
	gui/user_profile_gui.cpp
	gui/webbrowser_gui.cpp

	models/autoaway.cpp
	models/avatar.cpp
	models/away.cpp
	models/awaylog.cpp
	models/busy.cpp
	models/cache.cpp
	models/channel.cpp
	models/channel_favourites.cpp
	models/channel_homes.cpp
	models/channel_list.cpp
	models/channel_settings.cpp
	models/emoticons.cpp
	models/find_nick.cpp
	models/friends.cpp
	models/highlight.cpp
	models/ignore.cpp
	models/invite.cpp
	models/lag.cpp
	models/message.cpp
	models/my_avatar.cpp
	models/my_profile.cpp
	models/my_stats.cpp
	models/nick.cpp
	models/nicklist.cpp
	models/notes.cpp
	models/notification.cpp
	models/offline.cpp
	models/profile_manager.cpp
	models/punish_reason.cpp
	models/settings.cpp
	models/sound_notify.cpp
	models/themes.cpp
	models/tray.cpp
	models/update.cpp
	models/user_profile.cpp
	models/utils.cpp

	onet/onet_auth.cpp
	onet/onet_avatar.cpp
	onet/onet_kernel.cpp
	onet/onet_utils.cpp

	tab/tab_container.cpp
	tab/tab_manager.cpp
	tab/tab_widget.cpp

	widgets/avatar_edit_scene.cpp
	widgets/avatar_list_widget.cpp
	widgets/chat_view.cpp
	widgets/inputline_widget.cpp
	widgets/nicklist_delegate.cpp
	widgets/nicklist_widget.cpp
	widgets/simple_percentage_widget.cpp
	widgets/simple_rank_widget.cpp
	widgets/simple_stats_widget.cpp
	widgets/tool_widget.cpp
	widgets/topic_widget.cpp
)

set (scc_MOC_HDRS
	common/commands.h
	common/config.h
	common/html_messages_renderer.h
	common/simple_crypt.h
	common/xml.h

	core/core.h
	core/mainwindow.h
	core/network.h

	gui/about_gui.h
	gui/avatar_edit_gui.h
	gui/awaylog_gui.h
	gui/captcha_gui.h
	gui/channel_favourites_gui.h
	gui/channel_homes_gui.h
	gui/channel_key_gui.h
	gui/channel_list_gui.h
	gui/channel_settings_gui.h
	gui/emoticons_gui.h
	gui/find_nick_gui.h
	gui/find_text_gui.h
	gui/friends_gui.h
	gui/ignore_gui.h
	gui/invite_gui.h
	gui/invite_list_gui.h
	gui/join_channel_gui.h
	gui/moderation_gui.h
	gui/my_avatar_gui.h
	gui/my_profile_gui.h
	gui/my_stats_gui.h
	gui/notes_gui.h
	gui/offline_list_gui.h
	gui/offline_message_gui.h
	gui/options_gui.h
	gui/profile_add_gui.h
	gui/profile_edit_gui.h
	gui/profile_manager_gui.h
	gui/register_nick_gui.h
	gui/themes_gui.h
	gui/update_gui.h
	gui/user_avatar_gui.h
	gui/user_profile_gui.h
	gui/webbrowser_gui.h

	models/autoaway.h
	models/avatar.h
	models/away.h
	models/awaylog.h
	models/busy.h
	models/cache.h
	models/channel.h
	models/channel_favourites.h
	models/channel_homes.h
	models/channel_list.h
	models/channel_settings.h
	models/emoticons.h
	models/find_nick.h
	models/friends.h
	models/highlight.h
	models/ignore.h
	models/invite.h
	models/lag.h
	models/message.h
	models/my_profile.h
	models/my_stats.h
	models/nick.h
	models/nicklist.h
	models/notes.h
	models/notification.h
	models/offline.h
	models/profile_manager.h
	models/punish_reason.h
	models/settings.h
	models/sound_notify.h
	models/themes.h
	models/tray.h
	models/update.h
	models/user_profile.h
	models/utils.h

	onet/onet_auth.h
	onet/onet_avatar.h
	onet/onet_kernel.h

	tab/tab_container.h
	tab/tab_manager.h
	tab/tab_widget.h

	widgets/avatar_edit_scene.h
	widgets/avatar_list_widget.h
	widgets/chat_view.h
	widgets/inputline_widget.h
	widgets/nicklist_delegate.h
	widgets/nicklist_widget.h
	widgets/simple_percentage_widget.h
	widgets/simple_rank_widget.h
	widgets/simple_stats_widget.h
	widgets/tool_widget.h
	widgets/topic_widget.h
)

set (scc_HDRS
	common/convert.h
	common/log.cpp
	common/replace.h
	core/defines.h
	models/my_avatar.h
)

# kamerzysta
if (WIN32)
	LIST (APPEND scc_SRCS
		common/kamerzysta.cpp
	)
	LIST (APPEND scc_MOC_HDRS
		common/kamerzysta.h
	)
endif (WIN32)

# internal camera
LIST (APPEND scc_UIS
	../ui/webcam_standard.ui
	../ui/webcam_mini.ui
)
LIST (APPEND scc_SRCS
	common/webcam_engine.cpp
	common/webcam_network.cpp
	gui/webcam_gui.cpp
	gui/webcam_standard_gui.cpp
	gui/webcam_mini_gui.cpp
	widgets/webcam_delegate.cpp
)
LIST (APPEND scc_MOC_HDRS
	common/webcam_engine.h
	common/webcam_network.h
	gui/webcam_gui.h
	gui/webcam_standard_gui.h
	gui/webcam_mini_gui.h
	widgets/webcam_delegate.h
)

# winamp
if (WIN32)
	LIST (APPEND scc_SRCS
		common/winamp.cpp
	)
else (WIN32)
	find_package(Qt5DBus)
	LIST (APPEND scc_MOC_HDRS
		common/mpris_player.h
	)

	LIST (APPEND scc_SRCS
		common/mpris_player.cpp
	)
endif (WIN32)

# qca2
if (WIN32)
	if (MINGW)
		set (QCA2_LIBRARIES ${Qt5Core_DIR}/../../libqca.dll.a)
	else (MINGW)
		set (QCA2_LIBRARIES ${Qt5Core_DIR}/../../qca.lib)
	endif (MINGW)
else (WIN32)
	pkg_search_module (QCA2 REQUIRED qca2-qt5)
	include_directories (${QCA2_INCLUDE_DIRS})
	link_directories (${QCA2_LIBRARY_DIRS})
	add_definitions (${QCA2_CFLAGS_OTHER})
endif (WIN32)

# Tell CMake to run moc when necessary:
set(CMAKE_AUTOMOC ON)

# As moc files are generated in the binary dir, tell CMake
# to always look for includes there:
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# qt
find_package(Qt5Widgets)
find_package(Qt5Multimedia)
find_package(Qt5Network)
find_package(Qt5WebKit)
find_package(Qt5WebKitWidgets)
find_package(Qt5Xml)
 
# include qt qtnetwork
include_directories (
	${CMAKE_BINARY_DIR}
)

# resources
set (scc_QRC ../scc.qrc)
qt5_add_resources (scc_QRC_SRCS ${scc_QRC})

# ui
qt5_wrap_ui (scc_UIS_H ${scc_UIS})

# moc
qt5_generate_moc (scc MOC_FILES ${scc_MOC_HDRS})

# resource
set (scc_RESOURCE ../scc_mingw.rc)

include_directories (${CMAKE_SOURCE_DIR}/ui)

# link
if (MINGW) # mingw
	add_executable (scc ${scc_SRCS} ${MOC_FILES} ${scc_HDRS} ${scc_QRC_SRCS} ${scc_UIS_H} ${scc_RESOURCE})
else (MINGW) # linux
	add_executable (scc ${scc_SRCS} ${MOC_FILES} ${scc_HDRS} ${scc_QRC_SRCS} ${scc_UIS_H})
endif (MINGW)

target_link_libraries (scc Qt5::Widgets Qt5::Multimedia Qt5::Network Qt5::WebKit Qt5::WebKitWidgets Qt5::Xml ${QCA2_LIBRARIES})
if (NOT WIN32)
	target_link_libraries (scc Qt5::DBus)
endif (NOT WIN32)

# install
if (UNIX AND NOT APPLE)
	install (TARGETS scc DESTINATION ${SCC_BIN_DIR})
endif (UNIX AND NOT APPLE)
