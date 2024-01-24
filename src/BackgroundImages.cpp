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
 * This file implements a collection of background images to be used as overlays on
 * a canvas for the purpose of tracing.
 */

// Class include
#include "BackgroundImages.h"

// Qt includes
#include <QDataStream>

// KF5 includes
#include <KLocalizedString>

// Application includes
#include "BackgroundImage.h"
#include "Exceptions.h"

void BackgroundImages::clear()
{
    m_backgroundImages.clear();
}

QListIterator<QSharedPointer<BackgroundImage>> BackgroundImages::backgroundImages()
{
    return QListIterator<QSharedPointer<BackgroundImage>>(m_backgroundImages);
}

void BackgroundImages::addBackgroundImage(QSharedPointer<BackgroundImage> backgroundImage)
{
    m_backgroundImages.append(backgroundImage);
}

bool BackgroundImages::removeBackgroundImage(QSharedPointer<BackgroundImage> backgroundImage)
{
    return m_backgroundImages.removeOne(backgroundImage);
}

QRect BackgroundImages::fitBackgroundImage(QSharedPointer<BackgroundImage> backgroundImage, const QRect &rectangle)
{
    QRect oldRectangle = backgroundImage->location();
    backgroundImage->setLocation(rectangle);

    return oldRectangle;
}

bool BackgroundImages::showBackgroundImage(QSharedPointer<BackgroundImage> backgroundImage, bool show)
{
    bool shown = backgroundImage->isVisible();
    backgroundImage->setVisible(show);

    return shown;
}

QDataStream &operator<<(QDataStream &stream, const BackgroundImages &backgroundImages)
{
    stream << qint32(backgroundImages.version);
    stream << qint32(backgroundImages.m_backgroundImages.count());

    for (auto backgroundImage : backgroundImages.m_backgroundImages) {
        stream << *backgroundImage;
    }

    if (stream.status() != QDataStream::Ok) {
        throw FailedWriteFile(stream.status());
    }

    return stream;
}

QDataStream &operator>>(QDataStream &stream, BackgroundImages &backgroundImages)
{
    qint32 version;
    qint32 backgroundImageCount;

    stream >> version;

    switch (version) {
    case 100:
        stream >> backgroundImageCount;

        while (backgroundImageCount--) {
            QSharedPointer<BackgroundImage> backgroundImage(new BackgroundImage);
            stream >> *backgroundImage;
            backgroundImages.m_backgroundImages.append(backgroundImage);
        }

        break;

    default:
        throw InvalidFileVersion(QString(i18n("Background images version %1", version)));
        break;
    }

    if (stream.status() != QDataStream::Ok) {
        throw FailedReadFile(stream.status());
    }

    return stream;
}
