/*
 * Copyright (C) 2010-2022 by Stephen Allewell
 * steve.allewell@gmail.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#include "Preview.h"

#include <QMouseEvent>
#include <QPainter>
#include <QScrollArea>
#include <QStyleOptionRubberBand>

#include "configuration.h"
#include "Document.h"


Preview::Preview(QWidget *parent)
    :   QWidget(parent),
        m_document(nullptr),
        m_zoomFactor(1.0)
{
    setObjectName(QStringLiteral("Preview#"));
    m_renderer.setRenderStitchesAs(Configuration::EnumRenderer_RenderStitchesAs::ColorBlocks);
    m_renderer.setRenderBackstitchesAs(Configuration::EnumRenderer_RenderBackstitchesAs::ColorLines);
    m_renderer.setRenderKnotsAs(Configuration::EnumRenderer_RenderKnotsAs::ColorBlocks);
}


void Preview::setDocument(Document *document)
{
    m_document = document;
    readDocumentSettings();
}


Document *Preview::document()
{
    return m_document;
}


void Preview::readDocumentSettings()
{
    int width = m_document->pattern()->stitches().width();
    int height = m_document->pattern()->stitches().height();
    m_cellWidth = 4;
    m_cellHeight = 4 * m_document->property(QStringLiteral("horizontalClothCount")).toDouble() / m_document->property(QStringLiteral("verticalClothCount")).toDouble();
    m_previewWidth = m_cellWidth * width * m_zoomFactor;
    m_previewHeight = m_cellHeight * height * m_zoomFactor;
    resize(m_previewWidth, m_previewHeight);
    m_cachedContents = QPixmap(m_previewWidth, m_previewHeight);
    drawContents();
}


void Preview::setVisibleCells(const QRect &cells)
{
    m_visible = cells;
    update();
}


void Preview::loadSettings()
{
    drawContents();
}


void Preview::mousePressEvent(QMouseEvent *e)
{
    if (e->buttons() & Qt::LeftButton) {
        m_start = m_tracking = m_end = contentToCell(e->pos());
    }
}


void Preview::mouseMoveEvent(QMouseEvent *e)
{
    if (e->buttons() & Qt::LeftButton) {
        m_tracking = contentToCell(e->pos());

        if (m_tracking != m_start) {
            m_end = m_tracking;
            m_rubberBand = QRect(m_start, m_end).normalized();
            update();
        }
    }
}


void Preview::mouseReleaseEvent(QMouseEvent *)
{
    if (m_start == m_end) {
        emit clicked(m_start);
    } else {
        emit clicked(QRect(m_start, m_end).normalized());
    }

    m_rubberBand = QRect();
    update();
}


void Preview::drawContents()
{
    if ((m_document == nullptr) || (m_cachedContents.isNull())) {
        return;
    }

    m_cachedContents.fill(m_document->property(QStringLiteral("fabricColor")).value<QColor>());

    QPainter painter(&m_cachedContents);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setWindow(0, 0, m_document->pattern()->stitches().width(), m_document->pattern()->stitches().height());

    m_renderer.render(&painter, m_document->pattern(), painter.window(), false, true, true, true, -1);

    painter.end();
    update();
}


void Preview::paintEvent(QPaintEvent *)
{
    if (m_cachedContents.isNull()) {
        return;
    }

    QPainter painter(this);

    painter.drawPixmap(0, 0, m_cachedContents);

    QPen visibleAreaPen(Qt::white);
    visibleAreaPen.setCosmetic(true);

    painter.setPen(visibleAreaPen);
    painter.setBrush(Qt::NoBrush);
    painter.setCompositionMode(QPainter::RasterOp_SourceXorDestination);
    painter.setWindow(0, 0, m_document->pattern()->stitches().width(), m_document->pattern()->stitches().height());
    painter.drawRect(m_visible);

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


QPoint Preview::contentToCell(const QPoint &content) const
{
    return QPoint(content.x() / m_cellWidth, content.y() / m_cellHeight);
}
