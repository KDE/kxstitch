/********************************************************************************
 *	Copyright (C) 2010 by Stephen Allewell					*
 *	stephen@mirramar.adsl24.co.uk						*
 *										*
 *	This program is free software; you can redistribute it and/or modify	*
 *	it under the terms of the GNU General Public License as published by	*
 *	the Free Software Foundation; either version 2 of the License, or	*
 *	(at your option) any later version.					*
 ********************************************************************************/


#include "PageLayoutEditor.h"

#include <QMouseEvent>
#include <QPainter>
#include <QRubberBand>

#include <math.h>

#include "configuration.h"
#include "Document.h"
#include "Element.h"
#include "Page.h"
#include "PagePreviewListWidgetItem.h"


PageLayoutEditor::PageLayoutEditor(QWidget *parent, Document *document)
	:	QWidget(parent),
		m_document(document),
		m_selecting(false),
		m_resizing(false),
		m_moved(false),
		m_node(0),
		m_gridSize(Configuration::page_GridSize()),
		m_zoomFactor(1.0)
{
	setContextMenuPolicy(Qt::CustomContextMenu);
	setPagePreview(0);
	setMouseTracking(true);
}


PageLayoutEditor::~PageLayoutEditor()
{
}


double PageLayoutEditor::zoomFactor() const
{
	return m_zoomFactor;
}


void PageLayoutEditor::setPagePreview(PagePreviewListWidgetItem *pagePreview)
{
	if ((m_pagePreview = pagePreview))
	{
		m_boundary.setElement(0);
		show();
		updatePagePreview();
	}
	else
		hide();
}


void PageLayoutEditor::updatePagePreview()
{
	m_paperWidth = m_pagePreview->paperWidth();
	m_paperHeight = m_pagePreview->paperHeight();
	m_zoomFactor = m_pagePreview->zoomFactor();
	setMinimumSize(scale(m_paperWidth), scale(m_paperHeight));
	resize(minimumSize());
	update();
}


void PageLayoutEditor::setZoomFactor(double zoomFactor)
{
	m_zoomFactor = zoomFactor;
	if (m_pagePreview)
	{
		setMinimumSize(scale(m_paperWidth), scale(m_paperHeight));
		resize(minimumSize());
	}
	repaint();
}


void PageLayoutEditor::setSelecting(bool selecting)
{
	m_selecting = selecting;
}


void PageLayoutEditor::mousePressEvent(QMouseEvent *event)
{
	m_moved = false;
	m_start = m_end = toSnap(event->pos());

	if (event->buttons() & Qt::LeftButton)
	{
		if (m_selecting)
		{
			if (!m_node)
			{
				m_boundary.setElement(m_pagePreview->page()->itemAt(unscale(event->pos())));
			}
		}
		else
		{
			m_rubberBand = QRect(m_start, m_end).normalized();
		}
		update();
	}
}


void PageLayoutEditor::mouseMoveEvent(QMouseEvent *event)
{
	m_end = toSnap(event->pos());

	if (event->buttons() & Qt::LeftButton)
	{
		if (m_selecting)
		{
			QPoint offset = unscale(m_end) - unscale(m_start);
			if (offset != QPoint(0, 0) && m_boundary.element())
			{
				if (m_node)
				{
					m_boundary.moveNode(m_node, unscale(m_end));
				}
				else
				{
					m_boundary.element()->move(offset);
				}
				m_moved = true;
				m_start = m_end;
				update();
//				emit elementGeometryChanged();
			}
		}
		else
		{
			m_rubberBand = QRect(m_start, m_end).normalized();
			update();
		}
//		update();
	}
	else
	{
		if (m_boundary.element() && (m_node = m_boundary.node(unscale(event->pos()))))
		{
			setCursor(m_boundary.cursor(m_node));
		}
		else
			setCursor(Qt::ArrowCursor);
	}
}


void PageLayoutEditor::mouseReleaseEvent(QMouseEvent *event)
{
	if (!m_selecting)
	{
		if (m_rubberBand.isValid())
		{
			m_end = toSnap(event->pos());
			m_rubberBand = QRect(m_start, m_end).normalized();
			QRect selection = unscale(m_rubberBand).adjusted(0, 0, -1, -1);
			m_rubberBand = QRect();
			if (selection.isValid())
			{
				emit selectionMade(selection);
			}
		}
	}
	else if (m_boundary.element() && m_moved)
	{
		emit elementGeometryChanged();
	}
	m_moved = false;
	update();
}


void PageLayoutEditor::paintEvent(QPaintEvent *event)
{
	QPainter painter;
	painter.begin(this);
	QRect updateRect = event->rect();
	painter.fillRect(updateRect, Qt::white);
	int w = updateRect.width();
	int h = updateRect.height();

	painter.setPen(Qt::black);
	painter.setBrush(Qt::NoBrush);
	painter.drawRect(0, 0, width()-1, height()-1); // bounding rect for the page
	if (m_document && m_pagePreview && !m_moved)
	{
		m_pagePreview->page()->render(m_document, &painter);
	}

	painter.resetTransform();

	// draw grid
	int scaledGridSize = scale(m_gridSize);
	int xOffset = (width()/2)%scaledGridSize;
	int yOffset = (height()/2)%scaledGridSize;
	for (int y = yOffset ; y < h ; y += scaledGridSize)
		for (int x = xOffset ; x < w ; x += scaledGridSize)
			painter.drawPoint(x, y);

	if (m_rubberBand.isValid())
	{
		QStyleOptionRubberBand opt;
		opt.initFrom(this);
		opt.shape = QRubberBand::Rectangle;
		opt.opaque = false;
		opt.rect = m_rubberBand;
		style()->drawControl(QStyle::CE_RubberBand, &opt, &painter);
	}

	if (m_boundary.isValid())
	{
		m_boundary.render(&painter, m_zoomFactor);
	}

	painter.end();
}


QPoint PageLayoutEditor::toSnap(const QPoint &pos) const
{
	int scaledGridSize = scale(m_gridSize);
	int xOffset = (width()/2)%scaledGridSize;
	int yOffset = (height()/2)%scaledGridSize;
	int xSnap = (((pos.x()-xOffset+5)/scaledGridSize)*scaledGridSize)+xOffset;
	int ySnap = (((pos.y()-yOffset+5)/scaledGridSize)*scaledGridSize)+yOffset;
	return QPoint(xSnap, ySnap);
}


int PageLayoutEditor::scale(int n) const
{
	return int(double(n) * m_zoomFactor);
}


int PageLayoutEditor::unscale(int n) const
{
	return int(double(n) / m_zoomFactor);
}


QPoint PageLayoutEditor::scale(const QPoint &p) const
{
	return QPoint(scale(p.x()), scale(p.y()));
}


QPoint PageLayoutEditor::unscale(const QPoint &p) const
{
	return QPoint(unscale(p.x()), unscale(p.y()));
}


QRect PageLayoutEditor::scale(const QRect &r) const
{
	return QRect(scale(r.topLeft()), scale(r.bottomRight()));
}


QRect PageLayoutEditor::unscale(const QRect &r) const
{
	return QRect(unscale(r.topLeft()), unscale(r.bottomRight()));
}
