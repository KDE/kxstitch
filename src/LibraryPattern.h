/*
 * Copyright (C) 2003-2015 by Stephen Allewell
 * steve.allewell@gmail.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#ifndef LibraryPattern_H
#define LibraryPattern_H

#include <QByteArray>
#include <QDataStream>
#include <QString>

class LibraryListWidgetItem;
class Pattern;

class LibraryPattern
{
public:
    LibraryPattern();
    explicit LibraryPattern(Pattern *, qint32 key = 0, Qt::KeyboardModifiers modifiers = Qt::NoModifier, qint16 baseline = 0);
    explicit LibraryPattern(QByteArray, qint32 key = 0, Qt::KeyboardModifiers modifiers = Qt::NoModifier, qint16 baseline = 0);

    qint32 key() const;
    Qt::KeyboardModifiers modifiers() const;
    qint16 baseline() const;
    Pattern *pattern();
    LibraryListWidgetItem *libraryListWidgetItem() const;
    bool hasChanged() const;

    void setKeyModifiers(qint32, Qt::KeyboardModifiers);
    void setBaseline(qint16);
    void setLibraryListWidgetItem(LibraryListWidgetItem *);

    friend QDataStream &operator<<(QDataStream &, const LibraryPattern &);
    friend QDataStream &operator>>(QDataStream &, LibraryPattern &);

private:
    static const int version = 100;

    Pattern *m_pattern;
    qint32 m_key;
    Qt::KeyboardModifiers m_modifiers;
    qint16 m_baseline;
    LibraryListWidgetItem *m_libraryListWidgetItem;
    bool m_changed;
};

QDataStream &operator<<(QDataStream &, const LibraryPattern &);
QDataStream &operator>>(QDataStream &, LibraryPattern &);

#endif // LibraryPattern_H
