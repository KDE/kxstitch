/****************************************************************************
 *	Copyright (C) 2010 by Stephen Allewell									*
 *	stephen@mirramar.adsl24.co.uk											*
 *																			*
 *	This program is free software; you can redistribute it and/or modify	*
 *	it under the terms of the GNU General Public License as published by	*
 *	the Free Software Foundation; either version 2 of the License, or		*
 *	(at your option) any later version.										*
 ****************************************************************************/


#include "backgroundimage.h"


BackgroundImage::BackgroundImage(const KUrl &url, const QRect &location)
	:	m_url(url),
		m_location(location),
		m_visible(true)
{
	m_image.load(m_url.path());
	QPixmap pixmap = QPixmap::fromImage(m_image);
	m_icon.addPixmap(pixmap.scaled(64, 64, Qt::KeepAspectRatio, Qt::SmoothTransformation));
}


BackgroundImage::BackgroundImage(const KUrl &url, const QRect &location, bool visible, const QImage &image, const QIcon &icon)
	:	m_url(url),
		m_location(location),
		m_visible(visible),
		m_image(image),
		m_icon(icon)
{
}


BackgroundImage::~BackgroundImage()
{
}


const KUrl &BackgroundImage::URL() const
{
	return m_url;
}


const QRect &BackgroundImage::location() const
{
	return m_location;
}


void BackgroundImage::setLocation(const QRect &location)
{
	m_location = location;
}


bool BackgroundImage::isVisible() const
{
	return m_visible;
}


void BackgroundImage::setVisible(bool visible)
{
	m_visible = visible;
}


const QImage &BackgroundImage::image() const
{
	return m_image;
}


const QIcon &BackgroundImage::icon() const
{
	return m_icon;
}
