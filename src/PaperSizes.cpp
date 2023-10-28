/*
 * Copyright (C) 2010-2015 by Stephen Allewell
 * steve.allewell@gmail.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#include "PaperSizes.h"

#include <QSizeF>


int PageSizes::width(QPageSize::PageSizeId size, QPageLayout::Orientation orientation)
{
    const QSizeF pageSize = QPageSize::size(size, QPageSize::Millimeter);
    return static_cast<int>((orientation == QPageLayout::Portrait) ? pageSize.width() : pageSize.height());
}


int PageSizes::height(QPageSize::PageSizeId size, QPageLayout::Orientation orientation)
{
    const QSizeF pageSize = QPageSize::size(size, QPageSize::Millimeter);
    return static_cast<int>((orientation == QPageLayout::Portrait) ? pageSize.height() : pageSize.width());
}
