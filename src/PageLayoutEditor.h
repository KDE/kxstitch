/*
 * Copyright (C) 2010 by Stephen Allewell
 * stephen@mirramar.adsl24.co.uk
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#ifndef PageLayoutEditor_H
#define PageLayoutEditor_H


#include <QWidget>

#include "Boundary.h"


class Document;
class PagePreviewListWidgetItem;
class QMouseEvent;


class PageLayoutEditor : public QWidget
{
    Q_OBJECT

public:
    PageLayoutEditor(QWidget *, Document *);
    ~PageLayoutEditor();

    double zoomFactor() const;
    int gridSize() const;
    bool showGrid() const;

    void setPagePreview(PagePreviewListWidgetItem *);
    void updatePagePreview();
    void setZoomFactor(double);
    void setGridSize(int);
    void setShowGrid(bool);
    void setSelecting(bool);

    int scale(int) const;
    int unscale(int) const;
    QPoint scale(const QPoint &) const;
    QPoint unscale(const QPoint &) const;
    QRect scale(const QRect &) const;
    QRect unscale(const QRect &) const;

signals:
    void selectionMade(QRect);
    void elementGeometryChanged();

protected:
    virtual void mousePressEvent(QMouseEvent *);
    virtual void mouseMoveEvent(QMouseEvent *);
    virtual void mouseReleaseEvent(QMouseEvent *);
    virtual void paintEvent(QPaintEvent *);

private slots:
    void contextMenuRequestedOn(const QPoint &);

private:
    QPoint toSnap(const QPoint &) const;

    Document                    *m_document;
    PagePreviewListWidgetItem   *m_pagePreview;
    Element                     *m_elementUnderCursor;

    int m_paperWidth;
    int m_paperHeight;

    bool            m_selecting;
    bool            m_resizing;
    bool            m_moved;
    Boundary        m_boundary;
    QRect           m_rubberBand;
    QPoint          m_start;
    QPoint          m_end;
    const QPoint    *m_node;

    bool    m_showGrid;
    int     m_gridSize;
    double  m_zoomFactor;
};


#endif // PageLayoutEditor_H
