/********************************************************************************
 *	Copyright (C) 2010 by Stephen Allewell					*
 *	stephen@mirramar.adsl24.co.uk						*
 *										*
 *	This program is free software; you can redistribute it and/or modify	*
 *	it under the terms of the GNU General Public License as published by	*
 *	the Free Software Foundation; either version 2 of the License, or	*
 *	(at your option) any later version.					*
 ********************************************************************************/


/**
	@file
	Implement classes for Stitch, StitchQueue, Backstitch and Knot.
	*/


#include "Stitch.h"

#include <KLocale>

#include "Exceptions.h"


/**
	Constructor.
	Used when creating instances for streaming.
	*/
Stitch::Stitch()
{
}


/**
	Constructor.
	@param t stitch type
	@param i color index
	*/
Stitch::Stitch(Stitch::Type t, int i)
	:	type(t),
		colorIndex(i)

{
}


QDataStream &operator<<(QDataStream &stream, const Stitch &stitch)
{
	stream << qint32(stitch.version);
	stream << qint32(stitch.type);
	stream << qint32(stitch.colorIndex);
	return stream;
}


QDataStream &operator>>(QDataStream &stream, Stitch &stitch)
{
	qint32 version;
	qint32 type;
	qint32 colorIndex;

	stream >> version;
	switch (version)
	{
		case 100:
			stream >> type;
			stream >> colorIndex;
			stitch.type = static_cast<Stitch::Type>(type);
			stitch.colorIndex = colorIndex;
			break;

		default:
			throw InvalidFileVersion(QString(i18n("Stitch version %1", version)));
			break;
	}

	return stream;
}


/**
	Constructor.
	*/
StitchQueue::StitchQueue()
	:	QQueue<Stitch *>()
{
}


StitchQueue::~StitchQueue()
{
	while (!isEmpty())
	{
		delete dequeue();
	}
}


StitchQueue::StitchQueue(StitchQueue *stitchQueue)
{
	QListIterator<Stitch *> stitchIterator(*stitchQueue);
	while (stitchIterator.hasNext())
	{
		Stitch *stitch = stitchIterator.next();
		enqueue(new Stitch(stitch->type, stitch->colorIndex));
	}
}


/**
	Add a stitch to the queue.
	@param type a Stitch::Type value to be added
	@param colorIndex the palette index
	*/
int StitchQueue::add(Stitch::Type type, int colorIndex)
{
	bool miniStitch = (type & 192);
	int stitchCount = count();
	int stitches = stitchCount;

	if (!miniStitch)
	{
		// try and merge it with any existing stitches in the queue to update the stitch being added
		while (stitches--)
		{
			Stitch *stitch = dequeue();
			if (!(stitch->type & 192)) // so we don't try and merge existing mini stitches
				if (stitch->colorIndex == colorIndex)
					type = (Stitch::Type)(type | stitch->type);
			enqueue(stitch);
		}
	}

	switch (type) // add the new stitch checking for illegal types
	{
		case 3: // TLQtr and TRQtr
			enqueue(new Stitch(Stitch::TLQtr, colorIndex));
			enqueue(new Stitch(Stitch::TRQtr, colorIndex));
			break;
		case 5: // TLQtr and BLQtr
			enqueue(new Stitch(Stitch::TLQtr, colorIndex));
			enqueue(new Stitch(Stitch::BLQtr, colorIndex));
			break;
		case 10: // TRQtr and BRQtr
			enqueue(new Stitch(Stitch::TRQtr, colorIndex));
			enqueue(new Stitch(Stitch::BRQtr, colorIndex));
			break;
		case 12: // BLQtr and BRQtr
			enqueue(new Stitch(Stitch::BLQtr, colorIndex));
			enqueue(new Stitch(Stitch::BRQtr, colorIndex));
			break;
		default: // other values are acceptable as is including mini stitches
			enqueue(new Stitch(type, colorIndex));
			break;
	}

	/** iterate the queue of existing stitches for any that have been overwritten by the new stitch */
	while (stitchCount--)								// while there are existing stitches
	{
		Stitch *stitch = dequeue();						// get the stitch at the head of the queue
		Stitch::Type currentStitchType = (Stitch::Type)(stitch->type);		// and find its type
		int currentColorIndex = stitch->colorIndex;				// and color
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
					enqueue(new Stitch(Stitch::TLQtr, currentColorIndex));
					enqueue(new Stitch(Stitch::TRQtr, currentColorIndex));
					changeMask = Stitch::Delete;
					break;
				case 5:
					enqueue(new Stitch(Stitch::TLQtr, currentColorIndex));
					enqueue(new Stitch(Stitch::BLQtr, currentColorIndex));
					changeMask = Stitch::Delete;
					break;
				case 10:
					enqueue(new Stitch(Stitch::TRQtr, currentColorIndex));
					enqueue(new Stitch(Stitch::BRQtr, currentColorIndex));
					changeMask = Stitch::Delete;
					break;
				case 12:
					enqueue(new Stitch(Stitch::BLQtr, currentColorIndex));
					enqueue(new Stitch(Stitch::BRQtr, currentColorIndex));
					changeMask = Stitch::Delete;
					break;
				default:
					// other values are acceptable as is
					break;
			}

			if (changeMask)				// Check if there is anything left of the original stitch, Stitch::Delete is 0
			{
				stitch->type = changeMask;	// and change stitch type to the changeMask value
				enqueue(stitch);		// and then add it back to the queue
			}
		}
		else
		{
			enqueue(stitch);
		}
	}
	return count();
}


/**
	Delete a stitch from the queue.
	@param type a Stitch::Type mask to remove, Stitch::Delete for all stitches.
	@param colorIndex a palette index mask, -1 for all flosses.
	@return the number of stitches left in the queue
	*/
int StitchQueue::remove(Stitch::Type type, int colorIndex)
{
	int stitchCount = count();
	if (type == Stitch::Delete)
	{
		while (stitchCount--)
		{
			Stitch *stitch = dequeue();
			if ((colorIndex != -1) && (stitch->colorIndex != colorIndex))
			{
				enqueue(stitch);
			}
		}
	}
	else
	{
		while (stitchCount--)
		{
			Stitch *stitch = dequeue();
			if ((stitch->type != type) || ((colorIndex != -1) && (stitch->colorIndex != colorIndex)))
			{
				if ((stitch->type & type) && ((colorIndex == -1) || (stitch->colorIndex == colorIndex)) && ((stitch->type & 192) == 0))
				{
					// the mask covers a part of the current stitch and is the correct color or if the color doesn't matter
					Stitch::Type changeMask = (Stitch::Type)(stitch->type ^ type);
					int index = stitch->colorIndex;
					switch (changeMask)
					{
						// changeMask contains what is left of the original stitch after deleting the maskStitch
						// it may contain illegal values, so these are checked for
						case 3:
							enqueue(new Stitch(Stitch::TLQtr, index));
							enqueue(new Stitch(Stitch::TRQtr, index));
							break;
						case 5:
							enqueue(new Stitch(Stitch::TLQtr, index));
							enqueue(new Stitch(Stitch::BLQtr, index));
							break;
						case 10:
							enqueue(new Stitch(Stitch::TRQtr, index));
							enqueue(new Stitch(Stitch::BRQtr, index));
							break;
						case 12:
							enqueue(new Stitch(Stitch::BLQtr, index));
							enqueue(new Stitch(Stitch::BRQtr, index));
							break;
						default:
							enqueue(new Stitch((Stitch::Type)changeMask, index));
							break;
					}
				}
				else
				{
					enqueue(stitch);
				}
			}
		}
	}
	return count();
}


QDataStream &operator<<(QDataStream &stream, const StitchQueue &stitchQueue)
{
	stream << qint32(stitchQueue.version);
	stream << qint32(stitchQueue.count());
	QListIterator<Stitch *> stitchIterator(stitchQueue);
	while (stitchIterator.hasNext())
	{
		stream << *stitchIterator.next();
	}

	return stream;
}


QDataStream &operator>>(QDataStream &stream, StitchQueue &stitchQueue)
{
	qint32 version;
	qint32 count;

	stream >> version;
	switch (version)
	{
		case 100:
			stream >> count;
			while (count--)
			{
				Stitch *stitch = new Stitch;
				stitchQueue.enqueue(stitch);
				stream >> *stitch;
			}
			break;

		default:
			throw InvalidFileVersion(QString(i18n("Stitch queue version %1", version)));
			break;
	}
}


/**
	Constructor.
	Used when creating instances for streaming.
	*/
Backstitch::Backstitch()
{
}


/**
	Constructor.
	@param s start of backstitch line
	@param e end of backstitch line
	@param i palette index
	*/
Backstitch::Backstitch(const QPoint &s, const QPoint &e, int i)
	:	start(s),
		end(e),
		colorIndex(i)
{
}


/**
	Test if the backstitch start or end is equal to the
	requested point.
	@param point point to test
	@return true if p starts or ends the backstitch line,
	false other wise.
	*/
bool Backstitch::contains(const QPoint &point) const
{
	if (start == point || end == point)
		return true;
	return false;
}


void Backstitch::move(const QPoint &offset)
{
	start += offset;
	end += offset;
}


QDataStream &operator<<(QDataStream &stream, const Backstitch &backstitch)
{
	stream << qint32(backstitch.version);
	stream << backstitch.start;
	stream << backstitch.end;
	stream << qint32(backstitch.colorIndex);
	return stream;
}


QDataStream &operator>>(QDataStream &stream, Backstitch &backstitch)
{
	qint32 version;
	qint32 colorIndex;

	stream >> version;
	switch (version)
	{
		case 100:
			stream >> backstitch.start;
			stream >> backstitch.end;
			stream >> colorIndex;
			backstitch.colorIndex = colorIndex;
			break;

		default:
			throw InvalidFileVersion(QString(i18n("Backstitch version %1", version)));
			break;
	}

	return stream;
}


/**
	Constructor.
	Used when creating instances for streaming.
	*/
Knot::Knot()
{
}


/**
	Constructor.
	@param p position for the knot
	@param i palette index
	*/
Knot::Knot(const QPoint &p, int i)
	:	position(p),
		colorIndex(i)
{
}


void Knot::move(const QPoint &offset)
{
	position += offset;
}


QDataStream &operator<<(QDataStream &stream, const Knot &knot)
{
	stream << qint32(knot.version);
	stream << knot.position;
	stream << knot.colorIndex;
	return stream;
}


QDataStream &operator>>(QDataStream &stream, Knot &knot)
{
	qint32 version;
	QPoint position;
	qint32 colorIndex;

	stream >> version;
	switch (version)
	{
		case 100:
			stream >> knot.position;
			stream >> colorIndex;
			knot.colorIndex = colorIndex;
			break;

		default:
			throw InvalidFileVersion(QString(i18n("Knot version %1", version)));
			break;
	}

	return stream;
}
