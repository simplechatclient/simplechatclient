/*
 * Simple Chat Client
 *
 *   Copyright (C) 2009-2015 Piotr Łuczko <piotr.luczko@gmail.com>
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
#include "common/config.h"
#include "widgets/inputline_widget.h"
#include "emoticons_gui.h"

#ifndef Q_OS_WIN
    #include "scc-config.h"
#endif

EmoticonsThread::EmoticonsThread() {}

void EmoticonsThread::setDir(const QString &_strDir)
{
    strDir = _strDir;
}

void EmoticonsThread::run()
{
    QDir dEmoticonsDir = strDir;

    QStringList lSupportedEmoticons;
    lSupportedEmoticons << "*.gif" << "*.jpg" << "*.jpeg" << "*.png" << "*.bmp";

    QStringList lFiles = dEmoticonsDir.entryList(lSupportedEmoticons, QDir::Files | QDir::NoSymLinks, QDir::Name | QDir::IgnoreCase);

    foreach (const QString &strFileName, lFiles)
    {
        QString strEmoticon = QFileInfo(strFileName).baseName();

        QByteArray bData;
        QFile f(strDir+"/"+strFileName);
        if (f.open(QIODevice::ReadOnly))
        {
            bData = f.readAll();
            f.close();
        }

        if (!bData.isEmpty())
            emit addEmoticon("//"+strEmoticon, bData);
    }
    emit sortEmoticons();

    exec();
}

EmoticonsTabGui::EmoticonsTabGui(const QString &_strDir, QWidget *parent) : QWidget(parent)
{
    listWidget = new QListWidget(this);
    listWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    listWidget->setDragDropMode(QAbstractItemView::NoDragDrop);
    listWidget->setViewMode(QListView::IconMode);
    listWidget->setMovement(QListView::Static);
    listWidget->setResizeMode(QListView::Adjust);
    listWidget->show();

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(listWidget);
    setLayout(mainLayout);

    connect(&thread, SIGNAL(addEmoticon(const QString&, const QByteArray&)), this, SLOT(addEmoticon(const QString&, const QByteArray&)));
    connect(&thread, SIGNAL(sortEmoticons()), this, SLOT(sortEmoticons()));

    thread.setDir(_strDir);
}

void EmoticonsTabGui::addEmoticon(const QString &strEmoticon, const QByteArray &bData)
{
    QPixmap pix;
    if (!bData.isEmpty())
        pix.loadFromData(bData);

    QListWidgetItem *item = new QListWidgetItem;
    item->setIcon(QIcon(pix));
    item->setData(EmoticonNameRole, strEmoticon);
    item->setToolTip(strEmoticon);

    listWidget->addItem(item);
}

void EmoticonsTabGui::sortEmoticons()
{
    listWidget->setSortingEnabled(true);
}

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

    // default read first tab
    if (ui.tabWidget->count() != 0)
        tabChanged(0);
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
    QString path;
#ifdef Q_OS_WIN
    path = QCoreApplication::applicationDirPath();
#else
    path = SCC_DATA_DIR;
#endif

    QStringList lSupportedEmoticons;
    lSupportedEmoticons << "*.gif" << "*.jpg" << "*.jpeg" << "*.png" << "*.bmp";

    QDir dAllEmoticonsDirs = path+"/emoticons/";
    QStringList lDirs = dAllEmoticonsDirs.entryList(QStringList("*"), QDir::Dirs | QDir::NoDotAndDotDot | QDir::NoSymLinks, QDir::Name | QDir::IgnoreCase);

    foreach (const QString &strDir, lDirs)
    {
        QDir dEmoticonsDir = dAllEmoticonsDirs.absolutePath()+"/"+strDir;

        QStringList lFiles = dEmoticonsDir.entryList(lSupportedEmoticons, QDir::Files | QDir::NoSymLinks, QDir::Name | QDir::IgnoreCase);
        QString strFirstIconPath = dEmoticonsDir.absolutePath()+"/"+lFiles.first();

        ui.tabWidget->addTab(new EmoticonsTabGui(dEmoticonsDir.absolutePath()), QIcon(strFirstIconPath), strDir);
    }
}

void EmoticonsGui::createSignals()
{
    connect(ui.tabWidget, SIGNAL(currentChanged(int)), this, SLOT(tabChanged(int)));
    connect(ui.pushButton_insert, SIGNAL(clicked()), this, SLOT(buttonInsert()));
    connect(ui.buttonBox, SIGNAL(rejected()), this, SLOT(close()));

    for (int i = 0; i < ui.tabWidget->count(); ++i)
    {
        EmoticonsTabGui *tab = (EmoticonsTabGui *)ui.tabWidget->widget(i);
        connect(tab->listWidget, SIGNAL(clicked(QModelIndex)), this, SLOT(buttonInsert()));
    }
}

EmoticonsGui::~EmoticonsGui()
{
    for (int i = 0; i < ui.tabWidget->count(); ++i)
    {
        EmoticonsTabGui *tab = (EmoticonsTabGui *)ui.tabWidget->widget(i);
        tab->thread.quit();
        tab->thread.wait();
        tab->thread.deleteLater();
    }
}

void EmoticonsGui::tabChanged(int index)
{
    if (!lReadedTabIndex.contains(index))
    {
        lReadedTabIndex.append(index);

        EmoticonsTabGui *tab = (EmoticonsTabGui *)ui.tabWidget->widget(index);
        tab->thread.start();
    }
}

void EmoticonsGui::buttonInsert()
{
    if (ui.tabWidget->count() == 0) // not found tabs
        return;

    EmoticonsTabGui *tab = (EmoticonsTabGui *)ui.tabWidget->currentWidget();

    if (tab->listWidget->selectedItems().size() == 0)
        return;

    QString strEmoticon = tab->listWidget->selectedItems().at(0)->data(EmoticonNameRole).toString();

    // insert
    pInputLineWidget->insertText(strEmoticon);

    // close
    this->close();
}
