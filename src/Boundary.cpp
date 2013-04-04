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
        if (QRect(-4, -4, 8, 8).translated(m_nodes[node]).contains(pos)) {
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


void Boundary::render(QPainter *painter)
{
    if (!m_element) {
        return;
    }

    painter->save();

    QTransform transform = painter->combinedTransform();
    painter->resetTransform();

    QPen pen(Qt::blue);
    pen.setWidth(0);
    painter->setPen(pen);
    painter->setBrush(Qt::blue);

    for (int node = 0 ; node < 4 ; node++) {
        painter->drawRect(QRect(-2, -2, 4, 4).translated(transform.map(QPoint(m_nodes[node]))));
    }

    painter->drawLine(transform.map(QLine(m_nodes[0], m_nodes[1])));
    painter->drawLine(transform.map(QLine(m_nodes[1], m_nodes[2])));
    painter->drawLine(transform.map(QLine(m_nodes[2], m_nodes[3])));
    painter->drawLine(transform.map(QLine(m_nodes[3], m_nodes[0])));

    painter->restore();
}
