/********************************************************************************
 *	Copyright (C) 2010 by Stephen Allewell					*
 *	stephen@mirramar.adsl24.co.uk						*
 *										*
 *	This program is free software; you can redistribute it and/or modify	*
 *	it under the terms of the GNU General Public License as published by	*
 *	the Free Software Foundation; either version 2 of the License, or	*
 *	(at your option) any later version.					*
 ********************************************************************************/


#include "DocumentPalette.h"

#include <KLocale>

#include "Exceptions.h"
#include "FlossScheme.h"
#include "SchemeManager.h"

#include "configuration.h"


DocumentPalette::DocumentPalette()
	:	m_currentIndex(-1),
		m_showSymbols(Configuration::palette_ShowSymbols())
{
}


DocumentPalette::DocumentPalette(const DocumentPalette &other)
{
	*this = other;
}


DocumentPalette::~DocumentPalette()
{
	clear();
}


void DocumentPalette::clear()
{
	qDeleteAll(m_documentFlosses);
	m_documentFlosses.clear();
	m_currentIndex = -1;
}


const QString &DocumentPalette::schemeName() const
{
	return m_schemeName;
}


QMap<int, DocumentFloss *> &DocumentPalette::flosses()
{
	return m_documentFlosses;
}


QVector<int> DocumentPalette::sortedFlosses() const
{
	int colors = m_documentFlosses.count();

	QVector<int> sorted(colors);
	QMapIterator<int, DocumentFloss *> iterator(m_documentFlosses);

	int i = 0;
	while (iterator.hasNext())
		sorted[i++] = iterator.next().key();

	bool exchanged;
	do
	{
		exchanged = false;
		for (i = 0 ; i < colors-1 ; ++i)
		{
			QString flossName1(m_documentFlosses[sorted[i]]->flossName());
			QString flossName2(m_documentFlosses[sorted[i+1]]->flossName());
			int length1 = flossName1.length();
			int length2 = flossName2.length();
			if (((flossName1 > flossName2) && (length1 >= length2)) || (length1 > length2))
			{
				int tmp = sorted[i];
				sorted[i] = sorted[i+1];
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
	
	QMapIterator<int, DocumentFloss *> iterator(m_documentFlosses);
	while (iterator.hasNext())
		used << iterator.next().value()->stitchSymbol();
	
	return used;
}


DocumentFloss *DocumentPalette::currentFloss() const
{
	DocumentFloss *documentFloss = 0;

	if (m_currentIndex != -1)
	{
		documentFloss = m_documentFlosses[m_currentIndex];
	}

	return documentFloss;
}


DocumentFloss *DocumentPalette::floss(int colorIndex) const
{
	DocumentFloss *documentFloss = 0;
	
	if (m_documentFlosses.contains(colorIndex))
		documentFloss = m_documentFlosses.value(colorIndex);
	
	return documentFloss;
}


int DocumentPalette::currentIndex() const
{
	return m_currentIndex;
}


bool DocumentPalette::showSymbols() const
{
	return m_showSymbols;
}


void DocumentPalette::setSchemeName(const QString &schemeName)
{
	if (m_schemeName == schemeName)
		return;
	
	m_schemeName = schemeName;

	FlossScheme *scheme = SchemeManager::scheme(m_schemeName);

	QMapIterator<int, DocumentFloss *> flossIterator(m_documentFlosses);
	while (flossIterator.hasNext())
	{
		
		int key = flossIterator.next().key();
		DocumentFloss *documentFloss = flossIterator.value();
		Floss *floss = scheme->convert(documentFloss->flossColor());
		DocumentFloss *replacement = new DocumentFloss(floss->name(), documentFloss->stitchSymbol(), documentFloss->backstitchSymbol(), documentFloss->stitchStrands(), documentFloss->backstitchStrands());
		replacement->setFlossColor(floss->color());
		delete documentFloss;
		replace(key, replacement);
	}
}


void DocumentPalette::setCurrentIndex(int currentIndex)
{
	m_currentIndex = currentIndex;
}


void DocumentPalette::add(int flossIndex, DocumentFloss *documentFloss)
{
	m_documentFlosses.insert(flossIndex, documentFloss);
	if (m_currentIndex == -1)
	{
		m_currentIndex = 0;
	}
}


int DocumentPalette::add(const QColor &srcColor)
{
	int colorIndex = -1;

	FlossScheme *scheme = SchemeManager::scheme(m_schemeName);
	Floss *floss = scheme->find(scheme->find(srcColor));
	if (floss == 0)
		floss = scheme->convert(srcColor);

	QMapIterator<int, DocumentFloss *> flossIterator(m_documentFlosses);
	while (flossIterator.hasNext() && colorIndex == -1)
	{
		flossIterator.next();
		if (flossIterator.value()->flossColor() == floss->color())
		{
			colorIndex = flossIterator.key();
		}
	}
	
	if (colorIndex == -1) // the color hasn't been found in the existing list
	{
			
		for (colorIndex = 0 ; m_documentFlosses.contains(colorIndex) ; ++colorIndex) ; // colorIndex contains a free index
		
		int c = -1;
		bool found = false;
		QChar symbol;
		while (!found)
		{
			symbol = QChar(++c);
			if (symbol.isPrint() && !symbol.isSpace() && !symbol.isPunct())
			{
				found = true;
				QMapIterator<int, DocumentFloss *> i(m_documentFlosses);
				while (i.hasNext() && found)
				{
					if (i.next().value()->stitchSymbol() == symbol)
					found = false;
				}
			}
		}

		DocumentFloss *documentFloss = new DocumentFloss(floss->name(), symbol, Qt::SolidLine, Configuration::palette_StitchStrands(), Configuration::palette_BackstitchStrands());
		documentFloss->setFlossColor(floss->color());
		add(colorIndex, documentFloss);
	}
	
	return colorIndex;
}


DocumentFloss *DocumentPalette::remove(int flossIndex)
{
	DocumentFloss documentFloss = m_documentFlosses.take(flossIndex);
	if (m_documentFlosses.count() == 0)
	{
		m_currentIndex = -1;
	}
}


DocumentFloss *DocumentPalette::replace(int flossIndex, DocumentFloss *documentFloss)
{
	DocumentFloss *old = m_documentFlosses.take(flossIndex);
	m_documentFlosses.insert(flossIndex, documentFloss);
	return old;
}


void DocumentPalette::swap(int originalIndex, int swappedIndex)
{
	DocumentFloss *original = m_documentFlosses.take(originalIndex);
	m_documentFlosses.insert(originalIndex, m_documentFlosses.take(swappedIndex));
	m_documentFlosses.insert(swappedIndex, original);
}


void DocumentPalette::setShowSymbols(bool show)
{
	m_showSymbols = show;
}


DocumentPalette &DocumentPalette::operator=(const DocumentPalette &other)
{
	m_schemeName = other.m_schemeName;
	m_currentIndex = other.m_currentIndex;
	m_documentFlosses.clear();
	foreach (int index, other.m_documentFlosses.keys())
		m_documentFlosses.insert(index, new DocumentFloss(other.m_documentFlosses.value(index)));
	m_showSymbols = other.m_showSymbols;
}


bool DocumentPalette::operator==(const DocumentPalette &other)
{
	bool equal = (	(m_schemeName == other.m_schemeName) &&
			(m_currentIndex == other.m_currentIndex) &&
			(m_showSymbols == other.m_showSymbols) &&
			(m_documentFlosses.keys() == other.m_documentFlosses.keys()) );
	
	if (equal)
	{
		foreach (int index, m_documentFlosses.keys())
		{
			if (*m_documentFlosses.value(index) != *other.m_documentFlosses.value(index))
				equal = false;
		}
	}
	
	return equal;
}


bool DocumentPalette::operator!=(const DocumentPalette &other)
{
	return !(*this == other);
}


QDataStream &operator<<(QDataStream &stream, const DocumentPalette &documentPalette)
{
	stream << qint32(documentPalette.version);
	stream << documentPalette.m_schemeName;
	stream << qint32(documentPalette.m_currentIndex);
	stream << documentPalette.m_showSymbols;
	stream << qint32(documentPalette.m_documentFlosses.count());
	QMapIterator<int, DocumentFloss *> documentFlossIterator(documentPalette.m_documentFlosses);
	while (documentFlossIterator.hasNext())
	{
		documentFlossIterator.next();
		stream << qint32(documentFlossIterator.key());
		stream << *documentFlossIterator.value();
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

	documentPalette.clear();
	
	stream >> version;
	switch (version)
	{
		case 100:
			stream >> documentPalette.m_schemeName;
			stream >> currentIndex;
			documentPalette.m_currentIndex = currentIndex;
			stream >> documentPalette.m_showSymbols;
			stream >> documentPaletteCount;
			while (documentPaletteCount--)
			{
				documentFloss = new DocumentFloss;
				stream >> key;
				stream >> *documentFloss;
				documentPalette.m_documentFlosses.insert(key, documentFloss);
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
