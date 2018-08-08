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
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QObject>

/**
 * @brief The DesignerLayer class
 */
class DesignerLayer : public QObject {
    Q_OBJECT
    Q_ENUMS( Types )
    Q_DISABLE_COPY( DesignerLayer )
    Q_PROPERTY( QString name READ name WRITE setName )
    //Q_PROPERTY( Type type READ type WRITE setType )
    Q_PROPERTY( int horizontalOffset READ horizontalOffset WRITE setHorizontalOffset )
    Q_PROPERTY( int verticalOffset READ verticalOffset WRITE setVerticalOffset )
    Q_PROPERTY( qreal scale READ scale WRITE setScale )

public:
    enum class Types {
        NoType = -1,
        Text,
        Shape,
        Image,
        Bezel
    };

    /**
     * @brief DesignerLayer
     * @param s
     */
    explicit DesignerLayer( QGraphicsScene *s = nullptr ) :
        m_type( DesignerLayer::Types::NoType ),
        m_scene( s ),
        m_horizontalOffset( 0 ),
        m_verticalOffset( 0 ),
        m_scale( 1.0 )
    {}
    Types type() const { return this->m_type; }
    QGraphicsScene *scene() const { return this->m_scene; }
    QString name() const { return this->m_name; }
    virtual int horizontalOffset() const { return this->m_horizontalOffset; }
    virtual int verticalOffset() const { return this->m_verticalOffset; }
    virtual qreal scale() const { return this->m_scale; }

    /**
     * @brief item
     * @return
     */
    virtual QGraphicsItem *item() = 0;

    /**
     * @brief adjustPosition
     */
    virtual void adjust() {
        if ( this->item() == nullptr )
            return;

        QRectF sceneRect( this->scene()->sceneRect());
        QRectF rect( this->item()->sceneBoundingRect());
        this->item()->setPos(
                    sceneRect.width() / 2.0 - rect.width() * this->scale() / 2.0 + this->horizontalOffset(),
                    sceneRect.height() / 2.0 - rect.height() * this->scale() / 2.0 + this->verticalOffset());
    }

public slots:
    void setName( const QString &name ) { this->m_name = name; }
    void setType( Types type ) { this->m_type = type; }
    virtual void setHorizontalOffset( int offset ) { this->m_horizontalOffset = offset; }
    virtual void setVerticalOffset( int offset ) { this->m_verticalOffset = offset; }

    /**
     * @brief setScale
     * @param scale
     */
    virtual void setScale( qreal scale ) {
        this->m_scale = scale;

        if ( this->item() != nullptr )
            this->item()->setScale( scale );
    }

private:
    Types m_type;
    QGraphicsScene *m_scene;
    QString m_name;
    int m_horizontalOffset;
    int m_verticalOffset;
    qreal m_scale;
};
