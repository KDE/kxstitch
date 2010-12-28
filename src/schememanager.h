/****************************************************************************
 *	Copyright (C) 2010 by Stephen Allewell									*
 *	stephen@mirramar.adsl24.co.uk											*
 *																			*
 *	This program is free software; you can redistribute it and/or modify	*
 *	it under the terms of the GNU General Public License as published by	*
 *	the Free Software Foundation; either version 2 of the License, or		*
 *	(at your option) any later version.										*
 ****************************************************************************/


#ifndef SCHEMEMANAGER_H
#define SCHEMEMANAGER_H


#include <QList>
#include <QStringList>

#include <KDirWatch>


class FlossScheme;


class SchemeManager : private QObject
{
	Q_OBJECT

	public:
		SchemeManager();
		~SchemeManager();
		FlossScheme *createScheme(QString schemeName);
		FlossScheme *readScheme(QString name);
		bool writeScheme(QString name);
		QStringList schemes();
		FlossScheme *scheme(QString name);

	private slots:
		void refresh(QString name = QString());

	private:
		typedef QMap<QString, QColor> CalibratedColor;
		QList<FlossScheme*> m_flossSchemes;
		KDirWatch m_dirWatch;
};


#endif
