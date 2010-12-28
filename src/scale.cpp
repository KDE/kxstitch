/****************************************************************************
 *	Copyright (C) 2010 by Stephen Allewell									*
 *	stephen@mirramar.adsl24.co.uk											*
 *																			*
 *	This program is free software; you can redistribute it and/or modify	*
 *	it under the terms of the GNU General Public License as published by	*
 *	the Free Software Foundation; either version 2 of the License, or		*
 *	(at your option) any later version.										*
 ****************************************************************************/


#include <QScrollBar>
#include <QPainter>
#include <KDebug>

#include "configuration.h"
#include "scale.h"


Scale::Scale(Qt::Orientation orientation) : QWidget()
{
	setContextMenuPolicy(Qt::ActionsContextMenu);

	if (orientation == Qt::Horizontal)
	{
		setContentsMargins(3, 0, 0, 0);
		setMinimumHeight(Configuration::editor_HorizontalRulerHeight());
	}
	else
	{
		setContentsMargins(0, 3, 0, 0);
		setMinimumWidth(Configuration::editor_VerticalRulerWidth());
	}

	m_orientation = orientation;
	m_units = Configuration::editor_FormatScalesAs();
	m_cellSize = (m_orientation == Qt::Horizontal) ? Configuration::editor_CellWidth() : Configuration::editor_CellHeight();
	m_cellCount = (m_orientation == Qt::Horizontal) ? Configuration::document_Width() : Configuration::document_Height();
	m_offset = 0;
}


Scale::~Scale()
{
}


void Scale::setUnits(Configuration::EnumEditor_FormatScalesAs::type units)
{
	m_units = units;
	update();
}


void Scale::setCellSize(int cellSize)
{
	m_cellSize = cellSize;
	update();
}


void Scale::setCellGrouping(int cellGrouping)
{
	m_cellGrouping = cellGrouping;
	update();
}


void Scale::setCellCount(int cellCount)
{
	m_cellCount = cellCount;
	update();
}


void Scale::setClothCount(double clothCount)
{
	m_clothCount = clothCount;
	update();
}


void Scale::setOffset(int offset)
{
	m_offset = offset;
	update();
}


QSize Scale::sizeHint() const
{
	if (m_orientation == Qt::Horizontal)
		return QSize(500, 30);
	else
		return QSize(30, 500);
}


void Scale::paintEvent(QPaintEvent *event)
{
	int length = m_cellCount*m_cellSize;
	int width = contentsRect().width();
	int height = contentsRect().height();
	int left = contentsRect().left();
	int right = contentsRect().right();
	int bottom = contentsRect().bottom();
	int top = contentsRect().top();
	int majorTick = 0;
	int minorTick = 0;
	int subTick = 0;
	int ticklen = 0;
	int textValue = 0;
	int textValueIncrement = 0;

	switch (m_units)
	{
		case Configuration::EnumEditor_FormatScalesAs::Stitches:
		subTick = minorTick = m_cellSize;
		majorTick = minorTick*m_cellGrouping;
		textValueIncrement = m_cellGrouping;
		break;

		case Configuration::EnumEditor_FormatScalesAs::CM:
		subTick = (int)(m_cellSize*m_clothCount*100/2540);
		minorTick = subTick*5;
		majorTick = subTick*10;
		textValueIncrement = 1;
		break;

		case Configuration::EnumEditor_FormatScalesAs::Inches:
		majorTick = (int)(m_cellSize*m_clothCount);
		minorTick = majorTick/4;
		subTick = majorTick/16;
		textValueIncrement = 1;
		break;
	}

	QPainter painter;
	painter.begin(this);

	if (m_orientation == Qt::Horizontal)
		painter.drawLine(left, bottom, right, bottom);
	else
		painter.drawLine(right, top, right, bottom);

	for (int i = 0 ; i <= length ; i++)
	{
		if ((i % subTick) == 0) ticklen = 3;
		if ((i % minorTick) == 0) ticklen = 6;
		if ((i % majorTick) == 0) ticklen = 9;
		if ((i % subTick) == 0)
		{
			if (m_orientation == Qt::Horizontal)
				painter.drawLine(left+i+m_offset, bottom, left+i+m_offset, bottom-ticklen);
			else
				painter.drawLine(right, top+i+m_offset, right-ticklen, top+i+m_offset);
		}
		if ((i % majorTick) == 0)
		{
			if (m_orientation == Qt::Horizontal)
				painter.drawText(left+i+m_offset+1, bottom-ticklen, QString("%1").arg(textValue));
			else
				painter.drawText(left, top+i+m_offset+1, width, height, Qt::AlignTop, QString("%1").arg(textValue));
			textValue += textValueIncrement;
		}
	}

	painter.end();
}
