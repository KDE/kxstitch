/*
 * Copyright (C) 2010-2015 by Stephen Allewell
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

#include <QCoreApplication>
#include <QDir>
#include <QDirIterator>
#include <QFile>
#include <QStandardPaths>
#include <QUrl>

#include <iterator>
#include <utility>

#include <KLocalizedString>
#include <KMessageBox>

#include "Exceptions.h"
#include "Symbol.h"
#include "SymbolLibrary.h"

SymbolManager *SymbolManager::symbolManager = nullptr;

/**
 * Accessor for the static object
 */
SymbolManager &SymbolManager::self()
{
    if (symbolManager == nullptr) {
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
 * @return QStringList of unique symbol library names, sorted.
 */
QStringList SymbolManager::libraries()
{
    return self().m_symbolLibraries.keys();
}

/**
 * Get a pointer to a symbol library by name
 *
 * @param name of the library required.
 *
 * @return pointer to the SymbolLibrary instance, falls back to the first library if no library found.
 */
SymbolLibrary *SymbolManager::library(const QString &name)
{
    const QMap<QString, SymbolLibrary *> &libraries = self().m_symbolLibraries;

    if (SymbolLibrary *symbolLibrary = libraries.value(name)) {
        return symbolLibrary;
    }

    // Support legacy configs that stored the library as a numeric index.
    bool legacyIndexValid = false;
    int legacyIndex = name.toInt(&legacyIndexValid);

    if (legacyIndexValid && legacyIndex >= 0 && legacyIndex < libraries.count()) {
        return *std::next(libraries.cbegin(), legacyIndex);
    }

    return libraries.isEmpty() ? nullptr : libraries.first();
}

/**
 * Get a list of files stored in the symbols path, iterating each one to create a new SymbolLibrary instance
 * and read the library.
 * Assumes that local resources are given before global ones and should take priority, so a library
 * is only read the first time its name is found.
 */
void SymbolManager::refresh()
{
    QStringList dirs = QStandardPaths::locateAll(QStandardPaths::AppDataLocation, QStringLiteral("symbols"), QStandardPaths::LocateDirectory);

    const QDir appDir(QCoreApplication::applicationDirPath());
    dirs << appDir.absoluteFilePath(QStringLiteral("../share/kxstitch/symbols")) << appDir.absoluteFilePath(QStringLiteral("../../usr/share/kxstitch/symbols"));

    QStringList canonicalDirs;

    for (const QString &dir : std::as_const(dirs)) {
        const QString canonicalDir = QDir(dir).canonicalPath(); // empty if the directory does not exist

        if (!canonicalDir.isEmpty() && !canonicalDirs.contains(canonicalDir)) {
            canonicalDirs.append(canonicalDir);
        }
    }

    for (const QString &dir : std::as_const(canonicalDirs)) {
        QDirIterator it(dir, QStringList() << QStringLiteral("*.sym"), QDir::Files);

        while (it.hasNext()) {
            const QString path = it.next();
            const QString name = it.fileInfo().baseName();

            if (m_symbolLibraries.contains(name)) {
                continue;
            }

            if (SymbolLibrary *symbolLibrary = readLibrary(path)) {
                m_symbolLibraries.insert(name, symbolLibrary);
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
            KMessageBox::error(nullptr, i18n("This does not appear to be a valid symbol file"));
            delete symbolLibrary;
            symbolLibrary = nullptr;
        } catch (const InvalidFileVersion &e) {
            KMessageBox::error(nullptr, e.version);
            delete symbolLibrary;
            symbolLibrary = nullptr;
        } catch (const FailedReadFile &e) {
            KMessageBox::error(nullptr, e.status);
            delete symbolLibrary;
            symbolLibrary = nullptr;
        }

        file.close();
    } else {
        KMessageBox::error(nullptr, i18n("Failed to open the file %1", name));
        delete symbolLibrary;
        symbolLibrary = nullptr;
    }

    return symbolLibrary;
}
