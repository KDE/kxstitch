/********************************************************************************
 *	Copyright (C) 2010 by Stephen Allewell					*
 *	stephen@mirramar.adsl24.co.uk						*
 *										*
 *	This program is free software; you can redistribute it and/or modify	*
 *	it under the terms of the GNU General Public License as published by	*
 *	the Free Software Foundation; either version 2 of the License, or	*
 *	(at your option) any later version.					*
 ********************************************************************************/


#include "DocumentFloss.h"


DocumentFloss::DocumentFloss()
{
}


DocumentFloss::DocumentFloss(const QString &flossName, const QChar &stitchSymbol, Qt::PenStyle backstitchSymbol, int stitchStrands, int backstitchStrands)
	:	m_flossName(flossName),
		m_stitchSymbol(stitchSymbol),
		m_backstitchSymbol(backstitchSymbol),
		m_stitchStrands(stitchStrands),
		m_backstitchStrands(backstitchStrands)
{
}


DocumentFloss::DocumentFloss(DocumentFloss *other)
	:	m_flossName(other->m_flossName),
		m_stitchSymbol(other->m_stitchSymbol),
		m_backstitchSymbol(other->m_backstitchSymbol),
		m_stitchStrands(other->m_stitchStrands),
		m_backstitchStrands(other->m_backstitchStrands)
{
}


QString DocumentFloss::flossName() const
{
	return m_flossName;
}


QColor DocumentFloss::flossColor() const
{
	return m_flossColor;
}


QChar DocumentFloss::stitchSymbol() const
{
	return m_stitchSymbol;
}


Qt::PenStyle DocumentFloss::backstitchSymbol() const
{
	return m_backstitchSymbol;
}


int DocumentFloss::stitchStrands() const
{
	return m_stitchStrands;
}


int DocumentFloss::backstitchStrands() const
{
	return m_backstitchStrands;
}


void DocumentFloss::setFlossName(const QString &flossName)
{
	m_flossName = flossName;
}


void DocumentFloss::setFlossColor(const QColor &flossColor)
{
	m_flossColor = flossColor;
}


void DocumentFloss::setStitchSymbol(const QChar &stitchSymbol)
{
	m_stitchSymbol = stitchSymbol;
}


void DocumentFloss::setBackstitchSymbol(Qt::PenStyle backstitchSymbol)
{
	m_backstitchSymbol = backstitchSymbol;
}


void DocumentFloss::setStitchStrands(int stitchStrands)
{
	m_stitchStrands = stitchStrands;
}


void DocumentFloss::setBackstitchStrands(int backstitchStrands)
{
	m_backstitchStrands = backstitchStrands;
}


bool DocumentFloss::operator==(const DocumentFloss &other) const
{
	return ((m_flossName == other.m_flossName) &&
		(m_stitchSymbol == other.m_stitchSymbol) &&
		(m_backstitchSymbol == other.m_backstitchSymbol) &&
		(m_stitchStrands == other.m_stitchStrands) &&
		(m_backstitchStrands == other.m_backstitchStrands));
}


bool DocumentFloss::operator!=(const DocumentFloss &other) const
{
	return !(*this == other);
}


QDataStream &operator<<(QDataStream &stream, const DocumentFloss &documentFloss)
{
	stream << qint32(documentFloss.version);

	stream << documentFloss.m_flossName;
	stream << documentFloss.m_flossColor;
	stream << documentFloss.m_stitchSymbol;
	stream << qint32(documentFloss.m_backstitchSymbol);
	stream << qint32(documentFloss.m_stitchStrands);
	stream << qint32(documentFloss.m_backstitchStrands);

	return stream;
}


QDataStream &operator>>(QDataStream &stream, DocumentFloss &documentFloss)
{
	qint32 version;
	qint32 backstitchSymbol;
	qint32 stitchStrands;
	qint32 backstitchStrands;

	stream >> version;
	switch(version)
	{
		case 100:
			stream >> documentFloss.m_flossName;
			stream >> documentFloss.m_flossColor;
			stream >> documentFloss.m_stitchSymbol;
			stream >> backstitchSymbol;
			stream >> stitchStrands;
			stream >> backstitchStrands;
			documentFloss.m_backstitchSymbol = Qt::PenStyle(backstitchSymbol);
			documentFloss.m_stitchStrands = stitchStrands;
			documentFloss.m_backstitchStrands = backstitchStrands;
	}

	return stream;
}
