/*
 * Copyright (C) 2010-2015 by Stephen Allewell
 * steve.allewell@gmail.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

/** @file
 * This file defines a collection of background images to be used as overlays on
 * a canvas for the purpose of tracing.
 */

#ifndef BackgroundImages_H
#define BackgroundImages_H

// Qt includes
#include <QList>
#include <QRect>
#include <QSharedPointer>

// Forward declaration of Qt classes
class QDataStream;

// Forward declaration of application classes
class BackgroundImage;

/**
 * This class defines a collection of background images allowing the addition
 * and removal of background images, setting the area occupied by an image and
 * changing the visibility status.
 *
 * The collection is stored in a QList of QSharedPointer objects. Generally the
 * BackgroundImage pointers would be owned by the collection and deleted in the
 * destructor. However if a background image is created and added to the list and
 * then undo is used it will be removed from the list but still referenced by the
 * AddBackgroundImageCommand which then becomes the owner. Using a QSharedPointer
 * to the BackgroundImage will ensure the BackgroundImage will be deleted
 * regardless of which object owns it or how many references there are to it.
 */
class BackgroundImages
{
public:
    /**
     * Constructor to create an empty list of background images. Use the default
     * implementation for this.
     */
    BackgroundImages() = default;

    /**
     * Clear the contents of the collection.
     */
    void clear();

    /**
     * Get an iterator for the list of QSharedPointer to the background images.
     *
     * @return a QListIterator for BackgroundImage
     */
    QListIterator<QSharedPointer<BackgroundImage>> backgroundImages();

    /**
     * Add a background image to the list.
     *
     * @param backgroundImage is a QSharedPointer to the BackgroundImage to add
     */
    void addBackgroundImage(QSharedPointer<BackgroundImage> backgroundImage);

    /**
     * Remove a background image from the list.
     *
     * @param backgroundImage is a QSharedPointer to the BackgroundImage to remove
     *
     * @return @c true if the pointer was in the list, @c false otherwise
     */
    bool removeBackgroundImage(QSharedPointer<BackgroundImage> backgroundImage);

    /**
     * Fit a background image to a new location.
     *
     * @param backgroundImage is a QSharedPointer to the BackgroundImage to change
     * @param location is a const reference to a QRect defining the new location
     *
     * @return a QRect representing the previous location
     */
    QRect fitBackgroundImage(QSharedPointer<BackgroundImage> backgroundImage, const QRect &location);

    /**
     * Show or hide a background image.
     *
     * @param backgroundImage is a QSharedPointer to the BackgroundImage to change
     * @param show is @c true if the image is to be shown, @c false to hide
     *
     * @return the previous visibility state, @c true if shown, @c false if hidden
     */
    bool showBackgroundImage(QSharedPointer<BackgroundImage> backgroundImage, bool show);

    /**
     * Operator to stream out the class instance to a QDataStream. This will
     * stream the instance of the BackgroundImage contained in the list.
     *
     * @param stream a reference to the QDataStream to write to
     * @param backgroundImages a const reference to the class instance to write
     *
     * @return a reference to the QDataStream allowing chaining
     */
    friend QDataStream &operator<<(QDataStream &stream, const BackgroundImages &backgroundImages);

    /**
     * Operator to stream in the class instance from a QDataStream. The class
     * instance will have been created previously as an empty instance. This will
     * create instances of BackgroundImage and read those from the stream adding
     * them to the list.
     *
     * @param stream a reference to the QDataStream to read from
     * @param backgroundImages a reference to the class instance to read into
     *
     * @return a reference to the QDataStream allowing chaining
     */
    friend QDataStream &operator>>(QDataStream &stream, BackgroundImages &backgroundImages);

private:
    static const int version = 100; /**< The version of the streamed object */

    QList<QSharedPointer<BackgroundImage>> m_backgroundImages; /**< A list of BackgroundImage shared pointers */
};

QDataStream &operator<<(QDataStream &, const BackgroundImages &);
QDataStream &operator>>(QDataStream &, BackgroundImages &);

#endif // BackgroundImages_H
