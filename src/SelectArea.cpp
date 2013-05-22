/*
 * Copyright (C) 2010 by Stephen Allewell
 * sallewell@users.sourceforge.net
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#include "SelectArea.h"

#include <QMouseEvent>
#include <QPainter>
#include <QRubberBand>
#include <QScrollArea>
#include <QStyleOptionRubberBand>

#include <KDebug>

#include "Document.h"
#include "Element.h"


SelectArea::SelectArea(QWidget *parent, PatternElement *patternElement, Document *document, const QList<QRect> &patternRects)
    :   QWidget(parent),
        m_patternElement(patternElement),
        m_document(document),
        m_patternRect(patternElement->patternRect()),
        m_patternRects(patternRects)
{
    m_width = m_document->pattern()->stitches().width();
    m_height = m_document->pattern()->stitches().height();
    resize(m_width * 8, m_height * 8);
    m_fullPatternElement = new PatternElement(0, QRect(QPoint(0, 0), size()));
    m_fullPatternElement->setPatternRect(QRect(0, 0, m_width, m_height));
    m_fullPatternElement->setRenderStitchesAs(patternElement->renderStitchesAs());
    m_fullPatternElement->setRenderBackstitchesAs(patternElement->renderBackstitchesAs());
    m_fullPatternElement->setRenderKnotsAs(patternElement->renderKnotsAs());
}


SelectArea::~SelectArea()
{
}


QRect SelectArea::patternRect() const
{
    return m_patternRect;
}


void SelectArea::setPatternRect(const QRect &rect)
{
    m_patternRect = rect;
}


void SelectArea::mousePressEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton) {
        if (m_rubberBand.isValid()) {
            QRect r = m_rubberBand;
            m_rubberBand = QRect(0, 0, 0, 0);
            repaint(r);
        }

        m_cellStart = m_cellTracking = m_cellEnd = contentsToCell(event->pos());
        QRect updateArea = cellToRect(m_cellStart);
        m_rubberBand = updateArea;
        repaint(updateArea.adjusted(-8, -8, 8, 8));
    }
}


void SelectArea::mouseMoveEvent(QMouseEvent *event)
{
    QPoint p = event->pos();

    dynamic_cast<QScrollArea *>(parentWidget()->parentWidget())->ensureVisible(p.x(), p.y());

    m_cellTracking = contentsToCell(p);

    if (m_cellTracking != m_cellEnd) {
        m_cellEnd = m_cellTracking;
        QRect updateArea = (cellToRect(m_cellStart).united(cellToRect(m_cellEnd))).normalized();
        m_rubberBand = updateArea;
        repaint(updateArea.adjusted(-8, -8, 8, 8));
    }
}


void SelectArea::mouseReleaseEvent(QMouseEvent*)
{
    m_patternRect.setTopLeft(m_cellStart);
    m_patternRect.setBottomRight(m_cellEnd);
    m_patternRect = m_patternRect.normalized();
}


void SelectArea::paintEvent(QPaintEvent *event)
{
    QPainter painter;
    painter.begin(this);

    painter.fillRect(event->rect(), Qt::white);
    m_fullPatternElement->render(m_document, &painter);

    QListIterator<QRect> patternRectIterator(m_patternRects);
    QColor color(Qt::gray);
    color.setAlpha(100);

    while (patternRectIterator.hasNext()) {
        QRect rect = patternRectIterator.next();
        QRect previewRect(rect.left() * 8, rect.top() * 8, rect.width() * 8, rect.height() * 8);
        painter.setPen(color);
        painter.setBrush(color);
        painter.drawRect(previewRect);
    }

    if (m_rubberBand.isValid()) {
        QStyleOptionRubberBand opt;
        opt.initFrom(this);
        opt.shape = QRubberBand::Rectangle;
        opt.opaque = false;
        opt.rect = m_rubberBand;
        style()->drawControl(QStyle::CE_RubberBand, &opt, &painter);
    }

    painter.end();
}


QPoint SelectArea::contentsToCell(const QPoint &point) const
{
    return QPoint(point.x() / 8, point.y() / 8);
}


QRect SelectArea::cellToRect(const QPoint &cell) const
{
    int x = cell.x() * 8;
    int y = cell.y() * 8;
    return QRect(x, y, 8, 8);
}
