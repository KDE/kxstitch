/********************************************************************************
 *	Copyright (C) 2010 by Stephen Allewell					*
 *	stephen@mirramar.adsl24.co.uk						*
 *										*
 *	This program is free software; you can redistribute it and/or modify	*
 *	it under the terms of the GNU General Public License as published by	*
 *	the Free Software Foundation; either version 2 of the License, or	*
 *	(at your option) any later version.					*
 ********************************************************************************/


#include "Document.h"

#include <QDataStream>
#include <QFile>
#include <QVariant>

#include <KDebug>
#include <KIO/NetAccess>
#include <KLocale>
#include <KMessageBox>

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
						case 101:
							stream.setVersion(QDataStream::Qt_4_0);	// maintain consistancy in the qt types
							// flow through to version 100
						case 100:
							stream >> m_properties;
							stream >> m_layers;
							stream >> m_backgroundImages;
							stream >> m_documentPalette;
							stream >> m_stitchData;
							readOk = true;
							break;

						default:
							KMessageBox::sorry(0, i18n("Version %1 of the file format is not supported\nin this version of the software.\nPlease try a more recent version.", version));
							break;
					}
				}
				else if (strncmp(header, "PCStitch 5 Pattern File", 23) == 0)
				{
					readOk = readPCStitch5File(stream);
					foreign = true;
				}
				else if (strncmp(header, "\x00\x00\x00\x10\x00\x4B\x00\x58\x00\x53\x00\x74\x00\x69\x00\x74\x00\x63\x00\x68", 20) == 0) // QString("KXStitch")
				{
					stream.device()->seek(20);
					qint16 version;
					stream >> version;
					kDebug() << "Older format kxstitch file" << "Version" << version;
					switch (version)
					{
						case 2:
							readOk = readKXStitchV2File(stream);
							break;

						case 3:
							readOk = readKXStitchV3File(stream);
							break;

						case 4:
							readOk = readKXStitchV4File(stream);
							break;

						case 5:
							readOk = readKXStitchV5File(stream);
							break;

						case 6:
							readOk = readKXStitchV6File(stream);
							break;

						case 7:
							readOk = readKXStitchV7File(stream);
							break;

						default:
							KMessageBox::sorry(0, i18n("Version %1 of the file format is not supported\nIf it is needed, please inform the developer.", version));
							break;
					}
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
			stream.setVersion(QDataStream::Qt_4_0);	// maintain consistancy in the qt types
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
			KMessageBox::error(0, i18n("The file %1\ncould not be opened.\n%2", m_url.path(), file.errorString()), i18n("Error opening file"));
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
			stream >> unknown;				// color value, probably RGBA, don't need it
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
			        KMessageBox::sorry(0, i18n("Unable to find color %1 in scheme DMC", colorName));
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
	return string;
}


bool Document::readKXStitchV2File(QDataStream &stream)
{
	/** File format
		// header
		#QString("KXStitch")		// read in the load function
		#qint16(FILE_FORMAT_VERSION)	// read in the load function
		QString(title)
		QString(author)
		QString(fabric)
		QColor(fabricColor)	// serialised format has changed over the various versions of qt.
					// for older versions this appears to be quint8 red, quint8 green quint8 blue, quint8 alpha
		qint16(index from flossscheme qcombobox)	// unpredictable, palette stream will contain the scheme name
		qint16(inches)		// boolean, true if the size displayed is in inches
		qint32(width)		// width of pattern (is this in stitches or inches?) // discard in favour of stitchdata width
		qint32(height)		// height of pattern (is this in stitches or inches?) // discard in favour of stitchdata height
		qint32(clothCount)	// this needs to be converted to a double
		QString(instructions)

		// palette
		qint32(current)		// the current color selected
		qint32(colors)		// number of colors in the palette
		QString(schemeName)
			QString(flossName)		]
			QString(flossDescription)	] Repeated for each
			QColor(flossColor)		] entry in the palette, note comment above regarding QColor
			qint16(flossSymbol)		]

		// stitches
		qint32(width)		// width in stitches // use instead of width from the header which might be in inches
		qint32(height)		// height in stitches // use instead of height from the header which might be in inches
			qint8(stitches)					]
				qint8(type)		] Repeated for	] Repeated for
				qint16(colorIndex)	] each stitch	] each stitchqueue

		// backstitches
		qint32(backstitches)	// the number of backstitches
			QPoint(start)		]
			QPoint(end)		] Repeated for
			qint16(colorIndex)	] each backstitch

		// there were no knots supported in this version
		*/

	/** this is currently untested */
	// read header
	QString title;
	QString author;
	QString fabric;
	stream	>> title
		>> author
		>> fabric;
	setProperty("title", title);
	setProperty("author", author);
	setProperty("fabric", fabric);

	quint8 red;
	quint8 green;
	quint8 blue;
	quint8 alpha;
	stream	>> red
		>> green
		>> blue
		>> alpha;
	setProperty("fabricColor", QColor(red, green, blue, alpha));

	qint16 schemeIndex;
	stream	>> schemeIndex;	// discard

	qint16 inches;
	stream	>> inches;
	setProperty("unitsFormat", (bool(inches))?Configuration::EnumDocument_UnitsFormat::Inches:Configuration::EnumDocument_UnitsFormat::Stitches);

	qint32 width;
	qint32 height;
	stream	>> width
		>> height;

	qint32 clothCount;
	stream	>> clothCount;
	setProperty("clothCountUnits", Configuration::EnumEditor_ClothCountUnits::Inches);
	setProperty("horizontalClothCount", double(clothCount));
	setProperty("verticalClothCount", double(clothCount));
	setProperty("clothCountLink", true);

	QString instructions;
	stream	>> instructions;
	setProperty("instructions", instructions);

	// read palette
	qint32 current;
	stream	>> current;

	qint32 colors;
	stream	>> colors;

	QString schemeName;
	stream	>> schemeName;
	m_documentPalette.setSchemeName(schemeName);

	FlossScheme *flossScheme = SchemeManager::scheme(schemeName);
	if (flossScheme == 0) return false;

	int colorIndex = 0;
	while (colors--)
	{
		QString flossName;
		QString flossDescription;
		// QColor color - read as rgba instead
		qint16 symbol;

		stream	>> flossName
			>> flossDescription
			>> red
			>> green
			>> blue
			>> alpha
			>> symbol;

		Floss *floss = flossScheme->find(flossName);
		if (floss == 0) return false;

		DocumentFloss *documentFloss = new DocumentFloss(flossName, QChar(symbol), Qt::SolidLine, Configuration::palette_StitchStrands(), Configuration::palette_BackstitchStrands());
		documentFloss->setFlossColor(floss->color());
		m_documentPalette.add(colorIndex++, documentFloss);
	}


	// read stitches
	stream	>> width
		>> height;
	qint32 cells = width*height;
	m_stitchData.resize(width, height);

	for (int i = 0 ; i < cells ; i++)
	{
		qint8 stitches;
		stream	>> stitches;
		while (stitches--)
		{
			qint8 type;
			qint16 colorIndex;
			stream	>> type
				>> colorIndex;

			m_stitchData.addStitch(0, QPoint(i%width, i/width), Stitch::Type(type), colorIndex);
		}
	}

	qint32 backstitches;
	stream	>> backstitches;
	while (backstitches--)
	{
		QPoint start;
		QPoint end;
		qint16 colorIndex;
		stream	>> start
			>> end
			>> colorIndex;

		m_stitchData.addBackstitch(0, start, end, colorIndex);
	}

	return (stream.status() == QDataStream::Ok);
}


bool Document::readKXStitchV3File(QDataStream &stream)
{
	/** File format
		// header
		#QString("KXStitch")		// read in the load function
		#qint16(FILE_FORMAT_VERSION)	// read in the load function
		QString(title)
		QString(author)
		QString(fabric)
		QColor(fabricColor)	// serialised format has changed over the various versions of qt.
					// for older versions this appears to be quint8 red, quint8 green quint8 blue, quint8 alpha
		qint16(index from flossscheme qcombobox)	// unpredictable, palette stream will contain the scheme name
		qint16(inches)		// boolean, true if the size displayed is in inches
		qint32(width)		// width of pattern (is this in stitches or inches?) // discard in favour of stitchdata width
		qint32(height)		// height of pattern (is this in stitches or inches?) // discard in favour of stitchdata height
		qint32(clothCount)	// this needs to be converted to a double
		QString(instructions)

		// palette
		QString(schemeName)
		qint32(current)		// the current color selected
		qint32(colors)		// number of colors in the palette
			qint32(colorIndex)		]
			QString(flossName)		] Repeated for each
			qint16(flossSymbol)		] entry in the palette

		// stitches
		qint32(width)		// width in stitches // use instead of width from the header which might be in inches
		qint32(height)		// height in stitches // use instead of height from the header which might be in inches
			qint8(stitches)					]
				qint8(type)		] Repeated for	] Repeated for
				qint16(colorIndex)	] each stitch	] each stitchqueue

		// backstitches
		qint32(backstitches)	// the number of backstitches
			QPoint(start)		]
			QPoint(end)		] Repeated for
			qint16(colorIndex)	] each backstitch

		// there were no knots supported in this version
		*/


	/** this is currently untested */
	// read header
	QString title;
	QString author;
	QString fabric;
	stream	>> title
		>> author
		>> fabric;
	setProperty("title", title);
	setProperty("author", author);
	setProperty("fabric", fabric);

	quint8 red;
	quint8 green;
	quint8 blue;
	quint8 alpha;
	stream	>> red
		>> green
		>> blue
		>> alpha;
	setProperty("fabricColor", QColor(red, green, blue, alpha));

	qint16 schemeIndex;
	stream	>> schemeIndex;	// discard

	qint16 inches;
	stream	>> inches;
	setProperty("unitsFormat", (bool(inches))?Configuration::EnumDocument_UnitsFormat::Inches:Configuration::EnumDocument_UnitsFormat::Stitches);

	qint32 width;
	qint32 height;
	stream	>> width
		>> height;

	qint32 clothCount;
	stream	>> clothCount;
	setProperty("clothCountUnits", Configuration::EnumEditor_ClothCountUnits::Inches);
	setProperty("horizontalClothCount", double(clothCount));
	setProperty("verticalClothCount", double(clothCount));
	setProperty("clothCountLink", true);

	QString instructions;
	stream	>> instructions;
	setProperty("instructions", instructions);

	// read palette
	QString schemeName;
	stream	>> schemeName;
	m_documentPalette.setSchemeName(schemeName);

	FlossScheme *flossScheme = SchemeManager::scheme(schemeName);
	if (flossScheme == 0) return false;

	qint32 current;
	stream	>> current;

	qint32 colors;
	stream	>> colors;

	while (colors--)
	{
		qint32 colorIndex;
		QString flossName;
		qint16 symbol;

		stream	>> colorIndex
			>> flossName
			>> symbol;

		Floss *floss = flossScheme->find(flossName);
		if (floss == 0) return false;

		DocumentFloss *documentFloss = new DocumentFloss(flossName, QChar(symbol), Qt::SolidLine, Configuration::palette_StitchStrands(), Configuration::palette_BackstitchStrands());
		documentFloss->setFlossColor(floss->color());
		m_documentPalette.add(colorIndex, documentFloss);
	}


	// read stitches
	stream	>> width
		>> height;
	qint32 cells = width*height;
	m_stitchData.resize(width, height);

	for (int i = 0 ; i < cells ; i++)
	{
		qint8 stitches;
		stream	>> stitches;
		while (stitches--)
		{
			qint8 type;
			qint16 colorIndex;
			stream	>> type
				>> colorIndex;

			m_stitchData.addStitch(0, QPoint(i%width, i/width), Stitch::Type(type), colorIndex);
		}
	}

	qint32 backstitches;
	stream	>> backstitches;
	while (backstitches--)
	{
		QPoint start;
		QPoint end;
		qint16 colorIndex;
		stream	>> start
			>> end
			>> colorIndex;

		m_stitchData.addBackstitch(0, start, end, colorIndex);
	}

	return (stream.status() == QDataStream::Ok);
}


bool Document::readKXStitchV4File(QDataStream &stream)
{
	// version 4 wasn't used in the release versions.
	// but was available in cvs
	/** File format
		// header
		#QString("KXStitch")		// read in the load function
		#qint16(FILE_FORMAT_VERSION)	// read in the load function
		QString(title)
		QString(author)
		QString(fabric)
		QColor(fabricColor)	// serialised format has changed over the various versions of qt.
					// for older versions this appears to be quint8 red, quint8 green quint8 blue, quint8 alpha
		qint16(index from flossscheme qcombobox)	// unpredictable, palette stream will contain the scheme name
		qint16(inches)		// boolean, true if the size displayed is in inches
		qint32(width)		// width of pattern (is this in stitches or inches?) // discard in favour of stitchdata width
		qint32(height)		// height of pattern (is this in stitches or inches?) // discard in favour of stitchdata height
		qint32(clothCount)	// this needs to be converted to a double
		QString(instructions)

		// palette
		QString(schemeName)
		qint32(current)		// the current color selected
		qint32(colors)		// number of colors in the palette
			qint32(colorIndex)		]
			QString(flossName)		] Repeated for each
			qint16(flossSymbol)		] entry in the palette

		// stitches
		qint32(width)		// width in stitches // use instead of width from the header which might be in inches
		qint32(height)		// height in stitches // use instead of height from the header which might be in inches
			qint8(stitches)					]
				qint8(type)		] Repeated for	] Repeated for
				qint16(colorIndex)	] each stitch	] each stitchqueue

		// knots
		qint32(knots)		// the number of knots
			QPoint(position)	] Repeated for
			qint16(colorIndex)	] each knot

		// backstitches
		qint32(backstitches)	// the number of backstitches
			QPoint(start)		]
			QPoint(end)		] Repeated for
			qint16(colorIndex)	] each backstitch
		*/


	/** this is currently untested */
	// read header
	QString title;
	QString author;
	QString fabric;
	stream	>> title
		>> author
		>> fabric;
	setProperty("title", title);
	setProperty("author", author);
	setProperty("fabric", fabric);

	quint8 red;
	quint8 green;
	quint8 blue;
	quint8 alpha;
	stream	>> red
		>> green
		>> blue
		>> alpha;
	setProperty("fabricColor", QColor(red, green, blue, alpha));

	qint16 schemeIndex;
	stream	>> schemeIndex;	// discard

	qint16 inches;
	stream	>> inches;
	setProperty("unitsFormat", (bool(inches))?Configuration::EnumDocument_UnitsFormat::Inches:Configuration::EnumDocument_UnitsFormat::Stitches);

	qint32 width;
	qint32 height;
	stream	>> width
		>> height;

	qint32 clothCount;
	stream	>> clothCount;
	setProperty("clothCountUnits", Configuration::EnumEditor_ClothCountUnits::Inches);
	setProperty("horizontalClothCount", double(clothCount));
	setProperty("verticalClothCount", double(clothCount));
	setProperty("clothCountLink", true);

	QString instructions;
	stream	>> instructions;
	setProperty("instructions", instructions);

	// read palette
	QString schemeName;
	stream	>> schemeName;
	m_documentPalette.setSchemeName(schemeName);

	FlossScheme *flossScheme = SchemeManager::scheme(schemeName);
	if (flossScheme == 0) return false;

	qint32 current;
	stream	>> current;

	qint32 colors;
	stream	>> colors;

	while (colors--)
	{
		qint32 colorIndex;
		QString flossName;
		qint16 symbol;

		stream	>> colorIndex
			>> flossName
			>> symbol;

		Floss *floss = flossScheme->find(flossName);
		if (floss == 0) return false;

		DocumentFloss *documentFloss = new DocumentFloss(flossName, QChar(symbol), Qt::SolidLine, Configuration::palette_StitchStrands(), Configuration::palette_BackstitchStrands());
		documentFloss->setFlossColor(floss->color());
		m_documentPalette.add(colorIndex, documentFloss);
	}


	// read stitches
	stream	>> width
		>> height;
	qint32 cells = width*height;
	m_stitchData.resize(width, height);

	for (int i = 0 ; i < cells ; i++)
	{
		qint8 stitches;
		stream	>> stitches;
		while (stitches--)
		{
			qint8 type;
			qint16 colorIndex;
			stream	>> type
				>> colorIndex;

			m_stitchData.addStitch(0, QPoint(i%width, i/width), Stitch::Type(type), colorIndex);
		}
	}

	qint32 knots;
	stream	>> knots;
	while (knots--)
	{
		QPoint position;
		qint16 colorIndex;
		stream	>> position
			>> colorIndex;

		m_stitchData.addFrenchKnot(0, position, colorIndex);
	}

	qint32 backstitches;
	stream	>> backstitches;
	while (backstitches--)
	{
		QPoint start;
		QPoint end;
		qint16 colorIndex;
		stream	>> start
			>> end
			>> colorIndex;

		m_stitchData.addBackstitch(0, start, end, colorIndex);
	}

	return (stream.status() == QDataStream::Ok);
}


bool Document::readKXStitchV5File(QDataStream &stream)
{
	/** File format
		// header
		#QString("KXStitch")		// read in the load function
		#qint16(FILE_FORMAT_VERSION)	// read in the load function

		// properties dialog
		QString(sizeUnits)
		qint32(width)		// this is in stitches
		qint32(height)		// this is in stitches
		double(clothCount)
		QString(clothCountUnits)
		QString(title)
		QString(author)
		QString(copyright)
		QString(fabric)
		QColor(fabricColor)	// serialised format has changed over the various versions of qt.
					// for older versions this appears to be quint8 red, quint8 green quint8 blue, quint8 alpha
		QString(schemeName)
		QString(instructions)

		// palette
		QString(schemeName)
		qint32(current)		// the current color selected
		qint32(colors)		// number of colors in the palette
			qint32(colorIndex)		]
			QString(flossName)		] Repeated for each
			qint16(flossSymbol)		] entry in the palette

		// stitches
		qint32(width)		// width in stitches // use instead of width from the header which might be in inches
		qint32(height)		// height in stitches // use instead of height from the header which might be in inches
			qint8(stitches)					]
				qint8(type)		] Repeated for	] Repeated for
				qint16(colorIndex)	] each stitch	] each stitchqueue

		// knots
		qint32(knots)		// the number of knots
			QPoint(position)	] Repeated for
			qint16(colorIndex)	] each knot

		// backstitches
		qint32(backstitches)	// the number of backstitches
			QPoint(start)		]
			QPoint(end)		] Repeated for
			qint16(colorIndex)	] each backstitch
		*/


	/** this is currently untested */
	// read header
	QString sizeUnits;
	qint32 width;
	qint32 height;
	stream	>> sizeUnits
		>> width
		>> height;
	if (sizeUnits == i18n("Stitches")) setProperty("unitsFormat", Configuration::EnumDocument_UnitsFormat::Stitches);
	if (sizeUnits == i18n("CM")) setProperty("unitsFormat", Configuration::EnumDocument_UnitsFormat::CM);
	if (sizeUnits == i18n("Inches")) setProperty("unitsFormat", Configuration::EnumDocument_UnitsFormat::Inches);

	double clothCount;
	QString clothCountUnits;
	stream	>> clothCount
		>> clothCountUnits;
	setProperty("horizontalClothCount", double(clothCount));
	setProperty("verticalClothCount", double(clothCount));
	setProperty("clothCountLink", true);
	if (clothCountUnits == "/cm") setProperty("clothCountUnits", Configuration::EnumEditor_ClothCountUnits::CM);
	if (clothCountUnits == "/in") setProperty("clothCountUnits", Configuration::EnumEditor_ClothCountUnits::Inches);

	QString title;
	QString author;
	QString copyright;
	QString fabric;
	stream	>> title
		>> author
		>> copyright
		>> fabric;
	setProperty("title", title);
	setProperty("author", author);
	setProperty("copyright", copyright);
	setProperty("fabric", fabric);

	quint8 red;
	quint8 green;
	quint8 blue;
	quint8 alpha;
	stream	>> red
		>> green
		>> blue
		>> alpha;
	setProperty("fabricColor", QColor(red, green, blue, alpha));

	QString schemeName;
	stream	>> schemeName;	// discard, also read in palette

	QString instructions;
	stream	>> instructions;
	setProperty("instructions", instructions);

	// read palette
	stream	>> schemeName;
	m_documentPalette.setSchemeName(schemeName);

	FlossScheme *flossScheme = SchemeManager::scheme(schemeName);
	if (flossScheme == 0) return false;

	qint32 current;
	stream	>> current;

	qint32 colors;
	stream	>> colors;

	while (colors--)
	{
		qint32 colorIndex;
		QString flossName;
		qint16 symbol;

		stream	>> colorIndex
			>> flossName
			>> symbol;

		Floss *floss = flossScheme->find(flossName);
		if (floss == 0) return false;

		DocumentFloss *documentFloss = new DocumentFloss(flossName, QChar(symbol), Qt::SolidLine, Configuration::palette_StitchStrands(), Configuration::palette_BackstitchStrands());
		documentFloss->setFlossColor(floss->color());
		m_documentPalette.add(colorIndex, documentFloss);
	}

	// read stitches
	stream	>> width
		>> height;
	qint32 cells = width*height;
	m_stitchData.resize(width, height);

	for (int i = 0 ; i < cells ; i++)
	{
		qint8 stitches;
		stream	>> stitches;
		while (stitches--)
		{
			qint8 type;
			qint16 colorIndex;
			stream	>> type
				>> colorIndex;

			m_stitchData.addStitch(0, QPoint(i%width, i/width), Stitch::Type(type), colorIndex);
		}
	}

	qint32 knots;
	stream	>> knots;
	while (knots--)
	{
		QPoint position;
		qint16 colorIndex;
		stream	>> position
			>> colorIndex;

		m_stitchData.addFrenchKnot(0, position, colorIndex);
	}

	qint32 backstitches;
	stream	>> backstitches;
	while (backstitches--)
	{
		QPoint start;
		QPoint end;
		qint16 colorIndex;
		stream	>> start
			>> end
			>> colorIndex;

		m_stitchData.addBackstitch(0, start, end, colorIndex);
	}

	return (stream.status() == QDataStream::Ok);
}


bool Document::readKXStitchV6File(QDataStream &stream)
{
	/** File format
		// header
		#QString("KXStitch")		// read in the load function
		#qint16(FILE_FORMAT_VERSION)	// read in the load function

		// properties dialog
		QString(sizeUnits)
		qint32(width)		// this is in stitches
		qint32(height)		// this is in stitches
		double(clothCount)
		QString(clothCountUnits)
		QString(title)
		QString(author)
		QString(copyright)
		QString(fabric)
		QColor(fabricColor)	// serialised format has changed over the various versions of qt.
					// for older versions this appears to be quint8 red, quint8 green quint8 blue, quint8 alpha
		QString(schemeName)
		QString(instructions)

		// palette
		QString(schemeName)
		qint32(current)		// the current color selected
		qint32(colors)		// number of colors in the palette
			qint32(colorIndex)		]
			QString(flossName)		] Repeated for each
			qint16(flossSymbol)		] entry in the palette
			qint8(stitchStrands)		]
			qint8(backstitchStrands)	]

		// stitches
		qint32(width)		// width in stitches // use instead of width from the header which might be in inches
		qint32(height)		// height in stitches // use instead of height from the header which might be in inches
			qint8(stitches)					]
				qint8(type)		] Repeated for	] Repeated for
				qint16(colorIndex)	] each stitch	] each stitchqueue

		// knots
		qint32(knots)		// the number of knots
			QPoint(position)	] Repeated for
			qint16(colorIndex)	] each knot

		// backstitches
		qint32(backstitches)	// the number of backstitches
			QPoint(start)		]
			QPoint(end)		] Repeated for
			qint16(colorIndex)	] each backstitch
		*/

	/** Working */
	// read header
	QString sizeUnits;
	qint32 width;
	qint32 height;
	stream	>> sizeUnits
		>> width
		>> height;
	if (sizeUnits == i18n("Stitches")) setProperty("unitsFormat", Configuration::EnumDocument_UnitsFormat::Stitches);
	if (sizeUnits == i18n("CM")) setProperty("unitsFormat", Configuration::EnumDocument_UnitsFormat::CM);
	if (sizeUnits == i18n("Inches")) setProperty("unitsFormat", Configuration::EnumDocument_UnitsFormat::Inches);

	double clothCount;
	QString clothCountUnits;
	stream	>> clothCount
		>> clothCountUnits;
	setProperty("horizontalClothCount", double(clothCount));
	setProperty("verticalClothCount", double(clothCount));
	setProperty("clothCountLink", true);
	if (clothCountUnits == "/cm") setProperty("clothCountUnits", Configuration::EnumEditor_ClothCountUnits::CM);
	if (clothCountUnits == "/in") setProperty("clothCountUnits", Configuration::EnumEditor_ClothCountUnits::Inches);

	QString title;
	QString author;
	QString copyright;
	QString fabric;
	stream	>> title
		>> author
		>> copyright
		>> fabric;
	setProperty("title", title);
	setProperty("author", author);
	setProperty("copyright", copyright);
	setProperty("fabric", fabric);

	quint8 red;
	quint8 green;
	quint8 blue;
	quint8 alpha;
	stream	>> red
		>> green
		>> blue
		>> alpha;
	setProperty("fabricColor", QColor(red, green, blue, alpha));

	QString schemeName;
	stream	>> schemeName;	// discard, also read in palette

	QString instructions;
	stream	>> instructions;
	setProperty("instructions", instructions);

	// read palette
	stream	>> schemeName;
	m_documentPalette.setSchemeName(schemeName);

	FlossScheme *flossScheme = SchemeManager::scheme(schemeName);
	if (flossScheme == 0) return false;

	qint32 current;
	stream	>> current;

	qint32 colors;
	stream	>> colors;

	while (colors--)
	{
		qint32 colorIndex;
		QString flossName;
		qint16 symbol;
		qint8 stitchStrands;
		qint8 backstitchStrands;

		stream	>> colorIndex
			>> flossName
			>> symbol
			>> stitchStrands
			>> backstitchStrands;

		Floss *floss = flossScheme->find(flossName);
		if (floss == 0) return false;

		DocumentFloss *documentFloss = new DocumentFloss(flossName, QChar(symbol), Qt::SolidLine, stitchStrands, backstitchStrands);
		documentFloss->setFlossColor(floss->color());
		m_documentPalette.add(colorIndex, documentFloss);
	}

	// read stitches
	stream	>> width
		>> height;
	qint32 cells = width*height;
	m_stitchData.resize(width, height);

	for (int i = 0 ; i < cells ; i++)
	{
		qint8 stitches;
		stream	>> stitches;
		while (stitches--)
		{
			qint8 type;
			qint16 colorIndex;
			stream	>> type
				>> colorIndex;

			m_stitchData.addStitch(0, QPoint(i%width, i/width), Stitch::Type(type), colorIndex);
		}
	}

	qint32 knots;
	stream	>> knots;
	while (knots--)
	{
		QPoint position;
		qint16 colorIndex;
		stream	>> position
			>> colorIndex;

		m_stitchData.addFrenchKnot(0, position, colorIndex);
	}

	qint32 backstitches;
	stream	>> backstitches;
	while (backstitches--)
	{
		QPoint start;
		QPoint end;
		qint16 colorIndex;
		stream	>> start
			>> end
			>> colorIndex;

		m_stitchData.addBackstitch(0, start, end, colorIndex);
	}

	return (stream.status() == QDataStream::Ok);
}


bool Document::readKXStitchV7File(QDataStream &stream)
{
	/** File format
		// header
		#QString("KXStitch")		// read in the load function
		#qint16(FILE_FORMAT_VERSION)	// read in the load function

		// properties dialog
		qint32(sizeUnits)
		qint32(width)		// this is in stitches
		qint32(height)		// this is in stitches
		double(clothCount)
		qint32(clothCountUnits)
		QString(title)
		QString(author)
		QString(copyright)
		QString(fabric)
		QColor(fabricColor)	// serialised format has changed over the various versions of qt.
					// for older versions this appears to be quint8 red, quint8 green quint8 blue, quint8 alpha
		QString(schemeName)
		QString(instructions)

		// palette
		QString(schemeName)
		qint32(current)		// the current color selected
		qint32(colors)		// number of colors in the palette
			qint32(colorIndex)		]
			QString(flossName)		] Repeated for each
			qint16(flossSymbol)		] entry in the palette
			qint8(stitchStrands)		]
			qint8(backstitchStrands)	]

		// stitches
		qint32(width)		// width in stitches // use instead of width from the header which might be in inches
		qint32(height)		// height in stitches // use instead of height from the header which might be in inches
			qint8(stitches)					]
				qint8(type)		] Repeated for	] Repeated for
				qint16(colorIndex)	] each stitch	] each stitchqueue

		// knots
		qint32(knots)		// the number of knots
			QPoint(position)	] Repeated for
			qint16(colorIndex)	] each knot

		// backstitches
		qint32(backstitches)	// the number of backstitches
			QPoint(start)		]
			QPoint(end)		] Repeated for
			qint16(colorIndex)	] each backstitch
		*/

	/** Working */
	Configuration::EnumDocument_UnitsFormat::type convertSizeUnits[] =
	{
		Configuration::EnumDocument_UnitsFormat::Stitches,
		Configuration::EnumDocument_UnitsFormat::CM,		// was MM
		Configuration::EnumDocument_UnitsFormat::CM,
		Configuration::EnumDocument_UnitsFormat::Inches
	};
	Configuration::EnumEditor_ClothCountUnits::type convertClothCountUnits[] =
	{
		Configuration::EnumEditor_ClothCountUnits::Inches,	// was Stitches
		Configuration::EnumEditor_ClothCountUnits::CM,
		Configuration::EnumEditor_ClothCountUnits::Inches
	};
	// read header
	qint32 sizeUnits;
	qint32 width;
	qint32 height;
	stream	>> sizeUnits
		>> width
		>> height;
	setProperty("unitsFormat", convertSizeUnits[sizeUnits]);

	double clothCount;
	qint32 clothCountUnits;
	stream	>> clothCount
		>> clothCountUnits;
	setProperty("horizontalClothCount", double(clothCount));
	setProperty("verticalClothCount", double(clothCount));
	setProperty("clothCountLink", true);
	setProperty("clothCountUnits", convertClothCountUnits[clothCountUnits]);

	QString title;
	QString author;
	QString copyright;
	QString fabric;
	stream	>> title
		>> author
		>> copyright
		>> fabric;
	setProperty("title", title);
	setProperty("author", author);
	setProperty("copyright", copyright);
	setProperty("fabric", fabric);

	quint8 red;
	quint8 green;
	quint8 blue;
	quint8 alpha;
	stream	>> red
		>> green
		>> blue
		>> alpha;
	setProperty("fabricColor", QColor(red, green, blue, alpha));

	QString schemeName;
	stream	>> schemeName;	// discard, also read in palette

	QString instructions;
	stream	>> instructions;
	setProperty("instructions", instructions);

	// read palette
	stream	>> schemeName;
	m_documentPalette.setSchemeName(schemeName);

	FlossScheme *flossScheme = SchemeManager::scheme(schemeName);
	if (flossScheme == 0) return false;

	qint32 current;
	stream	>> current;

	qint32 colors;
	stream	>> colors;

	while (colors--)
	{
		qint32 colorIndex;
		QString flossName;
		qint16 symbol;
		qint8 stitchStrands;
		qint8 backstitchStrands;

		stream	>> colorIndex
			>> flossName
			>> symbol
			>> stitchStrands
			>> backstitchStrands;

		Floss *floss = flossScheme->find(flossName);
		if (floss == 0) return false;

		DocumentFloss *documentFloss = new DocumentFloss(flossName, QChar(symbol), Qt::SolidLine, stitchStrands, backstitchStrands);
		documentFloss->setFlossColor(floss->color());
		m_documentPalette.add(colorIndex, documentFloss);
	}

	// read stitches
	stream	>> width
		>> height;
	qint32 cells = width*height;
	m_stitchData.resize(width, height);

	for (int i = 0 ; i < cells ; i++)
	{
		qint8 stitches;
		stream	>> stitches;
		while (stitches--)
		{
			qint8 type;
			qint16 colorIndex;
			stream	>> type
				>> colorIndex;

			m_stitchData.addStitch(0, QPoint(i%width, i/width), Stitch::Type(type), colorIndex);
		}
	}

	qint32 knots;
	stream	>> knots;
	while (knots--)
	{
		QPoint position;
		qint16 colorIndex;
		stream	>> position
			>> colorIndex;

		m_stitchData.addFrenchKnot(0, position, colorIndex);
	}

	qint32 backstitches;
	stream	>> backstitches;
	while (backstitches--)
	{
		QPoint start;
		QPoint end;
		qint16 colorIndex;
		stream	>> start
			>> end
			>> colorIndex;

		m_stitchData.addBackstitch(0, start, end, colorIndex);
	}

	return (stream.status() == QDataStream::Ok);
}
