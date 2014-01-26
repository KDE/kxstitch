/*
 * Copyright (C) 2010 by Stephen Allewell
 * sallewell@users.sourceforge.net
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#include "Scale.h"

#include <QScrollBar>
#include <QPainter>

#include "configuration.h"


Scale::Scale(Qt::Orientation orientation)
    :   QWidget()
{
    setContextMenuPolicy(Qt::ActionsContextMenu);

    if (orientation == Qt::Horizontal) {
        setContentsMargins(3, 0, 0, 0);
        setMinimumHeight(Configuration::editor_HorizontalScaleHeight());
    } else {
        setContentsMargins(0, 3, 0, 0);
        setMinimumWidth(Configuration::editor_VerticalScaleWidth());
    }

    m_orientation = orientation;
    m_units = Configuration::editor_FormatScalesAs();
    m_cellSize = 0;
    m_cellCount = 0;
    m_offset = 0;
}


Scale::~Scale()
{
}


void Scale::setUnits(Configuration::EnumEditor_FormatScalesAs::type units)
{
    m_units = units;
    update();
}


void Scale::setCellSize(double cellSize)
{
    m_cellSize = cellSize;
    update();
}


void Scale::setCellGrouping(int cellGrouping)
{
    m_cellGrouping = cellGrouping;
    update();
}


void Scale::setCellCount(int cellCount)
{
    m_cellCount = cellCount;
    update();
}


void Scale::setClothCount(double clothCount)
{
    m_clothCount = clothCount;
    update();
}


void Scale::setClothCountUnits(Configuration::EnumEditor_ClothCountUnits::type clothCountUnits)
{
    m_clothCountUnits = clothCountUnits;
    update();
}


void Scale::setOffset(double offset)
{
    m_offset = offset;
    update();
}


QSize Scale::sizeHint() const
{
    if (m_orientation == Qt::Horizontal) {
        return QSize(500, 30);
    } else {
        return QSize(30, 500);
    }
}


void Scale::paintEvent(QPaintEvent*)
{
    double length = m_cellSize * m_cellCount;
    double halfLength = length / 2;
    int width = contentsRect().width();
    int height = contentsRect().height();
    int left = contentsRect().left();
    int right = contentsRect().right();
    int bottom = contentsRect().bottom();
    int top = contentsRect().top();
    bool clothCountUnitsInches = (m_clothCountUnits == Configuration::EnumEditor_ClothCountUnits::Inches);

    double subTick;
    int minorTicks;
    int majorTicks;

    int ticklen;

    int textValue = 0;
    int textValueIncrement = 0;

    switch (m_units) {
    case Configuration::EnumEditor_FormatScalesAs::Stitches:
        // subtick should be 1 cell
        subTick = m_cellSize;
        minorTicks = 1;
        majorTicks = m_cellGrouping;
        textValueIncrement = m_cellGrouping;
        break;

    case Configuration::EnumEditor_FormatScalesAs::CM:
        // subtick should be 1/10 CM
        subTick = m_cellSize * m_clothCount / (clothCountUnitsInches ? 25.4 : 10);
        minorTicks = 5;
        majorTicks = 10;
        textValueIncrement = 1;
        break;

    case Configuration::EnumEditor_FormatScalesAs::Inches:
        // subtick should be 1/16 inch
        subTick = m_cellSize * m_clothCount / (clothCountUnitsInches ? 16 : 6.299);
        majorTicks = 16;
        minorTicks = 4;
        textValueIncrement = 1;
        break;

    default:
        break;

    }

    QPainter painter;
    painter.begin(this);
    painter.setBrush(Qt::black);

    QPolygonF midPoint;

    if (m_orientation == Qt::Horizontal) {
        painter.drawLine(left, bottom, right, bottom);
        midPoint << QPointF(left + halfLength + m_offset, bottom) << QPointF(left + halfLength - 5 + m_offset, bottom - 5) << QPointF(left + halfLength + 5 + m_offset, bottom - 5);
    } else {
        painter.drawLine(right, top, right, bottom);
        midPoint << QPointF(right, top + halfLength + m_offset) << QPointF(right - 5, top + halfLength - 5 + m_offset) << QPointF(right - 5, top + halfLength + 5 + m_offset);
    }

    painter.drawPolygon(midPoint);

    int ticks = length / subTick;

    for (int i = 0 ; i <= ticks ; i++) {
        ticklen = 3;
        double tickwidth = i * subTick;

        if ((i % minorTicks) == 0) {
            ticklen = 6;
        }

        if ((i % majorTicks) == 0) {
            ticklen = 9;
        }

        if (m_orientation == Qt::Horizontal) {
            painter.drawLine(QPointF(left + tickwidth + m_offset, bottom), QPointF(left + tickwidth + m_offset, bottom - ticklen));
        } else {
            painter.drawLine(QPointF(right, top + tickwidth + m_offset), QPointF(right - ticklen, top + tickwidth + m_offset));
        }

        if ((i % majorTicks) == 0) {
            if (m_orientation == Qt::Horizontal) {
                painter.drawText(QPointF(left + tickwidth + m_offset + 1, bottom - ticklen), QString("%1").arg(textValue));
            } else {
                painter.drawText(QRectF(left, top + tickwidth + m_offset + 1, width, height), Qt::AlignTop, QString("%1").arg(textValue));
            }

            textValue += textValueIncrement;
        }
    }

    painter.end();
}
