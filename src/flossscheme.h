/****************************************************************************
 *	Copyright (C) 2010 by Stephen Allewell									*
 *	stephen@mirramar.fsnet.co.uk											*
 *																			*
 *	This program is free software; you can redistribute it and/or modify	*
 *	it under the terms of the GNU General Public License as published by	*
 *	the Free Software Foundation; either version 2 of the License, or		*
 *	(at your option) any later version.										*
 ****************************************************************************/


#ifndef FLOSSSCHEME_H
#define FLOSSSCHEME_H


#include <QString>
#include <QList>
#include <QColor>
#include <Magick++.h>

#include "floss.h"


class FlossScheme
{
	public:
		FlossScheme();
		~FlossScheme();

		QString schemeName();
		void setSchemeName(QString name);
		void addFloss(Floss *floss);
		Floss *find(QString name);
		Magick::Image* createImageMap();
		Floss *convert(QColor color);
		QListIterator<Floss*> flossIterator();
		void clearScheme();

	private:
		QString           m_name;
		QList<Floss*>     m_flosses;
		Magick::Image*    m_map;
};

#endif
