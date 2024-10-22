/*
 * Copyright (C) 2003-2015 by Stephen Allewell
 * steve.allewell@gmail.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

/** @file
 * This file implements an overlay to be used on top of a QLabel to allow the
 * selection of a pixel point to be used as a color to be ignored.
 */

// Class include
#include "AlphaSelect.h"

// Qt includes
#include <QMouseEvent>

AlphaSelect::AlphaSelect(ScaledPixmapLabel *parent)
    : ScaledPixmapLabel(parent)
{
    resize(parent->size());
    setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    setPixmap(parent->pixmap(Qt::ReturnByValue));
    setCursor(QCursor(Qt::CrossCursor));
}

void AlphaSelect::mouseReleaseEvent(QMouseEvent *event)
{
    if (pixmapRect().contains(event->pos())) {
        emit clicked(event->pos());
    }
}

#include "moc_AlphaSelect.cpp"
