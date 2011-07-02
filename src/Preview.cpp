/********************************************************************************
 *	Copyright (C) 2010 by Stephen Allewell					*
 *	stephen@mirramar.adsl24.co.uk						*
 *										*
 *	This program is free software; you can redistribute it and/or modify	*
 *	it under the terms of the GNU General Public License as published by	*
 *	the Free Software Foundation; either version 2 of the License, or	*
 *	(at your option) any later version.					*
 ********************************************************************************/


#include <QMouseEvent>
#include <QPainter>
#include <QScrollArea>

#include "configuration.h"
#include "Document.h"
#include "Preview.h"
#include "Renderer.h"


Preview::Preview(QWidget *parent)
	:	QWidget(parent),
		m_document(0),
		m_renderer(new Renderer())
{
	m_renderer->setRenderStitchesAs(Configuration::EnumRenderer_RenderStitchesAs::ColorBlocks);
	m_renderer->setRenderBackstitchesAs(Configuration::EnumRenderer_RenderBackstitchesAs::ColorLines);
	m_renderer->setRenderKnotsAs(Configuration::EnumRenderer_RenderKnotsAs::None);
}


Preview::~Preview()
{
	delete m_renderer;
}


void Preview::setDocument(Document *document)
{
	m_document = document;
	readDocumentSettings();
}


Document *Preview::document()
{
	return m_document;
}


void Preview::readDocumentSettings()
{
	int width = m_document->stitchData().width();
	int height = m_document->stitchData().height();
	int cellWidth = m_document->property("cellWidth").toInt();
	int cellHeight = m_document->property("cellHeight").toInt();
	m_previewWidth = (width * 4);
	m_previewHeight = (height * 4)*cellHeight/cellWidth;
	resize(m_previewWidth, m_previewHeight);
	m_cellWidth = 4;
	m_cellHeight = m_previewHeight/height;
	m_renderer->setCellSize(m_cellWidth, m_cellHeight);
}


void Preview::setVisibleCells(const QRect &cells)
{
	m_visible = cells;
	update();
}


void Preview::mousePressEvent(QMouseEvent *e)
{
	m_point = e->pos();
	m_moved = false;
}


void Preview::mouseMoveEvent(QMouseEvent *e)
{
	m_moved = (e->pos() != m_point);
}


void Preview::mouseReleaseEvent(QMouseEvent *)
{
	if (!m_moved)
		emit clicked(QPoint(m_point.x()/m_cellWidth, m_point.y()/m_cellHeight));
}


void Preview::paintEvent(QPaintEvent *e)
{
	if (m_document == 0) return;

	QPainter painter;
	painter.begin(this);
	QList<int> layerOrder = m_document->layers().layerNumbers();
	QList<int> visibleLayers = m_document->layers().layerNumbers();
	painter.fillRect(e->rect(), m_document->property("fabricColor").value<QColor>());
	m_renderer->render(&painter, m_document, e->rect(), layerOrder, visibleLayers, true, true, true, -1);
	if (m_visible.width()*m_cellWidth < m_previewWidth || m_visible.height()*m_cellHeight < m_previewHeight)
	{
		painter.setPen(Qt::white);
		painter.setBrush(Qt::NoBrush);
		painter.setCompositionMode(QPainter::RasterOp_SourceXorDestination);
		painter.drawRect(m_visible.left()*m_cellWidth, m_visible.top()*m_cellHeight, m_visible.width()*m_cellWidth, m_visible.height()*m_cellHeight);
	}
	painter.end();
}
