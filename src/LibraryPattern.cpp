/*
 * Copyright (C) 2003-2015 by Stephen Allewell
 * steve.allewell@gmail.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include "LibraryPattern.h"

#include <QListWidget>
#include <QIODevice>

#include "KeycodeLineEdit.h"
#include "LibraryListWidgetItem.h"
#include "Pattern.h"

LibraryPattern::LibraryPattern()
{
    m_pattern = new Pattern;
}

LibraryPattern::LibraryPattern(Pattern *pattern, qint32 key, Qt::KeyboardModifiers modifiers, qint16 baseline)
    : m_pattern(pattern)
    , m_key(key)
    , m_modifiers(modifiers)
    , m_baseline(baseline)
    , m_libraryListWidgetItem(nullptr)
    , m_changed(false)
{
}

LibraryPattern::LibraryPattern(QByteArray data, qint32 key, Qt::KeyboardModifiers modifiers, qint16 baseline)
    : m_key(key)
    , m_modifiers(modifiers)
    , m_baseline(baseline)
    , m_libraryListWidgetItem(nullptr)
    , m_changed(false)
{
    QDataStream stream(&data, QIODevice::ReadOnly);
    stream.setVersion(QDataStream::Qt_3_3);
    QString scheme;
    qint32 width;
    qint32 height;
    stream >> scheme >> width >> height;
    m_pattern = new Pattern;
    m_pattern->palette().setSchemeName(scheme);
    m_pattern->stitches().resize(width, height);

    QMap<int, QColor> colors;
    int colorIndex;

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            QPoint cell(x, y);
            qint8 stitches;
            stream >> stitches;

            while (stitches--) {
                qint8 type;
                QColor color;
                stream >> type >> color;

                if ((colorIndex = colors.key(color, -1)) == -1) {
                    colorIndex = m_pattern->palette().add(color);
                    colors.insert(colorIndex, color);
                }

                m_pattern->stitches().addStitch(cell, static_cast<Stitch::Type>(type), colorIndex);
            }
        }
    }

    qint32 backstitches;
    stream >> backstitches;

    while (backstitches--) {
        QPoint start;
        QPoint end;
        QColor color;
        stream >> start >> end >> color;

        if ((colorIndex = colors.key(color, -1)) == -1) {
            colorIndex = m_pattern->palette().add(color);
            colors.insert(colorIndex, color);
        }

        m_pattern->stitches().addBackstitch(start, end, colorIndex);
    }

    qint32 knots;
    stream >> knots;

    while (knots--) {
        QPoint position;
        QColor color;
        stream >> position >> color;

        if ((colorIndex = colors.key(color, -1)) == -1) {
            colorIndex = m_pattern->palette().add(color);
            colors.insert(colorIndex, color);
        }

        m_pattern->stitches().addFrenchKnot(position, colorIndex);
    }
}

qint32 LibraryPattern::key() const
{
    return m_key;
}

Qt::KeyboardModifiers LibraryPattern::modifiers() const
{
    return m_modifiers;
}

qint16 LibraryPattern::baseline() const
{
    return m_baseline;
}

Pattern *LibraryPattern::pattern()
{
    return m_pattern;
}

LibraryListWidgetItem *LibraryPattern::libraryListWidgetItem() const
{
    return m_libraryListWidgetItem;
}

bool LibraryPattern::hasChanged() const
{
    return m_changed;
}

void LibraryPattern::setKeyModifiers(qint32 key, Qt::KeyboardModifiers modifiers)
{
    m_key = key;
    m_modifiers = modifiers;
    m_libraryListWidgetItem->setText(KeycodeLineEdit::keyString(key, modifiers));
    m_changed = true;
}

void LibraryPattern::setBaseline(qint16 baseline)
{
    m_baseline = baseline;
    m_changed = true;
}

void LibraryPattern::setLibraryListWidgetItem(LibraryListWidgetItem *libraryListWidgetItem)
{
    m_libraryListWidgetItem = libraryListWidgetItem;
    libraryListWidgetItem->setText(KeycodeLineEdit::keyString(m_key, m_modifiers));
}

QDataStream &operator<<(QDataStream &stream, const LibraryPattern &libraryPattern)
{
    stream << libraryPattern.version;
    stream << libraryPattern.m_key;
    stream << qint32(libraryPattern.m_modifiers);
    stream << libraryPattern.m_baseline;
    stream << *(libraryPattern.m_pattern);
    return stream;
}

QDataStream &operator>>(QDataStream &stream, LibraryPattern &libraryPattern)
{
    qint32 version;
    qint32 modifiers;

    stream >> version;

    switch (version) {
    case 100:
        stream >> libraryPattern.m_key;
        stream >> modifiers;
        libraryPattern.m_modifiers = Qt::KeyboardModifiers(modifiers);
        stream >> libraryPattern.m_baseline;
        stream >> *(libraryPattern.m_pattern);
        libraryPattern.m_libraryListWidgetItem = nullptr;
        libraryPattern.m_changed = false;
        break;

    default:
        // not supported
        // throw exception
        break;
    }

    return stream;
}
