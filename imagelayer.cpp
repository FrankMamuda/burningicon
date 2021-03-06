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
#include "imagelayer.h"

/**
 * @brief ImageLayer::ImageLayer
 */
ImageLayer::ImageLayer( QGraphicsScene *scene, const QPixmap &pixmap ) :
    DesignerLayer( scene ),
    pixmapItem( nullptr )
{
    this->setScale( 0.9 );

    if ( this->scene() == nullptr )
        return;

    DesignerLayer::setName( this->tr( "Image layer" ));
    DesignerLayer::setType( DesignerLayer::Types::Image );
    this->pixmapItem = this->scene()->addPixmap( pixmap );
    this->pixmapItem->setTransformationMode( Qt::SmoothTransformation );
}

/**
 * @brief ImageLayer::item
 * @return
 */
QGraphicsItem *ImageLayer::item() {
    return dynamic_cast<QGraphicsItem*>( this->pixmapItem );
}
