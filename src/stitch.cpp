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
	Initialisation of static variables to collate stitch and floss usage.
	*/
QMap<Stitch::Type, int> StitchQueue::m_usedStitches;
QMap<int, int> StitchQueue::m_usedFlosses;



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
	Add a stitch to the queue.
	@param type a Stitch::Type value to be added
	@param floss the floss index
	*/
int StitchQueue::add(Stitch::Type type, int floss)
{
	bool miniStitch = (type & 192);
	unsigned int stitchCount = count();
	unsigned int stitches = stitchCount;

	if (!miniStitch)
	{
		// try and merge it with any existing stitches in the queue to update the stitch being added
		while (stitches--)
		{
			Stitch *stitch = dequeue();
			if (!(stitch->type() & 192)) // so we don't try and merge existing mini stitches
				if (stitch->floss() == floss)
					type = (Stitch::Type)(type | stitch->type());
			enqueue(stitch);
		}
	}

	switch (type) // add the new stitch checking for illegal types
	{
		case 3: // TLQtr and TRQtr
			enqueue(new Stitch(Stitch::TLQtr, floss));
			enqueue(new Stitch(Stitch::TRQtr, floss));
			m_usedStitches[Stitch::TLQtr]++;
			m_usedStitches[Stitch::TRQtr]++;
			m_usedFlosses[floss]+=2;
			break;
		case 5: // TLQtr and BLQtr
			enqueue(new Stitch(Stitch::TLQtr, floss));
			enqueue(new Stitch(Stitch::BLQtr, floss));
			m_usedStitches[Stitch::TLQtr]++;
			m_usedStitches[Stitch::BLQtr]++;
			m_usedFlosses[floss]+=2;
			break;
		case 10: // TRQtr and BRQtr
			enqueue(new Stitch(Stitch::TRQtr, floss));
			enqueue(new Stitch(Stitch::BRQtr, floss));
			m_usedStitches[Stitch::TRQtr]++;
			m_usedStitches[Stitch::BRQtr]++;
			m_usedFlosses[floss]+=2;
			break;
		case 12: // BLQtr and BRQtr
			enqueue(new Stitch(Stitch::BLQtr, floss));
			enqueue(new Stitch(Stitch::BRQtr, floss));
			m_usedStitches[Stitch::BLQtr]++;
			m_usedStitches[Stitch::BRQtr]++;
			m_usedFlosses[floss]+=2;
			break;
		default: // other values are acceptable as is including mini stitches
			enqueue(new Stitch(type, floss));
			m_usedStitches[type]++;
			m_usedFlosses[floss]+=stitchLength(type);
			break;
	}

	/** iterate the queue of existing stitches for any that have been overwritten by the new stitch */
	while (stitchCount--)													// while there are existing stitches
	{
		Stitch *stitch = dequeue();											// get the stitch at the head of the queue
		Stitch::Type currentStitchType = (Stitch::Type)(stitch->type());	// and find its type
		int currentFlossIndex = stitch->floss();							// and color
		m_usedStitches[currentStitchType]--;
		m_usedFlosses[currentFlossIndex]-=stitchLength(currentStitchType);
		Stitch::Type usageMask = (Stitch::Type)(currentStitchType & 15);	// and find which parts of a stitch cell are used
		Stitch::Type interferenceMask = (Stitch::Type)(usageMask & type);
		// interferenceMask now contains a mask of which bits are affected by new stitch
		if (interferenceMask)
		{
			// Some parts of the current stitch are being overwritten
			// but a check needs to be made for illegal values
			Stitch::Type changeMask = (Stitch::Type)(usageMask ^ interferenceMask);
			switch (changeMask)
			{
				// changeMask contains what is left of the original stitch after being overwritten
				// it may contain illegal values, so these are checked for
				case 3:
					enqueue(new Stitch(Stitch::TLQtr, currentFlossIndex));
					enqueue(new Stitch(Stitch::TRQtr, currentFlossIndex));
					m_usedStitches[Stitch::TLQtr]++;
					m_usedStitches[Stitch::TRQtr]++;
					m_usedFlosses[floss]+=2;
					changeMask = Stitch::Delete;
					break;
				case 5:
					enqueue(new Stitch(Stitch::TLQtr, currentFlossIndex));
					enqueue(new Stitch(Stitch::BLQtr, currentFlossIndex));
					m_usedStitches[Stitch::TLQtr]++;
					m_usedStitches[Stitch::BLQtr]++;
					m_usedFlosses[floss]+=2;
					changeMask = Stitch::Delete;
					break;
				case 10:
					enqueue(new Stitch(Stitch::TRQtr, currentFlossIndex));
					enqueue(new Stitch(Stitch::BRQtr, currentFlossIndex));
					m_usedStitches[Stitch::TRQtr]++;
					m_usedStitches[Stitch::BRQtr]++;
					m_usedFlosses[floss]+=2;
					changeMask = Stitch::Delete;
					break;
				case 12:
					enqueue(new Stitch(Stitch::BLQtr, currentFlossIndex));
					enqueue(new Stitch(Stitch::BRQtr, currentFlossIndex));
					m_usedStitches[Stitch::BLQtr]++;
					m_usedStitches[Stitch::BRQtr]++;
					m_usedFlosses[floss]+=2;
					changeMask = Stitch::Delete;
					break;
				default:
					// other values are acceptable as is
					break;
			}

			if (changeMask)						// Check if there is anything left of the original stitch, Stitch::Delete is 0
			{
				stitch->setType(changeMask);	// and change stitch type to the changeMask value
				enqueue(stitch);				// and then add it back to the queue
				m_usedStitches[changeMask]++;
				m_usedFlosses[stitch->floss()]+=stitchLength(changeMask);
			}
			else								// if changeMask is 0, it does not get requeued, effectively deleting it from the pattern
				delete stitch;					// delete the Stitch as it is no longer required
		}
		else
		{
			enqueue(stitch);
			m_usedStitches[stitch->type()]++;
			m_usedFlosses[stitch->floss()]+=stitchLength(stitch->type());
		}
	}
	return count();
}


/**
	Delete a stitch from the queue.
	@param type a Stitch::Type mask to remove, Stitch::Delete for all stitches.
	@param floss a floss mask, -1 for all flosses.
	*/
int StitchQueue::remove(Stitch::Type type, int floss)
{
	if (type == Stitch::Delete)
	{
		int stitchCount = count();
		while (stitchCount--)
		{
			Stitch *stitch = dequeue();
			m_usedStitches[stitch->type()]--;
			m_usedFlosses[stitch->floss()]-=stitchLength(stitch->type());
			if ((floss == -1) || (stitch->floss() == floss))
				delete stitch;
			else
			{
				enqueue(stitch);
				m_usedStitches[stitch->type()]++;
				m_usedFlosses[stitch->floss()]+=stitchLength(stitch->type());
			}
		}
	}
	else
	{
		int stitchCount = count();
		while (stitchCount--)
		{
			Stitch *stitch = dequeue();
			m_usedStitches[stitch->type()]--;
			m_usedFlosses[stitch->floss()]-=stitchLength(stitch->type());
			if ((stitch->type() == type) && ((floss == -1) || (stitch->floss() == floss)))
				// delete any stitches of the required stitch if it is the correct color or if the color doesn't matter
				delete stitch;
			else
			{
				if ((stitch->type() & type) && ((floss == -1) || (stitch->floss() == floss)) && ((stitch->type() & 192) == 0))
				{
					// the mask covers a part of the current stitch and is the correct color or if the color doesn't matter
					Stitch::Type changeMask = (Stitch::Type)(stitch->type() ^ type);
					int index = stitch->floss();
					delete stitch;
					switch (changeMask)
					{
						// changeMask contains what is left of the original stitch after deleting the maskStitch
						// it may contain illegal values, so these are checked for
						case 3:
							enqueue(new Stitch(Stitch::TLQtr, index));
							enqueue(new Stitch(Stitch::TRQtr, index));
							m_usedStitches[Stitch::TLQtr]++;
							m_usedStitches[Stitch::TRQtr]++;
							m_usedFlosses[index]+=2;
							break;
						case 5:
							enqueue(new Stitch(Stitch::TLQtr, index));
							enqueue(new Stitch(Stitch::BLQtr, index));
							m_usedStitches[Stitch::TLQtr]++;
							m_usedStitches[Stitch::BLQtr]++;
							m_usedFlosses[index]+=2;
							break;
						case 10:
							enqueue(new Stitch(Stitch::TRQtr, index));
							enqueue(new Stitch(Stitch::BRQtr, index));
							m_usedStitches[Stitch::TRQtr]++;
							m_usedStitches[Stitch::BRQtr]++;
							m_usedFlosses[index]+=2;
							break;
						case 12:
							enqueue(new Stitch(Stitch::BLQtr, index));
							enqueue(new Stitch(Stitch::BRQtr, index));
							m_usedStitches[Stitch::BLQtr]++;
							m_usedStitches[Stitch::BRQtr]++;
							m_usedFlosses[index]+=2;
							break;
						default:
							enqueue(new Stitch((Stitch::Type)changeMask, index));
							m_usedStitches[changeMask]++;
							m_usedFlosses[index]+=stitchLength(changeMask);
							break;
					}
				}
				else
				{
					enqueue(stitch);
					m_usedStitches[stitch->type()]++;
					m_usedFlosses[stitch->floss()]+=stitchLength(stitch->type());
				}
			}
		}
	}
	return count();
}


/**
	Get the length of floss used for a required stitch type.
	@param type a Stitch::Type value to search for.
	@return a int value defining the number of quadrants the stitch occupies.
	Note that a small full stitch is defined as being 2 quadrants.
	*/
int StitchQueue::stitchLength(Stitch::Type type)
{
	static QMap<Stitch::Type, int> lengths;
	if (!lengths.count())
	{
		lengths.insert(Stitch::Delete, 4);
		lengths.insert(Stitch::TLQtr, 1);
		lengths.insert(Stitch::TRQtr, 1);
		lengths.insert(Stitch::BLQtr, 1);
		lengths.insert(Stitch::BTHalf, 2);
		lengths.insert(Stitch::TL3Qtr, 3);
		lengths.insert(Stitch::BRQtr, 1);
		lengths.insert(Stitch::TBHalf, 2);
		lengths.insert(Stitch::TR3Qtr, 3);
		lengths.insert(Stitch::BL3Qtr, 3);
		lengths.insert(Stitch::BR3Qtr, 3);
		lengths.insert(Stitch::Full, 4);
		lengths.insert(Stitch::TLSmallHalf, 1);
		lengths.insert(Stitch::TRSmallHalf, 1);
		lengths.insert(Stitch::BLSmallHalf, 1);
		lengths.insert(Stitch::BRSmallHalf, 1);
		lengths.insert(Stitch::TLSmallFull, 2);
		lengths.insert(Stitch::TRSmallFull, 2);
		lengths.insert(Stitch::BLSmallFull, 2);
		lengths.insert(Stitch::BRSmallFull, 2);
		lengths.insert(Stitch::FrenchKnot, 4);
	}
	return lengths[type];
}


/**
	Get a reference to the floss usage QMap.
	@return a QMap<int index, int quantity>.  The quantity is the number of quarters
	the stitch covers.
	*/
const QMap<int, int> &StitchQueue::usedFlosses()
{
	return m_usedFlosses;
}


/**
	Get a reference to the stitch usage QMap.
	@return a reference to a QMap<Stitch::Type type, int quantity>
	*/
const QMap<Stitch::Type, int> &StitchQueue::usedStitches()
{
	return m_usedStitches;
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
