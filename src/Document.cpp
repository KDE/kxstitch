/*
 * Copyright (C) 2010-2015 by Stephen Allewell
 * steve.allewell@gmail.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#include "Document.h"

#include <QDataStream>
#include <QFile>
#include <QVariant>

#include <KLocalizedString>
#include <KMessageBox>

#include "Editor.h"
#include "Exceptions.h"
#include "Floss.h"
#include "FlossScheme.h"
#include "Layers.h"
#include "Palette.h"
#include "Preview.h"
#include "SchemeManager.h"


Document::Document()
    :   m_editor(nullptr),
        m_palette(nullptr),
        m_preview(nullptr),
        m_pattern(nullptr)
{
    initialiseNew();
}


Document::~Document()
{
    delete m_pattern;
}


void Document::initialiseNew()
{
    m_undoStack.clear();

    m_backgroundImages.clear();
    m_printerConfiguration = PrinterConfiguration();

    delete m_pattern;
    m_pattern = new Pattern(this);

    QString scheme = Configuration::palette_DefaultScheme();

    if (SchemeManager::scheme(scheme) == nullptr) {
        scheme = SchemeManager::schemes().at(scheme.toInt());
    }

    m_pattern->palette().setSchemeName(scheme);

    double documentWidth = Configuration::document_Width();
    double documentHeight = Configuration::document_Height();

    double horizontalClothCount = Configuration::editor_HorizontalClothCount();
    double verticalClothCount = Configuration::editor_VerticalClothCount();
    int clothCountUnitsIndex = Configuration::editor_ClothCountUnits();

    switch (Configuration::document_UnitsFormat()) {
    case 0: // Stitches
        // nothing needs to be done to convert these values
        break;

    case 1: // Inches
        documentWidth *= horizontalClothCount;
        documentHeight *= verticalClothCount;

        if (clothCountUnitsIndex == 1) { // CM
            documentWidth *= 2.54;
            documentHeight *= 2.54;
        }

        break;

    case 2: // CM
        documentWidth *= horizontalClothCount;
        documentHeight *= verticalClothCount;

        if (clothCountUnitsIndex == 0) { // Inches
            documentWidth /= 2.54;
            documentHeight /= 2.54;
        }

        break;

    default: // Avoid compiler warning about unhandled value
        break;
    }

    m_pattern->stitches().resize(static_cast<int>(documentWidth), static_cast<int>(documentHeight));

    setProperty(QStringLiteral("unitsFormat"), Configuration::document_UnitsFormat());
    setProperty(QStringLiteral("title"), QString());
    setProperty(QStringLiteral("author"), QString());
    setProperty(QStringLiteral("copyright"), QString());
    setProperty(QStringLiteral("fabric"), QString());
    setProperty(QStringLiteral("fabricColor"), Configuration::editor_BackgroundColor());
    setProperty(QStringLiteral("instructions"), QString());
    setProperty(QStringLiteral("cellHorizontalGrouping"), Configuration::editor_CellHorizontalGrouping());
    setProperty(QStringLiteral("cellVerticalGrouping"), Configuration::editor_CellVerticalGrouping());
    setProperty(QStringLiteral("horizontalClothCount"), Configuration::editor_HorizontalClothCount());
    setProperty(QStringLiteral("verticalClothCount"), Configuration::editor_VerticalClothCount());
    setProperty(QStringLiteral("clothCountUnits"), Configuration::editor_ClothCountUnits());
    setProperty(QStringLiteral("clothCountLink"), Configuration::editor_ClothCountLink());
    setProperty(QStringLiteral("thickLineColor"), Configuration::editor_ThickLineColor());
    setProperty(QStringLiteral("thinLineColor"), Configuration::editor_ThinLineColor());

    setUrl(QUrl(i18n("Untitled")));
}


QUndoStack &Document::undoStack()
{
    return m_undoStack;
}


void Document::setUrl(const QUrl &url)
{
    m_url = url;
}


QUrl Document::url() const
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


Editor *Document::editor() const
{
    return m_editor;
}


Palette *Document::palette() const
{
    return m_palette;
}


Preview *Document::preview() const
{
    return m_preview;
}


BackgroundImages &Document::backgroundImages()
{
    return m_backgroundImages;
}


Pattern *Document::pattern()
{
    return m_pattern;
}


const PrinterConfiguration &Document::printerConfiguration() const
{
    return m_printerConfiguration;
}


void Document::setPrinterConfiguration(const PrinterConfiguration &printerConfiguration)
{
    m_printerConfiguration = printerConfiguration;
}


void Document::readKXStitch(QDataStream &stream)
{
    initialiseNew();

    char header[30];
    stream.readRawData(header, 30);

    if (strncmp(header, "KXStitchDoc", 11) == 0) {
        // a current KXStitchDoc format file
        stream.device()->seek(11);
        Layers layers;
        qint32 version;
        stream >> version;

        switch (version) {
        case 104:
            stream.setVersion(QDataStream::Qt_4_0); // maintain consistancy in the qt types
            stream >> m_properties;
            stream >> m_backgroundImages;
            stream >> *m_pattern;
            stream >> m_printerConfiguration;
            break;

        case 103:
            stream.setVersion(QDataStream::Qt_4_0); // maintain consistancy in the qt types
            stream >> m_properties;
            stream >> m_backgroundImages;
            stream >> m_pattern->palette();
            stream >> m_pattern->stitches();
            stream >> m_printerConfiguration;
            break;

        case 102:
            stream.setVersion(QDataStream::Qt_4_0); // maintain consistancy in the qt types
            stream >> m_properties;
            stream >> layers;
            stream >> m_backgroundImages;
            stream >> m_pattern->palette();
            stream >> m_pattern->stitches();
            stream >> m_printerConfiguration;
            break;

        case 101:
            stream.setVersion(QDataStream::Qt_4_0); // maintain consistancy in the qt types

            // flow through to version 100
        case 100:
            stream >> m_properties;
            stream >> layers;
            stream >> m_backgroundImages;
            stream >> m_pattern->palette();
            stream >> m_pattern->stitches();
            break;

        default:
            throw InvalidFileVersion(QString(i18n("KXStitch version %1", version)));
            break;
        }
    } else if (strncmp(header, "\x00\x00\x00\x10\x00\x4B\x00\x58\x00\x53\x00\x74\x00\x69\x00\x74\x00\x63\x00\x68", 20) == 0) { // QString("KXStitch")
        stream.device()->seek(20);
        qint16 version;
        stream >> version;

        switch (version) {
        case 2:
            readKXStitchV2File(stream);
            break;

        case 3:
            readKXStitchV3File(stream);
            break;

        case 4:
            readKXStitchV4File(stream);
            break;

        case 5:
            readKXStitchV5File(stream);
            break;

        case 6:
            readKXStitchV6File(stream);
            break;

        case 7:
            readKXStitchV7File(stream);
            break;

        default:
            throw InvalidFileVersion(QString(i18n("KXStitch version %1", version)));
            break;
        }
    } else {
        throw InvalidFile();
    }
}


void Document::readPCStitch(QDataStream &stream)
{
    initialiseNew();

    char header[23];
    stream.readRawData(header, 23);

    if (strncmp(header, "PCStitch 5 Pattern File", 23) == 0) {
        readPCStitch5File(stream);
    } else if (strncmp(header, "PCStitch 6 Pattern File", 23) == 0) {
        readPCStitch6File(stream);
    } else if (strncmp(header, "PCStitch 7 Pattern File", 23) == 0) {
        readPCStitch7File(stream);
    } else {
        throw InvalidFile();
    }
}


void Document::write(QDataStream &stream)
{
    stream.setVersion(QDataStream::Qt_4_0); // maintain consistancy in the qt types
    stream.writeRawData("KXStitchDoc", 11);
    stream << version;
    stream << m_properties;
    stream << m_backgroundImages;
    stream << *m_pattern;
    stream << m_printerConfiguration;

    if (stream.status() != QDataStream::Ok) {
        throw FailedWriteFile(stream.status());
    }
}


QVariant Document::property(const QString &name) const
{
    QVariant p;

    if (m_properties.contains(name)) {
        p = m_properties[name];
    } else {
        qDebug() << "Asked for non existing property" << name;
    }

    return p;
}


void Document::setProperty(const QString &name, const QVariant &value)
{
    m_properties[name] = value;
}


void Document::readPCStitch5File(QDataStream &stream)
{
    /* File Format
        uchar[256]      // header 'PCStitch 5 Pattern File'
                        // other information is contained here
        uchar[4]        // unknown
        uchar[4]        // unknown, there is something unique to each file here
        quint16         // pattern width;
        quint16         // pattern height;
        quint16         // cloth count width;
        quint16         // cloth count height;
        quint16         // string length
        char[]          // author
        quint16         // string length
        char[]          // copyright
        quint16         // string length
        char[]          // title
        quint16         // string length
        char[]          // fabric
        quint16         // string length
        char[]          // instructions
        char[25]        // 'PCStitch 5 Floss Palette!'
        quint16         // palette entries
        char[10]        // 'DMC       '
        char[10]        // 'Anchor    '
        char[10]        // 'Coates    '
        quint16         // string length
        char[]          // symbol font
        uchar[4]        // unknown

        struct paletteEntry {
            uchar[4]    // RGBA
            char[10];   // dmc color name
            char[10];   // anchor color name
            char[10];   // coates color name
            char[50];   // color description
            char        // symbol
            quint16     // number of strands for stitches
            quint16     // number of strands for backstitches
        } repeated for the number of palette entries

        struct stitches {
            quint16     // stitch count in columns from top left downward
            quint8      // color, 1 based index of color list, 0xFF for none
            quint8      // stitch type, 0xFF for none
        } repeat until sum of stitchCount == width * height;

        quint32 extras
        struct extra {
            quint16     // x coordinate, 1 based index of cell
            quint16     // y coordinate, 1 based index of cell
            struct quadrant {
                quint8  // color, 1 based index of color list, 0xFF for none
                quint8  // stitch type, 0xFF for none
            } repeated 4 times
        } repeated for extras

        quint32 knots
        struct knot {
            quint16     // x coordinate, 1 based index of snap points
            quint16     // y coordinate, 1 based index of snap points
            quint8      // color, 1 based index of color list
        } repeated for knots

        quint32 backstitches
        struct backstitch {
            quint16     // start x, 1 based index of cell
            quint16     // start y, 1 based index of cell
            quint16     // start position, (1-9) based on snap points
            quint16     // end x, 1 based index of cell
            quint16     // end y, 1 based index of cell
            quint16     // end position, (1-9) based on snap points
            quint8      // color, 1 based index of color list
        } repeat for backstitches

        uchar[4]        // unknown;
        uchar[4]        // unknown;
    */
    stream.setByteOrder(QDataStream::LittleEndian);
    stream.device()->seek(256);

    quint32 unknown32;
    stream >> unknown32;
    stream >> unknown32;

    quint16 width;
    quint16 height;
    stream >> width;
    stream >> height;

    m_pattern->stitches().resize(width, height);
    setProperty(QStringLiteral("unitsFormat"), Configuration::EnumDocument_UnitsFormat::Stitches);

    quint16 clothCount;
    stream >> clothCount;
    setProperty(QStringLiteral("horizontalClothCount"), double(clothCount));
    stream >> clothCount;
    setProperty(QStringLiteral("verticalClothCount"), double(clothCount));
    setProperty(QStringLiteral("clothCountUnits"), Configuration::EnumEditor_ClothCountUnits::Inches);

    setProperty(QStringLiteral("author"), readPCStitchString(stream));
    setProperty(QStringLiteral("copyright"), readPCStitchString(stream));
    setProperty(QStringLiteral("title"), readPCStitchString(stream));
    setProperty(QStringLiteral("fabric"), readPCStitchString(stream));
    setProperty(QStringLiteral("fabricColor"), QColor(Qt::white));

    QString instructions = readPCStitchString(stream);

    if (!instructions.isEmpty()) {
        int index = instructions.indexOf(QLatin1String("}}"));      // end of font defs
        index += 4;                                 // skip paste }} and CR LF
        index = instructions.indexOf(QLatin1Char('}'), index);   // end of color table defs
        index += 3;                                 // skip paste } and CR LF
        index = instructions.indexOf(QLatin1Char(' '), index);   // find first space - end of text def
        index++;                                    // and skip past it
                                                    // index should now point to the first character of the instructions
        instructions = instructions.remove(0, index);
        instructions.truncate(instructions.length() - 10);
    }

    setProperty(QStringLiteral("instructions"), instructions);

    char buffer[51];
    stream.readRawData(buffer, 25);

    if (strncmp(buffer, "PCStitch 5 Floss Palette!", 25) == 0) {
        quint16 colors;
        stream >> colors;

        stream.readRawData(buffer, 30);                 // this should be 'DMC       Anchor    Coates    '

        QString fontName = readPCStitchString(stream);  // the font name, usually 'PCStitch Symbols'

        m_pattern->palette().setSchemeName(QStringLiteral("DMC"));      // assume this palette will be DMC
        FlossScheme *scheme = SchemeManager::scheme(QStringLiteral("DMC"));

        if (scheme == nullptr) {
            throw FailedReadFile(QString(i18n("The floss scheme DMC was not found")));    // this shouldn't happen because DMC should always be available
        }

        stream >> unknown32;

        for (int i = 0 ; i < colors ; i++) {
#pragma pack(push)
#pragma pack(1)
            struct PALETTE_ENTRY {
                unsigned char   RGBA[4];
                char            colorName[30];
                char            colorDescription[50];
                unsigned char   symbol;
                unsigned short  stitchStrands;
                unsigned short  backstitchStrands;
            } paletteEntry;
#pragma pack(pop)

            stream.readRawData(reinterpret_cast<char *>(&paletteEntry), sizeof(struct PALETTE_ENTRY));

            QColor color(int(paletteEntry.RGBA[0]), int(paletteEntry.RGBA[1]), int(paletteEntry.RGBA[2]));
            QString colorName = QString::fromLatin1(paletteEntry.colorName, 10).trimmed();

            if (colorName == QLatin1String("White")) {
                colorName = QLatin1String("Blanc");                        // fix colorName
            }

            if (colorName == QLatin1String("5200")) {
                colorName = QLatin1String("B5200");                        // fix colorName
            }

            Floss *floss = scheme->find(colorName);

            if (floss == nullptr) {
                floss = scheme->convert(color);
            }

            colorName = floss->name();
            DocumentFloss *documentFloss = new DocumentFloss(colorName, m_pattern->palette().freeSymbol(), Qt::SolidLine, (paletteEntry.stitchStrands) ? paletteEntry.stitchStrands : Configuration::palette_StitchStrands(), (paletteEntry.backstitchStrands) ? paletteEntry.backstitchStrands : Configuration::palette_BackstitchStrands());
            documentFloss->setFlossColor(floss->color());
            m_pattern->palette().add(i, documentFloss);
        }
    } else {
        throw FailedReadFile(QString(i18n("Invalid data read.")));
    }

    m_pattern->palette().setCurrentIndex(-1);

    Stitch::Type stitchType[] = {Stitch::Delete, Stitch::Full, Stitch::TL3Qtr, Stitch::TR3Qtr, Stitch::BL3Qtr, Stitch::BR3Qtr, Stitch::TBHalf, Stitch::BTHalf, Stitch::Delete, Stitch::TLQtr, Stitch::TRQtr, Stitch::BLQtr, Stitch::BRQtr}; // conversion of PCStitch to KXStitch

    int documentWidth = m_pattern->stitches().width();
    int documentHeight = m_pattern->stitches().height();
    int cells = documentWidth * documentHeight;

    quint16 cellCount;

    for (int i = 0 ; i < cells ; i += cellCount) {
        quint8 color;
        quint8 type;

        stream >> cellCount;
        stream >> color;
        stream >> type;

        if (type != 0xff) {
            for (int c = 0 ; c < cellCount ; c++) {
                int xc = (i + c) / documentHeight;
                int yc = (i + c) % documentHeight;
                m_pattern->stitches().addStitch(QPoint(xc, yc), stitchType[type], color - 1); // color-1 because PCStitch uses 1 based array
            }
        }
    }

    quint32 extras;
    stream >> extras;

    while (extras--) {
        quint16 x;
        quint16 y;
        quint8 color;
        quint8 type;

        stream >> x;
        stream >> y;

        for (int dx = 0 ; dx < 4 ; dx++) {
            stream >> color;
            stream >> type;

            if (type != 0xff) {
                m_pattern->stitches().addStitch(QPoint(x - 1, y - 1), stitchType[type], color - 1);
            }
        }
    }

    // read french knots

    quint32 knots;
    stream >> knots;

    while (knots--) {
        quint16 x;
        quint16 y;
        quint8 color;

        stream >> x;
        stream >> y;
        stream >> color;
        m_pattern->stitches().addFrenchKnot(QPoint(x - 1, y - 1), color - 1);
    }

    // read backstitches

    quint32 backstitches;
    stream >> backstitches;

    while (backstitches--) {
        quint16 sx;
        quint16 sy;
        quint16 sp;
        quint16 ex;
        quint16 ey;
        quint16 ep;
        quint8 color;
        stream >> sx >> sy >> sp >> ex >> ey >> ep >> color;
        m_pattern->stitches().addBackstitch(QPoint(--sx * 2 + ((sp - 1) % 3), --sy * 2 + ((sp - 1) / 3)), QPoint(--ex * 2 + ((ep - 1) % 3), --ey * 2 + ((ep - 1) / 3)), color - 1);
    }

    if (stream.status() != QDataStream::Ok) {
        throw FailedReadFile(QString(i18n("Stream error")));
    }
}


void Document::readPCStitch6File(QDataStream &stream)
{
    /* File Format
        uchar[256]      // header 'PCStitch 6 Pattern File'
                        // other information is contained here
        uchar[4]        // unknown
        uchar[4]        // unknown, there is something unique to each file here
        quint16         // pattern width
        quint16         // pattern height
        quint16         // cloth count width
        quint16         // cloth count height
        quint16         // string length
        char[]          // author
        quint16         // string length
        char[]          // copyright
        quint16         // string length
        char[]          // title
        quint16         // string length
        char[]          // fabric
        quint16         // string length
        char[]          // instructions
        char[25]        // 'PCStitch 6 Floss Palette!'
        quint16         // unknown
        char[30]        // 'DMC' padded with spaces
        char[10]        // scheme
        quint16         // palette entries
        quint16         // string length
        char[]          // symbol font
        uchar[4]        // unknown

        struct paletteEntry {
            char[30]    // color description
            uchar[4]    // RGBA
            char[10]    // color name
            uchar[65]   // unknown
                        // 59 unknown
                        // quint16 stitchStrands
                        // quint16 backstitchStrands
                        // quint16 possibly palette index (seems to increment from 1 to the number of palette entries, but odd files don't)
            char[30]    // color description
            uchar[5]    // unknown
            uchar[25]   // some color  // Black <padded with spaces> (possibly symbol color)
        } repeated for the number of palette entries

        struct stitches {
            quint16     // stitch count in columns from top left downward
            quint8      // color, 1 based index of color list, 0xFF for none
            quint8      // stitch type, 0xFF for none
        } repeated until sum of stitchCount == width * height

        quint32 extras
        struct extra {
            quint16     // x coordinate, 1 based index of cell
            quint16     // y coordinate, 1 based index of cell
            struct quadrant {
                quint8  // color, 1 based index of color list, 0xFF for none
                quint8  // stitch type, 0xFF for none
            } repeated 4 times
        } repeated for extras

        quint32         // unknown

        quint32 knots
        struct knot {
            quint16     // x coordinate, 1 based index of snap points
            quint16     // y coordinate, 1 based index of snap points
            quint8      // color, 1 based index of color list
        } repeated for knots

        uint32 backstitches
        struct backstitch {
            quint16     // start x, 1 based index of cell
            quint16     // start y, 1 based index of cell
            quint16     // start position, (1..9) based on snap points
            quint16     // end x, 1 based index of cell
            quint16     // end y, 1 based index of cell
            quint16     // end position, (1..9) based on snap points
            quint16     // color, 1 based index of color list
        } repeat for backstitches

        uchar[4]        // unknown
        uchar[4]        // unknown
    */
    stream.setByteOrder(QDataStream::LittleEndian);
    stream.device()->seek(256);

    quint16 unknown16;
    quint32 unknown32;

    stream >> unknown32;
    stream >> unknown32;

    quint16 width;
    quint16 height;
    stream >> width;
    stream >> height;

    m_pattern->stitches().resize(width, height);
    setProperty(QStringLiteral("unitsFormat"), Configuration::EnumDocument_UnitsFormat::Stitches);

    quint16 clothCount;
    stream >> clothCount;
    setProperty(QStringLiteral("horizontalClothCount"), double(clothCount));
    stream >> clothCount;
    setProperty(QStringLiteral("verticalClothCount"), double(clothCount));
    setProperty(QStringLiteral("clothCountUnits"), Configuration::EnumEditor_ClothCountUnits::Inches);

    setProperty(QStringLiteral("author"), readPCStitchString(stream));
    setProperty(QStringLiteral("copyright"), readPCStitchString(stream));
    setProperty(QStringLiteral("title"), readPCStitchString(stream));
    setProperty(QStringLiteral("fabric"), readPCStitchString(stream));
    setProperty(QStringLiteral("fabricColor"), QColor(Qt::white));

    QString instructions = readPCStitchString(stream);

    if (!instructions.isEmpty()) {
        int index = instructions.indexOf(QLatin1String("}}"));     // end of font defs
        index += 4;                                 // skip paste }} and CR LF
        index = instructions.indexOf(QLatin1Char(' '), index);   // find first space - end of text def
        index++;                                    // and skip past it
                                                    // index should now point to the first character of the instructions
        instructions = instructions.remove(0, index);
        instructions.truncate(instructions.length() - 10);
    }

    setProperty(QStringLiteral("instructions"), instructions);

    char buffer[125];
    stream.readRawData(buffer, 25);

    if (strncmp(buffer, "PCStitch 6 Floss Palette!", 25) == 0) {
        stream >> unknown16;
        stream.readRawData(buffer, 40);

        quint16 colors;
        stream >> colors;

        readPCStitchString(stream); // symbols font

        stream >> unknown32;

        m_pattern->palette().setSchemeName(QStringLiteral("DMC"));
        FlossScheme *scheme = SchemeManager::scheme(QStringLiteral("DMC"));

        if (scheme == nullptr) {
            throw FailedReadFile(QString(i18n("The floss scheme DMC was not found")));    // this shouldn't happen because DMC should always be available
        }

        for (int i = 0 ; i < colors ; i++) {
#pragma pack(push)
#pragma pack(1)     // pack the structure
            struct PALETTE_ENTRY {
                char            colorDescription_1[30];
                unsigned char   RGBA[4];
                char            colorName_1[10];
                unsigned char   unknown_1[59];
                unsigned short  stitchStrands;
                unsigned short  backstitchStrands;
                unsigned short  unknown_2;
                char            colorDescription_2[30];
                unsigned char   unknown_3[5];
                char            colorName_2[25];            // seems to be Black all the time
            } paletteEntry;
#pragma pack(pop)

            stream.readRawData(reinterpret_cast<char *>(&paletteEntry), sizeof(struct PALETTE_ENTRY));

            QColor color = QColor(paletteEntry.RGBA[0], paletteEntry.RGBA[1], paletteEntry.RGBA[2]);
            QString colorName = QString::fromLatin1(paletteEntry.colorName_1, 10).trimmed();  // minus the white space

            if (colorName == QLatin1String("White")) {
                colorName = QLatin1String("Blanc");                        // fix colorName
            }

            if (colorName == QLatin1String("5200")) {
                colorName = QLatin1String("B5200");                        // fix colorName
            }

            Floss *floss = scheme->find(colorName);

            if (floss == nullptr) {
                floss = scheme->convert(color);     // the name wasn't found so look for a similar color in DMC
            }

            colorName = floss->name();
            DocumentFloss *documentFloss = new DocumentFloss(colorName, m_pattern->palette().freeSymbol(), Qt::SolidLine, paletteEntry.stitchStrands, paletteEntry.backstitchStrands);
            documentFloss->setFlossColor(floss->color());
            m_pattern->palette().add(i, documentFloss);
        }
    } else {
        throw FailedReadFile(QString(i18n("Invalid data read.")));
    }

    m_pattern->palette().setCurrentIndex(-1);

    Stitch::Type stitchType[] = {Stitch::Delete, Stitch::Full, Stitch::TL3Qtr, Stitch::TR3Qtr, Stitch::BL3Qtr, Stitch::BR3Qtr, Stitch::TBHalf, Stitch::BTHalf, Stitch::Delete, Stitch::TLQtr, Stitch::TRQtr, Stitch::BLQtr, Stitch::BRQtr}; // conversion of PCStitch to KXStitch

    int documentWidth = m_pattern->stitches().width();
    int documentHeight = m_pattern->stitches().height();
    int cells = documentWidth * documentHeight;

    quint16 cellCount;

    for (int i = 0 ; i < cells ; i += cellCount) {
        quint8 color;
        quint8 type;

        stream >> cellCount;
        stream >> color;
        stream >> type;

        if (type != 0xff) {
            for (int c = 0 ; c < cellCount ; c++) {
                int xc = (i + c) / documentHeight;
                int yc = (i + c) % documentHeight;
                m_pattern->stitches().addStitch(QPoint(xc, yc), stitchType[type], color - 1); // color-1 because PCStitch uses 1 based array
            }
        }
    }

    quint32 extras;
    stream >> extras;

    while (extras--) {
        qint16 x;
        qint16 y;
        quint8 color;
        quint8 type;

        stream >> x;
        stream >> y;

        for (int dx = 0 ; dx < 4 ; dx++) {
            stream >> color;
            stream >> type;

            if (type != 0xff) {
                m_pattern->stitches().addStitch(QPoint(x - 1, y - 1), stitchType[type], color - 1);
            }
        }
    }

    // read french knots

    quint32 knots;
    stream >> knots;

    while (knots--) {
        qint16 x;
        qint16 y;
        quint8 color;

        stream >> x;
        stream >> y;
        stream >> color;
        m_pattern->stitches().addFrenchKnot(QPoint(x - 1, y - 1), color - 1);
    }

    // read backstitches

    qint32 backstitches;
    stream >> backstitches;

    while (backstitches--) {
        qint16 sx;
        qint16 sy;
        qint16 sp;
        qint16 ex;
        qint16 ey;
        qint16 ep;
        quint16 color;
        stream >> sx >> sy >> sp >> ex >> ey >> ep >> color;
        m_pattern->stitches().addBackstitch(QPoint(--sx * 2 + ((sp - 1) % 3), --sy * 2 + ((sp - 1) / 3)), QPoint(--ex * 2 + ((ep - 1) % 3), --ey * 2 + ((ep - 1) / 3)), color - 1);
    }

    if (stream.status() != QDataStream::Ok) {
        throw FailedReadFile(QString(i18n("Stream error")));
    }
}


void Document::readPCStitch7File(QDataStream &stream)
{
    /* File Format
        uchar[256]      // header 'PCStitch 7 Pattern File'
                        // other information is contained here
        uchar[4]        // unknown
        uchar[4]        // unknown, there is something unique to each file here
        quint16         // pattern width
        quint16         // pattern height
        quint16         // cloth count width
        quint16         // cloth count height
        uchar[4]        // RGBA - fabric colour
        quint16         // string length
        char[]          // author
        quint16         // string length
        char[]          // copyright
        quint16         // string length
        char[]          // title
        quint16         // string length
        char[]          // fabric
        quint16         // string length
        char[]          // instructions
        quint16         // string length
        char[]          // keywords
        quint16         // string length
        char[]          // website
        uchar[4]        // unknown - varies e.g. (0x02 0x00 0x02 0x00) (0x02 0x00 0x01 0x00)
                        // possible default stitch / backstitch strands quint16
        char[25]        // 'PCStitch Floss Palette!!!'
        uchar[4]        // unknown
        quint16         // palette entries

        [
            other possible entries within the header may be
            company
            logo
            read only flag
            read only password
            stitches/inches flag - size maybe in inches
            square weave flag
        ]

        struct paletteEntry {
            char[33]    // scheme
                        // palette allows for multiple schemes to be used
                        // keycode used to identify source list
            char[10]    // color name
            char[30]    // color description
            uchar[4]    // unknown
            uchar[4]    // RGBA
            uchar[81]   // unknown
            char[30]    // symbol font
            uchar[7]    // 0x00 0x00 0x42 0x00 0x00 0x00 0x00 - varies (symbols probably here, 3rd for stitches?)
            char[30]    // color description
            uchar[4]    // RGBA
            char[10]    // color name
            uchar[7]    // 0x00 0x00 0x00 0x00 0x00 0x01 0x00
                        // possibly stitch strands (not sure of order, 6th possibly backstitch) 0x00 for default
                        // Full, 3 quarter, quarter, petite, half, french, backstitches
        } repeated for the number of palette entries

        struct stitches {
            quint16     // stitch count in columns from top left downward
            quint8      // color, 1 based index of color list, 0xFF for none
            quint8      // stitch type, 0xFF for none
        } repeated until sum of stitch count = width*height

        uint32 extras
        struct extra {
            quint16     // x coordinate, 1 based index of cell
            quint16     // y coordinate, 1 based index of cell
            struct {
                quint8  // color, 1 based index of color list, 0xFF for none
                quint8  // stitch type, 0xFF for none
            } repeated 4 times
        } repeated for extras

        uint32 knots
        struct knot {
            quint16     // x coordinate, 1 based index of snap points
            quint16     // y coordinate, 1 based index of snap points
            quint16     // color, 1 based index of color list
        } repeated for knotCount

        uint32 backstitches
        struct backstitch {
            quint16     // start x, 1 based index of cell
            quint16     // start y, 1 based index of cell
            quint16     // start position, (1..9) based on snap points
            quint16     // end x, 1 based index of cell
            quint16     // end Y, 1 based index of cell
            quint16     // end position (1..9) based on snap points
            quint16     // color, 1 based index of color list
        } repeated for backstitches
    */
    stream.setByteOrder(QDataStream::LittleEndian);
    stream.device()->seek(256);

    quint32 unknown32;
    stream >> unknown32;
    stream >> unknown32;

    quint16 width;
    quint16 height;
    stream >> width;
    stream >> height;

    m_pattern->stitches().resize(width, height);
    setProperty(QStringLiteral("unitsFormat"), Configuration::EnumDocument_UnitsFormat::Stitches);

    quint16 clothCount;
    stream >> clothCount;
    setProperty(QStringLiteral("horizontalClothCount"), double(clothCount));
    stream >> clothCount;
    setProperty(QStringLiteral("verticalClothCount"), double(clothCount));
    setProperty(QStringLiteral("clothCountUnits"), Configuration::EnumEditor_ClothCountUnits::Inches);

    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
    stream >> r >> g >> b >> a;
    setProperty(QStringLiteral("fabricColor"), QColor(r, g, b)); // alpha defaults to 255
    setProperty(QStringLiteral("author"), readPCStitchString(stream));
    setProperty(QStringLiteral("copyright"), readPCStitchString(stream));
    setProperty(QStringLiteral("title"), readPCStitchString(stream));
    setProperty(QStringLiteral("fabric"), readPCStitchString(stream));

    QString instructions = readPCStitchString(stream);

    if (!instructions.isEmpty()) {
        int index = instructions.indexOf(QLatin1String("}}"));     // end of font defs
        index += 4;                                 // skip paste }} and CR LF
        index = instructions.indexOf(QLatin1Char(' '), index);   // find first space - end of text def
        index++;                                    // and skip past it
                                                    // index should now point to the first character of the instructions
        instructions = instructions.remove(0, index);
        instructions.truncate(instructions.length() - 10);
    }

    setProperty(QStringLiteral("instructions"), instructions);

    QString keywords(readPCStitchString(stream));
    QString website(readPCStitchString(stream));

    quint16 defaultStitchStrands;
    quint16 defaultBackstitchStrands;
    stream >> defaultStitchStrands >> defaultBackstitchStrands;

    char *buffer = new char[25];
    stream.readRawData(buffer, 25);

    if (strncmp(buffer, "PCStitch Floss Palette!!!", 25) == 0) {
        stream >> unknown32;

        quint16 colors;
        stream >> colors;

        m_pattern->palette().setSchemeName(QStringLiteral("DMC"));
        FlossScheme *scheme = SchemeManager::scheme(QStringLiteral("DMC"));

        if (scheme == nullptr) {
            throw FailedReadFile(QString(i18n("The floss scheme DMC was not found")));    // this shouldn't happen because DMC should always be available
        }

        for (int i = 0 ; i < colors ; i++) {
#pragma pack(push)
#pragma pack(1)
            struct PALETTE_ENTRY {
                char        scheme[33];
                char        colorName_1[10];
                char        colorDescription_1[30];
                uchar       unknown_1[4];
                uchar       RGBA_1[4];
                uchar       unknown_2[81];
                char        font[30];
                uchar       unknown_3[7];
                char        colorDescription_2[30];
                uchar       RGBA_2[4];
                char        colorName_2[10];
                uchar       unknown_4[7];
            } paletteEntry;
#pragma pack(pop)

            stream.readRawData(reinterpret_cast<char *>(&paletteEntry), sizeof(struct PALETTE_ENTRY));

            QColor color = QColor(int(paletteEntry.RGBA_1[0]), int(paletteEntry.RGBA_1[1]), int(paletteEntry.RGBA_1[2]));
            QString colorName = QString::fromLatin1(paletteEntry.colorName_1, 10).trimmed();  // minus the white space

            if (colorName == QLatin1String("White")) {
                colorName = QLatin1String("Blanc");     // fix colorName
            }

            if (colorName == QLatin1String("5200")) {
                colorName = QLatin1String("B5200");     // fix colorName
            }

            Floss *floss = scheme->find(colorName);

            if (floss == nullptr) {
                floss = scheme->convert(color);     // the name wasn't found so look for a similar color in DMC
            }

            colorName = floss->name();
            DocumentFloss *documentFloss = new DocumentFloss(colorName, m_pattern->palette().freeSymbol(), Qt::SolidLine, defaultStitchStrands, defaultBackstitchStrands);
            documentFloss->setFlossColor(floss->color());
            m_pattern->palette().add(i, documentFloss);
        }
    } else {
        throw FailedReadFile(QString(i18n("Invalid data read.")));
    }

    m_pattern->palette().setCurrentIndex(-1);

    Stitch::Type stitchType[] = {Stitch::Delete, Stitch::Full, Stitch::TL3Qtr, Stitch::TR3Qtr, Stitch::BL3Qtr, Stitch::BR3Qtr, Stitch::TBHalf, Stitch::BTHalf, Stitch::Delete, Stitch::TLQtr, Stitch::TRQtr, Stitch::BLQtr, Stitch::BRQtr}; // conversion of PCStitch to KXStitch
    // TODO above needs to include petite stitches
    int documentWidth = m_pattern->stitches().width();
    int documentHeight = m_pattern->stitches().height();
    int cells = documentWidth * documentHeight;

    quint16 cellCount;

    for (int i = 0 ; i < cells ; i += cellCount) {
        quint8 color;
        quint8 type;

        stream >> cellCount;
        stream >> color;
        stream >> type;

        if (type != 0xff) {
            for (int c = 0 ; c < cellCount ; c++) {
                int xc = (i + c) / documentHeight;
                int yc = (i + c) % documentHeight;
                m_pattern->stitches().addStitch(QPoint(xc, yc), stitchType[type], color - 1); // color-1 because PCStitch uses 1 based array
            }
        }
    }

    quint32 extras;
    stream >> extras;

    while (extras--) {
        quint16 x;
        quint16 y;
        quint8 color;
        quint8 type;

        stream >> x;
        stream >> y;

        for (int dx = 0 ; dx < 4 ; dx++) {
            stream >> color;
            stream >> type;

            if (type != 0xff) {
                m_pattern->stitches().addStitch(QPoint(x - 1, y - 1), stitchType[type], color - 1);
            }
        }
    }

    // read french knots

    quint32 knots;
    stream >> knots;

    while (knots--) {
        quint16 x;
        quint16 y;
        quint16 color;

        stream >> x;
        stream >> y;
        stream >> color;
        m_pattern->stitches().addFrenchKnot(QPoint(x - 1, y - 1), color - 1);
    }

    // read backstitches

    quint32 backstitches;
    stream >> backstitches;

    while (backstitches--) {
        quint16 sx;
        quint16 sy;
        quint16 sp;
        quint16 ex;
        quint16 ey;
        quint16 ep;
        quint16 color;
        stream >> sx >> sy >> sp >> ex >> ey >> ep >> color;
        m_pattern->stitches().addBackstitch(QPoint(--sx * 2 + ((sp - 1) % 3), --sy * 2 + ((sp - 1) / 3)), QPoint(--ex * 2 + ((ep - 1) % 3), --ey * 2 + ((ep - 1) / 3)), color - 1);
    }

    if (stream.status() != QDataStream::Ok) {
        throw FailedReadFile(QString(i18n("Stream error")));
    }
}


QString Document::readPCStitchString(QDataStream &stream)
{
    char *buffer;
    quint16 stringSize;
    stream >> stringSize;
    buffer = new char[stringSize + 1];
    stream.readRawData(buffer, stringSize);
    buffer[stringSize] = '\0';
    QString string = QString::fromLatin1(buffer);
    delete [] buffer;

    if (stream.status() != QDataStream::Ok) {
        throw FailedReadFile(QString(i18n("Stream error")));
    }

    return string;
}


void Document::readKXStitchV2File(QDataStream &stream)
{
    /* File format
        // header
        #QString("KXStitch")        // read in the load function
        #qint16(FILE_FORMAT_VERSION)    // read in the load function
        QString(title)
        QString(author)
        QString(fabric)
        QColor(fabricColor) // serialised format has changed over the various versions of qt.
                    // for older versions this appears to be quint8 red, quint8 green quint8 blue, quint8 alpha
        qint16(index from flossscheme qcombobox)    // unpredictable, palette stream will contain the scheme name
        qint16(inches)      // boolean, true if the size displayed is in inches
        qint32(width)       // width of pattern (is this in stitches or inches?) // discard in favour of stitchdata width
        qint32(height)      // height of pattern (is this in stitches or inches?) // discard in favour of stitchdata height
        qint32(clothCount)  // this needs to be converted to a double
        QString(instructions)

        // palette
        qint32(current)     // the current color selected
        qint32(colors)      // number of colors in the palette
        QString(schemeName)
            QString(flossName)      ]
            QString(flossDescription)   ] Repeated for each
            QColor(flossColor)      ] entry in the palette, note comment above regarding QColor
            qint16(flossSymbol)     ]

        // stitches
        qint32(width)       // width in stitches // use instead of width from the header which might be in inches
        qint32(height)      // height in stitches // use instead of height from the header which might be in inches
            qint8(stitches)                 ]
                qint8(type)     ] Repeated for  ] Repeated for
                qint16(colorIndex)  ] each stitch   ] each stitchqueue

        // backstitches
        qint32(backstitches)    // the number of backstitches
            QPoint(start)       ]
            QPoint(end)     ] Repeated for
            qint16(colorIndex)  ] each backstitch

        // there were no knots supported in this version
        */

    /* this is currently untested */
    // read header
    QString title;
    QString author;
    QString fabric;
    stream  >> title
            >> author
            >> fabric;
    setProperty(QStringLiteral("title"), title);
    setProperty(QStringLiteral("author"), author);
    setProperty(QStringLiteral("fabric"), fabric);

    quint8 red;
    quint8 green;
    quint8 blue;
    quint8 alpha;
    stream  >> red
            >> green
            >> blue
            >> alpha;
    setProperty(QStringLiteral("fabricColor"), QColor(red, green, blue, alpha));

    qint16 schemeIndex;
    stream  >> schemeIndex; // discard

    qint16 inches;
    stream  >> inches;
    setProperty(QStringLiteral("unitsFormat"), (bool(inches)) ? Configuration::EnumDocument_UnitsFormat::Inches : Configuration::EnumDocument_UnitsFormat::Stitches);

    qint32 width;
    qint32 height;
    stream  >> width
            >> height;

    qint32 clothCount;
    stream  >> clothCount;
    setProperty(QStringLiteral("clothCountUnits"), Configuration::EnumEditor_ClothCountUnits::Inches);
    setProperty(QStringLiteral("horizontalClothCount"), double(clothCount));
    setProperty(QStringLiteral("verticalClothCount"), double(clothCount));
    setProperty(QStringLiteral("clothCountLink"), true);

    QString instructions;
    stream  >> instructions;
    setProperty(QStringLiteral("instructions"), instructions);

    // read palette
    qint32 current;
    stream  >> current;

    qint32 colors;
    stream  >> colors;

    QString schemeName;
    stream  >> schemeName;
    m_pattern->palette().setSchemeName(schemeName);

    FlossScheme *flossScheme = SchemeManager::scheme(schemeName);

    if (flossScheme == nullptr) {
        throw FailedReadFile(QString(i18n("The floss scheme %1 was not found", schemeName)));
    }

    int colorIndex = 0;

    while (colors--) {
        QString flossName;
        QString flossDescription;
        // QColor color - read as rgba instead
        qint16 symbol;

        stream  >> flossName
                >> flossDescription
                >> red
                >> green
                >> blue
                >> alpha
                >> symbol;

        Floss *floss = flossScheme->find(flossName);

        if (floss == nullptr) {
            throw FailedReadFile(QString(i18n("The floss name %1 was not found", flossName)));
        }

        DocumentFloss *documentFloss = new DocumentFloss(flossName, m_pattern->palette().freeSymbol(), Qt::SolidLine, Configuration::palette_StitchStrands(), Configuration::palette_BackstitchStrands());
        documentFloss->setFlossColor(floss->color());
        m_pattern->palette().add(colorIndex++, documentFloss);
    }


    // read stitches
    stream  >> width
            >> height;
    qint32 cells = width * height;
    m_pattern->stitches().resize(width, height);

    for (int i = 0 ; i < cells ; i++) {
        qint8 stitches;
        stream  >> stitches;

        while (stitches--) {
            qint8 type;
            qint16 colorIndex;
            stream  >> type
                    >> colorIndex;

            m_pattern->stitches().addStitch(QPoint(i % width, i / width), Stitch::Type(type), colorIndex);
        }
    }

    qint32 backstitches;
    stream  >> backstitches;

    while (backstitches--) {
        QPoint start;
        QPoint end;
        qint16 colorIndex;
        stream  >> start
                >> end
                >> colorIndex;

        m_pattern->stitches().addBackstitch(start, end, colorIndex);
    }

    if (stream.status() != QDataStream::Ok) {
        throw FailedReadFile(QString(i18n("Stream error")));
    }
}


void Document::readKXStitchV3File(QDataStream &stream)
{
    /* File format
        // header
        #QString("KXStitch")        // read in the load function
        #qint16(FILE_FORMAT_VERSION)    // read in the load function
        QString(title)
        QString(author)
        QString(fabric)
        QColor(fabricColor) // serialised format has changed over the various versions of qt.
                    // for older versions this appears to be quint8 red, quint8 green quint8 blue, quint8 alpha
        qint16(index from flossscheme qcombobox)    // unpredictable, palette stream will contain the scheme name
        qint16(inches)      // boolean, true if the size displayed is in inches
        qint32(width)       // width of pattern (is this in stitches or inches?) // discard in favour of stitchdata width
        qint32(height)      // height of pattern (is this in stitches or inches?) // discard in favour of stitchdata height
        qint32(clothCount)  // this needs to be converted to a double
        QString(instructions)

        // palette
        QString(schemeName)
        qint32(current)     // the current color selected
        qint32(colors)      // number of colors in the palette
            qint32(colorIndex)      ]
            QString(flossName)      ] Repeated for each
            qint16(flossSymbol)     ] entry in the palette

        // stitches
        qint32(width)       // width in stitches // use instead of width from the header which might be in inches
        qint32(height)      // height in stitches // use instead of height from the header which might be in inches
            qint8(stitches)                 ]
                qint8(type)     ] Repeated for  ] Repeated for
                qint16(colorIndex)  ] each stitch   ] each stitchqueue

        // backstitches
        qint32(backstitches)    // the number of backstitches
            QPoint(start)       ]
            QPoint(end)     ] Repeated for
            qint16(colorIndex)  ] each backstitch

        // there were no knots supported in this version
        */


    /* this is currently untested */
    // read header
    QString title;
    QString author;
    QString fabric;
    stream  >> title
            >> author
            >> fabric;
    setProperty(QStringLiteral("title"), title);
    setProperty(QStringLiteral("author"), author);
    setProperty(QStringLiteral("fabric"), fabric);

    quint8 red;
    quint8 green;
    quint8 blue;
    quint8 alpha;
    stream  >> red
            >> green
            >> blue
            >> alpha;
    setProperty(QStringLiteral("fabricColor"), QColor(red, green, blue, alpha));

    qint16 schemeIndex;
    stream  >> schemeIndex; // discard

    qint16 inches;
    stream  >> inches;
    setProperty(QStringLiteral("unitsFormat"), (bool(inches)) ? Configuration::EnumDocument_UnitsFormat::Inches : Configuration::EnumDocument_UnitsFormat::Stitches);

    qint32 width;
    qint32 height;
    stream  >> width
            >> height;

    qint32 clothCount;
    stream  >> clothCount;
    setProperty(QStringLiteral("clothCountUnits"), Configuration::EnumEditor_ClothCountUnits::Inches);
    setProperty(QStringLiteral("horizontalClothCount"), double(clothCount));
    setProperty(QStringLiteral("verticalClothCount"), double(clothCount));
    setProperty(QStringLiteral("clothCountLink"), true);

    QString instructions;
    stream  >> instructions;
    setProperty(QStringLiteral("instructions"), instructions);

    // read palette
    QString schemeName;
    stream  >> schemeName;
    m_pattern->palette().setSchemeName(schemeName);

    FlossScheme *flossScheme = SchemeManager::scheme(schemeName);

    if (flossScheme == nullptr) {
        throw FailedReadFile(QString(i18n("The floss scheme %1 was not found", schemeName)));
    }

    qint32 current;
    stream  >> current;

    qint32 colors;
    stream  >> colors;

    while (colors--) {
        qint32 colorIndex;
        QString flossName;
        qint16 symbol;

        stream  >> colorIndex
                >> flossName
                >> symbol;

        Floss *floss = flossScheme->find(flossName);

        if (floss == nullptr) {
            throw FailedReadFile(QString(i18n("The floss name %1 was not found", flossName)));
        }

        DocumentFloss *documentFloss = new DocumentFloss(flossName, m_pattern->palette().freeSymbol(), Qt::SolidLine, Configuration::palette_StitchStrands(), Configuration::palette_BackstitchStrands());
        documentFloss->setFlossColor(floss->color());
        m_pattern->palette().add(colorIndex, documentFloss);
    }


    // read stitches
    stream  >> width
            >> height;
    qint32 cells = width * height;
    m_pattern->stitches().resize(width, height);

    for (int i = 0 ; i < cells ; i++) {
        qint8 stitches;
        stream  >> stitches;

        while (stitches--) {
            qint8 type;
            qint16 colorIndex;
            stream  >> type
                    >> colorIndex;

            m_pattern->stitches().addStitch(QPoint(i % width, i / width), Stitch::Type(type), colorIndex);
        }
    }

    qint32 backstitches;
    stream  >> backstitches;

    while (backstitches--) {
        QPoint start;
        QPoint end;
        qint16 colorIndex;
        stream  >> start
                >> end
                >> colorIndex;

        m_pattern->stitches().addBackstitch(start, end, colorIndex);
    }

    if (stream.status() != QDataStream::Ok) {
        throw FailedReadFile(QString(i18n("Stream error")));
    }
}


void Document::readKXStitchV4File(QDataStream &stream)
{
    // version 4 wasn't used in the release versions.
    // but was available in cvs
    /* File format
        // header
        #QString("KXStitch")        // read in the load function
        #qint16(FILE_FORMAT_VERSION)    // read in the load function
        QString(title)
        QString(author)
        QString(fabric)
        QColor(fabricColor) // serialised format has changed over the various versions of qt.
                    // for older versions this appears to be quint8 red, quint8 green quint8 blue, quint8 alpha
        qint16(index from flossscheme qcombobox)    // unpredictable, palette stream will contain the scheme name
        qint16(inches)      // boolean, true if the size displayed is in inches
        qint32(width)       // width of pattern (is this in stitches or inches?) // discard in favour of stitchdata width
        qint32(height)      // height of pattern (is this in stitches or inches?) // discard in favour of stitchdata height
        qint32(clothCount)  // this needs to be converted to a double
        QString(instructions)

        // palette
        QString(schemeName)
        qint32(current)     // the current color selected
        qint32(colors)      // number of colors in the palette
            qint32(colorIndex)      ]
            QString(flossName)      ] Repeated for each
            qint16(flossSymbol)     ] entry in the palette

        // stitches
        qint32(width)       // width in stitches // use instead of width from the header which might be in inches
        qint32(height)      // height in stitches // use instead of height from the header which might be in inches
            qint8(stitches)                 ]
                qint8(type)     ] Repeated for  ] Repeated for
                qint16(colorIndex)  ] each stitch   ] each stitchqueue

        // knots
        qint32(knots)       // the number of knots
            QPoint(position)    ] Repeated for
            qint16(colorIndex)  ] each knot

        // backstitches
        qint32(backstitches)    // the number of backstitches
            QPoint(start)       ]
            QPoint(end)     ] Repeated for
            qint16(colorIndex)  ] each backstitch
        */


    /* this is currently untested */
    // read header
    QString title;
    QString author;
    QString fabric;
    stream  >> title
            >> author
            >> fabric;
    setProperty(QStringLiteral("title"), title);
    setProperty(QStringLiteral("author"), author);
    setProperty(QStringLiteral("fabric"), fabric);

    quint8 red;
    quint8 green;
    quint8 blue;
    quint8 alpha;
    stream  >> red
            >> green
            >> blue
            >> alpha;
    setProperty(QStringLiteral("fabricColor"), QColor(red, green, blue, alpha));

    qint16 schemeIndex;
    stream  >> schemeIndex; // discard

    qint16 inches;
    stream  >> inches;
    setProperty(QStringLiteral("unitsFormat"), (bool(inches)) ? Configuration::EnumDocument_UnitsFormat::Inches : Configuration::EnumDocument_UnitsFormat::Stitches);

    qint32 width;
    qint32 height;
    stream  >> width
            >> height;

    qint32 clothCount;
    stream  >> clothCount;
    setProperty(QStringLiteral("clothCountUnits"), Configuration::EnumEditor_ClothCountUnits::Inches);
    setProperty(QStringLiteral("horizontalClothCount"), double(clothCount));
    setProperty(QStringLiteral("verticalClothCount"), double(clothCount));
    setProperty(QStringLiteral("clothCountLink"), true);

    QString instructions;
    stream  >> instructions;
    setProperty(QStringLiteral("instructions"), instructions);

    // read palette
    QString schemeName;
    stream  >> schemeName;
    m_pattern->palette().setSchemeName(schemeName);

    FlossScheme *flossScheme = SchemeManager::scheme(schemeName);

    if (flossScheme == nullptr) {
        throw FailedReadFile(QString(i18n("The floss scheme %1 was not found", schemeName)));
    }

    qint32 current;
    stream  >> current;

    qint32 colors;
    stream  >> colors;

    while (colors--) {
        qint32 colorIndex;
        QString flossName;
        qint16 symbol;

        stream  >> colorIndex
                >> flossName
                >> symbol;

        Floss *floss = flossScheme->find(flossName);

        if (floss == nullptr) {
            throw FailedReadFile(QString(i18n("The floss name %1 was not found", flossName)));
        }

        DocumentFloss *documentFloss = new DocumentFloss(flossName, m_pattern->palette().freeSymbol(), Qt::SolidLine, Configuration::palette_StitchStrands(), Configuration::palette_BackstitchStrands());
        documentFloss->setFlossColor(floss->color());
        m_pattern->palette().add(colorIndex, documentFloss);
    }


    // read stitches
    stream  >> width
            >> height;
    qint32 cells = width * height;
    m_pattern->stitches().resize(width, height);

    for (int i = 0 ; i < cells ; i++) {
        qint8 stitches;
        stream  >> stitches;

        while (stitches--) {
            qint8 type;
            qint16 colorIndex;
            stream  >> type
                    >> colorIndex;

            m_pattern->stitches().addStitch(QPoint(i % width, i / width), Stitch::Type(type), colorIndex);
        }
    }

    qint32 knots;
    stream  >> knots;

    while (knots--) {
        QPoint position;
        qint16 colorIndex;
        stream  >> position
                >> colorIndex;

        m_pattern->stitches().addFrenchKnot(position, colorIndex);
    }

    qint32 backstitches;
    stream  >> backstitches;

    while (backstitches--) {
        QPoint start;
        QPoint end;
        qint16 colorIndex;
        stream  >> start
                >> end
                >> colorIndex;

        m_pattern->stitches().addBackstitch(start, end, colorIndex);
    }

    if (stream.status() != QDataStream::Ok) {
        throw FailedReadFile(QString(i18n("Stream error")));
    }
}


void Document::readKXStitchV5File(QDataStream &stream)
{
    /* File format
        // header
        #QString("KXStitch")        // read in the load function
        #qint16(FILE_FORMAT_VERSION)    // read in the load function

        // properties dialog
        QString(sizeUnits)
        qint32(width)       // this is in stitches
        qint32(height)      // this is in stitches
        double(clothCount)
        QString(clothCountUnits)
        QString(title)
        QString(author)
        QString(copyright)
        QString(fabric)
        QColor(fabricColor) // serialised format has changed over the various versions of qt.
                    // for older versions this appears to be quint8 red, quint8 green quint8 blue, quint8 alpha
        QString(schemeName)
        QString(instructions)

        // palette
        QString(schemeName)
        qint32(current)     // the current color selected
        qint32(colors)      // number of colors in the palette
            qint32(colorIndex)      ]
            QString(flossName)      ] Repeated for each
            qint16(flossSymbol)     ] entry in the palette

        // stitches
        qint32(width)       // width in stitches // use instead of width from the header which might be in inches
        qint32(height)      // height in stitches // use instead of height from the header which might be in inches
            qint8(stitches)                 ]
                qint8(type)     ] Repeated for  ] Repeated for
                qint16(colorIndex)  ] each stitch   ] each stitchqueue

        // knots
        qint32(knots)       // the number of knots
            QPoint(position)    ] Repeated for
            qint16(colorIndex)  ] each knot

        // backstitches
        qint32(backstitches)    // the number of backstitches
            QPoint(start)       ]
            QPoint(end)     ] Repeated for
            qint16(colorIndex)  ] each backstitch
        */


    /* this is currently untested */
    // read header
    QString sizeUnits;
    qint32 width;
    qint32 height;
    stream  >> sizeUnits
            >> width
            >> height;

    if (sizeUnits == i18n("Stitches")) {
        setProperty(QStringLiteral("unitsFormat"), Configuration::EnumDocument_UnitsFormat::Stitches);
    }

    if (sizeUnits == i18n("CM")) {
        setProperty(QStringLiteral("unitsFormat"), Configuration::EnumDocument_UnitsFormat::CM);
    }

    if (sizeUnits == i18n("Inches")) {
        setProperty(QStringLiteral("unitsFormat"), Configuration::EnumDocument_UnitsFormat::Inches);
    }

    double clothCount;
    QString clothCountUnits;
    stream  >> clothCount
            >> clothCountUnits;
    setProperty(QStringLiteral("horizontalClothCount"), double(clothCount));
    setProperty(QStringLiteral("verticalClothCount"), double(clothCount));
    setProperty(QStringLiteral("clothCountLink"), true);

    if (clothCountUnits == QLatin1String("/cm")) {
        setProperty(QStringLiteral("clothCountUnits"), Configuration::EnumEditor_ClothCountUnits::CM);
    }

    if (clothCountUnits == QLatin1String("/in")) {
        setProperty(QStringLiteral("clothCountUnits"), Configuration::EnumEditor_ClothCountUnits::Inches);
    }

    QString title;
    QString author;
    QString copyright;
    QString fabric;
    stream  >> title
            >> author
            >> copyright
            >> fabric;
    setProperty(QStringLiteral("title"), title);
    setProperty(QStringLiteral("author"), author);
    setProperty(QStringLiteral("copyright"), copyright);
    setProperty(QStringLiteral("fabric"), fabric);

    quint8 red;
    quint8 green;
    quint8 blue;
    quint8 alpha;
    stream  >> red
            >> green
            >> blue
            >> alpha;
    setProperty(QStringLiteral("fabricColor"), QColor(red, green, blue, alpha));

    QString schemeName;
    stream  >> schemeName;  // discard, also read in palette

    QString instructions;
    stream  >> instructions;
    setProperty(QStringLiteral("instructions"), instructions);

    // read palette
    stream  >> schemeName;
    m_pattern->palette().setSchemeName(schemeName);

    FlossScheme *flossScheme = SchemeManager::scheme(schemeName);

    if (flossScheme == nullptr) {
        throw FailedReadFile(QString(i18n("The floss scheme %1 was not found", schemeName)));
    }

    qint32 current;
    stream  >> current;

    qint32 colors;
    stream  >> colors;

    while (colors--) {
        qint32 colorIndex;
        QString flossName;
        qint16 symbol;

        stream  >> colorIndex
                >> flossName
                >> symbol;

        Floss *floss = flossScheme->find(flossName);

        if (floss == nullptr) {
            throw FailedReadFile(QString(i18n("The floss name %1 was not found", flossName)));
        }

        DocumentFloss *documentFloss = new DocumentFloss(flossName, m_pattern->palette().freeSymbol(), Qt::SolidLine, Configuration::palette_StitchStrands(), Configuration::palette_BackstitchStrands());
        documentFloss->setFlossColor(floss->color());
        m_pattern->palette().add(colorIndex, documentFloss);
    }

    // read stitches
    stream  >> width
            >> height;
    qint32 cells = width * height;
    m_pattern->stitches().resize(width, height);

    for (int i = 0 ; i < cells ; i++) {
        qint8 stitches;
        stream  >> stitches;

        while (stitches--) {
            qint8 type;
            qint16 colorIndex;
            stream  >> type
                    >> colorIndex;

            m_pattern->stitches().addStitch(QPoint(i % width, i / width), Stitch::Type(type), colorIndex);
        }
    }

    qint32 knots;
    stream  >> knots;

    while (knots--) {
        QPoint position;
        qint16 colorIndex;
        stream  >> position
                >> colorIndex;

        m_pattern->stitches().addFrenchKnot(position, colorIndex);
    }

    qint32 backstitches;
    stream  >> backstitches;

    while (backstitches--) {
        QPoint start;
        QPoint end;
        qint16 colorIndex;
        stream  >> start
                >> end
                >> colorIndex;

        m_pattern->stitches().addBackstitch(start, end, colorIndex);
    }

    if (stream.status() != QDataStream::Ok) {
        throw FailedReadFile(QString(i18n("Stream error")));
    }
}


void Document::readKXStitchV6File(QDataStream &stream)
{
    /* File format
        // header
        #QString("KXStitch")        // read in the load function
        #qint16(FILE_FORMAT_VERSION)    // read in the load function

        // properties dialog
        QString(sizeUnits)
        qint32(width)       // this is in stitches
        qint32(height)      // this is in stitches
        double(clothCount)
        QString(clothCountUnits)
        QString(title)
        QString(author)
        QString(copyright)
        QString(fabric)
        QColor(fabricColor) // serialised format has changed over the various versions of qt.
                    // for older versions this appears to be quint8 red, quint8 green quint8 blue, quint8 alpha
        QString(schemeName)
        QString(instructions)

        // palette
        QString(schemeName)
        qint32(current)     // the current color selected
        qint32(colors)      // number of colors in the palette
            qint32(colorIndex)      ]
            QString(flossName)      ] Repeated for each
            qint16(flossSymbol)     ] entry in the palette
            qint8(stitchStrands)        ]
            qint8(backstitchStrands)    ]

        // stitches
        qint32(width)       // width in stitches // use instead of width from the header which might be in inches
        qint32(height)      // height in stitches // use instead of height from the header which might be in inches
            qint8(stitches)                 ]
                qint8(type)     ] Repeated for  ] Repeated for
                qint16(colorIndex)  ] each stitch   ] each stitchqueue

        // knots
        qint32(knots)       // the number of knots
            QPoint(position)    ] Repeated for
            qint16(colorIndex)  ] each knot

        // backstitches
        qint32(backstitches)    // the number of backstitches
            QPoint(start)       ]
            QPoint(end)     ] Repeated for
            qint16(colorIndex)  ] each backstitch
        */

    /* Working */
    // read header
    QString sizeUnits;
    qint32 width;
    qint32 height;
    stream  >> sizeUnits
            >> width
            >> height;

    if (sizeUnits == i18n("Stitches")) {
        setProperty(QStringLiteral("unitsFormat"), Configuration::EnumDocument_UnitsFormat::Stitches);
    }

    if (sizeUnits == i18n("CM")) {
        setProperty(QStringLiteral("unitsFormat"), Configuration::EnumDocument_UnitsFormat::CM);
    }

    if (sizeUnits == i18n("Inches")) {
        setProperty(QStringLiteral("unitsFormat"), Configuration::EnumDocument_UnitsFormat::Inches);
    }

    double clothCount;
    QString clothCountUnits;
    stream  >> clothCount
            >> clothCountUnits;
    setProperty(QStringLiteral("horizontalClothCount"), double(clothCount));
    setProperty(QStringLiteral("verticalClothCount"), double(clothCount));
    setProperty(QStringLiteral("clothCountLink"), true);

    if (clothCountUnits == QLatin1String("/cm")) {
        setProperty(QStringLiteral("clothCountUnits"), Configuration::EnumEditor_ClothCountUnits::CM);
    }

    if (clothCountUnits == QLatin1String("/in")) {
        setProperty(QStringLiteral("clothCountUnits"), Configuration::EnumEditor_ClothCountUnits::Inches);
    }

    QString title;
    QString author;
    QString copyright;
    QString fabric;
    stream  >> title
            >> author
            >> copyright
            >> fabric;
    setProperty(QStringLiteral("title"), title);
    setProperty(QStringLiteral("author"), author);
    setProperty(QStringLiteral("copyright"), copyright);
    setProperty(QStringLiteral("fabric"), fabric);

    quint8 red;
    quint8 green;
    quint8 blue;
    quint8 alpha;
    stream  >> red
            >> green
            >> blue
            >> alpha;
    setProperty(QStringLiteral("fabricColor"), QColor(red, green, blue, alpha));

    QString schemeName;
    stream  >> schemeName;  // discard, also read in palette

    QString instructions;
    stream  >> instructions;
    setProperty(QStringLiteral("instructions"), instructions);

    // read palette
    stream  >> schemeName;
    m_pattern->palette().setSchemeName(schemeName);

    FlossScheme *flossScheme = SchemeManager::scheme(schemeName);

    if (flossScheme == nullptr) {
        throw FailedReadFile(QString(i18n("The floss scheme %1 was not found", schemeName)));
    }

    qint32 current;
    stream  >> current;

    qint32 colors;
    stream  >> colors;

    while (colors--) {
        qint32 colorIndex;
        QString flossName;
        qint16 symbol;
        qint8 stitchStrands;
        qint8 backstitchStrands;

        stream  >> colorIndex
                >> flossName
                >> symbol
                >> stitchStrands
                >> backstitchStrands;

        Floss *floss = flossScheme->find(flossName);

        if (floss == nullptr) {
            throw FailedReadFile(QString(i18n("The floss name %1 was not found", flossName)));
        }

        DocumentFloss *documentFloss = new DocumentFloss(flossName, m_pattern->palette().freeSymbol(), Qt::SolidLine, stitchStrands, backstitchStrands);
        documentFloss->setFlossColor(floss->color());
        m_pattern->palette().add(colorIndex, documentFloss);
    }

    // read stitches
    stream  >> width
            >> height;
    qint32 cells = width * height;
    m_pattern->stitches().resize(width, height);

    for (int i = 0 ; i < cells ; i++) {
        qint8 stitches;
        stream  >> stitches;

        while (stitches--) {
            qint8 type;
            qint16 colorIndex;
            stream  >> type
                    >> colorIndex;

            m_pattern->stitches().addStitch(QPoint(i % width, i / width), Stitch::Type(type), colorIndex);
        }
    }

    qint32 knots;
    stream  >> knots;

    while (knots--) {
        QPoint position;
        qint16 colorIndex;
        stream  >> position
                >> colorIndex;

        m_pattern->stitches().addFrenchKnot(position, colorIndex);
    }

    qint32 backstitches;
    stream  >> backstitches;

    while (backstitches--) {
        QPoint start;
        QPoint end;
        qint16 colorIndex;
        stream  >> start
                >> end
                >> colorIndex;

        m_pattern->stitches().addBackstitch(start, end, colorIndex);
    }

    if (stream.status() != QDataStream::Ok) {
        throw FailedReadFile(QString(i18n("Stream error")));
    }
}


void Document::readKXStitchV7File(QDataStream &stream)
{
    /* File format
        // header
        #QString("KXStitch")        // read in the load function
        #qint16(FILE_FORMAT_VERSION)    // read in the load function

        // properties dialog
        qint32(sizeUnits)
        qint32(width)       // this is in stitches
        qint32(height)      // this is in stitches
        double(clothCount)
        qint32(clothCountUnits)
        QString(title)
        QString(author)
        QString(copyright)
        QString(fabric)
        QColor(fabricColor) // serialised format has changed over the various versions of qt.
                    // for older versions this appears to be quint8 red, quint8 green quint8 blue, quint8 alpha
        QString(schemeName)
        QString(instructions)

        // palette
        QString(schemeName)
        qint32(current)     // the current color selected
        qint32(colors)      // number of colors in the palette
            qint32(colorIndex)      ]
            QString(flossName)      ] Repeated for each
            qint16(flossSymbol)     ] entry in the palette
            qint8(stitchStrands)        ]
            qint8(backstitchStrands)    ]

        // stitches
        qint32(width)       // width in stitches // use instead of width from the header which might be in inches
        qint32(height)      // height in stitches // use instead of height from the header which might be in inches
            qint8(stitches)                 ]
                qint8(type)     ] Repeated for  ] Repeated for
                qint16(colorIndex)  ] each stitch   ] each stitchqueue

        // knots
        qint32(knots)       // the number of knots
            QPoint(position)    ] Repeated for
            qint16(colorIndex)  ] each knot

        // backstitches
        qint32(backstitches)    // the number of backstitches
            QPoint(start)       ]
            QPoint(end)     ] Repeated for
            qint16(colorIndex)  ] each backstitch
        */

    /* Working */
    Configuration::EnumDocument_UnitsFormat::type convertSizeUnits[] = {
        Configuration::EnumDocument_UnitsFormat::Stitches,
        Configuration::EnumDocument_UnitsFormat::CM,        // was MM
        Configuration::EnumDocument_UnitsFormat::CM,
        Configuration::EnumDocument_UnitsFormat::Inches
    };
    Configuration::EnumEditor_ClothCountUnits::type convertClothCountUnits[] = {
        Configuration::EnumEditor_ClothCountUnits::Inches,  // was Stitches
        Configuration::EnumEditor_ClothCountUnits::CM,
        Configuration::EnumEditor_ClothCountUnits::Inches
    };
    // read header
    qint32 sizeUnits;
    qint32 width;
    qint32 height;
    stream  >> sizeUnits
            >> width
            >> height;
    setProperty(QStringLiteral("unitsFormat"), convertSizeUnits[sizeUnits]);

    double clothCount;
    qint32 clothCountUnits;
    stream  >> clothCount
            >> clothCountUnits;
    setProperty(QStringLiteral("horizontalClothCount"), double(clothCount));
    setProperty(QStringLiteral("verticalClothCount"), double(clothCount));
    setProperty(QStringLiteral("clothCountLink"), true);
    setProperty(QStringLiteral("clothCountUnits"), convertClothCountUnits[clothCountUnits]);

    QString title;
    QString author;
    QString copyright;
    QString fabric;
    stream  >> title
            >> author
            >> copyright
            >> fabric;
    setProperty(QStringLiteral("title"), title);
    setProperty(QStringLiteral("author"), author);
    setProperty(QStringLiteral("copyright"), copyright);
    setProperty(QStringLiteral("fabric"), fabric);

    quint8 red;
    quint8 green;
    quint8 blue;
    quint8 alpha;
    stream  >> red
            >> green
            >> blue
            >> alpha;
    setProperty(QStringLiteral("fabricColor"), QColor(red, green, blue, alpha));

    QString schemeName;
    stream  >> schemeName;  // discard, also read in palette

    QString instructions;
    stream  >> instructions;
    setProperty(QStringLiteral("instructions"), instructions);

    // read palette
    stream  >> schemeName;
    m_pattern->palette().setSchemeName(schemeName);

    FlossScheme *flossScheme = SchemeManager::scheme(schemeName);

    if (flossScheme == nullptr) {
        throw FailedReadFile(QString(i18n("The floss scheme %1 was not found", schemeName)));
    }

    qint32 current;
    stream  >> current;

    qint32 colors;
    stream  >> colors;

    while (colors--) {
        qint32 colorIndex;
        QString flossName;
        qint16 symbol;
        qint8 stitchStrands;
        qint8 backstitchStrands;

        stream  >> colorIndex
                >> flossName
                >> symbol
                >> stitchStrands
                >> backstitchStrands;

        Floss *floss = flossScheme->find(flossName);

        if (floss == nullptr) {
            throw FailedReadFile(QString(i18n("The floss name %1 was not found", flossName)));
        }

        DocumentFloss *documentFloss = new DocumentFloss(flossName, m_pattern->palette().freeSymbol(), Qt::SolidLine, stitchStrands, backstitchStrands);
        documentFloss->setFlossColor(floss->color());
        m_pattern->palette().add(colorIndex, documentFloss);
    }

    // read stitches
    stream  >> width
            >> height;
    qint32 cells = width * height;
    m_pattern->stitches().resize(width, height);

    for (int i = 0 ; i < cells ; i++) {
        qint8 stitches;
        stream  >> stitches;

        while (stitches--) {
            qint8 type;
            qint16 colorIndex;
            stream  >> type
                    >> colorIndex;

            m_pattern->stitches().addStitch(QPoint(i % width, i / width), Stitch::Type(type), colorIndex);
        }
    }

    qint32 knots;
    stream  >> knots;

    while (knots--) {
        QPoint position;
        qint16 colorIndex;
        stream  >> position
                >> colorIndex;

        m_pattern->stitches().addFrenchKnot(position, colorIndex);
    }

    qint32 backstitches;
    stream  >> backstitches;

    while (backstitches--) {
        QPoint start;
        QPoint end;
        qint16 colorIndex;
        stream  >> start
                >> end
                >> colorIndex;

        m_pattern->stitches().addBackstitch(start, end, colorIndex);
    }

    if (stream.status() != QDataStream::Ok) {
        throw FailedReadFile(QString(i18n("Stream error")));
    }
}
