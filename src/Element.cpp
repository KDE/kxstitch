/*
 * Copyright (C) 2010-2015 by Stephen Allewell
 * steve.allewell@gmail.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#include "Element.h"

#include <math.h>

#include <QAbstractTextDocumentLayout>
#include <QPainter>
#include <QPaintEngine>
#include <QTextDocument>

#include <KLocalizedString>

#include "Document.h"
#include "FlossScheme.h"
#include "Page.h"
#include "SchemeManager.h"
#include "Symbol.h"
#include "SymbolLibrary.h"
#include "SymbolManager.h"


/*
 * Convenience function to round a double to a fixed number of decimal places
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
    :   m_parent(nullptr),                    // needs to be reparented by cloner
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
        m_indexCount(0),
        m_symbolColumn(Configuration::keyElement_SymbolColumn()),
        m_symbolColumnColor(Configuration::keyElement_SymbolColumnColor()),
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
        m_symbolColumnColor(other.m_symbolColumnColor),
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


bool KeyElement::showBorder() const
{
    return m_showBorder;
}


QColor KeyElement::borderColor() const
{
    return m_borderColor;
}


int KeyElement::borderThickness() const
{
    return m_borderThickness;
}


bool KeyElement::fillBackground() const
{
    return m_fillBackground;
}


QColor KeyElement::backgroundColor() const
{
    return m_backgroundColor;
}


int KeyElement::backgroundTransparency() const
{
    return m_backgroundTransparency;
}


QMargins KeyElement::margins() const
{
    return m_margins;
}


QColor KeyElement::textColor() const
{
    return m_textColor;
}


QFont KeyElement::textFont() const
{
    return m_textFont;
}


int KeyElement::indexStart() const
{
    return m_indexStart;
}


int KeyElement::indexCount() const
{
    return m_indexCount;
}


bool KeyElement::symbolColumn() const
{
    return m_symbolColumn;
}


bool KeyElement::symbolColumnColor() const
{
    return m_symbolColumnColor;
}


bool KeyElement::flossNameColumn() const
{
    return m_flossNameColumn;
}


bool KeyElement::strandsColumn() const
{
    return m_strandsColumn;
}


bool KeyElement::flossDescriptionColumn() const
{
    return m_flossDescriptionColumn;
}


bool KeyElement::stitchesColumn() const
{
    return m_stitchesColumn;
}


bool KeyElement::lengthColumn() const
{
    return m_lengthColumn;
}


bool KeyElement::skeinsColumn() const
{
    return m_skeinsColumn;
}


void KeyElement::setShowBorder(bool showBorder)
{
    m_showBorder = showBorder;
}


void KeyElement::setBorderColor(const QColor &borderColor)
{
    m_borderColor = borderColor;
}


void KeyElement::setBorderThickness(int borderThickness)
{
    m_borderThickness = borderThickness;
}


void KeyElement::setFillBackground(bool fillBackground)
{
    m_fillBackground = fillBackground;
}



void KeyElement::setBackgroundColor(const QColor &backgroundColor)
{
    m_backgroundColor = backgroundColor;
}


void KeyElement::setBackgroundTransparency(int backgroundTransparency)
{
    m_backgroundTransparency = backgroundTransparency;
}


void KeyElement::setMargins(const QMargins &margins)
{
    m_margins = margins;
}


void KeyElement::setTextColor(const QColor &textColor)
{
    m_textColor = textColor;
}


void KeyElement::setTextFont(const QFont &textFont)
{
    m_textFont = textFont;
}


void KeyElement::setIndexStart(int indexStart)
{
    m_indexStart = indexStart;
}


void KeyElement::setIndexCount(int indexCount)
{
    m_indexCount = indexCount;
}


void KeyElement::setSymbolColumn(bool symbolColumn)
{
    m_symbolColumn = symbolColumn;
}


void KeyElement::setSymbolColumnColor(bool symbolColumnColor)
{
    m_symbolColumnColor = symbolColumnColor;
}


void KeyElement::setFlossNameColumn(bool flossNameColumn)
{
    m_flossNameColumn = flossNameColumn;
}


void KeyElement::setStrandsColumn(bool strandsColumn)
{
    m_strandsColumn = strandsColumn;
}


void KeyElement::setFlossDescriptionColumn(bool flossDescriptionColumn)
{
    m_flossDescriptionColumn = flossDescriptionColumn;
}


void KeyElement::setStitchesColumn(bool stitchesColumn)
{
    m_stitchesColumn = stitchesColumn;
}


void KeyElement::setLengthColumn(bool lengthColumn)
{
    m_lengthColumn = lengthColumn;
}


void KeyElement::setSkeinsColumn(bool skeinsColumn)
{
    m_skeinsColumn = skeinsColumn;
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

    QPen pen(Qt::NoPen);

    // if a border is valid, draw it, otherwise if drawing on screen draw a faint border to indicate the
    // extents of the element.
    if (m_showBorder) {
        pen = QPen(m_borderColor);
        pen.setWidthF(double(m_borderThickness) / 10.0);
    } else if (painter->device()->paintEngine() == nullptr) {
        // TODO This is a hack to avoid a crash in QWidget::paintEngine returning a null pointer
        // There should be a better way to do this.
        pen = QPen(Qt::lightGray);
        pen.setCosmetic(true);
    }

    painter->setPen(pen);

    QColor backgroundColor = m_backgroundColor;
    backgroundColor.setAlpha(m_backgroundTransparency);

    if (m_fillBackground) {
        painter->setBrush(backgroundColor);
    } else {
        painter->setBrush(Qt::NoBrush);
    }

    painter->drawRect(painter->window());

    // Calculate field widths
    QFont font = m_textFont;
    font.setPixelSize(int(((font.pointSizeF() / 72.0) * 25.4) * deviceVRatio));

    QRect deviceTextArea = painter->combinedTransform().mapRect(QRect(0, 0, m_rectangle.width(), m_rectangle.height()).adjusted(m_margins.left(), m_margins.top(), -m_margins.left(), -m_margins.bottom()));

    painter->resetTransform();
    painter->setClipRect(deviceTextArea);

    pen = QPen(m_textColor);
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

        flossNameWidth = std::max(flossNameWidth, fontMetrics.width(flosses[index]->flossName()));
        strandsWidth = std::max(strandsWidth, fontMetrics.width(QString("%1 / %2").arg(flosses[index]->stitchStrands()).arg(flosses[index]->backstitchStrands())));
        flossDescriptionWidth = std::max(flossDescriptionWidth, fontMetrics.width(scheme->find(flosses[index]->flossName())->description()));
        stitchesWidth = std::max(stitchesWidth, fontMetrics.width(QString("%1").arg(usage.totalStitches())));
        double flossLength = round_n(usage.stitchLength() * unitLength * flosses[index]->stitchStrands() + usage.backstitchLength * unitLength * flosses[index]->backstitchStrands(), 2);
        lengthWidth = std::max(lengthWidth, fontMetrics.width(QString("%1").arg(flossLength)));
        skeinsWidth = std::max(skeinsWidth, fontMetrics.width(QString("%1").arg(flossLength / 48)));    // 1 skein = 6 strands of 8m
    }

    font.setBold(true);
    fontMetrics = QFontMetrics(font, painter->device());
    int spacing = fontMetrics.averageCharWidth() * 2;

    symbolWidth = std::max(symbolWidth, fontMetrics.width(i18n("Symbol")));
    flossNameWidth = std::max(flossNameWidth, fontMetrics.width(i18nc("The name of the floss", "Name")));
    strandsWidth = std::max(strandsWidth, fontMetrics.width(i18n("Strands")));
    flossDescriptionWidth = std::max(flossDescriptionWidth, fontMetrics.width(i18n("Description")));
    stitchesWidth = std::max(stitchesWidth, fontMetrics.width(i18n("Stitches")));
    lengthWidth = std::max(lengthWidth, fontMetrics.width(i18n("Length(m)")));
    skeinsWidth = std::max(skeinsWidth, fontMetrics.width(i18n("Skeins (8m)")));

    // if a column is not being displayed zero the width, otherwise add the spacing value
    symbolWidth = (m_symbolColumn) ? symbolWidth + spacing : 0;
    flossNameWidth = (m_flossNameColumn) ? flossNameWidth + spacing : 0;
    strandsWidth = (m_strandsColumn) ? strandsWidth + spacing : 0;
    flossDescriptionWidth = (m_flossDescriptionColumn) ? flossDescriptionWidth + spacing : 0;
    stitchesWidth = (m_stitchesColumn) ? stitchesWidth + spacing : 0;
    lengthWidth = (m_lengthColumn) ? lengthWidth + spacing : 0;
    skeinsWidth = (m_skeinsColumn) ? skeinsWidth + spacing : 0;

    painter->setFont(font);

    painter->drawText(deviceTextArea.topLeft() + QPoint(0, y), i18nc("%1 is a scheme name", "%1 Flosses", scheme->schemeName()));
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

        if (currentSortedFloss >= m_indexStart && (m_indexCount == 0 || currentSortedFloss < m_indexStart + m_indexCount)) {
            FlossUsage usage = flossUsage[index];

            if (m_symbolColumn) {
                Symbol symbol = SymbolManager::library(document->pattern()->palette().symbolLibrary())->symbol(flosses[index]->stitchSymbol());

                painter->setViewport(deviceTextArea.left() + symbolWidth / 3, deviceTextArea.top() + y - (lineSpacing - 2 - ((lineSpacing - ascent) / 2)), lineSpacing - 2, lineSpacing - 2);
                painter->setViewport(deviceTextArea.left(), deviceTextArea.top() + y - (lineSpacing - 2 - ((lineSpacing - ascent) / 2)), lineSpacing - 2, lineSpacing - 2);
                painter->setWindow(0, 0, 1, 1);

                QBrush brush = symbol.brush();
                QPen pen = symbol.pen();

                if (m_symbolColumnColor) {
                    brush.setColor(flosses[index]->flossColor());
                    pen.setColor(flosses[index]->flossColor());
                }

                painter->setBrush(brush);
                painter->setPen(pen);
                painter->drawPath(symbol.path());
                painter->resetTransform();
            }

            painter->setPen(Qt::black);

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
            << qint32(m_symbolColumnColor)
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
    qint32 symbolColumnColor;
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
    case 103:
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
                >> symbolColumnColor
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
        m_indexCount = (indexCount == -1) ? 0 : indexCount;
        m_symbolColumn = bool(symbolColumn);
        m_symbolColumnColor = bool(symbolColumnColor);
        m_flossNameColumn = bool(flossNameColumn);
        m_strandsColumn = bool(strandsColumn);
        m_flossDescriptionColumn = bool(flossDescriptionColumn);
        m_stitchesColumn = bool(stitchesColumn);
        m_lengthColumn = bool(lengthColumn);
        m_skeinsColumn = bool(skeinsColumn);
        break;

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
        m_indexCount = (indexCount == -1) ? 0 : indexCount;
        m_symbolColumn = bool(symbolColumn);
        m_symbolColumnColor = false;
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
        m_indexCount = 0;
        m_symbolColumn = bool(symbolColumn);
        m_symbolColumnColor = false;
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
        m_indexCount = 0;
        m_symbolColumn = bool(symbolColumn);
        m_symbolColumnColor = false;
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

    QPen pen(Qt::black);
    pen.setWidthF(0.05);

    painter->setPen(pen);
    painter->setBrush(Qt::black);
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
        m_showBorder(Configuration::patternElement_ShowBorder()),
        m_borderColor(Configuration::patternElement_BorderColor()),
        m_borderThickness(Configuration::patternElement_BorderThickness()),
        m_showScales(false),
        m_showPlan(false),
        m_formatScalesAs(Configuration::editor_FormatScalesAs()),
        m_renderStitchesAs(Configuration::renderer_RenderStitchesAs()),
        m_renderBackstitchesAs(Configuration::renderer_RenderBackstitchesAs()),
        m_renderKnotsAs(Configuration::renderer_RenderKnotsAs()),
        m_showGrid(true),
        m_showStitches(true),
        m_showBackstitches(true),
        m_showKnots(true),
        m_planElement(nullptr)
{
}


PatternElement::PatternElement(const PatternElement &other)
    :   Element(other),
        m_showBorder(other.m_showBorder),
        m_borderColor(other.m_borderColor),
        m_borderThickness(other.m_borderThickness),
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
        m_planElement(nullptr)
{
    if (other.m_planElement) {
        m_planElement = new PlanElement(*other.m_planElement);
    }
}


PatternElement::~PatternElement()
{
}


bool PatternElement::showBorder() const
{
    return m_showBorder;
}


QColor PatternElement::borderColor() const
{
    return m_borderColor;
}


int PatternElement::borderThickness() const
{
    return m_borderThickness;
}


void PatternElement::setShowBorder(bool showBorder)
{
    m_showBorder = showBorder;
}


void PatternElement::setBorderColor(const QColor &borderColor)
{
    m_borderColor = borderColor;
}


void PatternElement::setBorderThickness(int borderThickness)
{
    m_borderThickness = borderThickness;
}


PatternElement *PatternElement::clone() const
{
    return new PatternElement(*this);
}


void PatternElement::render(Document *document, QPainter *painter) const
{
    Renderer renderer;
    renderer.setRenderStitchesAs(m_renderStitchesAs);
    renderer.setRenderBackstitchesAs(m_renderBackstitchesAs);
    renderer.setRenderKnotsAs(m_renderKnotsAs);

    int scaleSize = (m_showScales) ? 6 : 0;

    painter->save();

    QPen pen;
    pen.setWidthF(0.05);
    painter->setPen(pen);
    painter->setBrush(Qt::black);

    double deviceHRatio = double(painter->device()->width()) / double(painter->window().width());
    double deviceVRatio = double(painter->device()->height()) / double(painter->window().height());

    int documentWidth = document->pattern()->stitches().width();
    int documentHeight = document->pattern()->stitches().height();

    double horizontalClothCount = document->property("horizontalClothCount").toDouble();
    double verticalClothCount = document->property("verticalClothCount").toDouble();

    bool clothCountUnitsInches = (static_cast<Configuration::EnumEditor_ClothCountUnits::type>(document->property("clothCountUnits").toInt()) == Configuration::EnumEditor_ClothCountUnits::Inches);

    // calculate the aspect ratio an the size of the cells to fit within the rectangle and the overall paint area size
    double patternWidth = m_rectangle.width() - scaleSize;
    double aspect =  horizontalClothCount / verticalClothCount;
    double cellWidth = patternWidth / m_patternRect.width();
    double cellHeight = cellWidth * aspect;
    double patternHeight = cellHeight * m_patternRect.height();

    if (patternHeight > m_rectangle.height() - scaleSize) {
        patternHeight = m_rectangle.height() - scaleSize;
        patternWidth = ((patternHeight / m_patternRect.height()) / aspect) * m_patternRect.width();
        cellWidth = patternWidth / m_patternRect.width();
        cellHeight = patternHeight / m_patternRect.height();
    }

    int cellHorizontalGrouping = document->property("cellHorizontalGrouping").toInt();
    int cellVerticalGrouping = document->property("cellVerticalGrouping").toInt();

    renderer.setCellGrouping(cellHorizontalGrouping, cellVerticalGrouping);
    renderer.setGridLineWidths(Configuration::editor_ThinLineWidth(), Configuration::editor_ThickLineWidth());
    renderer.setGridLineColors(document->property("thinLineColor").value<QColor>(), document->property("thickLineColor").value<QColor>());

    // find the position of the top left coordinate of the top left cell of the cells to be printed
    double patternHOffset = ((double(m_rectangle.width()) - double(patternWidth + scaleSize)) / 2);
    double patternVOffset = ((double(m_rectangle.height()) - double(patternHeight + scaleSize)) / 2);

    int vpLeft = int(deviceHRatio * (patternHOffset + m_rectangle.left()));
    int vpTop = int(deviceVRatio * (patternVOffset + m_rectangle.top()));
    int vpWidth = int((patternWidth + scaleSize) * deviceHRatio);
    int vpHeight = int((patternHeight + scaleSize) * deviceVRatio);

    double vpCellWidth = deviceHRatio * cellWidth;
    double vpScaleWidth = deviceHRatio * scaleSize;
    double vpScaleHeight = deviceVRatio * scaleSize;

    if (m_showScales) {
        QFont font = painter->font();
        font.setPixelSize(int(deviceVRatio * 2));
        painter->setFont(font);

        // draw horizontal ruler
        // default to Stitches values
        double subTick = 1.0;
        int minorTicks = 1;
        int majorTicks = cellHorizontalGrouping;

        int textValueIncrement = cellHorizontalGrouping;

        switch (m_formatScalesAs) {
        case Configuration::EnumEditor_FormatScalesAs::Stitches:
            // set as defaults above
            break;

        case Configuration::EnumEditor_FormatScalesAs::CM:
            // subtick should be 1/10 CM
            subTick = horizontalClothCount / (clothCountUnitsInches ? 25.4 : 10);
            minorTicks = 5;
            majorTicks = 10;
            textValueIncrement = 1;
            break;

        case Configuration::EnumEditor_FormatScalesAs::Inches:
            // subtick should be 1/16 inch
            subTick = horizontalClothCount / (clothCountUnitsInches ? 16 : 6.299);
            minorTicks = 4;
            majorTicks = 16;
            textValueIncrement = 1;
            break;

        default:
            break;
        }

        painter->setViewport(vpLeft + vpScaleWidth, vpTop, vpWidth - vpScaleWidth, vpScaleHeight);
        painter->setWindow(m_patternRect.left(), 0, m_patternRect.width(), 1);
        painter->drawLine(QLineF(m_patternRect.left(), 0.9, m_patternRect.right() + 1, 0.9));

        int ticks = int(double(documentWidth) / subTick);

        for (int i = 0 ; i <= ticks ; ++i) {
            double ticklen = 0.8;
            double tickPosition = subTick * i;

            if ((i % minorTicks) == 0) {
                ticklen = 0.7;
            }

            if ((i % majorTicks) == 0) {
                ticklen = 0.6;
            }

            if (tickPosition >= m_patternRect.left() && tickPosition <= m_patternRect.right() + 1) {
                painter->drawLine(QPointF(tickPosition, ticklen), QPointF(tickPosition, 0.9));
            }
        }

        QTransform transform = painter->combinedTransform();

        double patternHCenter = double(documentWidth) / 2;
        QPoint vpPatternHCenter = transform.map(QPointF(patternHCenter, 0.9)).toPoint();

        painter->resetTransform();

        if (patternHCenter >= m_patternRect.left() && patternHCenter <= m_patternRect.right() + 1) {
            QPolygon patternHCenterMarker;
            int markerSize = deviceHRatio * scaleSize / 3;
            patternHCenterMarker << vpPatternHCenter << vpPatternHCenter + QPoint(-markerSize / 2, -markerSize) << vpPatternHCenter + QPoint(markerSize / 2, -markerSize);
            painter->drawPolygon(patternHCenterMarker);
        }

        for (int i = 0 ; i <= ticks ; ++i) {
            int tickPosition = transform.map(QPointF(subTick * i, 0)).toPoint().x();

            if (tickPosition >= vpLeft + vpScaleWidth && tickPosition <= vpLeft + vpWidth && (i % majorTicks) == 0) {
                painter->drawText(QRect(tickPosition - vpCellWidth, vpTop, vpCellWidth * 2, int(3 * deviceVRatio)), Qt::AlignHCenter | Qt::AlignBottom, QString("%1").arg((i / majorTicks) * textValueIncrement));
            }
        }

        // draw vertical ruler
        switch (m_formatScalesAs) {
        case Configuration::EnumEditor_FormatScalesAs::Stitches:
            // subTick should be 1 cell
            subTick = 1.0;
            minorTicks = 1;
            majorTicks = cellVerticalGrouping;
            textValueIncrement = cellVerticalGrouping;
            break;

        case Configuration::EnumEditor_FormatScalesAs::CM:
            // subTick should be 1/10 CM
            subTick = verticalClothCount / (clothCountUnitsInches ? 25.4 : 10);
            minorTicks = 5;
            majorTicks = 10;
            textValueIncrement = 1;
            break;

        case Configuration::EnumEditor_FormatScalesAs::Inches:
            // subTick should be 1/16 inch
            subTick = verticalClothCount / (clothCountUnitsInches ? 16 : 6.299);
            minorTicks = 4;
            majorTicks = 16;
            textValueIncrement = 1;
            break;

        default:
            break;
        }

        painter->setViewport(vpLeft, vpTop + vpScaleHeight, vpScaleWidth, vpHeight - vpScaleHeight);
        painter->setWindow(0, m_patternRect.top(), 1, m_patternRect.height());
        painter->drawLine(QLineF(0.9, m_patternRect.top(), 0.9, m_patternRect.bottom() + 1));

        ticks = int(double(documentHeight) / subTick);

        for (int i = 0 ; i <= ticks ; ++i) {
            double ticklen = 0.8;
            double tickPosition = subTick * i;

            if ((i % minorTicks) == 0) {
                ticklen = 0.7;
            }

            if ((i % majorTicks) == 0) {
                ticklen = 0.6;
            }

            if (tickPosition >= m_patternRect.top() && tickPosition <= m_patternRect.bottom() + 1) {
                painter->drawLine(QPointF(ticklen, tickPosition), QPointF(0.9, tickPosition));
            }
        }

        transform = painter->combinedTransform();

        double patternVCenter = double(documentHeight) / 2;
        QPoint vpPatternVCenter = transform.map(QPointF(0.9, patternVCenter)).toPoint();

        painter->resetTransform();

        if (patternVCenter >= m_patternRect.top() && patternVCenter <= m_patternRect.bottom() + 1) {
            QPolygon patternVCenterMarker;
            int markerSize = deviceVRatio * scaleSize / 3;
            patternVCenterMarker << vpPatternVCenter << vpPatternVCenter + QPoint(-markerSize, -markerSize / 2) << vpPatternVCenter + QPoint(-markerSize, markerSize / 2);
            painter->drawPolygon(patternVCenterMarker);
        }

        for (int i = 0 ; i <= ticks ; ++i) {
            int tickPosition = transform.map(QPointF(0, subTick * i)).toPoint().y();

            if (tickPosition >= vpTop + vpScaleHeight && tickPosition <= vpTop + vpHeight && (i % majorTicks) == 0) {
                painter->drawText(QRect(vpLeft, tickPosition - vpScaleHeight, int(3 * deviceHRatio), vpScaleHeight * 2), Qt::AlignRight | Qt::AlignVCenter, QString("%1").arg((i / majorTicks) * textValueIncrement));
            }
        }

        painter->setViewport(vpLeft + vpScaleWidth, vpTop + vpScaleHeight, vpWidth - vpScaleWidth, vpHeight - vpScaleHeight);
        painter->setWindow(m_patternRect);
        painter->setClipRect(m_patternRect);
    } else {
        painter->setViewport(vpLeft, vpTop, vpWidth, vpHeight);
        painter->setWindow(m_patternRect);
        painter->setClipRect(m_patternRect);
    }

    renderer.render(painter,
                    document->pattern(),
                    m_patternRect,
                    m_showGrid,
                    m_showStitches,
                    m_showBackstitches,
                    m_showKnots,
                    -1);

    if (m_showBorder) {
        QPen pen(m_borderColor);
        pen.setWidthF(m_borderThickness / 10.0);
        painter->setPen(pen);
        painter->setBrush(Qt::NoBrush);
        painter->drawRect(m_patternRect);
    }

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
}


void PatternElement::setRenderBackstitchesAs(Configuration::EnumRenderer_RenderBackstitchesAs::type renderBackstitchesAs)
{
    m_renderBackstitchesAs = renderBackstitchesAs;
}


void PatternElement::setRenderKnotsAs(Configuration::EnumRenderer_RenderKnotsAs::type renderKnotsAs)
{
    m_renderKnotsAs = renderKnotsAs;
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

    stream  << qint32(m_showBorder)
            << m_borderColor
            << qint32(m_borderThickness)
            << m_patternRect
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
            << qint32((m_planElement == nullptr) ? false : true);

    if (m_planElement) {
        stream << *m_planElement;
    }

    return stream;
}


QDataStream &PatternElement::streamIn(QDataStream &stream)
{
    Element::streamIn(stream);

    qint32 version;
    qint32 showBorder;
    qint32 borderThickness;
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
    case 102:
        stream  >> showBorder
                >> m_borderColor
                >> borderThickness
                >> m_patternRect
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

        m_showBorder = bool(showBorder);
        m_borderThickness = borderThickness;
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
            m_planElement = nullptr;
        }

        break;

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
            m_planElement = nullptr;
        }

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
            m_planElement = nullptr;
        }

        break;

    default:
        // not supported
        // throw exception
        break;
    }

    /* A bug was introduced in commit d59704c30d709a23dd8f9e66fdf98e9d15702d07 causing patterns
     * created before this version to have rendering modes +1 from those currently.
     * The following code rectifies the limits. Other patterns created before this change will be
     * valid but will not be as the user originally set.
     */
    if (m_renderStitchesAs > 4) {
        m_renderStitchesAs = static_cast<Configuration::EnumRenderer_RenderStitchesAs::type>(4);
    }

    if (m_renderBackstitchesAs > 1) {
        m_renderBackstitchesAs = static_cast<Configuration::EnumRenderer_RenderBackstitchesAs::type>(1);
    }

    if (m_renderKnotsAs > 3) {
        m_renderKnotsAs = static_cast<Configuration::EnumRenderer_RenderKnotsAs::type>(3);
    }

    return stream;
}


ImageElement::ImageElement(Page *parent, const QRect &rectangle, Element::Type type)
    :   PatternElement(parent, rectangle, type)
{
    setShowBorder(Configuration::imageElement_ShowBorder());
    setBorderColor(Configuration::imageElement_BorderColor());
    setBorderThickness(Configuration::imageElement_BorderThickness());
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


bool TextElement::showBorder() const
{
    return m_showBorder;
}


QColor TextElement::borderColor() const
{
    return m_borderColor;
}


int TextElement::borderThickness() const
{
    return m_borderThickness;
}


bool TextElement::fillBackground() const
{
    return m_fillBackground;
}


QColor TextElement::backgroundColor() const
{
    return m_backgroundColor;
}


int TextElement::backgroundTransparency() const
{
    return m_backgroundTransparency;
}


QMargins TextElement::margins() const
{
    return m_margins;
}


QFont TextElement::textFont() const
{
    return m_textFont;
}


QColor TextElement::textColor() const
{
    return m_textColor;
}


Qt::Alignment TextElement::alignment() const
{
    return m_alignment;
}


QString TextElement::text() const
{
    return m_text;
}


void TextElement::setShowBorder(bool showBorder)
{
    m_showBorder = showBorder;
}


void TextElement::setBorderColor(const QColor &borderColor)
{
    m_borderColor = borderColor;
}


void TextElement::setBorderThickness(int borderThickness)
{
    m_borderThickness = borderThickness;;
}


void TextElement::setFillBackground(bool fillBackground)
{
    m_fillBackground = fillBackground;
}


void TextElement::setBackgroundColor(const QColor &backgroundColor)
{
    m_backgroundColor = backgroundColor;
}


void TextElement::setBackgroundTransparency(int backgroundTransparency)
{
    m_backgroundTransparency = backgroundTransparency;
}


void TextElement::setMargins(const QMargins &margins)
{
    m_margins = margins;
}


void TextElement::setTextFont(const QFont &textFont)
{
    m_textFont = textFont;
}


void TextElement::setTextColor(const QColor &textColor)
{
    m_textColor = textColor;
}


void TextElement::setAlignment(Qt::Alignment alignment)
{
    m_alignment = alignment;
}


void TextElement::setText(const QString &text)
{
    if (text.contains("<html>")) {
        m_text = text;
    } else {
        m_text = encodeToHtml(text);
    }
}


TextElement *TextElement::clone() const
{
    return new TextElement(*this);
}


void TextElement::render(Document *document, QPainter *painter) const
{
    painter->save();

    double pageWidthMM = painter->window().width();
    double pageHeightMM = painter->window().height();
    double deviceWidthPixels = painter->device()->width();
    double deviceHeightPixels = painter->device()->height();
    double devicePageWidthPixels = (double(pageWidthMM) / 25.4) * painter->device()->logicalDpiX();
    double devicePageHeightPixels = (double(pageHeightMM) / 25.4) * painter->device()->logicalDpiY();

    double deviceHRatio = devicePageWidthPixels / deviceWidthPixels;

    // set the viewport to be the rectangle converted to device coordinates
    painter->setViewport(painter->combinedTransform().mapRect(m_rectangle));
    // set the window to be the size of the rectangle in mm which the viewport will be mapped to.
    painter->setWindow(0, 0, m_rectangle.width(), m_rectangle.height());

    QPen pen(Qt::NoPen);

    // if a border is valid, draw it, otherwise if drawing on screen draw a faint border to indicate the
    // extents of the element.
    if (m_showBorder) {
        pen = QPen(m_borderColor);
        pen.setWidthF(double(m_borderThickness) / 10.0);
    } else if (painter->device()->paintEngine() == nullptr) {
        // TODO This is a hack to avoid a crash in QWidget::paintEngine returning a null pointer
        // There should be a better way to do this.
        pen = QPen(Qt::lightGray);
        pen.setCosmetic(true);
    }

    painter->setPen(pen);

    QColor backgroundColor = m_backgroundColor;
    backgroundColor.setAlpha(m_backgroundTransparency);

    if (m_fillBackground) {
        painter->setBrush(backgroundColor);
    } else {
        painter->setBrush(Qt::NoBrush);
    }

    painter->drawRect(painter->window());

    QRect deviceTextArea = painter->combinedTransform().mapRect(QRect(0, 0, m_rectangle.width(), m_rectangle.height()).adjusted(m_margins.left(), m_margins.top(), -m_margins.right(), -m_margins.bottom()));

    painter->resetTransform();
    painter->translate(deviceTextArea.topLeft());
    painter->scale(deviceWidthPixels / devicePageWidthPixels, deviceHeightPixels / devicePageHeightPixels);

    QTextDocument textDocument;
    textDocument.documentLayout()->setPaintDevice(painter->device());
    textDocument.setHtml(convertedText(document));
    textDocument.setTextWidth(deviceHRatio * deviceTextArea.width());
    textDocument.drawContents(painter);

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


QString TextElement::encodeToHtml(const QString &text) const
{
    QTextDocument document;
    document.setDefaultFont(m_textFont);

    QTextBlockFormat format;
    format.setAlignment(m_alignment);
    format.setForeground(QBrush(m_textColor));

    QTextCursor cursor(&document);
    cursor.movePosition(QTextCursor::Start);
    cursor.setBlockFormat(format);
    cursor.insertText(text);

    return document.toHtml();
}
