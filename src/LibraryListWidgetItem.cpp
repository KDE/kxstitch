/*
 * Copyright (C) 2010-2015 by Stephen Allewell
 * steve.allewell@gmail.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#include "LibraryListWidgetItem.h"

#include <QPainter>

#include "LibraryPattern.h"
#include "Pattern.h"
#include "Renderer.h"
#include "StitchData.h"

#include "configuration.h"

#include <KDebug>


LibraryListWidgetItem::LibraryListWidgetItem(QListWidget *listWidget, LibraryPattern *libraryPattern)
    :   QListWidgetItem(listWidget)
{
    setLibraryPattern(libraryPattern);
}


void LibraryListWidgetItem::setLibraryPattern(LibraryPattern *libraryPattern)
{
    static Renderer *renderer = 0;

    if (renderer == 0) {
        renderer = new Renderer();
        renderer->setRenderStitchesAs(Configuration::EnumRenderer_RenderStitchesAs::Stitches);
        renderer->setRenderBackstitchesAs(Configuration::EnumRenderer_RenderBackstitchesAs::ColorLines);
        renderer->setRenderKnotsAs(Configuration::EnumRenderer_RenderKnotsAs::ColorBlocks);
    }

    m_libraryPattern = libraryPattern;

    StitchData &stitches = libraryPattern->pattern()->stitches();
    int cellSize = 256 / std::max(stitches.width(), stitches.height());
    QPixmap pixmap(stitches.width() * cellSize, stitches.height() * cellSize);
    pixmap.fill(Qt::white);

    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setWindow(0, 0, stitches.width(), stitches.height());

    renderer->render(&painter,
                     libraryPattern->pattern(),
                     pixmap.rect(),
                     true,
                     true,
                     true,
                     true,
                     -1);

    setIcon(QIcon(pixmap));
}


LibraryPattern *LibraryListWidgetItem::libraryPattern()
{
    return m_libraryPattern;
}
