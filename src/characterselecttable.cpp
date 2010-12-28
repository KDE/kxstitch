/****************************************************************************
 *	Copyright (C) 2006 by Stephen Allewell									*
 *	stephen@mirramar.fsnet.co.uk											*
 *																			*
 *	This program is free software; you can redistribute it and/or modify	*
 *	it under the terms of the GNU General Public License as published by	*
 *	the Free Software Foundation; either version 2 of the License, or		*
 *	(at your option) any later version.										*
 ****************************************************************************/


#include <QPainter>
#include <QStyle>

#include "charselecttable.h"
#include "configuration.h"


CharSelectTable::CharSelectTable(QWidget *parent, const char *name)
  : KCharSelectTable(parent, name, Configuration::editor_SymbolFont(), QChar(), 0)
{
	setNumCols(32);
	setCellWidth(20);
	setNumRows(8);
	setCellHeight(25);
	resize(640,200);
	move(100,100);
}


void CharSelectTable::setUsed(QMap<int,QChar> *usedSymbols)
{
	m_used = usedSymbols;
}


bool CharSelectTable::symbolUsed(QChar c)
{
	QMapIterator<int, QChar> it;
	for (it = m_used->begin() ; it != m_used->end() ; ++it)
	{
		if (it.data() == c) return true;
	}
	return false;
}


void CharSelectTable::paintCell(QPainter *p, int row, int col)
{
	/**	code copied from KCharSelectTable paintCell, but modified
		so that used symbols will be shown in red
		*/
	const int w = cellWidth();
	const int h = cellHeight();
	const int x2 = w-1;
	const int y2 = h-1;

	QFont font = QFont(vFont);
	font.setPixelSize(int(0.7*h));
	unsigned short c = vTableNum*256;
	c += row*numCols();
	c += col;
	if (c == vChr.unicode())
	{
		p->setBrush(QBrush(colorGroup().highlight()));
		p->setPen(NoPen);
		p->drawRect(0, 0, w, h);
		if (symbolUsed(QChar(c)))
			p->setPen(colorGroup().mid());
		else
			p->setPen(colorGroup().highlightedText());
		vPos = QPoint(col, row);
	}
	else
	{
		QFontMetrics fm = QFontMetrics(font);
		if(fm.inFont(c))
			p->setBrush(QBrush(colorGroup().base()));
		else
			p->setBrush(QBrush(colorGroup().button()));
		p->setPen(NoPen);
		p->drawRect(0, 0, w, h);
		if (symbolUsed(QChar(c)))
			p->setPen(red);
		else
			p->setPen(colorGroup().text());
	}
	if (c == focusItem.unicode() && hasFocus())
	{
		style().drawPrimitive(QStyle::PE_FocusRect, p, QRect(2, 2, w-4, h-4), colorGroup());
		focusPos = QPoint(col, row);
	}
	p->setFont(font);
	p->drawText(0, 0, x2, y2, AlignHCenter | AlignVCenter, QString(QChar(c)));
	p->setPen(colorGroup().text());
	p->drawLine(x2, 0, x2, y2);
	p->drawLine(0, y2, x2, y2);
	if (row == 0)
		p->drawLine(0, 0, x2, 0);
	if (col == 0)
		p->drawLine(0, 0, 0, y2);
}
