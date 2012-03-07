/********************************************************************************
 *	Copyright (C) 2010 by Stephen Allewell					*
 *	stephen@mirramar.adsl24.co.uk						*
 *										*
 *	This program is free software; you can redistribute it and/or modify	*
 *	it under the terms of the GNU General Public License as published by	*
 *	the Free Software Foundation; either version 2 of the License, or	*
 *	(at your option) any later version.					*
 ********************************************************************************/


#ifndef StitchData_H
#define StitchData_H


#include <QHash>
#include <QList>
#include <QListIterator>
#include <QMap>
#include <QPoint>
#include <QRect>
#include <QSharedDataPointer>

#include "Stitch.h"


class FlossUsage
{
	public:
		FlossUsage();

		double totalLength() const;
		double stitchLength() const;
		int totalStitches() const;
		int stitchCount() const;

		QMap<Stitch::Type, int>	stitchCounts;
		QMap<Stitch::Type, double> stitchLengths;
		int	backstitchCount;
		double	backstitchLength;
};


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

		void addStitch(const QPoint &, Stitch::Type, int);
		void deleteStitch(const QPoint &, Stitch::Type, int);

		StitchQueue *stitchQueueAt(const QPoint &);
		StitchQueue *takeStitchQueueAt(const QPoint &);
		StitchQueue *replaceStitchQueueAt(const QPoint &, StitchQueue *);

		void addBackstitch(const QPoint &, const QPoint &, int);
		Backstitch *takeBackstitch(const QPoint &, const QPoint &, int);

		void addFrenchKnot(const QPoint &, int);
		Knot *takeFrenchKnot(const QPoint &, int);

		QListIterator<Backstitch *> backstitchIterator();
		QListIterator<Knot *> knotIterator();

		QMap<int, FlossUsage> flossUsage();

		friend QDataStream &operator<<(QDataStream &, const StitchData &);
		friend QDataStream &operator>>(QDataStream &, StitchData &);

	private:
		void	deleteStitches();

		static const int version = 101;

		int m_width;
		int m_height;

		QHash<int, QHash<int, StitchQueue *> >	m_stitches;
		QList<Backstitch *>			m_backstitches;
		QList<Knot *>				m_knots;
};


QDataStream &operator<<(QDataStream &, const StitchData &);
QDataStream &operator>>(QDataStream &, StitchData &);


#endif // StitchData_H
