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

#include "winamp.h"

#define WINAMP_VERSION_MAJOR(winampVersion) ((winampVersion & 0x0000FF00) >> 12)
#define WINAMP_VERSION_MINOR(winampVersion) (winampVersion & 0x000000FF)  // returns, i.e. 0x12 for 5.12 and 0x10 for 5.1...

#define IPC_GETVERSION 0
#define IPC_ISPLAYING 104
#define IPC_GETOUTPUTTIME 105

Winamp::Winamp()
{
    hwnd_winamp = FindWindow("Winamp v1.x", NULL);
}

bool Winamp::isRunning()
{
    if (hwnd_winamp != 0)
        return true;
    else
        return false;
}

int Winamp::state()
{
    if (hwnd_winamp != 0)
    {
        /*
        ** This is sent to retrieve the current playback state of Winamp.
        ** If it returns 1, Winamp is playing.
        ** If it returns 3, Winamp is paused.
        ** If it returns 0, Winamp is not playing.
        */
        int res = SendMessage(hwnd_winamp,WM_USER,0,IPC_ISPLAYING);
        return res;
    }
    else
        return -1;
}

QString Winamp::version()
{
    if (hwnd_winamp != 0)
    {
        /*
        ** The version returned will be 0x20yx for Winamp 2.yx.
        ** Versions previous to Winamp 2.0 typically (but not always) use 0x1zyx for 1.zx.
        ** Just a bit weird but that's the way it goes.
        **
        ** For Winamp 5.x it uses the format 0x50yx for Winamp 5.yx
        ** e.g.   5.01 -> 0x5001
        **        5.09 -> 0x5009
        **        5.1  -> 0x5010
        **
        ** Notes: For 5.02 this api will return the same value as for a 5.01 build.
        **        For 5.07 this api will return the same value as for a 5.06 build.
        */
        int version = SendMessage(hwnd_winamp,WM_USER,0,IPC_GETVERSION);
        int majorVersionHex = WINAMP_VERSION_MAJOR(version);
        int minorVersionHex = WINAMP_VERSION_MINOR(version);
        return (QString::number(majorVersionHex, 16)+"."+QString::number(minorVersionHex, 16));
    }
    else
        return QString::null;
}

QString Winamp::song()
{
    if (hwnd_winamp != 0)
    {
        char buffer[256];
        GetWindowText(hwnd_winamp, buffer, sizeof(buffer));

        QString song = QString(buffer);
        song.remove(0, song.indexOf(".")+2);
        song.remove(" - Winamp");

        return song;
    }
    else
        return QString::null;
}

QString Winamp::position()
{
    if (hwnd_winamp != 0)
    {
        /*
        ** This api can return two different sets of information about current playback status.
        **
        ** If mode = 0 then it will return the position (in ms) of the currently playing track.
        ** Will return -1 if Winamp is not playing.
        **
        ** If mode = 1 then it will return the current track length (in seconds).
        ** Will return -1 if there are no tracks (or possibly if Winamp cannot get the length).
        **
        ** If mode = 2 then it will return the current track length (in milliseconds).
        ** Will return -1 if there are no tracks (or possibly if Winamp cannot get the length).
        */
        int res = SendMessage(hwnd_winamp,WM_USER,0,IPC_GETOUTPUTTIME);
        res /= 1000;

        int m = res/60;
        int s = res%60;

        return QString::number(m)+":"+(s < 10 ? "0"+QString::number(s) : QString::number(s));
    }
    else
        return QString::null;
}

QString Winamp::length()
{
    if (hwnd_winamp != 0)
    {
        /*
        ** This api can return two different sets of information about current playback status.
        **
        ** If mode = 0 then it will return the position (in ms) of the currently playing track.
        ** Will return -1 if Winamp is not playing.
        **
        ** If mode = 1 then it will return the current track length (in seconds).
        ** Will return -1 if there are no tracks (or possibly if Winamp cannot get the length).
        **
        ** If mode = 2 then it will return the current track length (in milliseconds).
        ** Will return -1 if there are no tracks (or possibly if Winamp cannot get the length).
        */
        int res = SendMessage(hwnd_winamp,WM_USER,1,IPC_GETOUTPUTTIME);

        if (res == -1)
            return "stream";

        int m = res/60;
        int s = res%60;

        return QString::number(m)+":"+(s < 10 ? "0"+QString::number(s) : QString::number(s));
    }
    else
        return QString::null;
}
