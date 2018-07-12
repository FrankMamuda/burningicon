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
#include "iconwriter.h"
#include <QBuffer>
#include <QFile>
#include <QtWin>

/**
 * @brief IconWriter::getBitmapSize
 * @param bitmap
 * @return
 */
unsigned int IconWriter::getBitmapSize( const BITMAP &bitmap ) {
    int size = bitmap.bmWidthBytes;

    if ( size & 3 )
        size = ( size + 4 ) & ~3;

    return static_cast<unsigned int>( size * bitmap.bmHeight );
}

/**
 * @brief IconWriter::write
 * @param filename
 * @param pixmaps
 */
void IconWriter::write( const QString &filename, const QList<Layer*> pixmaps ) {
    QFile file( filename );

    // open icon file for writing
    if ( file.open( QIODevice::WriteOnly )) {
        QDataStream out( &file );
        out.setByteOrder( QDataStream::LittleEndian );
        IcoHeader header( static_cast<quint16>( pixmaps.count()));
        QPixmap pixmap;
        QList<IcoDirectory> dirs;
        int y;

        // write icon header
        out << header;

        // skip to icon entries
        for ( y = 0; y < static_cast<int>( sizeof( IcoDirectory )) * pixmaps.count(); y++ )
            out << static_cast<qint8>( 0 );

        // write icon data
        foreach ( Layer *layer, pixmaps )
            dirs << this->writeIconData( layer, out, file.pos());

        // write icon directory entries
        file.seek( static_cast<int>( sizeof( IcoHeader )));
        foreach ( const IcoDirectory &dir, dirs )
            out << dir;

        // close file
        file.close();
    }
}

/**
 * @brief IconWriter::writeData
 * @param out
 * @param handle
 */
void IconWriter::writeData( QDataStream &out, const HBITMAP &handle ) {
    BITMAP bitmap;
    int y, k;
    unsigned char *data;
    unsigned int iconSize;

    // get bitmap
    GetObject( handle, sizeof( BITMAP ), &bitmap );
    iconSize = this->getBitmapSize( bitmap );
    data = new unsigned char[iconSize];
    GetBitmapBits( handle, static_cast<int>( iconSize ), data );

    // generate image buffer
    for ( y = bitmap.bmHeight - 1; y >= 0; y-- ) {
        // write scanline
        out.writeRawData( reinterpret_cast<const char*>( data + ( y * bitmap.bmWidthBytes )), bitmap.bmWidthBytes );

        // add padding
        if ( bitmap.bmWidthBytes & 3 ) {
            for ( k = 0; k < 4 - bitmap.bmWidthBytes; k++ )
                out << static_cast<qint8>( 0 );
        }
    }

    delete data;
}

/**
 * @brief IconWriter::getIconData
 * @param pixmap
 * @return
 */
IcoDirectory IconWriter::writeIconData( Layer *layer, QDataStream &out, qint64 pos ) {
    ICONINFO iconInfo;
    QPixmap pixmap( layer->pixmap );
    HICON icon = QtWin::toHICON( pixmap );
    BITMAP bitmap, mask;
    BitmapHeader header;
    quint32 imageSize = 0;
    IcoDirectory dir;

    if ( !layer->isCompressed()) {
        if( !GetIconInfo( icon, &iconInfo ))
            return dir;

        if( !GetObject( iconInfo.hbmColor, sizeof( BITMAP ), &bitmap ) || !GetObject( iconInfo.hbmMask, sizeof( BITMAP ), &mask ))
            return dir;

        // generate header

        imageSize = this->getBitmapSize( bitmap ) + this->getBitmapSize( mask );
        header.width = bitmap.bmWidth;
        header.height = bitmap.bmHeight * 2;
        header.planes = bitmap.bmPlanes;
        header.depth = bitmap.bmBitsPixel;
        header.imageSize = imageSize;

        // write header
        out << header;

        // write bitmap data
        this->writeData( out, iconInfo.hbmColor );
        this->writeData( out, iconInfo.hbmMask );
    } else {
        QByteArray bytes;
        QBuffer buffer(&bytes);
        buffer.open( QIODevice::WriteOnly );
        pixmap.save( &buffer, "PNG" );
        out.writeRawData( bytes, bytes.size());
        imageSize = static_cast<quint32>( bytes.size());
        buffer.close();
    }

    // generate ico directory
    dir.width = layer->isCompressed() ? 0 : static_cast<quint8>( bitmap.bmWidth );
    dir.height = layer->isCompressed() ? 0 : static_cast<quint8>( bitmap.bmHeight );
    dir.numColours = layer->isCompressed() ? 0 : ( bitmap.bmBitsPixel >= 8 ? 0 : static_cast<quint8>( 1 << ( bitmap.bmBitsPixel * bitmap.bmPlanes )));
    dir.planes = layer->isCompressed() ? 1 : bitmap.bmPlanes;
    dir.depth = layer->isCompressed() ? static_cast<quint16>( pixmap.depth()) : bitmap.bmBitsPixel;
    dir.bytes = layer->isCompressed() ? imageSize : sizeof( BITMAPINFOHEADER ) + imageSize;
    dir.offset = static_cast<quint32>( pos );

    // delete bitmaps
    DeleteObject( iconInfo.hbmColor );
    DeleteObject( iconInfo.hbmMask );

    // return directory entry
    return dir;
}
