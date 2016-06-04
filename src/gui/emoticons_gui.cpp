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

#include <QDesktopWidget>
#include <QDir>
#include <QListWidget>
#include <QPixmap>
#include <QResizeEvent>
#include <QVBoxLayout>
#include "core/defines.h"
#include "common/config.h"
#include "models/emoticons.h"
#include "widgets/inputline_widget.h"
#include "emoticons_gui.h"

#ifndef Q_OS_WIN
    #include "scc-config.h"
#endif

EmoticonsGui::EmoticonsGui(InputLineWidget *_pInputLineWidget, QWidget *parent) : QDialog(parent), pInputLineWidget(_pInputLineWidget)
{
    ui.setupUi(this);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowTitle(tr("Emoticons"));
    // center screen
    move(QApplication::desktop()->screenGeometry(QApplication::desktop()->screenNumber(parent)).center()  - rect().center());

    createGui();
    setDefaultValues();
    createSignals();

    // select first category
    if (ui.listWidget_categories->count() > 0) {
        ui.listWidget_categories->setCurrentRow(0);
        categoryChanged(ui.listWidget_categories->item(0));
    }
}

void EmoticonsGui::createGui()
{
    ui.pushButton_insert->setIcon(QIcon(":/images/breeze/insert-image.svg"));
    ui.buttonBox->button(QDialogButtonBox::Close)->setIcon(QIcon(":/images/breeze/dialog-close.svg"));

    ui.pushButton_insert->setText(tr("Insert"));
    ui.buttonBox->button(QDialogButtonBox::Close)->setText(tr("Close"));
}

void EmoticonsGui::setDefaultValues()
{
    createCategoriesList();
}

void EmoticonsGui::createSignals()
{
    connect(ui.listWidget_emoticons, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(buttonInsert()));
    connect(ui.listWidget_categories, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(categoryChanged(QListWidgetItem*)));
    connect(ui.pushButton_insert, SIGNAL(clicked()), this, SLOT(buttonInsert()));
    connect(ui.buttonBox, SIGNAL(rejected()), this, SLOT(close()));
}

void EmoticonsGui::createCategoriesList()
{
    QList<EmoticonCategories> lEmoticonsCategoriesList = Emoticons::instance()->listCategories();
    foreach (EmoticonCategories emoticonsCategory, lEmoticonsCategoriesList) {

        QListWidgetItem *item = new QListWidgetItem(ui.listWidget_categories);
        item->setIcon(QIcon(emoticonsCategory.imagePath));
        item->setText(emoticonsCategory.name);
        item->setData(EmoticonDirectoryRole, emoticonsCategory.dir);
    }
}

void EmoticonsGui::categoryChanged(QListWidgetItem *item)
{
    ui.listWidget_emoticons->clear();

    QString path = item->data(EmoticonDirectoryRole).toString();
    displayEmoticons(path);
}

void EmoticonsGui::displayEmoticons(const QString &path)
{
    QList<Emoticon> lEmoticons = Emoticons::instance()->listEmoticonsFromPath(path);

    foreach (Emoticon eEmoticon, lEmoticons) {
        QListWidgetItem *item = new QListWidgetItem;
        item->setIcon(QIcon(eEmoticon.path));
        item->setData(Qt::UserRole, eEmoticon.nameWithPrefix);
        item->setToolTip(eEmoticon.nameWithPrefix);

        ui.listWidget_emoticons->addItem(item);
    }

//    ui.listWidget_emoticons->setSortingEnabled(true);
}

void EmoticonsGui::buttonInsert()
{
    if (ui.listWidget_emoticons->selectedItems().size() == 0)
        return;

    QString strEmoticon = ui.listWidget_emoticons->selectedItems().at(0)->data(Qt::UserRole).toString();

    // insert
    pInputLineWidget->insertText(strEmoticon);

    // close
    this->close();
}
