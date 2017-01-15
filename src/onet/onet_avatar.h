/*
 * Simple Chat Client
 *
 *   Copyright (C) 2012-2017 Piotr Łuczko <piotr.luczko@gmail.com>
 *   Copyright (C) 2012-2017 Dariusz Markowicz <dmarkowicz@alari.pl>
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

#ifndef ONET_AVATAR_H
#define ONET_AVATAR_H

#include <QNetworkReply>
#include <QNetworkAccessManager>
class MyAvatar;

QT_BEGIN_NAMESPACE
class QByteArray;
QT_END_NAMESPACE

class OnetAvatar : public QNetworkAccessManager
{
    Q_OBJECT
public:
    enum AvatarType
    {
        AT_my,
        AT_myRaw,
        AT_collection,
        AT_other
    };

    OnetAvatar();

    void requestGetCollections();
    void requestGetCollectionAvatars(int id);
    void requestGetMyAvatars();
    void requestSetAvatar(int imgId, int albumId);
    void requestUploadImage(const QString &fileName, const QByteArray &data);
    void requestUpdatePhoto(const MyAvatar &avatar);
    void requestAddPhoto(const MyAvatar &avatar);
    void requestDeletePhoto(int imgId);
    void requestGetAvatar(const QString &avatarUrl, AvatarType type);

signals:
    void getCollectionsReady(const QByteArray &content);
    void getCollectionAvatarsReady(const QByteArray &content);
    void getMyAvatarsReady(const QByteArray &content);
    void setAvatarReady(const QByteArray &content);
    void uploadImageReady(const QByteArray &content, const QString &fileName);
    void updatePhotoReady(const QByteArray &content);
    void addPhotoReady(const QByteArray &content);
    void deletePhotoReady(const QByteArray &content);
    void getAvatarReady(const QByteArray &content, const QString &avatarUrl, OnetAvatar::AvatarType type);
    void errorOccured(QNetworkReply::NetworkError error);

protected slots:
    void replyFinished(QNetworkReply *reply);

private:
    // Names here intentionally similar to Onet Avatar API
    enum RequestType
    {
        RT_getCollections,
        RT_getAvatarsFromCollect,
        RT_loadFAvatars,
        RT_setAvatar,
        RT_uploadImage,
        RT_updatePhoto,
        RT_addPhoto,
        RT_deletePhoto,

        RT_getMyAvatar,
        RT_getMyRawAvatar,
        RT_getCollectionAvatar,
        RT_getAvatar,

        RT_undefined
    };

    void getCookies();
    QString createRid();

    QNetworkRequest basicRequest;
};

#endif // ONET_AVATAR_H
