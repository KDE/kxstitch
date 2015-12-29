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
 * This file defines a background image to be used as an overlay on a canvas for
 * the purpose of tracing.
 */


#ifndef BackgroundImage_H
#define BackgroundImage_H


// Qt includes
#include <QIcon>
#include <QImage>
#include <QRect>
#include <QUrl>


// Forward declaration of Qt classes
class QDataStream;


/**
 * This class defines a background image including the area it occupies on the
 * canvas, its visibility state and its original source url.  It also stores an
 * icon for display in the menus and a Qimage which is scaled to fit the canvas
 * zoom factor.
 */
class BackgroundImage
{
public:
    /**
     * Constructor to initialise an empty background image to allow streaming
     * from a file. Use the default implementation for this.
     */
    BackgroundImage() = default;

    /**
     * Constructor to initialise a background image from a source image file and
     * a target rectangle. This image defaults to being visible.
     *
     * @param url is a const reference to a QUrl for the source file
     * @param location is a const reference to a QRect that the background should fit into
     */
    BackgroundImage(const QUrl &url, const QRect &location);

    /**
     * Get the url of the background image.
     *
     * @return a const reference to the QUrl representing the source file URL
     */
    const QUrl &url() const;

    /**
     * Get the location occupied by the background image on the canvas.
     *
     * @return a const reference to a QRect representing the area occupied
     */
    const QRect &location() const;

    /**
     * Get the visible state of the background image.
     *
     * @return @c true if the image is visible, @c false otherwise
     */
    bool isVisible() const;

    /**
     * Get the validity status of the background image.  When the image is read
     * from a file, errors may occur that prevent the correct initialisation of
     * the class instance. This needs to be checked for after loading.
     *
     * @return @c true if the image is valid, @c false otherwise
     */
    bool isValid() const;

    /**
     * Get the QImage of the background image. This will be used to paint onto
     * the canvas.
     *
     * @return a const reference to a QImage representing the image
     */
    const QImage &image() const;

    /**
     * Get the QIcon of the background image. This is used in the menus to show
     * which image any action would apply to.
     *
     * @return a const reference to a QIcon representing the image
     */
    const QIcon &icon() const;

    /**
     * Set the target area of the canvas that the background image should occupy.
     *
     * @param location is a const reference to a QRect representing the target area
     */
    void setLocation(const QRect &location);

    /**
     * Set the visibility status of the background image to show or hide it.
     *
     * @param visible @c true to show the image, @c false to hide it
     */
    void setVisible(bool visible);

    /**
     * Operator to stream out the class instance to a QDataStream.
     *
     * @param stream is a reference to the QDataStream to write to
     * @param backgroundImage is a const reference to the class instance to write
     *
     * @return a reference to the QDataStream allowing chaining
     */
    friend QDataStream &operator<<(QDataStream &stream, const BackgroundImage &backgroundImage);

    /**
     * Operator to stream in the class instance from a QDataStream. The class
     * instance will have been created previously as an empty instance.
     *
     * @param stream is a reference to the QDataStream to read from
     * @param backgroundImage is a reference to the class instance to read into
     *
     * @return a reference to the QDataStream allowing chaining
     */
    friend QDataStream &operator>>(QDataStream &stream, BackgroundImage &backgroundImage);

private:
    /**
     * Generate the QIcon from the image data. Used after reading from a file.
     */
    void generateIcon();

    static const int version = 101; /**< The version of the streamed object */
    // no longer store m_icon, generate it on loading

    QUrl    m_url;      /**< The URL of the source file */
    QRect   m_location; /**< The area of the canvas occupied by the image */
    bool    m_visible;  /**< The visibility state, @c true if visible, @c false otherwise */
    bool    m_status;   /**< The validity state of the class instance, @c true if valid, @c false otherwise */
    QImage  m_image;    /**< The image read from the URL */
    QIcon   m_icon;     /**< An icon of the image */
};


/**
 * Allows the QSharedPointer<BackgroundImage> to be stored in a QAction QVariant value
 * for showing in the menus.
 */
Q_DECLARE_METATYPE(QSharedPointer<BackgroundImage>)


QDataStream &operator<<(QDataStream &, const BackgroundImage &);
QDataStream &operator>>(QDataStream &, BackgroundImage &);


#endif // BackgroundImage_H
