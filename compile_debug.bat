@echo off 

set SCC_DIR=%CD%
set DESTINATION="C:\scc\release"
set QT_DIR="C:\Qt\5.5\mingw492_32"
set QT_TOOLS_DIR="C:\Qt\Tools"
set QCA2_DIR="C:\Qt\Tools\qca"
set OPENSSL_DIR="C:\OpenSSL-Win32"
set MINGW_DIR="C:\Qt\Tools\mingw492_32"
set CMAKE_DIR="C:\Program Files\CMake"
set CMAKE_32_DIR="C:\Program Files (x86)\CMake"
set GIT_DIR="C:\Program Files\Git"
set GIT_32_DIR="C:\Program Files (x86)\Git"

rem ----------------------- END OF CONFIG ---------------------------

rem settings for QCA2 and QCA-OSSL
set QTDIR=%QT_DIR%

if not exist %CMAKE_DIR%\bin\cmake.exe (
if not exist %CMAKE_32_DIR%\bin\cmake.exe (
echo Download CMake from http://www.cmake.org/files/v3.2/cmake-3.2.3-win32-x86.exe
pause
exit
)
)

if not exist %QT_DIR%\bin\qmake.exe (
echo Download Qt 5.5.0 for MinGW from http://www.qt.io/download/
pause
exit
)

if not exist %MINGW_DIR%\bin\mingw32-make.exe (
echo Download MinGW from http://sourceforge.net/projects/mingw/files/
pause
exit
)

if not exist %GIT_DIR%\bin\git.exe (
if not exist %GIT_32_DIR%\bin\git.exe (
echo Download Git 2.9.0 for Windows from https://github.com/git-for-windows/git/releases/download/v2.9.0.windows.1/Git-2.9.0-32-bit.exe
pause
exit
)
)

if not exist %QCA2_DIR%\include\QtCrypto\qca.h (goto compile_qca)
if not exist %QT_DIR%\bin\libqca-qt5.dll (goto compile_qca)
if not exist %QT_DIR%\mkspecs\features\crypto.prf (goto compile_qca)
goto end_compile_qca
:compile_qca
echo Compiling qca
cd /D %QT_TOOLS_DIR%
git clone git://anongit.kde.org/qca qca
cd /D %QCA2_DIR%
cmake . -G "MinGW Makefiles" -DBUILD_TESTS=OFF
mingw32-make
mingw32-make install
cd /D %SCC_DIR%
:end_compile_qca

if not exist %OPENSSL_DIR%\include\openssl\ssl.h (
echo Download OpenSSL from http://slproweb.com/download/Win32OpenSSL-1_0_2d.exe
pause
exit
)


if exist src\scc.exe (
del /Q /F /A src\scc.exe
)
cmake . -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Debug
mingw32-make

if not exist src\scc.exe (
echo SCC is not compiled. Please compile program.
pause
exit
)

echo Killing scc
taskkill /F /IM scc.exe

echo Removing destination folder
if exist %DESTINATION% (
rd %DESTINATION% /S /Q
)

echo Copying main files
xcopy AUTHORS      %DESTINATION%\ /R /Y /Q
xcopy ChangeLog    %DESTINATION%\ /R /Y /Q
xcopy LICENSE      %DESTINATION%\ /R /Y /Q
xcopy README.md    %DESTINATION%\ /R /Y /Q
xcopy TODO         %DESTINATION%\ /R /Y /Q
xcopy VERSION      %DESTINATION%\ /R /Y /Q
xcopy src\scc.exe  %DESTINATION%\ /R /Y /Q

echo Copying Qt files
xcopy %QT_DIR%\bin\Qt5Cored.dll          %DESTINATION%\ /C /H /R /Y /Q
xcopy %QT_DIR%\bin\Qt5Guid.dll           %DESTINATION%\ /C /H /R /Y /Q
xcopy %QT_DIR%\bin\Qt5Multimediad.dll    %DESTINATION%\ /C /H /R /Y /Q
xcopy %QT_DIR%\bin\Qt5Networkd.dll       %DESTINATION%\ /C /H /R /Y /Q
xcopy %QT_DIR%\bin\Qt5WebKitd.dll        %DESTINATION%\ /C /H /R /Y /Q
xcopy %QT_DIR%\bin\Qt5WebKitWidgetsd.dll %DESTINATION%\ /C /H /R /Y /Q
xcopy %QT_DIR%\bin\Qt5Widgetsd.dll       %DESTINATION%\ /C /H /R /Y /Q
xcopy %QT_DIR%\bin\Qt5Xmld.dll           %DESTINATION%\ /C /H /R /Y /Q

echo Copying Qt 5.4 dependency
xcopy %QT_DIR%\bin\icuin53.dll           %DESTINATION%\ /C /H /R /Y /Q
xcopy %QT_DIR%\bin\icuuc53.dll           %DESTINATION%\ /C /H /R /Y /Q
xcopy %QT_DIR%\bin\icudt53.dll           %DESTINATION%\ /C /H /R /Y /Q

echo Copying Qt 5.5 dependency
xcopy %QT_DIR%\bin\icuin54.dll           %DESTINATION%\ /C /H /R /Y /Q
xcopy %QT_DIR%\bin\icuuc54.dll           %DESTINATION%\ /C /H /R /Y /Q
xcopy %QT_DIR%\bin\icudt54.dll           %DESTINATION%\ /C /H /R /Y /Q

echo Copying Qt dependency
xcopy %QT_DIR%\bin\Qt5Sqld.dll           %DESTINATION%\ /C /H /R /Y /Q
xcopy %QT_DIR%\bin\Qt5Svgd.dll           %DESTINATION%\ /C /H /R /Y /Q
xcopy %QT_DIR%\bin\Qt5Qmld.dll           %DESTINATION%\ /C /H /R /Y /Q
xcopy %QT_DIR%\bin\Qt5Quickd.dll           %DESTINATION%\ /C /H /R /Y /Q
xcopy %QT_DIR%\bin\Qt5Sensorsd.dll           %DESTINATION%\ /C /H /R /Y /Q
xcopy %QT_DIR%\bin\Qt5Positioningd.dll           %DESTINATION%\ /C /H /R /Y /Q
xcopy %QT_DIR%\bin\Qt5OpenGLd.dll           %DESTINATION%\ /C /H /R /Y /Q
xcopy %QT_DIR%\bin\Qt5PrintSupportd.dll           %DESTINATION%\ /C /H /R /Y /Q
xcopy %QT_DIR%\bin\Qt5MultimediaWidgetsd.dll           %DESTINATION%\ /C /H /R /Y /Q
xcopy %QT_DIR%\bin\Qt5WebChanneld.dll           %DESTINATION%\ /C /H /R /Y /Q

echo Copying MinGW files
xcopy %QT_DIR%\bin\libgcc_s_dw2-1.dll   %DESTINATION%\ /C /H /R /Y /Q
xcopy %QT_DIR%\bin\libstdc++-6.dll      %DESTINATION%\ /C /H /R /Y /Q
xcopy %QT_DIR%\bin\libwinpthread-1.dll  %DESTINATION%\ /C /H /R /Y /Q

echo Copying Qt plugins
xcopy %QT_DIR%\plugins\imageformats\qgifd.dll         %DESTINATION%\plugins\imageformats\   /C /H /R /Y /Q
xcopy %QT_DIR%\plugins\imageformats\qjpegd.dll        %DESTINATION%\plugins\imageformats\   /C /H /R /Y /Q
xcopy %QT_DIR%\plugins\imageformats\qsvgd.dll         %DESTINATION%\plugins\imageformats\   /C /H /R /Y /Q
xcopy %QT_DIR%\plugins\iconengines\qsvgicond.dll         %DESTINATION%\plugins\iconengines\   /C /H /R /Y /Q
xcopy %QT_DIR%\plugins\mediaservice\dsengined.dll     %DESTINATION%\plugins\mediaservice\ /C /H /R /Y /Q
xcopy %QT_DIR%\plugins\platforms\qwindowsd.dll        %DESTINATION%\plugins\platforms\ /C /H /R /Y /Q

echo Creating Qt conf
echo [Paths] > %DESTINATION%\qt.conf
echo Plugins = plugins >> %DESTINATION%\qt.conf
echo Translations = translations >> %DESTINATION%\qt.conf

echo Copying OpenSSL libs
xcopy %OPENSSL_DIR%\libeay32.dll %DESTINATION%\ /C /H /R /Y /Q
xcopy %OPENSSL_DIR%\ssleay32.dll %DESTINATION%\ /C /H /R /Y /Q 

echo Copying QCA libs
xcopy %QT_DIR%\bin\libqca-qt5.dll %DESTINATION%\ /C /H /R /Y /Q
xcopy %QT_DIR%\plugins\crypto\libqca-ossl.dll %DESTINATION%\plugins\crypto\ /C /H /R /Y /Q

echo Copying translations
xcopy translations\*.qm %DESTINATION%\translations\ /C /H /R /Y /Q 

echo Copying images
xcopy images\registered_user_avatar.png %DESTINATION%\images\ /C /H /R /Y /Q 
xcopy images\registered_user_avatar_big.png %DESTINATION%\images\ /C /H /R /Y /Q 
xcopy images\unregistered_user_avatar.png %DESTINATION%\images\ /C /H /R /Y /Q 
xcopy images\unregistered_user_avatar_big.png %DESTINATION%\images\ /C /H /R /Y /Q 
xcopy images\breeze\media-playback-start.svg %DESTINATION%\images\ /C /H /R /Y /Q 
xcopy images\admin.png %DESTINATION%\images\ /C /H /R /Y /Q 
xcopy images\bot.png %DESTINATION%\images\ /C /H /R /Y /Q 
xcopy images\dev.png %DESTINATION%\images\ /C /H /R /Y /Q 
xcopy images\halfop.png %DESTINATION%\images\ /C /H /R /Y /Q 
xcopy images\mod.png %DESTINATION%\images\ /C /H /R /Y /Q 
xcopy images\op.png %DESTINATION%\images\ /C /H /R /Y /Q 
xcopy images\owner.png %DESTINATION%\images\ /C /H /R /Y /Q 
xcopy images\screener.png %DESTINATION%\images\ /C /H /R /Y /Q 
xcopy images\voice.png %DESTINATION%\images\ /C /H /R /Y /Q 

echo Copying wallpapers
xcopy images\wallpaper\*.jpg %DESTINATION%\images\wallpaper\ /C /H /R /Y /Q 

echo Copying sounds
xcopy sounds\LICENSE %DESTINATION%\sounds\ /C /H /R /Y /Q 
xcopy sounds\*.wav %DESTINATION%\sounds\ /C /H /R /Y /Q 

echo Copying emoticons
xcopy emoticons\LICENSE %DESTINATION%\emoticons\ /C /H /R /Y /Q 
xcopy emoticons\*.gif %DESTINATION%\emoticons\ /C /H /R /Y /Q /S
xcopy emoticons\*.png %DESTINATION%\emoticons\ /C /H /R /Y /Q /S

echo Copying emoticons emoji
xcopy emoticons_emoji\*.png %DESTINATION%\emoticons_emoji\ /C /H /R /Y /Q /S

echo Done!
pause
