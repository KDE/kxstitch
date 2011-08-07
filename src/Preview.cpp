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
#include <QStyleOptionRubberBand>

#include "configuration.h"
#include "Document.h"
#include "Preview.h"
#include "Renderer.h"


Preview::Preview(QWidget *parent)
	:	QWidget(parent),
		m_document(0),
		m_renderer(new Renderer()),
		m_zoomFactor(1.0)
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
	m_cellWidth = 4;
	m_cellHeight = 4 * m_document->property("horizontalClothCount").toDouble() / m_document->property("verticalClothCount").toDouble();
	m_previewWidth = m_cellWidth * width * m_zoomFactor;
	m_previewHeight = m_cellHeight * height * m_zoomFactor;
	m_renderer->setPatternRect(QRect(0, 0, width, height));
	m_renderer->setCellSize(m_cellWidth, m_cellHeight);
	m_renderer->setPaintDeviceArea(QRectF(0, 0, m_previewWidth, m_previewHeight));
	resize(m_previewWidth, m_previewHeight);
}


void Preview::setVisibleCells(const QRect &cells)
{
	m_visible = cells;
	update();
}


void Preview::mousePressEvent(QMouseEvent *e)
{
	if (e->buttons() & Qt::LeftButton)
	{
		m_start = m_tracking = m_end = e->pos(); // QPoint(e->pos().x()/m_cellWidth, e->pos().y()/m_cellHeight);
	}
}


void Preview::mouseMoveEvent(QMouseEvent *e)
{
	if (e->buttons() & Qt::LeftButton)
	{
		m_tracking = e->pos(); // QPoint(e->pos().x()/m_cellWidth, e->pos().y()/m_cellHeight);
		if (m_tracking != m_start)
		{
			QRect updateArea = QRect(m_start, m_end).normalized();
			m_end = m_tracking;
			m_rubberBand = QRect(m_start, m_end).normalized();
			update(updateArea.united(m_rubberBand));
		}
	}
}


void Preview::mouseReleaseEvent(QMouseEvent *e)
{
	QPoint p = e->pos();
	if (m_start == m_end)
		emit clicked(QPoint(m_start.x()/m_cellWidth, m_start.y()/m_cellHeight));
	else
		emit clicked(QRect(QPoint(m_start.x()/m_cellWidth, m_start.y()/m_cellHeight), QPoint(m_end.x()/m_cellWidth, m_end.y()/m_cellHeight)).normalized());
	QRect updateArea = m_rubberBand;
	m_rubberBand = QRect();
	update(updateArea);
}


void Preview::paintEvent(QPaintEvent *e)
{
	if (m_document == 0) return;

	QPainter painter;
	painter.begin(this);
	painter.setRenderHint(QPainter::Antialiasing, true);
	QList<int> layerOrder = m_document->layers().layerNumbers();
	QList<int> visibleLayers = m_document->layers().layerNumbers();
	painter.fillRect(e->rect(), m_document->property("fabricColor").value<QColor>());
	m_renderer->render(&painter, m_document, e->rect(), layerOrder, visibleLayers, false, true, true, true, -1);

	if (m_visible.width()*m_cellWidth < m_previewWidth || m_visible.height()*m_cellHeight < m_previewHeight)
	{
		painter.setPen(Qt::white);
		painter.setBrush(Qt::NoBrush);
		painter.setCompositionMode(QPainter::RasterOp_SourceXorDestination);
		painter.drawRect(m_visible.left()*m_cellWidth, m_visible.top()*m_cellHeight, m_visible.width()*m_cellWidth, m_visible.height()*m_cellHeight);
	}

	if (m_rubberBand.isValid())
	{
		QStyleOptionRubberBand opt;
		opt.initFrom(this);
		opt.shape = QRubberBand::Rectangle;
		opt.opaque = false;
		opt.rect = m_rubberBand;
		style()->drawControl(QStyle::CE_RubberBand, &opt, &painter);
	}

	painter.end();
}
