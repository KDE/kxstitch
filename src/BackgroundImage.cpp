/*
 * Copyright (C) 2010-2015 by Stephen Allewell
 * steve.allewell@gmail.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#include "BackgroundImage.h"

#include <KLocale>

#include "Exceptions.h"


BackgroundImage::BackgroundImage()
{
}


BackgroundImage::BackgroundImage(const KUrl &url, const QRect &location)
    :   m_url(url),
        m_location(location),
        m_visible(true)
{
    m_status = m_image.load(m_url.path());

    if (m_status) {
        generateIcon();
    }
}


const KUrl &BackgroundImage::url() const
{
    return m_url;
}


const QRect &BackgroundImage::location() const
{
    return m_location;
}


bool BackgroundImage::isVisible() const
{
    return m_visible;
}


bool BackgroundImage::isValid() const
{
    return m_status;
}


const QImage &BackgroundImage::image() const
{
    return m_image;
}


const QIcon &BackgroundImage::icon() const
{
    return m_icon;
}


void BackgroundImage::setLocation(const QRect &location)
{
    m_location = location;
}


void BackgroundImage::setVisible(bool visible)
{
    m_visible = visible;
}


void BackgroundImage::generateIcon()
{
    m_icon = QPixmap::fromImage(m_image).scaled(64, 64, Qt::KeepAspectRatio, Qt::SmoothTransformation);
}


QDataStream &operator<<(QDataStream &stream, const BackgroundImage &backgroundImage)
{
    stream << qint32(backgroundImage.version);
    stream << backgroundImage.m_url;
    stream << backgroundImage.m_location;
    stream << backgroundImage.m_visible;
    stream << backgroundImage.m_status;
    stream << backgroundImage.m_image;
    return stream;
}


QDataStream &operator>>(QDataStream &stream, BackgroundImage &backgroundImage)
{
    qint32 version;

    stream >> version;

    switch (version) {
    case 101:
        stream >> backgroundImage.m_url;
        stream >> backgroundImage.m_location;
        stream >> backgroundImage.m_visible;
        stream >> backgroundImage.m_status;
        stream >> backgroundImage.m_image;
        backgroundImage.generateIcon();
        break;

    case 100:
        stream >> backgroundImage.m_url;
        stream >> backgroundImage.m_location;
        stream >> backgroundImage.m_visible;
        stream >> backgroundImage.m_status;
        stream >> backgroundImage.m_image;
        stream >> backgroundImage.m_icon;
        break;

    default:
        throw InvalidFileVersion(QString(i18n("Background image version %1", version)));
        break;
    }

    return stream;
}
