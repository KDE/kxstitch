/********************************************************************************
 *	Copyright (C) 2010 by Stephen Allewell					*
 *	stephen@mirramar.adsl24.co.uk						*
 *										*
 *	This program is free software; you can redistribute it and/or modify	*
 *	it under the terms of the GNU General Public License as published by	*
 *	the Free Software Foundation; either version 2 of the License, or	*
 *	(at your option) any later version.					*
 ********************************************************************************/


#include <KDebug>
#include "StitchData.h"


StitchData::StitchData()
{
}


StitchData::~StitchData()
{
	clear();
}


void StitchData::clear()
{
	QHashIterator<int, QHash<int, QHash<int, StitchQueue *> > > layerIterator(m_stitches);
	while (layerIterator.hasNext())
	{
		layerIterator.next();
		QHashIterator<int, QHash<int, StitchQueue *> > rowIterator(layerIterator.value());
		while (rowIterator.hasNext())
		{
			rowIterator.next();
			qDeleteAll(m_stitches[layerIterator.key()][rowIterator.key()]);
			m_stitches[layerIterator.key()][rowIterator.key()].clear();
		}
		m_stitches[layerIterator.key()].clear();
	}
	m_stitches.clear();

	QHashIterator<int, QList<Backstitch *> > backstitchListIterator(m_backstitches);
	while (layerIterator.hasNext())
	{
		backstitchListIterator.next();
		qDeleteAll(m_backstitches[backstitchListIterator.key()]);
		m_backstitches[backstitchListIterator.key()].clear();
	}
	m_backstitches.clear();

	QHashIterator<int, QList<Knot *> > knotListIterator(m_knots);
	while (knotListIterator.hasNext())
	{
		knotListIterator.next();
		qDeleteAll(m_knots[knotListIterator.key()]);
		m_knots[knotListIterator.key()].clear();
	}
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
	QHashIterator<int, QHash<int, QHash<int, StitchQueue *> > > stitchLayerIterator(m_stitches);
	while (stitchLayerIterator.hasNext())
	{
		stitchLayerIterator.next();
		QHashIterator<int, QHash<int, StitchQueue *> > columnIterator(stitchLayerIterator.value());
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
	}

	QHashIterator<int, QList<Backstitch *> > backstitchLayerIterator(m_backstitches);
	while (backstitchLayerIterator.hasNext())
	{
		backstitchLayerIterator.next();
		QListIterator<Backstitch *> backstitchIterator(backstitchLayerIterator.value());
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
	}

	QHashIterator<int, QList<Knot *> > knotLayerIterator(m_knots);
	while (knotLayerIterator.hasNext())
	{
		knotLayerIterator.next();
		QListIterator<Knot *> knotIterator(knotLayerIterator.value());
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


	QHashIterator<int, QHash<int, QHash<int, StitchQueue *> > > stitchLayerIterator(m_stitches);
	while (stitchLayerIterator.hasNext())
	{
		stitchLayerIterator.next();
		int layer = stitchLayerIterator.key();

		for (int x = xStart ; x != xEnd ; x += xInc)
		{
			if (m_stitches[layer].contains(x))
			{
				QHash<int, StitchQueue *> column = m_stitches[layer].take(x);
				for (int y = yStart ; y != yEnd ; y += yInc)
				{
					if (column.contains(y))
					{
						StitchQueue *stitchQueue = column.take(y);
						if (!stitchQueue->isEmpty())
							column.insert(y+dy, stitchQueue);
					}
				}
				m_stitches[layer].insert(x+dx, column);
			}
		}
	}

	dx *= 2;
	dy *= 2;

	QPoint offset(dx, dy);

	QHashIterator<int, QList<Backstitch *> > backstitchLayerIterator(m_backstitches);
	while (backstitchLayerIterator.hasNext())
	{
		backstitchLayerIterator.next();
		QListIterator<Backstitch *> backstitchIterator(backstitchLayerIterator.value());
		while (backstitchIterator.hasNext())
		{
			Backstitch *backstitch = backstitchIterator.next();
			backstitch->move(QPoint(dx, dy));
		}
	}

	QHashIterator<int, QList<Knot *> > knotLayerIterator(m_knots);
	while (knotLayerIterator.hasNext())
	{
		knotLayerIterator.next();
		QListIterator<Knot *> knotIterator(knotLayerIterator.value());
		while (knotIterator.hasNext())
		{
			Knot *knot = knotIterator.next();
			knot->move(QPoint(dx, dy));
		}
	}
}


StitchData StitchData::cut(const QRect &area, const QList<int> &layerMask, const QList<int> &colorMask, const QList<Stitch::Type> &stitchMask, bool excludeBackstitches, bool excludeKnots, bool crossingBackstitches)
{
#if 0
	StitchData subSet;
	QIntListIterator layerIterator(layerMask);
	while (layerIterator.hasNext())
	{
		layerIterator.next();
		int layer = layerIterator.key();
		QHashIterator<int, QHash<int, StitchQueue *> > columnIterator(d->m_stitches[layerIterator.value()]);
		while (columnIterator.hasNext())
		{
			columnIterator.next();
			int column = columnIterator.key();
			QHashIterator<int, StitchQueue *> rowIterator(columnIterator.value());
			while (rowIterator.hasNext());
			{
				rowIterator.next();
				int row = rowIterator.key();
				StitchQueue queue = rowIterator.value();
				StitchQueue newQueue;
				QListIterator queueIterator(queue);
				while (queueIterator.hasNext())
				{
					Stitch stitch = queueIterator.next();
					if (stitchMask.contains(stitch.type()) && colorMask.contains(stitch.color()))
					{
						Stitch newStitch(stitch.type(), stitch.floss());
						newQueue.push(newStitch);
					}
				}
				subSet[layer][colum][row] = newQueue;
			}
		}

		if (!excludeBackstitches)
		{
			QListIterator<Backstitch>  backstitchIterator m_backstitches[layer];
			while (backstitchIterator.hasNext())
			{
				Backstitch backstitch = backstitchIterator.next();
				if (colorMask.contains(backstitch.floss()))
				{
					QRect backstitchRect(backstitch.start(), backstitch.end())
					// TODO calculate if intersection crosses the selected area
				}
			}
		}

		if (!excludeKnots)
		{
			QListIterator<Knot *> knotIterator m_knots[layer];
			// TODO calculate if the knot is in the selected area
		}
	}
#endif
}


StitchData StitchData::copy(const QRect &area, const QList<int> &layerMask, const QList<int> &colorMask, const QList<Stitch::Type> &stitchMask, bool excludeBackstitches, bool excludeKnots, bool crossingBackstitches) const
{
}


void StitchData::paste(const StitchData &stitchData, const QPoint &, bool merge)
{
}


void StitchData::addStitch(int layer, const QPoint &position, Stitch::Type type, int colorIndex)
{
	StitchQueue *stitchQueue = 0;
	int x = position.x();
	int y = position.y();
	if (m_stitches.contains(layer) && m_stitches[layer].contains(x) && m_stitches[layer][x].contains(y))
	{
		stitchQueue = m_stitches[layer][x][y];
	}

	if (stitchQueue == 0)
	{
		stitchQueue = new StitchQueue;
		m_stitches[layer][x][y] = stitchQueue;
	}
	stitchQueue->add(type, colorIndex);
}


void StitchData::deleteStitch(int layer, const QPoint &position, Stitch::Type type, int colorIndex)
{
	int x = position.x();
	int y = position.y();
	if (m_stitches.contains(layer) && m_stitches[layer].contains(x) && m_stitches[layer][x].contains(y))
	{
		StitchQueue *stitchQueue = m_stitches[layer][x][y];
		if (stitchQueue)
		{
			if (stitchQueue->remove(type, colorIndex) == 0)
			{
				m_stitches[layer][x].remove(y);
				if (m_stitches[layer][x].count() == 0)
				{
					m_stitches[layer].remove(x);
					if (m_stitches[layer].count() == 0)
						m_stitches.remove(layer);
				}
			}
		}
	}
}


StitchQueue *StitchData::stitchQueueAt(int layer, const QPoint &position)
{
	StitchQueue *stitchQueue = 0;
	int x = position.x();
	int y = position.y();

	if (m_stitches.contains(layer) && m_stitches[layer].contains(x) && m_stitches[layer][x].contains(y))
	{
		stitchQueue = m_stitches[layer][x][y];
	}

	return stitchQueue;
}


StitchQueue *StitchData::takeStitchQueueAt(int layer, const QPoint &position)
{
	StitchQueue *stitchQueue = stitchQueueAt(layer, position);

	if (stitchQueue)
	{
		m_stitches[layer][position.x()][position.y()] = 0;
	}

	return stitchQueue;
}


StitchQueue *StitchData::replaceStitchQueueAt(int layer, const QPoint &position, StitchQueue *stitchQueue)
{
	StitchQueue *originalQueue = stitchQueueAt(layer, position);

	m_stitches[layer][position.x()][position.y()] = stitchQueue;

	return originalQueue;
}


void StitchData::addBackstitch(int layer, const QPoint &start, const QPoint &end, int colorIndex)
{
	m_backstitches[layer].append(new Backstitch(start, end, colorIndex));
}


Backstitch *StitchData::takeBackstitch(int layer, const QPoint &start, const QPoint &end, int colorIndex)
{
	Backstitch *backstitch;

	if (m_backstitches.contains(layer))
	{
		QMutableListIterator<Backstitch *> iterator(m_backstitches[layer]);
		while (iterator.hasNext())
		{
			backstitch = iterator.next();
			if (backstitch->contains(start) && backstitch->contains(end))
			{
				if ((colorIndex == -1) || (backstitch->colorIndex == colorIndex))
				{
					iterator.remove();
					if (m_backstitches[layer].count() == 0)
					{
						m_backstitches.remove(layer);
					}
					break;
				}
			}
		}
	}

	return backstitch;
}


void StitchData::addFrenchKnot(int layer, const QPoint &position, int colorIndex)
{
	m_knots[layer].append(new Knot(position, colorIndex));
}


Knot *StitchData::takeFrenchKnot(int layer, const QPoint &position, int colorIndex)
{
	Knot *knot;

	if (m_knots.contains(layer))
	{
		QMutableListIterator<Knot *> iterator(m_knots[layer]);
		while (iterator.hasNext())
		{
			Knot *knot = iterator.next();
			if (knot->position == position)
			{
				if ((colorIndex == -1) || (knot->colorIndex == colorIndex))
				{
					iterator.remove();
					if (m_knots[layer].count() == 0)
					{
						m_knots.remove(layer);
					}
					break;
				}
			}
		}
	}

	return knot;
}


QList<int> StitchData::stitchLayers() const
{
	QList<int> layerList;

	QHashIterator<int, QHash<int, QHash<int, StitchQueue *> > > layerIterator(m_stitches);
	while (layerIterator.hasNext())
	{
		layerIterator.next();
		layerList.append(layerIterator.key());
	}

	return layerList;
}


QList<int> StitchData::backstitchLayers() const
{
	QList<int> layerList;

	QHashIterator<int, QList<Backstitch *> > layerIterator(m_backstitches);
	while (layerIterator.hasNext())
	{
		layerIterator.next();
		layerList.append(layerIterator.key());
	}

	return layerList;
}


QList<int> StitchData::knotLayers() const
{
	QList<int> layerList;

	QHashIterator<int, QList<Knot *> > layerIterator(m_knots);
	while (layerIterator.hasNext())
	{
		layerIterator.next();
		layerList.append(layerIterator.key());
	}

	return layerList;
}


QListIterator<Backstitch *> StitchData::backstitchIterator(int layer)
{
	return QListIterator<Backstitch *>(m_backstitches[layer]);
}


QListIterator<Knot *> StitchData::knotIterator(int layer)
{
	return QListIterator<Knot *>(m_knots[layer]);
}


QMap<int, int> StitchData::usedFlosses()
{
	QMap<int, int> used;
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

	QHashIterator<int, QHash<int, QHash<int, StitchQueue *> > > stitchLayerIterator(m_stitches);
	while (stitchLayerIterator.hasNext())
	{
		stitchLayerIterator.next();
		QHashIterator<int, QHash<int, StitchQueue *> > stitchColumnIterator(stitchLayerIterator.value());
		while (stitchColumnIterator.hasNext())
		{
			stitchColumnIterator.next();
			QHashIterator<int, StitchQueue *> stitchRowIterator(stitchColumnIterator.value());
			while (stitchRowIterator.hasNext())
			{
				stitchRowIterator.next();
				StitchQueue *stitchQueue = stitchRowIterator.value();
				if (stitchQueue)
				{
					QListIterator<Stitch *> stitchIterator(*stitchQueue);
					while (stitchIterator.hasNext())
					{
						Stitch *stitch = stitchIterator.next();
						used[stitch->colorIndex] += lengths[stitch->type];
					}
				}
			}
		}
	}

	QHashIterator<int, QList<Backstitch *> > backstitchLayers(m_backstitches);
	while(backstitchLayers.hasNext())
	{
		backstitchLayers.next();
		QListIterator<Backstitch *> backstitchIterator(backstitchLayers.value());
		while (backstitchIterator.hasNext())
		{
			Backstitch *backstitch = backstitchIterator.next();
			used[backstitch->colorIndex] += QPoint(backstitch->start - backstitch->end).manhattanLength();
		}
	}

	QHashIterator<int, QList<Knot *> > knotLayers(m_knots);
	while (knotLayers.hasNext())
	{
		knotLayers.next();
		QListIterator<Knot *> knotIterator(knotLayers.value());
		while (knotIterator.hasNext())
		{
			Knot *knot = knotIterator.next();
			used[knot->colorIndex] += 4; // arbitrary value for a relative amount of floss to do a knot.
		}
	}

	return used;
}


QDataStream &operator<<(QDataStream &stream, const StitchData &stitchData)
{
	stream << qint32(stitchData.version);
	stream << qint32(stitchData.m_width);
	stream << qint32(stitchData.m_height);

	stream << qint32(stitchData.m_stitches.count());
	QHashIterator<int, QHash<int, QHash<int, StitchQueue *> > > layerIterator(stitchData.m_stitches);
	while (layerIterator.hasNext())
	{
		layerIterator.next();
		int layer = layerIterator.key();
		QHashIterator<int, QHash<int, StitchQueue *> > columnIterator(layerIterator.value());
		stream << qint32(stitchData.m_stitches[layer].count());
		while (columnIterator.hasNext())
		{
			columnIterator.next();
			int column = columnIterator.key();
			QHashIterator<int, StitchQueue *> rowIterator(columnIterator.value());
			stream << qint32(stitchData.m_stitches[layer][column].count());
			while (rowIterator.hasNext())
			{
				rowIterator.next();
				int row = rowIterator.key();
				StitchQueue *stitchQueue = rowIterator.value();
				stream << qint32(layer);
				stream << qint32(column);
				stream << qint32(row);
				stream << *stitchQueue;
			}
		}
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

	stream >> version;
	switch (version)
	{
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
						stitchData.m_stitches[layer][column][row] = stitchQueue;
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
