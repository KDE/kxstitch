/*
 * Copyright (C) 2003-2015 by Stephen Allewell
 * steve.allewell@gmail.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

/** @file
 * This file defines an overlay to be used on top of a QLabel to allow the
 * selection of a pixel point to be used as a color to be ignored.
 */

#ifndef AlphaSelect_H
#define AlphaSelect_H

// Qt includes
#include <QPoint>

// Application includes
#include "ScaledPixmapLabel.h"

// Forward declaration of Qt classes
class QMouseEvent;

/**
 * This class defines a widget based on a ScaledPixmapLabel that will allow
 * user selection of a point on the widget to define a color to be ignored for
 * import.
 */
class AlphaSelect : public ScaledPixmapLabel
{
    Q_OBJECT

public:
    /**
     * Constructor to initialise the class from the parent ScaledPixmapLabel
     * used in the ImportImageDlg ui file. This class becomes a child of
     * the parent and initialises it with the parents QPixmap.
     *
     * @param parent is a pointer to the parent ScaledPixmapLabel
     */
    explicit AlphaSelect(ScaledPixmapLabel *parent);

signals:
    /**
     * This signal is emmitted when the user clicks the mouse button and includes
     * the QPoint of the clicked position relative to the origin of the
     * widget.
     */
    void clicked(QPoint);

protected:
    /**
     * This overrides the base class method to determine where the user clicked
     * the mouse. If the cursor is within the image preview the clicked
     * signal will be emmitted.
     *
     * @param event is a pointer to the QMouseEvent
     */
    virtual void mouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
};

#endif // AlphaSelect_H
