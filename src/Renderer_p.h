/********************************************************************************
 *	Copyright (C) 2010 by Stephen Allewell					*
 *	stephen@mirramar.adsl24.co.uk						*
 *										*
 *	This program is free software; you can redistribute it and/or modify	*
 *	it under the terms of the GNU General Public License as published by	*
 *	the Free Software Foundation; either version 2 of the License, or	*
 *	(at your option) any later version.					*
 ********************************************************************************/


#ifndef Renderer_p_H
#define Renderer_p_H


#include <QFont>
#include <QPolygon>
#include <QRect>
#include <QSharedData>

#include "configuration.h"


class QPainter;

class Backstitch;
class Document;
class Knot;
class StitchQueue;


class RendererPrivate : public QSharedData
{
	public:
		RendererPrivate();
		RendererPrivate(const RendererPrivate &);
		~RendererPrivate();

		friend class Renderer;

	private:
		Configuration::EnumRenderer_RenderStitchesAs::type	m_renderStitchesAs;
		Configuration::EnumRenderer_RenderBackstitchesAs::type	m_renderBackstitchesAs;
		Configuration::EnumRenderer_RenderKnotsAs::type		m_renderKnotsAs;

		QPainter	*m_painter;

		Document	*m_document;
		QRect		m_patternRect;
		QRectF		m_paintDeviceArea;

		int	m_hilight;

		double	m_cellWidth;
		double	m_cellHeight;

		QRectF	m_renderCell;
		QRectF	m_renderTLCell;
		QRectF	m_renderTL3Cell;
		QRectF	m_renderTRCell;
		QRectF	m_renderTR3Cell;
		QRectF	m_renderBLCell;
		QRectF	m_renderBL3Cell;
		QRectF	m_renderBRCell;
		QRectF	m_renderBR3Cell;

		QPolygonF	m_renderTLQ;
		QPolygonF	m_renderTRQ;
		QPolygonF	m_renderBLQ;
		QPolygonF	m_renderBRQ;

		QPolygonF	m_renderBLTRH;
		QPolygonF	m_renderTLBRH;

		QPolygonF	m_renderTL3Q;
		QPolygonF	m_renderTR3Q;
		QPolygonF	m_renderBL3Q;
		QPolygonF	m_renderBR3Q;

		QFont	m_renderFont;
		QFont	m_renderQtrFont;
		QFont	m_render3QtrFont;

		int	m_renderFontSize;
		int	m_renderQtrFontSize;
		int	m_render3QtrFontSize;
};


#endif // Renderer_p_H