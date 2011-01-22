/****************************************************************************
 *	Copyright (C) 2010 by Stephen Allewell									*
 *	stephen@mirramar.adsl24.co.uk											*
 *																			*
 *	This program is free software; you can redistribute it and/or modify	*
 *	it under the terms of the GNU General Public License as published by	*
 *	the Free Software Foundation; either version 2 of the License, or		*
 *	(at your option) any later version.										*
 ****************************************************************************/


#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QXmlInputSource>
#include <QXmlSimpleReader>

#include <KDebug>
#include <KLocale>
#include <KMessageBox>
#include <KStandardDirs>
#include <KUrl>

#include "floss.h"
#include "flossscheme.h"
#include "schememanager.h"
#include "schemeparser.h"


/**
	Constructor.
	*/
SchemeManager::SchemeManager() : QObject()
{
	/** Refresh the list of floss schemes. */
	refresh();
	/** Connect the KDirWatch dirty signal to the refresh function so that new floss schemes are read
		automatically if changed outside of this instance of KXStitch. */
	connect(&m_dirWatch, SIGNAL(dirty(QString)), this, SLOT(refresh(QString)));
}


/**
	Destructor. Delete all the floss schemes.
	*/
SchemeManager::~SchemeManager()
{
	qDeleteAll(m_flossSchemes.constBegin(), m_flossSchemes.constEnd());
}


/**
	Create a new scheme.  The name is checked to see if it already exists, if not a new instance of a FlossScheme
	is created.
	@param schemeName the name of the scheme to be created.
	@return a pointer to the new FlossScheme instance, null if it already exists or a new one could not be created.
	*/
FlossScheme *SchemeManager::createScheme(QString schemeName)
{
	FlossScheme *flossScheme = 0;

	if (scheme(schemeName) == 0)
	{
		if (flossScheme = new FlossScheme)
		{
			flossScheme->setSchemeName(schemeName);
			m_flossSchemes.append(flossScheme);
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

	for (int i = 0 ; i < m_flossSchemes.size() ; i++)
		schemeNames.append(m_flossSchemes.at(i)->schemeName());

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
	for (int i = 0 ; i < m_flossSchemes.size() ; i++)
	{
		flossScheme = m_flossSchemes.at(i);
		if (flossScheme->schemeName() == name)
		{
			return flossScheme;
		}
	}

	return 0;
}


/**
	Read a scheme.
	@param name path to the xml file to be read.
	@return pointer to the FlossScheme instance created.
	*/
FlossScheme *SchemeManager::readScheme(QString name)
{
	SchemeParser handler;
	QFile xmlFile(name);
	QXmlInputSource source(&xmlFile);
	QXmlSimpleReader reader;
	reader.setContentHandler(&handler);

	bool success = reader.parse(source);
	if (!success)
		KMessageBox::error(0, QString(i18n("Error reading scheme %1\n%2.")).arg(name).arg(handler.errorString()), i18n("Error reading floss scheme."));

	FlossScheme *flossScheme = handler.flossScheme();
	if (!success || scheme(flossScheme->schemeName()))
	{
		delete flossScheme;
		flossScheme = 0;
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
	QString writableDir = KGlobal::dirs()->saveLocation("appdata", "schemes/");
	if (writableDir.isNull())
	{
		KMessageBox::sorry(0,i18n("Unable to locate a writable directory\nto store the scheme."));
		// TODO Allow user to select a location to store the calibrated schemes
	}
	else
	{
		QFile schemeFile(QString("%1/%2.xml").arg(writableDir).arg(name.toLower()));
		if (schemeFile.open(QIODevice::WriteOnly))
		{
			QTextStream stream(&schemeFile);
			stream << "<!DOCTYPE flossScheme SYSTEM \"flossScheme.dtd\">\n<flossScheme>\n";
			stream << "<title>" << name << "</title>\n";
			FlossScheme *flossScheme = scheme(name);
			QListIterator<Floss*> it = flossScheme->flossIterator();
			while (it.hasNext())
			{
				Floss *floss = it.next();
				stream << "<floss><name>" << floss->name() << "</name><description>" << floss->description() << "</description>";
				stream << "<color><red>" << floss->color().red() << "</red><green>" << floss->color().green() << "</green><blue>" << floss->color().blue() << "</blue></color></floss>\n";
			}
			stream << "</flossScheme>\n";
			schemeFile.close();

			return true;
		}
	}

	return false;
}


/**
	Get a list of files that contain xml schemes, iterating each one to create a new FlossScheme instance
	and read the scheme.  Each path found is added to the KDirWatch instance to allow automatic reload of scheme
	data if it is changed outside of KXStitch.
	@param name // TODO is this parameter required.
	*/
void SchemeManager::refresh(QString name)
{
	QStringList schemeList = KGlobal::dirs()->findAllResources("appdata", "schemes/*.xml");
	QStringListIterator it(schemeList);
	while (it.hasNext())
	{
		QString nextResource = it.next();
		QString resourcePath = KUrl(nextResource).path(KUrl::RemoveTrailingSlash);
		if (! m_dirWatch.contains(resourcePath))
			m_dirWatch.addDir(resourcePath);
		FlossScheme *flossScheme = readScheme(nextResource);
		if (flossScheme)
			m_flossSchemes.append(flossScheme);
	}
}
