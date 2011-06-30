/********************************************************************************
 *	Copyright (C) 2010 by Stephen Allewell					*
 *	stephen@mirramar.adsl24.co.uk						*
 *										*
 *	This program is free software; you can redistribute it and/or modify	*
 *	it under the terms of the GNU General Public License as published by	*
 *	the Free Software Foundation; either version 2 of the License, or	*
 *	(at your option) any later version.					*
 ********************************************************************************/


#include <QDataStream>
#include <QFile>
#include <QVariant>

#include <KDebug>
#include <KIO/NetAccess>
#include <KLocale>
#include <KMessageBox>

#include "Document.h"
#include "Editor.h"
#include "Palette.h"
#include "Preview.h"


Document::Document()
	:	m_editor(0),
		m_palette(0),
		m_preview(0)
{
	initialiseNew();
}


Document::~Document()
{
}


void Document::initialiseNew()
{
	m_undoStack.clear();

	m_layers.clear();
	m_layers.addLayer(0, i18n("Layer 0"));

	m_backgroundImages.clear();

	m_documentPalette.clear();
	m_documentPalette.setSchemeName(Configuration::palette_DefaultScheme());

	m_stitchData.clear();
	m_stitchData.resize(Configuration::document_Width(), Configuration::document_Height());

	setProperty("unitsFormat", Configuration::document_UnitsFormat());
	setProperty("title", QString());
	setProperty("author", QString());
	setProperty("copyright", QString());
	setProperty("fabric", QString());
	setProperty("fabricColor", Configuration::editor_BackgroundColor());
	setProperty("instructions", QString());
	setProperty("cellWidth", Configuration::editor_CellWidth());
	setProperty("cellHeight", Configuration::editor_CellHeight());
	setProperty("cellHorizontalGrouping", Configuration::editor_CellHorizontalGrouping());
	setProperty("cellVerticalGrouping", Configuration::editor_CellVerticalGrouping());
	setProperty("horizontalClothCount", Configuration::editor_HorizontalClothCount());
	setProperty("verticalClothCount", Configuration::editor_VerticalClothCount());
	setProperty("clothCountUnits", Configuration::editor_ClothCountUnits());
	setProperty("clothCountLink", Configuration::editor_ClothCountLink());
	setProperty("thickLineWidth", Configuration::editor_ThickLineWidth());
	setProperty("thinLineWidth", Configuration::editor_ThinLineWidth());
	setProperty("thickLineColor", Configuration::editor_ThickLineColor());
	setProperty("thinLineColor", Configuration::editor_ThinLineColor());
	setProperty("formatScalesAs", Configuration::editor_FormatScalesAs());
	setProperty("renderStitchesAs", Configuration::renderer_RenderStitchesAs());
	setProperty("renderBackstitchesAs", Configuration::renderer_RenderBackstitchesAs());
	setProperty("renderKnotsAs", Configuration::renderer_RenderKnotsAs());
	setProperty("colorHilight", false); // Configuration::editor_ColorHilight()
	setProperty("renderBackgroundImages", true); // Configuration::editor_PaintBackgroundImages();
	setProperty("renderGrid", true); // Configuration::editor_PaintGrid();
	setProperty("renderStitches", true); // Configuration::editor_PaintStitches();
	setProperty("renderBackstitches", true); // Configuration::editor_PaintBackstitches;
	setProperty("renderFrenchKnots", true); // Configuration::editor_PaintFrenchKnots;
	setProperty("maskLayer",false);
	setProperty("maskStitch", false);
	setProperty("maskColor", false);
	setProperty("maskBackstitch", false);
	setProperty("maskKnot", false);

	setUrl(i18n("Untitled"));
}


QUndoStack &Document::undoStack()
{
	return m_undoStack;
}


void Document::setUrl(const KUrl &url)
{
	m_url = url;
}


KUrl Document::url() const
{
	return m_url;
}


void Document::addView(Editor *editor)
{
	m_editor = editor;
}


void Document::addView(Palette *palette)
{
	m_palette = palette;
}


void Document::addView(Preview *preview)
{
	m_preview = preview;
}


Layers &Document::layers()
{
	return m_layers;
}


BackgroundImages &Document::backgroundImages()
{
	return m_backgroundImages;
}


DocumentPalette &Document::documentPalette()
{
	return m_documentPalette;
}


StitchData &Document::stitchData()
{
	return m_stitchData;
}


void Document::load(const KUrl &documentUrl)
{
	initialiseNew();
	bool readOk = false;

	if (!documentUrl.isEmpty())
	{
		QString sourceFile;
		if (KIO::NetAccess::download(documentUrl, sourceFile, 0))
		{
			QFile file(sourceFile);
			if (file.open(QIODevice::ReadOnly))
			{
				QDataStream stream(&file);
				char header[30];
				stream.readRawData(header, 30);
				if (strncmp(header, "KXStitchDoc", 11) == 0)
				{
					// a current KXStitchDoc format file
					stream.device()->seek(11);
					qint32 version;
					stream >> version;
					switch (version)
					{
						case 100:
							stream >> m_properties;
							stream >> m_layers;
							stream >> m_backgroundImages;
							stream >> m_documentPalette;
							stream >> m_stitchData;
							readOk = true;
							break;

						default:
							// not supported
							// display error
							break;
					}
				}
				else
				{
					// try some other formats
				}
				file.close();
			}
			KIO::NetAccess::removeTempFile(sourceFile);
		}
		else
		{
			KMessageBox::error(0, KIO::NetAccess::lastErrorString());
		}
	}

	if (readOk)
	{
		setUrl(documentUrl);
	}
	else
	{
		initialiseNew();
	}
}


void Document::save()
{
	if (!m_url.isEmpty())
	{
		QFile file(m_url.path());
		if (file.open(QIODevice::WriteOnly | QIODevice::Truncate))
		{
			QDataStream stream(&file);
			stream.writeRawData("KXStitchDoc", 11);
			stream << version;
			stream << m_properties;
			stream << m_layers;
			stream << m_backgroundImages;
			stream << m_documentPalette;
			stream << m_stitchData;

			file.close();

			m_undoStack.setClean();
		}
		else
		{
			KMessageBox::error(0, QString(i18n("The file %1\ncould not be opened.\n%2")).arg(m_url.path()).arg(file.errorString()), i18n("Error opening file"));
		}
	}
}


QVariant Document::property(const QString &name) const
{
	if (m_properties.contains(name))
	{
		return m_properties[name];
	}
	else
		kDebug() << "Asked for non existing property" << name;
}


void Document::setProperty(const QString &name, const QVariant &value)
{
	if (name == QString("verticalClothCount"))
	{
		setProperty("cellHeight", int(property("cellWidth").toDouble()*property("horizontalClothCount").toDouble()/value.toDouble()));
	}

	m_properties[name] = value;
}
