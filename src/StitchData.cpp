/********************************************************************************
 *	Copyright (C) 2010 by Stephen Allewell					*
 *	stephen@mirramar.adsl24.co.uk						*
 *										*
 *	This program is free software; you can redistribute it and/or modify	*
 *	it under the terms of the GNU General Public License as published by	*
 *	the Free Software Foundation; either version 2 of the License, or	*
 *	(at your option) any later version.					*
 ********************************************************************************/


#include "StitchData.h"


FlossUsage::FlossUsage()
	:	backstitchCount(0),
		backstitchLength(0.0)
{
}


double FlossUsage::totalLength() const
{
	return stitchLength()+backstitchLength;
}


double FlossUsage::stitchLength() const
{
	double total = 0;

	foreach (double length, stitchLengths)
		total += length;

	return total;
}


int FlossUsage::totalStitches() const
{
	return stitchCount()+backstitchCount;
}


int FlossUsage::stitchCount() const
{
	int total = 0;

	foreach (int count, stitchCounts)
		total += count;

	return total;
}


StitchData::StitchData()
{
}


StitchData::~StitchData()
{
	clear();
}


void StitchData::clear()
{
	QHashIterator<int, QHash<int, StitchQueue *> > rowIterator(m_stitches);
	while (rowIterator.hasNext())
	{
		rowIterator.next();
		qDeleteAll(m_stitches[rowIterator.key()]);
		m_stitches[rowIterator.key()].clear();
	}
	m_stitches.clear();

	qDeleteAll(m_backstitches);
	m_backstitches.clear();

	qDeleteAll(m_knots);
	m_knots.clear();
}


int StitchData::width() const
{
	return m_width;
}


int StitchData::height() const
{
	return m_height;
}


void StitchData::resize(int width, int height)
{
	m_width = width;
	m_height = height;
}


QRect StitchData::extents() const
{
	QRect extentsRect;

	bool first = true;
	QHashIterator<int, QHash<int, StitchQueue *> > columnIterator(m_stitches);
	while (columnIterator.hasNext())
	{
		columnIterator.next();
		int x = columnIterator.key();
		QHashIterator<int, StitchQueue *> rowIterator(columnIterator.value());
		while (rowIterator.hasNext())
		{
			rowIterator.next();
			int y = rowIterator.key();
			if (first)
			{
				extentsRect = QRect(x*2, y*2, 2, 2);			// initialise in snap coordinates
				first = false;
			}
			else
				extentsRect = extentsRect.unite(QRect(x*2, y*2, 2, 2));	// update with snap coordinates
		}
	}

	QListIterator<Backstitch *> backstitchIterator(m_backstitches);
	if (backstitchIterator.hasNext())
	{
		Backstitch *backstitch = backstitchIterator.next();
		int minX = std::min(backstitch->start.x(), backstitch->end.x());
		int minY = std::min(backstitch->start.y(), backstitch->end.y());
		int maxX = std::max(backstitch->start.x(), backstitch->end.x());
		int maxY = std::max(backstitch->start.y(), backstitch->end.y());
		while (backstitchIterator.hasNext())
		{
			backstitch = backstitchIterator.next();
			minX = std::min(minX, std::min(backstitch->start.x(), backstitch->end.x()));
			minY = std::min(minY, std::min(backstitch->start.y(), backstitch->end.y()));
			maxX = std::max(maxX, std::max(backstitch->start.x(), backstitch->end.x()));
			maxY = std::max(maxY, std::max(backstitch->start.y(), backstitch->end.y()));
		}
		if (extentsRect.isValid())
		{
			if (minX < extentsRect.left())
				extentsRect.setLeft(minX);
			if (minY < extentsRect.top())
				extentsRect.setTop(minY);
			if (maxX > extentsRect.right())
				extentsRect.setRight(maxX);
			if (maxY > extentsRect.bottom())
				extentsRect.setBottom(maxY);
		}
		else
		{
			extentsRect.setLeft(minX);
			extentsRect.setTop(minY);
			extentsRect.setRight(maxX);
			extentsRect.setBottom(maxY);
		}
	}

	QListIterator<Knot *> knotIterator(m_knots);
	if (knotIterator.hasNext())
	{
		Knot *knot = knotIterator.next();
		int minX = knot->position.x();
		int maxX = minX;
		int minY = knot->position.y();
		int maxY = minY;
		while (knotIterator.hasNext())
		{
			knot = knotIterator.next();
			int x = knot->position.x();
			int y = knot->position.y();
			minX = std::min(x, minX);
			minY = std::min(y, minY);
			maxX = std::max(x, maxX);
			maxY = std::max(y, maxY);
		}
		if (extentsRect.isValid())
		{
			if (minX < extentsRect.left())
				extentsRect.setLeft(minX);
			if (minY < extentsRect.top())
				extentsRect.setTop(minY);
			if (maxX > extentsRect.right())
				extentsRect.setRight(maxX);
			if (maxY > extentsRect.bottom())
				extentsRect.setBottom(maxY);
		}
		else
		{
			extentsRect.setLeft(minX);
			extentsRect.setTop(minY);
			extentsRect.setRight(maxX);
			extentsRect.setBottom(maxY);
		}
	}

	// extentsRect is in snap coordinates
	// if width or height is zero, change to 2;
	if (extentsRect.width() == 0)
		extentsRect.setWidth(2);
	if (extentsRect.height() == 0)
		extentsRect.setHeight(2);

	extentsRect.adjust(-(extentsRect.left()%2), -(extentsRect.top()%2), extentsRect.right()%2, extentsRect.bottom()%2);

	return QRect(extentsRect.left()/2, extentsRect.top()/2, extentsRect.width()/2, extentsRect.height()/2);
}


void StitchData::movePattern(int dx, int dy)
{
	QRect extentsRect = extents();
	int xStart = extentsRect.left();
	int yStart = extentsRect.top();
	int xEnd = extentsRect.right();
	int yEnd = extentsRect.bottom();
	int xInc;
	int yInc;

	if (dx < 0) // moving left
	{
		++xEnd;
		xInc = 1;
	}
	else // moving right
	{
		int x = xStart;
		xStart = xEnd;
		xEnd = x-1;
		xInc = -1;
	}

	if (dy < 0) // moving up
	{
		++yEnd;
		yInc = 1;
	}
	else // moving down
	{
		int y = yStart;
		yStart = yEnd;
		yEnd = y-1;
		yInc = -1;
	}


	for (int x = xStart ; x != xEnd ; x += xInc)
	{
		if (m_stitches.contains(x))
		{
			QHash<int, StitchQueue *> column = m_stitches.take(x);
			for (int y = yStart ; y != yEnd ; y += yInc)
			{
				if (column.contains(y))
				{
					StitchQueue *stitchQueue = column.take(y);
					if (!stitchQueue->isEmpty())
						column.insert(y+dy, stitchQueue);
				}
			}
			m_stitches.insert(x+dx, column);
		}
	}

	dx *= 2;
	dy *= 2;

	QPoint offset(dx, dy);

	QListIterator<Backstitch *> backstitchIterator(m_backstitches);
	while (backstitchIterator.hasNext())
	{
		Backstitch *backstitch = backstitchIterator.next();
		backstitch->move(QPoint(dx, dy));
	}

	QListIterator<Knot *> knotIterator(m_knots);
	while (knotIterator.hasNext())
	{
		Knot *knot = knotIterator.next();
		knot->move(QPoint(dx, dy));
	}
}


void StitchData::addStitch(const QPoint &position, Stitch::Type type, int colorIndex)
{
	StitchQueue *stitchQueue = 0;
	int x = position.x();
	int y = position.y();
	if (m_stitches.contains(x) && m_stitches[x].contains(y))
	{
		stitchQueue = m_stitches[x][y];
	}

	if (stitchQueue == 0)
	{
		stitchQueue = new StitchQueue;
		m_stitches[x][y] = stitchQueue;
	}
	stitchQueue->add(type, colorIndex);
}


void StitchData::deleteStitch(const QPoint &position, Stitch::Type type, int colorIndex)
{
	int x = position.x();
	int y = position.y();
	if (m_stitches.contains(x) && m_stitches[x].contains(y))
	{
		StitchQueue *stitchQueue = m_stitches[x][y];
		if (stitchQueue)
		{
			if (stitchQueue->remove(type, colorIndex) == 0)
			{
				m_stitches[x].remove(y);
				if (m_stitches[x].count() == 0)
				{
					m_stitches.remove(x);
				}
			}
		}
	}
}


StitchQueue *StitchData::stitchQueueAt(const QPoint &position)
{
	StitchQueue *stitchQueue = 0;
	int x = position.x();
	int y = position.y();

	if (m_stitches.contains(x) && m_stitches[x].contains(y))
	{
		stitchQueue = m_stitches[x][y];
	}

	return stitchQueue;
}


StitchQueue *StitchData::takeStitchQueueAt(const QPoint &position)
{
	StitchQueue *stitchQueue = stitchQueueAt(position);

	if (stitchQueue)
	{
		m_stitches[position.x()].remove(position.y());
		if (m_stitches[position.x()].isEmpty())
			m_stitches.remove(position.x());
	}

	return stitchQueue;
}


StitchQueue *StitchData::replaceStitchQueueAt(const QPoint &position, StitchQueue *stitchQueue)
{
	StitchQueue *originalQueue = stitchQueueAt(position);

	m_stitches[position.x()][position.y()] = stitchQueue;

	return originalQueue;
}


void StitchData::addBackstitch(const QPoint &start, const QPoint &end, int colorIndex)
{
	m_backstitches.append(new Backstitch(start, end, colorIndex));
}


void StitchData::addBackstitch(Backstitch *backstitch)
{
	m_backstitches.append(backstitch);
}


Backstitch *StitchData::takeBackstitch(const QPoint &start, const QPoint &end, int colorIndex)
{
	Backstitch *removed = 0;

	QMutableListIterator<Backstitch *> iterator(m_backstitches);
	while (iterator.hasNext())
	{
		Backstitch *backstitch = iterator.next();
		if (backstitch->contains(start) && backstitch->contains(end))
		{
			if ((colorIndex == -1) || (backstitch->colorIndex == colorIndex))
			{
				iterator.remove();
				removed = backstitch;
				break;
			}
		}
	}

	return removed;
}


Backstitch *StitchData::takeBackstitch(Backstitch *backstitch)
{
	Backstitch *removed = 0;
	
	QMutableListIterator<Backstitch *> iterator(m_backstitches);
	while (iterator.hasNext())
	{
		if (backstitch == iterator.next())
		{
			removed = backstitch;
			iterator.remove();
			break;
		}
	}
	
	return removed;
}


void StitchData::addFrenchKnot(const QPoint &position, int colorIndex)
{
	m_knots.append(new Knot(position, colorIndex));
}


void StitchData::addFrenchKnot(Knot *knot)
{
	m_knots.append(knot);
}


Knot *StitchData::takeFrenchKnot(const QPoint &position, int colorIndex)
{
	Knot *removed;

	QMutableListIterator<Knot *> iterator(m_knots);
	while (iterator.hasNext())
	{
		Knot *knot = iterator.next();
		if (knot->position == position)
		{
			if ((colorIndex == -1) || (knot->colorIndex == colorIndex))
			{
				iterator.remove();
				removed = knot;
				break;
			}
		}
	}

	return removed;
}


Knot *StitchData::takeFrenchKnot(Knot *knot)
{
	Knot *removed = 0;
	
	QMutableListIterator<Knot *> iterator(m_knots);
	while (iterator.hasNext())
	{
		if (knot == iterator.next())
		{
			removed = knot;
			iterator.remove();
			break;
		}
	}
	
	return removed;
}


QListIterator<Backstitch *> StitchData::backstitchIterator()
{
	return QListIterator<Backstitch *>(m_backstitches);
}


QMutableListIterator<Backstitch *> StitchData::mutableBackstitchIterator()
{
	return QMutableListIterator<Backstitch *>(m_backstitches);
}


QListIterator<Knot *> StitchData::knotIterator()
{
	return QListIterator<Knot *>(m_knots);
}


QMutableListIterator<Knot *> StitchData::mutableKnotIterator()
{
	return QMutableListIterator<Knot *>(m_knots);
}


QMap<int, FlossUsage> StitchData::flossUsage()
{
	QMap<int, FlossUsage> usage;
	static QMap<Stitch::Type, double> lengths;
	if (!lengths.count())
	{
		lengths.insert(Stitch::Delete, 0.0);
		lengths.insert(Stitch::TLQtr, 0.707107 + 0.5);
		lengths.insert(Stitch::TRQtr, 0.707107 + 0.5);
		lengths.insert(Stitch::BLQtr, 0.707107 + 0.5);
		lengths.insert(Stitch::BTHalf, 1.414213 + 1.0);
		lengths.insert(Stitch::TL3Qtr, 1.414213 + 0.707107 + 1.0 + 0.5);
		lengths.insert(Stitch::BRQtr, 0.707107 + 0.5);
		lengths.insert(Stitch::TBHalf, 1.414213 + 1.0);
		lengths.insert(Stitch::TR3Qtr, 1.414213 + 0.707107 + 1.0 + 0.5);
		lengths.insert(Stitch::BL3Qtr, 1.414213 + 0.707107 + 1.0 + 0.5);
		lengths.insert(Stitch::BR3Qtr, 1.414213 + 0.707107 + 1.0 + 0.5);
		lengths.insert(Stitch::Full, 1.414213 + 1.414213 + 1.0 + 1.0);
		lengths.insert(Stitch::TLSmallHalf, 0.707107 + 0.5);
		lengths.insert(Stitch::TRSmallHalf, 0.707107 + 0.5);
		lengths.insert(Stitch::BLSmallHalf, 0.707107 + 0.5);
		lengths.insert(Stitch::BRSmallHalf, 0.707107 + 0.5);
		lengths.insert(Stitch::TLSmallFull, 0.707107 + 0.5 + 0.707107 + 0.5);
		lengths.insert(Stitch::TRSmallFull, 0.707107 + 0.5 + 0.707107 + 0.5);
		lengths.insert(Stitch::BLSmallFull, 0.707107 + 0.5 + 0.707107 + 0.5);
		lengths.insert(Stitch::BRSmallFull, 0.707107 + 0.5 + 0.707107 + 0.5);
		lengths.insert(Stitch::FrenchKnot, 2.0);
	}

	QHashIterator<int, QHash<int, StitchQueue *> > columnIterator(m_stitches);
	while (columnIterator.hasNext())
	{
		columnIterator.next();
		QHashIterator<int, StitchQueue *> rowIterator(columnIterator.value());
		while (rowIterator.hasNext())
		{
			rowIterator.next();
			StitchQueue *stitchQueue = rowIterator.value();
			if (stitchQueue)
			{
				QListIterator<Stitch *> stitchIterator(*stitchQueue);
				while (stitchIterator.hasNext())
				{
					Stitch *stitch = stitchIterator.next();
					usage[stitch->colorIndex].stitchCounts[stitch->type]++;
					usage[stitch->colorIndex].stitchLengths[stitch->type] += lengths[stitch->type];
				}
			}
		}
	}


	QListIterator<Backstitch *> backstitchIterator(m_backstitches);
	while (backstitchIterator.hasNext())
	{
		Backstitch *backstitch = backstitchIterator.next();
		usage[backstitch->colorIndex].backstitchCount++;
		usage[backstitch->colorIndex].backstitchLength += QPoint(backstitch->start - backstitch->end).manhattanLength();
	}

	QListIterator<Knot *> knotIterator(m_knots);
	while (knotIterator.hasNext())
	{
		Knot *knot = knotIterator.next();
		usage[knot->colorIndex].stitchCounts[Stitch::FrenchKnot]++;
		usage[knot->colorIndex].stitchLengths[Stitch::FrenchKnot] += lengths[Stitch::FrenchKnot];
	}

	return usage;
}


QDataStream &operator<<(QDataStream &stream, const StitchData &stitchData)
{
	stream << qint32(stitchData.version);
	stream << qint32(stitchData.m_width);
	stream << qint32(stitchData.m_height);

	QHashIterator<int, QHash<int, StitchQueue *> > columnIterator(stitchData.m_stitches);
	stream << qint32(stitchData.m_stitches.count());
	while (columnIterator.hasNext())
	{
		columnIterator.next();
		int column = columnIterator.key();
		QHashIterator<int, StitchQueue *> rowIterator(columnIterator.value());
		stream << qint32(stitchData.m_stitches[column].count());
		while (rowIterator.hasNext())
		{
			rowIterator.next();
			int row = rowIterator.key();
			StitchQueue *stitchQueue = rowIterator.value();
			stream << qint32(column);
			stream << qint32(row);
			stream << *stitchQueue;
		}
	}
	
	QListIterator<Backstitch *> backstitchIterator(stitchData.m_backstitches);
	stream << qint32(stitchData.m_backstitches.count());
	while (backstitchIterator.hasNext())
	{
		stream << *(backstitchIterator.next());
	}
	
	QListIterator<Knot *> knotIterator(stitchData.m_knots);
	stream << qint32(stitchData.m_knots.count());
	while (knotIterator.hasNext())
	{
		stream << *(knotIterator.next());
	}

	return stream;
}


QDataStream  &operator>>(QDataStream &stream, StitchData &stitchData)
{
	qint32 version;
	qint32 width;
	qint32 height;
	qint32 layers;
	qint32 columns;
	qint32 rows;
	qint32 count;

	stitchData.clear();
	
	stream >> version;
	switch (version)
	{
		case 102:
			stream >> width;
			stream >> height;
			stitchData.m_width = width;
			stitchData.m_height = height;
			
			stream >> columns;
			while (columns--)
			{
				stream >> rows;
				while (rows--)
				{
					qint32 column;
					qint32 row;

					stream >> column;
					stream >> row;

					StitchQueue *stitchQueue = new StitchQueue;
					stitchData.m_stitches[column][row] = stitchQueue;
					stream >> *stitchQueue;
				}
			}
			
			stream >> count;
			while (count--)
			{
				Backstitch *backstitch = new Backstitch;
				stream >> *(backstitch);
				stitchData.addBackstitch(backstitch);
			}
			
			stream >> count;
			while (count--)
			{
				Knot *knot = new Knot;
				stream >> *knot;
				stitchData.addFrenchKnot(knot);
			}
			break;
						
		case 101:
			stream >> width;
			stream >> height;
			stitchData.m_width = width;
			stitchData.m_height = height;
			
			stream >> columns;
			while (columns--)
			{
				stream >> rows;
				while (rows--)
				{
					qint32 column;
					qint32 row;

					stream >> column;
					stream >> row;

					StitchQueue *stitchQueue = new StitchQueue;
					stitchData.m_stitches[column][row] = stitchQueue;
					stream >> *stitchQueue;
				}
			}
			break;
			
		case 100:
			stream >> width;
			stream >> height;
			stitchData.m_width = width;
			stitchData.m_height = height;

			stream >> layers;
			while (layers--)
			{
				stream >> columns;
				while (columns--)
				{
					stream >> rows;
					while (rows--)
					{
						qint32 layer;
						qint32 column;
						qint32 row;

						stream >> layer;
						stream >> column;
						stream >> row;

						StitchQueue *stitchQueue = new StitchQueue;
						stitchData.m_stitches[column][row] = stitchQueue;
						stream >> *stitchQueue;
					}
				}
			}
			break;

		default:
			// not supported
			// throw exception
			break;
	}

	return stream;
}
