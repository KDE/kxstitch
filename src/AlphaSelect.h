/***************************************************************************
 *   Copyright (C) 2003 by Stephen Allewell                                *
 *   stephen@mirramar.fsnet.co.uk                                          *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/


#ifndef __AlphaSelect_H
#define __AlphaSelect_H


#include <QLabel>
#include <QPoint>


class QMouseEvent;


class AlphaSelect : public QLabel
{
	Q_OBJECT

	public:
		AlphaSelect(QLabel *);

	signals:
		void clicked(QPoint);

	protected:
		virtual void mouseReleaseEvent(QMouseEvent *);
};


#endif
