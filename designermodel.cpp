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
#include "designermodel.h"
#include "designerlayer.h"
#include "designer.h"

/**
 * @brief DesignerModel::rowCount
 * @param parent
 * @return
 */
int DesignerModel::rowCount( const QModelIndex &parent ) const {
    if ( parent.isValid())
        return 0;

    return Designer::instance()->layers.count();
}

/**
 * @brief DesignerModel::data
 * @param index
 * @param role
 * @return
 */
QVariant DesignerModel::data( const QModelIndex &index, int role ) const {
    if ( !index.isValid())
        return QVariant();

    if ( role == Qt::DisplayRole ) {
        return Designer::instance()->layers.at( index.row())->name();
    } else if ( role == Qt::DecorationRole ) {
        if ( Designer::instance()->layers.at( index.row())->type() == DesignerLayer::Types::Text )
            return this->textIcon;
    }

    return QVariant();
}

/**
 * @brief DesignerModel::resetModel
 */
void DesignerModel::resetModel() {
    this->beginResetModel();
    this->endResetModel();
}
