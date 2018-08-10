/*
 * Copyright (C) 2018 Factory #12
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see http://www.gnu.org/licenses/.
 *
 */

#pragma once

//
// includes
//
#include "designerlayer.h"
#include <QGraphicsEllipseItem>
#include <QGraphicsRectItem>

/**
 * @brief The ImageLayer class
 */
class ShapeLayer final : public DesignerLayer {
    Q_OBJECT
    Q_ENUMS( Shapes )
    friend class Designer;

public:
    enum class Shapes {
        NoShape = -1,
        Ellipse,
        Rectangle
    };

    ShapeLayer( QGraphicsScene *scene = nullptr, Shapes shape = ShapeLayer::Shapes::Ellipse );
    Shapes shape() const { return this->m_shape; }
    QGraphicsItem *item() override;

private:
    QGraphicsEllipseItem *ellipseItem;
    QGraphicsRectItem *rectItem;
    Shapes m_shape;
    QPen pen;
    QBrush brush;
};
