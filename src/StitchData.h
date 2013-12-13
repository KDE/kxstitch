/*
 * Copyright (C) 2010 by Stephen Allewell
 * sallewell@users.sourceforge.net
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#ifndef StitchData_H
#define StitchData_H


#include <QList>
#include <QListIterator>
#include <QMap>
#include <QPoint>
#include <QRect>
#include <QSharedDataPointer>
#include <QVector>

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
    void insertColumns(int, int);
    void insertRows(int, int);
    void removeColumns(int, int);
    void removeRows(int, int);

    QRect extents() const;
    void movePattern(int dx, int dy);
    void mirror(Qt::Orientation);
    void rotate(Rotation);

    void addStitch(const QPoint &, Stitch::Type, int);
    void deleteStitch(const QPoint &, Stitch::Type, int);

    StitchQueue *stitchQueueAt(int, int);
    StitchQueue *stitchQueueAt(const QPoint &);
    StitchQueue *takeStitchQueueAt(int, int);
    StitchQueue *takeStitchQueueAt(const QPoint &);
    StitchQueue *replaceStitchQueueAt(int, int, StitchQueue *);
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
    int     index(int, int) const;
    int     index(const QPoint &) const;

    static const int version = 103;

    int m_width;
    int m_height;

    QVector<StitchQueue *>                  m_stitches;
    QList<Backstitch *>                     m_backstitches;
    QList<Knot *>                           m_knots;
};


QDataStream &operator<<(QDataStream &, const StitchData &);
QDataStream &operator>>(QDataStream &, StitchData &);


#endif // StitchData_H
