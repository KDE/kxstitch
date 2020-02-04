/*
 * Copyright (C) 2010-2015 by Stephen Allewell
 * steve.allewell@gmail.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#ifndef PaperSizes_H
#define PaperSizes_H


#include <QPageLayout>


class PageSizes
{
public:
    static QPageSize::PageSizeId size(const QString &);
    static QString name(QPageSize::PageSizeId);
    static int width(QPageSize::PageSizeId, QPageLayout::Orientation);
    static int height(QPageSize::PageSizeId, QPageLayout::Orientation);
};


#endif // PaperSizes_H
