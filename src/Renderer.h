/*
 * Copyright (C) 2010 by Stephen Allewell
 * sallewell@users.sourceforge.net
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#ifndef Renderer_H
#define Renderer_H


#include <QFont>
#include <QPolygon>
#include <QRect>

#include "configuration.h"


class QPainter;

class Backstitch;
class Document;
class Knot;
class Pattern;
class RendererData;
class StitchQueue;


class Renderer
{
public:
    Renderer();
    Renderer(const Renderer &);
    ~Renderer();

    void setCellGrouping(int, int);
    void setGridLineWidths(double, double);
    void setGridLineColors(QColor, QColor);

    void setRenderStitchesAs(Configuration::EnumRenderer_RenderStitchesAs::type);
    void setRenderBackstitchesAs(Configuration::EnumRenderer_RenderBackstitchesAs::type);
    void setRenderKnotsAs(Configuration::EnumRenderer_RenderKnotsAs::type);

    void render(QPainter *,
                Pattern *,
                QRect updateCells,
                bool renderGrid,
                bool renderStitches,
                bool renderBackstitches,
                bool renderKnots,
                int colorHilight);

    Renderer &operator=(const Renderer &);

private:
    typedef void (Renderer::*renderStitchCallPointer)(StitchQueue *);
    typedef void (Renderer::*renderBackstitchCallPointer)(Backstitch *);
    typedef void (Renderer::*renderKnotCallPointer)(Knot *);

    static const renderStitchCallPointer renderStitchCallPointers[];
    static const renderBackstitchCallPointer renderBackstitchCallPointers[];
    static const renderKnotCallPointer renderKnotCallPointers[];

    void renderStitchesAsStitches(StitchQueue *);
    void renderStitchesAsBlackWhiteSymbols(StitchQueue *);
    void renderStitchesAsColorSymbols(StitchQueue *);
    void renderStitchesAsColorBlocks(StitchQueue *);
    void renderStitchesAsColorBlocksSymbols(StitchQueue *);

    void renderBackstitchesAsColorLines(Backstitch *);
    void renderBackstitchesAsBlackWhiteSymbols(Backstitch *);

    void renderKnotsAsColorBlocks(Knot *);
    void renderKnotsAsColorBlocksSymbols(Knot *);
    void renderKnotsAsColorSymbols(Knot *);
    void renderKnotsAsBlackWhiteSymbols(Knot *);

    QSharedDataPointer<RendererData>    d;
};


#endif // Renderer_H
