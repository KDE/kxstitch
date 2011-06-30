/********************************************************************************
 *	Copyright (C) 2010 by Stephen Allewell					*
 *	stephen@mirramar.adsl24.co.uk						*
 *										*
 *	This program is free software; you can redistribute it and/or modify	*
 *	it under the terms of the GNU General Public License as published by	*
 *	the Free Software Foundation; either version 2 of the License, or	*
 *	(at your option) any later version.					*
 ********************************************************************************/


#include "LibraryListWidgetItem.h"
#include "LibraryPattern.h"


LibraryListWidgetItem::LibraryListWidgetItem(QListWidget *listWidget, LibraryPattern *libraryPattern)
	:	QListWidgetItem(listWidget)
{
	setLibraryPattern(libraryPattern);
}


void LibraryListWidgetItem::setLibraryPattern(LibraryPattern *libraryPattern)
{
#if 0
	m_libraryPattern = libraryPattern;
	setPixmap(PatternMimeData(libraryPattern->data()).encodedData("image/png"));
#endif
}


LibraryPattern *LibraryListWidgetItem::libraryPattern()
{
	return m_libraryPattern;
}


bool LibraryListWidgetItem::acceptDrop(const QMimeSource *mimeSource)
{
	return false;
}


void LibraryListWidgetItem::dropped(QDropEvent *, const QList<QIconDragItem> &)
{
}
