/********************************************************************************
 *	Copyright (C) 2010 by Stephen Allewell					*
 *	stephen@mirramar.adsl24.co.uk						*
 *										*
 *	This program is free software; you can redistribute it and/or modify	*
 *	it under the terms of the GNU General Public License as published by	*
 *	the Free Software Foundation; either version 2 of the License, or	*
 *	(at your option) any later version.					*
 ********************************************************************************/


#ifndef Document_H
#define Document_H


#include <QPolygon>
#include <QUndoStack>

#include <KUrl>

#include "BackgroundImages.h"
#include "configuration.h"
#include "Pattern.h"
#include "PrinterConfiguration.h"


class Editor;
class Palette;
class Preview;


class Document
{
	public:
		Document();
		~Document();

		void initialiseNew();

		bool load(const KUrl &);
		void save();

		void setUrl(const KUrl &);
		KUrl url() const;

		void addView(Editor *);
		void addView(Palette *);
		void addView(Preview *);
		
		Editor *editor() const;
		Palette *palette() const;
		Preview *preview() const;

		QVariant property(const QString &) const;
		void setProperty(const QString &, const QVariant &);

		QUndoStack &undoStack();

		BackgroundImages &backgroundImages();
		Pattern *pattern();
		PrinterConfiguration printerConfiguration() const;
		void setPrinterConfiguration(const PrinterConfiguration &);

	private:
		bool readPCStitch5File(QDataStream &);
		QString readPCStitchString(QDataStream &);

		bool readKXStitchV2File(QDataStream &);
		bool readKXStitchV3File(QDataStream &);
		bool readKXStitchV4File(QDataStream &);
		bool readKXStitchV5File(QDataStream &);
		bool readKXStitchV6File(QDataStream &);
		bool readKXStitchV7File(QDataStream &);

		static const int version = 104;

		QMap<QString, QVariant>	m_properties;

		KUrl	m_url;

		QUndoStack	m_undoStack;

		Editor	*m_editor;
		Palette	*m_palette;
		Preview	*m_preview;

		BackgroundImages	m_backgroundImages;
		Pattern			*m_pattern;
		PrinterConfiguration	m_printerConfiguration;
};


#endif // Document_H
