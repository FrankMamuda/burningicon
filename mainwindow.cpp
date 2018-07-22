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
// TODO:
//
// GENERAL:
//  progressbar and QtConcurrent
//  aliasing layers
//  disabling layers
//  generic icon creation
//  project save/load
//  icon crop dialog
//  icon resource export
//  make generic icons for layers only?
//
// GENERIC ICON MAKER:
//  multi layered design
//  multi-type design (text, shape, etc.)
//  basic bezel support (draw, fill, etc.)
//  image import
//  save settings/project
//
// FIXME:
//  occasional indexOutOfRange on addLayer
//

//
// includes
//
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDir>
#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>
#include <designer.h>
#include "iconwriter.h"
#include "layermodel.h"
#include "main.h"
#include "variable.h"
#include <QDebug>
#include "settings.h"
#include "layer.h"

/**
 * @brief MainWindow::MainWindow
 * @param parent
 */
MainWindow::MainWindow( QWidget *parent ) :
    QMainWindow( parent ),
    ui( new Ui::MainWindow ),
    model( new LayerModel()),
    settingsDialog( new Settings( this )) {

    this->ui->setupUi( this );
}

/**
 * @brief MainWindow::scaleToLayer
 * @param scale
 * @return
 */
Layer *MainWindow::scaleToLayer( int scale ) const {
    Layer *layer = nullptr;

    if ( this->layerMap.contains( scale ))
        layer = this->layerMap[scale];

    return layer;
}

/**
 * @brief MainWindow::setPixmap
 * @param pixmap
 */
void MainWindow::setPixmap( const QPixmap &px ) {
    QPixmap pixmap( px );

    // resize if larger than 512
    if ( pixmap.width() > Ui::MaximumScale || pixmap.height() > Ui::MaximumScale ) {
        if ( pixmap.width() > pixmap.height())
            pixmap = pixmap.scaledToHeight( Ui::MaximumScale, Qt::SmoothTransformation );
        else if ( pixmap.width() < pixmap.height())
            pixmap = pixmap.scaledToWidth( Ui::MaximumScale, Qt::SmoothTransformation );
        else
            pixmap = pixmap.scaled( Ui::MaximumScale, Ui::MaximumScale, Qt::IgnoreAspectRatio, Qt::SmoothTransformation );
    }
    this->scaled = pixmap;

    // generate mipmaps
    this->generateLayers( this->settingsDialog->currentScales());

    // update pixmap and display its scale
    if ( !this->layers.isEmpty())
        this->ui->pixmapLabel->setPixmap( this->layers.last()->pixmap );

    this->ui->stackedWidget->setCurrentIndex( Preview );
}

/**
 * @brief MainWindow::initialize
 */
void MainWindow::initialize() {
    // set the layer model
    this->ui->layerView->setModel( this->model );

    // disable layers if no image is loaded
    this->connect( this->ui->stackedWidget, &QStackedWidget::currentChanged, [ this ]( int index ) {
        this->ui->dockWidget->setEnabled( index == Preview );
        this->ui->actionExport->setEnabled( index == Preview );
        this->ui->actionClear->setEnabled( index == Preview );
    } );
    this->ui->dockWidget->setEnabled( false );
    this->ui->actionExport->setEnabled( false );
    this->ui->actionClear->setEnabled( false );

    // layer dock button enabler/disabler
    auto buttonTest = [ this ]() {
        bool valid = this->ui->layerView->currentIndex().isValid();

        // enable buttons only for valid indexes
        this->ui->removeButton->setEnabled( valid );
        this->ui->overrideButton->setEnabled( valid );
        this->ui->pngButton->setEnabled( valid );
        this->ui->overrideButton->setChecked( valid );
        this->ui->pngButton->setEnabled( valid );

        if ( valid ) {
            Layer *layer( this->scaleToLayer( this->ui->layerView->currentIndex().data( LayerModel::ScaleRole ).toInt()));

            if ( layer != nullptr ) {
                this->ui->overrideButton->setChecked( layer->isOverriden());
                this->ui->pngButton->setChecked( layer->isCompressed());
            }
        }
    };

    // open pixmap lambda
    auto openPixmap = [ this ]( bool &ok ) {
        QPixmap pixmap;
        QString path( Variable::instance()->string( "previousOpenPath" ));
        const QDir dir( path );

        // check previous path
        if ( path.isEmpty() || !dir.exists())
            path = QDir::currentPath();

        // failure by default
        ok = false;

        // get fileName
        const QString fileName( QFileDialog::getOpenFileName( this, this->tr( "Open Image" ),
                                                              path,
                                                              this->tr( "Image Files (*.png *.jpg *.bmp *.svg)" )));

        if ( fileName.isEmpty())
            return pixmap;

        // pre-render svs to maximum scale (ignores aspect ratio)
        if ( fileName.endsWith( ".svg" )) {
            QIcon icon;

            icon.addFile( fileName, QSize( Ui::MaximumScale, Ui::MaximumScale ));
            if ( icon.isNull()) {
                QMessageBox::critical( this, this->tr( "Image selector" ),
                                       this->tr( "Invalid svg. Try another one." ),
                                       QMessageBox::Ok );
            }

            pixmap = icon.pixmap( Ui::MaximumScale, Ui::MaximumScale );
        } else {
            // validate pixmap
            if ( !pixmap.load( fileName ))
                QMessageBox::critical( this, this->tr( "Image selector" ),
                                       this->tr( "Invalid image. Try another image." ),
                                       QMessageBox::Ok );
        }

        // check if pixmap is valid
        if ( !pixmap.isNull())
            ok = true;

        // store new path
        Variable::instance()->setString( "previousOpenPath", QFileInfo( fileName ).absolutePath());

        // return loaded pixmap
        return pixmap;
    };

    // check button state on index change (disable by default)
    this->connect( this->ui->layerView->selectionModel(), &QItemSelectionModel::selectionChanged, [ this, buttonTest ]() {
        buttonTest();

        if ( !this->layers.isEmpty())
            this->ui->pixmapLabel->setPixmap( this->layers.at( this->ui->layerView->currentIndex().row())->pixmap );
        else
            this->ui->pixmapLabel->setPixmap( QPixmap());
    } );
    buttonTest();

    // add new layer lambda
    this->connect( this->ui->addButton, &QToolButton::clicked, [ this ]() {
        bool ok;

        // get scale from an input dialog
        const int scale = QInputDialog::getInt( this, this->tr( "New layer" ), this->tr( "Layer scale:" ), 16, Ui::MinimumScale, Ui::MaximumScale, 1, &ok );

        // validate scale
        if ( ok && scale >= Ui::MinimumScale ) {
            if ( this->layerMap.contains( scale )) {
                QMessageBox::critical( this, this->tr( "Add layer" ),
                                       this->tr( "Layer %1x%1 already exists." ).arg( scale ),
                                       QMessageBox::Ok );
                return;
            }

            this->addLayer( scale, true );
        }
    } );

    // remove layer lambda
    this->connect( this->ui->removeButton, &QToolButton::clicked, [ this ]() {
        const QModelIndex &index = this->ui->layerView->currentIndex();

        if ( !index.isValid())
            return;

        this->removeLayer( index.data( LayerModel::ScaleRole ).toInt());
    } );

    // override layer with a different image lambda
    this->connect( this->ui->overrideButton, &QToolButton::clicked, [ this, openPixmap ]( bool checked ) {
        QModelIndex index( this->ui->layerView->currentIndex());

        if ( index.isValid()) {
            const int scale = this->ui->layerView->currentIndex().data( LayerModel::ScaleRole ).toInt();

            if ( this->layerMap.contains( scale )) {
                if ( checked ) {
                    bool ok;
                    QPixmap pixmap;

                    // get pixmap
                    pixmap = openPixmap( ok );
                    if ( !ok )
                        return;

                    // override layer pixmap
                    this->overrideLayer( scale, pixmap );
                } else {
                    this->restoreLayer( scale );
                }
            }
        }
    } );

    // png mode toggle
    this->connect( this->ui->pngButton, &QToolButton::clicked, [ this ]( bool checked ) {
        QModelIndex index( this->ui->layerView->currentIndex());

        if ( index.isValid()) {
            Layer *layer( this->scaleToLayer( this->ui->layerView->currentIndex().data( LayerModel::ScaleRole ).toInt()));

            if ( layer != nullptr )
                layer->setCompressed( checked );
        }
    } );

    // image selector lambda
    this->connect( this->ui->openButton, &QPushButton::clicked, [ this, openPixmap ]() {
        bool ok;
        QPixmap pixmap;

        // get pixmap
        pixmap = openPixmap( ok );
        if ( !ok )
            return;

        // make mipMaps
        this->setPixmap( pixmap );
    } );

    // repopulate lambda
    this->connect( this->ui->repopulateButton, &QPushButton::clicked, [ this, buttonTest ]() {
        if ( QMessageBox::question( this, Ui::AppName,
                               this->tr( "Repopulate layers with scales defined in settings?\nThis will delete the existing layers." ),
                               QMessageBox::Yes | QMessageBox::No ) == QMessageBox::Yes )
            this->generateLayers( this->settingsDialog->currentScales());

        // update pixmap and display its scale
        if ( !this->layers.isEmpty())
            this->ui->pixmapLabel->setPixmap( this->layers.last()->pixmap );

        buttonTest();
    } );

    // icon maker lambda
    this->connect( this->ui->makeButton, &QPushButton::clicked, [ this ]() {
        Designer::instance()->show();
    } );

    // initialize icon designer
    Designer::instance();
    Designer::instance()->setupLayers();

    // add to garbage man
    GarbageMan::instance()->add( this );
}

/**
 * @brief MainWindow::~MainWindow
 */
MainWindow::~MainWindow() {
    this->clearLayers();

    // disconnect all lambdas, etc.
    this->disconnect( this->ui->makeButton, SLOT( clicked()));
    this->disconnect( this->ui->openButton, SLOT( clicked()));
    this->disconnect( this->ui->addButton, SLOT( clicked()));
    this->disconnect( this->ui->removeButton, SLOT( clicked()));
    this->disconnect( this->ui->overrideButton, SLOT( clicked( bool )));
    this->disconnect( this->ui->pngButton, SLOT( clicked( bool )));
    this->disconnect( this->ui->repopulateButton, SLOT( clicked()));

    this->disconnect( this->ui->layerView->selectionModel(), SLOT( selectionChanged( QItemSelection, QItemSelection )));
    this->disconnect( this->ui->stackedWidget, SLOT( currentChanged( int )));

    // get rid of ui
    delete this->ui;
}

/**
 * @brief MainWindow::on_actionExport_triggered
 */
void MainWindow::on_actionExport_triggered() {
    const QPixmap *pixmap( &this->scaled );
    bool ok = true;
    QString path( Variable::instance()->string( "previousSavePath" ));
    const QDir dir( path );

    // check previous path
    if ( path.isEmpty() || !dir.exists())
        path = QDir::currentPath();

    // check if any pixmap selected
    if ( pixmap == nullptr ) {
        QMessageBox::critical( this, Ui::AppName, this->tr( "No image selected." ), QMessageBox::Ok );
        return;
    }

    // check if a valid pixmap selected
    if ( pixmap->isNull() || pixmap->width() < Ui::MinimumScale || pixmap->height() < Ui::MinimumScale ) {
        QMessageBox::critical( this, Ui::AppName, this->tr( "Invalid image selected." ), QMessageBox::Ok );
        return;
    }

    // get fileName
    QString fileName( QFileDialog::getSaveFileName( this, this->tr( "Save Icon" ), path, this->tr( "Icon Files (*.ico)" )));
    if ( fileName.isEmpty())
        return;

    // check if exists
    QFile file( fileName );

    // add extension
    if ( !fileName.endsWith( ".ico" ))
        fileName.append( ".ico" );

    if ( file.exists()) {
        // check if file is readable
        if ( !file.open( QIODevice::ReadOnly )) {
            QMessageBox::critical( this, Ui::AppName,
                                   this->tr( "Cannot open destination file." ),
                                   QMessageBox::Ok );
            return;
        }
        file.close();

        // check if file is writable
        if ( !file.open( QIODevice::WriteOnly )) {
            QMessageBox::critical( this, Ui::AppName,
                                   this->tr( "Cannot write destination file." ),
                                   QMessageBox::Ok );
            return;
        }
        file.close();
    }

    if ( !ok )
        return;


    // store new path
    Variable::instance()->setString( "previousSavePath", QFileInfo( fileName ).absolutePath());

    // write out
    IconWriter::instance()->write( fileName, this->layers );
}

/**
 * @brief MainWindow::generateLayers
 * @param scales
 */
void MainWindow::generateLayers( const QList<int> scales ) {
    this->clearLayers();

    // build unique mipmaps for each scale
    // TODO: use fast downscaling on create and smooth on write
    foreach ( const int scale, scales )
        this->addLayer( scale, false );

    this->resetModel();
}

/**
 * @brief MainWindow::generateLayers
 * @param scales
 */
void MainWindow::addLayer( int scale, bool reset ) {
    bool compress = Variable::instance()->isEnabled( "settings/compress" );

    if ( this->scaled.isNull())
        return;

    if ( scale < Ui::MinimumScale || scale > Ui::MaximumScale )
        return;

    if ( compress ) {
        if ( scale < Variable::instance()->integer( "settings/compressThreshold" ))
            compress = false;
    }

    Layer *layer( new Layer( this->scaled, scale, compress ));
    this->layers << layer;
    this->layerMap[scale] = layer;

    if ( reset )
        this->resetModel();
}

/**
 * @brief MainWindow::overrideLayer
 * @param scale
 * @param pixmap
 */
void MainWindow::overrideLayer( int scale, const QPixmap &pixmap ) {
    QPixmap px( pixmap );

    if ( pixmap.isNull() || pixmap.width() < Ui::MinimumScale || pixmap.height() < Ui::MinimumScale || scale < Ui::MinimumScale || !this->layerMap.contains( scale ))
        return;

    if ( pixmap.width() != scale || pixmap.height() != scale )
        px = pixmap.scaled( scale, scale, Qt::IgnoreAspectRatio, Qt::SmoothTransformation );

    Layer *layer( this->layerMap[scale] );
    layer->pixmap = px;
    layer->setOverriden( true );

    this->resetModel();
}

/**
 * @brief MainWindow::restoreLayer
 * @param scale
 */
void MainWindow::restoreLayer( int scale ) {
    if ( this->scaled.isNull() || !this->layerMap.contains( scale ))
        return;

    Layer *layer( this->layerMap[scale] );
    layer->pixmap = this->scaled.scaled( scale, scale, Qt::IgnoreAspectRatio, Qt::SmoothTransformation );
    layer->setOverriden( false );

    this->resetModel();
}

/**
 * @brief MainWindow::removeLayer
 * @param scale
 */
void MainWindow::removeLayer( int scale ) {
    if ( this->scaled.isNull() || !this->layerMap.contains( scale ))
        return;

    Layer *layer( this->layerMap[scale] );
    this->layers.removeAll( layer );
    this->layerMap.remove( scale );
    delete layer;
    this->resetModel();
}

/**
 * @brief MainWindow::reset
 */
void MainWindow::resetModel() {
    std::sort( this->layers.begin(), this->layers.end(), []( Layer *one, Layer *two ) { return one->scale() < two->scale(); } );
    this->model->resetModel();
}

/**
 * @brief MainWindow::clearLayers
 */
void MainWindow::clearLayers() {
    qDeleteAll( this->layers );
    this->layers.clear();
}

/**
 * @brief MainWindow::on_actionClear_triggered
 */
void MainWindow::on_actionClear_triggered() {
    this->ui->stackedWidget->setCurrentIndex( Load );
    this->clearLayers();
    this->resetModel();
}

/**
 * @brief MainWindow::on_actionSettings_triggered
 */
void MainWindow::on_actionSettings_triggered() {
    this->settingsDialog->exec();
}
