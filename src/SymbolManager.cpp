/*
 * Copyright (C) 2010-2014 by Stephen Allewell
 * steve.allewell@gmail.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


/**
 * @file
 * Implement the SymbolManager class. This loads all symbol libraries from the kxstitch application
 * data folders and allows the selection of a library by name. The manager is implemented as a
 * singleton class accessible from anywhere in the application. Symbols need to be available to the
 * palette manager and from the renderer.
 */


/**
 * @page symbol_manager Symbol Manager
 * The symbol manager provides an interface between the application and the collection of symbol sets.
 * A number of symbol libraries can exist and a particular set can be selected in the palette manager
 * symbol selector.
 */


#include "SymbolManager.h"

#include <QDir>
#include <QDirIterator>
#include <QFile>
#include <QStandardPaths>
#include <QUrl>

#include <KLocalizedString>
#include <KMessageBox>

#include "Exceptions.h"
#include "Symbol.h"
#include "SymbolLibrary.h"


SymbolManager *SymbolManager::symbolManager = 0;


/**
 * Accessor for the static object
 */
SymbolManager &SymbolManager::self()
{
    if (symbolManager == 0) {
        symbolManager = new SymbolManager();
    }

    return *symbolManager;
}


/**
 * Constructor.
 */
SymbolManager::SymbolManager()
{
    refresh();
}


/**
 *Destructor. Delete all the symbol libraries.
 */
SymbolManager::~SymbolManager()
{
    qDeleteAll(m_symbolLibraries);
}


/**
 * Get a list of the symbol libraries available.
 *
 * @return QStringList of symbol library names.
 */
QStringList SymbolManager::libraries()
{
    QStringList libraryNames;

    QListIterator<SymbolLibrary *> i(self().m_symbolLibraries);

    while (i.hasNext()) {
        libraryNames.append(i.next()->name());
    }

    return libraryNames;
}


/**
 * Get a pointer to a symbol library by name
 *
 * @param name of the library required.
 *
 * @return pointer to the SymbolLibrary instance, returns null if no library found.
 */
SymbolLibrary *SymbolManager::library(const QString &name)
{
    for (int i = 0 ; i < self().m_symbolLibraries.count() ; ++i) {
        SymbolLibrary *symbolLibrary = self().m_symbolLibraries.at(i);

        if (symbolLibrary->name() == name) {
            return symbolLibrary;
        }
    }

    return 0;
}


/**
 * Get a list of files stored in the symbols path, iterating each one to create a new SymbolLibrary instance
 * and read the library.
 * Assumes that local resources are given before global ones and should take priority.
 */
void SymbolManager::refresh()
{
    const QStringList dirs = QStandardPaths::locateAll(QStandardPaths::DataLocation, "symbols", QStandardPaths::LocateDirectory);

    Q_FOREACH (const QString &dir, dirs) {
        QDirIterator it(dir, QStringList() << QStringLiteral("*.sym"));

        while (it.hasNext()) {
            SymbolLibrary *symbolLibrary = readLibrary(it.next());

            if (symbolLibrary) {
                m_symbolLibraries.append(symbolLibrary);
            }
        }
    }
}


/**
 * Read a symbol library.
 *
 * @param name path to the symbol file to be read.
 *
 * @return pointer to the SymbolLibrary instance created.
 */
SymbolLibrary *SymbolManager::readLibrary(const QString &name)
{
    SymbolLibrary *symbolLibrary = new SymbolLibrary;

    QFile file(name);

    if (file.open(QIODevice::ReadOnly)) {
        QDataStream stream(&file);

        try {
            stream >> *symbolLibrary;
            symbolLibrary->setName(QFileInfo(name).baseName());
        } catch (const InvalidFile &e) {
            KMessageBox::sorry(0, i18n("This does not appear to be a valid symbol file"));
            delete symbolLibrary;
            symbolLibrary = 0;
        } catch (const InvalidFileVersion &e) {
            KMessageBox::sorry(0, e.version);
            delete symbolLibrary;
            symbolLibrary = 0;
        } catch (const FailedReadFile &e) {
            KMessageBox::sorry(0, e.status);
            delete symbolLibrary;
            symbolLibrary = 0;
        }

        file.close();
    } else {
        KMessageBox::sorry(0, i18n("Failed to open the file %1", name));
        delete symbolLibrary;
        symbolLibrary = 0;
    }

    return symbolLibrary;
}
