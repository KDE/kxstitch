/********************************************************************************
 *	Copyright (C) 2010 by Stephen Allewell					*
 *	stephen@mirramar.adsl24.co.uk						*
 *										*
 *	This program is free software; you can redistribute it and/or modify	*
 *	it under the terms of the GNU General Public License as published by	*
 *	the Free Software Foundation; either version 2 of the License, or	*
 *	(at your option) any later version.					*
 ********************************************************************************/


#ifndef Boundary_H
#define Boundary_H


#include <QPoint>
#include <QRect>


class QPainter;

class Element;


class Boundary
{
	public:
		Boundary();

		Element *element() const;
		const QPoint *node(const QPoint &) const;
		QRect rectangle() const;
		Qt::CursorShape cursor(const QPoint *);
		bool isValid() const;

		void setElement(Element *);
		void setRectangle(const QRect &);
		void moveNode(const QPoint *, const QPoint &);

		void render(QPainter *, double scale);

	private:
		Element	*m_element;
		QPoint	m_nodes[4];
		QRect	m_rectangle;
};


#endif // Boundary_H
