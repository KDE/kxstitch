/*
 * Copyright (C) 2010-2015 by Stephen Allewell
 * steve.allewell@gmail.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#ifndef Preview_H
#define Preview_H


#include <QImage>
#include <QWidget>

#include "Renderer.h"


class Document;


class Preview : public QWidget
{
    Q_OBJECT

public:
    explicit Preview(QWidget *);
    ~Preview();

    void setDocument(Document *);
    Document *document();

    void readDocumentSettings();
    void drawContents();

public slots:
    void setVisibleCells(const QRect &);

    void loadSettings();

signals:
    void clicked(QPoint);
    void clicked(QRect);

protected:
    virtual void mousePressEvent(QMouseEvent *);
    virtual void mouseMoveEvent(QMouseEvent *);
    virtual void mouseReleaseEvent(QMouseEvent *);
    virtual void paintEvent(QPaintEvent *);

private:
    QPoint contentToCell(const QPoint &content) const;

    Document    *m_document;
    Renderer    m_renderer;
    QRect       m_visible;
    QPoint      m_start;
    QPoint      m_tracking;
    QPoint      m_end;
    QRect       m_rubberBand;
    double      m_cellWidth;
    double      m_cellHeight;
    double      m_previewWidth;
    double      m_previewHeight;
    double      m_zoomFactor;

    QImage      m_cachedContents;
};


#endif // Preview_H
