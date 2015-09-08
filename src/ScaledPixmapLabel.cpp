/*
 * Copyright (C) 2015 by Stephen Allewell
 * steve.allewell@gmail.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


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


void ScaledPixmapLabel::resizeEvent(QResizeEvent *)
{
    QLabel::setPixmap(m_pixmap.scaled(this->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
}
