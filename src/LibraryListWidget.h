/***************************************************************************
 *   Copyright (C) 2011 by Stephen Allewell                                *
 *   stephen@mirramar.adsl24.co.uk                                          *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/


#ifndef __LibraryListWidget_H
#define __LibraryListWidget_H


#include <QListWidget>
#include <QWidget>


class QDragObject;
class QString;


class LibraryListWidget : public QListWidget
{
	public:
		LibraryListWidget(QWidget *parent);

		void setScaleSize(int scaleSize);

	protected:
		virtual QDragObject *dragObject();

	private:
		int m_scaleSize;
};


#endif
