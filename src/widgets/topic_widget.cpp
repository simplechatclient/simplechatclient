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

#include <QAbstractTextDocumentLayout>
#include "topic_widget.h"

TopicWidget::TopicWidget()
{
    setReadOnly(true);
    setFrameShape(QFrame::NoFrame);
    setOpenExternalLinks(true);

    connect(this, SIGNAL(textChanged()), this, SLOT(topicTextChanged()));
}

void TopicWidget::refreshTopicHeight()
{
    int height = this->document()->documentLayout()->documentSize().toSize().height();

    this->setMinimumHeight(height);
    this->setMaximumHeight(height);
}

void TopicWidget::topicTextChanged()
{
    this->refreshTopicHeight();
}

void TopicWidget::resizeEvent(QResizeEvent *event)
{
    QTextEdit::resizeEvent(event);

    this->refreshTopicHeight();
}
