/*
 * Simple Chat Client
 *
 *   Copyright (C) 2012-2015 Dariusz Markowicz <dmarkowicz@alari.pl>
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

#include <QDebug>
#include <QDomDocument>
#include <QFileDialog>

#include "gui/avatar_edit_gui.h"

#include "models/settings.h"
#include "avatar_list_widget.h"
#include "ui_avatar_list_widget.h"

AvatarListWidget::AvatarListWidget(QWidget *parent) : QWidget(parent), onetAvatar(0)
{
    ui.setupUi(this);

    createGui();
}

void AvatarListWidget::initialize(OnetAvatar *_onetAvatar)
{
    // let's make it simple, you can init widget only once
    if (onetAvatar || _onetAvatar == 0)
        return;

    createSignals();

    onetAvatar = _onetAvatar;

    connect(onetAvatar, SIGNAL(getCollectionsReady(const QByteArray &)),
            this, SLOT(getCollectionsReady(const QByteArray &)));
    connect(onetAvatar, SIGNAL(getCollectionAvatarsReady(const QByteArray &)),
            this, SLOT(getCollectionAvatarsReady(const QByteArray &)));
    connect(onetAvatar, SIGNAL(getMyAvatarsReady(const QByteArray &)),
            this, SLOT(getMyAvatarsReady(const QByteArray &)));
    connect(onetAvatar, SIGNAL(uploadImageReady(const QByteArray &, const QString &)),
            this, SLOT(uploadImageReady(const QByteArray &, const QString &)));
    connect(onetAvatar, SIGNAL(updatePhotoReady(const QByteArray &)),
            this, SLOT(updatePhotoReady(const QByteArray &)));
    connect(onetAvatar, SIGNAL(addPhotoReady(const QByteArray &)),
            this, SLOT(addPhotoReady(const QByteArray &)));
    connect(onetAvatar, SIGNAL(deletePhotoReady(const QByteArray &)),
            this, SLOT(deletePhotoReady(const QByteArray &)));
    connect(onetAvatar, SIGNAL(getAvatarReady(const QByteArray &,const QString &,OnetAvatar::AvatarType)),
            this, SLOT(getAvatarReady(const QByteArray &,const QString &,OnetAvatar::AvatarType)));

    // load my avatars
    onetAvatar->requestGetMyAvatars();
}

bool AvatarListWidget::isInitialized() const
{
    return onetAvatar != 0;
}

void AvatarListWidget::createGui()
{
    ui.pushButton_add_avatar->setIcon(QIcon(":/images/breeze/list-add.svg"));
    ui.pushButton_edit_avatar->setIcon(QIcon(":/images/breeze/document-edit.svg"));
    ui.pushButton_remove_avatar->setIcon(QIcon(":/images/breeze/list-remove.svg"));
    ui.pushButton_set_empty_avatar1->setIcon(QIcon(":/images/breeze/image-missing.svg"));
    ui.pushButton_apply_avatar->setIcon(QIcon(":/images/breeze/dialog-ok-apply.svg"));
    ui.pushButton_set_empty_avatar2->setIcon(QIcon(":/images/breeze/image-missing.svg"));
    ui.pushButton_apply_collection_avatar->setIcon(QIcon(":/images/breeze/dialog-ok-apply.svg"));

    ui.tabWidget->setTabText(0, tr("My avatars"));
    ui.tabWidget->setTabText(1, tr("Collection of avatars"));
    ui.pushButton_add_avatar->setText(tr("Add"));
    ui.pushButton_edit_avatar->setText(tr("Edit"));
    ui.pushButton_remove_avatar->setText(tr("Remove"));
    ui.pushButton_set_empty_avatar1->setText(tr("Apply empty"));
    ui.pushButton_apply_avatar->setText(tr("Apply avatar"));
    ui.pushButton_set_empty_avatar2->setText(tr("Apply empty"));
    ui.pushButton_apply_collection_avatar->setText(tr("Apply avatar"));
}

void AvatarListWidget::createSignals()
{
    connect(ui.pushButton_add_avatar, SIGNAL(clicked()), this, SLOT(addAvatarClicked()));
    connect(ui.pushButton_edit_avatar, SIGNAL(clicked()), this, SLOT(editAvatarClicked()));
    connect(ui.pushButton_apply_collection_avatar, SIGNAL(clicked()), this, SLOT(setCollectionAvatarClicked()));
    connect(ui.pushButton_apply_avatar, SIGNAL(clicked()), this, SLOT(setMyAvatarClicked()));
    connect(ui.pushButton_remove_avatar, SIGNAL(clicked()), this, SLOT(removeAvatarClicked()));
    connect(ui.pushButton_set_empty_avatar1, SIGNAL(clicked()), this, SLOT(setEmptyAvatarClicked()));
    connect(ui.pushButton_set_empty_avatar2, SIGNAL(clicked()), this, SLOT(setEmptyAvatarClicked()));
    connect(ui.tabWidget, SIGNAL(currentChanged(int)), this, SLOT(tabChanged(int)));
    connect(ui.listWidget_list_collections, SIGNAL(currentTextChanged(QString)), this, SLOT(collectionChanged(QString)));
}

/*!
    OK response:
    \code
        <root>
            <error>
                <code>0</code>
                <text><![CDATA[OK]]></text>
            </error>
            <data>
                <id>5</id>
                <name><![CDATA[Zestaw 1]]></name>
            </data>
            <data>
                <id>6</id>
                <name><![CDATA[Zestaw 2]]></name>
            </data>
            <count>2</count>
            <reqId><![CDATA[{5ed3b996-6faa-4bcb-903d-d4a394235795}]]></reqId>
        </root>
    \endcode

    Error response:
    \code
        <root>
            <error>
                <code>1</code>
                <text><![CDATA[Not logged]]></text>
            </error>
            <reqId><![CDATA[{d1f76dc3-d939-4389-aa5c-4bb428a62363}]]></reqId>
        </root>
    \endcode
*/
void AvatarListWidget::getCollectionsReady(const QByteArray &content)
{
    collections.clear();

    QDomDocument doc;
    doc.setContent(content);
    QDomElement docElem = doc.documentElement();

    // error
    QDomNodeList error = docElem.elementsByTagName("error");
    int code = error.at(0).firstChildElement("code").text().toInt();
    QString text = error.at(0).firstChildElement("text").text();

    if (code != 0)
    {
        if (Settings::instance()->getBool("debug"))
            qWarning() << "Error getCollections: " <<  text;
        return;
    }

    // data
    QDomNodeList nodeList = docElem.elementsByTagName("data");

    for (int i = 0; i < nodeList.size(); ++i)
    {
        QDomElement el = nodeList.at(i).toElement();
        QDomNode pEntries = el.firstChild();

        QString name;
        int id = 0;
        while (!pEntries.isNull())
        {
            QDomElement eData = pEntries.toElement();
            QString tagName = eData.tagName();

            if (tagName == "name")
                name = eData.text();
            else if (tagName == "id")
                id = eData.text().toInt();

            pEntries = pEntries.nextSibling();
        }

        if ((!name.isEmpty()) && (id != 0))
            collections.insert(name, id);
    }

    ui.listWidget_list_collections->clear();
    ui.listWidget_list_collections->insertItems(0, collections.keys());
}

/*!
    OK response:
    \code
        <root>
            <error>
                <code>0</code>
                <text><![CDATA[OK]]></text>
            </error>
            <data>
                <avatars>
                    <avatarId>110</avatarId>
                    <collectId>5</collectId>
                    <fileName><![CDATA[9f78f599c34c97336ae3a501fe29395e]]></fileName>
                    <avType>4</avType>
                    <server><![CDATA[http://foto2.m.onet.pl/_m/]]></server>
                    <img><![CDATA[9f78f599c34c97336ae3a501fe29395e]]></img>
                    <crop><![CDATA[]]></crop>
                    <angle>0</angle>
                    <bbox>0</bbox>
                    <mApp>27</mApp>
                    <ext><![CDATA[jpg]]></ext>
                </avatars>
                <avatarsCnt>27</avatarsCnt>
                <collectId>5</collectId>
            </data>
            <reqId><![CDATA[eae58d2a-f090-4439-9f97-a109a5e2e6f4]]></reqId>
        </root>
    \endcode

    Error response:
    \code
        <root>
            <error>
                <code>-1</code>
                <text><![CDATA[getAvatarsFromCollect: -2|Nie istnieje kolekcja o Id: 1]]></text>
            </error>
            <data><![CDATA[]]></data>
            <reqId><![CDATA[{52bb1aab-c1b0-4e5a-9b06-e46285374251}]]></reqId>
        </root>
    \endcode
*/
void AvatarListWidget::getCollectionAvatarsReady(const QByteArray &content)
{
    QList<QString> lCollectionAvatars;

    QDomDocument doc;
    doc.setContent(content);
    QDomElement docElem = doc.documentElement();

    // error
    QDomNodeList error = docElem.elementsByTagName("error");
    int code = error.at(0).firstChildElement("code").text().toInt();
    QString text = error.at(0).firstChildElement("text").text();

    if (code != 0)
    {
        if (Settings::instance()->getBool("debug"))
            qWarning() << "Error getCollectionAvatars: " << text;
        return;
    }

    // data
    QDomNodeList avatarsNodeList = docElem.elementsByTagName("data");

    for (int n = 0; n < avatarsNodeList.size(); ++n)
    {
        QDomNodeList nodeList = avatarsNodeList.at(n).childNodes();

        for (int c = 0; c < nodeList.size(); ++c)
        {
            QDomElement el = nodeList.at(c).toElement();
            QDomNode pEntries = el.firstChild();

            QString server;
            QString img;
            QString ext;
            QString mApp;
            QString angle;
            while (!pEntries.isNull())
            {
                QString tagName = pEntries.toElement().tagName();

                if (tagName == "server")
                    server = pEntries.toElement().text();
                else if (tagName == "img")
                    img = pEntries.toElement().text();
                else if (tagName == "ext")
                    ext = pEntries.toElement().text();
                else if (tagName == "mApp")
                    mApp = pEntries.toElement().text();
                else if (tagName == "angle")
                    angle = pEntries.toElement().text();

                pEntries = pEntries.nextSibling();
            }

            if ((!server.isEmpty()) && (!img.isEmpty()) && (!ext.isEmpty()))
            {
                QString avatarUrl = QString("%1%2,1,%3,%4.%5").arg(server, img, mApp, angle, ext);
                lCollectionAvatars.append(avatarUrl);
            }
        }
    }

    QPixmap pixmap = QPixmap(50, 50);
    pixmap.fill(Qt::white);

    QIcon icon = QIcon(pixmap);

    ui.listWidget_collections->clear();

    foreach(const QString &avatarUrl, lCollectionAvatars)
    {
        QListWidgetItem *item = new QListWidgetItem();
        item->setData(AvatarUrlRole, avatarUrl);
        item->setIcon(icon);

        // add blank avatar placeholder
        ui.listWidget_collections->addItem(item);
        // get avatar for current position
        onetAvatar->requestGetAvatar(avatarUrl, OnetAvatar::AT_collection);
    }
}

/*!
    OK response:
    \code
        <root>
            <error>
                <code>0</code>
                <text><![CDATA[OK]]></text>
            </error>
            <data>
                <count>1</count>
                <images>
                    <angle>0</angle>
                    <crop><![CDATA[0-0-675-675]]></crop>
                    <height>677</height>
                    <img><![CDATA[9250d9265e492780cc1bb46e955ed21d]]></img>
                    <imgId>44231885</imgId>
                    <width>1016</width>
                    <desc><![CDATA[100_3893.jpg]]></desc>
                    <mApp>19</mApp>
                    <mSrv><![CDATA[http://foto1.m.onet.pl/_m/]]></mSrv>
                </images>
            </data>
            <reqId><![CDATA[3722766d-737e-41e8-9c26-5fff7b0e5e3b]]></reqId>
        </root>
    \endcode
*/
void AvatarListWidget::getMyAvatarsReady(const QByteArray &content)
{
    QList<MyAvatar> lMyAvatars;

    QDomDocument doc;
    doc.setContent(content);
    QDomElement docElem = doc.documentElement();

    // error
    QDomNodeList error = docElem.elementsByTagName("error");
    int code = error.at(0).firstChildElement("code").text().toInt();
    QString text = error.at(0).firstChildElement("text").text();

    if (code != 0)
    {
        if (Settings::instance()->getBool("debug"))
            qWarning() << "Error getMyAvatars: " << text;
        return;
    }

    // images
    QDomNodeList nodes = docElem.elementsByTagName("images");
    for (int n = 0; n < nodes.size(); ++n)
    {
        MyAvatar avatar;

        QDomNode node = nodes.item(n);
        QDomNodeList child_nodes = node.childNodes();
        for (int c = 0; c < child_nodes.size(); ++c)
        {
            QDomNode child_node = child_nodes.item(c);

            if (child_node.nodeName() == "angle")
                avatar.setAngle(child_node.toElement().text().toInt());
            else if (child_node.nodeName() == "crop")
                avatar.setCrop(child_node.toElement().text());
            else if (child_node.nodeName() == "height")
                avatar.setHeight(child_node.toElement().text().toInt());
            else if (child_node.nodeName() == "img")
                avatar.setImg(child_node.toElement().text());
            else if (child_node.nodeName() == "imgId")
                avatar.setImgId(child_node.toElement().text().toInt());
            else if (child_node.nodeName() == "width")
                avatar.setWidth(child_node.toElement().text().toInt());
            else if (child_node.nodeName() == "desc")
                avatar.setDesc(child_node.toElement().text());
            else if (child_node.nodeName() == "mApp")
                avatar.setMApp(child_node.toElement().text().toInt());
            else if (child_node.nodeName() == "mSrv")
                avatar.setMSrv(child_node.toElement().text());
        }

        lMyAvatars.append(avatar);
    }

    QPixmap pixmap = QPixmap(50, 50);
    pixmap.fill(Qt::white);

    QIcon icon = QIcon(pixmap);

    ui.listWidget_my_avatars->clear();

    foreach(const MyAvatar &avatar, lMyAvatars)
    {
        MyAvatarListWidgetItem *item = new MyAvatarListWidgetItem(avatar);
        item->setIcon(icon);

        // add blank avatar placeholder
        ui.listWidget_my_avatars->addItem(item);
        // get avatar for current position
        onetAvatar->requestGetAvatar(avatar.getUrl(), OnetAvatar::AT_my);
    }
}

/*!
    OK response:
    \code
        <root>
            <data>
                <mHash><![CDATA[7ec5873b8e5a0b735c2b4e217c2a5d90]]></mHash>
                <width>1200</width>
                <height>750</height>
                <exif><![CDATA[]]></exif>
                <fotoSrv><![CDATA[http://foto0.m.onet.pl/_m/]]></fotoSrv>
                <mApp>19</mApp>
            </data>
            <reqId><![CDATA[7cdd9f84-0f9d-49f7-99bc-951c1e537f2f]]></reqId>
        </root>
    \endcode
*/
void AvatarListWidget::uploadImageReady(const QByteArray &content, const QString &fileName)
{
    QDomDocument doc;
    doc.setContent(content);
    QDomElement docElem = doc.documentElement();

    // error tag is optional here
    QDomNodeList error = docElem.elementsByTagName("error");
    if (!error.isEmpty())
    {
        int code = error.at(0).firstChildElement("code").text().toInt();
        QString text = error.at(0).firstChildElement("text").text();

        if (code != 0)
        {
            if (Settings::instance()->getBool("debug"))
                qWarning() << "Error uploadImage: " << text;
            return;
        }
    }

    // data
    QDomNodeList dataNodeList = docElem.elementsByTagName("data");
    if (!dataNodeList.isEmpty())
    {

        QDomElement el = dataNodeList.at(0).toElement();
        QDomNode pEntries = el.firstChild();

        MyAvatar avatar;

        while (!pEntries.isNull())
        {
            QString tagName = pEntries.toElement().tagName();
            if (tagName == "mHash")
                avatar.setImg(pEntries.toElement().text());
            else if (tagName == "width")
                avatar.setWidth(pEntries.toElement().text().toInt());
            else if (tagName == "height")
                avatar.setHeight(pEntries.toElement().text().toInt());
            else if (tagName == "fotoSrv")
                avatar.setMSrv(pEntries.toElement().text());
            else if (tagName == "mApp")
                avatar.setMApp(pEntries.toElement().text().toInt());

            pEntries = pEntries.nextSibling();
        }

        if ((!avatar.img().isEmpty()) && (avatar.width() > 0) && (avatar.height() > 0)
            && (!avatar.mSrv().isEmpty()) && (avatar.mApp() > 0))
        {
            avatar.setAngle(0);
            avatar.setDesc(fileName);

            onetAvatar->requestAddPhoto(avatar);
        }
    }
}

/*!
    OK response:
    \code
        <root>
            <error>
                <code>0</code>
                <text><![CDATA[OK]]></text>
            </error>
            <data>
                <functionName><![CDATA[updatePhoto]]></functionName>
            </data>
            <reqId><![CDATA[28891280-12ce-4462-8abc-62d0bf690d3d]]></reqId>
        </root>
    \endcode
*/
void AvatarListWidget::updatePhotoReady(const QByteArray &content)
{
    QDomDocument doc;
    doc.setContent(content);
    QDomElement docElem = doc.documentElement();

    // error
    QDomNodeList error = docElem.elementsByTagName("error");
    int code = error.at(0).firstChildElement("code").text().toInt();
    QString text = error.at(0).firstChildElement("text").text();

    if (code != 0)
    {
        if (Settings::instance()->getBool("debug"))
            qWarning() << "Error updatePhoto: " << text;
        return;
    }

    // FIXME: just update one in ui.listWidget_my_avatars
    onetAvatar->requestGetMyAvatars();
}

/*!
    OK response:
    \code
        <root>
            <data>
                <mHash><![CDATA[7bec84066455a12e87e7bea1decc58a1]]></mHash>
                <width>200</width>
                <height>200</height>
                <exif><![CDATA[]]></exif>
                <fotoSrv><![CDATA[http://foto1.m.onet.pl/_m/]]></fotoSrv>
                <mApp>19</mApp>
            </data>
            <reqId><![CDATA[37bebcec-da81-4a17-85aa-8b1205e4b0d9]]></reqId>
        </root>
    \endcode

    Error response:
    \code
        <root>
            <error>
                <code>-500</code>
                <text><![CDATA[Bad params]]></text>
            </error>
            <data><![CDATA[]]></data>
            <reqId><![CDATA[37bebcec-da81-4a17-85aa-8b1205e4b0d9]]></reqId>
        </root>
    \endcode
*/
void AvatarListWidget::addPhotoReady(const QByteArray &content)
{
    QDomDocument doc;
    doc.setContent(content);
    QDomElement docElem = doc.documentElement();

    // error tag is optional here
    QDomNodeList error = docElem.elementsByTagName("error");
    if (!error.isEmpty())
    {
        int code = error.at(0).firstChildElement("code").text().toInt();
        QString text = error.at(0).firstChildElement("text").text();

        if (code != 0)
        {
            if (Settings::instance()->getBool("debug"))
                qWarning() << "Error addPhoto: " << text;
            return;
        }
    }

    // FIXME: just add one to ui.listWidget_my_avatars
    onetAvatar->requestGetMyAvatars();
}

/*!
    OK response:
    \code
        <root>
            <error>
                <code>0</code>
                <text><![CDATA[OK]]></text>
            </error>
            <data><![CDATA[]]></data>
            <reqId><![CDATA[3dba2705-f507-43d7-ae75-a677457f027f]]></reqId>
        </root>
    \endcode
*/
void AvatarListWidget::deletePhotoReady(const QByteArray &content)
{
    QDomDocument doc;
    doc.setContent(content);
    QDomElement docElem = doc.documentElement();

    // error
    QDomNodeList error = docElem.elementsByTagName("error");
    int code = error.at(0).firstChildElement("code").text().toInt();
    QString text = error.at(0).firstChildElement("text").text();

    if (code != 0)
    {
        if (Settings::instance()->getBool("debug"))
            qWarning() << "Error deletePhoto: " << text;
        return;
    }

    // FIXME: just remove one from ui.listWidget_my_avatars
    onetAvatar->requestGetMyAvatars();
}

void AvatarListWidget::getAvatarReady(const QByteArray &content, const QString &avatarUrl,
    OnetAvatar::AvatarType type)
{
    QPixmap pixmap;
    if (!pixmap.loadFromData(content))
    {
        if (Settings::instance()->getBool("debug"))
            qWarning() << "Unable to load image from: " << avatarUrl;
        return;
    }

    switch (type)
    {
    case OnetAvatar::AT_my:
        drawMyAvatar(pixmap, avatarUrl);
        break;
    case OnetAvatar::AT_collection:
        drawAvatarFromCollection(pixmap, avatarUrl);
        break;
    default:
        break;
    }
}

void AvatarListWidget::drawMyAvatar(const QPixmap &pixmap, const QString &avatarUrl)
{
    for (int i = 0; i < ui.listWidget_my_avatars->count(); ++i)
    {
        MyAvatarListWidgetItem *item = static_cast<MyAvatarListWidgetItem*>(ui.listWidget_my_avatars->item(i));
        Q_ASSERT(item != 0);
        QString tUrl = item->avatar.getUrl();
        if (avatarUrl == tUrl)
        {
            item->setIcon(QIcon(pixmap));
            ui.listWidget_my_avatars->update();
        }
    }
}

void AvatarListWidget::drawAvatarFromCollection(const QPixmap &pixmap, const QString &avatarUrl)
{
    for (int i = 0; i < ui.listWidget_collections->count(); ++i)
    {
        QListWidgetItem *item = ui.listWidget_collections->item(i);
        Q_ASSERT(item != 0);
        QString tUrl = item->data(AvatarUrlRole).toString();
        if (avatarUrl == tUrl)
        {
            item->setIcon(QIcon(pixmap));
            ui.listWidget_collections->update();
        }
    }
}

void AvatarListWidget::tabChanged(int index)
{
    if (index == 1 && collections.empty())
    {
        onetAvatar->requestGetCollections();
    }
}

void AvatarListWidget::collectionChanged(QString name)
{
    if (collections.contains(name))
    {
        int index = collections.value(name);
        onetAvatar->requestGetCollectionAvatars(index);
    }
}

void AvatarListWidget::addAvatarClicked()
{
    QString selectedFilter;
    QString fileName = QFileDialog::getOpenFileName(this,
                                     tr("Select Image File"),
                                     "",
                                     tr("JPG Files (*.jpg)"),
                                     &selectedFilter,
                                     0);
    if (!fileName.isEmpty())
    {
        QByteArray content;

        // read file
        QFile file(fileName);
        if (!file.open(QIODevice::ReadOnly))
        {
            if (Settings::instance()->getBool("debug"))
                qWarning() << "Error opening file: " << fileName;
            return;
        }
        content = file.readAll();
        file.close();
        onetAvatar->requestUploadImage(fileName, content);
    }
}

void AvatarListWidget::editAvatarClicked()
{
    if (!ui.listWidget_my_avatars->selectedItems().isEmpty())
    {
        MyAvatarListWidgetItem *item = static_cast<MyAvatarListWidgetItem*>(ui.listWidget_my_avatars->selectedItems().at(0));
        AvatarEditGui dialog(this, item->avatar, onetAvatar);
        if (dialog.exec() == QDialog::Accepted)
        {
            onetAvatar->requestUpdatePhoto(dialog.getAvatar());
        }
    }
}

void AvatarListWidget::setMyAvatarClicked()
{
    if (!ui.listWidget_my_avatars->selectedItems().isEmpty())
    {
        MyAvatarListWidgetItem *item = static_cast<MyAvatarListWidgetItem*>(ui.listWidget_my_avatars->selectedItems().at(0));
        QString avatarUrl = item->avatar.getUrl();
        if (!avatarUrl.isEmpty())
        {
            emit avatarSelected(avatarUrl);
        }
    }
}

void AvatarListWidget::removeAvatarClicked()
{
    if (!ui.listWidget_my_avatars->selectedItems().isEmpty())
    {
        MyAvatarListWidgetItem *item = static_cast<MyAvatarListWidgetItem*>(ui.listWidget_my_avatars->selectedItems().at(0));
        int imgId = item->avatar.imgId();
        if (imgId > 0)
        {
            onetAvatar->requestDeletePhoto(imgId);
        }
    }
}

void AvatarListWidget::setEmptyAvatarClicked()
{
    emit avatarSelected(QString());
}

void AvatarListWidget::setCollectionAvatarClicked()
{
    if (ui.listWidget_collections->selectedItems().size() == 0)
        return;

    QString avatarUrl = ui.listWidget_collections->selectedItems().at(0)->data(AvatarUrlRole).toString();

    if (!avatarUrl.isEmpty())
    {
        emit avatarSelected(avatarUrl);
    }
}

