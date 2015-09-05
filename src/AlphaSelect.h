/*
 * Copyright (C) 2003-2015 by Stephen Allewell
 * steve.allewell@gmail.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#ifndef AlphaSelect_H
#define AlphaSelect_H


#include <QLabel>
#include <QPoint>


class QMouseEvent;


class AlphaSelect : public QLabel
{
    Q_OBJECT

public:
    explicit AlphaSelect(QLabel *);

signals:
    void clicked(QPoint);

protected:
    virtual void mouseReleaseEvent(QMouseEvent *);
};


#endif // AlphaSelect_H
