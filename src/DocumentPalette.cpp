/********************************************************************************
 *	Copyright (C) 2010 by Stephen Allewell					*
 *	stephen@mirramar.adsl24.co.uk						*
 *										*
 *	This program is free software; you can redistribute it and/or modify	*
 *	it under the terms of the GNU General Public License as published by	*
 *	the Free Software Foundation; either version 2 of the License, or	*
 *	(at your option) any later version.					*
 ********************************************************************************/


#include "configuration.h"
#include "DocumentPalette.h"


DocumentPalette::DocumentPalette()
	:	m_currentIndex(-1),
		m_showSymbols(Configuration::palette_ShowSymbols())
{
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


DocumentFloss *DocumentPalette::currentFloss() const
{
	DocumentFloss *documentFloss = 0;

	if (m_currentIndex != -1)
	{
		documentFloss = m_documentFlosses[m_currentIndex];
	}

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
	m_schemeName = schemeName;
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
}


void DocumentPalette::setShowSymbols(bool show)
{
	m_showSymbols = show;
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

	return stream;
}


QDataStream &operator>>(QDataStream &stream, DocumentPalette &documentPalette)
{
	qint32 version;
	qint32 currentIndex;
	qint32 documentPaletteCount;

	qint32 key;
	DocumentFloss *documentFloss;

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
			// not supported
			// throw exception
			break;
	}

	return stream;
}
