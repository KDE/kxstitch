/****************************************************************************
 *	Copyright (C) 2010 by Stephen Allewell									*
 *	stephen@mirramar.adsl24.co.uk											*
 *																			*
 *	This program is free software; you can redistribute it and/or modify	*
 *	it under the terms of the GNU General Public License as published by	*
 *	the Free Software Foundation; either version 2 of the License, or		*
 *	(at your option) any later version.										*
 ****************************************************************************/


#include "floss.h"


Floss::Floss(const QString &name, const QString &description, const QColor &color)
	:	m_name(name),
		m_description(description),
		m_color(color)
{
}


Floss::~Floss()
{
}


const QString &Floss::name() const
{
	return m_name;
}


void Floss::setName(const QString &name)
{
	m_name = name;
}


const QString &Floss::description() const
{
	return m_description;
}


void Floss::setDescription(const QString &description)
{
	m_description = description;
}


const QColor &Floss::color() const
{
	return m_color;
}


void Floss::setColor(const QColor &color)
{
	m_color = color;
}


DocumentFloss::DocumentFloss(Floss *floss, QChar symbol, unsigned stitchStrands, unsigned backstitchStrands)
	:	m_floss(floss),
		m_symbol(symbol),
		m_stitchStrands(stitchStrands),
		m_backstitchStrands(backstitchStrands)
{
}


DocumentFloss::DocumentFloss(DocumentFloss *documentFloss)
{
	m_floss = documentFloss->m_floss;
	m_symbol = documentFloss->m_symbol;
	m_stitchStrands = documentFloss->m_stitchStrands;
	m_backstitchStrands = documentFloss->m_backstitchStrands;
}


DocumentFloss::~DocumentFloss()
{
}


const Floss *DocumentFloss::floss() const
{
	return m_floss;
}


void DocumentFloss::setFloss(const Floss *floss)
{
	m_floss = floss;
}


const QChar DocumentFloss::symbol() const
{
	return m_symbol;
}


void DocumentFloss::setSymbol(QChar symbol)
{
	m_symbol = symbol;
}


unsigned DocumentFloss::stitchStrands() const
{
	return m_stitchStrands;
}


void DocumentFloss::setStitchStrands(unsigned stitchStrands)
{
	m_stitchStrands = stitchStrands;
}


unsigned DocumentFloss::backstitchStrands() const
{
	return m_backstitchStrands;
}


void DocumentFloss::setBackstitchStrands(unsigned backstitchStrands)
{
	m_backstitchStrands = backstitchStrands;
}


bool DocumentFloss::operator==(const DocumentFloss &floss)
{
	return ((m_floss == floss.m_floss) && (m_symbol == floss.m_symbol) && (m_stitchStrands == floss.m_stitchStrands) && (m_backstitchStrands == floss.m_backstitchStrands));
}


bool DocumentFloss::operator!=(const DocumentFloss &floss)
{
	return !(*this == floss);
}
