/*
 * Copyright (C) 2011-2015 by Stephen Allewell
 * steve.allewell@gmail.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#ifndef LibraryListWidget_H
#define LibraryListWidget_H


#include <QListWidget>
#include <QWidget>

#include "Renderer.h"


class QString;

class Renderer;


class LibraryListWidget : public QListWidget
{
public:
    explicit LibraryListWidget(QWidget *parent);
    virtual ~LibraryListWidget();

    void setCellSize(double, double);
    void changeIconSize(int);

protected:
    virtual void dragEnterEvent(QDragEnterEvent *) Q_DECL_OVERRIDE;
    virtual void dragMoveEvent(QDragMoveEvent *) Q_DECL_OVERRIDE;
    virtual void dragLeaveEvent(QDragLeaveEvent *) Q_DECL_OVERRIDE;
    virtual void mousePressEvent(QMouseEvent *) Q_DECL_OVERRIDE;
    virtual void mouseMoveEvent(QMouseEvent *) Q_DECL_OVERRIDE;

private:
    Renderer    m_renderer;

    QPoint  m_startDrag;

    double  m_cellWidth;
    double  m_cellHeight;
};


#endif // LibraryListWidget_H
