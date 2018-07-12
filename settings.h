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
#include <QDialog>
#include <QMap>

/**
 * @brief The Ui namespace
 */
namespace Ui {
class Settings;
}

/**
 * @brief The LayerTemplate struct
 */
struct LayerTemplate {
    QString name;
    QList<int> scales;
    LayerTemplate( const QString &n = QString(), QList<int> s = QList<int>()) : name( n ), scales( s ) {}
};

/**
 * @brief The Settings class
 */
class Settings : public QDialog {
    Q_OBJECT
    Q_ENUMS( Templates )

public:
    enum Templates {
        NoTemplate = -1,
        Legacy,
        Windows7,
        Windows10,
        Custom,

        TemplateCount
    };

    explicit Settings( QWidget *parent = nullptr );
    ~Settings();
    QList<int> currentScales() const;

private:
    Ui::Settings *ui;
    QMap<Templates,LayerTemplate> layerTemplates;
};
