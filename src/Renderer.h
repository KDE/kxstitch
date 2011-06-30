/********************************************************************************
 *	Copyright (C) 2010 by Stephen Allewell					*
 *	stephen@mirramar.adsl24.co.uk						*
 *										*
 *	This program is free software; you can redistribute it and/or modify	*
 *	it under the terms of the GNU General Public License as published by	*
 *	the Free Software Foundation; either version 2 of the License, or	*
 *	(at your option) any later version.					*
 ********************************************************************************/


#ifndef __Renderer_H
#define __Renderer_H


#include <QFont>
#include <QPolygon>
#include <QRect>

#include "configuration.h"


class QPainter;

class Backstitch;
class Document;
class Knot;
class StitchQueue;


class Renderer
{
	public:
		Renderer();
		~Renderer();

		void setCellSize(int, int);

		void setRenderStitchesAs(Configuration::EnumRenderer_RenderStitchesAs::type);
		void setRenderBackstitchesAs(Configuration::EnumRenderer_RenderBackstitchesAs::type);
		void setRenderKnotsAs(Configuration::EnumRenderer_RenderKnotsAs::type);

		void render(QPainter *,
			    Document *,
			    const QRect &updateRectangle,
			    const QList<int> &layerOrder,
			    const QList<int> &visibleLayers,
			    bool renderStitches,
			    bool renderBackstitches,
			    bool renderKnots,
			    int colorHilight);

	private:
		typedef void (Renderer::*renderStitchCallPointer)(StitchQueue *);
		typedef void (Renderer::*renderBackstitchCallPointer)(Backstitch *);
		typedef void (Renderer::*renderKnotCallPointer)(Knot *);

		static const renderStitchCallPointer renderStitchCallPointers[];
		static const renderBackstitchCallPointer renderBackstitchCallPointers[];
		static const renderKnotCallPointer renderKnotCallPointers[];

		void renderStitchesAsNone(StitchQueue *);
		void renderStitchesAsStitches(StitchQueue *);
		void renderStitchesAsBlackWhiteSymbols(StitchQueue *);
		void renderStitchesAsColorSymbols(StitchQueue *);
		void renderStitchesAsColorBlocks(StitchQueue *);
		void renderStitchesAsColorBlocksSymbols(StitchQueue *);

		void renderBackstitchesAsNone(Backstitch *);
		void renderBackstitchesAsColorLines(Backstitch *);
		void renderBackstitchesAsBlackWhiteSymbols(Backstitch *);

		void renderKnotsAsNone(Knot *);
		void renderKnotsAsColorBlocks(Knot *);
		void renderKnotsAsColorSymbols(Knot *);
		void renderKnotsAsBlackWhiteSymbols(Knot *);

		Configuration::EnumRenderer_RenderStitchesAs::type	m_renderStitchesAs;
		Configuration::EnumRenderer_RenderBackstitchesAs::type	m_renderBackstitchesAs;
		Configuration::EnumRenderer_RenderKnotsAs::type		m_renderKnotsAs;

		QPainter	*m_painter;

		Document	*m_document;

		int	m_hilight;

		int	m_cellWidth;
		int	m_cellHeight;

		QRect	m_renderCell;
		QRect	m_renderTLCell;
		QRect	m_renderTL3Cell;
		QRect	m_renderTRCell;
		QRect	m_renderTR3Cell;
		QRect	m_renderBLCell;
		QRect	m_renderBL3Cell;
		QRect	m_renderBRCell;
		QRect	m_renderBR3Cell;

		QPolygon	m_renderTLQ;
		QPolygon	m_renderTRQ;
		QPolygon	m_renderBLQ;
		QPolygon	m_renderBRQ;

		QPolygon	m_renderBLTRH;
		QPolygon	m_renderTLBRH;

		QPolygon	m_renderTL3Q;
		QPolygon	m_renderTR3Q;
		QPolygon	m_renderBL3Q;
		QPolygon	m_renderBR3Q;

		QFont	m_renderFont;
		QFont	m_renderQtrFont;
		QFont	m_render3QtrFont;

		int	m_renderFontSize;
		int	m_renderQtrFontSize;
		int	m_render3QtrFontSize;
};


#endif
