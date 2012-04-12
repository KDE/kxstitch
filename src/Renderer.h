/********************************************************************************
 *	Copyright (C) 2010 by Stephen Allewell					*
 *	stephen@mirramar.adsl24.co.uk						*
 *										*
 *	This program is free software; you can redistribute it and/or modify	*
 *	it under the terms of the GNU General Public License as published by	*
 *	the Free Software Foundation; either version 2 of the License, or	*
 *	(at your option) any later version.					*
 ********************************************************************************/


#ifndef Renderer_H
#define Renderer_H


#include <QFont>
#include <QPolygon>
#include <QRect>

#include "configuration.h"

#include "Renderer_p.h"


class QPainter;

class Backstitch;
class Document;
class Knot;
class StitchQueue;


class Renderer
{
	public:
		Renderer();
		Renderer(const Renderer &);

		void setCellSize(double, double);

		void setPatternRect(const QRect &);
		void setRenderStitchesAs(Configuration::EnumRenderer_RenderStitchesAs::type);
		void setRenderBackstitchesAs(Configuration::EnumRenderer_RenderBackstitchesAs::type);
		void setRenderKnotsAs(Configuration::EnumRenderer_RenderKnotsAs::type);
		void setPaintDeviceArea(const QRectF &);

		void render(QPainter *,
			    Pattern *,
			    const QRect &updateRectangle,
			    bool renderGrid,
			    bool renderStitches,
			    bool renderBackstitches,
			    bool renderKnots,
			    int colorHilight,
			    const QPoint &offset = QPoint(0, 0));

	private:
		typedef void (Renderer::*renderStitchCallPointer)(StitchQueue *);
		typedef void (Renderer::*renderBackstitchCallPointer)(Backstitch *, const QPoint &);
		typedef void (Renderer::*renderKnotCallPointer)(Knot *, const QPoint &);

		static const renderStitchCallPointer renderStitchCallPointers[];
		static const renderBackstitchCallPointer renderBackstitchCallPointers[];
		static const renderKnotCallPointer renderKnotCallPointers[];

		void renderStitchesAsNone(StitchQueue *);
		void renderStitchesAsStitches(StitchQueue *);
		void renderStitchesAsBlackWhiteSymbols(StitchQueue *);
		void renderStitchesAsColorSymbols(StitchQueue *);
		void renderStitchesAsColorBlocks(StitchQueue *);
		void renderStitchesAsColorBlocksSymbols(StitchQueue *);

		void renderBackstitchesAsNone(Backstitch *, const QPoint &);
		void renderBackstitchesAsColorLines(Backstitch *, const QPoint &);
		void renderBackstitchesAsBlackWhiteSymbols(Backstitch *, const QPoint &);

		void renderKnotsAsNone(Knot *, const QPoint &);
		void renderKnotsAsColorBlocks(Knot *, const QPoint &);
		void renderKnotsAsColorBlocksSymbols(Knot *, const QPoint &);
		void renderKnotsAsColorSymbols(Knot *, const QPoint &);
		void renderKnotsAsBlackWhiteSymbols(Knot *, const QPoint &);

		QSharedDataPointer<RendererPrivate>	d;
};


#endif // Renderer_H
