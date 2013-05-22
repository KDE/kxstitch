/*
 * Copyright (C) 2003 by Stephen Allewell
 * sallewell@users.sourceforge.net
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#ifndef LibraryFile_H
#define LibraryFile_H


#include <QList>
#include <QString>


class LibraryPattern;


class LibraryFile
{
public:
    explicit LibraryFile(const QString &);
    ~LibraryFile();

    QString path() const;
    QString localFile() const;
    bool isWritable() const;

    void addPattern(LibraryPattern *);
    void deletePattern(LibraryPattern *);
    LibraryPattern *first();
    LibraryPattern *next();

private:
    void readFile();
    void writeFile();
    bool hasChanged();

    static const int version = 100;

    bool            m_exists;
    bool            m_read;
    QString         m_path;
    QList<LibraryPattern *> m_libraryPatterns;
    int         m_current;


};


#endif // LibraryFile_H
