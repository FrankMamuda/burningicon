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
#include <QSlider>

/**
 * @brief The Slider class
 */
class Slider final : public QSlider {
    Q_OBJECT

public:
    explicit Slider( QWidget *parent = nullptr ) : QSlider( parent ) {}
    ~Slider() = default;

public slots:
    void setValue( int value ) { emit this->valueChanged( value ); QSlider::setValue( value ); }
};
