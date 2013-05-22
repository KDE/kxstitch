/*
 * Copyright (C) 2010 by Stephen Allewell
 * sallewell@users.sourceforge.net
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


/**
 * @file
 * Header file for the SymbolManager class.
 */


#ifndef SymbolManager_H
#define SymbolManager_H


#include <QList>
#include <QStringList>


class SymbolLibrary;


/**
 * @brief Manages the collection of symbol libraries.
 *
 * The symbol manager manages the set of symbol libraries. It allows retrieval of the names
 * of the libraries and retrieval of a specific library by name.
 *
 * The manager is implemented as a singleton class accessible via static functions from all
 * parts of the application.
 */
class SymbolManager
{
public:
    ~SymbolManager();

    static QStringList libraries();
    static SymbolLibrary *library(const QString &name);

private:
    static SymbolManager &self();
    SymbolManager();

    void refresh();
    SymbolLibrary *readLibrary(const QString &name);

    static SymbolManager    *symbolManager;         /**< pointer to the static symbol manager */
    QList<SymbolLibrary *>  m_symbolLibraries;      /**< list of the symbol libraries loaded */
};


#endif // SymbolManager_H
