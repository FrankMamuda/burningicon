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
#include "designerlayer.h"
#include <QGraphicsScene>
#include <QGraphicsTextItem>

/**
 * @brief The TextItem namespace
 */
namespace TextItem {
static constexpr int MinimumPointSize = 8;
static constexpr int DefaultPointSize = 96;
}

/**
 * @brief The ImageLayer class
 */
class TextLayer final : public DesignerLayer {
    Q_OBJECT
    friend class Designer;

public:
    TextLayer( QGraphicsScene *scene = nullptr, const QString &text = QString());
    QGraphicsItem *item() override;
    void adjust() override;

public slots:
    void setScale( qreal scale ) override;

private:
    QGraphicsTextItem *textItem;
    QFont font;
};
