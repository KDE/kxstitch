/*
 * Copyright (C) 2010-2014 by Stephen Allewell
 * steve.allewell@gmail.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#ifndef Scale_H
#define Scale_H


#include <QWidget>

#include "configuration.h"


class Scale : public QWidget
{
    Q_OBJECT

public:
    explicit Scale(Qt::Orientation);
    ~Scale();

    void setUnits(Configuration::EnumEditor_FormatScalesAs::type);
    void setCellSize(double);
    void setCellGrouping(int);
    void setCellCount(int);
    void setClothCount(double);
    void setClothCountUnits(Configuration::EnumEditor_ClothCountUnits::type);
    void setOffset(double);
    virtual QSize sizeHint() const;

protected:
    void paintEvent(QPaintEvent *);

private:
    Configuration::EnumEditor_FormatScalesAs::type  m_units;
    double  m_cellSize;
    int     m_cellGrouping;
    int     m_cellCount;
    double  m_clothCount;
    Configuration::EnumEditor_ClothCountUnits::type m_clothCountUnits;
    double  m_offset;
    Qt::Orientation m_orientation;
};


#endif // Scale_H
