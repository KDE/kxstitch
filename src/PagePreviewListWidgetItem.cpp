/********************************************************************************
 *	Copyright (C) 2010 by Stephen Allewell					*
 *	stephen@mirramar.adsl24.co.uk						*
 *										*
 *	This program is free software; you can redistribute it and/or modify	*
 *	it under the terms of the GNU General Public License as published by	*
 *	the Free Software Foundation; either version 2 of the License, or	*
 *	(at your option) any later version.					*
 ********************************************************************************/


#include "PagePreviewListWidgetItem.h"

#include <algorithm>

#include <QPainter>
#include <QPixmap>

#include <KDebug>

#include "Page.h"
#include "PaperSizes.h"


PagePreviewListWidgetItem::PagePreviewListWidgetItem(Document *document, Page *page)
	:	QListWidgetItem(0, QListWidgetItem::UserType),
		m_document(document),
		m_page(page)
{
	generatePreviewIcon();
}


PagePreviewListWidgetItem::~PagePreviewListWidgetItem()
{
}


QPrinter::PaperSize PagePreviewListWidgetItem::paperSize() const
{
	return m_page->paperSize();
}


QPrinter::Orientation PagePreviewListWidgetItem::orientation() const
{
	return m_page->orientation();
}


int PagePreviewListWidgetItem::paperWidth() const
{
	return m_paperWidth;
}


int PagePreviewListWidgetItem::paperHeight() const
{
	return m_paperHeight;
}


double PagePreviewListWidgetItem::zoomFactor() const
{
	return m_page->zoomFactor();
}


void PagePreviewListWidgetItem::setPaperSize(QPrinter::PaperSize paperSize)
{
	m_page->setPaperSize(paperSize);
	generatePreviewIcon();
}


void PagePreviewListWidgetItem::setOrientation(QPrinter::Orientation orientation)
{
	m_page->setOrientation(orientation);
	generatePreviewIcon();
}


void PagePreviewListWidgetItem::setZoomFactor(double zoomFactor)
{
	m_page->setZoomFactor(zoomFactor);
}


Page *PagePreviewListWidgetItem::page() const
{
	return m_page;
}


void PagePreviewListWidgetItem::generatePreviewIcon()
{
	m_paperWidth = PaperSizes::width(m_page->paperSize(), m_page->orientation());
	m_paperHeight = PaperSizes::height(m_page->paperSize(), m_page->orientation());
	QSize iconSize(140, 140);
	float widthScale = float(iconSize.width())/m_paperWidth;
	float heightScale = float(iconSize.height())/m_paperHeight;
	float scale = std::min(widthScale, heightScale);

	QPixmap pixmap(m_paperWidth*scale, m_paperHeight*scale);

	QPainter painter;
	painter.begin(&pixmap);
	painter.setPen(Qt::black);
	painter.setBrush(Qt::white);
	painter.drawRect(0, 0, pixmap.width()-1, pixmap.height()-1);
	m_page->render(m_document, &painter);
	painter.end();
	setIcon(pixmap);
}
