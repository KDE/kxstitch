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

QVector<QPageSize::PageSizeId> PageSizes::sizesForPrint()
{
    static QVector<QPageSize::PageSizeId> sizes{
        QPageSize::A0,        QPageSize::A1,    QPageSize::A2,     QPageSize::A3,    QPageSize::A4,     QPageSize::A5,      QPageSize::A6,      QPageSize::A7,
        QPageSize::A8,        QPageSize::A9,    QPageSize::B0,     QPageSize::B1,    QPageSize::B2,     QPageSize::B3,      QPageSize::B4,      QPageSize::B5,
        QPageSize::B6,        QPageSize::B7,    QPageSize::B8,     QPageSize::B9,    QPageSize::B10,    QPageSize::C5E,     QPageSize::Comm10E, QPageSize::DLE,
        QPageSize::Executive, QPageSize::Folio, QPageSize::Ledger, QPageSize::Legal, QPageSize::Letter, QPageSize::Tabloid,
    };
    return sizes;
}

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
