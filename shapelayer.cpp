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

//
// includes
//
#include "shapelayer.h"

/**
 * @brief ShapeLayer::ShapeLayer
 */
ShapeLayer::ShapeLayer( QGraphicsScene *scene, Shapes shape ) :
    DesignerLayer( scene ),
    ellipseItem( nullptr ),
    rectItem( nullptr ),
    m_shape( shape )
{
    if ( this->scene() == nullptr )
        return;

    this->pen = QPen( Qt::black );
    this->brush = QBrush( Qt::darkGreen );
    this->pen.setWidth( 16 );
    this->setScale( 0.9 );

    DesignerLayer::setType( DesignerLayer::Types::Shape );

    if ( shape == ShapeLayer::Shapes::Ellipse ) {
        this->ellipseItem = this->scene()->addEllipse( this->scene()->sceneRect(), this->pen, this->brush );
        DesignerLayer::setName( this->tr( "Ellipse" ));
    } else if ( shape == ShapeLayer::Shapes::Rectangle ) {
        this->rectItem = this->scene()->addRect( this->scene()->sceneRect(), this->pen, this->brush );
        DesignerLayer::setName( this->tr( "Rectangle" ));
    } else {
        DesignerLayer::setName( this->tr( "Shape layer" ));
    }
}

/**
 * @brief ShapeLayer::item
 * @return
 */
QGraphicsItem *ShapeLayer::item() {
    if ( this->shape() == ShapeLayer::Shapes::Ellipse ) {
        return dynamic_cast<QGraphicsItem*>( this->ellipseItem );
    } else if ( this->shape() == ShapeLayer::Shapes::Rectangle ) {
        return dynamic_cast<QGraphicsItem*>( this->rectItem );
    }

    return nullptr;
}
