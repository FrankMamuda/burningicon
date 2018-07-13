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
#include <QGraphicsEllipseItem>
#include <QMainWindow>
#include <QPen>

namespace Ui {
class IconMaker;
}

/**
 * @brief The MainWindow class
 */
class IconMaker : public QMainWindow {
    Q_OBJECT
    Q_ENUMS( ColourTarget )

public:
    enum ColourTarget {
        NoColourTarget = -1,
        BrushTarget,
        PenTarget,
        TextTarget
    };

    explicit IconMaker( QWidget *parent = nullptr );
    ~IconMaker();

private slots:
    void on_ellipseSizelSlider_valueChanged( int value );
    void on_penSizeSlider_valueChanged( int value );
    void colourChanged( ColourTarget target, const QColor &colour );
    void on_textEdit_textChanged( const QString &text );
    void on_pointSizeSlider_valueChanged( int value );
    void on_textXSlider_valueChanged( int value );
    void on_textYSlider_valueChanged( int value );
    void adjustText();
    void on_exportButton_clicked();

private:
    Ui::IconMaker *ui;
    QGraphicsEllipseItem *ellipse;
    QGraphicsTextItem *text;
    QGraphicsScene *scene;
    QPen pen;
    QBrush brush;
    QFont font;
};
