# init
project (scc)
cmake_minimum_required (VERSION 2.8.11 FATAL_ERROR)
include (FindPkgConfig)
set (QT_MIN_VERSION "5.0.0")
find_package(Qt5Core REQUIRED)
find_package(Qt5LinguistTools)

# gcc minimum
if (CMAKE_COMPILER_IS_GNUCXX)
    if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 4.8)
        message(FATAL_ERROR "G++ version must be at least 4.8!")
    endif()
else()
    message(WARNING "You are using an unsupported compiler!")
endif()

# optimization
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build, options are: Debug Release RelWithDebInfo." FORCE)
endif (NOT CMAKE_BUILD_TYPE)

if (CMAKE_COMPILER_IS_GNUCXX)
	# standard
	set (CMAKE_C_FLAGS "-Wall -Wextra ${CMAKE_C_FLAGS}")
	set (CMAKE_CXX_FLAGS "-Wall -Wextra ${CMAKE_CXX_FLAGS}")

	# pedantic
	#set (CMAKE_C_FLAGS "-Wall -Wextra -pedantic -ansi ${CMAKE_C_FLAGS}")
	#set (CMAKE_CXX_FLAGS "-Wall -Wextra -pedantic -ansi -std=c++11 ${CMAKE_CXX_FLAGS}")

	# debug
	#set (CMAKE_C_FLAGS "-Wall -Wextra -pedantic -ansi ${CMAKE_C_FLAGS}")
	#set (CMAKE_CXX_FLAGS "-Wall -Wextra -pedantic -ansi -std=c++11 -ggdb -pg ${CMAKE_CXX_FLAGS}")
endif (CMAKE_COMPILER_IS_GNUCXX)

if (MSVC)
	set (COMMON_COMPILE_FLAGS "/MP")
endif (MSVC)

if (MINGW)
	# enable ASLR
	set (CMAKE_CXX_LINK_FLAGS "${CMAKE_CXX_LINK_FLAGS} -Wl,--dynamicbase -Wl,--nxcompat")

	# disable console on windows mingw
	if (CMAKE_BUILD_TYPE STREQUAL "Release")
		set (CMAKE_CXX_LINK_FLAGS "${CMAKE_CXX_LINK_FLAGS} -mwindows")
	endif (CMAKE_BUILD_TYPE STREQUAL "Release")
endif (MINGW)

# set
set (SCC_BIN_DIR ${CMAKE_INSTALL_PREFIX}/bin)
set (SCC_SHARE_DIR ${CMAKE_INSTALL_PREFIX}/share)
set (SCC_DATA_DIR ${CMAKE_INSTALL_PREFIX}/share/scc)

# sub dir
add_subdirectory (src)
add_subdirectory (translations)

# configure
configure_file (${CMAKE_SOURCE_DIR}/scc-config.h.cmake ${CMAKE_BINARY_DIR}/scc-config.h)

if (UNIX AND NOT APPLE)
	message (STATUS " * install prefix: ${CMAKE_INSTALL_PREFIX}")
endif (UNIX AND NOT APPLE)
message (STATUS " * build type: ${CMAKE_BUILD_TYPE}")

# install
if (UNIX AND NOT APPLE)
	install (FILES AUTHORS ChangeLog LICENSE README.md scc.png TODO DESTINATION ${SCC_DATA_DIR})
	install (FILES scc.desktop DESTINATION ${SCC_SHARE_DIR}/applications)
	
	install (FILES images/registered_user_avatar.png DESTINATION ${SCC_DATA_DIR}/images)
	install (FILES images/registered_user_avatar_big.png DESTINATION ${SCC_DATA_DIR}/images)
	install (FILES images/unregistered_user_avatar.png DESTINATION ${SCC_DATA_DIR}/images)
	install (FILES images/unregistered_user_avatar_big.png DESTINATION ${SCC_DATA_DIR}/images)
	install (FILES images/breeze/media-playback-start.svg DESTINATION ${SCC_DATA_DIR}/images)
	install (FILES images/admin.png DESTINATION ${SCC_DATA_DIR}/images)
	install (FILES images/bot.png DESTINATION ${SCC_DATA_DIR}/images)
	install (FILES images/dev.png DESTINATION ${SCC_DATA_DIR}/images)
	install (FILES images/halfop.png DESTINATION ${SCC_DATA_DIR}/images)
	install (FILES images/mod.png DESTINATION ${SCC_DATA_DIR}/images)
	install (FILES images/op.png DESTINATION ${SCC_DATA_DIR}/images)
	install (FILES images/owner.png DESTINATION ${SCC_DATA_DIR}/images)
	install (FILES images/screener.png DESTINATION ${SCC_DATA_DIR}/images)
	install (FILES images/voice.png DESTINATION ${SCC_DATA_DIR}/images)

	install (DIRECTORY images/wallpaper DESTINATION ${SCC_DATA_DIR}/images)
	install (DIRECTORY sounds DESTINATION ${SCC_DATA_DIR})
	install (DIRECTORY emoticons DESTINATION ${SCC_DATA_DIR})
	install (DIRECTORY emoticons_emoji DESTINATION ${SCC_DATA_DIR})
endif (UNIX AND NOT APPLE)

if (MINGW)
	install (FILES src/scc.exe DESTINATION ${CMAKE_SOURCE_DIR}/release)
endif (MINGW)

# uninstall
if (UNIX AND NOT APPLE)
	configure_file ("${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in" "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake" IMMEDIATE @ONLY)
	add_custom_target (uninstall COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)
endif (UNIX AND NOT APPLE)
