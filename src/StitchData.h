/********************************************************************************
 *	Copyright (C) 2010 by Stephen Allewell					*
 *	stephen@mirramar.adsl24.co.uk						*
 *										*
 *	This program is free software; you can redistribute it and/or modify	*
 *	it under the terms of the GNU General Public License as published by	*
 *	the Free Software Foundation; either version 2 of the License, or	*
 *	(at your option) any later version.					*
 ********************************************************************************/


#ifndef __StitchData_H
#define __StitchData_H


#include <QHash>
#include <QList>
#include <QListIterator>
#include <QMap>
#include <QPoint>
#include <QRect>
#include <QSharedDataPointer>

#include "Stitch.h"


class StitchData
{
	public:
		StitchData();
		~StitchData();

		void clear();

		int width() const;
		int height() const;

		void resize(int, int);

		QRect extents() const;
		void movePattern(int dx, int dy);

		StitchData cut(const QRect &area, const QList<int> &layerMask, const QList<int> &colorMask, const QList<Stitch::Type> &stitchMask, bool excludeBackstitches, bool excludeKnots, bool crossingBackstitches);
		StitchData copy(const QRect &area, const QList<int> &layerMask, const QList<int> &colorMask, const QList<Stitch::Type> &stitchMask, bool excludeBackstitches, bool excludeKnots, bool crossingBackstitches) const;
		void paste(const StitchData &dataSet, const QPoint &, bool merge);

		void addStitch(int, const QPoint &, Stitch::Type, int);
		void deleteStitch(int, const QPoint &, Stitch::Type, int);

		StitchQueue *stitchQueueAt(int, const QPoint &);
		StitchQueue *takeStitchQueueAt(int, const QPoint &);
		StitchQueue *replaceStitchQueueAt(int, const QPoint &, StitchQueue *);

		void addBackstitch(int, const QPoint &, const QPoint &, int);
		Backstitch *takeBackstitch(int, const QPoint &, const QPoint &, int);

		void addFrenchKnot(int, const QPoint &, int);
		Knot *takeFrenchKnot(int, const QPoint &, int);

		QList<int> stitchLayers() const;
		QList<int> backstitchLayers() const;
		QList<int> knotLayers() const;

		QListIterator<Backstitch *> backstitchIterator(int layer);
		QListIterator<Knot *> knotIterator(int layer);

		QMap<int, int> usedFlosses();

		friend QDataStream &operator<<(QDataStream &, const StitchData &);
		friend QDataStream &operator>>(QDataStream &, StitchData &);

	private:
		void	deleteStitches();

		static const int version = 100;

		int m_width;
		int m_height;

		QHash<int, QHash<int, QHash<int, StitchQueue *> > >	m_stitches;
		QHash<int, QList<Backstitch *> >			m_backstitches;
		QHash<int, QList<Knot *> >				m_knots;
};


QDataStream &operator<<(QDataStream &, const StitchData &);
QDataStream &operator>>(QDataStream &, StitchData &);


#endif
