/*
 * Copyright (C) 2012-2015 by Stephen Allewell
 * steve.allewell@gmail.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#include "Pattern.h"

#include <KLocalizedString>

#include "Exceptions.h"


Pattern::Pattern(Document *document)
    :   m_document(document)
{
}


void Pattern::clear()
{
    m_documentPalette = DocumentPalette();
    m_stitchData.clear();
}


Document *Pattern::document()
{
    return m_document;
}


DocumentPalette &Pattern::palette()
{
    return m_documentPalette;
}


StitchData &Pattern::stitches()
{
    return m_stitchData;
}


void Pattern::constructPalette(Pattern *pattern)
{
    pattern->palette().setSchemeName(m_documentPalette.schemeName());
    QMap<int, FlossUsage> usage = pattern->stitches().flossUsage();
    QMapIterator<int, FlossUsage> flossIterator(usage);

    while (flossIterator.hasNext()) {
        flossIterator.next();
        int index = flossIterator.key();
        double length = flossIterator.value().totalLength();

        if (length > 0) {
            pattern->palette().add(index, new DocumentFloss(palette().flosses().value(index)));
        }
    }
}


Pattern *Pattern::cut(const QRect &area, int colorMask, const QList<Stitch::Type> &stitchMask, bool excludeBackstitches, bool excludeKnots)
{
    Pattern *pattern = new Pattern;
    pattern->stitches().resize(area.width(), area.height());

    for (int row = area.top() ; row <= area.bottom() ; row++) {
        for (int column = area.left() ; column <= area.right() ; ++column) {
            QPoint src(column, row);
            QPoint dst(src - area.topLeft());
            StitchQueue *srcQ = stitches().takeStitchQueueAt(src);
            StitchQueue *dstQ = new StitchQueue;

            if (srcQ) {
                // iterate the queue adding anything that matches the stitch mask or color mask to a new queue
                int count = srcQ->count();

                while (count--) {
                    Stitch *stitch = srcQ->dequeue();

                    if (((colorMask == -1) || (colorMask == stitch->colorIndex)) && (stitchMask.contains(stitch->type))) {
                        dstQ->enqueue(stitch);
                    } else {
                        srcQ->enqueue(stitch);
                    }
                }

                if (srcQ->count()) {
                    stitches().replaceStitchQueueAt(src, srcQ);
                } else {
                    delete srcQ;
                }

                if (dstQ->count()) {
                    pattern->stitches().replaceStitchQueueAt(dst, dstQ);
                } else {
                    delete dstQ;
                }
            }
        }
    }

    QRect snapArea(area.left() * 2, area.top() * 2, area.width() * 2, area.height() * 2);

    if (!excludeBackstitches) {
        QMutableListIterator<Backstitch *> mutableListIterator = stitches().mutableBackstitchIterator();

        while (mutableListIterator.hasNext()) {
            Backstitch *backstitch = mutableListIterator.next();

            if (((colorMask == -1) || (colorMask == backstitch->colorIndex)) && (snapArea.contains(backstitch->start) && snapArea.contains(backstitch->end))) {
                mutableListIterator.remove();
                backstitch->start -= snapArea.topLeft();
                backstitch->end -= snapArea.topLeft();
                pattern->stitches().addBackstitch(backstitch);
            }
        }
    }

    if (!excludeKnots) {
        QMutableListIterator<Knot *> mutableListIterator = stitches().mutableKnotIterator();

        while (mutableListIterator.hasNext()) {
            Knot *knot = mutableListIterator.next();

            if (((colorMask == -1) || (colorMask == knot->colorIndex)) && (snapArea.contains(knot->position))) {
                mutableListIterator.remove();
                knot->position -= snapArea.topLeft();
                pattern->stitches().addFrenchKnot(knot);
            }
        }
    }

    constructPalette(pattern);

    return pattern;
}


Pattern *Pattern::copy(const QRect &area,  int colorMask, const QList<Stitch::Type> &stitchMask, bool excludeBackstitches, bool excludeKnots)
{
    Pattern *pattern = new Pattern;
    pattern->stitches().resize(area.width(), area.height());

    for (int row = area.top() ; row <= area.bottom() ; row++) {
        for (int column = area.left() ; column <= area.right() ; ++column) {
            QPoint src(column, row);
            QPoint dst(src - area.topLeft());
            StitchQueue *srcQ = stitches().stitchQueueAt(src);
            StitchQueue *dstQ = new StitchQueue;

            if (srcQ) {
                QListIterator<Stitch *> stitchIterator(*srcQ);

                while (stitchIterator.hasNext()) {
                    Stitch *stitch = stitchIterator.next();

                    if (((colorMask == -1) || (colorMask == stitch->colorIndex)) && (stitchMask.contains(stitch->type))) {
                        dstQ->add(stitch->type, stitch->colorIndex);
                    }
                }

                if (dstQ->count()) {
                    pattern->stitches().replaceStitchQueueAt(dst, dstQ);
                } else {
                    delete dstQ;
                }
            }
        }
    }

    QRect snapArea(area.left() * 2, area.top() * 2, area.width() * 2, area.height() * 2);

    if (!excludeBackstitches) {
        QListIterator<Backstitch *> backstitchIterator = stitches().backstitchIterator();

        while (backstitchIterator.hasNext()) {
            Backstitch *backstitch = backstitchIterator.next();

            if (((colorMask == -1) || (colorMask == backstitch->colorIndex)) && (snapArea.contains(backstitch->start) && snapArea.contains(backstitch->end))) {
                pattern->stitches().addBackstitch(backstitch->start - snapArea.topLeft(), backstitch->end - snapArea.topLeft(), backstitch->colorIndex);
            }
        }
    }

    if (!excludeKnots) {
        QListIterator<Knot *> knotIterator = stitches().knotIterator();

        while (knotIterator.hasNext()) {
            Knot *knot = knotIterator.next();

            if (((colorMask == -1) || (colorMask == knot->colorIndex)) && (snapArea.contains(knot->position))) {
                pattern->stitches().addFrenchKnot(knot->position - snapArea.topLeft(), knot->colorIndex);
            }
        }
    }

    constructPalette(pattern);

    return pattern;
}


void Pattern::paste(Pattern *pattern, const QPoint &cell, bool merge)
{
    pattern->palette().setSchemeName(palette().schemeName());

    for (int row = 0 ; row < pattern->stitches().height() ; ++row) {
        for (int col = 0 ; col < pattern->stitches().width() ; ++col) {
            QPoint src(col, row);
            QPoint dst(cell + src);

            StitchQueue *srcQ = pattern->stitches().stitchQueueAt(src);
            StitchQueue *dstQ = stitches().takeStitchQueueAt(dst);

            if (!merge) {
                delete dstQ;
                dstQ = nullptr;
            }

            if (srcQ) {
                if (dstQ == nullptr) {
                    dstQ = new StitchQueue();
                }

                QListIterator<Stitch *> stitchIterator(*srcQ);

                while (stitchIterator.hasNext()) {
                    Stitch *stitch = stitchIterator.next();
                    int colorIndex = palette().add(pattern->palette().flosses().value(stitch->colorIndex)->flossColor());
                    dstQ->add(stitch->type, colorIndex);
                }
            }

            stitches().replaceStitchQueueAt(dst, dstQ);
        }
    }

    QRect snapArea(0, 0, stitches().width() * 2, stitches().height() * 2);
    QPoint targetOffset(cell * 2);

    QListIterator<Backstitch *> backstitchIterator = pattern->stitches().backstitchIterator();

    while (backstitchIterator.hasNext()) {
        Backstitch *backstitch = backstitchIterator.next();
        int colorIndex = palette().add(pattern->palette().flosses().value(backstitch->colorIndex)->flossColor());

        if (snapArea.contains(backstitch->start + targetOffset) && snapArea.contains(backstitch->end + targetOffset)) {
            stitches().addBackstitch(backstitch->start + targetOffset, backstitch->end + targetOffset, colorIndex);
        }
    }

    QListIterator<Knot *> knotIterator = pattern->stitches().knotIterator();

    while (knotIterator.hasNext()) {
        Knot *knot = knotIterator.next();
        int colorIndex = palette().add(pattern->palette().flosses().value(knot->colorIndex)->flossColor());

        if (snapArea.contains(knot->position + targetOffset)) {
            stitches().addFrenchKnot(knot->position + targetOffset, colorIndex);
        }
    }
}


QDataStream &operator<<(QDataStream &stream, const Pattern &pattern)
{
    stream << qint32(pattern.version);

    stream << pattern.m_documentPalette;
    stream << pattern.m_stitchData;

    return stream;
}


QDataStream  &operator>>(QDataStream &stream, Pattern &pattern)
{
    qint32 version;

    stream >> version;

    switch (version) {
    case 100:
        stream >> pattern.m_documentPalette;
        stream >> pattern.m_stitchData;
        break;

    default:
        throw InvalidFileVersion(QString(i18n("Pattern version %1", version)));
        break;
    }

    return stream;
}
