/*
 * Copyright (C) 2011 by Stephen Allewell
 * stephen@mirramar.adsl24.co.uk
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


class QDragObject;
class QString;

class Renderer;


class LibraryListWidget : public QListWidget
{
public:
    explicit LibraryListWidget(QWidget *parent);
    ~LibraryListWidget();

    void setCellSize(double, double);
    void changeIconSize(int);

protected:
    virtual void dragEnterEvent(QDragEnterEvent *);
    virtual void dragMoveEvent(QDragMoveEvent *);
    virtual void dragLeaveEvent(QDragLeaveEvent *);
    virtual void mousePressEvent(QMouseEvent *);
    virtual void mouseMoveEvent(QMouseEvent *);

private:
    Renderer    *m_renderer;

    QPoint  m_startDrag;

    double  m_cellWidth;
    double  m_cellHeight;
};


#endif // LibraryListWidget_H
