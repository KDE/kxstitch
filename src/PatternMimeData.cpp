/********************************************************************************
 *	Copyright (C) 2010 by Stephen Allewell					*
 *	stephen@mirramar.adsl24.co.uk						*
 *										*
 *	This program is free software; you can redistribute it and/or modify	*
 *	it under the terms of the GNU General Public License as published by	*
 *	the Free Software Foundation; either version 2 of the License, or	*
 *	(at your option) any later version.					*
 ********************************************************************************/



#include "PatternMimeData.h"

#include <KDebug>


PatternMimeData::PatternMimeData()
	:	QMimeData()
{
	m_outputFormats.append("application/kxstitch");
}


PatternMimeData::~PatternMimeData()
{
}


QStringList PatternMimeData::formats() const
{
	return m_outputFormats;
}


bool PatternMimeData::hasFormat(const QString &mimeType) const
{
	return m_outputFormats.contains(mimeType);
}


QVariant PatternMimeData::retrieveData(const QString &mimeType, QVariant::Type variantType) const
{
	kDebug() << "mimeType =" << mimeType;
	kDebug() << "QVariant::Type =" << QVariant::typeToName(variantType);
	return QVariant();
}


void PatternMimeData::setScale(int scaleWidth, int scaleHeight)
{
	m_scaleWidth = scaleWidth;
	m_scaleHeight = scaleHeight;
}
