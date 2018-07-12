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
#include <QLabel>
#include <QPainter>
#include <QStyle>
#include <QPaintEngine>

/**
 * @brief The PixmapLabel class
 */
class PixmapLabel final : public QLabel {
    Q_OBJECT

public:
    PixmapLabel( QWidget *parent ) : QLabel( parent ) { this->transparency.load( ":/icons/transparency" ); }
    ~PixmapLabel() = default;

protected:
    /**
     * @brief paintEvent
     * @param event
     */
    void paintEvent( QPaintEvent* event ) {
        if ( this->pixmap() != nullptr ) {
            QPainter painter( this );
            const QRect pixmapRect( this->rect().width()  / 2 - qMin( this->width(),  this->pixmap()->width())  / 2,
                                    this->rect().height() / 2 - qMin( this->height(), this->pixmap()->height()) / 2,
                                    qMin( this->width(), this->pixmap()->width()),
                                    qMin( this->height(), this->pixmap()->height()));

            painter.drawTiledPixmap( pixmapRect, this->transparency, QPointF());
            painter.drawPixmap( pixmapRect, *this->pixmap());

            painter.save();
            painter.setPen( Qt::gray );
            painter.drawRect( pixmapRect );
            painter.restore();
        } else {
            QLabel::paintEvent( event );
        }
    }

private:
    QPixmap transparency;
};
