/*
 * Copyright (C) 2015 by Stephen Allewell
 * steve.allewell@gmail.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


/** @file
 * This file defines an extension to a QLabel that scales the associated QPixmap
 * to maintain the aspect ratio when scaling to fill the extents of the QLabel
 * area.
 */


#ifndef ScaledPixmapLabel_H
#define ScaledPixmapLabel_H


// Qt includes
#include <QLabel>
#include <QPixmap>


/**
 * This class extends QLabel to scale the associated QPixmap to fill the QLabel
 * area whilst maintaining the aspect ratio of the pixmap.
 */
class ScaledPixmapLabel : public QLabel
{
    Q_OBJECT

public:
    /**
     * Constructor to initialise the class as a child of the parent.
     *
     * @param parent is a pointer to the parent QWidget
     */
    explicit ScaledPixmapLabel(QWidget *parent);

    /**
     * This overrides the base class method to calculate the height required
     * to maintain the aspect ratio for the specified width.
     *
     * @param width is the required width
     *
     * @return an int representing the height required
     */
    virtual int heightForWidth(int width) const Q_DECL_OVERRIDE;

    /**
     * This overrides the base class method to calculate the size of the widget
     * required to fill the parent window and maintain the aspect ratio.
     *
     * @return a QSize representing the width and height
     */
    virtual QSize sizeHint() const Q_DECL_OVERRIDE;

    /**
     * Calculate the size and position occupied by the scaled pixmap.
     *
     * @return a QRect representing the area occupied by the pixmap
     */
    QRect pixmapRect() const;

public slots:
    /**
     * Set the pixmap of the QLabel to a scaled version of the supplied pixmap.
     * The supplied pixmap is stored within the class to be reused when resizing.
     * This method hides the QLabel version which is then called from this one.
     *
     * @param pixmap a const reference to the QPixmap to be assigned to the QLabel
     */
    void setPixmap(const QPixmap &pixmap);

protected:
    /**
     * This overrides the base class method to resize the pixmap maintaining the
     * aspect ratio.  The widget has been resized at this point and the event
     * parameter is not used to determine the new size of the pixmap as the size
     * is taken from the widget size.
     *
     * @param event is a pointer to a QResizeEvent
     */
    virtual void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;

private:
    QPixmap m_pixmap;   /**< copy of the pixmap which is scaled to fill the label */
};


#endif // ScaledPixmapLabel_H
