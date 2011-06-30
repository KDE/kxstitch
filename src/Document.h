/********************************************************************************
 *	Copyright (C) 2010 by Stephen Allewell					*
 *	stephen@mirramar.adsl24.co.uk						*
 *										*
 *	This program is free software; you can redistribute it and/or modify	*
 *	it under the terms of the GNU General Public License as published by	*
 *	the Free Software Foundation; either version 2 of the License, or	*
 *	(at your option) any later version.					*
 ********************************************************************************/


#ifndef __Document_H
#define __Document_H


#include <QPolygon>
#include <QUndoStack>

#include <KUrl>

#include "BackgroundImages.h"
#include "configuration.h"
#include "DocumentPalette.h"
#include "Layers.h"
#include "StitchData.h"


class Editor;
class Palette;
class Preview;


class Document
{
	public:
		Document();
		~Document();

		void initialiseNew();

		void load(const KUrl &);
		void save();

		void setUrl(const KUrl &);
		KUrl url() const;

		void addView(Editor *);
		void addView(Palette *);
		void addView(Preview *);

		QVariant property(const QString &) const;
		void setProperty(const QString &, const QVariant &);

		QUndoStack &undoStack();

		Layers &layers();
		BackgroundImages &backgroundImages();
		DocumentPalette &documentPalette();
		StitchData &stitchData();

	private:
		static const int version = 100;

		QMap<QString, QVariant>	m_properties;

		KUrl	m_url;

		QUndoStack	m_undoStack;

		Editor	*m_editor;
		Palette	*m_palette;
		Preview	*m_preview;

		Layers			m_layers;
		BackgroundImages	m_backgroundImages;
		DocumentPalette		m_documentPalette;
		StitchData		m_stitchData;
};


#endif
