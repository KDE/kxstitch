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
#include "Floss.h"
#include "FlossScheme.h"
#include "Palette.h"
#include "Preview.h"
#include "SchemeManager.h"


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
	bool foreign = false;

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
				else if (strncmp(header, "PCStitch 5 Pattern File", 23) == 0)
				{
					readOk = readPCStitch5File(stream);
					foreign = true;
				}
				else
				{
					// try other formats
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
		if (!foreign)			// foreign formats like PC Stitch 5 should not have the url set in order not
			setUrl(documentUrl);	// to overwrite the existing file.
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


bool Document::readPCStitch5File(QDataStream &stream)
{
	stream.setByteOrder(QDataStream::LittleEndian);
	stream.device()->seek(256);

	qint32 unknown;
	stream >> unknown;
	stream >> unknown;

	qint16 width;
	qint16 height;
	stream >> width;
	stream >> height;
	m_stitchData.resize(width, height);
	setProperty("unitsFormat", Configuration::EnumDocument_UnitsFormat::Stitches);

	qint16 clothCount;
	stream >> clothCount;
	setProperty("horizontalClothCount", double(clothCount));
	stream >> clothCount;
	setProperty("verticalClothCount", double(clothCount));
	setProperty("clothCountUnits", Configuration::EnumEditor_ClothCountUnits::Inches);

	setProperty("author", readPCStitchString(stream));
	setProperty("copyright", readPCStitchString(stream));
	setProperty("title", readPCStitchString(stream));
	setProperty("fabric", readPCStitchString(stream));

	QString instructions = readPCStitchString(stream);
	int index = instructions.indexOf("}}");		// end of font defs
	index += 4;					// skip paste }} and CR LF
	index = instructions.indexOf('}', index);	// end of color table defs
	index += 3;					// skip paste } and CR LF
	index = instructions.indexOf(' ', index);	// find first space - end of text def
	index++;					// and skip past it
	// index should now point to the first character of the instructions
	instructions = instructions.remove(0, index);
	instructions.truncate(instructions.length()-10);
	setProperty("instructions", instructions);

	setProperty("fabricColor", Qt::white);

	char *buffer = new char[51];
	stream.readRawData(buffer, 25);
	if (strncmp(buffer, "PCStitch 5 Floss Palette!", 25) == 0)
	{
		qint16 colors;
		stream >> colors;
		stream.readRawData(buffer, 30);	// this should be 'DMC       Anchor    Coates    '
		buffer[30] = '\0';
		QString fontName = readPCStitchString(stream); // the font name, usually 'PCStitch Symbols'
		// assume this palette will be DMC
		m_documentPalette.setSchemeName("DMC");
		FlossScheme *scheme = SchemeManager::scheme("DMC");
		if (scheme == 0) return false;	// this shouldn't happen because DMC should always be available
		stream >> unknown;
		for (int i = 0 ; i < colors ; i++)
		{
			stream >> unknown;				// color value, probably RGBA, dont need it
			stream.readRawData(buffer, 30);			// includes DMC,Anchor and Coates names
			buffer[10] = '\0';
			QString colorName = QString(buffer).trimmed();	// minus the white space
			stream.readRawData(buffer, 50);			// floss description, discard as this will come from the scheme manager
			buffer[50] = '\0';
			QString description = QString(buffer).trimmed();
			qint8 symbol;
			qint16 stitchStrands;
			qint16 backstitchStrands;
			stream >> symbol >> stitchStrands >> backstitchStrands;
			Floss *floss = scheme->find(colorName);
			if (floss == 0)
			{
			        KMessageBox::sorry(0, QString(i18n("Unable to find color %1 in scheme DMC")).arg(colorName));
			}
			else
			{
				DocumentFloss *documentFloss = new DocumentFloss(colorName, QChar(symbol), Qt::SolidLine, 2, 1);
				documentFloss->setFlossColor(floss->color());
				m_documentPalette.add(i, documentFloss);
			}

		}
	}
	else
	{
		return false;	// something went wrong somewhere
	}
	m_documentPalette.setCurrentIndex(-1);

	Stitch::Type stitchType[] = {Stitch::Delete, Stitch::Full, Stitch::TL3Qtr, Stitch::TR3Qtr, Stitch::BL3Qtr, Stitch::BR3Qtr, Stitch::TBHalf, Stitch::BTHalf, Stitch::Delete, Stitch::TLQtr, Stitch::TRQtr, Stitch::BLQtr, Stitch::BRQtr}; // conversion of PCStitch to KXStitch
	int documentWidth = m_stitchData.width();
	int documentHeight = m_stitchData.height();
	int cells = documentWidth * documentHeight;
	qint64 fileSize = stream.device()->size();
	qint16 cellCount;
	quint8 color;
	quint8 type;
	for (int i = 0 ; i < cells ; i+=cellCount)
	{
		if (stream.device()->pos()+4 > fileSize) return false;	// not enough data to read
		stream >> cellCount;
		stream >> color;
		stream >> type;
		kDebug() << i << cellCount << color << type;
		if (type != 0xff)
		{
			for (int c = 0 ; c < cellCount ; c++)
			{
				int xc = (i+c)/documentHeight;
				int yc = (i+c)%documentHeight;
				m_stitchData.addStitch(0, QPoint(xc, yc), stitchType[type], color-1);	// color-1 because PCStitch uses 1 based array
			}
		}
	}
	if (stream.device()->pos()+4 > fileSize) return false;	// not enough data to read
	qint32 extras;
	qint16 x;
	qint16 y;
	stream >> extras;
	while (extras--)
	{
		if (stream.device()->pos()+4 > fileSize) return false;
		stream >> x;
		stream >> y;
		for (int dx = 0 ; dx < 4 ; dx++)
		{
			if (stream.device()->pos()+2 > fileSize) return false;
			stream >> color;
			stream >> type;
			if (type != 0xff)
				m_stitchData.addStitch(0, QPoint(x-1, y-1), stitchType[type], color-1);
		}
	}
	// read french knots
	if (stream.device()->pos()+4 > fileSize) return false;
	qint32 knots;
	stream >> knots;
	while (knots--)
	{
		if (stream.device()->pos()+5 > fileSize) return false;
		stream >> x;
		stream >> y;
		stream >> color;
		m_stitchData.addFrenchKnot(0, QPoint(x-1, y-1), color-1);
	}
	// read backstitches
	if (stream.device()->pos()+4 > fileSize) return false;
	qint32 backstitches;
	stream >> backstitches;
	while (backstitches--)
	{
		if (stream.device()->pos()+13 > fileSize) return false;
		qint16 sx;
		qint16 sy;
		qint16 sp;
		qint16 ex;
		qint16 ey;
		qint16 ep;
		stream >> sx >> sy >> sp >> ex >> ey >> ep >> color;
		m_stitchData.addBackstitch(0, QPoint(--sx*2+((sp-1)%3), --sy*2+((sp-1)/3)), QPoint(--ex*2+((ep-1)%3), --ey*2+((ep-1)/3)), color-1);
	}
	return true;
}


QString Document::readPCStitchString(QDataStream &stream)
{
	char *buffer;
	qint16 stringSize;
	stream >> stringSize;
	buffer = new char[stringSize+1];
	stream.readRawData(buffer, stringSize);
	buffer[stringSize] = '\0';
	QString string(buffer);
	delete [] buffer;
	kDebug() << stringSize << string;
	return string;
}
