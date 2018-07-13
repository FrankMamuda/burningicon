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
#include <QDebug>

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
 * @param pixmap
 */
void IconWriter::writeData( QDataStream &out, const QPixmap &pixmap ) {
    int y, x;
    const QImage image( pixmap.toImage());

    for ( y = 0; y < image.height(); y++ ) {
        //int padSize = 0;

        for ( x = 0; x < image.width(); x++ ) {
            QColor colour( image.pixelColor( x, image.height() - y - 1 ));

            const char pixel[4] = {
                static_cast<char>( colour.blue()),
                static_cast<char>( colour.green()),
                static_cast<char>( colour.red()),
                static_cast<char>( colour.alpha())
            };
            out.writeRawData( pixel, 4 );
        }
    }
}

/**
 * @brief IconWriter::getIconData
 * @param pixmap
 * @return
 */
IcoDirectory IconWriter::writeIconData( Layer *layer, QDataStream &out, qint64 pos ) {
    QPixmap pixmap( layer->pixmap );
    BitmapHeader header;
    quint32 imageSize = 0;
    IcoDirectory dir;

    if ( !layer->isCompressed()) {
        // generate header
        header.width = pixmap.width();
        header.height = pixmap.height() * 2;
        header.imageSize = static_cast<quint32>( pixmap.width() * pixmap.height() * 4 );

        // write header
        out << header;

        // write pixmap data
        this->writeData( out, pixmap );
    } else {
        QByteArray bytes;
        QBuffer buffer( &bytes );
        buffer.open( QIODevice::WriteOnly );
        pixmap.save( &buffer, "PNG" );
        out.writeRawData( bytes, bytes.size());
        imageSize = static_cast<quint32>( bytes.size());
        buffer.close();
    }

    // generate ico directory
    dir.width = layer->isCompressed() ? 0 : static_cast<quint8>( pixmap.width());
    dir.height = layer->isCompressed() ? 0 : static_cast<quint8>( pixmap.height());
    dir.bytes = layer->isCompressed() ? imageSize : sizeof( BitmapHeader ) + header.imageSize;
    dir.offset = static_cast<quint32>( pos );

    // return directory entry
    return dir;
}
