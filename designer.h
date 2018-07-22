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
#include <QMenu>
#include <QPen>
#include "designerlayer.h"

/**
 * @brief The Ui namespace
 */
namespace Ui {
class Designer;
}

//
// classes
//
//class DesignerLayer;
class DesignerModel;

/**
 * @brief The MainWindow class
 */
class Designer final : public QMainWindow {
    Q_OBJECT
    Q_ENUMS( ColourTarget )
    Q_DISABLE_COPY( Designer )
    Q_ENUMS( ToolPages )

public:
    enum ColourTarget {
        NoColourTarget = -1,
        BrushTarget,
        PenTarget,
        TextTarget
    };

    enum ToolPages {
        NoPage = -1,
        Text,
        Shape,
        Image
    };

    static Designer *instance() { static Designer *instance = new Designer(); return instance; }
    ~Designer();
    QList<DesignerLayer*> layers;

public slots:
    void setupLayers();
    void setupText();
    void setupShape();
    void addLayer( DesignerLayer *layer );

private slots:
    void on_ellipseSizeSlider_valueChanged( int value );
    void on_penSizeSlider_valueChanged( int value );
    void colourChanged( ColourTarget target, const QColor &colour );
    void on_textEdit_textChanged( const QString &text );
    void on_pointSizeSlider_valueChanged( int value );
    void on_textXSlider_valueChanged( int value );
    void on_textYSlider_valueChanged( int value );
    void adjustText();
    void on_exportButton_clicked();
    void on_imageScaleSlider_valueChanged( int );
    void on_imageXSlider_valueChanged( int );
    void on_imageYSlider_valueChanged( int );
    void adjustImage();

private:
    explicit Designer( QWidget *parent = nullptr );
    Ui::Designer *ui;
    QGraphicsScene *scene;
    DesignerModel *model;
    QMap<int, DesignerLayer*> layerMap;
    DesignerLayer *currentLayer() const;
    QMenu *addMenu;
};
