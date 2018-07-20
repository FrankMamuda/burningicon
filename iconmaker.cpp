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
#include "iconmaker.h"
#include "mainwindow.h"
#include "ui_iconmaker.h"
#include <QBitmap>
#include <QColorDialog>
#include <QRectF>
#include <QFileDialog>
#include <QDir>

/**
 * @brief MainWindow::MainWindow
 * @param parent
 */
IconMaker::IconMaker( QWidget *parent ) : QMainWindow( parent ), ui( new Ui::IconMaker ) {
    this->ui->setupUi( this );

    this->ui->graphicsView->setRenderHints( QPainter::Antialiasing | QPainter::SmoothPixmapTransform );
    this->ui->graphicsView->setHorizontalScrollBarPolicy ( Qt::ScrollBarAlwaysOff );
    this->ui->graphicsView->setVerticalScrollBarPolicy ( Qt::ScrollBarAlwaysOff );

    this->scene = new QGraphicsScene();
    const QRectF rect( 0, 0, 256, 256 );
    scene->setSceneRect( rect );

    this->pen = QPen( Qt::black );
    this->brush = QBrush( Qt::darkGreen );

    this->ellipse = scene->addEllipse( rect, this->pen, this->brush );
    this->ui->ellipseSizelSlider->setValue( this->ui->ellipseSizelSlider->value());
    this->ui->penSizeSlider->setValue( this->ui->penSizeSlider->value());
    this->ui->graphicsView->setScene( scene );

    // brush colour picker lambda
    this->connect( this->ui->brushColourButton, &QPushButton::pressed, [ this ] () {
        QColor colour( QColorDialog::getColor( this->brush.color(), this ));
        if ( !colour.isValid())
            return;

        this->colourChanged( BrushTarget, colour );
    } );

    // pen colour picker lambda
    this->connect( this->ui->penColourButton, &QPushButton::pressed, [ this ] () {
        QColor colour( QColorDialog::getColor( this->pen.color(), this ));
        if ( !colour.isValid())
            return;

        this->colourChanged( PenTarget, colour );
    } );

    // text colour picker lambda
    this->connect( this->ui->textColourButton, &QPushButton::pressed, [ this ] () {
        QColor colour( QColorDialog::getColor( this->text->defaultTextColor(), this ));
        if ( !colour.isValid())
            return;

        this->colourChanged( TextTarget, colour );
    } );

    this->text = this->scene->addText( this->ui->textEdit->text(), this->font );
    this->on_pointSizeSlider_valueChanged( this->ui->pointSizeSlider->value());

    this->colourChanged( PenTarget, this->pen.color());
    this->colourChanged( BrushTarget, this->brush.color());
    this->colourChanged( TextTarget, QColor( Qt::white ));

    this->scene->setBackgroundBrush( QBrush( QPixmap( ":/icons/transparency" )));


    // bold lambda
    this->connect( this->ui->boldButton, &QToolButton::toggled, [ this ]( bool enabled ) {
        this->font.setBold( enabled );
        this->text->setFont( font );
        this->adjustText();
    } );

    // italic lambda
    this->connect( this->ui->italicButton, &QToolButton::toggled, [ this ]( bool enabled ) {
        this->font.setItalic( enabled );
        this->text->setFont( font );
        this->adjustText();
    } );

    // underline lambda
    this->connect( this->ui->underlineButton, &QToolButton::toggled, [ this ]( bool enabled ) {
        this->font.setUnderline( enabled );
        this->text->setFont( font );
        this->adjustText();
    } );
}

/**
 * @brief PropertyEditor::colourChanged
 * @param colour
 */
void IconMaker::colourChanged( ColourTarget target, const QColor &colour ) {
    QPixmap pixmap( ":/icons/colour" );
    QBitmap mask( pixmap.createMaskFromColor( Qt::transparent, Qt::MaskInColor ));

    pixmap.fill( colour );
    pixmap.setMask( mask );

    if ( target == BrushTarget ) {
        this->brush.setColor( colour );
        this->ellipse->setBrush( this->brush );
        this->ui->brushColourButton->setIcon( QIcon( pixmap ));
    } else if ( target == PenTarget ) {
        this->pen.setColor( colour );
        this->ellipse->setPen( this->pen );
        this->ui->penColourButton->setIcon( QIcon( pixmap ));
    } else if ( target == TextTarget ) {
        this->text->setDefaultTextColor( colour );
        this->ui->textColourButton->setIcon( QIcon( pixmap ));
    }
}

/**
 * @brief MainWindow::~MainWindow
 */
IconMaker::~IconMaker() {
    this->disconnect( this->ui->brushColourButton, SLOT( pressed()));
    this->disconnect( this->ui->penColourButton, SLOT( pressed()));
    this->disconnect( this->ui->textColourButton, SLOT( pressed()));
    this->disconnect( this->ui->boldButton, SIGNAL( toggled( bool )));
    this->disconnect( this->ui->italicButton, SIGNAL( toggled( bool )));
    this->disconnect( this->ui->underlineButton, SIGNAL( toggled( bool )));


    delete this->scene;
    delete this->ui;
}

/**
 * @brief MainWindow::on_ellipseSizelSlider_valueChanged
 * @param value
 */
void IconMaker::on_ellipseSizelSlider_valueChanged( int value ) {
    const qreal factor = static_cast<qreal>( value ) / 100.0;
    const QRectF rect( this->scene->sceneRect());

    qreal width = rect.width() * factor;
    qreal height = rect.height() * factor;
    QRectF scaled( rect.width() / 2.0 - width / 2.0,
                   rect.height() / 2.0 - height / 2.0,
                   width,
                   height
                   );

    this->ellipse->setRect( scaled );
}

/**
 * @brief MainWindow::on_penSizeSlider_valueChanged
 * @param value
 */
void IconMaker::on_penSizeSlider_valueChanged( int value ) {
    this->pen.setWidth( value );
    this->ellipse->setPen( pen );
}

/**
 * @brief MainWindow::on_textEdit_textChanged
 * @param arg1
 */
void IconMaker::on_textEdit_textChanged( const QString &text ) {
    this->text->setPlainText( text );
    this->adjustText();
}

/**
 * @brief MainWindow::on_pointSizeSlider_valueChanged
 * @param value
 */
void IconMaker::on_pointSizeSlider_valueChanged( int value ) {
    this->font.setPointSize( value );
    this->text->setFont( font );
    this->adjustText();
}

/**
 * @brief MainWindow::on_textXSlider_valueChanged
 * @param value
 */
void IconMaker::on_textXSlider_valueChanged( int ) {
    this->adjustText();
}

/**
 * @brief MainWindow::on_textYSlider_valueChanged
 * @param value
 */
void IconMaker::on_textYSlider_valueChanged( int ) {
    this->adjustText();
}

/**
 * @brief MainWindow::adjustText
 */
void IconMaker::adjustText() {
    QRectF sceneRect( this->scene->sceneRect());
    QRectF rect( this->text->sceneBoundingRect());
    this->text->setPos( sceneRect.width() / 2.0 - rect.width() / 2.0 + this->ui->textXSlider->value(),
                        sceneRect.height() / 2.0 - rect.height() / 2.0 + this->ui->textYSlider->value());
}

/**
 * @brief MainWindow::on_exportButton_clicked
 */
void IconMaker::on_exportButton_clicked() {
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
