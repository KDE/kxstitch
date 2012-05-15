/********************************************************************************
 * Copyright (C) 2010 by Stephen Allewell                                       *
 * stephen@mirramar.adsl24.co.uk                                                *
 *                                                                              *
 * This program is free software; you can redistribute it and/or modify         *
 * it under the terms of the GNU General Public License as published by         *
 * the Free Software Foundation; either version 2 of the License, or            *
 * (at your option) any later version.                                          *
 ********************************************************************************/


#include "DocumentPalette.h"

#include <KLocale>

#include "Exceptions.h"
#include "FlossScheme.h"
#include "SchemeManager.h"

#include "configuration.h"


class DocumentPaletteData : public QSharedData
{
public:
    DocumentPaletteData();
    DocumentPaletteData(const DocumentPaletteData &);
    ~DocumentPaletteData();

    static const int version = 100;

    QString                     m_schemeName;
    int                         m_currentIndex;
    bool                        m_showSymbols;
    QMap<int, DocumentFloss *>  m_documentFlosses;
};


DocumentPaletteData::DocumentPaletteData()
    :   QSharedData(),
        m_schemeName(Configuration::palette_DefaultScheme()),
        m_currentIndex(-1),
        m_showSymbols(Configuration::palette_ShowSymbols())
{
}


DocumentPaletteData::DocumentPaletteData(const DocumentPaletteData &other)
    :   QSharedData(other),
        m_schemeName(other.m_schemeName),
        m_currentIndex(other.m_currentIndex),
        m_showSymbols(other.m_showSymbols)
{
    foreach (int index, other.m_documentFlosses.keys())
        m_documentFlosses.insert(index, new DocumentFloss(other.m_documentFlosses.value(index)));
}


DocumentPaletteData::~DocumentPaletteData()
{
    qDeleteAll(m_documentFlosses);
}


DocumentPalette::DocumentPalette()
    :   d(new DocumentPaletteData)
{
}


DocumentPalette::DocumentPalette(const DocumentPalette &other)
    :   d(other.d)
{
}


DocumentPalette::~DocumentPalette()
{
}


QString DocumentPalette::schemeName() const
{
    return d->m_schemeName;
}


QMap<int, DocumentFloss *> DocumentPalette::flosses() const
{
    return d->m_documentFlosses;
}


QVector<int> DocumentPalette::sortedFlosses() const
{
    int colors = d->m_documentFlosses.count();
    QVector<int> sorted = d->m_documentFlosses.keys().toVector();

    bool exchanged;
    do
    {
        exchanged = false;
        for (int i = 0 ; i < colors-1 ; ++i)
        {
            QString flossName1(d->m_documentFlosses.value(sorted[i])->flossName());
            QString flossName2(d->m_documentFlosses.value(sorted[i+1])->flossName());
            int length1 = flossName1.length();
            int length2 = flossName2.length();
            if (((flossName1 > flossName2) && (length1 >= length2)) || (length1 > length2))
            {
                int tmp = sorted.value(i);
                sorted[i] = sorted.value(i+1);
                sorted[i+1] = tmp;
                exchanged = true;
            }
        }
    } while (exchanged);

    return sorted;
}


QList<QChar> DocumentPalette::usedSymbols() const
{
    QList<QChar> used;

    foreach (int index, d->m_documentFlosses.keys())
        used << d->m_documentFlosses.value(index)->stitchSymbol();

    return used;
}


const DocumentFloss *DocumentPalette::currentFloss() const
{
    DocumentFloss *documentFloss = 0;

    if (d->m_currentIndex != -1)
        documentFloss = d->m_documentFlosses.value(d->m_currentIndex);

    return documentFloss;
}


DocumentFloss *DocumentPalette::floss(int colorIndex)
{
    DocumentFloss *documentFloss = 0;

    if (d->m_documentFlosses.contains(colorIndex))
        documentFloss = d->m_documentFlosses.value(colorIndex);

    return documentFloss;
}


int DocumentPalette::currentIndex() const
{
    return d->m_currentIndex;
}


bool DocumentPalette::showSymbols() const
{
    return d->m_showSymbols;
}


void DocumentPalette::setSchemeName(const QString &schemeName)
{
    if (d->m_schemeName == schemeName)
        return;

    d->m_schemeName = schemeName;

    FlossScheme *scheme = SchemeManager::scheme(d->m_schemeName);

    foreach (int index, d->m_documentFlosses.keys())
    {
        DocumentFloss *documentFloss = d->m_documentFlosses.value(index);
        Floss *floss = scheme->convert(documentFloss->flossColor());
        DocumentFloss *replacement = new DocumentFloss(floss->name(), documentFloss->stitchSymbol(), documentFloss->backstitchSymbol(), documentFloss->stitchStrands(), documentFloss->backstitchStrands());
        replacement->setFlossColor(floss->color());
        delete documentFloss;
        replace(index, replacement);
    }
}


void DocumentPalette::setCurrentIndex(int currentIndex)
{
    d->m_currentIndex = currentIndex;
}


void DocumentPalette::add(int flossIndex, DocumentFloss *documentFloss)
{
    d->m_documentFlosses.insert(flossIndex, documentFloss);
    if (d->m_currentIndex == -1)
    {
        d->m_currentIndex = 0;
    }
}


int DocumentPalette::add(const QColor &srcColor)
{
    int colorIndex = -1;

    FlossScheme *scheme = SchemeManager::scheme(d->m_schemeName);
    Floss *floss = scheme->find(scheme->find(srcColor));
    if (floss == 0)
        floss = scheme->convert(srcColor);

    foreach (int index, d->m_documentFlosses.keys())
    {
        if (d->m_documentFlosses.value(index)->flossColor() == floss->color())
            colorIndex = index;
    }

    if (colorIndex == -1) // the color hasn't been found in the existing list
    {
        colorIndex = freeIndex();
        DocumentFloss *documentFloss = new DocumentFloss(floss->name(), freeSymbol(), Qt::SolidLine, Configuration::palette_StitchStrands(), Configuration::palette_BackstitchStrands());
        documentFloss->setFlossColor(floss->color());
        add(colorIndex, documentFloss);
    }

    return colorIndex;
}


DocumentFloss *DocumentPalette::remove(int flossIndex)
{
    DocumentFloss documentFloss = d->m_documentFlosses.take(flossIndex);
    if (d->m_documentFlosses.count() == 0)
    {
        d->m_currentIndex = -1;
    }
}


DocumentFloss *DocumentPalette::replace(int flossIndex, DocumentFloss *documentFloss)
{
    DocumentFloss *old = d->m_documentFlosses.take(flossIndex);
    d->m_documentFlosses.insert(flossIndex, documentFloss);
    return old;
}


void DocumentPalette::swap(int originalIndex, int swappedIndex)
{
    DocumentFloss *original = d->m_documentFlosses.take(originalIndex);
    d->m_documentFlosses.insert(originalIndex, d->m_documentFlosses.take(swappedIndex));
    d->m_documentFlosses.insert(swappedIndex, original);
}


void DocumentPalette::setShowSymbols(bool show)
{
    d->m_showSymbols = show;
}


DocumentPalette &DocumentPalette::operator=(const DocumentPalette &other)
{
    d = other.d;
    return *this;
}


bool DocumentPalette::operator==(const DocumentPalette &other)
{
    return d == other.d;
}


bool DocumentPalette::operator!=(const DocumentPalette &other)
{
    return d != other.d;
}


QDataStream &operator<<(QDataStream &stream, const DocumentPalette &documentPalette)
{
    stream << qint32(DocumentPaletteData::version);
    stream << documentPalette.d->m_schemeName;
    stream << qint32(documentPalette.d->m_currentIndex);
    stream << documentPalette.d->m_showSymbols;
    stream << qint32(documentPalette.d->m_documentFlosses.count());
    foreach (int index, documentPalette.d->m_documentFlosses.keys())
    {
        stream << qint32(index);
        stream << *documentPalette.d->m_documentFlosses.value(index);
    }

    if (stream.status() != QDataStream::Ok)
        throw FailedWriteFile();

    return stream;
}


QDataStream &operator>>(QDataStream &stream, DocumentPalette &documentPalette)
{
    qint32 version;
    qint32 currentIndex;
    qint32 documentPaletteCount;

    qint32 key;
    DocumentFloss *documentFloss;

    documentPalette = DocumentPalette();

    stream >> version;
    switch (version)
    {
        case 100:
            stream >> documentPalette.d->m_schemeName;
            stream >> currentIndex;
            documentPalette.d->m_currentIndex = currentIndex;
            stream >> documentPalette.d->m_showSymbols;
            stream >> documentPaletteCount;
            while (documentPaletteCount--)
            {
                documentFloss = new DocumentFloss;
                stream >> key;
                stream >> *documentFloss;
                documentPalette.d->m_documentFlosses.insert(key, documentFloss);
            }
            break;

        default:
            throw InvalidFileVersion(QString(i18n("Palette version %1", version)));
            break;
    }

    if (stream.status() != QDataStream::Ok)
        throw FailedReadFile(QString(i18n("Failed reading palette")));

    return stream;
}


int DocumentPalette::freeIndex() const
{
    int i = 0;
    while (d->m_documentFlosses.contains(i))
        ++i;
    return i;
}


QChar DocumentPalette::freeSymbol() const
{
    QList<QChar> used = usedSymbols();

    int c = -1;
    bool found = false;
    QChar symbol;
    while (!found)
    {
        symbol = QChar(++c);
        if (symbol.isPrint() && !symbol.isSpace() && !symbol.isPunct())
            found = !used.contains(symbol);
    }

    return symbol;
}
