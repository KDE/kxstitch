/*
 * Copyright (C) 2011-2014 by Stephen Allewell
 * steve.allewell@gmail.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


/**
 * @file
 * Implement the SymbolLibrary class
 */


/**
 * @page symbol_library Symbol Library
 * The symbols created are stored in symbol files. A number of different files can be created containing symbols
 * of different themes. When files are opened the symbol library will be populated with the contents of the file.
 *
 * The symbol library can be associated with a LibraryListWidget which will provide an icon view of all the
 * existing symbols. Clicking on one of the icons will open this symbol in the editor and allow it to be modified.
 * New symbols created can be saved to the library.
 *
 * Note that symbols saved to the library are not immediately saved to disk. The user is required to save the
 * library with the File->Save command. The user may also use the File->Save As command to save the library to a
 * different file.
 *
 * @image html ui-main-library.png "The user interface showing the library tab"
 *
 * All changes to the library through adding new symbols, editing a symbol or importing a library can be undone
 * or redone with the undo and redo commands. A separate undo stack is managed for the library and for the editor
 * and the active one is dependent on which of the tabs is selected. Undoing commands in the editor tab does not
 * affect the library, similarly undoing commands in the library does not affect the editor.
 *
 * A context menu is available for the symbols in the list allowing individual symbols to be deleted. This can be
 * undone if required.
 *
 * Using the File->Import Library it is also possible to import symbols from another symbol file into the current
 * symbol library. These will then be appended to the current set of symbols.
 *
 * File->Close will close the current library leaving a new empty library that new symbols can be added to.
 */


#include "SymbolLibrary.h"

#include <QDataStream>
#include <QListWidget>
#include <QListWidgetItem>
#include <QPainter>
#include <QtAlgorithms>

#include <KLocale>

#include "Exceptions.h"
#include "SymbolListWidget.h"


/**
 * Construct a SymbolLibrary.
 * Set the url to Untitled and the index to 1.
 * The index will be set when a file is loaded and will be incremented when new symbols
 * have been added. It will be saved with the file for the next time it is loaded.
 */
SymbolLibrary::SymbolLibrary(SymbolListWidget *listWidget)
    :   m_listWidget(listWidget)
{
    clear();
}


/**
 * Destructor
 */
SymbolLibrary::~SymbolLibrary()
{
    clear();
}


/**
 * Clear the file of symbols.
 * Clears the undo stack, deletes all the QListWidgetItems and clears the symbol map.
 * The index is reset to 1.
 * The url is initialized to Untitled
 */
void SymbolLibrary::clear()
{
    m_undoStack.clear();

    if (m_listWidget) {
        foreach (qint16 index, indexes()) {
            m_listWidget->removeSymbol(index);
        }
    }

    m_symbols.clear();
    m_nextIndex = 1;
    m_url = QUrl(i18n("Untitled"));
}


/**
 * Get the path associated with an index.
 * If the index is not in the library it returns a default constructed Symbol.
 *
 * @param index a qint16 representing the index to find
 *
 * @return a Symbol
 */
Symbol SymbolLibrary::symbol(qint16 index) const
{
    return (m_symbols.contains(index) ? m_symbols.value(index) : Symbol());
}


/**
 * Take a symbol from the library.
 * Remove a symbol identified by it's index and return it.
 * The QListWidgetItem associated with the symbol is also removed and deleted.
 * If the index is not in the library it returns a default constructed symbol.
 *
 * @param index the index of the Symbol to be removed
 *
 * @return a Symbol
 */
Symbol SymbolLibrary::takeSymbol(qint16 index)
{
    Symbol symbol;

    if (m_symbols.contains(index)) {
        symbol = m_symbols.take(index);

        if (m_listWidget) {
            m_listWidget->removeSymbol(index);
        }
    }

    return symbol;
}


/**
 * Update the Symbol for an index in the library.
 * If the index supplied is 0, a new index will be taken from the m_nextIndex value which is
 * then incremented. This value will be returned.
 * When a LibraryListWidget has been linked to the SymbolLibrary the symbol is added to the
 * LibraryListWidget.
 *
 * @param index a qint16 representing the index
 * @param symbol a const reference to a Symbol
 *
 * @return a qint16 representing the index, this is useful when the original index was 0
 */
qint16 SymbolLibrary::setSymbol(qint16 index, const Symbol &symbol)
{
    if (!index) {
        index = m_nextIndex++;
    }

    m_symbols.insert(index, symbol);

    if (m_listWidget) {
        m_listWidget->addSymbol(index, symbol);
    }

    return index;
}


/**
 * Get the url for the file.
 *
 * @return a reference to a QUrl having the url of the file
 */
QUrl SymbolLibrary::url() const
{
    return m_url;
}


/**
 * Set the url for the file.
 *
 * @param url a const reference to a QUrl having the url of the file.
 */
void SymbolLibrary::setUrl(const QUrl &url)
{
    m_url = url;
    m_name = url.fileName();
}


/**
 * Get the name of the symbol library.
 */
QString  SymbolLibrary::name() const
{
    return m_name;
}


/**
 * Set the name of the symbol library.
 *
 * @param name the name to be set
 */
void SymbolLibrary::setName(const QString &name)
{
    m_name = name;
}


/**
 * Get a sorted list of symbol indexes
 *
 * @return a QList<qint16> of sorted indexes
 */
QList<qint16> SymbolLibrary::indexes() const
{
    QList<qint16> keys = m_symbols.keys();
    qSort(keys.begin(), keys.end());
    return keys;
}


/**
 * Get a pointer to the symbol library undo stack.
 *
 * @return a pointer to a QUndoStack
 */
QUndoStack *SymbolLibrary::undoStack()
{
    return &m_undoStack;
}


/**
 * Generate all the items in the library.
 * This will be called when a library file is loaded to generate all the new
 * QListWidgetItems for the symbols in the library and generate an icon for it.
 * A list of sorted QMap keys is retrieved to allow adding items in the correct
 * order.
 */
void SymbolLibrary::generateItems()
{
    if (!m_listWidget) {
        return;
    }

    foreach (qint16 index, indexes()) {
        m_listWidget->addSymbol(index, m_symbols[index]);
    }
}


/**
 * Stream out the file.
 * Symbol files are indicated with a magic string of KXStitchSymbols. The stream version is set
 * to maintain consistency with the streamed objects.
 * Write the version, current index and the map of symbols.
 *
 * @param stream a reference to a QDataStream
 * @param library a const reference to a SymbolLibrary
 *
 * @return a reference to a QDataStream
 */
QDataStream &operator<<(QDataStream &stream, const SymbolLibrary &library)
{
    stream.writeRawData("KXStitchSymbols", 15);
    stream.setVersion(QDataStream::Qt_4_0);
    stream << library.version;
    stream << library.m_nextIndex;

    if (stream.status() != QDataStream::Ok) {
        throw FailedWriteFile(stream.status());
    }

    stream << library.m_symbols;
    return stream;
}


/**
 * Stream in the file.
 * Initially clear the current contents and reset the url.
 * Symbol files are indicated with a magic string of KXStitchSymbols. The stream version is set
 * to maintain consistency with the streamed objects. Read and check the magic string. If this
 * is not a symbol file throw an exception.
 * Read in the version.
 * Read the data for the specified version. The stream is checked for errors and an exception is
 * thrown if there was an error.
 *
 * @param stream a reference to a QDataStream
 * @param library a reference to a SymbolLibrary
 *
 * @return a reference to a QDataStream
 */
QDataStream &operator>>(QDataStream &stream, SymbolLibrary &library)
{
    library.clear();

    char magic[15];
    stream.readRawData(magic, 15);

    if (strncmp(magic, "KXStitchSymbols", 15) == 0) {
        stream.setVersion(QDataStream::Qt_4_0);
        qint32 version;
        QMap<qint16, QPainterPath> paths_v100;
        QList<qint16> paths_v100_keys;
        stream >> version;

        switch (version) {
        case 101:
            stream >> library.m_nextIndex;

            if (stream.status() != QDataStream::Ok) {
                throw FailedReadFile(QString(i18n("Stream error")));
            }

            stream >> library.m_symbols;
            library.generateItems();
            break;

        case 100:
            stream >> library.m_nextIndex;
            library.m_nextIndex++;
            stream >> paths_v100;
            paths_v100_keys = paths_v100.keys();

            foreach (qint16 index, paths_v100_keys) {
                Symbol symbol;
                symbol.setPath(paths_v100[index]);
                library.setSymbol(index, symbol);
            }

            break;

        default:
            throw InvalidFileVersion(QString(i18n("Symbol file version %1", version)));
            break;
        }
    } else {
        throw InvalidFile();
    }

    return stream;
}
