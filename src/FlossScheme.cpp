/********************************************************************************
 *	Copyright (C) 2010 by Stephen Allewell					*
 *	stephen@mirramar.fsnet.co.uk						*
 *										*
 *	This program is free software; you can redistribute it and/or modify	*
 *	it under the terms of the GNU General Public License as published by	*
 *	the Free Software Foundation; either version 2 of the License, or	*
 *	(at your option) any later version.					*
 ********************************************************************************/


#include "FlossScheme.h"

#include <Magick++.h>
#include <magick/version.h>


FlossScheme::FlossScheme()
	:	m_map(0)
{
}


FlossScheme::~FlossScheme()
{
	delete m_map;
}


Floss *FlossScheme::convert(const QColor &color)
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
	QListIterator<Floss *> flossIterator(m_flosses);
	while (flossIterator.hasNext())
	{
		Floss *floss = flossIterator.next();
		if (floss->color() == newColor)
			return floss;
	}

	return 0;  // return 0 as nothing found. It shouldn't happen though
}


Floss *FlossScheme::find(const QString &name) const
{
	QListIterator<Floss *> flossIterator(m_flosses);
	while (flossIterator.hasNext())
	{
		Floss *floss = flossIterator.next();
		if (floss->name() == name)
			return floss;
	}

	return 0;
}


QString FlossScheme::find(const QColor &color) const
{
	QListIterator<Floss *> flossIterator(m_flosses);
	while (flossIterator.hasNext())
	{
		Floss *floss = flossIterator.next();
		if (floss->color() == color)
			return floss->name();
	}

	return QString();
}


QString FlossScheme::schemeName() const
{
	return m_schemeName;
}


QString FlossScheme::path() const
{
	return m_path;
}


const QList<Floss *> &FlossScheme::flosses() const
{
	return m_flosses;
}


void FlossScheme::addFloss(Floss *floss)
{
	m_flosses.append(floss);
	delete m_map;
	m_map = 0;
}


void FlossScheme::clearScheme()
{
	qDeleteAll(m_flosses);
	m_flosses.clear();
}


void FlossScheme::setSchemeName(const QString &name)
{
	m_schemeName = name;
}


void FlossScheme::setPath(const QString &name)
{
	m_path = name;
}


Magick::Image *FlossScheme::createImageMap()
{
	if (m_map == 0)
	{
		char *pixels = new char[(m_flosses.size()+1)*4];
		char *pixel = pixels;
		QListIterator<Floss *> flossIterator(m_flosses);
		while (flossIterator.hasNext())
		{
			Floss *floss = flossIterator.next();
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
