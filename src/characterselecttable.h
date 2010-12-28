/****************************************************************************
 *	Copyright (C) 2010 by Stephen Allewell									*
 *	stephen@mirramar.fsnet.co.uk											*
 *																			*
 *	This program is free software; you can redistribute it and/or modify	*
 *	it under the terms of the GNU General Public License as published by	*
 *	the Free Software Foundation; either version 2 of the License, or		*
 *	(at your option) any later version.										*
 ****************************************************************************/


#ifndef CHARSELECTTABLE_H
#define CHARSELECTTABLE_H


#include <kcharselect.h>


class CharSelectTable : public KCharSelectTable
{
	Q_OBJECT

	public:
		CharSelectTable(QWidget *parent, const char *name);

	void setUsed(QMap<int, QChar> *);

	signals:
		void charSelected(QChar);

	protected:
		virtual void paintCell(QPainter *p, int row, int col);

	private:
		bool				symbolUsed(QChar c);
		QMap<int, QChar>	*m_used;
};


#endif
