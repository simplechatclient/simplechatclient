/*
 * Simple Chat Client
 *
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

#include <QDebug>
#include <QRect>
#include <QStringList>

#include "my_avatar.h"

MyAvatar::MyAvatar()
{
    m_angle = -1;
    m_width = -1;
    m_height = -1;
    m_imgId = -1;
    m_mApp = -1;
}

MyAvatar::~MyAvatar()
{
}

QString MyAvatar::getUrl() const
{
    if ((!m_mSrv.isEmpty()) && (!m_img.isEmpty()) && (m_mApp >= 0) && (m_angle >= 0) && (m_imgId >= 0))
    {
        return QString("%1%2,1,%3,%4%5%6.jpg").arg(m_mSrv, m_img, QString::number(m_mApp), m_crop, m_crop.isEmpty()?"":"-", QString::number(m_angle));
    }
    else
    {
        return QString::null;
    }
}

QString MyAvatar::getRawUrl() const
{
    return getRawUrl(m_angle);
}

QString MyAvatar::getRawUrl(int angle) const
{
    if ((!m_mSrv.isEmpty()) && (!m_img.isEmpty()) && (m_mApp >= 0) && (m_angle >= 0) && (m_imgId >= 0))
    {
        return QString("%1%2,8,%3,%4.jpg").arg(m_mSrv, m_img, QString::number(m_mApp), QString::number(angle));
    }
    else
    {
        return QString::null;
    }
}

int MyAvatar::rotateLeft()
{
    return rotate(LEFT);
}

int MyAvatar::rotateRight()
{
    return rotate(RIGHT);
}

int MyAvatar::angle() const
{
    return m_angle;
}

int MyAvatar::mApp() const
{
    return m_mApp;
}

QString MyAvatar::crop() const
{
    return m_crop;
}

QString MyAvatar::desc() const
{
    return m_desc;
}

int MyAvatar::height() const
{
    return m_height;
}

QString MyAvatar::img() const
{
    return m_img;
}

int MyAvatar::imgId() const
{
    return m_imgId;
}

QString MyAvatar::mSrv() const
{
    return m_mSrv;
}

int MyAvatar::width() const
{
    return m_width;
}

void MyAvatar::setAngle(int angle)
{
    m_angle = angle;
}

void MyAvatar::setMApp(int mApp)
{
    m_mApp = mApp;
}

void MyAvatar::setCrop(QString crop)
{
    m_crop = crop;
}

void MyAvatar::setDesc(QString desc)
{
    m_desc = desc;
}

void MyAvatar::setHeight(int height)
{
    m_height = height;
}

void MyAvatar::setImg(QString img)
{
    m_img = img;
}

void MyAvatar::setImgId(int imgId)
{
    m_imgId = imgId;
}

void MyAvatar::setMSrv(QString mSrv)
{
    m_mSrv = mSrv;
}

void MyAvatar::setWidth(int width)
{
    m_width = width;
}


int MyAvatar::rotate(Rotate i)
{
    m_angle += i;

    if (m_angle > 3)
        m_angle = 0;
    if (m_angle < 0)
        m_angle = 3;

    return m_angle;
}

QString MyAvatar::scaledCropToString(QRect crop, QSize scaled, QSize original, int angle, bool rotate)
{
    if (crop.isNull() || (crop.x() == 0 && crop.y() == 0 && crop.size() == scaled))
        return QString::null;

    qreal scale = 1.0;
    if (angle % 2 == 0)
    {
        scale = (qreal) original.width() / (qreal) scaled.width();
    }
    else
    {
        scale = (qreal) original.width() / (qreal) scaled.height();
    }

    if (rotate)
    {
        int tmp;
        switch (angle)
        {
        case A_90:
            tmp = crop.x();
            crop.moveLeft(crop.y());
            crop.moveTop(scaled.width() - crop.width() - tmp);
            break;
        case A_180:
            crop.moveLeft(scaled.width() - crop.width() - crop.x());
            crop.moveTop(scaled.height() - crop.height() - crop.y());
            break;
        case A_270:
            tmp = crop.y();
            crop.moveTop(crop.x());
            crop.moveLeft(scaled.height() - crop.height() - tmp);
            break;
        }
    }

    crop.moveLeft(qRound(crop.x() * scale));
    crop.moveTop(qRound(crop.y() * scale));
    crop.setWidth(qRound(crop.width() * scale));
    crop.setHeight(qRound(crop.height() * scale));

    QString s = QString("%1-%2-%3-%4").arg(QString::number(crop.x()), QString::number(crop.y()),
        QString::number(crop.width()), QString::number(crop.height()));

    /*
    qDebug() << "scaledCropToString: string:" << s << "crop:" << crop << "scale:" << scale
        << "orgW:" << original.width() << "orgH:" << original.height() << "scaledW:"
        << scaled.width() << "scaledH:" << scaled.height() << "angle:" << angle;
    */

    return s;
}

QRect MyAvatar::stringToScaledCrop(QString s, QSize scaled, QSize original, int angle, bool rotate)
{
    QStringList result = s.split("-");
    if (result.size() != 4)
        return QRect();

    if (!scaled.isValid())
        return QRect();

    qreal scale = 1.0;
    if (angle % 2 == 0)
    {
        scale = (qreal) scaled.width() / (qreal) original.width();
    }
    else
    {
        scale = (qreal) scaled.height() / (qreal) original.width();
    }
    QRect crop = QRect(result.at(0).toInt(), result.at(1).toInt(), result.at(2).toInt(), result.at(3).toInt());

    if (rotate)
    {
        int tmp;
        switch (angle)
        {
        case A_90:
            tmp = crop.x();
            crop.moveLeft(original.height() - crop.y() - crop.height());
            crop.moveTop(tmp);
            break;
        case A_180:
            crop.moveLeft(original.width() - crop.x() - crop.width());
            crop.moveTop(original.height() - crop.y() - crop.height());
            break;
        case A_270:
            tmp = crop.y();
            crop.moveTop(original.width() - crop.x() - crop.width());
            crop.moveLeft(tmp);
            break;
        }
    }

    crop.moveLeft(qRound(crop.x() * scale));
    crop.moveTop(qRound(crop.y() * scale));
    crop.setWidth(qRound(crop.width() * scale));
    crop.setHeight(qRound(crop.height() * scale));

    /*
    qDebug() << "stringToScaledCrop: string:" << s << "crop:" << crop << "scale:" << scale
        << "orgW:" << original.width() << "orgH:" << original.height() << "scaledW:"
        << scaled.width() << "scaledH:" << scaled.height() << "angle:" << angle;
    */

    return crop;
}

QSize MyAvatar::size() const
{
    return QSize(m_width, m_height);
}
/*
void MyAvatar::debug(QString title) const
{
    qDebug() << title << "angle:" << angle() << "crop:" << crop() << "height:" << height() << "img:" << img() << "imgId:" << imgId() << "width:" << width() << "desc:" << desc() << "mApp:" << mApp() << "mSrv:" << mSrv();
}
*/
