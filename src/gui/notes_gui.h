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

#ifndef NOTES_GUI_H
#define NOTES_GUI_H

#include <QDialog>
#include "ui_notes.h"

class NotesGui : public QDialog
{
    Q_OBJECT
public:
    NotesGui(QWidget *parent = 0);

private:
    Ui::uiNotes ui;

    void createGui();
    void createSignals();

    void read();
    void save();

private slots:
    void buttonOk();

protected:
    virtual void resizeEvent(QResizeEvent *);
};

#endif // NOTES_GUI_H
