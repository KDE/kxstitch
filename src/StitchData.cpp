/*
 * Copyright (C) 2010-2014 by Stephen Allewell
 * steve.allewell@gmail.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#include "StitchData.h"

#include <KLocale>

#include "Exceptions.h"


FlossUsage::FlossUsage()
    :   backstitchCount(0),
        backstitchLength(0.0)
{
}


double FlossUsage::totalLength() const
{
    return stitchLength() + backstitchLength;
}


double FlossUsage::stitchLength() const
{
    double total = 0;

    foreach (double length, stitchLengths) {
        total += length;
    }

    return total;
}


int FlossUsage::totalStitches() const
{
    return stitchCount() + backstitchCount;
}


int FlossUsage::stitchCount() const
{
    int total = 0;

    foreach (int count, stitchCounts) {
        total += count;
    }

    return total;
}


StitchData::StitchData()
    :   m_width(0),
        m_height(0)
{
}


StitchData::~StitchData()
{
    clear();
}


void StitchData::clear()
{
    qDeleteAll(m_stitches);
    m_stitches.fill(0);

    qDeleteAll(m_backstitches);
    m_backstitches.clear();

    qDeleteAll(m_knots);
    m_knots.clear();
}


int StitchData::width() const
{
    return m_width;
}


int StitchData::height() const
{
    return m_height;
}


void StitchData::resize(int width, int height)
{
    QVector<StitchQueue *> newVector(width * height);
    QRect extentsRect = extents();

    for (int y = extentsRect.top() ; y <= extentsRect.bottom() ; ++y) {
        for (int x = extentsRect.left() ; x <= extentsRect.right() ; ++x) {
            newVector[y * width + x] = takeStitchQueueAt(x, y);
        }
    }

    m_stitches = newVector;
    m_width = width;
    m_height = height;
}


void StitchData::insertColumns(int startColumn, int columns)
{
    int originalWidth = m_width;

    resize(originalWidth + columns, m_height);

    for (int y = 0 ; y < m_height ; ++y) {
        for (int destinationColumn = m_width - 1, sourceColumn = originalWidth - 1 ; sourceColumn >= startColumn ; --destinationColumn, --sourceColumn) {
            m_stitches[index(destinationColumn, y)] = takeStitchQueueAt(sourceColumn, y);
        }
    }

    startColumn *= 2;
    columns *= 2;

    QListIterator<Backstitch *> backstitchIterator(m_backstitches);

    while (backstitchIterator.hasNext()) {
        Backstitch *backstitch = backstitchIterator.next();

        if (backstitch->start.x() >= startColumn) {
            backstitch->start.setX(backstitch->start.x() + columns);
        }

        if (backstitch->end.x() >= startColumn) {
            backstitch->end.setX(backstitch->end.x() + columns);
        }
    }

    QListIterator<Knot *> knotIterator(m_knots);

    while (knotIterator.hasNext()) {
        Knot *knot = knotIterator.next();

        if (knot->position.x() >= startColumn) {
            knot->position.setX(knot->position.x() + columns);
        }
    }
}


void StitchData::insertRows(int startRow, int rows)
{
    int originalHeight = m_height;

    resize(m_width, originalHeight + rows);

    for (int destinationRow = m_height - 1, sourceRow = originalHeight - 1; sourceRow >= startRow ; --destinationRow, --sourceRow) {
        for (int x = 0 ; x < m_width ; ++x) {
            m_stitches[index(x, destinationRow)] = takeStitchQueueAt(x, sourceRow);
        }
    }

    startRow *= 2;
    rows *= 2;

    QListIterator<Backstitch *> backstitchIterator(m_backstitches);

    while (backstitchIterator.hasNext()) {
        Backstitch *backstitch = backstitchIterator.next();

        if (backstitch->start.y() >= startRow) {
            backstitch->start.setY(backstitch->start.y() + rows);
        }

        if (backstitch->end.y() >= startRow) {
            backstitch->end.setY(backstitch->end.y() + rows);
        }
    }

    QListIterator<Knot *> knotIterator(m_knots);

    while (knotIterator.hasNext()) {
        Knot *knot = knotIterator.next();

        if (knot->position.y() >= startRow) {
            knot->position.setY(knot->position.y() + rows);
        }
    }
}


void StitchData::removeColumns(int startColumn, int columns)
{
    for (int y = 0 ; y < m_height ; ++y) {
        for (int destinationColumn = startColumn, sourceColumn = startColumn + columns ; sourceColumn < m_width ; ++destinationColumn, ++sourceColumn) {
            m_stitches[index(destinationColumn, y)] = takeStitchQueueAt(sourceColumn, y);
        }
    }

    int snapStartColumn = startColumn * 2;
    int snapColumns = columns * 2;

    QListIterator<Backstitch *> backstitchIterator(m_backstitches);

    while (backstitchIterator.hasNext()) {
        Backstitch *backstitch = backstitchIterator.next();

        if (backstitch->start.x() >= snapStartColumn + snapColumns) {
            backstitch->start.setX(backstitch->start.x() - snapColumns);
        }

        if (backstitch->end.x() >= snapStartColumn + snapColumns) {
            backstitch->end.setX(backstitch->end.x() - snapColumns);
        }
    }

    QListIterator<Knot *> knotIterator(m_knots);

    while (knotIterator.hasNext()) {
        Knot *knot = knotIterator.next();

        if (knot->position.x() >= snapStartColumn + snapColumns) {
            knot->position.setX(knot->position.x() - snapColumns);
        }
    }

    resize(m_width - columns, m_height);
}


void StitchData::removeRows(int startRow, int rows)
{
    for (int destinationRow = startRow, sourceRow = startRow + rows ; sourceRow < m_height ; ++destinationRow, ++sourceRow) {
        for (int x = 0 ; x < m_width ; ++x) {
            m_stitches[index(x, destinationRow)] = takeStitchQueueAt(x, sourceRow);
        }
    }

    int snapStartRow = startRow * 2;
    int snapRows = rows * 2;

    QListIterator<Backstitch *> backstitchIterator(m_backstitches);

    while (backstitchIterator.hasNext()) {
        Backstitch *backstitch = backstitchIterator.next();

        if (backstitch->start.y() >= snapStartRow + snapRows) {
            backstitch->start.setY(backstitch->start.y() - snapRows);
        }

        if (backstitch->end.y() >= snapStartRow + snapRows) {
            backstitch->end.setY(backstitch->end.y() - snapRows);
        }
    }

    QListIterator<Knot *> knotIterator(m_knots);

    while (knotIterator.hasNext()) {
        Knot *knot = knotIterator.next();

        if (knot->position.y() >= snapStartRow + snapRows) {
            knot->position.setY(knot->position.y() - snapRows);
        }
    }

    resize(m_width, m_height - rows);
}


QRect StitchData::extents() const
{
    QRect extentsRect;

    for (int y = 0 ; y < m_height ; ++y) {
        for (int x = 0 ; x < m_width ; ++x) {
            if (m_stitches.at(index(x, y))) {
                extentsRect |= QRect(x * 2, y * 2, 2, 2);
            }
        }
    }

    QListIterator<Backstitch *> backstitchIterator(m_backstitches);

    while (backstitchIterator.hasNext()) {
        Backstitch *backstitch = backstitchIterator.next();
        extentsRect |= QRect(backstitch->start, backstitch->end).normalized();
    }

    QListIterator<Knot *> knotIterator(m_knots);

    while (knotIterator.hasNext()) {
        Knot *knot = knotIterator.next();
        extentsRect |= QRect(knot->position, QSize(0, 0));
    }

    extentsRect.adjust(-(extentsRect.left() % 2), -(extentsRect.top() % 2), extentsRect.right() % 2, extentsRect.bottom() % 2);

    if (extentsRect.isValid()) {
        extentsRect = QRect(extentsRect.left() / 2, extentsRect.top() / 2, extentsRect.width() / 2, extentsRect.height() / 2);
    }

    return extentsRect;
}


void StitchData::movePattern(int dx, int dy)
{
    QRect extentsRect = extents();

    QVector<StitchQueue *> newVector(m_width * m_height);

    for (int y = extentsRect.top() ; y <= extentsRect.bottom() ; ++y) {
        for (int x = extentsRect.left() ; x <= extentsRect.right() ; ++x) {
            newVector[index(x + dx, y + dy)] = takeStitchQueueAt(x, y);
        }
    }

    m_stitches = newVector;

    dx *= 2;
    dy *= 2;

    QListIterator<Backstitch *> backstitchIterator(m_backstitches);

    while (backstitchIterator.hasNext()) {
        backstitchIterator.next()->move(dx, dy);
    }

    QListIterator<Knot *> knotIterator(m_knots);

    while (knotIterator.hasNext()) {
        knotIterator.next()->move(dx, dy);
    }
}


void StitchData::mirror(Qt::Orientation orientation)
{
    int rows = m_height;
    int cols = m_width;

    if (orientation == Qt::Vertical) {
        rows = rows / 2 + rows % 2;
    } else {
        cols = cols / 2 + cols % 2;
    }

    for (int row = 0 ; row < rows ; ++row) {
        for (int col = 0 ; col < cols ; ++col) {
            QPoint srcCell(col, row);
            QPoint dstCell;

            if (orientation == Qt::Vertical) {
                dstCell = QPoint(col, m_height - row - 1);
            } else {
                dstCell = QPoint(m_width - col - 1, row);
            }

            StitchQueue *src = takeStitchQueueAt(srcCell);
            StitchQueue *dst = takeStitchQueueAt(dstCell);

            if (src) {
                invertQueue(orientation, src);
                replaceStitchQueueAt(dstCell, src);
            }

            if (dst) {
                invertQueue(orientation, dst);
                replaceStitchQueueAt(srcCell, dst);
            }
        }
    }

    int maxXSnap = m_width * 2;
    int maxYSnap = m_height * 2;
    QListIterator<Backstitch *> bi(m_backstitches);

    while (bi.hasNext()) {
        Backstitch *backstitch = bi.next();

        if (orientation == Qt::Horizontal) {
            backstitch->start.setX(maxXSnap - backstitch->start.x());
            backstitch->end.setX(maxXSnap - backstitch->end.x());
        } else {
            backstitch->start.setY(maxYSnap - backstitch->start.y());
            backstitch->end.setY(maxYSnap - backstitch->end.y());
        }
    }

    QListIterator<Knot *> ki(m_knots);

    while (ki.hasNext()) {
        Knot *knot = ki.next();

        if (orientation == Qt::Horizontal) {
            knot->position.setX(maxXSnap - knot->position.x());
        } else {
            knot->position.setY(maxYSnap - knot->position.y());
        }
    }
}


void StitchData::rotate(Rotation rotation)
{
    int rows = m_height;
    int cols = m_width;

    QVector<StitchQueue *> rotatedData(m_width * m_height);

    for (int y = 0 ; y < rows ; ++y) {
        for (int x = 0 ; x < cols ; ++x) {
            StitchQueue *src = takeStitchQueueAt(x, y);
            int index = (cols - x - 1) * rows + y; // default to Rotate90

            switch (rotation) {
            case Rotate90:
                // index = (cols - x - 1) * rows + y;
                break;

            case Rotate180:
                index = (rows - y - 1) * cols + (cols - x - 1);
                break;

            case Rotate270:
                index = x * rows + (rows - y - 1);
                break;
            }

            if (src) {
                rotateQueue(rotation, src);
                rotatedData[index] = src;
            }
        }
    }

    if ((rotation == Rotate90) || (rotation == Rotate270)) {
        int height = m_height;
        m_height = m_width;
        m_width = height;
    }

    m_stitches = rotatedData;

    int maxXSnap = m_width * 2;
    int maxYSnap = m_height * 2;
    QListIterator<Backstitch *> bi(m_backstitches);

    while (bi.hasNext()) {
        Backstitch *backstitch = bi.next();

        switch (rotation) {
        case Rotate90:
            backstitch->start = QPoint(backstitch->start.y(), maxXSnap - backstitch->start.x());
            backstitch->end = QPoint(backstitch->end.y(), maxXSnap - backstitch->end.x());
            break;

        case Rotate180:
            backstitch->start = QPoint(maxXSnap - backstitch->start.x(), maxYSnap - backstitch->start.y());
            backstitch->end = QPoint(maxXSnap - backstitch->end.x(), maxYSnap - backstitch->end.y());
            break;

        case Rotate270:
            backstitch->start = QPoint(maxYSnap - backstitch->start.y(), backstitch->start.x());
            backstitch->end = QPoint(maxYSnap - backstitch->end.y(), backstitch->end.x());
            break;
        }
    }

    QListIterator<Knot *> ki(m_knots);

    while (ki.hasNext()) {
        Knot *knot = ki.next();

        switch (rotation) {
        case Rotate90:
            knot->position = QPoint(knot->position.y(), maxXSnap - knot->position.x());
            break;

        case Rotate180:
            knot->position = QPoint(maxXSnap - knot->position.x(), maxYSnap - knot->position.y());
            break;

        case Rotate270:
            knot->position = QPoint(maxYSnap - knot->position.y(), knot->position.x());
            break;
        }
    }
}


void StitchData::invertQueue(Qt::Orientation orientation, StitchQueue *queue)
{
    static QMap<Qt::Orientation, QMap<Stitch::Type, Stitch::Type> > mirrorMap;

    if (mirrorMap.isEmpty()) {
        mirrorMap[Qt::Horizontal][Stitch::Delete] = Stitch::Delete;
        mirrorMap[Qt::Horizontal][Stitch::TLQtr] = Stitch::TRQtr;
        mirrorMap[Qt::Horizontal][Stitch::TRQtr] = Stitch::TLQtr;
        mirrorMap[Qt::Horizontal][Stitch::BLQtr] = Stitch::BRQtr;
        mirrorMap[Qt::Horizontal][Stitch::BRQtr] = Stitch::BLQtr;
        mirrorMap[Qt::Horizontal][Stitch::BTHalf] = Stitch::TBHalf;
        mirrorMap[Qt::Horizontal][Stitch::TBHalf] = Stitch::BTHalf;
        mirrorMap[Qt::Horizontal][Stitch::TL3Qtr] = Stitch::TR3Qtr;
        mirrorMap[Qt::Horizontal][Stitch::TR3Qtr] = Stitch::TL3Qtr;
        mirrorMap[Qt::Horizontal][Stitch::BL3Qtr] = Stitch::BR3Qtr;
        mirrorMap[Qt::Horizontal][Stitch::BR3Qtr] = Stitch::BL3Qtr;
        mirrorMap[Qt::Horizontal][Stitch::TLSmallHalf] = Stitch::TRSmallHalf;
        mirrorMap[Qt::Horizontal][Stitch::TRSmallHalf] = Stitch::TLSmallHalf;
        mirrorMap[Qt::Horizontal][Stitch::BLSmallHalf] = Stitch::BRSmallHalf;
        mirrorMap[Qt::Horizontal][Stitch::BRSmallHalf] = Stitch::BLSmallHalf;
        mirrorMap[Qt::Horizontal][Stitch::TLSmallFull] = Stitch::TRSmallFull;
        mirrorMap[Qt::Horizontal][Stitch::TRSmallFull] = Stitch::TLSmallFull;
        mirrorMap[Qt::Horizontal][Stitch::BLSmallFull] = Stitch::BRSmallFull;
        mirrorMap[Qt::Horizontal][Stitch::BRSmallFull] = Stitch::BLSmallFull;
        mirrorMap[Qt::Horizontal][Stitch::Full] = Stitch::Full;

        mirrorMap[Qt::Vertical][Stitch::Delete] = Stitch::Delete;
        mirrorMap[Qt::Vertical][Stitch::TLQtr] = Stitch::BLQtr;
        mirrorMap[Qt::Vertical][Stitch::TRQtr] = Stitch::BRQtr;
        mirrorMap[Qt::Vertical][Stitch::BLQtr] = Stitch::TLQtr;
        mirrorMap[Qt::Vertical][Stitch::BRQtr] = Stitch::TRQtr;
        mirrorMap[Qt::Vertical][Stitch::BTHalf] = Stitch::TBHalf;
        mirrorMap[Qt::Vertical][Stitch::TBHalf] = Stitch::BTHalf;
        mirrorMap[Qt::Vertical][Stitch::TL3Qtr] = Stitch::BL3Qtr;
        mirrorMap[Qt::Vertical][Stitch::TR3Qtr] = Stitch::BR3Qtr;
        mirrorMap[Qt::Vertical][Stitch::BL3Qtr] = Stitch::TL3Qtr;
        mirrorMap[Qt::Vertical][Stitch::BR3Qtr] = Stitch::TR3Qtr;
        mirrorMap[Qt::Vertical][Stitch::TLSmallHalf] = Stitch::BLSmallHalf;
        mirrorMap[Qt::Vertical][Stitch::TRSmallHalf] = Stitch::BRSmallHalf;
        mirrorMap[Qt::Vertical][Stitch::BLSmallHalf] = Stitch::TLSmallHalf;
        mirrorMap[Qt::Vertical][Stitch::BRSmallHalf] = Stitch::TRSmallHalf;
        mirrorMap[Qt::Vertical][Stitch::TLSmallFull] = Stitch::BLSmallFull;
        mirrorMap[Qt::Vertical][Stitch::TRSmallFull] = Stitch::BRSmallFull;
        mirrorMap[Qt::Vertical][Stitch::BLSmallFull] = Stitch::TLSmallFull;
        mirrorMap[Qt::Vertical][Stitch::BRSmallFull] = Stitch::TRSmallFull;
        mirrorMap[Qt::Vertical][Stitch::Full] = Stitch::Full;
    }

    QListIterator<Stitch *> i(*queue);

    while (i.hasNext()) {
        Stitch *stitch = i.next();
        stitch->type = mirrorMap[orientation][stitch->type];
    }
}


void StitchData::rotateQueue(Rotation rotation, StitchQueue *queue)
{
    static QMap<Rotation, QMap<Stitch::Type, Stitch::Type> > rotateMap;

    if (rotateMap.isEmpty()) {
        rotateMap[Rotate90][Stitch::TLQtr] = Stitch::BLQtr;
        rotateMap[Rotate90][Stitch::TRQtr] = Stitch::TLQtr;
        rotateMap[Rotate90][Stitch::BLQtr] = Stitch::BRQtr;
        rotateMap[Rotate90][Stitch::BRQtr] = Stitch::TRQtr;
        rotateMap[Rotate90][Stitch::BTHalf] = Stitch::TBHalf;
        rotateMap[Rotate90][Stitch::TBHalf] = Stitch::BTHalf;
        rotateMap[Rotate90][Stitch::TL3Qtr] = Stitch::BL3Qtr;
        rotateMap[Rotate90][Stitch::TR3Qtr] = Stitch::TL3Qtr;
        rotateMap[Rotate90][Stitch::BL3Qtr] = Stitch::BR3Qtr;
        rotateMap[Rotate90][Stitch::BR3Qtr] = Stitch::TR3Qtr;
        rotateMap[Rotate90][Stitch::TLSmallHalf] = Stitch::BLSmallHalf;
        rotateMap[Rotate90][Stitch::TRSmallHalf] = Stitch::TLSmallHalf;
        rotateMap[Rotate90][Stitch::BLSmallHalf] = Stitch::BRSmallHalf;
        rotateMap[Rotate90][Stitch::BRSmallHalf] = Stitch::TRSmallHalf;
        rotateMap[Rotate90][Stitch::TLSmallFull] = Stitch::BLSmallFull;
        rotateMap[Rotate90][Stitch::TRSmallFull] = Stitch::TLSmallFull;
        rotateMap[Rotate90][Stitch::BLSmallFull] = Stitch::BRSmallFull;
        rotateMap[Rotate90][Stitch::BRSmallFull] = Stitch::TRSmallFull;
        rotateMap[Rotate90][Stitch::Full] = Stitch::Full;

        rotateMap[Rotate180][Stitch::TLQtr] = Stitch::BRQtr;
        rotateMap[Rotate180][Stitch::TRQtr] = Stitch::BLQtr;
        rotateMap[Rotate180][Stitch::BLQtr] = Stitch::TRQtr;
        rotateMap[Rotate180][Stitch::BRQtr] = Stitch::TLQtr;
        rotateMap[Rotate180][Stitch::BTHalf] = Stitch::BTHalf;
        rotateMap[Rotate180][Stitch::TBHalf] = Stitch::TBHalf;
        rotateMap[Rotate180][Stitch::TL3Qtr] = Stitch::BR3Qtr;
        rotateMap[Rotate180][Stitch::TR3Qtr] = Stitch::BL3Qtr;
        rotateMap[Rotate180][Stitch::BL3Qtr] = Stitch::TR3Qtr;
        rotateMap[Rotate180][Stitch::BR3Qtr] = Stitch::TL3Qtr;
        rotateMap[Rotate180][Stitch::TLSmallHalf] = Stitch::BRSmallHalf;
        rotateMap[Rotate180][Stitch::TRSmallHalf] = Stitch::BLSmallHalf;
        rotateMap[Rotate180][Stitch::BLSmallHalf] = Stitch::TRSmallHalf;
        rotateMap[Rotate180][Stitch::BRSmallHalf] = Stitch::TLSmallHalf;
        rotateMap[Rotate180][Stitch::TLSmallFull] = Stitch::BRSmallFull;
        rotateMap[Rotate180][Stitch::TRSmallFull] = Stitch::BLSmallFull;
        rotateMap[Rotate180][Stitch::BLSmallFull] = Stitch::TRSmallFull;
        rotateMap[Rotate180][Stitch::BRSmallFull] = Stitch::TLSmallFull;
        rotateMap[Rotate180][Stitch::Full] = Stitch::Full;

        rotateMap[Rotate270][Stitch::TLQtr] = Stitch::TRQtr;
        rotateMap[Rotate270][Stitch::TRQtr] = Stitch::BRQtr;
        rotateMap[Rotate270][Stitch::BLQtr] = Stitch::TLQtr;
        rotateMap[Rotate270][Stitch::BRQtr] = Stitch::BLQtr;
        rotateMap[Rotate270][Stitch::BTHalf] = Stitch::TBHalf;
        rotateMap[Rotate270][Stitch::TBHalf] = Stitch::BTHalf;
        rotateMap[Rotate270][Stitch::TL3Qtr] = Stitch::TR3Qtr;
        rotateMap[Rotate270][Stitch::TR3Qtr] = Stitch::BR3Qtr;
        rotateMap[Rotate270][Stitch::BL3Qtr] = Stitch::TL3Qtr;
        rotateMap[Rotate270][Stitch::BR3Qtr] = Stitch::BL3Qtr;
        rotateMap[Rotate270][Stitch::TLSmallHalf] = Stitch::TRSmallHalf;
        rotateMap[Rotate270][Stitch::TRSmallHalf] = Stitch::BRSmallHalf;
        rotateMap[Rotate270][Stitch::BLSmallHalf] = Stitch::TLSmallHalf;
        rotateMap[Rotate270][Stitch::BRSmallHalf] = Stitch::BLSmallHalf;
        rotateMap[Rotate270][Stitch::TLSmallFull] = Stitch::TRSmallFull;
        rotateMap[Rotate270][Stitch::TRSmallFull] = Stitch::BRSmallFull;
        rotateMap[Rotate270][Stitch::BLSmallFull] = Stitch::TLSmallFull;
        rotateMap[Rotate270][Stitch::BRSmallFull] = Stitch::BLSmallFull;
        rotateMap[Rotate270][Stitch::Full] = Stitch::Full;
    }

    QListIterator<Stitch *> i(*queue);

    while (i.hasNext()) {
        Stitch *stitch = i.next();
        stitch->type = rotateMap[rotation][stitch->type];
    }
}


int StitchData::index(int x, int y) const
{
    return y * m_width + x;
}


int StitchData::index(const QPoint &cell) const
{
    return index(cell.x(), cell.y());
}


bool StitchData::isValid(int x, int y) const
{
    return ((x >= 0) && (x < m_width) && (y >= 0) && (y < m_height));
}


void StitchData::addStitch(const QPoint &position, Stitch::Type type, int colorIndex)
{
    int i = index(position);
    StitchQueue *stitchQueue = m_stitches.at(i);

    if (stitchQueue == 0) {
        stitchQueue = new StitchQueue;
        m_stitches[i] = stitchQueue;
    }

    stitchQueue->add(type, colorIndex);
}


Stitch *StitchData::findStitch(const QPoint &cell, Stitch::Type type, int colorIndex)
{
    StitchQueue *stitchQueue = stitchQueueAt(cell);
    Stitch *found = 0;

    if (stitchQueue) {
        if (Stitch *stitch = stitchQueue->find(type, colorIndex)) {
            found = stitch;
        }
    }

    return found;
}


void StitchData::deleteStitch(const QPoint &position, Stitch::Type type, int colorIndex)
{
    int i = index(position);
    StitchQueue *stitchQueue = m_stitches.at(i);

    if (stitchQueue) {
        if (stitchQueue->remove(type, colorIndex) == 0) {
            m_stitches[i] = 0;
            delete stitchQueue;
        }
    }
}


StitchQueue *StitchData::stitchQueueAt(int x, int y)
{
    StitchQueue *stitchQueue = 0;

    if (isValid(x, y)) {
        stitchQueue = m_stitches.at(index(x, y));
    }

    return stitchQueue;
}


StitchQueue *StitchData::stitchQueueAt(const QPoint &position)
{
    return stitchQueueAt(position.x(), position.y());
}


StitchQueue *StitchData::takeStitchQueueAt(int x, int y)
{
    StitchQueue *stitchQueue = stitchQueueAt(x, y);

    if (stitchQueue) {
        m_stitches[index(x, y)] = 0;
    }

    return stitchQueue;
}


StitchQueue *StitchData::takeStitchQueueAt(const QPoint &position)
{
    return takeStitchQueueAt(position.x(), position.y());
}


StitchQueue *StitchData::replaceStitchQueueAt(int x, int y, StitchQueue *stitchQueue)
{
    StitchQueue *originalQueue = takeStitchQueueAt(x, y);

    if (isValid(x, y)) {
        m_stitches[index(x, y)] = stitchQueue;
    }

    return originalQueue;
}


StitchQueue *StitchData::replaceStitchQueueAt(const QPoint &position, StitchQueue *stitchQueue)
{
    return replaceStitchQueueAt(position.x(), position.y(), stitchQueue);
}


void StitchData::addBackstitch(const QPoint &start, const QPoint &end, int colorIndex)
{
    m_backstitches.append(new Backstitch(start, end, colorIndex));
}


void StitchData::addBackstitch(Backstitch *backstitch)
{
    m_backstitches.append(backstitch);
}


Backstitch *StitchData::findBackstitch(const QPoint &start, const QPoint &end, int colorIndex)
{
    Backstitch *found = 0;

    foreach (Backstitch *backstitch, m_backstitches) {
        if (backstitch->contains(start) && backstitch->contains(end) && ((colorIndex == -1) || backstitch->colorIndex == colorIndex)) {
            found = backstitch;
            break;
        }
    }

    return found;
}


Backstitch *StitchData::takeBackstitch(const QPoint &start, const QPoint &end, int colorIndex)
{
    Backstitch *removed = findBackstitch(start, end, colorIndex);
    m_backstitches.removeOne(removed);

    return removed;
}


Backstitch *StitchData::takeBackstitch(Backstitch *backstitch)
{
    Backstitch *removed = 0;

    if (m_backstitches.removeOne(backstitch)) {
        removed = backstitch;
    }

    return removed;
}


void StitchData::addFrenchKnot(const QPoint &position, int colorIndex)
{
    m_knots.append(new Knot(position, colorIndex));
}


void StitchData::addFrenchKnot(Knot *knot)
{
    m_knots.append(knot);
}


Knot *StitchData::findKnot(const QPoint &position, int colorIndex)
{
    Knot *found = 0;

    foreach (Knot *knot, m_knots) {
        if ((knot->position == position) && ((colorIndex == -1) || (knot->colorIndex == colorIndex))) {
            found = knot;
            break;
        }
    }

    return found;
}


Knot *StitchData::takeFrenchKnot(const QPoint &position, int colorIndex)
{
    Knot *removed = findKnot(position, colorIndex);

    if (removed) {
        m_knots.removeOne(removed);
    }

    return removed;
}


Knot *StitchData::takeFrenchKnot(Knot *knot)
{
    Knot *removed = 0;

    if (m_knots.removeOne(knot)) {
        removed = knot;
    }

    return removed;
}


QList<Backstitch *> &StitchData::backstitches()
{
    return m_backstitches;
}


QList<Knot *> &StitchData::knots()
{
    return m_knots;
}


QListIterator<Backstitch *> StitchData::backstitchIterator()
{
    return QListIterator<Backstitch *>(m_backstitches);
}


QMutableListIterator<Backstitch *> StitchData::mutableBackstitchIterator()
{
    return QMutableListIterator<Backstitch *>(m_backstitches);
}


QListIterator<Knot *> StitchData::knotIterator()
{
    return QListIterator<Knot *>(m_knots);
}


QMutableListIterator<Knot *> StitchData::mutableKnotIterator()
{
    return QMutableListIterator<Knot *>(m_knots);
}


QMap<int, FlossUsage> StitchData::flossUsage()
{
    QMap<int, FlossUsage> usage;
    static QMap<Stitch::Type, double> lengths;

    if (!lengths.count()) {
        lengths.insert(Stitch::Delete, 0.0);
        lengths.insert(Stitch::TLQtr, 0.707107 + 0.5);
        lengths.insert(Stitch::TRQtr, 0.707107 + 0.5);
        lengths.insert(Stitch::BLQtr, 0.707107 + 0.5);
        lengths.insert(Stitch::BTHalf, 1.414213 + 1.0);
        lengths.insert(Stitch::TL3Qtr, 1.414213 + 0.707107 + 1.0 + 0.5);
        lengths.insert(Stitch::BRQtr, 0.707107 + 0.5);
        lengths.insert(Stitch::TBHalf, 1.414213 + 1.0);
        lengths.insert(Stitch::TR3Qtr, 1.414213 + 0.707107 + 1.0 + 0.5);
        lengths.insert(Stitch::BL3Qtr, 1.414213 + 0.707107 + 1.0 + 0.5);
        lengths.insert(Stitch::BR3Qtr, 1.414213 + 0.707107 + 1.0 + 0.5);
        lengths.insert(Stitch::Full, 1.414213 + 1.414213 + 1.0 + 1.0);
        lengths.insert(Stitch::TLSmallHalf, 0.707107 + 0.5);
        lengths.insert(Stitch::TRSmallHalf, 0.707107 + 0.5);
        lengths.insert(Stitch::BLSmallHalf, 0.707107 + 0.5);
        lengths.insert(Stitch::BRSmallHalf, 0.707107 + 0.5);
        lengths.insert(Stitch::TLSmallFull, 0.707107 + 0.5 + 0.707107 + 0.5);
        lengths.insert(Stitch::TRSmallFull, 0.707107 + 0.5 + 0.707107 + 0.5);
        lengths.insert(Stitch::BLSmallFull, 0.707107 + 0.5 + 0.707107 + 0.5);
        lengths.insert(Stitch::BRSmallFull, 0.707107 + 0.5 + 0.707107 + 0.5);
        lengths.insert(Stitch::FrenchKnot, 2.0);
    }

    QVectorIterator<StitchQueue *> stitchesIterator(m_stitches);

    while (stitchesIterator.hasNext()) {
        StitchQueue *stitchQueue = stitchesIterator.next();

        if (stitchQueue) {
            QListIterator<Stitch *> stitchIterator(*stitchQueue);

            while (stitchIterator.hasNext()) {
                Stitch *stitch = stitchIterator.next();
                usage[stitch->colorIndex].stitchCounts[stitch->type]++;
                usage[stitch->colorIndex].stitchLengths[stitch->type] += lengths[stitch->type];
            }
        }
    }


    QListIterator<Backstitch *> backstitchIterator(m_backstitches);

    while (backstitchIterator.hasNext()) {
        Backstitch *backstitch = backstitchIterator.next();
        usage[backstitch->colorIndex].backstitchCount++;
        usage[backstitch->colorIndex].backstitchLength += QPoint(backstitch->start - backstitch->end).manhattanLength();
    }

    QListIterator<Knot *> knotIterator(m_knots);

    while (knotIterator.hasNext()) {
        Knot *knot = knotIterator.next();
        usage[knot->colorIndex].stitchCounts[Stitch::FrenchKnot]++;
        usage[knot->colorIndex].stitchLengths[Stitch::FrenchKnot] += lengths[Stitch::FrenchKnot];
    }

    return usage;
}


QDataStream &operator<<(QDataStream &stream, const StitchData &stitchData)
{
    stream << qint32(stitchData.version);
    stream << qint32(stitchData.m_width);
    stream << qint32(stitchData.m_height);

    QVectorIterator<StitchQueue *> stitchesIterator(stitchData.m_stitches);
    int queues = 0;

    while (stitchesIterator.hasNext()) {
        if (stitchesIterator.next()) {
            ++queues;
        }
    }

    stream << qint32(queues);

    for (int row = 0 ; row < stitchData.m_height ; ++row) {
        for (int column = 0 ; column < stitchData.m_width ; ++column) {
            if (StitchQueue *stitchQueue = stitchData.m_stitches[stitchData.index(column, row)]) {
                stream << qint32(column);
                stream << qint32(row);
                stream << *stitchQueue;
            }
        }
    }

    QListIterator<Backstitch *> backstitchIterator(stitchData.m_backstitches);
    stream << qint32(stitchData.m_backstitches.count());

    while (backstitchIterator.hasNext()) {
        stream << *(backstitchIterator.next());

        if (stream.status() != QDataStream::Ok) {
            throw FailedWriteFile(stream.status());
        }
    }

    QListIterator<Knot *> knotIterator(stitchData.m_knots);
    stream << qint32(stitchData.m_knots.count());

    while (knotIterator.hasNext()) {
        stream << *(knotIterator.next());

        if (stream.status() != QDataStream::Ok) {
            throw FailedWriteFile(stream.status());
        }
    }

    if (stream.status() != QDataStream::Ok) {
        throw FailedWriteFile(stream.status());
    }

    return stream;
}


QDataStream &operator>>(QDataStream &stream, StitchData &stitchData)
{
    qint32 version;
    qint32 width;
    qint32 height;
    qint32 layers;
    qint32 columns;
    qint32 rows;
    qint32 count;
    QHash<int, QHash<int, StitchQueue *> > stitches;

    stitchData.clear();

    stream >> version;

    switch (version) {
    case 103:
        stream >> width;
        stream >> height;
        stitchData.resize(width, height);
        stream >> count;

        while (count--) {
            stream >> columns;
            stream >> rows;
            StitchQueue *stitchQueue = new StitchQueue;
            stitchData.replaceStitchQueueAt(columns, rows, stitchQueue);
            stream >> *stitchQueue;
        }

        stream >> count;

        while (count--) {
            Backstitch *backstitch = new Backstitch;
            stream >> *(backstitch);
            stitchData.addBackstitch(backstitch);
        }

        stream >> count;

        while (count--) {
            Knot *knot = new Knot;
            stream >> *knot;
            stitchData.addFrenchKnot(knot);
        }

        break;

    case 102:
        stream >> width;
        stream >> height;
        stitchData.resize(width, height);

        stream >> columns;

        while (columns--) {
            stream >> rows;

            while (rows--) {
                qint32 column;
                qint32 row;

                stream >> column;
                stream >> row;

                StitchQueue *stitchQueue = new StitchQueue;
                stitches[column][row] = stitchQueue;
                stream >> *stitchQueue;
            }
        }

        for (int y = 0 ; y < height ; ++y) {
            for (int x = 0 ; x < width ; ++x) {
                if (stitches[x][y]) {
                    stitchData.replaceStitchQueueAt(x, y, stitches[x][y]);
                }
            }
        }

        stream >> count;

        while (count--) {
            Backstitch *backstitch = new Backstitch;
            stream >> *(backstitch);
            stitchData.addBackstitch(backstitch);
        }

        stream >> count;

        while (count--) {
            Knot *knot = new Knot;
            stream >> *knot;
            stitchData.addFrenchKnot(knot);
        }

        break;

    case 101:
        stream >> width;
        stream >> height;
        stitchData.resize(width, height);

        stream >> columns;

        while (columns--) {
            stream >> rows;

            while (rows--) {
                qint32 column;
                qint32 row;

                stream >> column;
                stream >> row;

                StitchQueue *stitchQueue = new StitchQueue;
                stitches[column][row] = stitchQueue;
                stream >> *stitchQueue;
            }
        }

        for (int y = 0 ; y < height ; ++y) {
            for (int x = 0 ; x < width ; ++x) {
                if (stitches[x][y]) {
                    stitchData.replaceStitchQueueAt(x, y, stitches[x][y]);
                }
            }
        }

        break;

    case 100:
        stream >> width;
        stream >> height;
        stitchData.m_width = width;
        stitchData.m_height = height;

        stream >> layers;

        while (layers--) {
            stream >> columns;

            while (columns--) {
                stream >> rows;

                while (rows--) {
                    qint32 layer;
                    qint32 column;
                    qint32 row;

                    stream >> layer;
                    stream >> column;
                    stream >> row;

                    StitchQueue *stitchQueue = new StitchQueue;
                    stitches[column][row] = stitchQueue;
                    stream >> *stitchQueue;
                }
            }
        }

        for (int y = 0 ; y < height ; ++y) {
            for (int x = 0 ; x < width ; ++x) {
                if (stitches[x][y]) {
                    stitchData.replaceStitchQueueAt(x, y, stitches[x][y]);
                }
            }
        }

        break;

    default:
        throw InvalidFileVersion(QString(i18n("Stitch data version %1", version)));
        break;
    }

    if (stream.status() != QDataStream::Ok) {
        throw FailedReadFile(QString(i18n("Failed reading stitch data")));
    }

    return stream;
}
