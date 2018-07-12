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
#include <QDataStream>
#include <QPixmap>
#include "layer.h"
#include "windows.h"
#include "main.h"

/**
 * @brief The IcoHeader struct
 */
struct IcoHeader {
    quint16 zero;
    quint16 one;
    quint16 numIcons;
    IcoHeader( quint16 n = 0 ) : zero( 0 ), one( 1 ), numIcons( n ) {}
};
Q_DECLARE_METATYPE( IcoHeader )
inline static QDataStream &operator<<( QDataStream &out, const IcoHeader &h ) { out << h.zero << h.one << h.numIcons; return out; }

/**
 * @brief The IcoDirectory struct
 */
struct IcoDirectory {
    quint8 width;
    quint8 height;
    quint8 numColours;
    quint8 zero;
    quint16 planes;
    quint16 depth;
    quint32 bytes;
    quint32 offset;
    IcoDirectory() : width( 0 ), height( 0 ), numColours( 0 ), zero( 0 ), planes( 0 ), depth( 0), bytes( 0 ), offset( 0 ) {}
};
Q_DECLARE_METATYPE( IcoDirectory )
inline static QDataStream &operator<<( QDataStream &out, const IcoDirectory &d ) { out << d.width << d.height << d.numColours << d.zero << d.planes << d.depth << d.bytes << d.offset; return out; }

/**
 * @brief The BitmapHeader struct
 */
struct BitmapHeader {
    quint32 headerSize;
    qint32 width;
    qint32 height;
    quint16 planes;
    quint16 depth;
    quint32 compression;
    quint32 imageSize;
    qint32 xpm;
    qint32 ypm;
    quint32 numColors;
    quint32 indexes;
    BitmapHeader() : headerSize( 40 ), width( 0 ), height( 0 ), planes( 1 ), depth( 0 ), compression( 0 ), imageSize( 0 ), xpm( 0 ), ypm( 0 ), numColors( 0 ), indexes( 0 ) { }
};
inline static QDataStream &operator<<( QDataStream &out, const BitmapHeader &b ) { out << b.headerSize << b.width << b.height << b.planes << b.depth << b.compression << b.imageSize << b.xpm << b.ypm << b.numColors << b.indexes; return out; }


/**
 * @brief The IconWriter class
 */
class IconWriter final : public QObject {
    Q_OBJECT
    Q_DISABLE_COPY( IconWriter )

public:
    static IconWriter *instance() { static IconWriter *instance = new IconWriter(); return instance; }
    virtual ~IconWriter() = default;
    void write( const QString &filename, const QList<Layer*> pixmaps );

private:
    unsigned int getBitmapSize( const BITMAP &bitmap );
    IcoDirectory writeIconData( Layer *layer, QDataStream &out, qint64 pos );
    void writeData( QDataStream &out, const HBITMAP &handle );
    IconWriter() { GarbageMan::instance()->add( this ); }
};
