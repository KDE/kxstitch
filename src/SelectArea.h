/*
 * Copyright (C) 2010-2015 by Stephen Allewell
 * steve.allewell@gmail.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#ifndef SelectArea_H
#define SelectArea_H


#include <QList>
#include <QMap>
#include <QRect>
#include <QWidget>


class QMouseEvent;
class QPaintEvent;

class Document;
class PatternElement;


class SelectArea : public QWidget
{
    Q_OBJECT

public:
    SelectArea(QWidget *, PatternElement *, Document *, const QMap<int, QList<QRect> > &);
    virtual ~SelectArea() = default;

    QRect patternRect() const;
    void setPatternRect(const QRect &);

protected:
    virtual void mousePressEvent(QMouseEvent *) Q_DECL_OVERRIDE;
    virtual void mouseMoveEvent(QMouseEvent *) Q_DECL_OVERRIDE;
    virtual void mouseReleaseEvent(QMouseEvent *) Q_DECL_OVERRIDE;
    virtual void paintEvent(QPaintEvent *) Q_DECL_OVERRIDE;

private:
    QPoint contentsToCell(const QPoint &) const;
    QRect cellToRect(const QPoint &) const;

    PatternElement  *m_patternElement;
    PatternElement  *m_fullPatternElement;

    Document        *m_document;
    int             m_width;
    int             m_height;
    QRect           m_patternRect;
    QMap<int, QList<QRect> >    m_patternRects;

    QPoint  m_cellStart;
    QPoint  m_cellTracking;
    QPoint  m_cellEnd;

    QRect   m_rubberBand;
};


#endif // SelectArea_H
