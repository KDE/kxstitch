/*
 * Copyright (C) 2010-2015 by Stephen Allewell
 * steve.allewell@gmail.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include "SchemeManager.h"

#include <QDir>
#include <QDirIterator>
#include <QFile>
#include <QFileInfo>
#include <QStandardPaths>
#include <QUrl>
#include <QDomDocument>

#include <KLocalizedString>
#include <KMessageBox>

#include "Floss.h"
#include "FlossScheme.h"

SchemeManager *SchemeManager::schemeManager = nullptr;

/**
    Accessor for the static object
    */
SchemeManager &SchemeManager::self()
{
    if (schemeManager == nullptr) {
        schemeManager = new SchemeManager();
    }

    return *schemeManager;
}

/**
    Constructor.
    */
SchemeManager::SchemeManager()
    : QObject()
{
    /** Refresh the list of floss schemes. */
    refresh();
}

/**
    Destructor. Delete all the floss schemes.
    */
SchemeManager::~SchemeManager()
{
    qDeleteAll(m_flossSchemes);
}

/**
    Create a new scheme.  The name is checked to see if it already exists, if not a new instance of a FlossScheme
    is created.
    @param schemeName the name of the scheme to be created.
    @return a pointer to the new FlossScheme instance, null if it already exists or a new one could not be created.
    */
FlossScheme *SchemeManager::createScheme(QString schemeName)
{
    FlossScheme *flossScheme = nullptr;

    if (scheme(schemeName) == nullptr) {
        if ((flossScheme = new FlossScheme)) {
            flossScheme->setSchemeName(schemeName);
            self().m_flossSchemes.append(flossScheme);
        }
    }

    return flossScheme;
}

/**
    Get a list of the FlossSchemes available.
    @return QStringList of scheme names.
    */
QStringList SchemeManager::schemes()
{
    QStringList schemeNames;

    QListIterator<FlossScheme *> i(self().m_flossSchemes);

    while (i.hasNext()) {
        schemeNames.append(i.next()->schemeName());
    }

    return schemeNames;
}

/**
    Get a pointer to a scheme by name
    @param name of the scheme required.
    @return pointer to the FlossScheme instance, returns null if no scheme found.
    */
FlossScheme *SchemeManager::scheme(QString name)
{
    FlossScheme *flossScheme;

    QListIterator<FlossScheme *> i(self().m_flossSchemes);

    while (i.hasNext()) {
        flossScheme = i.next();

        if (flossScheme->schemeName() == name) {
            return flossScheme;
        }
    }

    return nullptr;
}

/**
    Read a scheme.
    @param name path to the xml file to be read.
    @return pointer to the FlossScheme instance created.
    */
FlossScheme *SchemeManager::readScheme(QString name)
{
    QFile xmlFile(name);

    if (!xmlFile.open(QIODevice::ReadOnly)) {
        KMessageBox::error(nullptr, QString(i18n("Cannot open file: %1", name)), i18n("Error"));
        return nullptr;
    }

    QDomDocument    dom;

    if (!dom.setContent(&xmlFile)) {
        KMessageBox::error(nullptr, QString(i18n("Cannot set DomDocument content from file: %1", name)), i18n("Error"));
        xmlFile.close();
        return nullptr;
    }
    xmlFile.close();

    FlossScheme *flossScheme = new FlossScheme();

    if (flossScheme == nullptr) {
        KMessageBox::error(nullptr, QString(i18n("Error creating instance of a FlossScheme for scheme: %1", name)), i18n("Error"));
        return nullptr;
    }

    QDomElement documentElement = dom.documentElement();

    if (documentElement.tagName() != QStringLiteral("flossScheme")) {
        // wrong content
        KMessageBox::error(nullptr, QString(i18n("File %1 does not appear to be a floss scheme", name)), i18n("Error"));
        return nullptr;
    }

    QDomNodeList nodes = documentElement.elementsByTagName(QStringLiteral("title"));

    if (nodes.count() != 1) {
        // only expecting one of these
        KMessageBox::error(nullptr, QString(i18n("Multiple title elements found in file: %1", name)), i18n("Error"));
        return nullptr;
    }

    QString title = nodes.at(0).toElement().text();

    flossScheme->setSchemeName(title);
    flossScheme->setPath(name);

    QDomNodeList flossNodes = documentElement.elementsByTagName(QStringLiteral("floss"));

    for (int index = 0 ; index < flossNodes.count() ; ++index) {

        QDomElement node = flossNodes.at(index).toElement();  // <floss>...</floss>

        // Create default invalid values to ensure all elements are read.
        QString flossName;
        QString flossDescription;
        int red = -1, green = -1, blue = -1;

        QDomNodeList flossElements = node.childNodes();

        for (int flossChild = 0 ; flossChild < flossElements.count() ; ++flossChild) {
            QDomElement flossElement = flossElements.at(flossChild).toElement();

            if (flossElement.tagName() == QStringLiteral("name")) {
                flossName = flossElement.text();
            }

            if (flossElement.tagName() == QStringLiteral("description")) {
                flossDescription = flossElement.text();
            }

            if (flossElement.tagName() == QStringLiteral("color")) {
                QDomNodeList colorElements = flossElement.childNodes();

                for (int colorChild = 0 ; colorChild < colorElements.count(); ++colorChild) {
                    QDomElement colorElement = colorElements.at(colorChild).toElement();

                    if (colorElement.tagName() == QStringLiteral("red")) {
                        red = colorElement.text().toInt();
                    }

                    if (colorElement.tagName() == QStringLiteral("green")) {
                        green = colorElement.text().toInt();
                    }

                    if (colorElement.tagName() == QStringLiteral("blue")) {
                        blue = colorElement.text().toInt();
                    }
                }
            }
        }

        if (flossName.isNull() || flossDescription.isNull() || (red == -1) || (green == -1) || (blue == -1)) {
            KMessageBox::error(nullptr, QString(i18n("Malformed XML in file: %1", name)), i18n("Error"));
        } else {
            flossScheme->addFloss(new Floss(flossName, flossDescription, QColor(red, green, blue)));
        }
    }

    return flossScheme;
}

/**
    Save a modified scheme to a writable location.
    @param name of the scheme to be saved.
    @return true if the scheme was successfully saved, false otherwise.
    */
bool SchemeManager::writeScheme(QString name)
{
    FlossScheme *flossScheme = scheme(name);
    QFileInfo fileInfo(flossScheme->path());

    if (!fileInfo.isWritable()) {
        QString writableDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation); // this may be empty or may not exist

        if (writableDir.isEmpty()) {
            KMessageBox::error(nullptr, i18n("Unable to locate a writable directory\nto store the scheme."));
            return false;
            // TODO Allow user to select a location to store the calibrated schemes
        }

        writableDir += QLatin1String("/schemes/");

        if (!QDir(writableDir).exists()) {
            if (!QDir().mkpath(writableDir)) {
                KMessageBox::error(nullptr, i18n("Unable to locate a writable directory\nto store the scheme."));
                return false;
                // TODO Allow user to select a location to store the calibrated schemes
            }
        }

        // at this point a writable directory should be in writableDir

        QString writablePath = writableDir + fileInfo.fileName();
        fileInfo.setFile(writablePath);
    }

    QFile schemeFile(fileInfo.filePath());

    // TODO convert the following to use QDomDocument

    if (schemeFile.open(QIODevice::WriteOnly)) {
        QTextStream stream(&schemeFile);
        stream << "<!DOCTYPE flossScheme SYSTEM \"flossScheme.dtd\">\n<flossScheme>\n";
        stream << "<title>" << name.replace(QLatin1Char('&'), QLatin1String("&amp;")) << "</title>\n"; // includes fixup for the J&P Coates scheme
        QListIterator<Floss *> it(flossScheme->flosses());

        while (it.hasNext()) {
            Floss *floss = it.next();
            stream << "<floss><name>" << floss->name() << "</name><description>" << floss->description() << "</description>";
            stream << "<color><red>" << floss->color().red() << "</red><green>" << floss->color().green() << "</green><blue>" << floss->color().blue()
                   << "</blue></color></floss>\n";
        }

        stream << "</flossScheme>\n";
        schemeFile.close();

        return true;
    }

    return false;
}

/**
    Get a list of files that contain xml schemes, iterating each one to create a new FlossScheme instance
    and read the scheme.  Each path found is added to the KDirWatch instance to allow automatic reload of scheme
    data if it is changed outside of KXStitch.
    Assumes that local resources are given before global ones and should take priority.
    */
void SchemeManager::refresh()
{
    const QStringList dirs = QStandardPaths::locateAll(QStandardPaths::AppDataLocation, QStringLiteral("schemes"), QStandardPaths::LocateDirectory);

    Q_FOREACH (const QString &dir, dirs) {
        QDirIterator it(dir, QStringList() << QLatin1String("*.xml"));

        while (it.hasNext()) {
            FlossScheme *flossScheme = readScheme(it.next());

            if (flossScheme) {
                for (int i = 0; i < m_flossSchemes.count(); ++i) {
                    if (m_flossSchemes.at(i)->schemeName() == flossScheme->schemeName()) {
                        delete flossScheme;
                        flossScheme = nullptr;
                        break;
                    }
                }

                if (flossScheme) {
                    m_flossSchemes.append(flossScheme);
                }
            }
        }
    }
}

#include "moc_SchemeManager.cpp"
