/***************************************************************************
 *   Copyright (C) 2009 by Stephen Allewell                                *
 *   stephen@mirramar.fsnet.co.uk                                          *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/


#ifndef LibraryListWidgetItem_H
#define LibraryListWidgetItem_H


#include <QListWidgetItem>


class LibraryPattern;
class QListWidget;


class LibraryListWidgetItem : public QListWidgetItem
{
	public:
		LibraryListWidgetItem(QListWidget *listWidget, LibraryPattern *libraryPattern);

		void setLibraryPattern(LibraryPattern *libraryPattern);
		LibraryPattern *libraryPattern();

	private:
		LibraryPattern *m_libraryPattern;
};


#endif // LibraryListWidgetItem_H
