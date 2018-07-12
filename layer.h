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
#include <QPixmap>

/**
 * @brief The LayerModel class
 */
class Layer final {
public:
    /**
     * @brief Layer
     */
    Layer( const QPixmap& px = QPixmap(), int scale = 0, bool compressed = false ) : m_scale( scale ), m_compressed( compressed ), m_override( false ) {
        if ( !px.isNull())
            this->pixmap = px.scaled( scale, scale, Qt::IgnoreAspectRatio, Qt::SmoothTransformation );
        else
            this->m_scale = 0;
    }
    Layer& operator=( const Layer & ) = default;
    Layer( const Layer& ) = default;

    QPixmap pixmap;
    int scale() const { return this->m_scale; }
    bool isCompressed() const { return this->m_compressed; }
    bool isOverriden() const { return this->m_override; }
    void setScale( int scale ) { this->m_scale = scale; }
    void setCompressed( bool compressed ) { this->m_compressed = compressed; }
    void setOverriden( bool override = false ) { this->m_override = override; }
    bool operator>( const Layer& layer ) const { return ( this->scale() > layer.scale()); }
    bool operator<( const Layer& layer ) const { return ( this->scale() < layer.scale()); }
    bool operator==( const Layer& layer ) const { return ( this->scale() == layer.scale()); }

private:
    int m_scale;
    bool m_compressed;
    bool m_override;
};
