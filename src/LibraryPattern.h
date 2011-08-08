/***************************************************************************
 *   Copyright (C) 2003 by Stephen Allewell                                *
 *   stephen@mirramar.fsnet.co.uk                                          *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/


#ifndef LibraryPattern_H
#define LibraryPattern_H


#include <QByteArray>
#include <QDataStream>
#include <QString>

#include "DocumentPalette.h"
#include "StitchData.h"


class LibraryListWidgetItem;


class LibraryPattern
{
	public:
		LibraryPattern();
		LibraryPattern(qint32, Qt::KeyboardModifiers, qint16, quint16, const DocumentPalette &, const StitchData &);

		qint32 key() const;
		Qt::KeyboardModifiers modifiers() const;
		qint16 baseline() const;
		quint16 checksum() const;
		const DocumentPalette &documentPalette() const;
		const StitchData &stitchData() const;
		LibraryListWidgetItem *libraryListWidgetItem() const;
		bool hasChanged() const;

		void setKeyModifiers(qint32, Qt::KeyboardModifiers);
		void setBaseline(qint16);
		void setLibraryListWidgetItem(LibraryListWidgetItem *);

		friend QDataStream &operator<<(QDataStream &, const LibraryPattern &);
		friend QDataStream &operator>>(QDataStream &, LibraryPattern &);

	private:
		static const int version = 100;

		qint32			m_key;
		Qt::KeyboardModifiers	m_modifiers;
		qint16			m_baseline;
		quint16			m_checksum;
		DocumentPalette		m_documentPalette;
		StitchData		m_stitchData;
		LibraryListWidgetItem	*m_libraryListWidgetItem;
		bool			m_changed;
};


QDataStream &operator<<(QDataStream &, const LibraryPattern &);
QDataStream &operator>>(QDataStream &, LibraryPattern &);


#endif // LibraryPattern_H
