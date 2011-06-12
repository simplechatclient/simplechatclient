/****************************************************************************
 *                                                                          *
 *   This file is part of Simple Chat Client                                *
 *   Copyright (C) 2011 Piotr Łuczko <piotr.luczko@gmail.com>               *
 *                                                                          *
 *   This program is free software: you can redistribute it and/or modify   *
 *   it under the terms of the GNU General Public License as published by   *
 *   the Free Software Foundation, either version 3 of the License, or      *
 *   (at your option) any later version.                                    *
 *                                                                          *
 *   This program is distributed in the hope that it will be useful,        *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *   GNU General Public License for more details.                           *
 *                                                                          *
 *   You should have received a copy of the GNU General Public License      *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 *                                                                          *
 ****************************************************************************/

#include <QEvent>
#include <QKeyEvent>
#include "inputlinewidget.h"

InputLineWidget::InputLineWidget(QWidget *parent) : QLineEdit(parent)
{
    index = 0;
    strLastWord = QString::null;
}

InputLineWidget::~InputLineWidget()
{
    strLastWord = QString::null;
    usersList.clear();
}

void InputLineWidget::set_userslist(QList<QString> newUsersList)
{
    usersList = newUsersList;
}

void InputLineWidget::insert_text(QString strText)
{
    // pos
    int iPos = this->cursorPosition();

    // insert text
    QString strLine = this->text();
    strLine.insert(iPos, strText);
    this->setText(strLine);
}

QString InputLineWidget::get_word()
{
    QString strWord = this->text();
    if (!strWord.isEmpty())
    {
        int iLength = 0;
        int iStart = 0;

        // if first word -1, if next -2
        if (this->cursorPosition()-1 == 0)
            iStart = strWord.lastIndexOf(' ', this->cursorPosition()-1)+1;
        else
            iStart = strWord.lastIndexOf(' ', this->cursorPosition()-2)+1;

        iLength = this->cursorPosition()-iStart;
        strWord = strWord.mid(iStart, iLength);

        strWord.remove(" ");
    }
    return strWord;
}

void InputLineWidget::set_word(QString strSetWord)
{
    QString strWord = this->text();
    if (!strWord.isEmpty())
    {
        int iLength = 0;
        int iStart = 0;

        // if first word -1, if next -2
        if (this->cursorPosition()-1 == 0)
            iStart = strWord.lastIndexOf(' ', this->cursorPosition()-1)+1;
        else
            iStart = strWord.lastIndexOf(' ', this->cursorPosition()-2)+1;

        iLength = this->cursorPosition()-iStart;
        strWord = strWord.mid(iStart, iLength);

        strSetWord += " ";

        QString strNewLine = this->text();
        strNewLine = strNewLine.replace(iStart, iLength, strSetWord);
        this->setText(strNewLine);

        // set cursor
        this->setCursorPosition(iStart+strSetWord.length());
    }
}

bool InputLineWidget::event(QEvent *e)
{
    if (e->type() != QEvent::KeyPress)
        return QLineEdit::event(e);

    QKeyEvent *k = static_cast<QKeyEvent*>(e);

    if (k->key() == Qt::Key_Tab)
    {
        if (k->modifiers() == Qt::ControlModifier)
        {
            emit ctrlTabPressed();
            return true;
        }
        else
        {
            if (usersList.size() == 0)
                return true;

            QString strWord = get_word();

            if (strWord.isEmpty())
                return true;

            if (strLastWord.isEmpty())
            {
                find.clear();
                for (int i = 0; i < usersList.size(); i++)
                {
                    if (usersList.at(i).startsWith(strWord, Qt::CaseInsensitive))
                        find.append(usersList.at(i));
                    if (usersList.at(i).startsWith("~"+strWord, Qt::CaseInsensitive))
                        find.append(usersList.at(i));
                }

                strLastWord = strWord;
            }

            if (!find.isEmpty())
            {
                if (index > find.size())
                    index = 0;

                if (!find.isEmpty())
                {
                    QString strSetWord = find.at(index);
                    set_word(strSetWord);
                }

                index++;
                if (index >= find.size())
                    index = 0;
            }

            return true;
        }
    }
    else if ((k->key() == Qt::Key_Backtab) && (k->modifiers() == (Qt::ControlModifier | Qt::ShiftModifier)))
    {
        emit ctrlShiftTabPressed();
        return true;
    }
    else if ((k->key() == Qt::Key_Enter) || (k->key() == Qt::Key_Return))
    {
        strLastMsg = this->text().simplified();
        emit returnPressed();
        return true;
    }
    else if (k->key() == Qt::Key_Up)
    {
        // text
        this->setText(strLastMsg);

        // cursor
        this->setCursorPosition(strLastMsg.size());

        return true;
    }
    else if (k->key() == Qt::Key_Down)
    {
        clear();
        return true;
    }
    else
    {
        index = 0;
        strLastWord = QString::null;
        return QLineEdit::event(e);
    }
}
