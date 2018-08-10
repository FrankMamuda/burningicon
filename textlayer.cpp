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
    this->font.setPointSize( TextItem::DefaultPointSize );
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

/**
 * @brief TextLayer::setScale
 * @param scale
 */
void TextLayer::setScale( qreal scale ) {
    if ( this->item() == nullptr )
        return;

    this->font.setPointSize( qMax( TextItem::MinimumPointSize, static_cast<int>( TextItem::DefaultPointSize * scale )));
    this->textItem->setFont( this->font );
}

/**
 * @brief TextLayer::adjust
 */
void TextLayer::adjust() {
    if ( this->item() == nullptr )
        return;

    this->item()->setPos(
                this->scene()->sceneRect().width() / 2.0 -
                this->item()->sceneBoundingRect().width() / 2.0 + this->horizontalOffset(),
                this->scene()->sceneRect().height() / 2.0 -
                this->item()->sceneBoundingRect().height() / 2.0 + this->verticalOffset());
}
