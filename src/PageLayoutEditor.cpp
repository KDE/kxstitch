/*
 * Copyright (C) 2010-2015 by Stephen Allewell
 * steve.allewell@gmail.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include "PageLayoutEditor.h"

#include <QMenu>
#include <QMouseEvent>
#include <QPainter>
#include <QPointer>
#include <QRubberBand>
#include <QToolTip>

#include <KLocalizedString>

#include <math.h>

#include "Document.h"
#include "Element.h"
#include "KeyElementDlg.h"
#include "Page.h"
#include "PagePreviewListWidgetItem.h"
#include "PagePropertiesDlg.h"
#include "TextElementDlg.h"
#include "configuration.h"

PageLayoutEditor::PageLayoutEditor(QWidget *parent, Document *document)
    : QWidget(parent)
    , m_document(document)
    , m_pagePreview(nullptr)
    , m_elementUnderCursor(nullptr)
    , m_selecting(false)
    , m_resizing(false)
    , m_moved(false)
    , m_node(nullptr)
    , m_showGrid(Configuration::page_ShowGrid())
    , m_gridSize(Configuration::page_GridSize())
    , m_zoomFactor(1.0)
{
    setContextMenuPolicy(Qt::CustomContextMenu);
    setPagePreview(nullptr);
    setMouseTracking(true);

    connect(this, &PageLayoutEditor::customContextMenuRequested, this, &PageLayoutEditor::contextMenuRequestedOn);
}

double PageLayoutEditor::zoomFactor() const
{
    return m_zoomFactor;
}

int PageLayoutEditor::gridSize() const
{
    return m_gridSize;
}

bool PageLayoutEditor::showGrid() const
{
    return m_showGrid;
}

void PageLayoutEditor::setPagePreview(PagePreviewListWidgetItem *pagePreview)
{
    if ((m_pagePreview = pagePreview)) {
        m_boundary.setElement(nullptr);
        show();
        updatePagePreview();
    } else {
        hide();
    }
}

void PageLayoutEditor::updatePagePreview()
{
    if (m_pagePreview) {
        m_paperWidth = m_pagePreview->paperWidth();
        m_paperHeight = m_pagePreview->paperHeight();
        setMinimumSize(scale(m_paperWidth), scale(m_paperHeight));
        resize(minimumSize());
        update();
    }
}

void PageLayoutEditor::setZoomFactor(double zoomFactor)
{
    m_zoomFactor = zoomFactor;

    if (m_pagePreview) {
        setMinimumSize(scale(m_paperWidth), scale(m_paperHeight));
        resize(minimumSize());
    }

    repaint();
}

void PageLayoutEditor::setGridSize(int size)
{
    m_gridSize = size;
    repaint();
}

void PageLayoutEditor::setShowGrid(bool show)
{
    m_showGrid = show;
    repaint();
}

void PageLayoutEditor::setSelecting(bool selecting)
{
    m_selecting = selecting;

    if (!selecting) {
        m_boundary.setElement(nullptr);
    }
}

void PageLayoutEditor::mousePressEvent(QMouseEvent *event)
{
    m_moved = false;
    m_start = m_end = toSnap(event->pos());

    if (event->buttons() & Qt::LeftButton) {
        if (m_selecting) {
            if (!m_node) {
                m_boundary.setElement(m_pagePreview->page()->itemAt(unscale(event->pos())));
            }
        } else {
            m_rubberBand = QRect(m_start, m_end).normalized();
        }

        update();
    }
}

void PageLayoutEditor::mouseMoveEvent(QMouseEvent *event)
{
    m_end = toSnap(event->pos());

    if (event->buttons() & Qt::LeftButton) {
        if (m_selecting) {
            QPoint offset = m_end - m_start;

            if (offset != QPoint(0, 0) && m_boundary.element()) {
                if (m_node) {
                    m_boundary.moveNode(m_node, m_end);
                } else {
                    m_boundary.element()->move(offset);
                    m_boundary.setRectangle(m_boundary.rectangle().translated(offset));
                }

                QToolTip::showText(QCursor::pos(),
                                   QString::fromLatin1("%1,%2 %3 x %4")
                                       .arg(m_boundary.rectangle().left())
                                       .arg(m_boundary.rectangle().top())
                                       .arg(m_boundary.rectangle().width())
                                       .arg(m_boundary.rectangle().height()));
                m_moved = true;
                m_start = m_end;
                update();
            }
        } else {
            m_rubberBand = QRect(m_start, m_end).normalized();
            update();

            QToolTip::showText(
                QCursor::pos(),
                QString::fromLatin1("%1,%2 %3 x %4").arg(m_rubberBand.left()).arg(m_rubberBand.top()).arg(m_rubberBand.width()).arg(m_rubberBand.height()));
        }
    } else {
        if (m_boundary.element() && (m_node = m_boundary.node(unscale(event->pos())))) {
            setCursor(m_boundary.cursor(m_node));
        } else {
            setCursor(Qt::ArrowCursor);
        }
    }
}

void PageLayoutEditor::mouseReleaseEvent(QMouseEvent *event)
{
    if (!m_selecting) {
        if (m_rubberBand.isValid()) {
            m_end = toSnap(event->pos());
            QRect selection = QRect(m_start, m_end).normalized();
            m_rubberBand = QRect();

            if (selection.isValid()) {
                emit selectionMade(selection.adjusted(0, 0, -1, -1));
            }
        }
    } else if (m_boundary.element() && m_moved) {
        emit elementGeometryChanged();
    }

    m_moved = false;
    update();
}

void PageLayoutEditor::paintEvent(QPaintEvent *event)
{
    QPainter painter;
    QRect updateRect = event->rect(); // in device coordinates

    painter.begin(this);
    painter.fillRect(updateRect, Qt::white);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setWindow(0, 0, m_paperWidth, m_paperHeight);

    m_pagePreview->page()->render(m_document, &painter);

    // draw snap grid
    if (m_showGrid) {
        int xOffset = (m_paperWidth / 2) % m_gridSize;
        int yOffset = (m_paperHeight / 2) % m_gridSize;

        for (int y = yOffset; y < m_paperHeight; y += m_gridSize) {
            for (int x = xOffset; x < m_paperWidth; x += m_gridSize) {
                painter.setPen(QPen(Qt::black, 0.2));
                painter.drawPoint(x, y);
            }
        }
    }

    if (m_rubberBand.isValid()) {
        bool qt4CompatiblePainting = painter.renderHints() & QPainter::Qt4CompatiblePainting;
        painter.setRenderHint(QPainter::Qt4CompatiblePainting, true);
        QStyleOptionRubberBand opt;
        opt.initFrom(this);
        opt.shape = QRubberBand::Rectangle;
        opt.opaque = false;
        opt.rect = m_rubberBand;
        style()->drawControl(QStyle::CE_RubberBand, &opt, &painter);
        painter.setRenderHint(QPainter::Qt4CompatiblePainting, qt4CompatiblePainting);
    }

    if (m_boundary.isValid()) {
        if (m_pagePreview->page()->elements().contains(m_boundary.element())) {
            m_boundary.render(&painter);
        } else {
            m_boundary.setElement(nullptr);
        }
    }

    painter.end();
}

void PageLayoutEditor::contextMenuRequestedOn(const QPoint &pos)
{
    Element *element = m_pagePreview->page()->itemAt(unscale(pos));

    if (element) {
        if (m_boundary.element() != element) {
            m_boundary.setElement(element);
        }
    } else {
        m_boundary.setElement(nullptr);
    }

    update();
}

QPoint PageLayoutEditor::toSnap(const QPoint &pos) const
{
    int scaledGridSize = scale(m_gridSize);
    int xOffset = (width() / 2) % scaledGridSize;
    int yOffset = (height() / 2) % scaledGridSize;
    int xSnap = unscale((((pos.x() - xOffset + 5) / scaledGridSize) * scaledGridSize) + xOffset);
    int ySnap = unscale((((pos.y() - yOffset + 5) / scaledGridSize) * scaledGridSize) + yOffset);
    return QPoint(xSnap, ySnap);
}

int PageLayoutEditor::scale(int n) const
{
    return int(double(n) * m_zoomFactor);
}

int PageLayoutEditor::unscale(int n) const
{
    return int(double(n) / m_zoomFactor);
}

QPoint PageLayoutEditor::scale(const QPoint &p) const
{
    return QPoint(scale(p.x()), scale(p.y()));
}

QPoint PageLayoutEditor::unscale(const QPoint &p) const
{
    return QPoint(unscale(p.x()), unscale(p.y()));
}

QRect PageLayoutEditor::scale(const QRect &r) const
{
    return QRect(scale(r.topLeft()), scale(r.bottomRight()));
}

QRect PageLayoutEditor::unscale(const QRect &r) const
{
    return QRect(unscale(r.topLeft()), unscale(r.bottomRight()));
}

#include "moc_PageLayoutEditor.cpp"
