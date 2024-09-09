/*
 * Copyright (C) 2015 by Stephen Allewell
 * steve.allewell@gmail.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

/** @file
 * This file defines an extension to a QLabel that scales the associated QPixmap
 * to maintain the aspect ratio when scaling to fill the extents of the QLabel
 * area.
 */

// Class include
#include "ScaledPixmapLabel.h"

// Qt includes
#include <QMouseEvent>
#include <QPainter>
#include <QStyleOptionRubberBand>

ScaledPixmapLabel::ScaledPixmapLabel(QWidget *parent)
    : QLabel(parent)
    , m_cropping(false)
{
    setMinimumSize(1, 1);
    setAlignment(Qt::AlignCenter);
    setMouseTracking(true);
}

void ScaledPixmapLabel::setPixmap(const QPixmap &pixmap)
{
    m_pixmap = pixmap;
    m_crop = QRect();
    QLabel::setPixmap(m_pixmap.scaled(size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

void ScaledPixmapLabel::setCropping(bool checked)
{
    m_cropping = checked;
    m_crop = QRect();
    update();
}

int ScaledPixmapLabel::heightForWidth(int width) const
{
    return ((qreal)m_pixmap.height() * width) / m_pixmap.width();
}

QSize ScaledPixmapLabel::sizeHint() const
{
    int w = width();
    return QSize(w, heightForWidth(w));
}

QRect ScaledPixmapLabel::pixmapRect() const
{
    QSize previewSize = size();
#if QT_VERSION < QT_VERSION_CHECK(5, 15, 0)
    QSize pixmapSize = pixmap()->size();
#else
    QSize pixmapSize = pixmap(Qt::ReturnByValue).size();
#endif

    int dx = (previewSize.width() - pixmapSize.width()) / 2;
    int dy = (previewSize.height() - pixmapSize.height()) / 2;

    return QRect(dx, dy, pixmapSize.width(), pixmapSize.height());
}

void ScaledPixmapLabel::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);

    QLabel::setPixmap(m_pixmap.scaled(this->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

void ScaledPixmapLabel::paintEvent(QPaintEvent *event)
{
    QLabel::paintEvent(event);

    QPainter painter(this);
    //painter.setRenderHint(QPainter::Qt4CompatiblePainting, true);

    if (m_crop.isValid()) {
        QStyleOptionRubberBand opt;
        opt.initFrom(this);
        opt.shape = QRubberBand::Rectangle;
        opt.opaque = false;
        opt.rect = m_crop.adjusted(0, 0, 1, 1);

        style()->drawControl(QStyle::CE_RubberBand, &opt, &painter);
    } else {
        if (m_cropping && underMouse() && (children().count() == 0)) {
            // draw guides
            QPoint cursor = mapFromGlobal(QCursor::pos());
            painter.drawLine(cursor.x(), 0, cursor.x(), size().height());
            painter.drawLine(0, cursor.y(), size().width(), cursor.y());
        }
    }
}

void ScaledPixmapLabel::mousePressEvent(QMouseEvent *event)
{
    if (m_cropping) {
        m_start = event->pos();
        m_crop = QRect();
        update();
    }
}

void ScaledPixmapLabel::mouseMoveEvent(QMouseEvent *event)
{
    if (m_cropping && (event->buttons() & Qt::LeftButton)) {
        m_end = event->pos();
        m_crop = pixmapRect().intersected(QRect(m_start, m_end).normalized());
    }

    update();
}

void ScaledPixmapLabel::mouseReleaseEvent(QMouseEvent *event)
{
    if (m_cropping) {
        m_end = event->pos();
        m_crop = pixmapRect().intersected(QRect(m_start, m_end).normalized());
        update();

#if QT_VERSION < QT_VERSION_CHECK(5, 15, 0)
        double scale = (double)m_pixmap.size().width() / (double)pixmap()->size().width();
#else
        double scale = (double)m_pixmap.size().width() / (double)pixmap(Qt::ReturnByValue).size().width();
#endif
        m_crop.translate(-pixmapRect().topLeft());
        QRectF cropF(scale * m_crop.x(), scale * m_crop.y(), scale * m_crop.width(), scale * m_crop.height());

        emit imageCropped(cropF);

        m_crop = QRect();
    }
}

void ScaledPixmapLabel::leaveEvent(QEvent *event)
{
    update();
    QLabel::leaveEvent(event);
}

#include "moc_ScaledPixmapLabel.cpp"
