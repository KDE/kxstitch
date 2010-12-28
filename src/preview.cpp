/****************************************************************************
 *	Copyright (C) 2010 by Stephen Allewell									*
 *	stephen@mirramar.adsl24.co.uk											*
 *																			*
 *	This program is free software; you can redistribute it and/or modify	*
 *	it under the terms of the GNU General Public License as published by	*
 *	the Free Software Foundation; either version 2 of the License, or		*
 *	(at your option) any later version.										*
 ****************************************************************************/


#include <KDebug>

#include "preview.h"
#include "document.h"


Preview::Preview() : QScrollArea()
{
	setObjectName("Preview#");
	m_document = 0;
	loadSettings();
}


Preview::~Preview()
{
}


QSize Preview::sizeHint() const
{
	return QSize(400, 400);
}


void Preview::setDocument(Document *document)
{
	m_document = document;
}


void Preview::loadSettings()
{
}


void Preview::setVisibleArea(QRect visibleArea)
{
}


void Preview::contentsMousePressEvent(QMouseEvent *e)
{
}


void Preview::contentsMouseMoveEvent(QMouseEvent *e)
{
}


void Preview::contentsMouseReleaseEvent(QMouseEvent *e)
{
}


void Preview::paintEvent(QPaintEvent *e)
{
}
