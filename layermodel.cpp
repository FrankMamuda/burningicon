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
#include "layer.h"
#include "layermodel.h"
#include "mainwindow.h"

/**
 * @brief LayerModel::rowCount
 * @param parent
 * @return
 */
int LayerModel::rowCount( const QModelIndex &parent ) const {
    if ( parent.isValid())
        return 0;

    return MainWindow::instance()->layers.count();
}

/**
 * @brief LayerModel::data
 * @param index
 * @param role
 * @return
 */
QVariant LayerModel::data( const QModelIndex &index, int role ) const {
    if ( !index.isValid())
        return QVariant();

    if ( role == Qt::DisplayRole ) {
        const int scale = MainWindow::instance()->layers.at( index.row())->scale();
        const bool doubleScale = MainWindow::instance()->layers.at( index.row())->isDoubleScale();

        return doubleScale ? QString( "%1x%1@2x" ).arg( scale / 2 ) : QString( "%1x%1" ).arg( scale );
    } else if ( role == Qt::DecorationRole ) {
        return MainWindow::instance()->layers.at( index.row())->pixmap.scaled( 16, 16, Qt::IgnoreAspectRatio, Qt::FastTransformation );
    } else if ( role == ScaleRole ) {
        return MainWindow::instance()->layers.at( index.row())->scale();
    }

    return QVariant();
}

/**
 * @brief LayerModel::resetModel
 * @param limit
 */
void LayerModel::resetModel() {
    this->beginResetModel();
    this->endResetModel();
}
