
/****************************************************************************
 *	Copyright (C) 2010 by Stephen Allewell									*
 *	stephen@mirramar.adsl24.co.uk											*
 *																			*
 *	This program is free software; you can redistribute it and/or modify	*
 *	it under the terms of the GNU General Public License as published by	*
 *	the Free Software Foundation; either version 2 of the License, or		*
 *	(at your option) any later version.										*
 ****************************************************************************/


#ifndef BACKGROUNDIMAGE_H
#define BACKGROUNDIMAGE_H


#include <QIcon>
#include <QImage>
#include <QRect>

#include <KUrl>


class BackgroundImage
{
	public:
		BackgroundImage(const KUrl &, const QRect &);
		BackgroundImage(const KUrl &, const QRect &, bool, const QImage &, const QIcon &);
		~BackgroundImage();

		const KUrl &URL() const;
		const QRect &location() const;
		void setLocation(const QRect &);
		bool isVisible() const;
		void setVisible(bool);
		bool isValid() const;
		const QImage &image() const;
		const QIcon &icon() const;

	private:
		KUrl	m_url;
		QRect	m_location;
		bool	m_visible;
		bool	m_status;
		QImage	m_image;
		QIcon	m_icon;
};


#endif
