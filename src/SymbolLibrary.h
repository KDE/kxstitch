/*
 * Copyright (C) 2011-2015 by Stephen Allewell
 * steve.allewell@gmail.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


/**
 * @file
 * Header file for the SymbolLibrary class.
 */


#ifndef SymbolLibrary_H
#define SymbolLibrary_H


#include <QMap>
#include <QPainterPath>
#include <QUndoStack>

#include <KUrl>

#include "Symbol.h"


class QDataStream;
class QListWidgetItem;

class SymbolListWidget;


/**
 * @brief Manages the library of symbols.
 *
 * The symbol library holds all the symbols that have been added to it or loaded from a file.
 * The symbols are indexed and the index is incremented for each symbol added starting from 1.
 * When a SymbolListWidget is assigned to the SymbolLibrary each of the symbols is added to
 * the SymbolListWidget which will create a QListWidgetItem which is assigned the QIcon that
 * is generated from the QPainterPath associated with the index.
 */
class SymbolLibrary
{
public:
    explicit SymbolLibrary(SymbolListWidget *listWidget = 0);
    ~SymbolLibrary();

    void clear();

    Symbol symbol(qint16 index) const;
    Symbol takeSymbol(qint16 index);
    qint16 setSymbol(qint16 index, const Symbol &symbol);

    KUrl url() const;
    void setUrl(const KUrl &url);

    QString name() const;
    void setName(const QString &name);

    QList<qint16> indexes() const;

    QUndoStack *undoStack();

    friend QDataStream &operator<<(QDataStream &stream, const SymbolLibrary &library);
    friend QDataStream &operator>>(QDataStream &stream, SymbolLibrary &library);

private:
    void generateItems();

    static const qint32 version = 101;      /**< stream version of this file */

    QUndoStack m_undoStack;                 /**< holds the commands that have made changes to this library */

    KUrl    m_url;                          /**< url of the file loaded, this may be Untitled */

    QString m_name;                         /**< name of the symbol library */

    SymbolListWidget *m_listWidget;         /**< pointer to a QListWidget containing the QListWidgetItems for the QIcons, this may be null for an imported file */

    qint16                  m_nextIndex;    /**< index for the next symbol added */
    QMap<qint16, Symbol>    m_symbols;      /**< map of the Symbol to indexes */
};


QDataStream &operator<<(QDataStream &stream, const SymbolLibrary &library);
QDataStream &operator>>(QDataStream &stream, SymbolLibrary &library);


#endif
