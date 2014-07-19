/*
 * Copyright (C) 2011-2014 by Stephen Allewell
 * steve.allewell@gmail.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


/**
 * @file
 * Header file for the Symbol class.
 */


#ifndef Symbol_H
#define Symbol_H


#include <QMap>
#include <QPainterPath>

#include "Stitch.h"


/**
 * @brief Defines the Symbol class encapsulating the QPainterPath with how it is drawn.
 *
 * The Symbol incorporates the QPainterPath object with additional information as to how
 * it should be drawn. Originally the path was drawn filled, the implementation of this
 * Symbol class allows a fill attribute and a pen width attribute for outline paths. In
 * addition the line end style and line join style can be changed.
 */
class Symbol
{
public:
    Symbol();
    ~Symbol();

    QPainterPath path(Stitch::Type type);
    QPainterPath path() const;
    bool filled() const;
    qreal lineWidth() const;
    Qt::PenCapStyle capStyle() const;
    Qt::PenJoinStyle joinStyle() const;

    void setPath(const QPainterPath &path);
    void setFilled(bool filled);
    void setLineWidth(qreal width);
    void setCapStyle(Qt::PenCapStyle penCapStyle);
    void setJoinStyle(Qt::PenJoinStyle penJoinStyle);

    QPen pen() const;
    QBrush brush() const;

    friend QDataStream &operator<<(QDataStream &stream, const Symbol &symbol);
    friend QDataStream &operator>>(QDataStream &stream, Symbol &symbol);

private:
    static const qint32 version = 100;              /**< version of the stream object */

    QMap<Stitch::Type, QPainterPath>    m_paths;    /**< the symbols paths, cached by type, incorporates fill method if m_filled is true */

    bool                m_filled;                   /**< true if the path is filled, false if an outline path */
    qreal               m_lineWidth;                /**< width of the pen, this is scaled with the painter */
    Qt::PenCapStyle     m_capStyle;                 /**< pen cap style, see the QPen documentation for details */
    Qt::PenJoinStyle    m_joinStyle;                /**< pen join style, see the QPen documentation for details */
};


QDataStream &operator<<(QDataStream &stream, const Symbol &symbol);
QDataStream &operator>>(QDataStream &stream, Symbol &symbol);


#endif
