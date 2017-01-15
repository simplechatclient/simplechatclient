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

#include <QDesktopWidget>
#include "widgets/chat_view.h"
#include "find_text_gui.h"

FindTextGui::FindTextGui(ChatView *_chatView, QWidget *parent) : QDialog(parent), chatView(_chatView)
{
    ui.setupUi(this);
    setWindowFlags(Qt::Dialog | Qt::WindowStaysOnTopHint);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowTitle(tr("Find"));
    // center screen
    move(QApplication::desktop()->screenGeometry(QApplication::desktop()->screenNumber(parent)).center()  - rect().center());

    createGui();
    createSignals();
}

void FindTextGui::createGui()
{
    ui.label_find->setText(tr("Find what:"));
    ui.checkBox_highlight_all_occurrences->setText(tr("Highlight all occurrences"));
    ui.checkBox_match_case->setText(tr("Match case"));
    ui.groupBox_direction->setTitle(tr("Direction"));
    ui.radioButton_direction_up->setText(tr("Up"));
    ui.radioButton_direction_down->setText(tr("Down"));
    ui.pushButton_find_next->setText(tr("Find next"));
    ui.pushButton_close->setText(tr("Close"));

    ui.pushButton_find_next->setIcon(QIcon(":/images/breeze/edit-find.svg"));
    ui.pushButton_close->setIcon(QIcon(":/images/breeze/dialog-close.svg"));

    ui.radioButton_direction_up->setChecked(true);
}

void FindTextGui::createSignals()
{
    connect(ui.pushButton_find_next, SIGNAL(clicked()), this, SLOT(buttonFindNext()));
    connect(ui.pushButton_close, SIGNAL(clicked()), this, SLOT(close()));
}

void FindTextGui::buttonFindNext()
{
    QString strText = ui.lineEdit_find->text().trimmed();
    bool bHighlightAllOccurrences = ui.checkBox_highlight_all_occurrences->isChecked();
    bool bMatchCase = ui.checkBox_match_case->isChecked();
    bool bDirectionUp = ui.radioButton_direction_up->isChecked();

    QWebPage::FindFlags findFlags;
    if (bHighlightAllOccurrences) findFlags |= QWebPage::HighlightAllOccurrences;
    if (bMatchCase) findFlags |= QWebPage::FindCaseSensitively;
    if (bDirectionUp) findFlags |= QWebPage::FindBackward;

    chatView->findText(QString(), QWebPage::HighlightAllOccurrences);
    chatView->findText(strText, findFlags);
}
