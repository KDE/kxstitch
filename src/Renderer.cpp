/********************************************************************************
 *	Copyright (C) 2010 by Stephen Allewell					*
 *	stephen@mirramar.adsl24.co.uk						*
 *										*
 *	This program is free software; you can redistribute it and/or modify	*
 *	it under the terms of the GNU General Public License as published by	*
 *	the Free Software Foundation; either version 2 of the License, or	*
 *	(at your option) any later version.					*
 ********************************************************************************/


#include "Renderer.h"

#include <QPaintEngine>
#include <QPainter>
#include <QPen>

#include "Document.h"
#include "DocumentFloss.h"
#include "Stitch.h"


RendererPrivate::RendererPrivate()
	:	m_renderStitchesAs(Configuration::EnumRenderer_RenderStitchesAs::None),
		m_renderBackstitchesAs(Configuration::EnumRenderer_RenderBackstitchesAs::None),
		m_renderKnotsAs(Configuration::EnumRenderer_RenderKnotsAs::None),
		m_painter(0),
		m_pattern(0)
{
}


RendererPrivate::RendererPrivate(const RendererPrivate &other)
	:	QSharedData(other),
		m_renderStitchesAs(other.m_renderStitchesAs),
		m_renderBackstitchesAs(other.m_renderBackstitchesAs),
		m_renderKnotsAs(other.m_renderKnotsAs),
		m_pattern(other.m_pattern),
		m_patternRect(other.m_patternRect),
		m_cellWidth(other.m_cellWidth),
		m_cellHeight(other.m_cellHeight),
		m_renderCell(other.m_renderCell),
		m_renderTLCell(other.m_renderTLCell),
		m_renderTL3Cell(other.m_renderTL3Cell),
		m_renderTRCell(other.m_renderTRCell),
		m_renderTR3Cell(other.m_renderTR3Cell),
		m_renderBLCell(other.m_renderBLCell),
		m_renderBL3Cell(other.m_renderBL3Cell),
		m_renderBRCell(other.m_renderBRCell),
		m_renderBR3Cell(other.m_renderBR3Cell),
		m_renderTLQ(other.m_renderTLQ),
		m_renderTRQ(other.m_renderTRQ),
		m_renderBLQ(other.m_renderBLQ),
		m_renderBRQ(other.m_renderBRQ),
		m_renderBLTRH(other.m_renderBLTRH),
		m_renderTLBRH(other.m_renderTLBRH),
		m_renderTL3Q(other.m_renderTL3Q),
		m_renderTR3Q(other.m_renderTR3Q),
		m_renderBL3Q(other.m_renderBL3Q),
		m_renderBR3Q(other.m_renderBR3Q),
		m_renderFont(other.m_renderFont),
		m_renderQtrFont(other.m_renderQtrFont),
		m_render3QtrFont(other.m_render3QtrFont),
		m_renderFontSize(other.m_renderFontSize),
		m_renderQtrFontSize(other.m_renderQtrFontSize),
		m_render3QtrFontSize(other.m_render3QtrFontSize)
{
}


RendererPrivate::~RendererPrivate()
{
}


const Renderer::renderStitchCallPointer Renderer::renderStitchCallPointers[] =
{
	&Renderer::renderStitchesAsNone,
	&Renderer::renderStitchesAsStitches,
	&Renderer::renderStitchesAsBlackWhiteSymbols,
	&Renderer::renderStitchesAsColorSymbols,
	&Renderer::renderStitchesAsColorBlocks,
	&Renderer::renderStitchesAsColorBlocksSymbols,
};

const Renderer::renderBackstitchCallPointer Renderer::renderBackstitchCallPointers[] =
{
	&Renderer::renderBackstitchesAsNone,
	&Renderer::renderBackstitchesAsColorLines,
	&Renderer::renderBackstitchesAsBlackWhiteSymbols,
};

const Renderer::renderKnotCallPointer Renderer::renderKnotCallPointers[] =
{
	&Renderer::renderKnotsAsNone,
	&Renderer::renderKnotsAsColorBlocks,
	&Renderer::renderKnotsAsColorSymbols,
	&Renderer::renderKnotsAsBlackWhiteSymbols,
};


Renderer::Renderer()
	:	d(new RendererPrivate)
{
}


Renderer::Renderer(const Renderer &other)
	:	d(other.d)
{
}


void Renderer::setCellSize(double cellWidth, double cellHeight)
{
	if ((cellWidth != d->m_cellWidth) || (cellHeight != d->m_cellHeight))
	{
		double halfWidth = cellWidth/2;
		double halfHeight = cellHeight/2;
		double thirdWidth = cellWidth/3;
		double thirdHeight = cellHeight/3;

		d->m_renderCell = QRectF(0, 0, cellWidth, cellHeight);
		d->m_renderTLCell = QRectF(0, 0, halfWidth, halfHeight);
		d->m_renderTL3Cell = QRectF(0, 0, cellWidth-thirdWidth, cellHeight-thirdHeight);
		d->m_renderTRCell = QRectF(halfWidth, 0, halfWidth, halfHeight);
		d->m_renderTR3Cell = QRectF(thirdWidth, 0, cellWidth-thirdWidth, cellHeight-thirdHeight);
		d->m_renderBLCell = QRectF(0, halfHeight, halfWidth, halfHeight);
		d->m_renderBL3Cell = QRectF(0, thirdHeight, cellWidth-thirdWidth, cellHeight-thirdHeight);
		d->m_renderBRCell = QRectF(halfWidth, halfHeight, halfWidth, halfHeight);
		d->m_renderBR3Cell = QRectF(thirdWidth, thirdHeight, cellWidth-thirdWidth, cellHeight-thirdHeight);

		d->m_renderTLQ << QPointF(0, 0) << QPointF(halfWidth, 0) << QPointF(0, halfHeight);
		d->m_renderTRQ << QPointF(halfWidth, 0) << QPointF(cellWidth, 0) << QPointF(cellWidth, halfHeight);
		d->m_renderBLQ << QPointF(0, halfHeight) << QPointF(halfWidth, cellHeight) << QPointF(0, cellHeight);
		d->m_renderBRQ << QPointF(cellWidth, halfHeight) << QPointF(cellWidth, cellHeight) << QPointF(halfWidth, cellHeight);
		d->m_renderBLTRH << QPointF(0, cellHeight) << QPointF(0, halfHeight) << QPointF(halfWidth, 0) << QPointF(cellWidth, 0) << QPointF(cellWidth, halfHeight) << QPointF(halfWidth, cellHeight);
		d->m_renderTLBRH << QPointF(0, 0) << QPointF(halfWidth, 0) << QPointF(cellWidth, halfHeight) << QPointF(cellWidth, cellHeight) << QPointF(halfWidth, cellHeight) << QPointF(0, halfWidth);
		d->m_renderTL3Q << QPointF(0, 0) << QPointF(cellWidth, 0) << QPointF(cellWidth, halfHeight) << QPointF(halfWidth, cellHeight) << QPointF(0, cellHeight);
		d->m_renderTR3Q << QPointF(0, 0) << QPointF(cellWidth, 0) << QPointF(cellWidth, cellHeight) << QPointF(halfWidth, cellHeight) << QPointF(0, halfHeight);
		d->m_renderBL3Q << QPointF(0, 0) << QPointF(halfWidth, 0) << QPointF(cellWidth, halfHeight) << QPointF(cellWidth, cellHeight) << QPointF(0, cellHeight);
		d->m_renderBR3Q << QPointF(halfWidth, 0) << QPointF(cellWidth, 0) << QPointF(cellWidth, cellHeight) << QPointF(0, cellHeight) << QPointF(0, halfHeight);

		d->m_renderFontSize = std::min(cellWidth, cellHeight);
		d->m_renderQtrFontSize = d->m_renderFontSize/2;
		d->m_render3QtrFontSize = d->m_renderFontSize*3/4;

		d->m_cellWidth = cellWidth;
		d->m_cellHeight = cellHeight;
	}
}


void Renderer::setPatternRect(const QRect &rect)
{
	d->m_patternRect = rect;
}


void Renderer::setRenderStitchesAs(Configuration::EnumRenderer_RenderStitchesAs::type renderStitchesAs)
{
	d->m_renderStitchesAs = renderStitchesAs;
}


void Renderer::setRenderBackstitchesAs(Configuration::EnumRenderer_RenderBackstitchesAs::type renderBackstitchesAs)
{
	d->m_renderBackstitchesAs = renderBackstitchesAs;
}


void Renderer::setRenderKnotsAs(Configuration::EnumRenderer_RenderKnotsAs::type renderKnotsAs)
{
	d->m_renderKnotsAs = renderKnotsAs;
}


void Renderer::setPaintDeviceArea(const QRectF &rect)
{
	d->m_paintDeviceArea = rect;
}


void Renderer::render(QPainter *painter,
		      Pattern *pattern,
		      const QRect &updateRectangle,
		      bool renderGrid,
		      bool renderStitches,
		      bool renderBackstitches,
		      bool renderKnots,
		      int colorHilight,
		      const QPoint &offset)
{
	bool paintDeviceIsScreen = (painter->device()->paintEngine()->type() == QPaintEngine::X11); // test for other types
	
	QPoint snapOffset(offset.x()*2, offset.y()*2);

	painter->save();
	d->m_painter = painter;
	d->m_pattern = pattern;
	d->m_hilight = colorHilight;

	painter->setClipRect(d->m_paintDeviceArea, Qt::IntersectClip);

	double l = d->m_paintDeviceArea.left();
	double t = d->m_paintDeviceArea.top();
	double w = d->m_paintDeviceArea.width();
	double h = d->m_paintDeviceArea.height();
	double dx = w / d->m_patternRect.width();
	double dy = h / d->m_patternRect.height();

	painter->translate(l, t);

	if (renderGrid)
	{
		int cellHorizontalGrouping = -1;
		int cellVerticalGrouping = -1;
		if (pattern->document())
		{
			cellHorizontalGrouping = pattern->document()->property("cellHorizontalGrouping").toInt();
			cellVerticalGrouping = pattern->document()->property("cellVerticalGrouping").toInt();
		}

		for (int x = 0 ; x < d->m_patternRect.width() ; x++)
		{
			if ((cellHorizontalGrouping == -1) || (x % cellHorizontalGrouping))
				painter->setPen(paintDeviceIsScreen?QPen(Qt::lightGray):QPen(Qt::black, 0.5));
			else
				painter->setPen(paintDeviceIsScreen?QPen(Qt::darkGray):QPen(Qt::black, 2.0));
			painter->drawLine(x*dx, 0, x*dx, h);
		}

		for (int y = 0 ; y < d->m_patternRect.height() ; y++)
		{
			if ((cellVerticalGrouping == -1) || (y % cellVerticalGrouping))
				painter->setPen(paintDeviceIsScreen?QPen(Qt::lightGray):QPen(Qt::black, 0.5));
			else
				painter->setPen(paintDeviceIsScreen?QPen(Qt::darkGray):QPen(Qt::black, 2.0));
			painter->drawLine(0, y*dy, w, y*dy);
		}
	}

	int leftCell = std::max(0, int((updateRectangle.left()-l)/dx));
	int topCell = std::max(0, int((updateRectangle.top()-t)/dy));
	int rightCell = std::min(d->m_patternRect.width(), int((updateRectangle.right()-l)/dx));
	int bottomCell = std::min(d->m_patternRect.height(), int((updateRectangle.bottom()-t)/dx));

	d->m_renderFont = painter->font();
	d->m_renderFont.setFamily(Configuration::editor_SymbolFont());
	d->m_renderFont.setPixelSize(d->m_renderFontSize);
	d->m_renderQtrFont = d->m_renderFont;
	d->m_renderQtrFont.setPixelSize(d->m_renderQtrFontSize);
	d->m_render3QtrFont = d->m_renderFont;
	d->m_render3QtrFont.setPixelSize(d->m_render3QtrFontSize);

	QRect snapUpdateRectangle(leftCell*2, topCell*2, (rightCell-leftCell)*2, (bottomCell-topCell)*2);

	if (renderStitches)
	{
		for (int y =  topCell ; y <= bottomCell ; y++)
		{
			for (int x = leftCell ; x <= rightCell ; x++)
			{
				StitchQueue *queue = pattern->stitches().stitchQueueAt(QPoint(x, y)-offset);
				if (queue)
				{
					double xpos = x*d->m_cellWidth;
					double ypos = y*d->m_cellHeight;
					painter->resetTransform();
					painter->translate(xpos+l, ypos+t);
					(this->*renderStitchCallPointers[d->m_renderStitchesAs])(queue);
				}
			}
		}
	}
	painter->resetTransform();
	painter->translate(l, t);
	// process backstitches
	if (renderBackstitches)
	{
		QListIterator<Backstitch *> backstitchIterator = pattern->stitches().backstitchIterator();
		while (backstitchIterator.hasNext())
		{
			(this->*renderBackstitchCallPointers[d->m_renderBackstitchesAs])(backstitchIterator.next(), snapOffset);
		}
	}
	// process knots
	if (renderKnots)
	{
		QListIterator<Knot *> knotIterator = pattern->stitches().knotIterator();
		while (knotIterator.hasNext())
		{
			(this->*renderKnotCallPointers[d->m_renderKnotsAs])(knotIterator.next(), snapOffset);
		}
	}
	painter->restore();
}


void Renderer::renderStitchesAsNone(StitchQueue *)
{
}


void Renderer::renderStitchesAsStitches(StitchQueue *stitchQueue)
{
	QPen pen;
	StitchQueue::const_iterator it;
	for (it = stitchQueue->begin() ; it != stitchQueue->end() ; ++it)
	{
		Stitch *stitch = *it;
		DocumentFloss *documentFloss = d->m_pattern->palette().floss(stitch->colorIndex);
		if ((d->m_hilight == -1) || (stitch->colorIndex == d->m_hilight))
		{
			pen.setColor(documentFloss->flossColor());
			pen.setWidth(documentFloss->stitchStrands());
		}
		else
		{
			pen.setColor(Qt::lightGray);
			pen.setWidth(0);
		}

		d->m_painter->setPen(pen);

		switch (stitch->type)
		{
			case Stitch::Delete:
				break;

			case Stitch::TLQtr:
				d->m_painter->drawLine(d->m_renderCell.topLeft(), d->m_renderCell.center());
				break;

			case Stitch::TRQtr:
				d->m_painter->drawLine(d->m_renderCell.topRight(), d->m_renderCell.center());
				break;

			case Stitch::BLQtr:
				d->m_painter->drawLine(d->m_renderCell.bottomLeft(), d->m_renderCell.center());
				break;

			case Stitch::BTHalf:
				d->m_painter->drawLine(d->m_renderCell.bottomLeft(), d->m_renderCell.topRight());
				break;

			case Stitch::TL3Qtr:
				d->m_painter->drawLine(d->m_renderCell.bottomLeft(), d->m_renderCell.topRight());
				d->m_painter->drawLine(d->m_renderCell.topLeft(), d->m_renderCell.center());
				break;

			case Stitch::BRQtr:
				d->m_painter->drawLine(d->m_renderCell.bottomRight(), d->m_renderCell.center());
				break;

			case Stitch::TBHalf:
				d->m_painter->drawLine(d->m_renderCell.topLeft(), d->m_renderCell.bottomRight());
				break;

			case Stitch::TR3Qtr:
				d->m_painter->drawLine(d->m_renderCell.topLeft(), d->m_renderCell.bottomRight());
				d->m_painter->drawLine(d->m_renderCell.topRight(), d->m_renderCell.center());
				break;

			case Stitch::BL3Qtr:
				d->m_painter->drawLine(d->m_renderCell.topLeft(), d->m_renderCell.bottomRight());
				d->m_painter->drawLine(d->m_renderCell.bottomLeft(), d->m_renderCell.center());
				break;

			case Stitch::BR3Qtr:
				d->m_painter->drawLine(d->m_renderCell.bottomLeft(), d->m_renderCell.topRight());
				d->m_painter->drawLine(d->m_renderCell.bottomRight(), d->m_renderCell.center());
				break;

			case Stitch::Full:
				d->m_painter->drawLine(d->m_renderCell.topLeft(), d->m_renderCell.bottomRight());
				d->m_painter->drawLine(d->m_renderCell.bottomLeft(), d->m_renderCell.topRight());
				break;

			case Stitch::TLSmallHalf:
				d->m_painter->drawLine(d->m_renderCell.left(), d->m_renderCell.center().y(), d->m_renderCell.center().x(), d->m_renderCell.top());
				break;

			case Stitch::TRSmallHalf:
				d->m_painter->drawLine(d->m_renderCell.center().x(), d->m_renderCell.top(), d->m_renderCell.right(), d->m_renderCell.center().y());
				break;

			case Stitch::BLSmallHalf:
				d->m_painter->drawLine(d->m_renderCell.left(), d->m_renderCell.center().y(), d->m_renderCell.center().x(), d->m_renderCell.bottom());
				break;

			case Stitch::BRSmallHalf:
				d->m_painter->drawLine(d->m_renderCell.center().x(), d->m_renderCell.bottom(), d->m_renderCell.right(), d->m_renderCell.center().y());
				break;

			case Stitch::TLSmallFull:
				d->m_painter->drawLine(d->m_renderCell.left(), d->m_renderCell.center().y(), d->m_renderCell.center().x(), d->m_renderCell.top());
				d->m_painter->drawLine(d->m_renderCell.topLeft(), d->m_renderCell.center());
				break;

			case Stitch::TRSmallFull:
				d->m_painter->drawLine(d->m_renderCell.center().x(), d->m_renderCell.top(), d->m_renderCell.right(), d->m_renderCell.center().y());
				d->m_painter->drawLine(d->m_renderCell.topRight(), d->m_renderCell.center());
				break;

			case Stitch::BLSmallFull:
				d->m_painter->drawLine(d->m_renderCell.left(), d->m_renderCell.center().y(), d->m_renderCell.center().x(), d->m_renderCell.bottom());
				d->m_painter->drawLine(d->m_renderCell.bottomLeft(), d->m_renderCell.center());
				break;

			case Stitch::BRSmallFull:
				d->m_painter->drawLine(d->m_renderCell.center().x(), d->m_renderCell.bottom(), d->m_renderCell.right(), d->m_renderCell.center().y());
				d->m_painter->drawLine(d->m_renderCell.bottomRight(), d->m_renderCell.center());
				break;
		}
	}
}


void Renderer::renderStitchesAsBlackWhiteSymbols(StitchQueue *stitchQueue)
{
	QPen symbolPen;
	QPen outlinePen(Qt::lightGray);
	outlinePen.setWidth(0);
	StitchQueue::const_iterator it;
	for (it = stitchQueue->begin() ; it != stitchQueue->end() ; ++it)
	{
		Stitch *stitch = *it;
		DocumentFloss *documentFloss = d->m_pattern->palette().floss(stitch->colorIndex);
		if ((d->m_hilight == -1) || (stitch->colorIndex == d->m_hilight))
		{
			symbolPen.setColor(Qt::black);
			symbolPen.setWidth(documentFloss->stitchStrands());
		}
		else
		{
			symbolPen.setColor(Qt::lightGray);
			symbolPen.setWidth(0);
		}

		switch (stitch->type)
		{
			case Stitch::Delete:
				break;

			case Stitch::TLQtr:
				d->m_painter->setFont(d->m_renderQtrFont);
				d->m_painter->setPen(outlinePen);
				d->m_painter->drawLine(d->m_renderTLCell.topLeft(), d->m_renderTLCell.bottomRight());
				d->m_painter->setPen(symbolPen);
				d->m_painter->drawText(d->m_renderTLCell, Qt::AlignCenter, documentFloss->stitchSymbol());
				break;

			case Stitch::TRQtr:
				d->m_painter->setFont(d->m_renderQtrFont);
				d->m_painter->setPen(outlinePen);
				d->m_painter->drawLine(d->m_renderTRCell.bottomLeft(), d->m_renderTRCell.topRight());
				d->m_painter->setPen(symbolPen);
				d->m_painter->drawText(d->m_renderTRCell, Qt::AlignCenter, documentFloss->stitchSymbol());
				break;

			case Stitch::BLQtr:
				d->m_painter->setFont(d->m_renderQtrFont);
				d->m_painter->setPen(outlinePen);
				d->m_painter->drawLine(d->m_renderBLCell.bottomLeft(), d->m_renderBLCell.topRight());
				d->m_painter->setPen(symbolPen);
				d->m_painter->drawText(d->m_renderBLCell, Qt::AlignCenter, documentFloss->stitchSymbol());
				break;

			case Stitch::BTHalf:
				d->m_painter->setFont(d->m_render3QtrFont);
				d->m_painter->setPen(outlinePen);
				d->m_painter->drawLine(d->m_renderCell.bottomLeft(), d->m_renderCell.topRight());
				d->m_painter->setPen(symbolPen);
				d->m_painter->drawText(d->m_renderCell, Qt::AlignCenter, documentFloss->stitchSymbol());
				break;

			case Stitch::TL3Qtr:
				d->m_painter->setFont(d->m_render3QtrFont);
				d->m_painter->setPen(outlinePen);
				d->m_painter->drawLine(d->m_renderTL3Cell.bottomLeft(), d->m_renderTL3Cell.topRight());
				d->m_painter->drawLine(d->m_renderTL3Cell.center(), d->m_renderTL3Cell.topLeft());
				d->m_painter->setPen(symbolPen);
				d->m_painter->drawText(d->m_renderTL3Cell, Qt::AlignCenter, documentFloss->stitchSymbol());
				break;

			case Stitch::BRQtr:
				d->m_painter->setFont(d->m_renderQtrFont);
				d->m_painter->setPen(outlinePen);
				d->m_painter->drawLine(d->m_renderBRCell.topLeft(), d->m_renderBRCell.bottomRight());
				d->m_painter->setPen(symbolPen);
				d->m_painter->drawText(d->m_renderBRCell, Qt::AlignCenter, documentFloss->stitchSymbol());
				break;

			case Stitch::TBHalf:
				d->m_painter->setFont(d->m_render3QtrFont);
				d->m_painter->setPen(outlinePen);
				d->m_painter->drawLine(d->m_renderCell.topLeft(), d->m_renderCell.bottomRight());
				d->m_painter->setPen(symbolPen);
				d->m_painter->drawText(d->m_renderCell, Qt::AlignCenter, documentFloss->stitchSymbol());
				break;

			case Stitch::TR3Qtr:
				d->m_painter->setFont(d->m_render3QtrFont);
				d->m_painter->setPen(outlinePen);
				d->m_painter->drawLine(d->m_renderTR3Cell.topLeft(), d->m_renderTR3Cell.bottomRight());
				d->m_painter->drawLine(d->m_renderTR3Cell.center(), d->m_renderTR3Cell.topRight());
				d->m_painter->setPen(symbolPen);
				d->m_painter->drawText(d->m_renderTR3Cell, Qt::AlignCenter, documentFloss->stitchSymbol());
				break;

			case Stitch::BL3Qtr:
				d->m_painter->setFont(d->m_render3QtrFont);
				d->m_painter->setPen(outlinePen);
				d->m_painter->drawLine(d->m_renderBL3Cell.topLeft(), d->m_renderBL3Cell.bottomRight());
				d->m_painter->drawLine(d->m_renderBL3Cell.center(), d->m_renderBL3Cell.bottomLeft());
				d->m_painter->setPen(symbolPen);
				d->m_painter->drawText(d->m_renderBL3Cell, Qt::AlignCenter, documentFloss->stitchSymbol());
				break;

			case Stitch::BR3Qtr:
				d->m_painter->setFont(d->m_render3QtrFont);
				d->m_painter->setPen(outlinePen);
				d->m_painter->drawLine(d->m_renderBR3Cell.bottomLeft(), d->m_renderBR3Cell.topRight());
				d->m_painter->drawLine(d->m_renderBR3Cell.center(), d->m_renderBR3Cell.bottomRight());
				d->m_painter->setPen(symbolPen);
				d->m_painter->drawText(d->m_renderBR3Cell, Qt::AlignCenter, documentFloss->stitchSymbol());
				break;

			case Stitch::Full:
				d->m_painter->setFont(d->m_renderFont);
				d->m_painter->setPen(symbolPen);
				d->m_painter->drawText(d->m_renderCell, Qt::AlignCenter, documentFloss->stitchSymbol());
				break;

			case Stitch::TLSmallHalf:
				d->m_painter->setFont(d->m_renderQtrFont);
				d->m_painter->setPen(outlinePen);
				d->m_painter->drawLine(d->m_renderTLCell.bottomLeft(), d->m_renderTLCell.topRight());
				d->m_painter->setPen(symbolPen);
				d->m_painter->drawText(d->m_renderTLCell, Qt::AlignCenter, documentFloss->stitchSymbol());
				break;

			case Stitch::TRSmallHalf:
				d->m_painter->setFont(d->m_renderQtrFont);
				d->m_painter->setPen(outlinePen);
				d->m_painter->drawLine(d->m_renderTRCell.topLeft(), d->m_renderTRCell.bottomRight());
				d->m_painter->setPen(symbolPen);
				d->m_painter->drawText(d->m_renderTRCell, Qt::AlignCenter, documentFloss->stitchSymbol());
				break;

			case Stitch::BLSmallHalf:
				d->m_painter->setFont(d->m_renderQtrFont);
				d->m_painter->setPen(outlinePen);
				d->m_painter->drawLine(d->m_renderBLCell.topLeft(), d->m_renderBLCell.bottomRight());
				d->m_painter->setPen(symbolPen);
				d->m_painter->drawText(d->m_renderBLCell, Qt::AlignCenter, documentFloss->stitchSymbol());
				break;

			case Stitch::BRSmallHalf:
				d->m_painter->setFont(d->m_renderQtrFont);
				d->m_painter->setPen(outlinePen);
				d->m_painter->drawLine(d->m_renderBRCell.bottomLeft(), d->m_renderBRCell.topRight());
				d->m_painter->setPen(symbolPen);
				d->m_painter->drawText(d->m_renderBRCell, Qt::AlignCenter, documentFloss->stitchSymbol());
				break;

			case Stitch::TLSmallFull:
				d->m_painter->setFont(d->m_renderQtrFont);
				d->m_painter->setPen(outlinePen);
				d->m_painter->drawLine(d->m_renderTLCell.topLeft(), d->m_renderTLCell.bottomRight());
				d->m_painter->drawLine(d->m_renderTLCell.bottomLeft(), d->m_renderTLCell.topRight());
				d->m_painter->setPen(symbolPen);
				d->m_painter->drawText(d->m_renderTLCell, Qt::AlignCenter, documentFloss->stitchSymbol());
				break;

			case Stitch::TRSmallFull:
				d->m_painter->setFont(d->m_renderQtrFont);
				d->m_painter->setPen(outlinePen);
				d->m_painter->drawLine(d->m_renderTRCell.topLeft(), d->m_renderTRCell.bottomRight());
				d->m_painter->drawLine(d->m_renderTRCell.bottomLeft(), d->m_renderTRCell.topRight());
				d->m_painter->setPen(symbolPen);
				d->m_painter->drawText(d->m_renderTRCell, Qt::AlignCenter, documentFloss->stitchSymbol());
				break;

			case Stitch::BLSmallFull:
				d->m_painter->setFont(d->m_renderQtrFont);
				d->m_painter->setPen(outlinePen);
				d->m_painter->drawLine(d->m_renderBLCell.topLeft(), d->m_renderBLCell.bottomRight());
				d->m_painter->drawLine(d->m_renderBLCell.bottomLeft(), d->m_renderBLCell.topRight());
				d->m_painter->setPen(symbolPen);
				d->m_painter->drawText(d->m_renderBLCell, Qt::AlignCenter, documentFloss->stitchSymbol());
				break;

			case Stitch::BRSmallFull:
				d->m_painter->setFont(d->m_renderQtrFont);
				d->m_painter->setPen(outlinePen);
				d->m_painter->drawLine(d->m_renderBRCell.topLeft(), d->m_renderBRCell.bottomRight());
				d->m_painter->drawLine(d->m_renderBRCell.bottomLeft(), d->m_renderBRCell.topRight());
				d->m_painter->setPen(symbolPen);
				d->m_painter->drawText(d->m_renderBRCell, Qt::AlignCenter, documentFloss->stitchSymbol());
				break;
		}
	}
}


void Renderer::renderStitchesAsColorSymbols(StitchQueue *stitchQueue)
{
	QPen pen;
	StitchQueue::const_iterator it;
	for (it = stitchQueue->begin() ; it != stitchQueue->end() ; ++it)
	{
		Stitch *stitch = *it;
		DocumentFloss *documentFloss = d->m_pattern->palette().floss(stitch->colorIndex);
		if ((d->m_hilight == -1) || (stitch->colorIndex == d->m_hilight))
		{
			pen.setColor(documentFloss->flossColor());
		}
		else
		{
			pen.setColor(Qt::lightGray);
		}
		d->m_painter->setPen(pen);

		switch (stitch->type)
		{
			case Stitch::Delete:
				break;

			case Stitch::TLQtr:
				d->m_painter->setFont(d->m_renderQtrFont);
				d->m_painter->drawLine(d->m_renderTLCell.topLeft(), d->m_renderTLCell.bottomRight());
				d->m_painter->drawText(d->m_renderTLCell, Qt::AlignCenter, documentFloss->stitchSymbol());
				break;

			case Stitch::TRQtr:
				d->m_painter->setFont(d->m_renderQtrFont);
				d->m_painter->drawLine(d->m_renderTRCell.bottomLeft(), d->m_renderTRCell.topRight());
				d->m_painter->drawText(d->m_renderTRCell, Qt::AlignCenter, documentFloss->stitchSymbol());
				break;

			case Stitch::BLQtr:
				d->m_painter->setFont(d->m_renderQtrFont);
				d->m_painter->drawLine(d->m_renderBLCell.bottomLeft(), d->m_renderBLCell.topRight());
				d->m_painter->drawText(d->m_renderBLCell, Qt::AlignCenter, documentFloss->stitchSymbol());
				break;

			case Stitch::BTHalf:
				d->m_painter->setFont(d->m_render3QtrFont);
				d->m_painter->drawLine(d->m_renderCell.bottomLeft(), d->m_renderCell.topRight());
				d->m_painter->drawText(d->m_renderCell, Qt::AlignCenter, documentFloss->stitchSymbol());
				break;

			case Stitch::TL3Qtr:
				d->m_painter->setFont(d->m_render3QtrFont);
				d->m_painter->drawLine(d->m_renderTL3Cell.bottomLeft(), d->m_renderTL3Cell.topRight());
				d->m_painter->drawLine(d->m_renderTL3Cell.center(), d->m_renderTL3Cell.topLeft());
				d->m_painter->drawText(d->m_renderTL3Cell, Qt::AlignCenter, documentFloss->stitchSymbol());
				break;

			case Stitch::BRQtr:
				d->m_painter->setFont(d->m_renderQtrFont);
				d->m_painter->drawLine(d->m_renderBRCell.topLeft(), d->m_renderBRCell.bottomRight());
				d->m_painter->drawText(d->m_renderBRCell, Qt::AlignCenter, documentFloss->stitchSymbol());
				break;

			case Stitch::TBHalf:
				d->m_painter->setFont(d->m_render3QtrFont);
				d->m_painter->drawLine(d->m_renderCell.topLeft(), d->m_renderCell.bottomRight());
				d->m_painter->drawText(d->m_renderCell, Qt::AlignCenter, documentFloss->stitchSymbol());
				break;

			case Stitch::TR3Qtr:
				d->m_painter->setFont(d->m_render3QtrFont);
				d->m_painter->drawLine(d->m_renderTR3Cell.topLeft(), d->m_renderTR3Cell.bottomRight());
				d->m_painter->drawLine(d->m_renderTR3Cell.center(), d->m_renderTR3Cell.topRight());
				d->m_painter->drawText(d->m_renderTR3Cell, Qt::AlignCenter, documentFloss->stitchSymbol());
				break;

			case Stitch::BL3Qtr:
				d->m_painter->setFont(d->m_render3QtrFont);
				d->m_painter->drawLine(d->m_renderBL3Cell.topLeft(), d->m_renderBL3Cell.bottomRight());
				d->m_painter->drawLine(d->m_renderBL3Cell.center(), d->m_renderBL3Cell.bottomLeft());
				d->m_painter->drawText(d->m_renderBL3Cell, Qt::AlignCenter, documentFloss->stitchSymbol());
				break;

			case Stitch::BR3Qtr:
				d->m_painter->setFont(d->m_render3QtrFont);
				d->m_painter->drawLine(d->m_renderBR3Cell.bottomLeft(), d->m_renderBR3Cell.topRight());
				d->m_painter->drawLine(d->m_renderBR3Cell.center(), d->m_renderBR3Cell.bottomRight());
				d->m_painter->drawText(d->m_renderBR3Cell, Qt::AlignCenter, documentFloss->stitchSymbol());
				break;

			case Stitch::Full:
				d->m_painter->setFont(d->m_renderFont);
				d->m_painter->drawText(d->m_renderCell, Qt::AlignCenter, documentFloss->stitchSymbol());
				break;

			case Stitch::TLSmallHalf:
				d->m_painter->setFont(d->m_renderQtrFont);
				d->m_painter->drawLine(d->m_renderTLCell.bottomLeft(), d->m_renderTLCell.topRight());
				d->m_painter->drawText(d->m_renderTLCell, Qt::AlignCenter, documentFloss->stitchSymbol());
				break;

			case Stitch::TRSmallHalf:
				d->m_painter->setFont(d->m_renderQtrFont);
				d->m_painter->drawLine(d->m_renderTRCell.topLeft(), d->m_renderTRCell.bottomRight());
				d->m_painter->drawText(d->m_renderTRCell, Qt::AlignCenter, documentFloss->stitchSymbol());
				break;

			case Stitch::BLSmallHalf:
				d->m_painter->setFont(d->m_renderQtrFont);
				d->m_painter->drawLine(d->m_renderBLCell.topLeft(), d->m_renderBLCell.bottomRight());
				d->m_painter->drawText(d->m_renderBLCell, Qt::AlignCenter, documentFloss->stitchSymbol());
				break;

			case Stitch::BRSmallHalf:
				d->m_painter->setFont(d->m_renderQtrFont);
				d->m_painter->drawLine(d->m_renderBRCell.bottomLeft(), d->m_renderBRCell.topRight());
				d->m_painter->drawText(d->m_renderBRCell, Qt::AlignCenter, documentFloss->stitchSymbol());
				break;

			case Stitch::TLSmallFull:
				d->m_painter->setFont(d->m_renderQtrFont);
				d->m_painter->drawLine(d->m_renderTLCell.topLeft(), d->m_renderTLCell.bottomRight());
				d->m_painter->drawLine(d->m_renderTLCell.bottomLeft(), d->m_renderTLCell.topRight());
				d->m_painter->drawText(d->m_renderTLCell, Qt::AlignCenter, documentFloss->stitchSymbol());
				break;

			case Stitch::TRSmallFull:
				d->m_painter->setFont(d->m_renderQtrFont);
				d->m_painter->drawLine(d->m_renderTRCell.topLeft(), d->m_renderTRCell.bottomRight());
				d->m_painter->drawLine(d->m_renderTRCell.bottomLeft(), d->m_renderTRCell.topRight());
				d->m_painter->drawText(d->m_renderTRCell, Qt::AlignCenter, documentFloss->stitchSymbol());
				break;

			case Stitch::BLSmallFull:
				d->m_painter->setFont(d->m_renderQtrFont);
				d->m_painter->drawLine(d->m_renderBLCell.topLeft(), d->m_renderBLCell.bottomRight());
				d->m_painter->drawLine(d->m_renderBLCell.bottomLeft(), d->m_renderBLCell.topRight());
				d->m_painter->drawText(d->m_renderBLCell, Qt::AlignCenter, documentFloss->stitchSymbol());
				break;

			case Stitch::BRSmallFull:
				d->m_painter->setFont(d->m_renderQtrFont);
				d->m_painter->drawLine(d->m_renderBRCell.topLeft(), d->m_renderBRCell.bottomRight());
				d->m_painter->drawLine(d->m_renderBRCell.bottomLeft(), d->m_renderBRCell.topRight());
				d->m_painter->drawText(d->m_renderBRCell, Qt::AlignCenter, documentFloss->stitchSymbol());
				break;
		}
	}
}


void Renderer::renderStitchesAsColorBlocks(StitchQueue *stitchQueue)
{
	StitchQueue::const_iterator it;
	for (it = stitchQueue->begin() ; it != stitchQueue->end() ; ++it)
	{
		Stitch *stitch = *it;
		DocumentFloss *documentFloss = d->m_pattern->palette().floss(stitch->colorIndex);
		QBrush brush(Qt::SolidPattern);
		QPen pen;
		if ((d->m_hilight == -1) || (stitch->colorIndex == d->m_hilight))
		{
			brush.setColor(documentFloss->flossColor());
			pen.setColor(documentFloss->flossColor());
		}
		else
		{
			brush.setColor(Qt::lightGray);
			pen.setColor(Qt::lightGray);
		}
		d->m_painter->setBrush(brush);
		d->m_painter->setPen(pen);

		switch (stitch->type)
		{
			case Stitch::Delete:
				break;

			case Stitch::TLQtr:
				d->m_painter->drawPolygon(d->m_renderTLQ);
				break;

			case Stitch::TRQtr:
				d->m_painter->drawPolygon(d->m_renderTRQ);
				break;

			case Stitch::BLQtr:
				d->m_painter->drawPolygon(d->m_renderBLQ);
				break;

			case Stitch::BTHalf:
				d->m_painter->drawPolygon(d->m_renderBLTRH);
				break;

			case Stitch::TL3Qtr:
				d->m_painter->drawPolygon(d->m_renderTL3Q);
				break;

			case Stitch::BRQtr:
				d->m_painter->drawPolygon(d->m_renderBRQ);
				break;

			case Stitch::TBHalf:
				d->m_painter->drawPolygon(d->m_renderTLBRH);
				break;

			case Stitch::TR3Qtr:
				d->m_painter->drawPolygon(d->m_renderTR3Q);
				break;

			case Stitch::BL3Qtr:
				d->m_painter->drawPolygon(d->m_renderBL3Q);
				break;

			case Stitch::BR3Qtr:
				d->m_painter->drawPolygon(d->m_renderBR3Q);
				break;

			case Stitch::Full:
				d->m_painter->fillRect(d->m_renderCell, brush);
				break;

			case Stitch::TLSmallHalf:
				break;

			case Stitch::TRSmallHalf:
				break;

			case Stitch::BLSmallHalf:
				break;

			case Stitch::BRSmallHalf:
				break;

			case Stitch::TLSmallFull:
				d->m_painter->fillRect(d->m_renderTLCell, brush);
				break;

			case Stitch::TRSmallFull:
				d->m_painter->fillRect(d->m_renderTRCell, brush);
				break;

			case Stitch::BLSmallFull:
				d->m_painter->fillRect(d->m_renderBLCell, brush);
				break;

			case Stitch::BRSmallFull:
				d->m_painter->fillRect(d->m_renderBRCell, brush);
				break;
		}
	}
}


void Renderer::renderStitchesAsColorBlocksSymbols(StitchQueue *stitchQueue)
{
	StitchQueue::const_iterator it;
	for (it = stitchQueue->begin() ; it != stitchQueue->end() ; ++it)
	{
		Stitch *stitch = *it;
		DocumentFloss *documentFloss = d->m_pattern->palette().floss(stitch->colorIndex);
		QPen textPen;
		QPen outlinePen;
		QBrush brush(Qt::SolidPattern);
		if ((d->m_hilight == -1) || (stitch->colorIndex == d->m_hilight))
		{
			textPen.setColor((qGray(documentFloss->flossColor().rgb()) < 128)?Qt::white:Qt::black);
			outlinePen.setColor(documentFloss->flossColor());
			brush.setColor(documentFloss->flossColor());
		}
		else
		{
			outlinePen.setColor(Qt::lightGray);
			brush.setColor(Qt::lightGray);
			textPen.setColor(Qt::darkGray);
		}
		d->m_painter->setPen(outlinePen);
		d->m_painter->setBrush(brush);

		switch (stitch->type)
		{
			case Stitch::Delete:
				break;

			case Stitch::TLQtr:
				d->m_painter->drawPolygon(d->m_renderTLQ);
				d->m_painter->setPen(textPen);
				d->m_painter->setFont(d->m_renderQtrFont);
				d->m_painter->drawText(d->m_renderTLCell, Qt::AlignCenter, documentFloss->stitchSymbol());
				break;

			case Stitch::TRQtr:
				d->m_painter->drawPolygon(d->m_renderTRQ);
				d->m_painter->setPen(textPen);
				d->m_painter->setFont(d->m_renderQtrFont);
				d->m_painter->drawText(d->m_renderTRCell, Qt::AlignCenter, documentFloss->stitchSymbol());
				break;

			case Stitch::BLQtr:
				d->m_painter->drawPolygon(d->m_renderBLQ);
				d->m_painter->setFont(d->m_renderQtrFont);
				d->m_painter->setPen(textPen);
				d->m_painter->drawText(d->m_renderBLCell, Qt::AlignCenter, documentFloss->stitchSymbol());
				break;

			case Stitch::BTHalf:
				d->m_painter->drawPolygon(d->m_renderBLTRH);
				d->m_painter->setPen(textPen);
				d->m_painter->setFont(d->m_render3QtrFont);
				d->m_painter->drawText(d->m_renderCell, Qt::AlignCenter, documentFloss->stitchSymbol());
				break;

			case Stitch::TL3Qtr:
				d->m_painter->drawPolygon(d->m_renderTL3Q);
				d->m_painter->setPen(textPen);
				d->m_painter->setFont(d->m_render3QtrFont);
				d->m_painter->drawText(d->m_renderTL3Cell, Qt::AlignCenter, documentFloss->stitchSymbol());
				break;

			case Stitch::BRQtr:
				d->m_painter->drawPolygon(d->m_renderBRQ);
				d->m_painter->setPen(textPen);
				d->m_painter->setFont(d->m_renderQtrFont);
				d->m_painter->drawText(d->m_renderBRCell, Qt::AlignCenter, documentFloss->stitchSymbol());
				break;

			case Stitch::TBHalf:
				d->m_painter->drawPolygon(d->m_renderTLBRH);
				d->m_painter->setPen(textPen);
				d->m_painter->setFont(d->m_render3QtrFont);
				d->m_painter->drawText(d->m_renderCell, Qt::AlignCenter, documentFloss->stitchSymbol());
				break;

			case Stitch::TR3Qtr:
				d->m_painter->drawPolygon(d->m_renderTR3Q);
				d->m_painter->setPen(textPen);
				d->m_painter->setFont(d->m_render3QtrFont);
				d->m_painter->drawText(d->m_renderTR3Cell, Qt::AlignCenter, documentFloss->stitchSymbol());
				break;

			case Stitch::BL3Qtr:
				d->m_painter->drawPolygon(d->m_renderBL3Q);
				d->m_painter->setPen(textPen);
				d->m_painter->setFont(d->m_render3QtrFont);
				d->m_painter->drawText(d->m_renderBL3Cell, Qt::AlignCenter, documentFloss->stitchSymbol());
				break;

			case Stitch::BR3Qtr:
				d->m_painter->drawPolygon(d->m_renderBR3Q);
				d->m_painter->setPen(textPen);
				d->m_painter->setFont(d->m_render3QtrFont);
				d->m_painter->drawText(d->m_renderBR3Cell, Qt::AlignCenter, documentFloss->stitchSymbol());
				break;

			case Stitch::Full:
				d->m_painter->fillRect(d->m_renderCell, brush);
				d->m_painter->setPen(textPen);
				d->m_painter->setFont(d->m_renderFont);
				d->m_painter->drawText(d->m_renderCell, Qt::AlignCenter, documentFloss->stitchSymbol());
				break;

			case Stitch::TLSmallHalf:
				d->m_painter->setPen(textPen);
				d->m_painter->setFont(d->m_renderQtrFont);
				d->m_painter->drawText(d->m_renderTLCell, Qt::AlignCenter, documentFloss->stitchSymbol());
				break;

			case Stitch::TRSmallHalf:
				d->m_painter->setPen(textPen);
				d->m_painter->setFont(d->m_renderQtrFont);
				d->m_painter->drawText(d->m_renderTRCell, Qt::AlignCenter, documentFloss->stitchSymbol());
				break;

			case Stitch::BLSmallHalf:
				d->m_painter->setPen(textPen);
				d->m_painter->setFont(d->m_renderQtrFont);
				d->m_painter->drawText(d->m_renderBLCell, Qt::AlignCenter, documentFloss->stitchSymbol());
				break;

			case Stitch::BRSmallHalf:
				d->m_painter->setPen(textPen);
				d->m_painter->setFont(d->m_renderQtrFont);
				d->m_painter->drawText(d->m_renderBRCell, Qt::AlignCenter, documentFloss->stitchSymbol());
				break;

			case Stitch::TLSmallFull:
				d->m_painter->fillRect(d->m_renderTLCell, brush);
				d->m_painter->setPen(textPen);
				d->m_painter->setFont(d->m_renderQtrFont);
				d->m_painter->drawText(d->m_renderTLCell, Qt::AlignCenter, documentFloss->stitchSymbol());
				break;

			case Stitch::TRSmallFull:
				d->m_painter->fillRect(d->m_renderTRCell, brush);
				d->m_painter->setPen(textPen);
				d->m_painter->setFont(d->m_renderQtrFont);
				d->m_painter->drawText(d->m_renderTRCell, Qt::AlignCenter, documentFloss->stitchSymbol());
				break;

			case Stitch::BLSmallFull:
				d->m_painter->fillRect(d->m_renderBLCell, brush);
				d->m_painter->setPen(textPen);
				d->m_painter->setFont(d->m_renderQtrFont);
				d->m_painter->drawText(d->m_renderBLCell, Qt::AlignCenter, documentFloss->stitchSymbol());
				break;

			case Stitch::BRSmallFull:
				d->m_painter->fillRect(d->m_renderBRCell, brush);
				d->m_painter->setPen(textPen);
				d->m_painter->setFont(d->m_renderQtrFont);
				d->m_painter->drawText(d->m_renderBRCell, Qt::AlignCenter, documentFloss->stitchSymbol());
				break;
		}
	}
}


void Renderer::renderBackstitchesAsNone(Backstitch *backstitch, const QPoint &offset)
{
}


void Renderer::renderBackstitchesAsColorLines(Backstitch *backstitch, const QPoint &offset)
{
	QPoint start(((backstitch->start.x()+offset.x())*d->m_cellWidth)/2, ((backstitch->start.y()+offset.y())*d->m_cellHeight)/2);
	QPoint end(((backstitch->end.x()+offset.x())*d->m_cellWidth)/2, ((backstitch->end.y()+offset.y())*d->m_cellHeight)/2);
	DocumentFloss *documentFloss = d->m_pattern->palette().floss(backstitch->colorIndex);

	QPen pen;
	if ((d->m_hilight == -1) || (backstitch->colorIndex == d->m_hilight))
	{
		pen.setColor(documentFloss->flossColor());
		pen.setWidth(documentFloss->backstitchStrands());
	}
	else
	{
		pen.setColor(Qt::lightGray);
		pen.setWidth(0);
	}

	d->m_painter->setPen(pen);
	d->m_painter->drawLine(start, end);
}


void Renderer::renderBackstitchesAsBlackWhiteSymbols(Backstitch *backstitch, const QPoint &offset)
{
	QPoint start(((backstitch->start.x()+offset.x())*d->m_cellWidth)/2, ((backstitch->start.y()+offset.y())*d->m_cellHeight)/2);
	QPoint end(((backstitch->end.x()+offset.x())*d->m_cellWidth)/2, ((backstitch->end.y()+offset.y())*d->m_cellHeight)/2);
	DocumentFloss *documentFloss = d->m_pattern->palette().floss(backstitch->colorIndex);

	QPen pen;
	pen.setStyle(documentFloss->backstitchSymbol());
	if ((d->m_hilight == -1) || (backstitch->colorIndex == d->m_hilight))
	{
		pen.setColor(Qt::black);
		pen.setWidth(documentFloss->backstitchStrands());
	}
	else
	{
		pen.setColor(Qt::lightGray);
		pen.setWidth(0);
	}

	d->m_painter->setPen(pen);
	d->m_painter->drawLine(start, end);
}


void Renderer::renderKnotsAsNone(Knot *knot, const QPoint &offset)
{
}


void Renderer::renderKnotsAsColorBlocks(Knot *knot, const QPoint &offset)
{
}


void Renderer::renderKnotsAsColorSymbols(Knot *knot, const QPoint &offset)
{
}


void Renderer::renderKnotsAsBlackWhiteSymbols(Knot *knot, const QPoint &offset)
{
}
