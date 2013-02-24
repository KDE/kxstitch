/*
 * Copyright (C) 2010 by Stephen Allewell
 * stephen@mirramar.adsl24.co.uk
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#include "Boundary.h"

#include <QPainter>

#include "Element.h"


Boundary::Boundary()
    :   m_element(0)
{
}


Element *Boundary::element() const
{
    return m_element;
}


const QPoint *Boundary::node(const QPoint &pos) const
{
    for (int node = 0 ; node < 4 ; node++) {
        if (QRect(0, 0, 5, 5).translated(m_nodes[node]).contains(pos)) {
            return &m_nodes[node];
        }
    }

    return 0;
}


QRect Boundary::rectangle() const
{
    return m_rectangle;
}


Qt::CursorShape Boundary::cursor(const QPoint *node)
{
    static const Qt::CursorShape nodeCursors[2] = {Qt::SizeFDiagCursor, Qt::SizeBDiagCursor};
    Qt::CursorShape shape = Qt::ArrowCursor;

    for (int i = 0 ; i < 4 ; i++) {
        if (m_nodes[i] == *node) {
            shape = nodeCursors[i % 2];
        }
    }

    return shape;
}


bool Boundary::isValid() const
{
    return (m_element && m_rectangle.isValid());
}


void Boundary::setElement(Element *element)
{
    if (element != m_element) {
        m_element = element;

        if (m_element) {
            setRectangle(element->rectangle());
        }
    }
}


void Boundary::setRectangle(const QRect &rectangle)
{
    m_rectangle = rectangle;
    m_nodes[0] = rectangle.topLeft();
    m_nodes[1] = rectangle.topRight() + QPoint(1, 0);
    m_nodes[2] = rectangle.bottomRight() + QPoint(1, 1);
    m_nodes[3] = rectangle.bottomLeft() + QPoint(0, 1);
}


void Boundary::moveNode(const QPoint *node, const QPoint &pos)
{
    for (int i = 0 ; i < 4 ; i++) {
        if (m_nodes[i] == *node) {
            int dx = pos.x() - node->x();
            int dy = pos.y() - node->y();
            m_nodes[i] = pos;

            switch (i) {
            case 0:
                m_nodes[1] += QPoint(0, dy);
                m_nodes[3] += QPoint(dx, 0);
                break;

            case 1:
                m_nodes[0] += QPoint(0, dy);
                m_nodes[2] += QPoint(dx, 0);
                break;

            case 2:
                m_nodes[1] += QPoint(dx, 0);
                m_nodes[3] += QPoint(0, dy);
                break;

            case 3:
                m_nodes[0] += QPoint(dx, 0);
                m_nodes[2] += QPoint(0, dy);
                break;
            }

            m_rectangle = QRect(m_nodes[0], m_nodes[2] - QPoint(1, 1));
            m_element->setRectangle(m_rectangle);
            break;
        }
    }
}


void Boundary::render(QPainter *painter, double scale)
{
    if (!m_element) {
        return;
    }

    setRectangle(m_element->rectangle());
    painter->save();
    QPen pen = painter->pen();
    pen.setColor(Qt::blue);
    pen.setWidth(2);
    painter->setPen(pen);
    painter->setBrush(Qt::NoBrush);

    for (int node = 0 ; node < 4 ; node++) {
        painter->drawEllipse(QRect(m_nodes[node] * scale, QSize(9, 9)).adjusted(-5, -5, -5, -5));
    }

    painter->drawLine(m_nodes[0] * scale + QPoint(5, 0), m_nodes[1] * scale - QPoint(5, 0));
    painter->drawLine(m_nodes[1] * scale + QPoint(0, 5), m_nodes[2] * scale - QPoint(0, 5));
    painter->drawLine(m_nodes[2] * scale - QPoint(5, 0), m_nodes[3] * scale + QPoint(5, 0));
    painter->drawLine(m_nodes[3] * scale - QPoint(0, 5), m_nodes[0] * scale + QPoint(0, 5));
    painter->restore();
}
