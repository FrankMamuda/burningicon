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
#include "mainwindow.h"
#include "variable.h"
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
        out.setByteOrder( QDataStream::BigEndian );

        if ( Variable::instance()->isEnabled( "settings/macOS" )) {
            QByteArray pixmapData;
            QBuffer pixmapBuffer( &pixmapData );
            pixmapBuffer.open( QIODevice::WriteOnly );
            QDataStream pixmapSteam( &pixmapBuffer );
            pixmapSteam.setByteOrder( QDataStream::BigEndian );

            int y = 0;
            foreach ( Layer *layer, pixmaps ) {
                const QImage image( layer->pixmap.toImage().convertToFormat( QImage::Format_ARGB32 ));

                QByteArray bytes;
                QBuffer buffer( &bytes );
                buffer.open( QIODevice::WriteOnly );
                image.save( &buffer, "PNG" );
                buffer.close();

                pixmapSteam.writeRawData( Ui::macOSLayers.at( y ).code, 4 );
                pixmapSteam << static_cast<quint32>( bytes.size() + 8 );
                pixmapSteam.writeRawData( bytes, bytes.length());

                y++;
            }

            pixmapBuffer.close();

            out << static_cast<quint8>( 0x69 );
            out << static_cast<quint8>( 0x63 );
            out << static_cast<quint8>( 0x6e );
            out << static_cast<quint8>( 0x73 );
            out << static_cast<quint32>( pixmapData.size() + 8 );
            out.writeRawData( pixmapData.constData(), pixmapData.size());

        } else {
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
        }

        // close file
        file.close();
    }
}

/**
 * @brief IconWriter::writeData
 * @param out
 * @param pixmap
 */
void IconWriter::writeData( QDataStream &out, const QImage &image, int bytesPerRow ) {
    int y, x;

    for ( y = 0; y < image.height(); y++ ) {
        for ( x = 0; x < image.width(); x++ ) {
            const QColor colour( image.pixelColor( x, image.height() - y - 1 ));

            const char pixel[4] = {
                static_cast<char>( colour.blue()),
                static_cast<char>( colour.green()),
                static_cast<char>( colour.red()),
                static_cast<char>( colour.alpha())
            };
            out.writeRawData( pixel, 4 );
        }
    }

    // write mask
    for ( y = 0; y < image.height(); y++ ) {
        unsigned char *data = new unsigned char[static_cast<unsigned long long>( bytesPerRow )];

        memset( data, 0, static_cast<size_t>( bytesPerRow ));
        x = 0;

        while ( x != image.width()) {
            const int bytePos = x % 8 ? ( x + 7 ) / 8 - 1 : x / 8;
            const int bitPos = 7 - x % 8;

            if ( !image.pixelColor( x, image.height() - y - 1 ).alpha())
                data[bytePos] |= 1UL << bitPos;

            x++;
        }

        out.writeRawData( reinterpret_cast<const char*>( data ), bytesPerRow );
        delete[] data;
    }
}

/**
 * @brief IconWriter::getIconData
 * @param pixmap
 * @return
 */
IcoDirectory IconWriter::writeIconData( Layer *layer, QDataStream &out, qint64 pos ) {
    const QImage image( layer->pixmap.toImage().convertToFormat( QImage::Format_ARGB32 ));
    BitmapHeader header;
    quint32 imageSize = 0;
    IcoDirectory dir;
    const int bytesPerRow = image.width() % 32 ? ( image.width() / 32 + 1 ) * 4 : image.width() / 8;

    if ( !layer->isCompressed()) {
        // generate header
        header.width = image.width();
        header.height = image.height() * 2;
        header.imageSize = static_cast<quint32>( image.width() * image.height() * 4 + bytesPerRow * image.height());
        header.xpm = image.dotsPerMeterX();
        header.ypm = image.dotsPerMeterY();

        // write header
        out << header;

        // write pixmap data
        this->writeData( out, image, bytesPerRow );
    } else {
        QByteArray bytes;
        QBuffer buffer( &bytes );
        buffer.open( QIODevice::WriteOnly );
        image.save( &buffer, "PNG" );
        out.writeRawData( bytes, bytes.size());
        imageSize = static_cast<quint32>( bytes.size());
        buffer.close();
    }

    // generate ico directory
    dir.width = layer->isCompressed() ? 0 : static_cast<quint8>( header.width );
    dir.height = layer->isCompressed() ? 0 : static_cast<quint8>( header.height / 2 );
    dir.bytes = layer->isCompressed() ? imageSize : sizeof( BitmapHeader ) + header.imageSize;
    dir.offset = static_cast<quint32>( pos );

    // return directory entry
    return dir;
}
