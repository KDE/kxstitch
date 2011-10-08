/********************************************************************************
 *	Copyright (C) 2010 by Stephen Allewell					*
 *	stephen@mirramar.adsl24.co.uk						*
 *										*
 *	This program is free software; you can redistribute it and/or modify	*
 *	it under the terms of the GNU General Public License as published by	*
 *	the Free Software Foundation; either version 2 of the License, or	*
 *	(at your option) any later version.					*
 ********************************************************************************/


#ifndef PagePreviewListWidgetItem_H
#define PagePreviewListWidgetItem_H


#include <QListWidgetItem>
#include <QPrinter>


class Document;
class Page;


class PagePreviewListWidgetItem : public QListWidgetItem
{
	public:
		PagePreviewListWidgetItem(Document *, Page *);
		~PagePreviewListWidgetItem();

		QPrinter::PaperSize paperSize() const;
		QPrinter::Orientation orientation() const;
		int paperWidth() const;
		int paperHeight() const;
		double zoomFactor() const;

		void setPaperSize(QPrinter::PaperSize);
		void setOrientation(QPrinter::Orientation);
		void setZoomFactor(double);

		Page *page() const;
		void generatePreviewIcon();

	private:

		Document	*m_document;
		Page		*m_page;
		int		m_paperWidth;
		int		m_paperHeight;
};


#endif // PagePreviewListWidgetItem_H
