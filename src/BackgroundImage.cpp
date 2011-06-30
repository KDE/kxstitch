/********************************************************************************
 *	Copyright (C) 2010 by Stephen Allewell					*
 *	stephen@mirramar.adsl24.co.uk						*
 *										*
 *	This program is free software; you can redistribute it and/or modify	*
 *	it under the terms of the GNU General Public License as published by	*
 *	the Free Software Foundation; either version 2 of the License, or	*
 *	(at your option) any later version.					*
 ********************************************************************************/


#include "BackgroundImage.h"


BackgroundImage::BackgroundImage()
{
}


BackgroundImage::BackgroundImage(const KUrl &url, const QRect &location)
{
	setUrl(url);
	setLocation(location);
	setVisible(true);
	QImage image;
	setStatus(image.load(m_url.path()));
	setImage(image);
	setIcon(QPixmap::fromImage(m_image).scaled(64, 64, Qt::KeepAspectRatio, Qt::SmoothTransformation));
}


const KUrl &BackgroundImage::url() const
{
	return m_url;
}


const QRect &BackgroundImage::location() const
{
	return m_location;
}


bool BackgroundImage::isVisible() const
{
	return m_visible;
}


bool BackgroundImage::isValid() const
{
	return m_status;
}


const QImage &BackgroundImage::image() const
{
	return m_image;
}


const QIcon &BackgroundImage::icon() const
{
	return m_icon;
}


void BackgroundImage::setUrl(const KUrl &url)
{
	m_url = url;
}


void BackgroundImage::setLocation(const QRect &location)
{
	m_location = location;
}


void BackgroundImage::setVisible(bool visible)
{
	m_visible = visible;
}


void BackgroundImage::setStatus(bool valid)
{
	m_status = valid;
}


void BackgroundImage::setImage(const QImage &image)
{
	m_image = image;
}


void BackgroundImage::setIcon(const QIcon &icon)
{
	m_icon = icon;
}


QDataStream &operator<<(QDataStream &stream, const BackgroundImage &backgroundImage)
{
	stream << qint32(backgroundImage.version);
	stream << backgroundImage.m_url;
	stream << backgroundImage.m_location;
	stream << backgroundImage.m_visible;
	stream << backgroundImage.m_status;
	stream << backgroundImage.m_image;
	stream << backgroundImage.m_icon;

	return stream;
}


QDataStream &operator>>(QDataStream &stream, BackgroundImage &backgroundImage)
{
	qint32 version;

	stream >> version;
	switch (version)
	{
		case 100:
			stream >> backgroundImage.m_url;
			stream >> backgroundImage.m_location;
			stream >> backgroundImage.m_visible;
			stream >> backgroundImage.m_status;
			stream >> backgroundImage.m_image;
			stream >> backgroundImage.m_icon;
			break;

		default:
			// not supported
			// throw exception
			break;
	}

	return stream;
}
