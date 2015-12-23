/*
 * Copyright (C) 2010-2015 by Stephen Allewell
 * steve.allewell@gmail.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#include "LibraryListWidget.h"

#include <QApplication>
#include <QBitmap>
#include <QDrag>
#include <QMimeData>
#include <QMouseEvent>
#include <QPainter>

#include "LibraryListWidgetItem.h"
#include "LibraryPattern.h"
#include "Pattern.h"


LibraryListWidget::LibraryListWidget(QWidget *parent)
    :   QListWidget(parent)
{
    m_renderer.setRenderStitchesAs(Configuration::EnumRenderer_RenderStitchesAs::Stitches);
    m_renderer.setRenderBackstitchesAs(Configuration::EnumRenderer_RenderBackstitchesAs::ColorLines);
    m_renderer.setRenderKnotsAs(Configuration::EnumRenderer_RenderKnotsAs::ColorBlocks);
}


LibraryListWidget::~LibraryListWidget()
{
}


void LibraryListWidget::setCellSize(double cellWidth, double cellHeight)
{
    m_cellWidth = cellWidth;
    m_cellHeight = cellHeight;
}


void LibraryListWidget::changeIconSize(int size)
{
    setIconSize(QSize(size, size));
    setGridSize(QSize(size + 10, size + 20));
}


void LibraryListWidget::dragEnterEvent(QDragEnterEvent*)
{
}


void LibraryListWidget::dragMoveEvent(QDragMoveEvent*)
{
}


void LibraryListWidget::dragLeaveEvent(QDragLeaveEvent*)
{
}


void LibraryListWidget::mousePressEvent(QMouseEvent *e)
{
    m_startDrag = e->pos();
    LibraryListWidgetItem *item = static_cast<LibraryListWidgetItem *>(itemAt(m_startDrag));

    if (item && (e->button() == Qt::LeftButton)) {
        e->accept();
    } else {
        e->ignore();
    }
}


void LibraryListWidget::mouseMoveEvent(QMouseEvent *e)
{
    if ((e->pos() - m_startDrag).manhattanLength() > QApplication::startDragDistance()) {
        LibraryListWidgetItem *item = static_cast<LibraryListWidgetItem *>(itemAt(m_startDrag));

        if (item && (e->buttons() & Qt::LeftButton)) {
            QByteArray data;
            QDataStream stream(&data, QIODevice::WriteOnly);
            Pattern *pattern = item->libraryPattern()->pattern();
            stream << *pattern;
            QMimeData *mimeData = new QMimeData();
            mimeData->setData(QStringLiteral("application/kxstitch"), data);

            QPixmap pixmap(pattern->stitches().width()*m_cellWidth, pattern->stitches().height()*m_cellHeight);
            pixmap.fill(Qt::white);

            QPainter painter(&pixmap);
            painter.setWindow(0, 0, pattern->stitches().width(), pattern->stitches().height());
            painter.setRenderHint(QPainter::Antialiasing, true);

            m_renderer.render(&painter,
                               pattern,
                               painter.window(),
                               false,      // render grid
                               true,       // render stitches
                               true,       // render backstitches
                               true,       // render knots
                               -1);        // no color mask

            painter.end();
            pixmap.setMask(pixmap.createMaskFromColor(Qt::white));

            QDrag *drag = new QDrag(this);
            drag->setMimeData(mimeData);
            drag->setPixmap(pixmap);
            drag->setHotSpot(QPoint(m_cellWidth / 2, m_cellHeight / 2));
            drag->exec(Qt::CopyAction);

            e->accept();
        }
    } else {
        e->ignore();
    }
}
