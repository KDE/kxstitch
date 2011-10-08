/********************************************************************************
 *	Copyright (C) 2010 by Stephen Allewell					*
 *	stephen@mirramar.adsl24.co.uk						*
 *										*
 *	This program is free software; you can redistribute it and/or modify	*
 *	it under the terms of the GNU General Public License as published by	*
 *	the Free Software Foundation; either version 2 of the License, or	*
 *	(at your option) any later version.					*
 ********************************************************************************/


#include "PaperSizes.h"


QPrinter::PaperSize PaperSizes::size(const QString &name)
{
	int i = 0;
	while (PaperSizeList[i].size != QPrinter::Custom)
	{
		if (PaperSizeList[i].name == name)
			break;
		++i;
	}
	return PaperSizeList[i].size; // will return QPrinter::Custom if name isn't found
}


QString PaperSizes::name(QPrinter::PaperSize size)
{
	int i = 0;
	while (PaperSizeList[i].size != QPrinter::Custom)
	{
		if (PaperSizeList[i].size == size)
			break;
		++i;
	}
	return QString(PaperSizeList[i].name);
}


int PaperSizes::width(QPrinter::PaperSize size, QPrinter::Orientation orientation)
{
	int i = 0;
	while (PaperSizeList[i].size != QPrinter::Custom)
	{
		if (PaperSizeList[i].size == size)
			break;
		++i;
	}
	return (orientation == QPrinter::Portrait)?PaperSizeList[i].width:PaperSizeList[i].height;
}


int PaperSizes::height(QPrinter::PaperSize size, QPrinter::Orientation orientation)
{
	int i = 0;
	while (PaperSizeList[i].size != QPrinter::Custom)
	{
		if (PaperSizeList[i].size == size)
			break;
		++i;
	}
	return (orientation == QPrinter::Portrait)?PaperSizeList[i].height:PaperSizeList[i].width;
}
