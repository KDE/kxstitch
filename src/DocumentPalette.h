/*
 * Copyright (C) 2010 by Stephen Allewell
 * sallewell@users.sourceforge.net
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#ifndef DocumentPalette_H
#define DocumentPalette_H


#include <QDataStream>
#include <QList>
#include <QMap>
#include <QSharedDataPointer>
#include <QStringList>

#include "DocumentFloss.h"


class QString;

class DocumentPaletteData;


class DocumentPalette
{
public:
    DocumentPalette();
    DocumentPalette(const DocumentPalette &other);
    ~DocumentPalette();

    QString schemeName() const;
    QString symbolLibrary() const;
    QMap<int, DocumentFloss *> flosses() const;
    QVector<int> sortedFlosses() const;
    QList<qint16> usedSymbols() const;
    const DocumentFloss *currentFloss() const;
    DocumentFloss *floss(int);
    int currentIndex() const;

    void setSchemeName(const QString &);
    void setSymbolLibrary(const QString &);
    void setCurrentIndex(int);
    void add(int, DocumentFloss *);
    int add(const QColor &);
    DocumentFloss *remove(int);
    DocumentFloss *replace(int, DocumentFloss *);
    void swap(int, int);
    qint16 freeSymbol() const;

    DocumentPalette &operator=(const DocumentPalette &);
    bool operator==(const DocumentPalette &) const;
    bool operator!=(const DocumentPalette &) const;

    friend QDataStream &operator<<(QDataStream &, const DocumentPalette &);
    friend QDataStream &operator>>(QDataStream &, DocumentPalette &);

private:
    int freeIndex() const;

    QSharedDataPointer<DocumentPaletteData> d;
};


QDataStream &operator<<(QDataStream &, const DocumentPalette &);
QDataStream &operator>>(QDataStream &, DocumentPalette &);


#endif // DocumentPalette_H
