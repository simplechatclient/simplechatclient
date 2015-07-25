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

#include <QDateTime>
#include <QDesktopWidget>
#include <QKeyEvent>
#include <QMessageBox>
#include <QTimer>
#include "core/core.h"
#include "models/channel_list.h"
#include "core/mainwindow.h"
#include "models/settings.h"
#include "models/utils.h"
#include "channel_list_gui.h"

ChannelListGui::ChannelListGui(QWidget *parent) : QDialog(parent)
{
    ui.setupUi(this);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowTitle(tr("Channel list"));
    // center screen
    move(QApplication::desktop()->screenGeometry(QApplication::desktop()->screenNumber(parent)).center()  - rect().center());

    createGui();
    setDefaultValues();
    createSignals();

    createList();
}

void ChannelListGui::createGui()
{
    ui.pushButton_search->setIcon(QIcon(":/images/breeze/edit-find.svg"));
    ui.pushButton_clear->setIcon(QIcon(":/images/breeze/draw-eraser.svg"));
    ui.buttonBox->button(QDialogButtonBox::Retry)->setIcon(QIcon(":/images/breeze/view-refresh.svg"));
    ui.buttonBox->button(QDialogButtonBox::Close)->setIcon(QIcon(":/images/breeze/dialog-close.svg"));

    ui.groupBox_search->setTitle(tr("Search"));
    ui.groupBox_type->setTitle(tr("Type"));
    ui.groupBox_category->setTitle(tr("Category"));

    ui.label_channel_name->setText(tr("Channel name:"));
    ui.pushButton_search->setText(tr("Search"));
    ui.pushButton_clear->setText(tr("Clear"));
    ui.checkBox_hide_empty_channels->setText(tr("Hide empty channels"));
    ui.checkBox_show_adv_options->setText(tr("Show advanced"));

    ui.checkBox_teen->setText(tr("Teen"));
    ui.checkBox_common->setText(tr("Common"));
    ui.checkBox_erotic->setText(tr("Erotic"));
    ui.checkBox_thematic->setText(tr("Thematic"));
    ui.checkBox_regional->setText(tr("Regional"));

    ui.checkBox_wild->setText(tr("Wild"));
    ui.checkBox_tame->setText(tr("Tame"));
    ui.checkBox_with_class->setText(tr("Classy"));
    ui.checkBox_cult->setText(tr("Cult"));
    ui.checkBox_moderated->setText(tr("Moderated"));
    ui.checkBox_recommended->setText(tr("Recommended"));

    ui.tabWidget->setTabText(0, tr("All"));
    ui.tabWidget->setTabText(1, tr("Teen"));
    ui.tabWidget->setTabText(2, tr("Common"));
    ui.tabWidget->setTabText(3, tr("Erotic"));
    ui.tabWidget->setTabText(4, tr("Thematic"));
    ui.tabWidget->setTabText(5, tr("Regional"));

    ui.tabWidget->setTabIcon(0, QIcon(":/images/breeze/accessories-dictionary.svg"));
    ui.tabWidget->setTabIcon(1, QIcon(":/images/breeze/applications-education-university.svg"));
    ui.tabWidget->setTabIcon(2, QIcon(":/images/breeze/system-users.svg"));
    ui.tabWidget->setTabIcon(3, QIcon(":/images/breeze/taxes-finances.svg"));
    ui.tabWidget->setTabIcon(4, QIcon(":/images/breeze/karbon.svg"));
    ui.tabWidget->setTabIcon(5, QIcon(":/images/breeze/applications-education-language.svg"));

    ui.buttonBox->button(QDialogButtonBox::Retry)->setText(tr("Refresh"));
    ui.buttonBox->button(QDialogButtonBox::Close)->setText(tr("Close"));

    Core::instance()->mainWindow()->toolButtonFix(ui.tabWidget);
}

void ChannelListGui::refresh()
{
    ChannelList::instance()->clear();
    Core::instance()->network->send("SLIST  R- 0 0 100 null");
    createList();
}

void ChannelListGui::setDefaultValues()
{
    // clear
    clearFields();
    clearLists();

    // hide adv options
    ui.groupBox_type->hide();
    ui.groupBox_category->hide();

    // need refresh
    qint64 iCheckRefresh = QDateTime::currentMSecsSinceEpoch() - ChannelList::instance()->getTime();
    if ((ChannelList::instance()->getStatus() == StatusCompleted) && (iCheckRefresh > 3600000)) // 3600
    {
        ChannelList::instance()->clear();
        Core::instance()->network->send("SLIST  R- 0 0 100 null");
    }
}

void ChannelListGui::createSignals()
{
    connect(ui.tableWidget_all, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(allCellDoubleClicked(int,int)));
    connect(ui.tableWidget_teen, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(teenCellDoubleClicked(int,int)));
    connect(ui.tableWidget_common, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(commonCellDoubleClicked(int,int)));
    connect(ui.tableWidget_erotic, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(eroticCellDoubleClicked(int,int)));
    connect(ui.tableWidget_thematic, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(thematicCellDoubleClicked(int,int)));
    connect(ui.tableWidget_regional, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(regionalCellDoubleClicked(int,int)));
    connect(ui.buttonBox, SIGNAL(accepted()), this, SLOT(refresh()));
    connect(ui.buttonBox, SIGNAL(rejected()), this, SLOT(close()));

    connect(ui.lineEdit_search, SIGNAL(returnPressed()), this, SLOT(createList()));
    connect(ui.lineEdit_search, SIGNAL(textChanged(QString)), this, SLOT(createList()));
    connect(ui.pushButton_search, SIGNAL(clicked()), this, SLOT(createList()));
    connect(ui.pushButton_clear, SIGNAL(clicked()), this, SLOT(buttonClear()));
    connect(ui.checkBox_hide_empty_channels, SIGNAL(clicked()), this, SLOT(createList()));
    connect(ui.checkBox_show_adv_options, SIGNAL(clicked()), this, SLOT(showAdvOptions()));

    connect(ui.checkBox_teen, SIGNAL(clicked()), this, SLOT(createList()));
    connect(ui.checkBox_common, SIGNAL(clicked()), this, SLOT(createList()));
    connect(ui.checkBox_erotic, SIGNAL(clicked()), this, SLOT(createList()));
    connect(ui.checkBox_thematic, SIGNAL(clicked()), this, SLOT(createList()));
    connect(ui.checkBox_regional, SIGNAL(clicked()), this, SLOT(createList()));

    connect(ui.checkBox_wild, SIGNAL(clicked()), this, SLOT(createList()));
    connect(ui.checkBox_tame, SIGNAL(clicked()), this, SLOT(createList()));
    connect(ui.checkBox_with_class, SIGNAL(clicked()), this, SLOT(createList()));
    connect(ui.checkBox_cult, SIGNAL(clicked()), this, SLOT(createList()));
    connect(ui.checkBox_moderated, SIGNAL(clicked()), this, SLOT(createList()));
    connect(ui.checkBox_recommended, SIGNAL(clicked()), this, SLOT(createList()));
}

bool ChannelListGui::showChannel(const QString &name, int people, int cat, int type, bool moderated, bool recommended)
{
    if ((people == 0) && (bHideEmpty))
        return false;

    if ((type == 1) && (!bShowTeen))
        return false;
    if ((type == 2) && (!bShowCommon))
        return false;
    if ((type == 3) && (!bShowErotic))
        return false;
    if ((type == 4) && (!bShowThematic))
        return false;
    if ((type == 5) && (!bShowRegional))
        return false;

    if ((cat == 0) && (!bShowWild))
    {
        if (!(((bShowModerated) && (moderated)) || ((bShowRecommended) && (recommended))))
            return false;
    }
    if ((cat == 1) && (!bShowTame))
    {
        if (!(((bShowModerated) && (moderated)) || ((bShowRecommended) && (recommended))))
            return false;
    }
    if ((cat == 2) && (!bShowWithClass))
    {
        if (!(((bShowModerated) && (moderated)) || ((bShowRecommended) && (recommended))))
            return false;
    }
    if ((cat == 3) && (!bShowCult))
    {
        if (!(((bShowModerated) && (moderated)) || ((bShowRecommended) && (recommended))))
            return false;
    }

    if (!strSearch.isEmpty())
    {
        if (name.toLower().contains(strSearch.toLower()))
            return true;
        else
            return false;
    }
    else
        return true;
}

void ChannelListGui::clearFields()
{
    ui.checkBox_teen->setChecked(true);
    ui.checkBox_common->setChecked(true);
    ui.checkBox_erotic->setChecked(true);
    ui.checkBox_thematic->setChecked(true);
    ui.checkBox_regional->setChecked(true);

    ui.checkBox_wild->setChecked(true);
    ui.checkBox_tame->setChecked(true);
    ui.checkBox_with_class->setChecked(true);
    ui.checkBox_cult->setChecked(true);
    ui.checkBox_moderated->setChecked(true);
    ui.checkBox_recommended->setChecked(true);

    ui.checkBox_hide_empty_channels->setChecked(false);
}

void ChannelListGui::clearLists()
{
    QStringList lLabels;
    lLabels << tr("Channel name") << tr("Number of persons") << tr("Category") << tr("Type");

    QList<QTableWidget *> tables = ui.tabWidget->findChildren<QTableWidget *>();
    foreach (QTableWidget *table, tables) {
        table->clear();
        table->setRowCount(0);
        table->setColumnWidth(0, 200);
        table->setHorizontalHeaderLabels(lLabels);
    }
}

void ChannelListGui::createList()
{
    // ready
    if (ChannelList::instance()->getStatus() != StatusCompleted)
    {
        QTimer::singleShot(1000, this, SLOT(createList())); // 1 sec
        return;
    }

    // focus
    ui.lineEdit_search->setFocus();

    // get options
    getOptions();

    // clear lists
    clearLists();

    QList<QTableWidget *> tables = ui.tabWidget->findChildren<QTableWidget *>();
    foreach (QTableWidget *table, tables) {
        table->setSortingEnabled(false);
    }

    // count rows
    int iAllCount = 0;
    int iTeenCount = 0;
    int iCommonCount = 0;
    int iEroticCount = 0;
    int iThematicCount = 0;
    int iRegionalCount = 0;

    QList<OnetChannelList> list1 = ChannelList::instance()->getAll();
    foreach (const OnetChannelList &oChannelList, list1)
    {
        QString strName = oChannelList.name;
        int iPeople = oChannelList.people;
        int iCat = oChannelList.cat;
        int iType = oChannelList.type;
        bool bModerated = oChannelList.moderated;
        bool bRecommended = oChannelList.recommended;

        bool bShow = showChannel(strName, iPeople, iCat, iType, bModerated, bRecommended);

        if (bShow)
        {
            iAllCount++;
            if (iType == 1)
                iTeenCount++;
            else if (iType == 2)
                iCommonCount++;
            else if (iType == 3)
                iEroticCount++;
            else if (iType == 4)
                iThematicCount++;
            else if (iType == 5)
                iRegionalCount++;
        }
    }

    ui.tableWidget_all->setRowCount(iAllCount);
    ui.tableWidget_teen->setRowCount(iTeenCount);
    ui.tableWidget_common->setRowCount(iCommonCount);
    ui.tableWidget_erotic->setRowCount(iEroticCount);
    ui.tableWidget_thematic->setRowCount(iThematicCount);
    ui.tableWidget_regional->setRowCount(iRegionalCount);

    // table counters
    int iAllRow = 0;
    int iTeenRow = 0;
    int iCommonRow = 0;
    int iEroticRow = 0;
    int iThematicRow = 0;
    int iRegionalRow = 0;

    QList<OnetChannelList> list2 = ChannelList::instance()->getAll();
    foreach (const OnetChannelList &oChannelList, list2)
    {
        QString strName = oChannelList.name;
        int iPeople = oChannelList.people;
        int iCat = oChannelList.cat;
        int iType = oChannelList.type;
        bool bModerated = oChannelList.moderated;
        bool bRecommended = oChannelList.recommended;

        bool bShow = showChannel(strName, iPeople, iCat, iType, bModerated, bRecommended);

        if (bShow)
        {
            ui.tableWidget_all->setItem(iAllRow, 0, new QTableWidgetItem(strName));
            ui.tableWidget_all->setItem(iAllRow, 1, new QTableWidgetItem());
            ui.tableWidget_all->item(iAllRow, 1)->setData(Qt::DisplayRole, iPeople);
            ui.tableWidget_all->setItem(iAllRow, 2, new QTableWidgetItem(Utils::instance()->convertChannelCatToString(iCat, bModerated, bRecommended)));
            ui.tableWidget_all->setItem(iAllRow, 3, new QTableWidgetItem(Utils::instance()->convertChannelTypeToString(iType)));
            iAllRow++;

            if (iType == 1)
            {
                ui.tableWidget_teen->setItem(iTeenRow, 0, new QTableWidgetItem(strName));
                ui.tableWidget_teen->setItem(iTeenRow, 1, new QTableWidgetItem());
                ui.tableWidget_teen->item(iTeenRow, 1)->setData(Qt::DisplayRole, iPeople);
                ui.tableWidget_teen->setItem(iTeenRow, 2, new QTableWidgetItem(Utils::instance()->convertChannelCatToString(iCat, bModerated, bRecommended)));
                ui.tableWidget_teen->setItem(iTeenRow, 3, new QTableWidgetItem(Utils::instance()->convertChannelTypeToString(iType)));
                iTeenRow++;
            }
            else if (iType == 2)
            {
                ui.tableWidget_common->setItem(iCommonRow, 0, new QTableWidgetItem(strName));
                ui.tableWidget_common->setItem(iCommonRow, 1, new QTableWidgetItem());
                ui.tableWidget_common->item(iCommonRow, 1)->setData(Qt::DisplayRole, iPeople);
                ui.tableWidget_common->setItem(iCommonRow, 2, new QTableWidgetItem(Utils::instance()->convertChannelCatToString(iCat, bModerated, bRecommended)));
                ui.tableWidget_common->setItem(iCommonRow, 3, new QTableWidgetItem(Utils::instance()->convertChannelTypeToString(iType)));
                iCommonRow++;
            }
            else if (iType == 3)
            {
                ui.tableWidget_erotic->setItem(iEroticRow, 0, new QTableWidgetItem(strName));
                ui.tableWidget_erotic->setItem(iEroticRow, 1, new QTableWidgetItem());
                ui.tableWidget_erotic->item(iEroticRow, 1)->setData(Qt::DisplayRole, iPeople);
                ui.tableWidget_erotic->setItem(iEroticRow, 2, new QTableWidgetItem(Utils::instance()->convertChannelCatToString(iCat, bModerated, bRecommended)));
                ui.tableWidget_erotic->setItem(iEroticRow, 3, new QTableWidgetItem(Utils::instance()->convertChannelTypeToString(iType)));
                iEroticRow++;
            }
            else if (iType == 4)
            {
                ui.tableWidget_thematic->setItem(iThematicRow, 0, new QTableWidgetItem(strName));
                ui.tableWidget_thematic->setItem(iThematicRow, 1, new QTableWidgetItem());
                ui.tableWidget_thematic->item(iThematicRow, 1)->setData(Qt::DisplayRole, iPeople);
                ui.tableWidget_thematic->setItem(iThematicRow, 2, new QTableWidgetItem(Utils::instance()->convertChannelCatToString(iCat, bModerated, bRecommended)));
                ui.tableWidget_thematic->setItem(iThematicRow, 3, new QTableWidgetItem(Utils::instance()->convertChannelTypeToString(iType)));
                iThematicRow++;
            }
            else if (iType == 5)
            {
                ui.tableWidget_regional->setItem(iRegionalRow, 0, new QTableWidgetItem(strName));
                ui.tableWidget_regional->setItem(iRegionalRow, 1, new QTableWidgetItem());
                ui.tableWidget_regional->item(iRegionalRow, 1)->setData(Qt::DisplayRole, iPeople);
                ui.tableWidget_regional->setItem(iRegionalRow, 2, new QTableWidgetItem(Utils::instance()->convertChannelCatToString(iCat, bModerated, bRecommended)));
                ui.tableWidget_regional->setItem(iRegionalRow, 3, new QTableWidgetItem(Utils::instance()->convertChannelTypeToString(iType)));
                iRegionalRow++;
            }
        }
    }

    foreach (QTableWidget *table, tables) {
        table->sortByColumn(1, Qt::DescendingOrder);
        table->setSortingEnabled(true);
    }
}

void ChannelListGui::getOptions()
{
    // search
    strSearch = ui.lineEdit_search->text().trimmed();

    // teen
    bShowTeen = ui.checkBox_teen->isChecked();

    // common
    bShowCommon = ui.checkBox_common->isChecked();

    // erotic
    bShowErotic = ui.checkBox_erotic->isChecked();

    // thematic
    bShowThematic = ui.checkBox_thematic->isChecked();

    // regional
    bShowRegional = ui.checkBox_regional->isChecked();

    // wild
    bShowWild = ui.checkBox_wild->isChecked();

    // tame
    bShowTame = ui.checkBox_tame->isChecked();

    // classy
    bShowWithClass = ui.checkBox_with_class->isChecked();

    // cult
    bShowCult = ui.checkBox_cult->isChecked();

    // moderated
    bShowModerated = ui.checkBox_moderated->isChecked();

    // recommended
    bShowRecommended = ui.checkBox_recommended->isChecked();

    // hide empty
    bHideEmpty = ui.checkBox_hide_empty_channels->isChecked();
}

void ChannelListGui::allCellDoubleClicked(int row, int column)
{
    Q_UNUSED (column);

    QString strChannel = ui.tableWidget_all->item(row, 0)->text();

    if (Utils::instance()->isErotic(strChannel))
    {
        if (Settings::instance()->getBool("age_check"))
        {
            QMessageBox msgBox;
            msgBox.setStyleSheet("background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:0.557, stop:0 rgba(198, 0, 0, 255), stop:1 rgba(255, 0, 0, 255));");
            msgBox.setIcon(QMessageBox::Warning);
            msgBox.setWindowIcon(QIcon(":/images/logo16x16.png"));
            msgBox.setWindowTitle(tr("Warning"));
            msgBox.setText(QString("%1\n%2").arg(tr("Erotic category may contain content intended only for adults."), tr("To enter you must be over 18 years.")));
            QPushButton *exitButton = msgBox.addButton(tr("Exit"), QMessageBox::AcceptRole);
            exitButton->setIcon(QIcon(":/images/breeze/dialog-cancel.svg"));
            QPushButton *enterButton = msgBox.addButton(tr("Enter"), QMessageBox::RejectRole);
            enterButton->setIcon(QIcon(":/images/breeze/dialog-ok.svg"));
            msgBox.exec();

            if (msgBox.clickedButton() == enterButton)
                Settings::instance()->setBool("age_check", false);
            else
                return;
        }
    }
    Core::instance()->network->send(QString("JOIN %1").arg(strChannel));

    this->close();
}

void ChannelListGui::teenCellDoubleClicked(int row, int column)
{
    Q_UNUSED (column);

    QString strChannel = ui.tableWidget_teen->item(row, 0)->text();
    Core::instance()->network->send(QString("JOIN %1").arg(strChannel));

    this->close();
}

void ChannelListGui::commonCellDoubleClicked(int row, int column)
{
    Q_UNUSED (column);

    QString strChannel = ui.tableWidget_common->item(row, 0)->text();
    Core::instance()->network->send(QString("JOIN %1").arg(strChannel));

    this->close();
}

void ChannelListGui::eroticCellDoubleClicked(int row, int column)
{
    Q_UNUSED (column);

    QString strChannel = ui.tableWidget_erotic->item(row, 0)->text();

    if (Utils::instance()->isErotic(strChannel))
    {
        if (Settings::instance()->getBool("age_check"))
        {
            QMessageBox msgBox;
            msgBox.setStyleSheet("background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:0.557, stop:0 rgba(198, 0, 0, 255), stop:1 rgba(255, 0, 0, 255));");
            msgBox.setIcon(QMessageBox::Warning);
            msgBox.setWindowIcon(QIcon(":/images/logo16x16.png"));
            msgBox.setWindowTitle(tr("Warning"));
            msgBox.setText(QString("%1\n%2").arg(tr("Erotic category may contain content intended only for adults."), tr("To enter you must be over 18 years.")));
            QPushButton *exitButton = msgBox.addButton(tr("Exit"), QMessageBox::AcceptRole);
            exitButton->setIcon(QIcon(":/images/breeze/dialog-cancel.svg"));
            QPushButton *enterButton = msgBox.addButton(tr("Enter"), QMessageBox::RejectRole);
            enterButton->setIcon(QIcon(":/images/breeze/dialog-ok.svg"));
            msgBox.exec();

            if (msgBox.clickedButton() == enterButton)
            {
                Settings::instance()->setBool("age_check", false);
                Core::instance()->network->send(QString("JOIN %1").arg(strChannel));
            }
        }
        else
        {
            Core::instance()->network->send(QString("JOIN %1").arg(strChannel));

            this->close();
        }
    }
    else
    {
        Core::instance()->network->send(QString("JOIN %1").arg(strChannel));

        this->close();
    }
}

void ChannelListGui::thematicCellDoubleClicked(int row, int column)
{
    Q_UNUSED (column);

    QString strChannel = ui.tableWidget_thematic->item(row, 0)->text();
    Core::instance()->network->send(QString("JOIN %1").arg(strChannel));

    this->close();
}

void ChannelListGui::regionalCellDoubleClicked(int row, int column)
{
    Q_UNUSED (column);

    QString strChannel = ui.tableWidget_regional->item(row, 0)->text();
    Core::instance()->network->send(QString("JOIN %1").arg(strChannel));

    this->close();
}

void ChannelListGui::buttonClear()
{
    clearFields();

    ui.lineEdit_search->clear();

    createList();
}

void ChannelListGui::showAdvOptions()
{
    if (ui.checkBox_show_adv_options->isChecked())
    {
        ui.groupBox_type->show();
        ui.groupBox_category->show();
    }
    else
    {
        ui.groupBox_type->hide();
        ui.groupBox_category->hide();
    }
}

void ChannelListGui::resizeEvent(QResizeEvent *)
{
    ui.verticalLayoutWidget->setGeometry(QRect(10, 10, this->width()-20, this->height()-20));
    ui.tabWidget->setGeometry(QRect(215, 0, this->width()-235, this->height()-50));
    ui.tableWidget_all->setGeometry(QRect(0, 0, ui.tabWidget->width()-10, ui.tabWidget->height()-35));
    ui.tableWidget_teen->setGeometry(QRect(0, 0, ui.tabWidget->width()-10, ui.tabWidget->height()-35));
    ui.tableWidget_common->setGeometry(QRect(0, 0, ui.tabWidget->width()-10, ui.tabWidget->height()-35));
    ui.tableWidget_erotic->setGeometry(QRect(0, 0, ui.tabWidget->width()-10, ui.tabWidget->height()-35));
    ui.tableWidget_thematic->setGeometry(QRect(0, 0, ui.tabWidget->width()-10, ui.tabWidget->height()-35));
    ui.tableWidget_regional->setGeometry(QRect(0, 0, ui.tabWidget->width()-10, ui.tabWidget->height()-35));
}

void ChannelListGui::keyPressEvent(QKeyEvent *event)
{
    switch (event->key())
    {
        case Qt::Key_Enter:
        case Qt::Key_Return:
            createList();
            break;
        default:
            QDialog::keyPressEvent(event);
    }
}
