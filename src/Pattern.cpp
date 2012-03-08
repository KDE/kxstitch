/********************************************************************************
 *	Copyright (C) 2012 by Stephen Allewell					*
 *	stephen@mirramar.adsl24.co.uk						*
 *										*
 *	This program is free software; you can redistribute it and/or modify	*
 *	it under the terms of the GNU General Public License as published by	*
 *	the Free Software Foundation; either version 2 of the License, or	*
 *	(at your option) any later version.					*
 ********************************************************************************/


#include "Pattern.h"


Pattern::Pattern()
{
}


Pattern::~Pattern()
{
}


void Pattern::clear()
{
	m_documentPalette.clear();
	m_stitchData.clear();
}


DocumentPalette &Pattern::palette()
{
	return m_documentPalette;
}


StitchData &Pattern::stitches()
{
	return m_stitchData;
}


QDataStream &operator<<(QDataStream &stream, const Pattern &pattern)
{
	stream << qint32(pattern.version);
	
	stream << pattern.m_documentPalette;
	stream << pattern.m_stitchData;
	
	return stream;
}


QDataStream  &operator>>(QDataStream &stream, Pattern &pattern)
{
	qint32 version;
	
	stream >> version;
	switch (version)
	{
		case 100:
			stream >> pattern.m_documentPalette;
			stream >> pattern.m_stitchData;
			break;

		default:
			// not supported
			// throw exception
			break;
	}

	return stream;
}

