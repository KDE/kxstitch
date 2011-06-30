/***************************************************************************
 *   Copyright (C) 2009 by Stephen Allewell                                *
 *   stephen@mirramar.fsnet.co.uk                                          *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/


#include <QDragEnterEvent>
#include <QDragLeaveEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QTimer>
#include <QTreeWidgetItem>

#include "LibraryPattern.h"
#include "LibraryTreeWidget.h"


LibraryTreeWidget::LibraryTreeWidget(QWidget *parent)
	:	QTreeWidget()
{
}


void LibraryTreeWidget::contentsDragEnterEvent(QDragEnterEvent *event)
{
}


void LibraryTreeWidget::contentsDragMoveEvent(QDragMoveEvent *event)
{
}


void LibraryTreeWidget::contentsDragLeaveEvent(QDragLeaveEvent *event)
{
}


void LibraryTreeWidget::contentsDropEvent(QDropEvent *event)
{
}


bool LibraryTreeWidget::acceptDrag(QDropEvent *event) const
{
}


void LibraryTreeWidget::openBranch()
{
}
