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

#ifndef AVATAR_EDIT_SCENE_H
#define AVATAR_EDIT_SCENE_H

#include <QGraphicsScene>

QT_BEGIN_NAMESPACE
class QGraphicsSceneMouseEvent;
class QPixmap;
class QGraphicsPixmapItem;
QT_END_NAMESPACE

class AvatarEditScene : public QGraphicsScene
{
    Q_OBJECT
public:
    AvatarEditScene(QObject *parent = 0);
    virtual ~AvatarEditScene();

    void setPhoto(const QPixmap &photo, const QRect &crop);

signals:
    void cropChanged(const QRect &);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent);

private:
    QGraphicsPixmapItem *photoItem;
    QGraphicsRectItem *photoBlurItem;
    QGraphicsPixmapItem *cropItem;
    QGraphicsPixmapItem *cropperItem;

    QRect crop;
    QRect cropper;
    QPixmap photo;

    QPoint origin;
    bool isSizing;
    bool isDragged;

    QPixmap getCropper();
    void redraw();
};

#endif /* AVATAR_EDIT_SCENE_H */
