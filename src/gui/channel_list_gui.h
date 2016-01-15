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

#ifndef CHANNEL_LIST_GUI_H
#define CHANNEL_LIST_GUI_H

#include <QDialog>
#include "ui_channel_list.h"

class ChannelListGui : public QDialog
{
    Q_OBJECT
public:
    ChannelListGui(QWidget *parent = 0);

private:
    Ui::uiChannelList ui;

    void createGui();
    void setDefaultValues();
    void createSignals();

    void clearFields();
    void clearLists();

    void getOptions();
    bool showChannel(const QString &name, int people, int cat, int type, bool moderated, bool recommended);

    QString strSearch;
    bool bShowTeen;
    bool bShowCommon;
    bool bShowErotic;
    bool bShowThematic;
    bool bShowRegional;

    bool bShowWild;
    bool bShowTame;
    bool bShowWithClass;
    bool bShowCult;
    bool bShowModerated;
    bool bShowRecommended;

    bool bHideEmpty;

protected:
    virtual void resizeEvent(QResizeEvent *);
    virtual void keyPressEvent(QKeyEvent *event);

private slots:
    void allCellDoubleClicked(int row, int column);
    void teenCellDoubleClicked(int row, int column);
    void commonCellDoubleClicked(int row, int column);
    void eroticCellDoubleClicked(int row, int column);
    void thematicCellDoubleClicked(int row, int column);
    void regionalCellDoubleClicked(int row, int column);
    void createList();
    void buttonClear();
    void showAdvOptions();
    void refresh();
};

#endif // CHANNEL_LIST_GUI_H
