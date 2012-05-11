/********************************************************************************
 *	Copyright (C) 2010 by Stephen Allewell					*
 *	stephen@mirramar.adsl24.co.uk						*
 *										*
 *	This program is free software; you can redistribute it and/or modify	*
 *	it under the terms of the GNU General Public License as published by	*
 *	the Free Software Foundation; either version 2 of the License, or	*
 *	(at your option) any later version.					*
 ********************************************************************************/


#include "BackgroundImages.h"

#include <KLocale>

#include "BackgroundImage.h"
#include "Exceptions.h"


BackgroundImages::BackgroundImages()
{
}


BackgroundImages::~BackgroundImages()
{
	clear();
}


void BackgroundImages::clear()
{
	qDeleteAll(m_backgroundImages);
	m_backgroundImages.clear();
}


QListIterator<BackgroundImage *> BackgroundImages::backgroundImages()
{
	return QListIterator<BackgroundImage *>(m_backgroundImages);
}


void BackgroundImages::addBackgroundImage(BackgroundImage *backgroundImage)
{
	m_backgroundImages.append(backgroundImage);
}


bool BackgroundImages::removeBackgroundImage(BackgroundImage *backgroundImage)
{
	return m_backgroundImages.removeOne(backgroundImage);
}


QRect BackgroundImages::fitBackgroundImage(BackgroundImage *backgroundImage, const QRect &rectangle)
{
	QRect oldRectangle = backgroundImage->location();
	backgroundImage->setLocation(rectangle);
	return oldRectangle;
}


bool BackgroundImages::showBackgroundImage(BackgroundImage *backgroundImage, bool show)
{
	bool shown = backgroundImage->isVisible();
	backgroundImage->setVisible(show);

	return shown;
}


QDataStream &operator<<(QDataStream &stream, const BackgroundImages &backgroundImages)
{
	stream << qint32(backgroundImages.version);
	stream << qint32(backgroundImages.m_backgroundImages.count());

	QListIterator<BackgroundImage *> backgroundImageIterator(backgroundImages.m_backgroundImages);
	while (backgroundImageIterator.hasNext())
	{
		stream << *backgroundImageIterator.next();
	}
	
	if (stream.status() != QDataStream::Ok)
		throw FailedWriteFile();

	return stream;
}


QDataStream &operator>>(QDataStream &stream, BackgroundImages &backgroundImages)
{
	qint32 version;
	qint32 backgroundImageCount;
	BackgroundImage *backgroundImage;

	stream >> version;
	switch (version)
	{
		case 100:
			stream >> backgroundImageCount;
			while (backgroundImageCount--)
			{
				backgroundImage = new BackgroundImage;
				stream >> *backgroundImage;
				backgroundImages.m_backgroundImages.append(backgroundImage);
			}
			break;

		default:
			throw InvalidFileVersion(QString(i18n("Background images version %1", version)));
			break;
	}
			
	if (stream.status() != QDataStream::Ok)
		throw FailedReadFile(QString(i18n("Failed reading background images")));
			
	return stream;
}
