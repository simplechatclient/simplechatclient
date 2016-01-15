/*
 * Simple Chat Client
 *
 *   Copyright (C) 2012-2016 Dariusz Markowicz <dmarkowicz@alari.pl>
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

#include <QtWidgets>
#include <QPainter>

#include "avatar_edit_scene.h"

// TODO: redesign in Qt way ;)
// i.e. crop as opacity mask not a copy
AvatarEditScene::AvatarEditScene(QObject *parent)
    : QGraphicsScene(parent)
{
    setSceneRect(0, 0, 480, 360);

    isSizing = false;
    isDragged = false;

    crop = QRect(0, 0, 100, 100);

    // background
    setBackgroundBrush(QPixmap(":/images/transparent.png"));
    // scaled photo
    photoItem = addPixmap(QPixmap());
    // gray it down
    photoBlurItem = addRect(QRect(), Qt::NoPen, QBrush(QColor(255, 255, 255, 127)));
    // crop
    cropItem = addPixmap(QPixmap());
    // cropper
    QPixmap pixmap = getCropper();
    cropper = pixmap.rect();
    cropperItem = addPixmap(pixmap);
    cropperItem->setOpacity(0.5);
}

AvatarEditScene::~AvatarEditScene()
{
}

QPixmap AvatarEditScene::getCropper()
{
    QPixmap foreground = QPixmap(":/images/crop.png");
    QPixmap pixmap = QPixmap(foreground.size());
    pixmap.fill(Qt::white);
    QPainter p(&pixmap);
    p.drawPixmap(foreground.rect(), foreground);
    return pixmap;
}

void AvatarEditScene::setPhoto(const QPixmap &photo, const QRect &crop)
{
    this->crop = crop;
    this->photo = photo;

    // scaled photo
    photoItem->setPixmap(this->photo);
    // gray it down
    photoBlurItem->setRect(photoItem->boundingRect());

    redraw();
}

void AvatarEditScene::redraw()
{
    // crop
    QPixmap c = photo.copy(crop);
    // crop border
    QPainter cp(&c);
    cp.setPen(Qt::DotLine);
    cp.setCompositionMode(QPainter::CompositionMode_Clear);
    cp.drawRect(0, 0, crop.width() - 1, crop.height() - 1);
    cropItem->setPixmap(c);
    cropItem->setPos(crop.topLeft());
    // cropper
    cropper.moveCenter(crop.bottomRight());
    cropperItem->setPos(cropper.topLeft());
}

void AvatarEditScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() != Qt::LeftButton)
        return;

    QGraphicsView *view = qobject_cast<QGraphicsView*>(event->widget()->parent());
    Q_ASSERT(view);
    QPoint p(view->mapFromScene(event->scenePos()));

    if (!isSizing && crop.contains(p))
    {
        isDragged = true;
        view->setCursor(Qt::ClosedHandCursor);
        origin = p;
    }
    else if (!isDragged && cropper.contains(p))
    {
        isSizing = true;
        view->setCursor(Qt::SizeFDiagCursor);
    }

    QGraphicsScene::mousePressEvent(event);
}

void AvatarEditScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsScene::mouseMoveEvent(event);

    QGraphicsView *view = qobject_cast<QGraphicsView*>(event->widget()->parent());
    Q_ASSERT(view);
    QPoint p(view->mapFromScene(event->scenePos()));

    if (isDragged)
    {
        crop.moveLeft(qMax(0, crop.x() + (p.x() - origin.x())));
        crop.moveTop(qMax(0, crop.y() + (p.y() - origin.y())));
        crop.moveLeft(qMin(photo.width() - crop.width(), crop.x()));
        crop.moveTop(qMin(photo.height() - crop.height(), crop.y()));
        origin = p;

        redraw();
        emit cropChanged(crop);
    }
    else if (isSizing)
    {
        crop.setWidth(qMax(1, p.x() - crop.x()));
        crop.setWidth(
            qMin(qMin(photo.width() - crop.x(), photo.height() - crop.y()), crop.width()));
        crop.setHeight(crop.width());

        redraw();
        emit cropChanged(crop);
    }
}

void AvatarEditScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() != Qt::LeftButton)
        return;

    QGraphicsView *view = qobject_cast<QGraphicsView*>(event->widget()->parent());
    Q_ASSERT(view);

    view->setCursor(Qt::ArrowCursor);

    isDragged = false;
    isSizing = false;

    QGraphicsScene::mouseMoveEvent(event);
}

