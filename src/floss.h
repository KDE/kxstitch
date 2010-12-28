/****************************************************************************
 *	Copyright (C) 2010 by Stephen Allewell									*
 *	stephen@mirramar.fsnet.co.uk											*
 *																			*
 *	This program is free software; you can redistribute it and/or modify	*
 *	it under the terms of the GNU General Public License as published by	*
 *	the Free Software Foundation; either version 2 of the License, or		*
 *	(at your option) any later version.										*
 ****************************************************************************/


#ifndef FLOSS_H
#define FLOSS_H


#include <QColor>
#include <QString>
#include <QVariant>


class Floss
{
	public:
		Floss(QString n, QString d, QColor c) : name(n),description(d),color(c) {};

		QString name;
		QString description;
		QColor  color;
};


#endif
