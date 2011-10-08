/********************************************************************************
 *	Copyright (C) 2010 by Stephen Allewell					*
 *	stephen@mirramar.adsl24.co.uk						*
 *										*
 *	This program is free software; you can redistribute it and/or modify	*
 *	it under the terms of the GNU General Public License as published by	*
 *	the Free Software Foundation; either version 2 of the License, or	*
 *	(at your option) any later version.					*
 ********************************************************************************/


#ifndef Page_H
#define Page_H


#include <QDataStream>
#include <QList>
#include <QMargins>
#include <QPrinter>


class Document;
class Element;
class QPainter;


class Page
{
	public:
		Page();
		Page(QPrinter::PaperSize, QPrinter::Orientation, double);
		Page(const Page &);
		~Page();

		int pageNumber() const;
		QPrinter::PaperSize paperSize() const;
		QPrinter::Orientation orientation() const;
		double zoomFactor() const;
		QList<Element *> elements();

		void setPageNumber(int);
		void setPaperSize(QPrinter::PaperSize);
		void setOrientation(QPrinter::Orientation);
		void setZoomFactor(double);

		void addElement(Element *);
		void removeElement(Element *);

		void render(Document *, QPainter *);

		Element *itemAt(const QPoint &) const;

		friend QDataStream &operator<<(QDataStream &, const Page &);
		friend QDataStream &operator>>(QDataStream &, Page &);

		friend class PagePropertiesDlg;

	private:
		void readElements(QDataStream &);

		static const int version = 101;

		int			m_pageNumber;

		QPrinter::PaperSize	m_paperSize;
		QPrinter::Orientation	m_orientation;

		QMargins		m_margins;

		bool			m_showGrid;
		int			m_gridX;
		int			m_gridY;

		QList<Element *>	m_elements;

		double			m_zoomFactor;
};


#endif // Page_H
