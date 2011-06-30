/********************************************************************************
 *	Copyright (C) 2010 by Stephen Allewell					*
 *	stephen@mirramar.adsl24.co.uk						*
 *										*
 *	This program is free software; you can redistribute it and/or modify	*
 *	it under the terms of the GNU General Public License as published by	*
 *	the Free Software Foundation; either version 2 of the License, or	*
 *	(at your option) any later version.					*
 ********************************************************************************/


#include "LibraryListWidget.h"
#include "LibraryListWidgetItem.h"
#include "PatternMimeData.h"


LibraryListWidget::LibraryListWidget(QWidget *parent)
	:	QListWidget(parent)
{
}


void LibraryListWidget::setScaleSize(int scaleSize)
{
	m_scaleSize = scaleSize;
}


QDragObject *LibraryListWidget::dragObject()
{
#if 0
	PatternMimeData *dragObject = new PatternMimeData(dynamic_cast<LibraryListWidgetItem>(currentItem()->data(), this);
	dragObject->setScale(m_scaleSize, m_scaleSize);
	return dragObject;
#endif
}
