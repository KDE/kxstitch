/*
 * Copyright (C) 2010-2015 by Stephen Allewell
 * steve.allewell@gmail.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

/** @file
 * This file defines an overlay rectangle with corner nodes to be used on top
 * of an element on the print layout pages allowing the user to change the size
 * and position of the underlying element.
 */

#ifndef Boundary_H
#define Boundary_H

// Qt includes
#include <QPoint>
#include <QRect>

// Forward declaration of Qt classes
class QPainter;

// Forward declaration of application classes
class Element;

/**
 * This class defines a rectangle around a print element allowing the user
 * to move the element or resize it by dragging the nodes.
 */
class Boundary
{
public:
    /**
     * Constructor
     */
    Boundary();

    /**
     * Get the pointer to the associated element.
     *
     * @return a pointer to the Element instance
     */
    Element *element() const;

    /**
     * Get the node that is nearest to the supplied point within the defined
     * snap distance.
     *
     * @param point is a const reference to a QPoint containing the supplied point
     *
     * @return a const pointer to the closest node, a nullptr is returned if no nodes are within the snap distance
     */
    const QPoint *node(const QPoint &point) const;

    /**
     * Get the rectangle outline of the boundary.
     *
     * @return a QRect defining the position and size of the boundary
     */
    QRect rectangle() const;

    /**
     * Get the cursor shape relating to the node specified.
     *
     * @param node is a const pointer to the node to provide the cursor for
     *
     * @return a Qt::CursorShape dependant on which corner of the boundary is pointed to
     */
    Qt::CursorShape cursor(const QPoint *node);

    /**
     * Check if the boundary is valid in that a element is assigned and the boundary
     * element rectangle is correcly defined.
     *
     * @return @c true if the boundary is valid, @c false otherwise
     */
    bool isValid() const;

    /**
     * Set the element to be associated with the boundary.
     *
     * @param element is a pointer to the printer element
     */
    void setElement(Element *element);

    /**
     * Set the rectangle position and size for the assigned Element.
     *
     * @param rectangle is a const reference to a QRect defining the boundary
     */
    void setRectangle(const QRect &rectangle);

    /**
     * Move a node to a new position updating the other corner nodes to suit.
     *
     * @param node is a const pointer to a QPoint representing the node to move
     * @param point is a const reference to a QPoint containing the new position
     */
    void moveNode(const QPoint *node, const QPoint &point);

    /**
     * Render the boundary using the supplied QPainter object. The caller must
     * initialise the QPainter appropriately.
     *
     * @param painter is a pointer to the QPainter to draw with
     */
    void render(QPainter *painter);

private:
    Element *m_element; /**< A pointer to the element the boundary is applied to */
    QPoint m_nodes[4]; /**< An array of corner nodes, topLeft, topRight, bottomRight and bottomLeft */
    QRect m_rectangle; /**< The rectangle defining the boundary size and position */
};

#endif // Boundary_H
