/*
 * Copyright (C) 2010 by Stephen Allewell
 * sallewell@users.sourceforge.net
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#include "Renderer.h"

#include <QPaintEngine>
#include <QPainter>
#include <QPen>
#include <QWidget>

#include "Document.h"
#include "DocumentFloss.h"
#include "Stitch.h"
#include "Symbol.h"
#include "SymbolLibrary.h"
#include "SymbolManager.h"


class RendererData : public QSharedData
{
public:
    RendererData();
    RendererData(const RendererData &);
    ~RendererData();

    friend class Renderer;

private:
    Configuration::EnumRenderer_RenderStitchesAs::type      m_renderStitchesAs;
    Configuration::EnumRenderer_RenderBackstitchesAs::type  m_renderBackstitchesAs;
    Configuration::EnumRenderer_RenderKnotsAs::type         m_renderKnotsAs;

    QPainter    *m_painter;

    Document        *m_document;
    Pattern         *m_pattern;
    SymbolLibrary   *m_symbolLibrary;

    int     m_hilight;

    QPointF m_topLeft;
    QPointF m_topRight;
    QPointF m_bottomLeft;
    QPointF m_bottomRight;
    QPointF m_center;
    QPointF m_centerTop;
    QPointF m_centerLeft;
    QPointF m_centerRight;
    QPointF m_centerBottom;

    QRectF  m_renderCell;
    QRectF  m_renderTLCell;
    QRectF  m_renderTL3Cell;
    QRectF  m_renderTRCell;
    QRectF  m_renderTR3Cell;
    QRectF  m_renderBLCell;
    QRectF  m_renderBL3Cell;
    QRectF  m_renderBRCell;
    QRectF  m_renderBR3Cell;

    QPolygonF   m_renderTLQ;
    QPolygonF   m_renderTRQ;
    QPolygonF   m_renderBLQ;
    QPolygonF   m_renderBRQ;

    QPolygonF   m_renderBLTRH;
    QPolygonF   m_renderTLBRH;

    QPolygonF   m_renderTL3Q;
    QPolygonF   m_renderTR3Q;
    QPolygonF   m_renderBL3Q;
    QPolygonF   m_renderBR3Q;
};


RendererData::RendererData()
    :   QSharedData(),
        m_renderStitchesAs(Configuration::EnumRenderer_RenderStitchesAs::None),
        m_renderBackstitchesAs(Configuration::EnumRenderer_RenderBackstitchesAs::None),
        m_renderKnotsAs(Configuration::EnumRenderer_RenderKnotsAs::None),
        m_painter(0),
        m_document(0),
        m_pattern(0),
        m_symbolLibrary(0)
{
    m_topLeft = QPointF(0.0, 0.0);
    m_topRight = QPointF(1.0, 0.0);
    m_bottomLeft = QPointF(0.0, 1.0);
    m_bottomRight = QPointF(1.0, 1.0);
    m_center = QPointF(0.5, 0.5);
    m_centerTop = QPointF(0.5, 0.0);
    m_centerLeft = QPointF(0.0, 0.5);
    m_centerRight = QPointF(1.0, 0.5);
    m_centerBottom = QPointF(0.5, 1.0);

    double oneThird = 1.0 / 3.0;
    double twoThird = 2.0 / 3.0;

    m_renderCell = QRectF(m_topLeft, m_bottomRight);
    m_renderTLCell = QRectF(m_topLeft, m_center);
    m_renderTL3Cell = QRectF(m_topLeft, QSize(twoThird, twoThird));
    m_renderTRCell = QRectF(m_centerTop, m_centerRight);
    m_renderTR3Cell = QRectF(oneThird, 0, twoThird, twoThird);
    m_renderBLCell = QRectF(m_centerLeft, m_centerBottom);
    m_renderBL3Cell = QRectF(0, oneThird, twoThird, twoThird);
    m_renderBRCell = QRectF(m_center, m_bottomRight);
    m_renderBR3Cell = QRectF(oneThird, oneThird, twoThird, twoThird);

    m_renderTLQ << m_topLeft << m_centerTop << m_centerLeft;
    m_renderTRQ << m_centerTop << m_topRight << m_centerRight;
    m_renderBLQ << m_centerLeft << m_centerBottom << m_bottomLeft;
    m_renderBRQ << m_centerRight << m_bottomRight << m_centerBottom;
    m_renderBLTRH << m_bottomLeft << m_centerLeft << m_centerTop << m_topRight << m_centerRight << m_centerBottom;
    m_renderTLBRH << m_topLeft << m_centerTop << m_centerRight << m_bottomRight << m_centerBottom << m_centerLeft;
    m_renderTL3Q << m_topLeft << m_topRight << m_centerRight << m_centerBottom << m_bottomLeft;
    m_renderTR3Q << m_topLeft << m_topRight << m_bottomRight << m_centerBottom << m_centerLeft;
    m_renderBL3Q << m_topLeft << m_centerTop << m_centerRight << m_bottomRight << m_bottomLeft;
    m_renderBR3Q << m_centerTop << m_topRight << m_bottomRight << m_bottomLeft << m_centerLeft;
}


RendererData::RendererData(const RendererData &other)
    :   QSharedData(other),
        m_renderStitchesAs(other.m_renderStitchesAs),
        m_renderBackstitchesAs(other.m_renderBackstitchesAs),
        m_renderKnotsAs(other.m_renderKnotsAs),
        m_document(other.m_document),
        m_pattern(other.m_pattern),
        m_symbolLibrary(other.m_symbolLibrary),
        m_topLeft(other.m_topLeft),
        m_topRight(other.m_topRight),
        m_bottomLeft(other.m_bottomLeft),
        m_bottomRight(other.m_bottomRight),
        m_center(other.m_center),
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
        m_renderBR3Q(other.m_renderBR3Q)
{
}


RendererData::~RendererData()
{
}


const Renderer::renderStitchCallPointer Renderer::renderStitchCallPointers[] = {
    &Renderer::renderStitchesAsNone,
    &Renderer::renderStitchesAsStitches,
    &Renderer::renderStitchesAsBlackWhiteSymbols,
    &Renderer::renderStitchesAsColorSymbols,
    &Renderer::renderStitchesAsColorBlocks,
    &Renderer::renderStitchesAsColorBlocksSymbols,
};

const Renderer::renderBackstitchCallPointer Renderer::renderBackstitchCallPointers[] = {
    &Renderer::renderBackstitchesAsNone,
    &Renderer::renderBackstitchesAsColorLines,
    &Renderer::renderBackstitchesAsBlackWhiteSymbols,
};

const Renderer::renderKnotCallPointer Renderer::renderKnotCallPointers[] = {
    &Renderer::renderKnotsAsNone,
    &Renderer::renderKnotsAsColorBlocks,
    &Renderer::renderKnotsAsColorBlocksSymbols,
    &Renderer::renderKnotsAsColorSymbols,
    &Renderer::renderKnotsAsBlackWhiteSymbols,
};


Renderer::Renderer()
    :   d(new RendererData)
{
}


Renderer::Renderer(const Renderer &other)
    :   d(other.d)
{
}


Renderer::~Renderer()
{
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


void Renderer::render(QPainter *painter,
                      Pattern *pattern,
                      const QRect &updateRectangle,
                      bool renderGrid,
                      bool renderStitches,
                      bool renderBackstitches,
                      bool renderKnots,
                      int colorHilight)
{
    painter->save();

    d->m_painter = painter;
    d->m_pattern = pattern;
    d->m_symbolLibrary = SymbolManager::library("kxstitch");    // TODO option for symbol library selection
    d->m_hilight = colorHilight;

    int patternLeft = painter->window().left();
    int patternRight = painter->window().right();
    int patternTop = painter->window().top();
    int patternBottom = painter->window().bottom();
    int patternWidth = painter->window().width();
    int patternHeight = painter->window().height();

    // draw grid
    if (renderGrid) {
        QPen thickPen;
        QPen thinPen;
        thickPen.setWidthF(0.15);
        thinPen.setWidthF(0.025);

        for (int y = patternTop ; y <= patternTop + patternHeight ; ++y) {
            painter->setPen((y % 10) ? thinPen : thickPen);
            painter->drawLine(patternLeft, y, patternLeft + patternWidth, y);
        }

        for (int x = patternLeft ; x <= patternLeft + patternWidth ; ++x) {
            painter->setPen((x % 10) ? thinPen : thickPen);
            painter->drawLine(x, patternTop, x, patternTop + patternHeight);
        }
    }

    QTransform transform = painter->transform();
    if (renderStitches) {
        for (int y = patternTop ; y <= patternBottom ; ++y) {
            for (int x = patternLeft ; x <= patternRight ; ++x) {
                QTransform tranform = painter->transform();
                painter->translate(x, y);

                if (StitchQueue *queue = pattern->stitches().stitchQueueAt(QPoint(x, y))) {
                    (this->*renderStitchCallPointers[d->m_renderStitchesAs])(queue);
                }

                painter->setTransform(transform);
            }
        }
    }

    if (renderBackstitches) {
        QList<Backstitch*> backstitches = pattern->stitches().backstitches();

        for (int i = 0 ; i < backstitches.count() ; ++i) {
            (this->*renderBackstitchCallPointers[d->m_renderBackstitchesAs])(backstitches.at(i));
        }
    }

    if (renderKnots) {
        QList<Knot*> knots = pattern->stitches().knots();

        for (int i = 0 ; i < knots.count() ; ++i) {
            (this->*renderKnotCallPointers[d->m_renderKnotsAs])(knots.at(i));
        }
    }

    painter->restore();
}


void Renderer::renderStitchesAsNone(StitchQueue *)
{
}


void Renderer::renderStitchesAsStitches(StitchQueue *stitchQueue)
{
    int i = stitchQueue->count();

    while (i) {
        Stitch *stitch = stitchQueue->at(--i);
        DocumentFloss *documentFloss = d->m_pattern->palette().flosses().value(stitch->colorIndex);

        QPen pen;

        if ((d->m_hilight == -1) || (stitch->colorIndex == d->m_hilight)) {
            pen.setColor(documentFloss->flossColor());
            pen.setWidthF(documentFloss->stitchStrands() / 10.0);
        } else {
            pen.setColor(Qt::lightGray);
            pen.setWidthF(0);
        }

        pen.setCapStyle(Qt::RoundCap);
        d->m_painter->setPen(pen);

        switch (stitch->type) {
        case Stitch::Delete:
            break;

        case Stitch::TLQtr:
            d->m_painter->drawLine(d->m_topLeft, d->m_center);
            break;

        case Stitch::TRQtr:
            d->m_painter->drawLine(d->m_topRight, d->m_center);
            break;

        case Stitch::BLQtr:
            d->m_painter->drawLine(d->m_bottomLeft, d->m_center);
            break;

        case Stitch::BTHalf:
            d->m_painter->drawLine(d->m_bottomLeft, d->m_topRight);
            break;

        case Stitch::TL3Qtr:
            d->m_painter->drawLine(d->m_bottomLeft, d->m_topRight);
            d->m_painter->drawLine(d->m_topLeft, d->m_center);
            break;

        case Stitch::BRQtr:
             d->m_painter->drawLine(d->m_center, d->m_bottomRight);
            break;

        case Stitch::TBHalf:
            d->m_painter->drawLine(d->m_topLeft, d->m_bottomRight);
            break;

        case Stitch::TR3Qtr:
            d->m_painter->drawLine(d->m_topLeft, d->m_bottomRight);
            d->m_painter->drawLine(d->m_topRight, d->m_center);
            break;

        case Stitch::BL3Qtr:
            d->m_painter->drawLine(d->m_topLeft, d->m_bottomRight);
            d->m_painter->drawLine(d->m_bottomLeft, d->m_center);
            break;

        case Stitch::BR3Qtr:
            d->m_painter->drawLine(d->m_bottomLeft, d->m_topRight);
            d->m_painter->drawLine(d->m_center, d->m_bottomRight);
            break;

        case Stitch::Full:
            d->m_painter->drawLine(d->m_topLeft, d->m_bottomRight);
            d->m_painter->drawLine(d->m_bottomLeft, d->m_topRight);
            break;

        case Stitch::TLSmallHalf:
            d->m_painter->drawLine(d->m_centerLeft, d->m_centerTop);
            break;

        case Stitch::TRSmallHalf:
            d->m_painter->drawLine(d->m_centerTop, d->m_centerRight);
            break;

        case Stitch::BLSmallHalf:
            d->m_painter->drawLine(d->m_centerLeft, d->m_centerBottom);
            break;

        case Stitch::BRSmallHalf:
            d->m_painter->drawLine(d->m_centerBottom, d->m_centerRight);
            break;

        case Stitch::TLSmallFull:
            d->m_painter->drawLine(d->m_topLeft, d->m_center);
            d->m_painter->drawLine(d->m_centerLeft, d->m_centerTop);
            break;

        case Stitch::TRSmallFull:
            d->m_painter->drawLine(d->m_centerTop, d->m_centerRight);
            d->m_painter->drawLine(d->m_center, d->m_topRight);
            break;

        case Stitch::BLSmallFull:
            d->m_painter->drawLine(d->m_bottomLeft, d->m_center);
            d->m_painter->drawLine(d->m_centerLeft, d->m_centerBottom);
            break;

        case Stitch::BRSmallFull:
            d->m_painter->drawLine(d->m_center, d->m_bottomRight);
            d->m_painter->drawLine(d->m_centerBottom, d->m_centerRight);
            break;
        }
    }
}


void Renderer::renderStitchesAsBlackWhiteSymbols(StitchQueue *stitchQueue)
{
    int i = stitchQueue->count();

    while (i) {
        Stitch *stitch = stitchQueue->at(--i);
        DocumentFloss *floss = d->m_pattern->palette().flosses().value(stitch->colorIndex);
        Symbol symbol = d->m_symbolLibrary->symbol(floss->stitchSymbol());

        QPen outlinePen(Qt::lightGray);
        QPen symbolPen = symbol.pen();
        QBrush symbolBrush = symbol.brush();

        if ((d->m_hilight == -1) || (stitch->colorIndex == d->m_hilight)) {
            // the symbolPen and symbolBrush are already set up as black at this point
        } else {
            symbolPen.setColor(Qt::lightGray);
            symbolBrush.setColor(Qt::lightGray);
        }

        d->m_painter->setPen(symbolPen);
        d->m_painter->setBrush(symbolBrush);

        d->m_painter->drawPath(symbol.path(stitch->type));

        d->m_painter->setPen(outlinePen);

        switch (stitch->type) {
        case Stitch::Delete:
            break;

        case Stitch::TLQtr:
            d->m_painter->drawLine(d->m_topLeft, d->m_center);
            break;

        case Stitch::TRQtr:
            d->m_painter->drawLine(d->m_center, d->m_topRight);
            break;

        case Stitch::BLQtr:
            d->m_painter->drawLine(d->m_bottomLeft, d->m_center);
            break;

        case Stitch::BTHalf:
            d->m_painter->drawLine(d->m_bottomLeft, d->m_topRight);
            break;

        case Stitch::TL3Qtr:
            d->m_painter->drawLine(d->m_topRight, d->m_bottomLeft);
            d->m_painter->drawLine(d->m_topLeft, d->m_center);
            break;

        case Stitch::BRQtr:
            d->m_painter->drawLine(d->m_center, d->m_bottomRight);
            break;

        case Stitch::TBHalf:
            d->m_painter->drawLine(d->m_topLeft, d->m_bottomRight);
            break;

        case Stitch::TR3Qtr:
            d->m_painter->drawLine(d->m_topLeft, d->m_bottomRight);
            d->m_painter->drawLine(d->m_topRight, d->m_center);
            break;

        case Stitch::BL3Qtr:
            d->m_painter->drawLine(d->m_topLeft, d->m_bottomRight);
            d->m_painter->drawLine(d->m_bottomLeft, d->m_center);
            break;

        case Stitch::BR3Qtr:
            d->m_painter->drawLine(d->m_bottomLeft, d->m_topRight);
            d->m_painter->drawLine(d->m_center, d->m_bottomRight);
            break;

        case Stitch::Full:
            break;

        case Stitch::TLSmallHalf:
            d->m_painter->drawLine(d->m_centerLeft, d->m_centerTop);
            break;

        case Stitch::TRSmallHalf:
            d->m_painter->drawLine(d->m_centerTop, d->m_centerRight);
            break;

        case Stitch::BLSmallHalf:
            d->m_painter->drawLine(d->m_centerLeft, d->m_centerBottom);
            break;

        case Stitch::BRSmallHalf:
            d->m_painter->drawLine(d->m_centerBottom, d->m_centerRight);
            break;

        case Stitch::TLSmallFull:
            d->m_painter->drawLine(d->m_topLeft, d->m_center);
            d->m_painter->drawLine(d->m_centerTop, d->m_centerLeft);
            break;

        case Stitch::TRSmallFull:
            d->m_painter->drawLine(d->m_centerTop, d->m_centerRight);
            d->m_painter->drawLine(d->m_center, d->m_topRight);
            break;

        case Stitch::BLSmallFull:
            d->m_painter->drawLine(d->m_centerLeft, d->m_centerBottom);
            d->m_painter->drawLine(d->m_bottomLeft, d->m_center);
            break;

        case Stitch::BRSmallFull:
            d->m_painter->drawLine(d->m_center, d->m_bottomRight);
            d->m_painter->drawLine(d->m_centerRight, d->m_centerBottom);
            break;
        }
    }
}


void Renderer::renderStitchesAsColorSymbols(StitchQueue *stitchQueue)
{
    int i = stitchQueue->count();

    while (i) {
        Stitch *stitch = stitchQueue->at(--i);
        DocumentFloss *documentFloss = d->m_pattern->palette().flosses().value(stitch->colorIndex);
        Symbol symbol = d->m_symbolLibrary->symbol(documentFloss->stitchSymbol());

        QPen outlinePen(Qt::lightGray);
        QPen symbolPen = symbol.pen();
        QBrush symbolBrush = symbol.brush();

        if ((d->m_hilight == -1) || (stitch->colorIndex == d->m_hilight)) {
            symbolPen.setColor(documentFloss->flossColor());
            symbolBrush.setColor(documentFloss->flossColor());
        } else {
            symbolPen.setColor(Qt::lightGray);
            symbolBrush.setColor(Qt::lightGray);
        }

        d->m_painter->setPen(symbolPen);
        d->m_painter->setBrush(symbolBrush);

        d->m_painter->drawPath(symbol.path(stitch->type));

        d->m_painter->setPen(outlinePen);

        switch (stitch->type) {
        case Stitch::Delete:
            break;

        case Stitch::TLQtr:
            d->m_painter->drawLine(d->m_topLeft, d->m_center);
            break;

        case Stitch::TRQtr:
            d->m_painter->drawLine(d->m_center, d->m_topRight);
            break;

        case Stitch::BLQtr:
            d->m_painter->drawLine(d->m_bottomLeft, d->m_center);
            break;

        case Stitch::BTHalf:
            d->m_painter->drawLine(d->m_bottomLeft, d->m_topRight);
            break;

        case Stitch::TL3Qtr:
            d->m_painter->drawLine(d->m_topRight, d->m_bottomLeft);
            d->m_painter->drawLine(d->m_topLeft, d->m_center);
            break;

        case Stitch::BRQtr:
            d->m_painter->drawLine(d->m_center, d->m_bottomRight);
            break;

        case Stitch::TBHalf:
            d->m_painter->drawLine(d->m_topLeft, d->m_bottomRight);
            break;

        case Stitch::TR3Qtr:
            d->m_painter->drawLine(d->m_topLeft, d->m_bottomRight);
            d->m_painter->drawLine(d->m_topRight, d->m_center);
            break;

        case Stitch::BL3Qtr:
            d->m_painter->drawLine(d->m_topLeft, d->m_bottomRight);
            d->m_painter->drawLine(d->m_bottomLeft, d->m_center);
            break;

        case Stitch::BR3Qtr:
            d->m_painter->drawLine(d->m_bottomLeft, d->m_topRight);
            d->m_painter->drawLine(d->m_center, d->m_bottomRight);
            break;

        case Stitch::Full:
            break;

        case Stitch::TLSmallHalf:
            d->m_painter->drawLine(d->m_centerLeft, d->m_centerTop);
            break;

        case Stitch::TRSmallHalf:
            d->m_painter->drawLine(d->m_centerTop, d->m_centerRight);
            break;

        case Stitch::BLSmallHalf:
            d->m_painter->drawLine(d->m_centerLeft, d->m_centerBottom);
            break;

        case Stitch::BRSmallHalf:
            d->m_painter->drawLine(d->m_centerBottom, d->m_centerRight);
            break;

        case Stitch::TLSmallFull:
            d->m_painter->drawLine(d->m_topLeft, d->m_center);
            d->m_painter->drawLine(d->m_centerTop, d->m_centerLeft);
            break;

        case Stitch::TRSmallFull:
            d->m_painter->drawLine(d->m_centerTop, d->m_centerRight);
            d->m_painter->drawLine(d->m_center, d->m_topRight);
            break;

        case Stitch::BLSmallFull:
            d->m_painter->drawLine(d->m_centerLeft, d->m_centerBottom);
            d->m_painter->drawLine(d->m_bottomLeft, d->m_center);
            break;

        case Stitch::BRSmallFull:
            d->m_painter->drawLine(d->m_center, d->m_bottomRight);
            d->m_painter->drawLine(d->m_centerRight, d->m_centerBottom);
            break;
        }
    }
}


void Renderer::renderStitchesAsColorBlocks(StitchQueue *stitchQueue)
{
    int i = stitchQueue->count();

    while (i) {
        Stitch *stitch = stitchQueue->at(--i);
        DocumentFloss *documentFloss = d->m_pattern->palette().flosses().value(stitch->colorIndex);

        QPen blockPen;
        QBrush blockBrush(Qt::SolidPattern);

        if ((d->m_hilight == -1) || (stitch->colorIndex == d->m_hilight)) {
            blockPen.setColor(documentFloss->flossColor());
            blockBrush.setColor(documentFloss->flossColor());
        } else {
            blockPen.setColor(Qt::lightGray);
            blockBrush.setColor(documentFloss->flossColor());
        }

        d->m_painter->setPen(blockPen);
        d->m_painter->setBrush(blockBrush);

        switch (stitch->type) {
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
            d->m_painter->fillRect(d->m_renderCell, blockBrush);
            break;

        case Stitch::TLSmallHalf:
            d->m_painter->fillRect(d->m_renderTLCell, blockBrush);
            break;

        case Stitch::TRSmallHalf:
            d->m_painter->fillRect(d->m_renderTRCell, blockBrush);
            break;

        case Stitch::BLSmallHalf:
            d->m_painter->fillRect(d->m_renderBLCell, blockBrush);
            break;

        case Stitch::BRSmallHalf:
            d->m_painter->fillRect(d->m_renderBRCell, blockBrush);
            break;

        case Stitch::TLSmallFull:
            d->m_painter->fillRect(d->m_renderTLCell, blockBrush);
            break;

        case Stitch::TRSmallFull:
            d->m_painter->fillRect(d->m_renderTRCell, blockBrush);
            break;

        case Stitch::BLSmallFull:
            d->m_painter->fillRect(d->m_renderBLCell, blockBrush);
            break;

        case Stitch::BRSmallFull:
            d->m_painter->fillRect(d->m_renderBRCell, blockBrush);
            break;
        }
    }
}


void Renderer::renderStitchesAsColorBlocksSymbols(StitchQueue *stitchQueue)
{
    int i = stitchQueue->count();

    while (i) {
        Stitch *stitch = stitchQueue->at(--i);
        DocumentFloss *floss = d->m_pattern->palette().flosses().value(stitch->colorIndex);
        Symbol symbol = d->m_symbolLibrary->symbol(floss->stitchSymbol());

        QPen symbolPen = symbol.pen();
        QBrush symbolBrush = symbol.brush();
        QPen blockPen;
        QBrush blockBrush(Qt::SolidPattern);

        if ((d->m_hilight == -1) || (stitch->colorIndex == d->m_hilight)) {
            QColor flossColor = floss->flossColor();
            QColor symbolColor = (qGray(flossColor.rgb()) < 128) ? Qt::white : Qt::black;
            symbolPen.setColor(symbolColor);
            symbolBrush.setColor(symbolColor);
            blockPen.setColor(flossColor);
            blockBrush.setColor(flossColor);
        } else {
            symbolPen.setColor(Qt::darkGray);
            symbolBrush.setColor(Qt::darkGray);
            blockPen.setColor(Qt::lightGray);
            blockBrush.setColor(Qt::lightGray);
        }

        d->m_painter->setPen(blockPen);
        d->m_painter->setBrush(blockBrush);

        switch (stitch->type) {
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
            d->m_painter->fillRect(d->m_renderCell, blockBrush);
            break;

        case Stitch::TLSmallHalf:
            d->m_painter->fillRect(d->m_renderTLCell, blockBrush);
            break;

        case Stitch::TRSmallHalf:
            d->m_painter->fillRect(d->m_renderTRCell, blockBrush);
            break;

        case Stitch::BLSmallHalf:
            d->m_painter->fillRect(d->m_renderBLCell, blockBrush);
            break;

        case Stitch::BRSmallHalf:
            d->m_painter->fillRect(d->m_renderBRCell, blockBrush);
            break;

        case Stitch::TLSmallFull:
            d->m_painter->fillRect(d->m_renderTLCell, blockBrush);
            break;

        case Stitch::TRSmallFull:
            d->m_painter->fillRect(d->m_renderTRCell, blockBrush);
            break;

        case Stitch::BLSmallFull:
            d->m_painter->fillRect(d->m_renderBLCell, blockBrush);
            break;

        case Stitch::BRSmallFull:
            d->m_painter->fillRect(d->m_renderBRCell, blockBrush);
            break;
        }

        d->m_painter->setPen(symbolPen);
        d->m_painter->setBrush(symbolBrush);

        d->m_painter->drawPath(symbol.path(stitch->type));
    }
}


void Renderer::renderBackstitchesAsNone(Backstitch*)
{
}


void Renderer::renderBackstitchesAsColorLines(Backstitch *backstitch)
{
    QPointF start(QPointF(backstitch->start) / 2);
    QPointF end(QPointF(backstitch->end) / 2);

    DocumentFloss *documentFloss = d->m_pattern->palette().floss(backstitch->colorIndex);

    QPen pen;

    if ((d->m_hilight == -1) || (backstitch->colorIndex == d->m_hilight)) {
        pen.setColor(documentFloss->flossColor());
        pen.setWidthF(double(documentFloss->backstitchStrands()) / 5);
        pen.setCapStyle(Qt::RoundCap);
    } else {
        pen.setColor(Qt::lightGray);
        pen.setWidth(0);
    }

    d->m_painter->setPen(pen);
    d->m_painter->drawLine(start, end);
}


void Renderer::renderBackstitchesAsBlackWhiteSymbols(Backstitch *backstitch)
{
    QPointF start(QPointF(backstitch->start) / 2);
    QPointF end(QPointF(backstitch->end) / 2);

    DocumentFloss *documentFloss = d->m_pattern->palette().floss(backstitch->colorIndex);

    QPen pen;
    pen.setStyle(documentFloss->backstitchSymbol());

    if ((d->m_hilight == -1) || (backstitch->colorIndex == d->m_hilight)) {
        pen.setColor(Qt::black);
        pen.setWidthF(double(documentFloss->backstitchStrands()) / 5);
        pen.setCapStyle(Qt::RoundCap);
    } else {
        pen.setColor(Qt::lightGray);
        pen.setWidth(0);
    }

    d->m_painter->setPen(pen);
    d->m_painter->drawLine(start, end);
}


void Renderer::renderKnotsAsNone(Knot*)
{
}


void Renderer::renderKnotsAsColorBlocks(Knot *knot)
{
    DocumentFloss *documentFloss = d->m_pattern->palette().floss(knot->colorIndex);
    QPen outlinePen;
    QBrush brush(Qt::SolidPattern);

    if ((d->m_hilight == -1) || (knot->colorIndex == d->m_hilight)) {
        outlinePen.setColor(documentFloss->flossColor());
        brush.setColor(documentFloss->flossColor());
    } else {
        outlinePen.setColor(Qt::lightGray);
        brush.setColor(Qt::lightGray);
    }

    d->m_painter->setPen(outlinePen);
    d->m_painter->setBrush(brush);

    double knotSize = 0.25;
    QRectF rect(0, 0, knotSize, knotSize);
    rect.moveCenter(QPointF(knot->position) / 2);
    d->m_painter->drawEllipse(rect);
}


void Renderer::renderKnotsAsColorBlocksSymbols(Knot *knot)
{
}


void Renderer::renderKnotsAsColorSymbols(Knot *knot)
{
}


void Renderer::renderKnotsAsBlackWhiteSymbols(Knot *knot)
{
}


Renderer &Renderer::operator=(const Renderer &other)
{
    d = other.d;
    return *this;
}
