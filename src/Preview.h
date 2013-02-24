/*
 * Copyright (C) 2010 by Stephen Allewell
 * stephen@mirramar.adsl24.co.uk
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#ifndef Preview_H
#define Preview_H


#include <QWidget>


class Document;
class Renderer;


class Preview : public QWidget
{
    Q_OBJECT

public:
    Preview(QWidget *);
    ~Preview();

    void setDocument(Document *);
    Document *document();

    void readDocumentSettings();

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

    Document    *m_document;
    Renderer    *m_renderer;
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
};


#endif // Preview_H
