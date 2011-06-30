/***************************************************************************
 *   Copyright (C) 2003 by Stephen Allewell                                *
 *   stephen@mirramar.fsnet.co.uk                                          *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/


#include <QListWidget>

#include "KeycodeLineEdit.h"
#include "LibraryListWidgetItem.h"
#include "LibraryPattern.h"


LibraryPattern::LibraryPattern()
{
}


LibraryPattern::LibraryPattern(qint32 key, Qt::KeyboardModifiers modifiers, qint16 baseline, quint16 checksum, const DocumentPalette &documentPalette, const StitchData &stitchData)
	:	m_key(key),
		m_modifiers(modifiers),
		m_baseline(baseline),
		m_checksum(checksum),
		m_documentPalette(documentPalette),
		m_stitchData(stitchData),
		m_libraryListWidgetItem(0),
		m_changed(false)
{
}


qint32 LibraryPattern::key() const
{
	return m_key;
}


Qt::KeyboardModifiers LibraryPattern::modifiers() const
{
	return m_modifiers;
}


qint16 LibraryPattern::baseline() const
{
	return m_baseline;
}


quint16 LibraryPattern::checksum() const
{
	return m_checksum;
}


const DocumentPalette &LibraryPattern::documentPalette() const
{
	return m_documentPalette;
}


const StitchData &LibraryPattern::stitchData() const
{
	return m_stitchData;
}


LibraryListWidgetItem *LibraryPattern::libraryListWidgetItem() const
{
	return m_libraryListWidgetItem;
}


bool LibraryPattern::hasChanged() const
{
	return m_changed;
}


void LibraryPattern::setKeyModifiers(qint32 key, Qt::KeyboardModifiers modifiers)
{
	m_key = key;
	m_modifiers = modifiers;
	m_libraryListWidgetItem->setText(KeycodeLineEdit::keyString(key, modifiers));
	m_changed = true;
}


void LibraryPattern::setBaseline(qint16 baseline)
{
	m_baseline = baseline;
	m_changed = true;
}


void LibraryPattern::setLibraryListWidgetItem(LibraryListWidgetItem *libraryListWidgetItem)
{
	m_libraryListWidgetItem = libraryListWidgetItem;
	libraryListWidgetItem->setText(KeycodeLineEdit::keyString(m_key, m_modifiers));
}


QDataStream &operator<<(QDataStream &stream, const LibraryPattern &libraryPattern)
{
	stream << libraryPattern.version;
	stream << libraryPattern.m_key;
	stream << qint32(libraryPattern.m_modifiers);
	stream << libraryPattern.m_baseline;
	stream << libraryPattern.m_checksum;
	stream << libraryPattern.m_documentPalette;
	stream << libraryPattern.m_stitchData;
}


QDataStream &operator>>(QDataStream &stream, LibraryPattern &libraryPattern)
{
	qint32 version;
	qint32 modifiers;

	stream >> version;
	switch (version)
	{
		case 100:
			stream >> libraryPattern.m_key;
			stream >> modifiers;
			libraryPattern.m_modifiers = Qt::KeyboardModifiers(modifiers);
			stream >> libraryPattern.m_baseline;
			stream >> libraryPattern.m_checksum;
			stream >> libraryPattern.m_documentPalette;
			stream >> libraryPattern.m_stitchData;
			libraryPattern.m_libraryListWidgetItem = 0;
			libraryPattern.m_changed = false;
			break;

		default:
			// not supported
			// throw exception
			break;

	}

	return stream;
}
