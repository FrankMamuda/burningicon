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
#include <QMainWindow>
#include <QMap>

//
// classes
//
class LayerModel;
class Settings;
class Layer;

/**
 * @brief The Ui namespace
 */
namespace Ui {
class MainWindow;
const static QList<int> DefaultScales = QList<int>() << 16 << 32 << 48 << 256;
#ifndef Q_CC_MSVC
constexpr int __attribute__((unused)) MinimumScale = 4;
constexpr int __attribute__((unused)) ThresholdScale = 256;
constexpr int __attribute__((unused)) MaximumScale = 512;
#else
constexpr int MinimumScale = 4;
constexpr int ThresholdScale = 256;
constexpr int MaximumScale = 512;
#endif
}

/**
 * @brief The MainWindow class
 */
class MainWindow final : public QMainWindow {
    Q_OBJECT
    Q_ENUMS( StackPages )
    Q_DISABLE_COPY( MainWindow )

public:
    enum StackPages {
        NoPage = -1,
        Preview,
        Load
    };

    static MainWindow *instance() { static MainWindow *instance = new MainWindow(); return instance; }
    ~MainWindow();
    QList<Layer*> layers;
    void initialize();
    Layer *scaleToLayer( int scale ) const;

public:
    void setPixmap( const QPixmap &pixmap );

private slots:
    void on_actionExport_triggered();
    void generateLayers( const QList<int> scales = Ui::DefaultScales );
    void addLayer( int scale, bool resetModel = false );
    void overrideLayer( int scale, const QPixmap &pixmap );
    void restoreLayer( int scale );
    void removeLayer( int scale );
    void resetModel();
    void clearLayers();
    void on_actionClear_triggered();
    void on_actionSettings_triggered();

private:
    explicit MainWindow( QWidget *parent = nullptr );
    QPixmap scaled;
    Ui::MainWindow *ui;
    LayerModel *model;
    QMap<int, Layer*> layerMap;
    Settings *settingsDialog;
};
