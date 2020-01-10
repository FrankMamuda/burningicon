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
#include "settings.h"
#include "ui_settings.h"
#include "variable.h"
#include "mainwindow.h"

/**
 * @brief Settings::Settings
 * @param parent
 */
Settings::Settings( QWidget *parent ) : QDialog( parent ), ui( new Ui::Settings ) {
    // set up ui
    this->ui->setupUi( this );

    // get custom layer scales from valiable
    const QStringList custom( Variable::instance()->string( "settings/customTemplate" ).split( "," ));
    QList<int> customValues;
    foreach ( const QString &num, custom ) {
        bool ok;
        const int value = num.toInt( &ok );

        if ( ok )
            customValues << value;
    }

    // fill out templates
    this->layerTemplates[Legacy]    = LayerTemplate( this->tr( "Windows XP" ),
                                                     QList<int>() << 16 << 32 << 48 );
    this->layerTemplates[Windows7]  = LayerTemplate( this->tr( "Windows Vista/7" ),
                                                     QList<int>() << 16 << 20 << 32 << 40 << 48 << 64 << 256 );
    this->layerTemplates[Windows10] = LayerTemplate( this->tr( "Windows 10" ),
                                                     QList<int>() << 16 << 20 << 24 << 28 << 30 << 31 << 32 << 40 << 42 << 47 << 48 << 56 << 60 << 63 << 84 << 256 );
    this->layerTemplates[macOS]     = LayerTemplate( this->tr( "macOS" ), QList<int>(), true );
    this->layerTemplates[Custom]    = LayerTemplate( this->tr( "Custom" ),
                                                     customValues );

    // add templates to comboBox
    for ( int y = 0; y < TemplateCount; y++ )
        this->ui->layerCombo->addItem( this->layerTemplates[static_cast<Templates>( y )].name, static_cast<Templates>( y ));

    // store current template in a variable
    Variable::instance()->bind( "settings/layerTemplate", this->ui->layerCombo );

    // compression state lambda
    auto compressionState = [ this ]() { this->ui->compressInteger->setEnabled( this->ui->compressBox->isChecked() && Variable::instance()->isDisabled( "settings/macOS" )); };

    // this lambda sets icon scales to an edit box
    auto displayScales = [ this, compressionState ]() {
        const Templates index = static_cast<Templates>( this->ui->layerCombo->currentIndex());
        QString sizes;
        int y = 0;

        // use only valid indexes
        if ( index == -1 || !this->layerTemplates.contains( index ))
            return;

        // get current template
        LayerTemplate layerTemplate = this->layerTemplates[index];

        if ( index == macOS ) {
            sizes = "16x16, 32x32, 64x64, 128x128, 256x256, 512x512, 1024, 16x16@2x, 32x32@2x, 128x128@2x, 256x256@2x";
            this->ui->compressBox->setDisabled( true );
            Variable::instance()->enable( "settings/macOS" );
        } else {
            // make a string of all available scales
            foreach ( const int size, layerTemplate.scales ) {
                sizes = sizes.append( "%1%2" ).arg( size ).arg( y == layerTemplate.scales.count() - 1 ? "" : ", " );
                y++;
            }
            this->ui->compressBox->setEnabled( true );
            Variable::instance()->disable( "settings/macOS" );
        }

        // set string to edit box
        this->ui->layerEdit->setText( qAsConst( sizes ));

        // disable/enable edit box
        this->ui->layerEdit->setEnabled( index == Custom );

        // hide/show compression integer
        compressionState();
    };

    // update scales on index change
    this->connect( this->ui->layerCombo, static_cast<void ( QComboBox::* )( int )>( &QComboBox::activated ), displayScales );
    displayScales();

    // update custom template
    this->connect( this->ui->layerEdit, &QLineEdit::editingFinished, [ this ]() {
        if ( static_cast<Templates>( this->ui->layerCombo->currentIndex()) == Custom ) {
            Variable::instance()->setString( "settings/customTemplate", this->ui->layerEdit->text());
        }
    } );

    // png compression variables
    Variable::instance()->bind( "settings/compressThreshold", this->ui->compressInteger );
    Variable::instance()->bind( "settings/compress", this->ui->compressBox );

    // hide/show compression integer
    this->connect( this->ui->compressBox, &QCheckBox::toggled, compressionState );
    compressionState();
}

/**
 * @brief Settings::~Settings
 */
Settings::~Settings() {
    this->disconnect( this->ui->layerCombo, SLOT( activated( int )));
    this->disconnect( this->ui->layerEdit, SLOT( editingFinished()));
    this->disconnect( this->ui->compressBox, SLOT( toggled( bool )));

    delete this->ui;
}

/**
 * @brief Settings::currentScales
 * @return
 */
QList<int> Settings::currentScales() const {
    const Templates index = static_cast<Templates>( this->ui->layerCombo->currentIndex());
    QString sizes;

    // use only valid indexes
    if ( index == -1 || !this->layerTemplates.contains( index ))
        return Ui::DefaultScales;

    // return current scales
    return this->layerTemplates[index].scales;
}
