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
#include "designer.h"
#include "mainwindow.h"
#include "ui_designer.h"
#include <QBitmap>
#include <QColorDialog>
#include <QRectF>
#include <QFileDialog>
#include <QDir>
#include <QInputDialog>
#include "designermodel.h"
#include "shapelayer.h"
#include "textlayer.h"
#include <QDebug>
#include "imagelayer.h"
#include "variable.h"
#include <QMessageBox>

/**
 * @brief Designer::Designer
 * @param parent
 */
Designer::Designer( QWidget *parent ) : QMainWindow( parent ), ui( new Ui::Designer ), model( nullptr ), addMenu( new QMenu()) {
    this->ui->setupUi( this );

    // set up graphics view
    this->ui->graphicsView->setRenderHints( QPainter::Antialiasing | QPainter::SmoothPixmapTransform );
    this->ui->graphicsView->setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
    this->ui->graphicsView->setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOff );

    // set up scene
    this->scene = new QGraphicsScene();
    this->scene->setSceneRect( QRectF( 0, 0, 256, 256 ));
    this->scene->setBackgroundBrush( QBrush( QPixmap( ":/icons/transparency" )));
    this->ui->graphicsView->setScene( this->scene );

    // setup ui element controls
    this->setupShape();
    this->setupText();

    // layer property change lambda
    auto changeProperty = [ this ]( Properties property, const QVariant &value ) {
        if ( this->currentLayer() == nullptr )
            return;

        if ( this->currentLayer()->item() == nullptr )
            return;

        switch ( property ) {
        case Opacity:
            this->currentLayer()->item()->setOpacity( value.toDouble() / 100.0 );
            break;

        case Scale:
            this->currentLayer()->setScale( value.toDouble() / 100.0 );
            break;

        case HorizontalOffset:
            this->currentLayer()->setHorizontalOffset( value.toInt());
            break;

        case VerticalOffset:
            this->currentLayer()->setVerticalOffset( value.toInt());
            break;

        case NoProperty:
            return;
        }
        this->currentLayer()->adjust();
    };

    // property change triggers
    this->connect( this->ui->opacitySlider, &QSlider::valueChanged, [ this, changeProperty ]( int value ) { changeProperty( Opacity, value ); } );
    this->connect( this->ui->scaleSlider, &QSlider::valueChanged, [ this, changeProperty ]( int value ) { changeProperty( Scale, value ); } );
    this->connect( this->ui->horizontalSlider, &QSlider::valueChanged, [ this, changeProperty ]( int value ) { changeProperty( HorizontalOffset, value ); } );
    this->connect( this->ui->verticalSlider, &QSlider::valueChanged, [ this, changeProperty ]( int value ) { changeProperty( VerticalOffset, value ); } );
}

/**
 * @brief Designer::currentLayer
 * @return
 */
DesignerLayer *Designer::currentLayer() const {
    const QModelIndex index( this->ui->layerView->currentIndex());

    if ( !index.isValid()) {
        this->ui->toolsDock->setDisabled( true );
        return nullptr;
    }

    return this->layers.at( index.row());
}

/**
 * @brief Designer::colourChanged
 * @param target
 * @param colour
 */
void Designer::colourChanged( ColourTarget target, const QColor &colour ) {
    QPixmap pixmap( ":/icons/colour" );
    QBitmap mask( pixmap.createMaskFromColor( Qt::transparent, Qt::MaskInColor ));

    pixmap.fill( colour );
    pixmap.setMask( mask );

    if ( target == BrushTarget ) {
        ShapeLayer *shape( qobject_cast<ShapeLayer *>( this->currentLayer()));
        if ( shape == nullptr )
            return;

        shape->brush.setColor( colour );

        if ( shape->shape() == ShapeLayer::Shapes::Ellipse )
            shape->ellipseItem->setBrush( shape->brush );
        else if ( shape->shape() == ShapeLayer::Shapes::Rectangle )
            shape->rectItem->setBrush( shape->brush );

        this->ui->brushColourButton->setIcon( QIcon( pixmap ));
    } else if ( target == PenTarget ) {
        ShapeLayer *shape( qobject_cast<ShapeLayer *>( this->currentLayer()));
        if ( shape == nullptr )
            return;

        shape->pen.setColor( colour );

        if ( shape->shape() == ShapeLayer::Shapes::Ellipse )
            shape->ellipseItem->setPen( shape->pen );
        else if ( shape->shape() == ShapeLayer::Shapes::Rectangle )
            shape->rectItem->setPen( shape->pen );

        this->ui->penColourButton->setIcon( QIcon( pixmap ));
    } else if ( target == TextTarget ) {
        TextLayer *text( qobject_cast<TextLayer *>( this->currentLayer()));
        if ( text == nullptr )
            return;

        text->textItem->setDefaultTextColor( colour );
        this->ui->textColourButton->setIcon( QIcon( pixmap ));
    }
}

/**
 * @brief Designer::~Designer
 */
Designer::~Designer() {
    this->disconnect( this->ui->brushColourButton, SLOT( pressed()));
    this->disconnect( this->ui->penColourButton, SLOT( pressed()));
    this->disconnect( this->ui->textColourButton, SLOT( pressed()));
    this->disconnect( this->ui->boldButton, SIGNAL( toggled( bool )));
    this->disconnect( this->ui->italicButton, SIGNAL( toggled( bool )));
    this->disconnect( this->ui->underlineButton, SIGNAL( toggled( bool )));
    this->disconnect( this->ui->fontCombo, SLOT( currentFontChanged( QFont )));
    this->disconnect( this->ui->layerView->selectionModel(), SLOT( selectionChanged( QItemSelection, QItemSelection )));
    this->disconnect( this->ui->addButton, SLOT( clicked()));
    this->disconnect( this->ui->removeButton, SLOT( clicked()));
    this->disconnect( this->ui->renameButton, SLOT( clicked()));
    this->disconnect( this->ui->opacitySlider, SLOT( valueChanged( int )));
    this->disconnect( this->ui->scaleSlider, SLOT( valueChanged( int )));
    this->disconnect( this->ui->horizontalSlider, SLOT( valueChanged( int )));
    this->disconnect( this->ui->verticalSlider, SLOT( valueChanged( int )));

    delete this->addMenu;
    delete this->scene;
    delete this->ui;
}

/**
 * @brief Designer::setupLayers
 */
void Designer::setupLayers() {
    // initialize model
    this->model = new DesignerModel();
    this->ui->layerView->setModel( this->model );

    // index change lambda
    this->connect( this->ui->layerView->selectionModel(), &QItemSelectionModel::selectionChanged, [ this ]() {
        const QModelIndex index( this->ui->layerView->currentIndex());

        // disable/enable tools dock
        this->ui->toolsDock->setEnabled( index.isValid());

        if ( !index.isValid())
            return;

        // switch current tool on layer change
        if ( this->layers.at( index.row())->type() == DesignerLayer::Types::Text ) {
            this->ui->stackedWidget->setCurrentIndex( Text );

            TextLayer *text( qobject_cast<TextLayer *>( this->currentLayer()));
            if ( text == nullptr )
                return;

            this->ui->textEdit->setText( text->textItem->toPlainText());
            this->ui->pointSizeSlider->setValue( text->font.pointSize());
            this->colourChanged( TextTarget, text->textItem->defaultTextColor());
            this->ui->fontCombo->setCurrentFont( text->font );
            this->ui->boldButton->setChecked( text->font.bold());
            this->ui->italicButton->setChecked( text->font.italic());
            this->ui->underlineButton->setChecked( text->font.underline());
            this->ui->fontCombo->setCurrentFont( text->font );
        } else if ( this->layers.at( index.row())->type() == DesignerLayer::Types::Shape ) {
            this->ui->stackedWidget->setCurrentIndex( Shape );

            ShapeLayer *shape( qobject_cast<ShapeLayer *>( this->currentLayer()));
            if ( shape == nullptr )
                return;

            this->colourChanged( PenTarget, shape->pen.color());
            this->colourChanged( BrushTarget, shape->brush.color());
            this->ui->penSizeSlider->setValue( shape->pen.width());
        } else if ( this->layers.at( index.row())->type() == DesignerLayer::Types::Image ) {
            this->ui->stackedWidget->setCurrentIndex( Image );
            //ImageLayer *image( qobject_cast<ImageLayer *>( this->currentLayer()));
            //if ( image == nullptr )
            //    return;
        } else
            this->ui->toolsDock->setEnabled( false );

        if ( this->currentLayer()->item() != nullptr ) {
            this->ui->opacitySlider->setValue( static_cast<int>( this->currentLayer()->item()->opacity() * 100 ));
            this->ui->scaleSlider->setValue( static_cast<int>( this->currentLayer()->scale() * 100 ));
            this->ui->horizontalSlider->setValue( this->currentLayer()->horizontalOffset());
            this->ui->verticalSlider->setValue( this->currentLayer()->verticalOffset());
            this->currentLayer()->adjust();
        } else {
            this->ui->toolsDock->setEnabled( false );
        }
    } );

    // add demo layers
    // TODO: delete on close
    this->addLayer( new ShapeLayer( this->scene, ShapeLayer::Shapes::Ellipse ));
    this->addLayer( new TextLayer( this->scene, "Aa" ));

    // reset model
    this->model->resetModel();
    this->ui->toolsDock->setEnabled( false );

    // add layer lambda
    // TODO: disconnect me?
    // TODO: button tests
    this->addMenu->addAction( this->tr( "Add text item" ), [ this ]() {
        bool ok;
        const QString name( QInputDialog::getText( this, this->tr( "Add text layer" ), this->tr( "Name:" ), QLineEdit::Normal, this->tr( "Aa" ), &ok ));

        if ( ok && !name.isEmpty()) {
            TextLayer *text( new TextLayer( this->scene, name ));

            text->setName( name );
            this->addLayer( text );
        }
    } );
    this->addMenu->addAction( this->tr( "Add ellipse item" ), [ this ]() { this->addLayer( new ShapeLayer( this->scene, ShapeLayer::Shapes::Ellipse )); } );
    this->addMenu->addAction( this->tr( "Add rectangle item" ), [ this ]() { this->addLayer( new ShapeLayer( this->scene, ShapeLayer::Shapes::Rectangle )); } );
    this->addMenu->addAction( this->tr( "Add image item" ), [ this ]() {
        QPixmap pixmap;
        bool ok;
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
            return;

        // pre-render svs to maximum scale (ignores aspect ratio)
        if ( fileName.endsWith( ".svg" )) {
            QIcon icon;

            icon.addFile( fileName, QSize( Ui::MaximumScale, Ui::MaximumScale ));
            if ( icon.isNull()) {
                QMessageBox::critical( this, this->tr( "Image selector" ),
                                       this->tr( "Invalid svg. Try another one." ),
                                       QMessageBox::Ok );
            }

            pixmap = icon.pixmap( Ui::ThresholdScale, Ui::ThresholdScale );
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

        // finally add layer
        if ( ok )
            this->addLayer( new ImageLayer( this->scene, pixmap ));
    } );

    this->connect( this->ui->addButton, &QToolButton::clicked, [ this ]() {
        this->addMenu->exec( QCursor::pos());
        this->model->resetModel();
    } );

    // TODO: remove layer lambda
    this->connect( this->ui->removeButton, &QToolButton::clicked, [ this ]() {} );

    // rename layer lambda
    this->connect( this->ui->renameButton, &QToolButton::clicked, [ this ]() {
        bool ok;

        if ( this->currentLayer() == nullptr )
            return;

        const QString name( QInputDialog::getText( this, this->tr( "Rename layer" ), this->tr( "New name:" ), QLineEdit::Normal, this->currentLayer()->name(), &ok ));
        if ( ok && !name.isEmpty()) {
            this->currentLayer()->setName( name );
            this->model->resetModel();
        }
    } );
}

/**
 * @brief Designer::setupText
 */
void Designer::setupText() {
    // text colour picker lambda
    this->connect( this->ui->textColourButton, &QPushButton::pressed, [ this ] () {
        TextLayer *text( qobject_cast<TextLayer *>( this->currentLayer()));
        if ( text == nullptr )
            return;

        QColor colour( QColorDialog::getColor( text->textItem->defaultTextColor(), this ));
        if ( !colour.isValid())
            return;

        this->colourChanged( TextTarget, colour );
    } );

    // bold lambda
    this->connect( this->ui->boldButton, &QToolButton::toggled, [ this ]( bool enabled ) {
        TextLayer *text( qobject_cast<TextLayer *>( this->currentLayer()));
        if ( text == nullptr )
            return;

        text->font.setBold( enabled );
        text->textItem->setFont( text->font );
        text->adjust();
    } );

    // italic lambda
    this->connect( this->ui->italicButton, &QToolButton::toggled, [ this ]( bool enabled ) {
        TextLayer *text( qobject_cast<TextLayer *>( this->currentLayer()));
        if ( text == nullptr )
            return;

        text->font.setItalic( enabled );
        text->textItem->setFont( text->font );
        text->adjust();
    } );

    // underline lambda
    this->connect( this->ui->underlineButton, &QToolButton::toggled, [ this ]( bool enabled ) {
        TextLayer *text( qobject_cast<TextLayer *>( this->currentLayer()));
        if ( text == nullptr )
            return;

        text->font.setUnderline( enabled );
        text->textItem->setFont( text->font );
        text->adjust();
    } );

    // font lambda
    this->connect( this->ui->fontCombo, &QFontComboBox::currentFontChanged, [ this ]( const QFont &font ) {
        TextLayer *text( qobject_cast<TextLayer *>( this->currentLayer()));
        if ( text == nullptr )
            return;

        text->font.setFamily( font.family());
        text->textItem->setFont( text->font );
        text->adjust();
    } );
}

/**
 * @brief Designer::setupShape
 */
void Designer::setupShape() {
    // brush colour picker lambda
    this->connect( this->ui->brushColourButton, &QPushButton::pressed, [ this ] () {
        ShapeLayer *shape( qobject_cast<ShapeLayer *>( this->currentLayer()));
        if ( shape == nullptr )
            return;

        QColor colour( QColorDialog::getColor( shape->brush.color(), this ));
        if ( !colour.isValid())
            return;

        this->colourChanged( BrushTarget, colour );
    } );

    // pen colour picker lambda
    this->connect( this->ui->penColourButton, &QPushButton::pressed, [ this ] () {
        ShapeLayer *shape( qobject_cast<ShapeLayer *>( this->currentLayer()));
        if ( shape == nullptr )
            return;

        QColor colour( QColorDialog::getColor( shape->pen.color(), this ));
        if ( !colour.isValid())
            return;

        this->colourChanged( PenTarget, colour );
    } );
}

/**
 * @brief Designer::addLayer
 */
void Designer::addLayer( DesignerLayer *layer ) {
    if ( layer == nullptr )
        return;

    this->layers << layer;

    const QModelIndex index( this->model->index( this->layers.indexOf( layer ), 0 ));
    if ( index.isValid())
        this->ui->layerView->setCurrentIndex( index );
}

/**
 * @brief Designer::on_penSizeSlider_valueChanged
 * @param value
 */
void Designer::on_penSizeSlider_valueChanged( int value ) {
    ShapeLayer *shape( qobject_cast<ShapeLayer *>( this->currentLayer()));
    if ( shape == nullptr )
        return;

    shape->pen.setWidth( value );

    if ( shape->shape() == ShapeLayer::Shapes::Ellipse )
        shape->ellipseItem->setPen( shape->pen );
    else if ( shape->shape() == ShapeLayer::Shapes::Rectangle )
        shape->rectItem->setPen( shape->pen );
}

/**
 * @brief Designer::on_textEdit_textChanged
 * @param text
 */
void Designer::on_textEdit_textChanged( const QString &text ) {
    TextLayer *textLayer( qobject_cast<TextLayer *>( this->currentLayer()));
    if ( textLayer == nullptr )
        return;

    textLayer->textItem->setPlainText( text );
    textLayer->adjust();
}

/**
 * @brief Designer::on_pointSizeSlider_valueChanged
 * @param value
 */
void Designer::on_pointSizeSlider_valueChanged( int value ) {
    TextLayer *textLayer( qobject_cast<TextLayer *>( this->currentLayer()));
    if ( textLayer == nullptr )
        return;

    textLayer->font.setPointSize( value );
    textLayer->textItem->setFont( textLayer->font );
    textLayer->adjust();
}

/**
 * @brief Designer::on_exportButton_clicked
 */
void Designer::on_exportButton_clicked() {
    QPixmap pixmap( 512, 512 );
    pixmap.fill( Qt::transparent );
    QPainter painter( &pixmap );

    QBrush background( this->scene->backgroundBrush());
    this->scene->setBackgroundBrush( QBrush( Qt::transparent ));
    painter.setRenderHint( QPainter::Antialiasing );
    this->scene->render( &painter );
    MainWindow::instance()->setPixmap( pixmap );
    this->scene->setBackgroundBrush( background );

    this->close();
}
