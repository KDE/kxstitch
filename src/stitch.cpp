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
	Implement classes for Stitch, StitchQueue, Backstitch and Knot.
	*/


#include "stitch.h"


/**
	Constructor.
	@param type stitch type
	@param floss color index
	*/
Stitch::Stitch(Type type, unsigned floss)
  : m_type(type),
	m_floss(floss)
{
}


/**
	Destructor.
	*/
Stitch::~Stitch()
{
}


/**
	Get the stitch type.
	@return enum value representing the stitch type
	*/
Stitch::Type Stitch::type() const
{
	return m_type;
}


/**
	Set the stitch type
	@param type stitch type
	*/
void Stitch::setType(Stitch::Type type)
{
	m_type = type;
}


/**
	Get the floss index.
	@return index value of the floss
	*/
unsigned Stitch::floss() const
{
	return m_floss;
}


/**
	Constructor.
	*/
StitchQueue::StitchQueue()
	:	QQueue<Stitch *>()
{
}


/**
	Copy constructor.  Makes a deep copy of the passed StitchQueue.
	@param queue a pointer to the StitchQueue to be copied.
	*/
StitchQueue::StitchQueue(const StitchQueue *const queue)
	:	QQueue<Stitch *>()
{
	if (queue)
	{
		for (int i = 0 ; i < queue->count() ; i++)
		{
			enqueue(new Stitch(queue->value(i)->type(), queue->value(i)->floss()));
		}
	}
}


/**
	Destructor.  Dequeues the stitches and deletes them.
	*/
StitchQueue::~StitchQueue()
{
	while (!isEmpty())
	{
		delete dequeue();
	}
}


/**
	Constructor.
	@param s start of backstitch line
	@param e end of backstitch line
	@param i color index
	*/
Backstitch::Backstitch(const QPoint &start, const QPoint &end, unsigned floss)
  : m_start(start),
	m_end(end),
	m_floss(floss)
{
}


/**
	Destructor
	*/
Backstitch::~Backstitch()
{
}


/**
	Get the start point.
	@return a const reference to a QPoint.
	*/
const QPoint &Backstitch::start() const
{
	return m_start;
}


/**
	Get the end point.
	@return a const reference to a QPoint.
	*/
const QPoint &Backstitch::end() const
{
	return m_end;
}


/**
	Get the floss index.
	@return index value of the floss.
	*/
unsigned Backstitch::floss() const
{
	return m_floss;
}


/**
	Test if the backstitch start or end is equal to the
	requested point.
	@param p point to test
	@return true if p starts or ends the backstitch line,
	false other wise.
	*/
bool Backstitch::contains(const QPoint &point) const
{
	if (m_start == point || m_end == point)
		return true;
	return false;
}


/**
	Constructor.
	@param p position for the knot
	@param i color index
	*/
Knot::Knot(const QPoint &point, unsigned floss)
  : m_position(point),
	m_floss(floss)
{
}


/**
	Destructor.
	*/
Knot::~Knot()
{
}


/**
	Get the position of the knot.
	@return a const reference to a QPoint.
	*/
const QPoint &Knot::position() const
{
	return m_position;
}


/**
	Get the floss index.
	@return index value of the floss.
	*/
unsigned Knot::floss() const
{
	return m_floss;
}
