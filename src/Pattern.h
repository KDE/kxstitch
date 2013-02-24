/*
 * Copyright (C) 2012 by Stephen Allewell
 * stephen@mirramar.adsl24.co.uk
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#ifndef Pattern_H
#define Pattern_H


#include "DocumentPalette.h"
#include "StitchData.h"


class Document;


class Pattern
{
public:
    Pattern(Document *document = 0);
    ~Pattern();

    void clear();

    Document *document();
    DocumentPalette &palette();
    StitchData &stitches();

    Pattern *cut(const QRect &area, int colorMask, const QList<Stitch::Type> &stitchMask, bool excludeBackstitches, bool excludeKnots);
    Pattern *copy(const QRect &area, int colorMask, const QList<Stitch::Type> &stitchMask, bool excludeBackstitches, bool excludeKnots);
    void paste(Pattern *pattern, const QPoint &cell, bool merge);

    friend QDataStream &operator<<(QDataStream &stream, const Pattern &pattern);
    friend QDataStream &operator>>(QDataStream &stream, Pattern &pattern);

private:
    static const int version = 100;

    void constructPalette(Pattern *pattern);

    Document        *m_document;
    DocumentPalette m_documentPalette;
    StitchData      m_stitchData;
};


QDataStream &operator<<(QDataStream &, const Pattern &);
QDataStream &operator>>(QDataStream &, Pattern &);


#endif // Pattern_H
