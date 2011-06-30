/********************************************************************************
 *	Copyright (C) 2010 by Stephen Allewell					*
 *	stephen@mirramar.adsl24.co.uk						*
 *										*
 *	This program is free software; you can redistribute it and/or modify	*
 *	it under the terms of the GNU General Public License as published by	*
 *	the Free Software Foundation; either version 2 of the License, or	*
 *	(at your option) any later version.					*
 ********************************************************************************/


#ifndef __Scale_H
#define __Scale_H


#include <QWidget>

#include "configuration.h"


class Scale : public QWidget
{
	Q_OBJECT

	public:
		Scale(Qt::Orientation);
		~Scale();

		void setUnits(Configuration::EnumEditor_FormatScalesAs::type);
		void setCellSize(int);
		void setCellGrouping(int);
		void setCellCount(int);
		void setClothCount(double);
		void setOffset(int);
		virtual QSize sizeHint() const;

	protected:
		void paintEvent(QPaintEvent *);

	private:
		Configuration::EnumEditor_FormatScalesAs::type m_units;
		int m_cellSize;
		int m_cellGrouping;
		int m_cellCount;
		double m_clothCount;
		int m_offset;
		Qt::Orientation m_orientation;
};


#endif