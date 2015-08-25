/*
 * Copyright (C) 2009-2014 by Stephen Allewell
 * steve.allewell@gmail.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#include "LibraryTreeWidgetItem.h"


/*
struct LIBRARY (original)
{
  Q_UINT32 patterns;
  [
    Q_UINT16 checksum
    QByteArray compressedDataStream
    [
      QString scheme
      Q_INT32 width
      Q_INT32 height
      [
        Q_INT8 count
        Q_INT8 type
        Q_UINT32 color
      ] height x width
      Q_INT32 backstitches
      [
        QPoint start
        QPoint end
        Q_INT32 color
      ] backstitches
      Q_INT32 knots
      [
        QPoint start
        Q_INT32 color
      ] knots
    ]
  ] patterns
]

struct LIBRARY (new kde3)
[
  "KXStitchLib"
  Q_INT16 version
  [
    Q_UINT16 key
    Q_UINT16 modifier
    Q_INT16 baseline
    Q_UINT16 checksum
    QByteArray compressedDataStream
    [
      QString scheme
      Q_INT32 width
      Q_INT32 height
      [
        Q_INT8 count
        Q_INT8 type
        Q_UINT32 color
      ] height x width
      Q_INT32 backstitches
      [
        QPoint start
        QPoint end
        Q_INT32 color
      ] backstitches
      Q_INT32 knots
      [
        QPoint start
        Q_INT32 color
      ] knots
    ]
  ]
]

struct LIBRARY (new kde4)
[
  "KXStitchLib"
  quint16 version
  [
    version 100
    qint32 patterns
    [
      quint32 key
      quint32 modifier
      quint32 baseline
      Pattern
    ]
  ]
]
*/

#include <QStandardPaths>
#include <QTreeWidget>

#include "LibraryPattern.h"
#include "Pattern.h"


LibraryTreeWidgetItem::LibraryTreeWidgetItem(QTreeWidget *parent, const QString &name)
    :   QTreeWidgetItem(parent, QTreeWidgetItem::UserType)
{
    setText(0, name);
}


LibraryTreeWidgetItem::LibraryTreeWidgetItem(LibraryTreeWidgetItem *parent, const QString &name)
    :   QTreeWidgetItem(parent, QTreeWidgetItem::UserType)
{
    setText(0, name);
}


LibraryTreeWidgetItem::~LibraryTreeWidgetItem()
{
    qDeleteAll(m_libraryFiles);
}


int LibraryTreeWidgetItem::maxHeight()
{
    int max = 0;

    for (LibraryPattern *libraryPattern = first() ; libraryPattern ; libraryPattern = next()) {
        max = std::max(max, libraryPattern->pattern()->stitches().height());
    }

    return max;
}


LibraryPattern *LibraryTreeWidgetItem::findCharacter(int key, Qt::KeyboardModifiers modifiers)
{
    LibraryPattern *libraryPattern;

    for (libraryPattern = first() ; libraryPattern ; libraryPattern = next()) {
        if (libraryPattern->key() == key && libraryPattern->modifiers() == modifiers) {
            break;
        }
    }

    return libraryPattern;
}


LibraryPattern *LibraryTreeWidgetItem::first()
{
    LibraryPattern *libraryPattern = 0;
    m_libraryFilesIndex = 0;

    if (!m_libraryFiles.isEmpty()) {
        libraryPattern = m_libraryFiles[0]->first();

        if (libraryPattern == 0) { // the first file is empty
            libraryPattern = next();
        }
    }

    return libraryPattern;
}


LibraryPattern *LibraryTreeWidgetItem::next()
{
    LibraryPattern *libraryPattern = 0;

    while (libraryPattern == 0 && !m_libraryFiles.isEmpty() && m_libraryFilesIndex != m_libraryFiles.count()) {
        libraryPattern = m_libraryFiles[m_libraryFilesIndex]->next();

        if (libraryPattern == 0) {  // reached the end of the current file
            if (++m_libraryFilesIndex < m_libraryFiles.count()) {
                libraryPattern = m_libraryFiles[m_libraryFilesIndex]->first();  // get the first pattern of the next file
            }
        }
    }

    return libraryPattern;
}


void LibraryTreeWidgetItem::addPath(const QString &path)
{
    m_libraryFiles.append(new LibraryFile(path));
}


QString LibraryTreeWidgetItem::path()
{
    return m_libraryFiles.first()->path();
}


QStringList LibraryTreeWidgetItem::paths()
{
    QStringList paths;
    QListIterator<LibraryFile *> libraryFilesIterator(m_libraryFiles);

    while (libraryFilesIterator.hasNext()) {
        paths.append(libraryFilesIterator.next()->localFile());
    }

    return paths;
}


LibraryFile *LibraryTreeWidgetItem::writablePath()
{
    LibraryFile *libraryFile = 0;
    QListIterator<LibraryFile *> libraryFilesIterator(m_libraryFiles);

    while (libraryFilesIterator.hasNext()) {
        libraryFile = libraryFilesIterator.next();

        if (libraryFile->isWritable()) {
            return libraryFile;
        }
    }

    QString path = m_libraryFiles[0]->path();
    path.remove(0, path.indexOf("/library"));
    path.prepend(QStandardPaths::writableLocation(QStandardPaths::DataLocation));
    addPath(path);
    libraryFile = m_libraryFiles.last();
    return libraryFile;
}


void LibraryTreeWidgetItem::addPattern(LibraryPattern *libraryPattern)
{
    writablePath()->addPattern(libraryPattern);
}


void LibraryTreeWidgetItem::deletePattern(LibraryPattern *libraryPattern)
{
    QListIterator<LibraryFile *> libraryFilesIterator(m_libraryFiles);

    while (libraryFilesIterator.hasNext()) {
        LibraryFile *file = libraryFilesIterator.next();

        for (LibraryPattern *pattern = file->first() ; pattern ; pattern = file->next()) {
            if (pattern == libraryPattern) {
                file->deletePattern(libraryPattern);
            }
        }
    }
}
