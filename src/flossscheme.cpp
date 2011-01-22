/****************************************************************************
 *	Copyright (C) 2010 by Stephen Allewell									*
 *	stephen@mirramar.fsnet.co.uk											*
 *																			*
 *	This program is free software; you can redistribute it and/or modify	*
 *	it under the terms of the GNU General Public License as published by	*
 *	the Free Software Foundation; either version 2 of the License, or		*
 *	(at your option) any later version.										*
 ***************************************************************************/


#include <Magick++.h>
#include <magick/version.h>

#include "flossscheme.h"


FlossScheme::FlossScheme()
  : m_map(0)
{
}


FlossScheme::~FlossScheme()
{
	delete m_map;
	qDeleteAll(m_flosses);
}


QString FlossScheme::schemeName()
{
	return m_name;
}


void FlossScheme::setSchemeName(QString name)
{
	m_name = name;
}


void FlossScheme::addFloss(Floss *floss)
{
	m_flosses.append(floss);
	delete m_map;
	m_map = 0;
}


Floss *FlossScheme::find(QString name)
{
	for (int i = 0 ; i < m_flosses.size() ; i++)
	{
		if (m_flosses.at(i)->name() == name)
			return m_flosses.at(i);
	}
	return 0;
}


Magick::Image* FlossScheme::createImageMap()
{
	if (m_map == 0)
	{
		char *pixels = new char[(m_flosses.size()+1)*4];
		char *pixel = pixels;
		Floss *floss;
		for (int i = 0 ; i < m_flosses.size() ; i++)
		{
			floss = m_flosses.at(i);
			*pixel++ = (char)(floss->color().red());
			*pixel++ = (char)(floss->color().green());
			*pixel++ = (char)(floss->color().blue());
			*pixel++ = (char)(0xff);
		}

		// transparent pixel
		*pixel++ = (char)0xff;
		*pixel++ = (char)0xff;
		*pixel++ = (char)0xff;
		*pixel++ = (char)0;
#if MagickLibVersion >= 0x642
		m_map = new Magick::Image(m_flosses.size() + 1, 1, "RGBA", MagickCore::CharPixel, pixels);
#else
		m_map = new Magick::Image(m_flosses.size() + 1, 1, "RGBA", MagickLib::CharPixel, pixels);
#endif
		delete pixels;
	}
	return m_map;
}


Floss *FlossScheme::convert(QColor color)
{
	createImageMap();
	char c[3];
	c[0] = (char)color.red();
	c[1] = (char)color.green();
	c[2] = (char)color.blue();
#if MagickLibVersion >= 0x642
	Magick::Image image = Magick::Image(1, 1, "RGB", MagickCore::CharPixel, c);
#else
	Magick::Image image = Magick::Image(1, 1, "RGB", MagickLib::CharPixel, c);
#endif
	image.map(*m_map);
	Magick::Pixels cache(image);
	const Magick::PixelPacket *pixels = cache.get(0,0,1,1);
	Magick::PixelPacket packet = *pixels;
	QColor newColor(packet.red/257, packet.green/257, packet.blue/257);
	for (int i = 0 ; i < m_flosses.size() ; i++)
	{
		Floss *floss = m_flosses.at(i);
		if (floss->color() == newColor)
			return floss;
	}
	return 0; // should never happen
}


QListIterator<Floss*> FlossScheme::flossIterator()
{
	return QListIterator<Floss*>(m_flosses);
}


void FlossScheme::clearScheme()
{
	m_flosses.clear();
}
