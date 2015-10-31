/*
 * Copyright (C) 2003-2015 by Stephen Allewell
 * steve.allewell@gmail.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#include "LibraryFile.h"

#include <QDataStream>
#include <QFile>
#include <QFileInfo>
#include <QProgressDialog>

#include <KLocalizedString>
#include <KMessageBox>

#include <stdlib.h>

#include "LibraryPattern.h"


LibraryFile::LibraryFile(const QString &path)
    :   m_read(false),
        m_path(path)
{
    m_exists = QFile::exists(localFile());
}


LibraryFile::~LibraryFile()
{
    if (hasChanged()) {
        writeFile();
    }

    qDeleteAll(m_libraryPatterns);
}


QString LibraryFile::path() const
{
    return m_path;
}


bool LibraryFile::isWritable() const
{
    return QFileInfo(m_path).isWritable();
}


void LibraryFile::addPattern(LibraryPattern *libraryPattern)
{
    m_libraryPatterns.append(libraryPattern);
    writeFile();
}


void LibraryFile::deletePattern(LibraryPattern *libraryPattern)
{
    if (m_libraryPatterns.removeOne(libraryPattern)) {
        delete libraryPattern;
        writeFile();
    }
}


LibraryPattern *LibraryFile::first()
{
    if (!m_read) {
        readFile();
    }

    m_current = 0;

    if (m_current < m_libraryPatterns.count()) {
        return m_libraryPatterns.at(m_current++);
    }

    return 0;
}


LibraryPattern *LibraryFile::next()
{
    if (m_current < m_libraryPatterns.count()) {
        return m_libraryPatterns.at(m_current++);
    }

    return 0;
}


void LibraryFile::readFile()
{
    if (m_exists) {
        bool ok = true;
        QFile file(localFile());

        if (file.open(QIODevice::ReadOnly)) {
            QDataStream stream(&file);
            char header[11];
            stream.readRawData(header, 11);

            if (strncmp(header, "KXStitchLib", 11) == 0) {
                qint16 version;
                qint32 count;
                qint32 key;         // version 1 of library format
                qint32 modifier;    // version 1 of library format
                qint16 baseline;    // version 1 of library format
                quint16 checksum;   // version 1 of library format
                ok = true;
                QByteArray data;
                LibraryPattern *libraryPattern;

                QProgressDialog progress(0);
                progress.setLabelText(i18n("Loading Library %1", file.fileName()));

                stream >> version;

                switch (version) {
                case 1:
                    progress.setRange(file.pos(), file.size());
                    progress.setValue(0);
                    progress.show();

                    while (!file.atEnd() && ok) {
                        stream >> key;
                        stream >> modifier;
                        stream >> baseline;
                        stream >> checksum; // no longer used
                        stream >> data;
                        Qt::KeyboardModifiers replacedModifiers;

                        if (modifier & 0x0100) {
                            replacedModifiers |= Qt::ShiftModifier;
                        }

                        if (modifier & 0x0200) {
                            replacedModifiers |= Qt::ControlModifier;
                        }

                        if (modifier & 0x0400) {
                            replacedModifiers |= Qt::AltModifier;
                        }

                        if (modifier & 0x0800) {
                            replacedModifiers |= Qt::MetaModifier;
                        }

                        if (checksum == qChecksum(data.data(), data.size())) {
                            m_libraryPatterns.append(new LibraryPattern(data, key, replacedModifiers, baseline));
                        } else {
                            KMessageBox::error(0, i18n("Failed to read a pattern from the library %1.\n%2", localFile(), file.errorString()), i18n("Failed to read library."));
                            ok = false;
                        }

                        progress.setValue(file.pos());
                    }

                    break;

                case 100:
                    stream >> count;
                    progress.setRange(0, count);
                    progress.setValue(0);
                    progress.show();

                    while (count--) {
                        libraryPattern = new LibraryPattern;
                        stream >> *libraryPattern;
                        m_libraryPatterns.append(libraryPattern);
                        progress.setValue(progress.value() + 1);
                    }

                    break;

                default:
                    // not supported
                    // throw exception
                    break;
                }
            }

            file.close();
            m_read = true;
        } else {
            KMessageBox::error(0, i18n("The file %1\ncould not be opened for reading.\n%2", localFile(), file.errorString()), i18n("Error opening file"));
        }
    }
}


void LibraryFile::writeFile()
{
    QFile file(localFile());

    if (file.open(QIODevice::WriteOnly)) { // truncates the file
        QDataStream stream(&file);
        stream.writeRawData("KXStitchLib", 11);
        stream << qint16(version);
        stream << qint32(m_libraryPatterns.count());
        QListIterator<LibraryPattern *> iterator(m_libraryPatterns);

        while (iterator.hasNext()) {
            LibraryPattern *libraryPattern = iterator.next();
            stream << *libraryPattern;
        }

        file.close();
        m_read = true;
    } else {
        KMessageBox::error(0, i18n("The file %1\ncould not be opened for writing.\n%2", localFile(), file.errorString()), i18n("Error opening file"));
    }
}


QString LibraryFile::localFile() const
{
    QFileInfo path(m_path);

    if (path.isDir()) {
        return m_path + "/kxstitch.library";
    } else {
        return m_path;
    }
}


bool LibraryFile::hasChanged()
{
    bool changed = false;

    QListIterator<LibraryPattern *> libraryPatternIterator(m_libraryPatterns);

    while (libraryPatternIterator.hasNext()) {
        changed = changed | libraryPatternIterator.next()->hasChanged();
    }

    return changed;
}
