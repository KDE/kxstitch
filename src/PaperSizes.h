/*
 * Copyright (C) 2010-2014 by Stephen Allewell
 * steve.allewell@gmail.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#ifndef PaperSizes_H
#define PaperSizes_H


#include <QPrinter>


class PaperSizes
{
public:
    static QPrinter::PaperSize size(const QString &);
    static QString name(QPrinter::PaperSize);
    static int width(QPrinter::PaperSize, QPrinter::Orientation);
    static int height(QPrinter::PaperSize, QPrinter::Orientation);
};


#endif // PaperSizes_H
