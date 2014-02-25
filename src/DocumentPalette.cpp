/*
 * Copyright (C) 2010 by Stephen Allewell
 * sallewell@users.sourceforge.net
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#include "DocumentPalette.h"

#include <KLocale>
#include <KMessageBox>

#include "Exceptions.h"
#include "FlossScheme.h"
#include "SchemeManager.h"
#include "SymbolLibrary.h"
#include "SymbolManager.h"

#include "configuration.h"


class DocumentPaletteData : public QSharedData
{
public:
    DocumentPaletteData();
    DocumentPaletteData(const DocumentPaletteData &);
    ~DocumentPaletteData();

    static const int version = 102; // remove m_showSymbols

    QString                     m_schemeName;
    int                         m_currentIndex;
    QMap<int, DocumentFloss *>  m_documentFlosses;
};


DocumentPaletteData::DocumentPaletteData()
    :   QSharedData(),
        m_schemeName(Configuration::palette_DefaultScheme()),
        m_currentIndex(-1)
{
}


DocumentPaletteData::DocumentPaletteData(const DocumentPaletteData &other)
    :   QSharedData(other),
        m_schemeName(other.m_schemeName),
        m_currentIndex(other.m_currentIndex)
{
    for (QMap<int, DocumentFloss*>::const_iterator i = other.m_documentFlosses.constBegin() ; i != other.m_documentFlosses.constEnd() ; ++i) {
        m_documentFlosses.insert(i.key(), new DocumentFloss(other.m_documentFlosses.value(i.key())));
    }
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

    do {
        exchanged = false;

        for (int i = 0 ; i < colors - 1 ; ++i) {
            QString flossName1(d->m_documentFlosses.value(sorted[i])->flossName());
            QString flossName2(d->m_documentFlosses.value(sorted[i + 1])->flossName());
            int length1 = flossName1.length();
            int length2 = flossName2.length();

            if (((flossName1 > flossName2) && (length1 >= length2)) || (length1 > length2)) {
                int tmp = sorted.value(i);
                sorted[i] = sorted.value(i + 1);
                sorted[i + 1] = tmp;
                exchanged = true;
            }
        }
    } while (exchanged);

    return sorted;
}


QList<qint16> DocumentPalette::usedSymbols() const
{
    QList<qint16> used;
    QList<int> keys = d->m_documentFlosses.keys();

    foreach (int index, keys) {
        used << d->m_documentFlosses[index]->stitchSymbol();
    }

    return used;
}


const DocumentFloss *DocumentPalette::currentFloss() const
{
    DocumentFloss *documentFloss = 0;

    if (d->m_currentIndex != -1) {
        documentFloss = d->m_documentFlosses.value(d->m_currentIndex);
    }

    return documentFloss;
}


DocumentFloss *DocumentPalette::floss(int colorIndex)
{
    DocumentFloss *documentFloss = 0;

    if (d->m_documentFlosses.contains(colorIndex)) {
        documentFloss = d->m_documentFlosses.value(colorIndex);
    }

    return documentFloss;
}


int DocumentPalette::currentIndex() const
{
    return d->m_currentIndex;
}


void DocumentPalette::setSchemeName(const QString &schemeName)
{
    if (d->m_schemeName == schemeName) {
        return;
    }

    d->m_schemeName = schemeName;

    FlossScheme *scheme = SchemeManager::scheme(d->m_schemeName);

    for (QMap<int, DocumentFloss*>::const_iterator i = d->m_documentFlosses.constBegin() ; i != d->m_documentFlosses.constEnd() ; ++i) {
        DocumentFloss *documentFloss = i.value();
        Floss *floss = scheme->convert(documentFloss->flossColor());
        DocumentFloss *replacement = new DocumentFloss(floss->name(), documentFloss->stitchSymbol(), documentFloss->backstitchSymbol(), documentFloss->stitchStrands(), documentFloss->backstitchStrands());
        replacement->setFlossColor(floss->color());
        delete documentFloss;
        replace(i.key(), replacement);
    }
}


void DocumentPalette::setCurrentIndex(int currentIndex)
{
    d->m_currentIndex = currentIndex;
}


void DocumentPalette::add(int flossIndex, DocumentFloss *documentFloss)
{
    d->m_documentFlosses.insert(flossIndex, documentFloss);

    if (d->m_currentIndex == -1) {
        d->m_currentIndex = 0;
    }
}


int DocumentPalette::add(const QColor &srcColor)
{
    int colorIndex = -1;

    FlossScheme *scheme = SchemeManager::scheme(d->m_schemeName);
    Floss *floss = scheme->find(scheme->find(srcColor));

    if (floss == 0) {
        floss = scheme->convert(srcColor);
    }

    for (QMap<int, DocumentFloss*>::const_iterator i = d->m_documentFlosses.constBegin() ; i != d->m_documentFlosses.constEnd() ; ++i) {
        if (i.value()->flossColor() == floss->color()) {
            colorIndex = i.key();
        }
    }

    if (colorIndex == -1) { // the color hasn't been found in the existing list
        colorIndex = freeIndex();
        DocumentFloss *documentFloss = new DocumentFloss(floss->name(), freeSymbol(), Qt::SolidLine, Configuration::palette_StitchStrands(), Configuration::palette_BackstitchStrands());
        documentFloss->setFlossColor(floss->color());
        add(colorIndex, documentFloss);
    }

    return colorIndex;
}


DocumentFloss *DocumentPalette::remove(int flossIndex)
{
    DocumentFloss *documentFloss = d->m_documentFlosses.take(flossIndex);

    if (d->m_documentFlosses.count() == 0) {
        d->m_currentIndex = -1;
    }

    return documentFloss;
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


DocumentPalette &DocumentPalette::operator=(const DocumentPalette &other)
{
    d = other.d;
    return *this;
}


bool DocumentPalette::operator==(const DocumentPalette &other) const
{
    return d == other.d;
}


bool DocumentPalette::operator!=(const DocumentPalette &other) const
{
    return d != other.d;
}


QDataStream &operator<<(QDataStream &stream, const DocumentPalette &documentPalette)
{
    stream << qint32(DocumentPaletteData::version);
    stream << documentPalette.d->m_schemeName;
    stream << qint32(documentPalette.d->m_currentIndex);
    stream << qint32(documentPalette.d->m_documentFlosses.count());

    for (QMap<int, DocumentFloss*>::const_iterator i = documentPalette.d->m_documentFlosses.constBegin() ; i != documentPalette.d->m_documentFlosses.constEnd() ; ++i) {
        stream << qint32(i.key());
        stream << *i.value();
    }

    if (stream.status() != QDataStream::Ok) {
        throw FailedWriteFile(stream.status());
    }

    return stream;
}


QDataStream &operator>>(QDataStream &stream, DocumentPalette &documentPalette)
{
    qint32 version;
    qint32 currentIndex;
    qint32 documentPaletteCount;
    bool showSymbols;

    qint32 key;
    DocumentFloss *documentFloss;

    documentPalette = DocumentPalette();

    stream >> version;

    switch (version) {
    case 102:
        stream >> documentPalette.d->m_schemeName;
        stream >> currentIndex;
        documentPalette.d->m_currentIndex = currentIndex;
        stream >> documentPaletteCount;

        while (documentPaletteCount--) {
            documentFloss = new DocumentFloss;
            stream >> key;
            stream >> *documentFloss;
            documentPalette.d->m_documentFlosses.insert(key, documentFloss);
        }

        break;

    case 101:
        stream >> documentPalette.d->m_schemeName;
        stream >> currentIndex;
        documentPalette.d->m_currentIndex = currentIndex;
        stream >> showSymbols;
        stream >> documentPaletteCount;

        while (documentPaletteCount--) {
            documentFloss = new DocumentFloss;
            stream >> key;
            stream >> *documentFloss;
            documentPalette.d->m_documentFlosses.insert(key, documentFloss);
        }

        break;

    case 100:
        stream >> documentPalette.d->m_schemeName;
        stream >> currentIndex;
        documentPalette.d->m_currentIndex = currentIndex;
        stream >> showSymbols;
        stream >> documentPaletteCount;

        while (documentPaletteCount--) {
            documentFloss = new DocumentFloss;
            stream >> key;
            stream >> *documentFloss;
            documentFloss->setStitchSymbol(documentPalette.freeSymbol());
            documentPalette.d->m_documentFlosses.insert(key, documentFloss);
        }

        break;

    default:
        throw InvalidFileVersion(QString(i18n("Palette version %1", version)));
        break;
    }

    if (stream.status() != QDataStream::Ok) {
        throw FailedReadFile(QString(i18n("Failed reading palette")));
    }

    // test DocumentFloss symbol indexes exist in the library
    QList<qint16> indexes = SymbolManager::library("kxstitch")->indexes();
    QList<DocumentFloss *> missingSymbols;
    QList<int> keys = documentPalette.d->m_documentFlosses.keys();

    foreach (int key, keys) {
        documentFloss = documentPalette.d->m_documentFlosses.value(key);
        qint16 symbol = documentFloss->stitchSymbol();

        if (indexes.contains(symbol)) {
            indexes.removeOne(symbol);
        } else {
            missingSymbols.append(documentFloss);
        }
    }

    // missingSymbols will contain pointers to DocumentFloss where the symbol index is not in the symbol library
    // iterate the list and allocate a free symbol to the missing ones.
    foreach (DocumentFloss *const documentFloss, missingSymbols) {
        documentFloss->setStitchSymbol(documentPalette.freeSymbol());
    }

    if (int count = missingSymbols.count()) {
        // display an information box to show symbols have been allocated
        QString information(i18np("The following floss color has had its symbol\nreplaced because it didn't exist in the symbol library.\n\n",
                                  "The following floss colors have had their symbols\nreplaced because they didn't exist in the symbol library.\n\n",
                                  count));

        foreach (DocumentFloss *documentFloss, missingSymbols) {
            information += QString("%1\n").arg(documentFloss->flossName());
        }

        information += QString(i18np("\nYou may want to check this is suitable.",
                                     "\nYou may want to check these are suitable.",
                                     count));

        KMessageBox::information(0, information);
    }

    return stream;
}


int DocumentPalette::freeIndex() const
{
    int i = 0;

    while (d->m_documentFlosses.contains(i)) {
        ++i;
    }

    return i;
}


qint16 DocumentPalette::freeSymbol() const
{
    QList<qint16> indexes = SymbolManager::library("kxstitch")->indexes();
    QList<int> keys = d->m_documentFlosses.keys();

    foreach (int index, keys) {
        indexes.removeOne(d->m_documentFlosses[index]->stitchSymbol());
    }

    return indexes.first();
}
