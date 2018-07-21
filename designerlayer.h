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
#include <QGraphicsScene>
#include <QObject>

/**
 * @brief The DesignerLayer class
 */
class DesignerLayer : public QObject {
    Q_OBJECT
    Q_ENUMS( Types )
    Q_DISABLE_COPY( DesignerLayer )

public:
    enum class Types {
        NoType = -1,
        Text,
        Shape,
        Image,
        Bezel
    };
    explicit DesignerLayer( QGraphicsScene *s = nullptr );
    Types type() const { return this->m_type; }
    QGraphicsScene *scene() const { return this->m_scene; }
    QString name() const { return this->m_name; }

public slots:
    void setName( const QString &name ) { this->m_name = name; }
    void setType( Types type ) { this->m_type = type; }

private:
    Types m_type;
    QGraphicsScene *m_scene;
    QString m_name;
};
