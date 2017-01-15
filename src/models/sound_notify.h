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

#ifndef SOUND_NOTIFY_H
#define SOUND_NOTIFY_H

#include "scc-config.h"
#include <QMediaPlayer>
#include <QObject>
#include <QThread>

enum NotifyCategory {Beep, Query};

class SoundThread : public QThread
{
public:
    void run() { music->stop(); music->play(); exec(); }

    void setMedia(QMediaPlayer *m) { music = m; }
private:
    QMediaPlayer *music;
};

/**
 * Notify using Phonon
 */
class SoundNotify : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(SoundNotify)
    static SoundNotify *Instance;

public:
    static SoundNotify *instance();

    SoundNotify();
    virtual ~SoundNotify();
    void prepare();
    void play(NotifyCategory);

private:
    QString apath;
    QMediaPlayer *music;
    NotifyCategory eCurrentCategory;
    SoundThread thread;

    void init();
};

#endif // SOUND_NOTIFY_H
