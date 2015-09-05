/*
 * Copyright (C) 2009-2015 by Stephen Allewell
 * steve.allewell@gmail.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#include "LibraryTreeWidget.h"

#include <QDragEnterEvent>
#include <QDragLeaveEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QTimer>
#include <QTreeWidgetItem>

#include "LibraryPattern.h"


LibraryTreeWidget::LibraryTreeWidget(QWidget *parent)
    :   QTreeWidget(parent)
{
}


void LibraryTreeWidget::contentsDragEnterEvent(QDragEnterEvent*)
{
}


void LibraryTreeWidget::contentsDragMoveEvent(QDragMoveEvent*)
{
}


void LibraryTreeWidget::contentsDragLeaveEvent(QDragLeaveEvent*)
{
}


void LibraryTreeWidget::contentsDropEvent(QDropEvent*)
{
}


bool LibraryTreeWidget::acceptDrag(QDropEvent*) const
{
    return false;
}


void LibraryTreeWidget::openBranch()
{
}
