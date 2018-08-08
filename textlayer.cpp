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
#include "textlayer.h"

// TODO: add alignment

/**
 * @brief TextLayer::TextLayer
 */
TextLayer::TextLayer( QGraphicsScene *scene, const QString &text ) :
    DesignerLayer( scene ),
    textItem( nullptr )
{

    if ( this->scene() == nullptr )
        return;

    DesignerLayer::setName( this->tr( "Text layer" ));
    DesignerLayer::setType( DesignerLayer::Types::Text );
    this->font.setPointSize( 96 );
    this->textItem = this->scene()->addText( text );
    this->textItem->setFont( this->font );
    this->textItem->setDefaultTextColor( Qt::white );
}

/**
 * @brief TextLayer::item
 * @return
 */
QGraphicsItem *TextLayer::item() {
    return dynamic_cast<QGraphicsItem*>( this->textItem );
}
