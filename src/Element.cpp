/*
 * Copyright (C) 2010 by Stephen Allewell
 * stephen@mirramar.adsl24.co.uk
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#include "Element.h"

#include <math.h>

#include <QPainter>

#include <KLocale>

#include "Document.h"
#include "FlossScheme.h"
#include "Page.h"
#include "Renderer.h"
#include "SchemeManager.h"
#include "Symbol.h"
#include "SymbolLibrary.h"
#include "SymbolManager.h"


/**
    Convenience function to round a double to a fixed number of decimal places
    @param v the double value to round
    @param n the number of decimal places
    @return a double value rounded to the number of decimal places.
    */
double round_n(double v, int n)
{
    double places = pow(10.0, n);
    return round(v * places) / places;
}


Element::Element(Page *parent, const QRect &rectangle, Element::Type type)
    :   m_parent(parent),
        m_rectangle(rectangle),
        m_type(type)
{
}


Element::Element(const Element &other)
    :   m_parent(0),                    // needs to be reparented by cloner
        m_rectangle(other.m_rectangle),
        m_type(other.m_type)
{
}


Element::~Element()
{
}


Page *Element::parent() const
{
    return m_parent;
}


Element::Type Element::type() const
{
    return m_type;
}


const QRect &Element::rectangle() const
{
    return m_rectangle;
}


void Element::setParent(Page *parent)
{
    m_parent = parent;
}


void Element::setRectangle(const QRect &rectangle)
{
    m_rectangle = rectangle;
}


void Element::move(const QPoint &offset)
{
    m_rectangle.translate(offset);
}


QDataStream &operator<<(QDataStream &stream, const Element &element)
{
    return element.streamOut(stream);
}


QDataStream &operator>>(QDataStream &stream, Element &element)
{
    return element.streamIn(stream);
}


QDataStream &Element::streamOut(QDataStream &stream) const
{
    stream << qint32(version);

    stream  << m_rectangle;

    return stream;
}


QDataStream &Element::streamIn(QDataStream &stream)
{
    qint32 version;
    qint32 visible;

    stream >> version;

    switch (version) {
    case 101:
        stream  >> m_rectangle;
        break;

    case 100:
        stream  >> m_rectangle
                >> visible;     // ignore
        break;

    default:
        // not supported
        // throw exception
        break;
    }

    return stream;
}


KeyElement::KeyElement(Page *parent, const QRect &rectangle, Element::Type type)
    :   Element(parent, rectangle, type),
        m_showBorder(Configuration::keyElement_ShowBorder()),
        m_borderColor(Configuration::keyElement_BorderColor()),
        m_borderThickness(Configuration::keyElement_BorderThickness()),
        m_fillBackground(Configuration::keyElement_FillBackground()),
        m_backgroundColor(Configuration::keyElement_BackgroundColor()),
        m_backgroundTransparency(Configuration::keyElement_BackgroundTransparency()),
        m_margins(QMargins(Configuration::keyElement_MarginLeft(), Configuration::keyElement_MarginTop(), Configuration::keyElement_MarginRight(), Configuration::keyElement_MarginBottom())),
        m_indexStart(0),
        m_indexCount(-1),
        m_symbolColumn(Configuration::keyElement_SymbolColumn()),
        m_flossNameColumn(Configuration::keyElement_FlossNameColumn()),
        m_strandsColumn(Configuration::keyElement_StrandsColumn()),
        m_flossDescriptionColumn(Configuration::keyElement_FlossDescriptionColumn()),
        m_stitchesColumn(Configuration::keyElement_StitchesColumn()),
        m_lengthColumn(Configuration::keyElement_LengthColumn()),
        m_skeinsColumn(Configuration::keyElement_SkeinsColumn())
{
}


KeyElement::KeyElement(const KeyElement &other)
    :   Element(other),
        m_showBorder(other.m_showBorder),
        m_borderColor(other.m_borderColor),
        m_borderThickness(other.m_borderThickness),
        m_fillBackground(other.m_fillBackground),
        m_backgroundColor(other.m_backgroundColor),
        m_backgroundTransparency(other.m_backgroundTransparency),
        m_margins(other.m_margins),
        m_textColor(other.m_textColor),
        m_textFont(other.m_textFont),
        m_indexStart(other.m_indexStart),
        m_indexCount(other.m_indexCount),
        m_symbolColumn(other.m_symbolColumn),
        m_flossNameColumn(other.m_flossNameColumn),
        m_strandsColumn(other.m_strandsColumn),
        m_flossDescriptionColumn(other.m_flossDescriptionColumn),
        m_stitchesColumn(other.m_stitchesColumn),
        m_lengthColumn(other.m_lengthColumn),
        m_skeinsColumn(other.m_skeinsColumn)
{
}


KeyElement::~KeyElement()
{
}


KeyElement *KeyElement::clone() const
{
    return new KeyElement(*this);
}


void KeyElement::render(Document *document, QPainter *painter) const
{
    painter->save();

    double unitLength = (1 / (document->property("horizontalClothCount").toDouble() * (static_cast<Configuration::EnumEditor_ClothCountUnits::type>(document->property("clothCountUnits").toInt() == Configuration::EnumEditor_ClothCountUnits::CM) ? 2.54 : 1.0))) * 0.0254;
    QMap<int, FlossUsage> flossUsage = document->pattern()->stitches().flossUsage();
    QMap<int, DocumentFloss *> flosses = document->pattern()->palette().flosses();
    QVector<int> sortedFlosses = document->pattern()->palette().sortedFlosses();

    FlossScheme *scheme = SchemeManager::scheme(document->pattern()->palette().schemeName());

    double deviceVRatio = double(painter->device()->height()) / double(painter->window().height());

    // set the viewport to be the rectangle converted to device coordinates
    painter->setViewport(painter->combinedTransform().mapRect(m_rectangle));
    // set the window to be the size of the rectangle in mm which the viewport will be mapped to.
    painter->setWindow(0, 0, m_rectangle.width(), m_rectangle.height());

    QPen pen(m_borderColor);
    pen.setWidthF(double(m_borderThickness) / 10.0);

    if (m_showBorder) {
        painter->setPen(pen);
    } else {
        painter->setPen(Qt::NoPen);
    }

    QColor backgroundColor = m_backgroundColor;
    backgroundColor.setAlpha(m_backgroundTransparency);

    if (m_fillBackground) {
        painter->setBrush(backgroundColor);
    } else {
        painter->setBrush(Qt::NoBrush);
    }

    painter->drawRect(painter->window());

    QFont font = m_textFont;
    font.setPixelSize(int(((font.pointSizeF() / 72.0) * 25.4) * deviceVRatio));

    QRect deviceTextArea = painter->combinedTransform().mapRect(QRect(0, 0, m_rectangle.width(), m_rectangle.height()).adjusted(m_margins.left(), m_margins.top(), -m_margins.left(), -m_margins.bottom()));

    painter->resetTransform();
    painter->setClipRect(deviceTextArea);

    pen.setColor(m_textColor);
    painter->setPen(pen);

    painter->setFont(font);

    QFontMetrics fontMetrics(painter->font(), painter->device());
    int lineSpacing = fontMetrics.lineSpacing();
    int ascent = fontMetrics.ascent();
    int y = lineSpacing;
    int symbolWidth = 0;
    int flossNameWidth = 0;
    int strandsWidth = 0;
    int flossDescriptionWidth = 0;
    int stitchesWidth = 0;
    int lengthWidth = 0;
    int skeinsWidth = 0;

    QVectorIterator<int> sortedFlossesIterator(sortedFlosses);

    while (sortedFlossesIterator.hasNext()) {
        int index = sortedFlossesIterator.next();
        FlossUsage usage = flossUsage[index];

        if (m_flossNameColumn) {
            flossNameWidth = std::max(flossNameWidth, fontMetrics.width(flosses[index]->flossName()));
        }

        if (m_strandsColumn) {
            strandsWidth = std::max(strandsWidth, fontMetrics.width(QString("%1 / %2").arg(flosses[index]->stitchStrands()).arg(flosses[index]->backstitchStrands())));
        }

        if (m_flossDescriptionColumn) {
            flossDescriptionWidth = std::max(flossDescriptionWidth, fontMetrics.width(scheme->find(flosses[index]->flossName())->description()));
        }

        if (m_stitchesColumn) {
            stitchesWidth = std::max(stitchesWidth, fontMetrics.width(QString("%1").arg(usage.totalStitches())));
        }

        double flossLength = round_n(usage.stitchLength() * unitLength * flosses[index]->stitchStrands() + usage.backstitchLength * unitLength * flosses[index]->backstitchStrands(), 2);

        if (m_lengthColumn) {
            lengthWidth = std::max(lengthWidth, fontMetrics.width(QString("%1").arg(flossLength)));
        }

        if (m_skeinsColumn) {
            skeinsWidth = std::max(skeinsWidth, fontMetrics.width(QString("%1").arg(flossLength / 48)));    // 1 skein = 6 strands of 8m
        }
    }

    font.setBold(true);
    fontMetrics = QFontMetrics(font, painter->device());

    if (m_symbolColumn) {
        symbolWidth = std::max(symbolWidth, fontMetrics.width(i18n("Symbol")));
    }

    if (m_flossNameColumn) {
        flossNameWidth = std::max(flossNameWidth, fontMetrics.width(i18nc("The name of the floss", "Name")));
    }

    if (m_strandsColumn) {
        strandsWidth = std::max(strandsWidth, fontMetrics.width(i18n("Strands")));
    }

    if (m_flossDescriptionColumn) {
        flossDescriptionWidth = std::max(flossDescriptionWidth, fontMetrics.width(i18n("Description")));
    }

    if (m_stitchesColumn) {
        stitchesWidth = std::max(stitchesWidth, fontMetrics.width(i18n("Stitches")));
    }

    if (m_lengthColumn) {
        lengthWidth = std::max(lengthWidth, fontMetrics.width(i18n("Length(m)")));
    }

    if (m_skeinsColumn) {
        skeinsWidth = std::max(skeinsWidth, fontMetrics.width(i18n("Skeins (8m)")));
    }

    int spacing = fontMetrics.averageCharWidth() * 2;

    symbolWidth += spacing;
    flossNameWidth += spacing;
    strandsWidth += spacing;
    flossDescriptionWidth += spacing;
    stitchesWidth += spacing;
    lengthWidth += spacing;
    skeinsWidth += spacing;

    painter->setFont(font);

    painter->drawText(deviceTextArea.topLeft() + QPoint(0, y), QString("%1 Flosses").arg(scheme->schemeName()));
    y += (2 * lineSpacing);

    if (m_symbolColumn) {
        painter->drawText(deviceTextArea.topLeft() + QPointF(0, y), i18n("Symbol"));
    }

    if (m_flossNameColumn) {
        painter->drawText(deviceTextArea.topLeft() + QPointF(symbolWidth, y), i18nc("The name of the floss", "Name"));
    }

    if (m_strandsColumn) {
        painter->drawText(deviceTextArea.topLeft() + QPointF(symbolWidth + flossNameWidth, y), i18n("Strands"));
    }

    if (m_flossDescriptionColumn) {
        painter->drawText(deviceTextArea.topLeft() + QPointF(symbolWidth + flossNameWidth + strandsWidth, y), i18n("Description"));
    }

    if (m_stitchesColumn) {
        painter->drawText(deviceTextArea.topLeft() + QPointF(symbolWidth + flossNameWidth + strandsWidth + flossDescriptionWidth, y), i18n("Stitches"));
    }

    if (m_lengthColumn) {
        painter->drawText(deviceTextArea.topLeft() + QPointF(symbolWidth + flossNameWidth + strandsWidth + flossDescriptionWidth + stitchesWidth, y), i18n("Length(m)"));
    }

    if (m_skeinsColumn) {
        painter->drawText(deviceTextArea.topLeft() + QPointF(symbolWidth + flossNameWidth + strandsWidth + flossDescriptionWidth + stitchesWidth + lengthWidth, y), i18n("Skeins (8m)"));
    }

    y += (1.5 * lineSpacing);

    font.setBold(false);
    painter->setFont(font);

    sortedFlossesIterator.toFront();
    int currentSortedFloss = 0;

    while (sortedFlossesIterator.hasNext()) {
        int index = sortedFlossesIterator.next();
        if (currentSortedFloss >= m_indexStart && (m_indexCount == -1 || currentSortedFloss < m_indexStart + m_indexCount)) {
            FlossUsage usage = flossUsage[index];

            if (m_symbolColumn) {
                Symbol symbol = SymbolManager::library("kxstitch")->symbol(flosses[index]->stitchSymbol());
                QPixmap symbolPixmap(lineSpacing-2, lineSpacing-2);
                symbolPixmap.fill(Qt::white);

                QPainter symbolPainter(&symbolPixmap);
                symbolPainter.setRenderHint(QPainter::Antialiasing, true);
                symbolPainter.setWindow(0, 0, 1, 1);
                symbolPainter.drawRect(0, 0, 1, 1);

                QBrush brush(symbol.filled() ? Qt::SolidPattern : Qt::NoBrush);
                QPen pen(Qt::black);

                if (!symbol.filled()) {
                    pen.setWidthF(symbol.lineWidth());
                    pen.setCapStyle(symbol.capStyle());
                    pen.setJoinStyle(symbol.joinStyle());
                }

                symbolPainter.setBrush(brush);
                symbolPainter.setPen(pen);
                symbolPainter.drawPath(symbol.path());
                symbolPainter.end();

                painter->drawPixmap(deviceTextArea.topLeft() + QPointF(symbolWidth / 3, y - (lineSpacing - 2 - ((lineSpacing - ascent) / 2))), symbolPixmap);
            }

            if (m_flossNameColumn) {
                painter->drawText(deviceTextArea.topLeft() + QPointF(symbolWidth, y), flosses[index]->flossName());
            }

            if (m_strandsColumn) {
                painter->drawText(deviceTextArea.topLeft() + QPointF(symbolWidth + flossNameWidth, y), QString("%1 / %2").arg(flosses[index]->stitchStrands()).arg(flosses[index]->backstitchStrands()));
            }

            if (m_flossDescriptionColumn) {
                painter->drawText(deviceTextArea.topLeft() + QPointF(symbolWidth + flossNameWidth + strandsWidth, y), scheme->find(flosses[index]->flossName())->description());
            }

            if (m_stitchesColumn) {
                painter->drawText(deviceTextArea.topLeft() + QPointF(symbolWidth + flossNameWidth + strandsWidth + flossDescriptionWidth, y), QString("%1").arg(usage.totalStitches()));
            }

            double totalLength = usage.stitchLength() * unitLength * flosses[index]->stitchStrands() + usage.backstitchLength * unitLength * flosses[index]->backstitchStrands();

            if (m_lengthColumn) {
                painter->drawText(deviceTextArea.topLeft() + QPointF(symbolWidth + flossNameWidth + strandsWidth + flossDescriptionWidth + stitchesWidth, y), QString("%1").arg(round_n(totalLength, 2)));
            }

            if (m_skeinsColumn) {
                painter->drawText(deviceTextArea.topLeft() + QPointF(symbolWidth + flossNameWidth + strandsWidth + flossDescriptionWidth + stitchesWidth + lengthWidth, y), QString("%1 (%2)").arg(ceil(totalLength / 48)).arg(round_n(totalLength / 48, 2)));    // total length / 48m (6 strands of 8m)
            }

            y += lineSpacing;
        }

        ++currentSortedFloss;
    }

    painter->restore();
}


QDataStream &KeyElement::streamOut(QDataStream &stream) const
{
    Element::streamOut(stream);

    stream << qint32(version);

    stream  << qint32(m_showBorder)
            << m_borderColor
            << qint32(m_borderThickness)
            << qint32(m_fillBackground)
            << m_backgroundColor
            << qint32(m_backgroundTransparency)
            << qint32(m_margins.left())
            << qint32(m_margins.top())
            << qint32(m_margins.right())
            << qint32(m_margins.bottom())
            << m_textColor
            << m_textFont
            << qint32(m_indexStart)
            << qint32(m_indexCount)
            << qint32(m_symbolColumn)
            << qint32(m_flossNameColumn)
            << qint32(m_strandsColumn)
            << qint32(m_flossDescriptionColumn)
            << qint32(m_stitchesColumn)
            << qint32(m_lengthColumn)
            << qint32(m_skeinsColumn);

    return stream;
}


QDataStream &KeyElement::streamIn(QDataStream &stream)
{
    Element::streamIn(stream);

    qint32 version;
    qint32 showBorder;
    qint32 borderThickness;
    qint32 fillBackground;
    qint32 backgroundTransparency;
    qint32 left;
    qint32 top;
    qint32 right;
    qint32 bottom;
    qint32 indexStart;
    qint32 indexCount;
    qint32 symbolColumn;
    qint32 flossNameColumn;
    qint32 strandsColumn;
    qint32 flossDescriptionColumn;
    qint32 stitchesColumn;
    qint32 stitchBreakdownColumn;
    qint32 lengthColumn;
    qint32 skeinsColumn;
    qint32 totalStitchesColumn;

    stream >> version;

    switch (version) {
    case 102:
        stream  >> showBorder
                >> m_borderColor
                >> borderThickness
                >> fillBackground
                >> m_backgroundColor
                >> backgroundTransparency
                >> left
                >> top
                >> right
                >> bottom
                >> m_textColor
                >> m_textFont
                >> indexStart
                >> indexCount
                >> symbolColumn
                >> flossNameColumn
                >> strandsColumn
                >> flossDescriptionColumn
                >> stitchesColumn
                >> lengthColumn
                >> skeinsColumn;
        m_showBorder = (bool)showBorder;
        m_borderThickness = borderThickness;
        m_fillBackground = (bool)fillBackground;
        m_backgroundTransparency = backgroundTransparency;
        m_margins = QMargins(left, top, right, bottom);
        m_indexStart = indexStart;
        m_indexCount = indexCount;
        m_symbolColumn = bool(symbolColumn);
        m_flossNameColumn = bool(flossNameColumn);
        m_strandsColumn = bool(strandsColumn);
        m_flossDescriptionColumn = bool(flossDescriptionColumn);
        m_stitchesColumn = bool(stitchesColumn);
        m_lengthColumn = bool(lengthColumn);
        m_skeinsColumn = bool(skeinsColumn);
        break;

    case 101:
        stream  >> showBorder
                >> m_borderColor
                >> borderThickness
                >> fillBackground
                >> m_backgroundColor
                >> backgroundTransparency
                >> left
                >> top
                >> right
                >> bottom
                >> m_textColor
                >> m_textFont
                >> symbolColumn
                >> flossNameColumn
                >> strandsColumn
                >> flossDescriptionColumn
                >> stitchesColumn
                >> lengthColumn
                >> skeinsColumn;
        m_showBorder = (bool)showBorder;
        m_borderThickness = borderThickness;
        m_fillBackground = (bool)fillBackground;
        m_backgroundTransparency = backgroundTransparency;
        m_margins = QMargins(left, top, right, bottom);
        m_indexStart = 0;
        m_indexCount = -1;
        m_symbolColumn = bool(symbolColumn);
        m_flossNameColumn = bool(flossNameColumn);
        m_strandsColumn = bool(strandsColumn);
        m_flossDescriptionColumn = bool(flossDescriptionColumn);
        m_stitchesColumn = bool(stitchesColumn);
        m_lengthColumn = bool(lengthColumn);
        m_skeinsColumn = bool(skeinsColumn);
        break;

    case 100:
        stream  >> showBorder
                >> m_borderColor
                >> borderThickness
                >> fillBackground
                >> m_backgroundColor
                >> backgroundTransparency
                >> left
                >> top
                >> right
                >> bottom
                >> m_textColor
                >> m_textFont
                >> symbolColumn
                >> flossNameColumn
                >> strandsColumn
                >> flossDescriptionColumn
                >> stitchesColumn
                >> stitchBreakdownColumn
                >> lengthColumn
                >> skeinsColumn
                >> totalStitchesColumn;
        m_showBorder = (bool)showBorder;
        m_borderThickness = borderThickness;
        m_fillBackground = (bool)fillBackground;
        m_backgroundTransparency = backgroundTransparency;
        m_margins = QMargins(left, top, right, bottom);
        m_indexStart = 0;
        m_indexCount = -1;
        m_symbolColumn = bool(symbolColumn);
        m_flossNameColumn = bool(flossNameColumn);
        m_strandsColumn = bool(strandsColumn);
        m_flossDescriptionColumn = bool(flossDescriptionColumn);
        m_stitchesColumn = bool(stitchesColumn);
        m_lengthColumn = bool(lengthColumn);
        m_skeinsColumn = bool(skeinsColumn);
        break;

    default:
        // not supported
        // throw exception
        break;
    }

    return stream;
}


PlanElement::PlanElement(Page *parent, const QRect &rectangle, Element::Type type)
    :   Element(parent, rectangle, type)
{
}


PlanElement::PlanElement(const PlanElement &other)
    :   Element(other),
        m_patternRect(other.m_patternRect)
{
}


PlanElement::~PlanElement()
{
}


void PlanElement::setPatternRect(const QRect &rect)
{
    m_patternRect = rect;
}


PlanElement *PlanElement::clone() const
{
    return new PlanElement(*this);
}


void PlanElement::render(Document *document, QPainter *painter) const
{
    painter->save();

    painter->setViewport(painter->combinedTransform().mapRect(m_rectangle));
    painter->setWindow(0, 0, m_rectangle.width(), m_rectangle.height());

    painter->setPen(Qt::black);

    int documentWidth = document->pattern()->stitches().width();
    int documentHeight = document->pattern()->stitches().height();
    double aspect = document->property("horizontalClothCount").toDouble() / document->property("verticalClothCount").toDouble();
    double mapWidth = m_rectangle.width() - 1;
    double cellWidth = mapWidth / documentWidth;
    double cellHeight = cellWidth * aspect;
    double patternHeight = cellHeight * documentHeight;
    double mapHeight = patternHeight * aspect;

    if (mapHeight > m_rectangle.height() - 1) {
        mapHeight = m_rectangle.height() - 1;
        mapWidth = ((mapHeight / documentHeight) / aspect) * documentWidth;
    }

    cellWidth = mapWidth / documentWidth;
    cellHeight = mapHeight / documentHeight;

    double hOffset = ((m_rectangle.width() - mapWidth - 1) / 2);
    double vOffset = ((m_rectangle.height() - mapHeight - 1) / 2);

    QRectF page(hOffset, vOffset, mapWidth, mapHeight);
    QRectF pattern(m_patternRect.left() * cellWidth, m_patternRect.top() * cellHeight, m_patternRect.width() * cellWidth, m_patternRect.height() * cellHeight);

    painter->setPen(Qt::black);
    painter->setBrush(Qt::darkGray);
    painter->drawRect(page.translated(0.5, 0.5)); // drop shadow
    painter->setBrush(Qt::white);
    painter->drawRect(page);
    painter->setBrush(Qt::lightGray);
    painter->drawRect(pattern.translated(hOffset, vOffset));

    painter->restore();
}


QDataStream &PlanElement::streamOut(QDataStream &stream) const
{
    Element::streamOut(stream);

    // no need to stream the m_patternRect variable as it will be set by the PatternElement class when read

    return stream;
}


QDataStream &PlanElement::streamIn(QDataStream &stream)
{
    Element::streamIn(stream);

    return stream;
}


PatternElement::PatternElement(Page *parent, const QRect &rectangle, Element::Type type)
    :   Element(parent, rectangle, type),
        m_showScales(false),
        m_showPlan(false),
        m_renderStitchesAs(Configuration::EnumRenderer_RenderStitchesAs::BlackWhiteSymbols),
        m_renderBackstitchesAs(Configuration::EnumRenderer_RenderBackstitchesAs::BlackWhiteSymbols),
        m_renderKnotsAs(Configuration::EnumRenderer_RenderKnotsAs::BlackWhiteSymbols),
        m_showGrid(true),
        m_showStitches(true),
        m_showBackstitches(true),
        m_showKnots(true),
        m_planElement(0)
{
    m_renderer = new Renderer();
    m_renderer->setRenderStitchesAs(m_renderStitchesAs);
    m_renderer->setRenderBackstitchesAs(m_renderBackstitchesAs);
    m_renderer->setRenderKnotsAs(m_renderKnotsAs);
}


PatternElement::PatternElement(const PatternElement &other)
    :   Element(other),
        m_patternRect(other.m_patternRect),
        m_showScales(other.m_showScales),
        m_showPlan(other.m_showPlan),
        m_formatScalesAs(other.m_formatScalesAs),
        m_renderStitchesAs(other.m_renderStitchesAs),
        m_renderBackstitchesAs(other.m_renderBackstitchesAs),
        m_renderKnotsAs(other.m_renderKnotsAs),
        m_showGrid(other.m_showGrid),
        m_showStitches(other.m_showStitches),
        m_showBackstitches(other.m_showBackstitches),
        m_showKnots(other.m_showKnots),
        m_renderer(new Renderer(*other.m_renderer)),
        m_planElement(0)
{
    if (other.m_planElement) {
        m_planElement = new PlanElement(*other.m_planElement);
    }
}


PatternElement::~PatternElement()
{
    delete m_renderer;
}


PatternElement *PatternElement::clone() const
{
    return new PatternElement(*this);
}


void PatternElement::render(Document *document, QPainter *painter) const
{
    painter->save();

    double deviceHRatio = double(painter->device()->width()) / double(painter->window().width());
    double deviceVRatio = double(painter->device()->height()) / double(painter->window().height());

    // calculate the aspect ratio an the size of the cells to fit within the rectangle and the overall paint area size
    double patternWidth = m_rectangle.width();
    double aspect = document->property("horizontalClothCount").toDouble() / document->property("verticalClothCount").toDouble();
    double cellWidth = patternWidth / m_patternRect.width();
    double cellHeight = cellWidth * aspect;
    double patternHeight = cellHeight * m_patternRect.height();

    if (patternHeight > m_rectangle.height()) {
        patternHeight = m_rectangle.height();
        patternWidth = ((patternHeight / m_patternRect.height()) / aspect) * m_patternRect.width();
    }

    // find the position of the top left coordinate of the top left cell of the cells to be printed
    double patternHOffset = ((double(m_rectangle.width()) - double(patternWidth)) / 2);
    double patternVOffset = ((double(m_rectangle.height()) - double(patternHeight)) / 2);

    int vpl = int(deviceHRatio * (patternHOffset + m_rectangle.left()));
    int vpt = int(deviceVRatio * (patternVOffset + m_rectangle.top()));
    int vpw = int(patternWidth * deviceHRatio);
    int vph = int(patternHeight * deviceVRatio);

    painter->setViewport(vpl, vpt, vpw, vph);
    painter->setWindow(m_patternRect);
    painter->setClipRect(m_patternRect);

    m_renderer->render(painter,
                       document->pattern(),
                       QRect(0, 0, patternWidth, patternHeight),
                       m_showGrid,
                       m_showStitches,
                       m_showBackstitches,
                       m_showKnots,
                       -1);

    painter->restore();
}


QRect PatternElement::patternRect() const
{
    return m_patternRect;
}


bool PatternElement::showScales() const
{
    return m_showScales;
}


bool PatternElement::showPlan() const
{
    return m_showPlan;
}


Element *PatternElement::planElement() const
{
    return m_planElement;
}


Configuration::EnumRenderer_RenderStitchesAs::type PatternElement::renderStitchesAs() const
{
    return m_renderStitchesAs;
}


Configuration::EnumRenderer_RenderBackstitchesAs::type PatternElement::renderBackstitchesAs() const
{
    return m_renderBackstitchesAs;
}


Configuration::EnumRenderer_RenderKnotsAs::type PatternElement::renderKnotsAs() const
{
    return m_renderKnotsAs;
}


bool PatternElement::showGrid() const
{
    return m_showGrid;
}


bool PatternElement::showStitches() const
{
    return m_showStitches;
}


bool PatternElement::showBackstitches() const
{
    return m_showBackstitches;
}


bool PatternElement::showKnots() const
{
    return m_showKnots;
}


void PatternElement::setPatternRect(const QRect &patternRect)
{
    m_patternRect = patternRect;
}


void PatternElement::setShowScales(bool showScales)
{
    m_showScales = showScales;
}


void PatternElement::setShowPlan(bool showPlan)
{
    m_showPlan = showPlan;
}


void PatternElement::setRenderStitchesAs(Configuration::EnumRenderer_RenderStitchesAs::type renderStitchesAs)
{
    m_renderStitchesAs = renderStitchesAs;
    m_renderer->setRenderStitchesAs(renderStitchesAs);
}


void PatternElement::setRenderBackstitchesAs(Configuration::EnumRenderer_RenderBackstitchesAs::type renderBackstitchesAs)
{
    m_renderBackstitchesAs = renderBackstitchesAs;
    m_renderer->setRenderBackstitchesAs(renderBackstitchesAs);
}


void PatternElement::setRenderKnotsAs(Configuration::EnumRenderer_RenderKnotsAs::type renderKnotsAs)
{
    m_renderKnotsAs = renderKnotsAs;
    m_renderer->setRenderKnotsAs(renderKnotsAs);
}


void PatternElement::setShowGrid(bool showGrid)
{
    m_showGrid = showGrid;
}


void PatternElement::setShowStitches(bool showStitches)
{
    m_showStitches = showStitches;
}


void PatternElement::setShowBackstitches(bool showBackstitches)
{
    m_showBackstitches = showBackstitches;
}


void PatternElement::setShowKnots(bool showKnots)
{
    m_showKnots = showKnots;
}


QDataStream &PatternElement::streamOut(QDataStream &stream) const
{
    Element::streamOut(stream);

    stream << qint32(version);

    stream  << m_patternRect
            << qint32(m_showScales)
            << qint32(m_showPlan)
            << qint32(m_formatScalesAs)
            << qint32(m_renderStitchesAs)
            << qint32(m_renderBackstitchesAs)
            << qint32(m_renderKnotsAs)
            << qint32(m_showGrid)
            << qint32(m_showStitches)
            << qint32(m_showBackstitches)
            << qint32(m_showKnots)
            << qint32((m_planElement == 0) ? false : true);

    if (m_planElement) {
        stream << *m_planElement;
    }

    return stream;
}


QDataStream &PatternElement::streamIn(QDataStream &stream)
{
    Element::streamIn(stream);

    qint32 version;
    qint32 showScales;
    qint32 showPlan;
    qint32 formatScalesAs;
    qint32 renderStitchesAs;
    qint32 renderBackstitchesAs;
    qint32 renderKnotsAs;
    qint32 showGrid;
    qint32 showStitches;
    qint32 showBackstitches;
    qint32 showKnots;
    qint32 planElement;
    QList<int> visibleLayers;
    QList<int> layerOrder;

    stream >> version;

    switch (version) {
    case 101:
        stream  >> m_patternRect
                >> showScales
                >> showPlan
                >> formatScalesAs
                >> renderStitchesAs
                >> renderBackstitchesAs
                >> renderKnotsAs
                >> showGrid
                >> showStitches
                >> showBackstitches
                >> showKnots
                >> planElement;

        m_showScales = bool(showScales);
        m_showPlan = bool(showPlan);
        m_formatScalesAs = static_cast<Configuration::EnumEditor_FormatScalesAs::type>(formatScalesAs);
        m_renderStitchesAs = static_cast<Configuration::EnumRenderer_RenderStitchesAs::type>(renderStitchesAs);
        m_renderBackstitchesAs = static_cast<Configuration::EnumRenderer_RenderBackstitchesAs::type>(renderBackstitchesAs);
        m_renderKnotsAs = static_cast<Configuration::EnumRenderer_RenderKnotsAs::type>(renderKnotsAs);
        m_showGrid = bool(showGrid);
        m_showStitches = bool(showStitches);
        m_showBackstitches = bool(showBackstitches);
        m_showKnots = bool(showKnots);

        if (m_showPlan) {
            m_planElement = new PlanElement(parent(), QRect());
            stream >> *m_planElement;
            m_planElement->setPatternRect(m_patternRect);
        } else {
            m_planElement = 0;
        }

        m_renderer->setRenderStitchesAs(m_renderStitchesAs);
        m_renderer->setRenderBackstitchesAs(m_renderBackstitchesAs);
        m_renderer->setRenderKnotsAs(m_renderKnotsAs);
        break;

    case 100:
        stream  >> m_patternRect
                >> showScales
                >> showPlan
                >> formatScalesAs
                >> renderStitchesAs
                >> renderBackstitchesAs
                >> renderKnotsAs
                >> visibleLayers
                >> layerOrder
                >> showGrid
                >> showStitches
                >> showBackstitches
                >> showKnots
                >> planElement;

        m_showScales = bool(showScales);
        m_showPlan = bool(showPlan);
        m_formatScalesAs = static_cast<Configuration::EnumEditor_FormatScalesAs::type>(formatScalesAs);
        m_renderStitchesAs = static_cast<Configuration::EnumRenderer_RenderStitchesAs::type>(renderStitchesAs);
        m_renderBackstitchesAs = static_cast<Configuration::EnumRenderer_RenderBackstitchesAs::type>(renderBackstitchesAs);
        m_renderKnotsAs = static_cast<Configuration::EnumRenderer_RenderKnotsAs::type>(renderKnotsAs);
        m_showGrid = bool(showGrid);
        m_showStitches = bool(showStitches);
        m_showBackstitches = bool(showBackstitches);
        m_showKnots = bool(showKnots);

        if (bool(planElement)) {
            m_planElement = new PlanElement(parent(), QRect());
            stream >> *m_planElement;
            m_planElement->setPatternRect(m_patternRect);
        } else {
            m_planElement = 0;
        }

        m_renderer->setRenderStitchesAs(m_renderStitchesAs);
        m_renderer->setRenderBackstitchesAs(m_renderBackstitchesAs);
        m_renderer->setRenderKnotsAs(m_renderKnotsAs);
        break;

    default:
        // not supported
        // throw exception
        break;
    }

    return stream;
}


ImageElement::ImageElement(Page *parent, const QRect &rectangle, Element::Type type)
    :   PatternElement(parent, rectangle, type)
{
    setShowScales(false);
    setShowPlan(false);
    setRenderStitchesAs(Configuration::EnumRenderer_RenderStitchesAs::ColorBlocks);
    setRenderBackstitchesAs(Configuration::EnumRenderer_RenderBackstitchesAs::ColorLines);
    setRenderKnotsAs(Configuration::EnumRenderer_RenderKnotsAs::ColorBlocks);
    setShowGrid(false);
    setShowStitches(true);
    setShowBackstitches(true);
    setShowKnots(true);
}


ImageElement::ImageElement(const ImageElement &other)
    :   PatternElement(other)
{
}


ImageElement::~ImageElement()
{
}


ImageElement *ImageElement::clone() const
{
    return new ImageElement(*this);
}


void ImageElement::render(Document *document, QPainter *painter) const
{
    PatternElement::render(document, painter);
}


QDataStream &ImageElement::streamOut(QDataStream &stream) const
{
    PatternElement::streamOut(stream);

    stream << qint32(version);  // stream the version in case of future expansion
    // All other variables held in the base class

    return stream;
}


QDataStream &ImageElement::streamIn(QDataStream &stream)
{
    PatternElement::streamIn(stream);

    qint32 version;

    stream >> version;

    switch (version) {
    case 100:
        // nothing to stream in for this version
        break;

    default:
        // not supported
        // throw exception
        break;
    }

    return stream;
}


TextElement::TextElement(Page *parent, const QRect &rectangle, Element::Type type)
    :   Element(parent, rectangle, type),
        m_showBorder(Configuration::textElement_ShowBorder()),
        m_borderColor(Configuration::textElement_BorderColor()),
        m_borderThickness(Configuration::textElement_BorderThickness()),
        m_fillBackground(Configuration::textElement_FillBackground()),
        m_backgroundColor(Configuration::textElement_BackgroundColor()),
        m_backgroundTransparency(Configuration::textElement_BackgroundTransparency()),
        m_margins(QMargins(Configuration::textElement_MarginTop(), Configuration::textElement_MarginLeft(), Configuration::textElement_MarginRight(), Configuration::textElement_MarginBottom())),
        m_textColor(Configuration::textElement_TextColor())
{
}


TextElement::TextElement(const TextElement &other)
    :   Element(other),
        m_showBorder(other.m_showBorder),
        m_borderColor(other.m_borderColor),
        m_borderThickness(other.m_borderThickness),
        m_fillBackground(other.m_fillBackground),
        m_backgroundColor(other.m_backgroundColor),
        m_backgroundTransparency(other.m_backgroundTransparency),
        m_margins(other.m_margins),
        m_textFont(other.m_textFont),
        m_textColor(other.m_textColor),
        m_alignment(other.m_alignment),
        m_text(other.m_text)
{
}


TextElement::~TextElement()
{
}


TextElement *TextElement::clone() const
{
    return new TextElement(*this);
}


void TextElement::render(Document *document, QPainter *painter) const
{
    painter->save();

    double deviceVRatio = double(painter->device()->height()) / double(painter->window().height());

    // set the viewport to be the rectangle converted to device coordinates
    painter->setViewport(painter->combinedTransform().mapRect(m_rectangle));
    // set the window to be the size of the rectangle in mm which the viewport will be mapped to.
    painter->setWindow(0, 0, m_rectangle.width(), m_rectangle.height());

    QPen pen(m_borderColor);
    pen.setWidthF(double(m_borderThickness) / 10.0);

    if (m_showBorder) {
        painter->setPen(pen);
    } else {
        painter->setPen(Qt::NoPen);
    }

    QColor backgroundColor = m_backgroundColor;
    backgroundColor.setAlpha(m_backgroundTransparency);

    if (m_fillBackground) {
        painter->setBrush(backgroundColor);
    }

    painter->drawRect(painter->window());

    QFont font = m_textFont;
    font.setPixelSize(int(((font.pointSizeF() / 72.0) * 25.4) * deviceVRatio));

    QRect deviceTextArea = painter->combinedTransform().mapRect(QRect(0, 0, m_rectangle.width(), m_rectangle.height()).adjusted(m_margins.left(), m_margins.top(), -m_margins.right(), -m_margins.bottom()));

    painter->resetTransform();
    painter->setClipRect(deviceTextArea);

    pen.setColor(m_textColor);
    painter->setPen(pen);

    painter->setFont(font);
    painter->drawText(deviceTextArea, m_alignment | Qt::TextWordWrap, convertedText(document));

    painter->restore();
}


QString TextElement::convertedText(Document *document) const
{
    QString replacement = m_text;
    replacement.replace(QRegExp("\\$\\{title\\}"), document->property("title").toString());
    replacement.replace(QRegExp("\\$\\{author\\}"), document->property("author").toString());
    replacement.replace(QRegExp("\\$\\{copyright\\}"), document->property("copyright").toString());
    replacement.replace(QRegExp("\\$\\{fabric\\}"), document->property("fabric").toString());
    replacement.replace(QRegExp("\\$\\{instructions\\}"), document->property("instructions").toString());
    replacement.replace(QRegExp("\\$\\{horizontalClothCount\\}"), document->property("horizontalClothCount").toString());
    replacement.replace(QRegExp("\\$\\{verticalClothCount\\}"), document->property("verticalClothCount").toString());
    replacement.replace(QRegExp("\\$\\{width.stitches\\}"), QString("%1").arg(document->pattern()->stitches().width()));
    replacement.replace(QRegExp("\\$\\{height.stitches\\}"), QString("%1").arg(document->pattern()->stitches().height()));
    replacement.replace(QRegExp("\\$\\{width.inches\\}"), QString("%1").arg(round_n(document->pattern()->stitches().width() /
                        (document->property("horizontalClothCount").toDouble() *
                         ((static_cast<Configuration::EnumEditor_ClothCountUnits::type>(document->property("clothCountUnits").toInt()) == Configuration::EnumEditor_ClothCountUnits::CM) ? 2.54 : 1)), 2)));
    replacement.replace(QRegExp("\\$\\{height.inches\\}"), QString("%1").arg(round_n(document->pattern()->stitches().height() /
                        (document->property("verticalClothCount").toDouble() *
                         ((static_cast<Configuration::EnumEditor_ClothCountUnits::type>(document->property("clothCountUnits").toInt()) == Configuration::EnumEditor_ClothCountUnits::CM) ? 2.54 : 1)), 2)));
    replacement.replace(QRegExp("\\$\\{width.cm\\}"), QString("%1").arg(round_n(document->pattern()->stitches().width() /
                        (document->property("horizontalClothCount").toDouble() /
                         ((static_cast<Configuration::EnumEditor_ClothCountUnits::type>(document->property("clothCountUnits").toInt()) == Configuration::EnumEditor_ClothCountUnits::Inches) ? 2.54 : 1)), 2)));
    replacement.replace(QRegExp("\\$\\{height.cm\\}"), QString("%1").arg(round_n(document->pattern()->stitches().height() /
                        (document->property("verticalClothCount").toDouble() /
                         ((static_cast<Configuration::EnumEditor_ClothCountUnits::type>(document->property("clothCountUnits").toInt()) == Configuration::EnumEditor_ClothCountUnits::Inches) ? 2.54 : 1)), 2)));
    replacement.replace(QRegExp("\\$\\{scheme\\}"), document->pattern()->palette().schemeName());
    replacement.replace(QRegExp("\\$\\{page\\}"), QString("%1").arg(parent()->pageNumber()));
    // repeat for all possible values

    return replacement;
}


QDataStream &TextElement::streamOut(QDataStream &stream) const
{
    Element::streamOut(stream);

    stream << qint32(version);

    stream  << qint32(m_showBorder)
            << m_borderColor
            << qint32(m_borderThickness)
            << qint32(m_fillBackground)
            << m_backgroundColor
            << qint32(m_backgroundTransparency)
            << qint32(m_margins.left())
            << qint32(m_margins.top())
            << qint32(m_margins.right())
            << qint32(m_margins.bottom())
            << m_textFont
            << m_textColor
            << qint32(m_alignment)
            << m_text;

    return stream;
}


QDataStream &TextElement::streamIn(QDataStream &stream)
{
    Element::streamIn(stream);

    qint32 version;
    qint32 showBorder;
    qint32 borderThickness;
    qint32 fillBackground;
    qint32 backgroundTransparency;
    qint32 left;
    qint32 top;
    qint32 right;
    qint32 bottom;
    qint32 alignment;

    stream >> version;

    switch (version) {
    case 100:
        stream  >> showBorder
                >> m_borderColor
                >> borderThickness
                >> fillBackground
                >> m_backgroundColor
                >> backgroundTransparency
                >> left
                >> top
                >> right
                >> bottom
                >> m_textFont
                >> m_textColor
                >> alignment
                >> m_text;
        m_showBorder = bool(showBorder);
        m_borderThickness = borderThickness;
        m_fillBackground = bool(fillBackground);
        m_backgroundTransparency = backgroundTransparency;
        m_margins = QMargins(left, top, right, bottom);
        m_alignment = Qt::Alignment(alignment);
        break;

    default:
        // not supported
        // throw exception
        break;
    }

    return stream;
}
