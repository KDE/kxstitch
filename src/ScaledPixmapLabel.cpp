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


ScaledPixmapLabel::ScaledPixmapLabel(QWidget *parent)
    : QLabel(parent)
{
    setMinimumSize(1,1);
    setAlignment(Qt::AlignCenter);
}


void ScaledPixmapLabel::setPixmap(const QPixmap &pixmap)
{
    m_pixmap = pixmap;
    QLabel::setPixmap(m_pixmap.scaled(size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
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
    QSize pixmapSize  = pixmap()->size();

    int dx = (previewSize.width()  - pixmapSize.width())  / 2;
    int dy = (previewSize.height() - pixmapSize.height()) / 2;

    return QRect(dx, dy, pixmapSize.width(), pixmapSize.height());
}


void ScaledPixmapLabel::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);

    QLabel::setPixmap(m_pixmap.scaled(this->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
}
