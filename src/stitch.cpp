/****************************************************************************
 *	Copyright (C) 2010 by Stephen Allewell									*
 *	stephen@mirramar.adsl24.co.uk											*
 *																			*
 *	This program is free software; you can redistribute it and/or modify	*
 *	it under the terms of the GNU General Public License as published by	*
 *	the Free Software Foundation; either version 2 of the License, or		*
 *	(at your option) any later version.										*
 ****************************************************************************/


/**
	@file
	Implement classes for Stitch, Backstitch and Knot.  Essentially
	these consist of constructors to initialise the classes.  The member
	variables are public for convenience.
	*/


#include "stitch.h"


/**
	Constructor.
	@param t stitch type
	@param i color index
	*/
Stitch::Stitch(Type t, int i)
  : type(t),
	floss(i)
{
}


/**
	Constructor.
	@param s start of backstitch line
	@param e end of backstitch line
	@param i color index
	*/
Backstitch::Backstitch(QPoint s, QPoint e, int i)
  : start(s),
	end(e),
	floss(i)
{
}


/**
	Test if the backstitch start or end is equal to the
	requested point.
	@param p point to test
	@return true if p starts or ends the backstitch line,
	false other wise.
	*/
bool Backstitch::contains(QPoint p)
{
	if (start == p || end == p)
		return true;
	return false;
}


/**
	Constructor.
	@param p position for the knot
	@param i color index
	*/
Knot::Knot(QPoint p, int i)
  : pos(p),
	floss(i)
{
}
