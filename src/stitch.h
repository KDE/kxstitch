/****************************************************************************
 *	Copyright (C) 2010 by Stephen Allewell									*
 *	stephen@mirramar.adsl24.co.uk											*
 *																			*
 *	This program is free software; you can redistribute it and/or modify	*
 *	it under the terms of the GNU General Public License as published by	*
 *	the Free Software Foundation; either version 2 of the License, or		*
 *	(at your option) any later version.										*
 ****************************************************************************/


#ifndef STITCH_H
#define STITCH_H


#include <QPoint>
#include <QQueue>


/** Store information about individual stitches.
	@author Stephen P. Allewell
	<pre>
	Stitches encoded as follows

	128 64 32 16  8  4  2  1
	SF SH -- -- BR BL TR TL

		1\/2
		4/\8

	Type		binary  	decimal
	Delete		00000000	0
	TLQtr		00000001	1
	TRQtr		00000010	2
	BLQtr		00000100	4
	BTHalf		00000110	6
	TL3Qtr		00000111	7
	BRQtr		00001000	8
	TBHalf		00001001	9
	TR3Qtr		00001011	11
	BL3Qtr		00001101	13
	BR3Qtr		00001110	14
	Full		00001111	15
	TLSmallHalf	01000001	65
	TRSmallHalf	01000010	66
	BLSmallHalf	01000100	68
	BRSmallHalf	01001000	72
	TLSmallFull	10000001	129
	TRSmallFull	10000010	130
	BLSmallFull	10000100	132
	BRSmallFull	10001000	136
	FrenchKnot	11111111	255
	</pre>
	*/


class Stitch
{
	public:
		enum Type {
			Delete=0,
			TLQtr=1,
			TRQtr=2,
			BLQtr=4,
			BTHalf=6,
			TL3Qtr=7,
			BRQtr=8,
			TBHalf=9,
			TR3Qtr=11,
			BL3Qtr=13,
			BR3Qtr=14,
			Full=15,
			TLSmallHalf=65,
			TRSmallHalf=66,
			BLSmallHalf=68,
			BRSmallHalf=72,
			TLSmallFull=129,
			TRSmallFull=130,
			BLSmallFull=132,
			BRSmallFull=136,
			FrenchKnot=255
		};
		typedef QQueue<Stitch *> Queue;

		Stitch(Type t, int i);

		Type  type;
		int   floss;
};


class Backstitch
{
	public:
		Backstitch(QPoint s, QPoint e, int i);
		bool contains(QPoint p);

		QPoint  start;
		QPoint  end;
		int     floss;
};


class Knot
{
	public:
		Knot(QPoint p, int i);

		QPoint  pos;
		int     floss;
};


#endif
