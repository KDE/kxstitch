/*
 * Copyright (C) 2010 by Stephen Allewell
 * sallewell@users.sourceforge.net
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#ifndef Stitch_H
#define Stitch_H

#include <QDataStream>
#include <QPoint>
#include <QQueue>


class Stitch
{
public:
    enum Type {
        Delete = 0,
        TLQtr = 1,
        TRQtr = 2,
        BLQtr = 4,
        BTHalf = 6,
        TL3Qtr = 7,
        BRQtr = 8,
        TBHalf = 9,
        TR3Qtr = 11,
        BL3Qtr = 13,
        BR3Qtr = 14,
        Full = 15,
        TLSmallHalf = 65,
        TRSmallHalf = 66,
        BLSmallHalf = 68,
        BRSmallHalf = 72,
        TLSmallFull = 129,
        TRSmallFull = 130,
        BLSmallFull = 132,
        BRSmallFull = 136,
        FrenchKnot = 255
    };

    Stitch();
    Stitch(Stitch::Type, int);

    static const int version = 100;

    Stitch::Type    type;
    int     colorIndex;
};


QDataStream &operator<<(QDataStream &, const Stitch &);
QDataStream &operator>>(QDataStream &, Stitch &);


class StitchQueue : public QQueue<Stitch *>
{
public:
    StitchQueue();
    explicit StitchQueue(StitchQueue *);
    ~StitchQueue();

    int add(Stitch::Type, int);
    int remove(Stitch::Type, int);

    static const int version = 100;
};


QDataStream &operator<<(QDataStream &, const StitchQueue &);
QDataStream &operator>>(QDataStream &, StitchQueue &);


class Backstitch
{
public:
    Backstitch();
    Backstitch(const QPoint &, const QPoint &, int);

    bool contains(const QPoint &) const;
    void move(const QPoint &);

    static const int version = 100;

    QPoint  start;
    QPoint  end;
    int colorIndex;
};


QDataStream &operator<<(QDataStream &, const Backstitch &);
QDataStream &operator>>(QDataStream &, Backstitch &);


class Knot
{
public:
    Knot();
    Knot(const QPoint &, int);

    void move(const QPoint &);

    static const int version = 100;

    QPoint  position;
    int colorIndex;
};


QDataStream &operator<<(QDataStream &, const Knot &);
QDataStream &operator>>(QDataStream &, Knot &);


const Stitch::Type stitchMap[][4] = {
    {
        Stitch::TLQtr,
        Stitch::TRQtr,
        Stitch::BLQtr,
        Stitch::BRQtr
    },
    {
        Stitch::TBHalf,
        Stitch::BTHalf,
        Stitch::BTHalf,
        Stitch::TBHalf
    },
    {
        Stitch::TL3Qtr,
        Stitch::TR3Qtr,
        Stitch::BL3Qtr,
        Stitch::BR3Qtr
    },
    {
        Stitch::Full,
        Stitch::Full,
        Stitch::Full,
        Stitch::Full
    },
    {
        Stitch::TLSmallHalf,
        Stitch::TRSmallHalf,
        Stitch::BLSmallHalf,
        Stitch::BRSmallHalf
    },
    {
        Stitch::TLSmallFull,
        Stitch::TRSmallFull,
        Stitch::BLSmallFull,
        Stitch::BRSmallFull
    }
};


#endif // Stitch_H
