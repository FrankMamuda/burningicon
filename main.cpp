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
#include "mainwindow.h"
#include "main.h"
#include "xmltools.h"
#include "variable.h"
#include "settings.h"
#include <QApplication>

/**
 * @brief qMain
 * @param argc
 * @param argv
 * @return
 */
int main( int argc, char *argv[] ) {
    QApplication a( argc, argv );

    // add variables
    Variable::instance()->add( "previousOpenPath", "" );
    Variable::instance()->add( "previousSavePath", "" );
    Variable::instance()->add( "settings/layerTemplate", static_cast<int>( Settings::Windows7 ));
    Variable::instance()->add( "settings/customTemplate", "16,32,48,64,256" );
    Variable::instance()->add( "settings/compressThreshold", Ui::ThresholdScale );
    Variable::instance()->add( "settings/compress", true );

    // read configuration
    XMLTools::instance()->read();

    // open main window
    MainWindow::instance();
    MainWindow::instance()->initialize();
    MainWindow::instance()->show();

    // clean up on exit
    qApp->connect( qApp, &QApplication::aboutToQuit, []() {
        XMLTools::instance()->write();
        GarbageMan::instance()->clear();

        delete GarbageMan::instance();
    } );

    return a.exec();
}
