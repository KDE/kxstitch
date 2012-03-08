/********************************************************************************
 *	Copyright (C) 2012 by Stephen Allewell					*
 *	stephen@mirramar.adsl24.co.uk						*
 *										*
 *	This program is free software; you can redistribute it and/or modify	*
 *	it under the terms of the GNU General Public License as published by	*
 *	the Free Software Foundation; either version 2 of the License, or	*
 *	(at your option) any later version.					*
 ********************************************************************************/


#ifndef Pattern_H
#define Pattern_H


#include "DocumentPalette.h"
#include "StitchData.h"


class Pattern
{
	public:
		Pattern();
		~Pattern();
		
		void clear();
		
		DocumentPalette &palette();
		StitchData &stitches();
		
		friend QDataStream &operator<<(QDataStream &stream, const Pattern &pattern);
		friend QDataStream &operator>>(QDataStream &stream, Pattern &pattern);
		
	private:
		static const int version = 100;
		
		DocumentPalette m_documentPalette;
		StitchData	m_stitchData;
};


QDataStream &operator<<(QDataStream &, const Pattern &);
QDataStream &operator>>(QDataStream &, Pattern &);


#endif // Pattern_H
 
