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

#ifndef IRC_KERNEL_H
#define IRC_KERNEL_H

#include <QObject>
#include <QStringList>
class TabContainer;

/**
 * Irc messages parser
 */
class IrcKernel : public QObject
{
    Q_OBJECT
public:
    IrcKernel(TabContainer *_pTabC);
    virtual ~IrcKernel();

public slots:
    void kernel(const QString &_strData);

private:
    TabContainer *pTabC;

    QString strData;
    QStringList strDataList;

private slots:
    void raw_error();
    void raw_ping();
    void raw_pong();
    void raw_join();
    void raw_part();
    void raw_quit();
    void raw_kick();
    void raw_mode();
    void raw_privmsg();
    void raw_notice();
    void raw_invite();
    void raw_topic();
    void raw_invreject();
    void raw_invignore();
    void raw_modermsg();
    void raw_modernotice();
    void raw_moderate();
    void raw_kill();
    void raw_nick();
    void raw_snonotice();
    void raw_001();
    void raw_002();
    void raw_003();
    void raw_004();
    void raw_005();
    void raw_211();
    void raw_212();
    void raw_213();
    void raw_215();
    void raw_218();
    void raw_219();
    void raw_240();
    void raw_242();
    void raw_243();
    void raw_244();
    void raw_248();
    void raw_249();
    void raw_251();
    void raw_252();
    void raw_253();
    void raw_254();
    void raw_255();
    void raw_256();
    void raw_257();
    void raw_258();
    void raw_259();
    void raw_265();
    void raw_266();
    void raw_271();
    void raw_272();
    void raw_301();
    void raw_302();
    void raw_303();
    void raw_304();
    void raw_305();
    void raw_306();
    void raw_307();
    void raw_311();
    void raw_312();
    void raw_313();
    void raw_314();
    void raw_315();
    void raw_316();
    void raw_317();
    void raw_318();
    void raw_319();
    void raw_320();
    void raw_321();
    void raw_322();
    void raw_323();
    void raw_324();
    void raw_329();
    void raw_330();
    void raw_332();
    void raw_333();
    void raw_335();
    void raw_340();
    void raw_341();
    void raw_352();
    void raw_353();
    void raw_355();
    void raw_366();
    void raw_369();
    void raw_371();
    void raw_372();
    void raw_374();
    void raw_375();
    void raw_376();
    void raw_378();
    void raw_379();
    void raw_381();
    void raw_391();
    void raw_396();
    void raw_401();
    void raw_402();
    void raw_403();
    void raw_405();
    void raw_406();
    void raw_412();
    void raw_421();
    void raw_404();
    void raw_432();
    void raw_433();
    void raw_441();
    void raw_442();
    void raw_443();
    void raw_445();
    void raw_446();
    void raw_451();
    void raw_461();
    void raw_462();
    void raw_470();
    void raw_471();
    void raw_473();
    void raw_474();
    void raw_475();
    void raw_480();
    void raw_481();
    void raw_482();
    void raw_484();
    void raw_491();
    void raw_492();
    void raw_495();
    void raw_530();
    void raw_531();
    void raw_600();
    void raw_601();
    void raw_602();
    void raw_604();
    void raw_605();
    void raw_607();
    void raw_666();
    void raw_671();
    void raw_702();
    void raw_703();
    void raw_704();
    void raw_807();
    void raw_808();
    void raw_809();
    void raw_811();
    void raw_812();
    void raw_815();
    void raw_816();
    void raw_817();
    void raw_818();
    void raw_819();
    void raw_820();
    void raw_821();
    void raw_942();
    void raw_950();
    void raw_951();
    void raw_952();
    void raw_974();
    void raw_975();
};

#endif // IRC_KERNEL_H
