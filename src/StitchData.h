/*
 * Copyright (C) 2010 by Stephen Allewell
 * stephen@mirramar.adsl24.co.uk
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


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

    QMap<Stitch::Type, int> stitchCounts;
    QMap<Stitch::Type, double> stitchLengths;
    int backstitchCount;
    double  backstitchLength;
};


class StitchData
{
public:
    enum Rotation {
        Rotate90,
        Rotate180,
        Rotate270
    };

    StitchData();
    ~StitchData();

    void clear();

    int width() const;
    int height() const;

    void resize(int, int);

    QRect extents() const;
    void movePattern(int dx, int dy);
    void mirror(Qt::Orientation);
    void rotate(Rotation);

    void addStitch(const QPoint &, Stitch::Type, int);
    void deleteStitch(const QPoint &, Stitch::Type, int);

    StitchQueue *stitchQueueAt(const QPoint &);
    StitchQueue *takeStitchQueueAt(const QPoint &);
    StitchQueue *replaceStitchQueueAt(const QPoint &, StitchQueue *);

    void addBackstitch(const QPoint &, const QPoint &, int);
    void addBackstitch(Backstitch *);
    Backstitch *takeBackstitch(const QPoint &, const QPoint &, int);
    Backstitch *takeBackstitch(Backstitch *);

    void addFrenchKnot(const QPoint &, int);
    void addFrenchKnot(Knot *);
    Knot *takeFrenchKnot(const QPoint &, int);
    Knot *takeFrenchKnot(Knot *);

    QList<Backstitch *> &backstitches();
    QList<Knot *> &knots();

    QListIterator<Backstitch *> backstitchIterator();
    QMutableListIterator<Backstitch *> mutableBackstitchIterator();
    QListIterator<Knot *> knotIterator();
    QMutableListIterator<Knot *> mutableKnotIterator();

    QMap<int, FlossUsage> flossUsage();

    friend QDataStream &operator<<(QDataStream &, const StitchData &);
    friend QDataStream &operator>>(QDataStream &, StitchData &);

private:
    void    deleteStitches();
    void    invertQueue(Qt::Orientation, StitchQueue *);
    void    rotateQueue(Rotation, StitchQueue *);

    static const int version = 102;

    int m_width;
    int m_height;

    QHash<int, QHash<int, StitchQueue *> >  m_stitches;
    QList<Backstitch *>                     m_backstitches;
    QList<Knot *>                           m_knots;
};


QDataStream &operator<<(QDataStream &, const StitchData &);
QDataStream &operator>>(QDataStream &, StitchData &);


#endif // StitchData_H
