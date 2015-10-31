/*
 * Copyright (C) 2010-2015 by Stephen Allewell
 * steve.allewell@gmail.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#include "Commands.h"

#include <QApplication>
#include <QClipboard>
#include <QMimeData>

#include <KLocalizedString>

#include "BackgroundImage.h"
#include "Document.h"
#include "Editor.h"
#include "Floss.h"
#include "FlossScheme.h"
#include "MainWindow.h"
#include "Palette.h"
#include "Preview.h"
#include "SchemeManager.h"
#include "StitchData.h"


FilePropertiesCommand::FilePropertiesCommand(Document *document)
    :   QUndoCommand(i18n("File Properties")),
        m_document(document)
{
}


FilePropertiesCommand::~FilePropertiesCommand()
{
}


void FilePropertiesCommand::redo()
{
    QUndoCommand::redo();
    m_document->editor()->readDocumentSettings();
    m_document->preview()->readDocumentSettings();
    m_document->palette()->update();
}


void FilePropertiesCommand::undo()
{
    QUndoCommand::undo();
    m_document->editor()->readDocumentSettings();
    m_document->preview()->readDocumentSettings();
    m_document->palette()->update();
}


ImportImageCommand::ImportImageCommand(Document *document)
    :   QUndoCommand(i18n("Import Image")),
        m_document(document)
{
}


ImportImageCommand::~ImportImageCommand()
{
}


void ImportImageCommand::redo()
{
    QUndoCommand::redo();
    m_document->editor()->readDocumentSettings();
    m_document->preview()->readDocumentSettings();
    m_document->palette()->update();
}


void ImportImageCommand::undo()
{
    QUndoCommand::undo();
    m_document->editor()->readDocumentSettings();
    m_document->preview()->readDocumentSettings();
    m_document->palette()->update();
}


PaintStitchesCommand::PaintStitchesCommand(Document *document)
    :   QUndoCommand(i18n("Paint Stitches")),
        m_document(document)
{
}


PaintStitchesCommand::~PaintStitchesCommand()
{
}


void PaintStitchesCommand::redo()
{
    QUndoCommand::redo();
    m_document->editor()->drawContents();
    m_document->preview()->drawContents();
}


void PaintStitchesCommand::undo()
{
    QUndoCommand::undo();
    m_document->editor()->drawContents();
    m_document->preview()->drawContents();
}


PaintKnotsCommand::PaintKnotsCommand(Document *document)
    :   QUndoCommand(i18n("Paint Knots")),
        m_document(document)
{
}


PaintKnotsCommand::~PaintKnotsCommand()
{
}


void PaintKnotsCommand::redo()
{
    QUndoCommand::redo();
    m_document->editor()->drawContents();
    m_document->preview()->drawContents();
}


void PaintKnotsCommand::undo()
{
    QUndoCommand::undo();
    m_document->editor()->drawContents();
    m_document->preview()->drawContents();
}


DrawLineCommand::DrawLineCommand(Document *document)
    :   QUndoCommand(i18n("Draw Line")),
        m_document(document)
{
}


DrawLineCommand::~DrawLineCommand()
{
}


void DrawLineCommand::redo()
{
    QUndoCommand::redo();
    m_document->editor()->drawContents();
    m_document->preview()->drawContents();
}


void DrawLineCommand::undo()
{
    QUndoCommand::undo();
    m_document->editor()->drawContents();
    m_document->preview()->drawContents();
}


EraseStitchesCommand::EraseStitchesCommand(Document *document)
    :   QUndoCommand(i18n("Erase Stitches")),
        m_document(document)
{
}


EraseStitchesCommand::~EraseStitchesCommand()
{
}


void EraseStitchesCommand::redo()
{
    QUndoCommand::redo();
    m_document->editor()->drawContents();
    m_document->preview()->drawContents();
}


void EraseStitchesCommand::undo()
{
    QUndoCommand::undo();
    m_document->editor()->drawContents();
    m_document->preview()->drawContents();
}


DrawRectangleCommand::DrawRectangleCommand(Document *document)
    :   QUndoCommand(i18n("Draw Rectangle")),
        m_document(document)
{
}


DrawRectangleCommand::~DrawRectangleCommand()
{
}


void DrawRectangleCommand::redo()
{
    QUndoCommand::redo();
    m_document->editor()->drawContents();
    m_document->preview()->drawContents();
}


void DrawRectangleCommand::undo()
{
    QUndoCommand::undo();
    m_document->editor()->drawContents();
    m_document->preview()->drawContents();
}


FillRectangleCommand::FillRectangleCommand(Document *document)
    :   QUndoCommand(i18n("Fill Rectangle")),
        m_document(document)
{
}


FillRectangleCommand::~FillRectangleCommand()
{
}


void FillRectangleCommand::redo()
{
    QUndoCommand::redo();
    m_document->editor()->drawContents();
    m_document->preview()->drawContents();
}


void FillRectangleCommand::undo()
{
    QUndoCommand::undo();
    m_document->editor()->drawContents();
    m_document->preview()->drawContents();
}



DrawEllipseCommand::DrawEllipseCommand(Document *document)
    :   QUndoCommand(i18n("Draw Ellipse")),
        m_document(document)
{
}


DrawEllipseCommand::~DrawEllipseCommand()
{
}


void DrawEllipseCommand::redo()
{
    QUndoCommand::redo();
    m_document->editor()->drawContents();
    m_document->preview()->drawContents();
}


void DrawEllipseCommand::undo()
{
    QUndoCommand::undo();
    m_document->editor()->drawContents();
    m_document->preview()->drawContents();
}


FillEllipseCommand::FillEllipseCommand(Document *document)
    :   QUndoCommand(i18n("Fill Ellipse")),
        m_document(document)
{
}


FillEllipseCommand::~FillEllipseCommand()
{
}


void FillEllipseCommand::redo()
{
    QUndoCommand::redo();
    m_document->editor()->drawContents();
    m_document->preview()->drawContents();
}


void FillEllipseCommand::undo()
{
    QUndoCommand::undo();
    m_document->editor()->drawContents();
    m_document->preview()->drawContents();
}


FillPolygonCommand::FillPolygonCommand(Document *document)
    :   QUndoCommand(i18n("Fill Polygon")),
        m_document(document)
{
}


FillPolygonCommand::~FillPolygonCommand()
{
}


void FillPolygonCommand::redo()
{
    QUndoCommand::redo();
    m_document->editor()->drawContents();
    m_document->preview()->drawContents();
}


void FillPolygonCommand::undo()
{
    QUndoCommand::undo();
    m_document->editor()->drawContents();
    m_document->preview()->drawContents();
}


AddStitchCommand::AddStitchCommand(Document *document, const QPoint &location, Stitch::Type type, int colorIndex, QUndoCommand *parent)
    :   QUndoCommand(i18n("Add Stitch"), parent),
        m_document(document),
        m_cell(location),
        m_type(type),
        m_colorIndex(colorIndex),
        m_original(0)
{
}


AddStitchCommand::~AddStitchCommand()
{
    delete m_original;
}


void AddStitchCommand::redo()
{
    m_original = m_document->pattern()->stitches().stitchQueueAt(m_cell);

    if (m_original) {
        m_document->pattern()->stitches().replaceStitchQueueAt(m_cell, new StitchQueue(m_original));
    }

    m_document->pattern()->stitches().addStitch(m_cell, m_type, m_colorIndex);
}


void AddStitchCommand::undo()
{
    delete m_document->pattern()->stitches().takeStitchQueueAt(m_cell);

    if (m_original) {
        m_document->pattern()->stitches().replaceStitchQueueAt(m_cell, m_original);
        m_original = 0;
    }
}


DeleteStitchCommand::DeleteStitchCommand(Document *document, const QPoint &cell, Stitch::Type type, int colorIndex, QUndoCommand *parent)
    :   QUndoCommand(i18n("Delete Stitches"), parent),
        m_document(document),
        m_cell(cell),
        m_type(type),
        m_colorIndex(colorIndex),
        m_original(0)
{
}


DeleteStitchCommand::~DeleteStitchCommand()
{
    delete m_original;
}


void DeleteStitchCommand::redo()
{
    m_original = m_document->pattern()->stitches().stitchQueueAt(m_cell);

    if (m_original) {
        m_original = m_document->pattern()->stitches().replaceStitchQueueAt(m_cell, new StitchQueue(m_original));
        m_document->pattern()->stitches().deleteStitch(m_cell, m_type, m_colorIndex);
    }
}


void DeleteStitchCommand::undo()
{
    if (m_original) {
        delete m_document->pattern()->stitches().replaceStitchQueueAt(m_cell, m_original);
        m_original = 0;
    }
}


AddBackstitchCommand::AddBackstitchCommand(Document *document, const QPoint &start, const QPoint &end, int colorIndex)
    :   QUndoCommand(i18n("Add Backstitch")),
        m_document(document),
        m_start(start),
        m_end(end),
        m_colorIndex(colorIndex)
{
}


AddBackstitchCommand::~AddBackstitchCommand()
{
}


void AddBackstitchCommand::redo()
{
    m_document->pattern()->stitches().addBackstitch(m_start, m_end, m_colorIndex);
    m_document->editor()->drawContents();
    m_document->preview()->drawContents();
}


void AddBackstitchCommand::undo()
{
    delete m_document->pattern()->stitches().takeBackstitch(m_start, m_end, m_colorIndex);
    m_document->editor()->drawContents();
    m_document->preview()->drawContents();
}


DeleteBackstitchCommand::DeleteBackstitchCommand(Document *document, const QPoint &start, const QPoint &end, int colorIndex)
    :   QUndoCommand(i18n("Delete Backstitch")),
        m_document(document),
        m_start(start),
        m_end(end),
        m_colorIndex(colorIndex),
        m_backstitch(0)
{
}


DeleteBackstitchCommand::~DeleteBackstitchCommand()
{
    delete m_backstitch;
}


void DeleteBackstitchCommand::redo()
{
    m_backstitch = m_document->pattern()->stitches().takeBackstitch(m_start, m_end, m_colorIndex);
    m_document->editor()->drawContents();
    m_document->preview()->drawContents();
}


void DeleteBackstitchCommand::undo()
{
    m_document->pattern()->stitches().addBackstitch(m_backstitch);
    m_backstitch = 0;
    m_document->editor()->drawContents();
    m_document->preview()->drawContents();
}


AddKnotCommand::AddKnotCommand(Document *document, const QPoint &snap, int colorIndex, QUndoCommand *parent)
    :   QUndoCommand(i18n("Add Knot"), parent),
        m_document(document),
        m_snap(snap),
        m_colorIndex(colorIndex)
{
}


AddKnotCommand::~AddKnotCommand()
{
}


void AddKnotCommand::redo()
{
    m_document->pattern()->stitches().addFrenchKnot(m_snap, m_colorIndex);
}


void AddKnotCommand::undo()
{
    delete m_document->pattern()->stitches().takeFrenchKnot(m_snap, m_colorIndex);
}


DeleteKnotCommand::DeleteKnotCommand(Document *document, const QPoint &snap, int colorIndex, QUndoCommand *parent)
    :   QUndoCommand(i18n("Delete Knots"), parent),
        m_document(document),
        m_snap(snap),
        m_colorIndex(colorIndex),
        m_knot(0)
{
}


DeleteKnotCommand::~DeleteKnotCommand()
{
    delete m_knot;
}


void DeleteKnotCommand::redo()
{
    m_knot = m_document->pattern()->stitches().takeFrenchKnot(m_snap, m_colorIndex);
}


void DeleteKnotCommand::undo()
{
    m_document->pattern()->stitches().addFrenchKnot(m_knot);
    m_knot = 0;
}


SetPropertyCommand::SetPropertyCommand(Document *document, const QString &name, const QVariant &value, QUndoCommand *parent)
    :   QUndoCommand(i18n("Set Property"), parent),
        m_document(document),
        m_name(name),
        m_value(value)
{
}


SetPropertyCommand::~SetPropertyCommand()
{
    // nothing needs to be done here
}


void SetPropertyCommand::redo()
{
    m_oldValue = m_document->property(m_name);
    m_document->setProperty(m_name, m_value);
}


void SetPropertyCommand::undo()
{
    m_document->setProperty(m_name, m_oldValue);
}


AddBackgroundImageCommand::AddBackgroundImageCommand(Document *document, BackgroundImage *backgroundImage, MainWindow *mainWindow)
    :   QUndoCommand(i18n("Add Background Image")),
        m_document(document),
        m_backgroundImage(backgroundImage),
        m_mainWindow(mainWindow)
{
}


AddBackgroundImageCommand::~AddBackgroundImageCommand()
{
}


void AddBackgroundImageCommand::redo()
{
    m_document->backgroundImages().addBackgroundImage(m_backgroundImage);
    m_mainWindow->updateBackgroundImageActionLists();
    m_document->editor()->drawContents();
}


void AddBackgroundImageCommand::undo()
{
    m_document->backgroundImages().removeBackgroundImage(m_backgroundImage);
    m_mainWindow->updateBackgroundImageActionLists();
    m_document->editor()->drawContents();
}


FitBackgroundImageCommand::FitBackgroundImageCommand(Document *document, BackgroundImage *backgroundImage, const QRect &rect)
    :   QUndoCommand(i18n("Fit Background to Selection")),
        m_document(document),
        m_backgroundImage(backgroundImage),
        m_rect(rect)
{
}


FitBackgroundImageCommand::~FitBackgroundImageCommand()
{
}


void FitBackgroundImageCommand::redo()
{
    m_rect = m_document->backgroundImages().fitBackgroundImage(m_backgroundImage, m_rect);
    m_document->editor()->resetSelectionArea();
    m_document->editor()->drawContents();
}


void FitBackgroundImageCommand::undo()
{
    redo(); // same code required
}


ShowBackgroundImageCommand::ShowBackgroundImageCommand(Document *document, BackgroundImage *backgroundImage, bool visible)
    :   QUndoCommand(i18n("Show Background Image")),
        m_document(document),
        m_backgroundImage(backgroundImage),
        m_visible(visible)
{
}


ShowBackgroundImageCommand::~ShowBackgroundImageCommand()
{
}


void ShowBackgroundImageCommand::redo()
{
    m_visible = m_document->backgroundImages().showBackgroundImage(m_backgroundImage, m_visible);
    m_document->editor()->drawContents();
}


void ShowBackgroundImageCommand::undo()
{
    redo(); // same code required
}


RemoveBackgroundImageCommand::RemoveBackgroundImageCommand(Document *document, BackgroundImage *backgroundImage, MainWindow *mainWindow)
    :   QUndoCommand(i18n("Remove Background Image")),
        m_document(document),
        m_backgroundImage(backgroundImage),
        m_mainWindow(mainWindow)
{
}


RemoveBackgroundImageCommand::~RemoveBackgroundImageCommand()
{
    // delete m_backgroundImage;
    // m_backgroundImage may also be deleted by the document, potential for a crash or memory leak
}


void RemoveBackgroundImageCommand::redo()
{
    m_document->backgroundImages().removeBackgroundImage(m_backgroundImage);
    m_mainWindow->updateBackgroundImageActionLists();

    if (m_backgroundImage->isVisible()) {
        m_document->editor()->drawContents();
    }
}


void RemoveBackgroundImageCommand::undo()
{
    m_document->backgroundImages().addBackgroundImage(m_backgroundImage);
    m_mainWindow->updateBackgroundImageActionLists();

    if (m_backgroundImage->isVisible()) {
        m_document->editor()->drawContents();
    }
}


AddDocumentFlossCommand::AddDocumentFlossCommand(Document *document, int key, DocumentFloss *documentFloss, QUndoCommand *parent)
    :   QUndoCommand(parent),
        m_document(document),
        m_key(key),
        m_documentFloss(documentFloss)
{
}


AddDocumentFlossCommand::~AddDocumentFlossCommand()
{
}


void AddDocumentFlossCommand::redo()
{
    m_document->pattern()->palette().add(m_key, m_documentFloss);
}


void AddDocumentFlossCommand::undo()
{
    m_document->pattern()->palette().remove(m_key);
}


RemoveDocumentFlossCommand::RemoveDocumentFlossCommand(Document *document, int key, DocumentFloss *documentFloss, QUndoCommand *parent)
    :   QUndoCommand(parent),
        m_document(document),
        m_key(key),
        m_documentFloss(documentFloss)
{
}


RemoveDocumentFlossCommand::~RemoveDocumentFlossCommand()
{
    delete m_documentFloss;
}


void RemoveDocumentFlossCommand::redo()
{
    m_document->pattern()->palette().remove(m_key);
}


void RemoveDocumentFlossCommand::undo()
{
    m_document->pattern()->palette().add(m_key, m_documentFloss);
}


ReplaceDocumentFlossCommand::ReplaceDocumentFlossCommand(Document *document, int key, DocumentFloss *documentFloss)
    :   QUndoCommand(),
        m_document(document),
        m_key(key),
        m_documentFloss(documentFloss)
{
}


ReplaceDocumentFlossCommand::~ReplaceDocumentFlossCommand()
{
    delete m_documentFloss;
}


void ReplaceDocumentFlossCommand::redo()
{
    m_documentFloss = m_document->pattern()->palette().replace(m_key, m_documentFloss);
}


void ReplaceDocumentFlossCommand::undo()
{
    redo(); // same code required
}


ClearUnusedFlossesCommand::ClearUnusedFlossesCommand(Document *document)
    :   QUndoCommand(i18n("Clear Unused Flosses")),
        m_document(document)
{
}


ClearUnusedFlossesCommand::~ClearUnusedFlossesCommand()
{
}


void ClearUnusedFlossesCommand::redo()
{
    QUndoCommand::redo();
    m_document->palette()->update();
}


void ClearUnusedFlossesCommand::undo()
{
    QUndoCommand::undo();
    m_document->palette()->update();
}


ResizeDocumentCommand::ResizeDocumentCommand(Document *document, int width, int height, QUndoCommand *parent)
    :   QUndoCommand(i18n("Resize Document"), parent),
        m_document(document),
        m_width(width),
        m_height(height)
{
}


ResizeDocumentCommand::~ResizeDocumentCommand()
{
}


void ResizeDocumentCommand::redo()
{
    m_originalWidth = m_document->pattern()->stitches().width();
    m_originalHeight = m_document->pattern()->stitches().height();
    QRect extents = m_document->pattern()->stitches().extents();
    int minx = std::min(extents.left(), m_width - extents.width());
    m_xOffset = minx - extents.left();
    int miny = std::min(extents.top(), m_height - extents.height());
    m_yOffset = miny - extents.top();
    m_document->pattern()->stitches().movePattern(m_xOffset, m_yOffset);
    m_document->pattern()->stitches().resize(m_width, m_height);
}


void ResizeDocumentCommand::undo()
{
    m_document->pattern()->stitches().resize(m_originalWidth, m_originalHeight);
    m_document->pattern()->stitches().movePattern(-m_xOffset, -m_yOffset);
}


CropToPatternCommand::CropToPatternCommand(Document *document)
    :   QUndoCommand(i18n("Crop to Pattern")),
        m_document(document)
{
}


CropToPatternCommand::~CropToPatternCommand()
{
}


void CropToPatternCommand::redo()
{
    m_originalWidth = m_document->pattern()->stitches().width();
    m_originalHeight = m_document->pattern()->stitches().height();
    QRect extents = m_document->pattern()->stitches().extents();
    m_xOffset = -extents.left();
    m_yOffset = -extents.top();
    m_document->pattern()->stitches().movePattern(m_xOffset, m_yOffset);
    m_document->pattern()->stitches().resize(extents.width(), extents.height());
    m_document->editor()->readDocumentSettings();
    m_document->preview()->readDocumentSettings();
}


void CropToPatternCommand::undo()
{
    m_document->pattern()->stitches().resize(m_originalWidth, m_originalHeight);
    m_document->pattern()->stitches().movePattern(-m_xOffset, -m_yOffset);
    m_document->editor()->readDocumentSettings();
    m_document->preview()->readDocumentSettings();
}


CropToSelectionCommand::CropToSelectionCommand(Document *document, const QRect &selectionArea)
    :   QUndoCommand(i18n("Crop to Selection")),
        m_document(document),
        m_selectionArea(selectionArea)
{
}


CropToSelectionCommand::~CropToSelectionCommand()
{
}


void CropToSelectionCommand::redo()
{
    QList<Stitch::Type> maskStitches;
    maskStitches << Stitch::TLQtr << Stitch::TRQtr << Stitch::BLQtr << Stitch::BTHalf << Stitch::TL3Qtr << Stitch::BRQtr
                 << Stitch::TBHalf << Stitch::TR3Qtr << Stitch::BL3Qtr << Stitch::BR3Qtr << Stitch::Full << Stitch::TLSmallHalf
                 << Stitch::TRSmallHalf << Stitch::BLSmallHalf << Stitch::BRSmallHalf << Stitch::TLSmallFull << Stitch::TRSmallFull
                 << Stitch::BLSmallFull << Stitch::BRSmallFull;

    QDataStream stream(&m_originalPattern, QIODevice::WriteOnly);
    stream << m_document->pattern()->stitches();

    Pattern *pattern = m_document->pattern()->copy(m_selectionArea, -1, maskStitches, false, false);
    m_document->pattern()->stitches().clear();
    m_document->pattern()->stitches().resize(m_selectionArea.width(), m_selectionArea.height());
    m_document->pattern()->paste(pattern, QPoint(0, 0), true);
    delete pattern;

    m_document->editor()->readDocumentSettings();
    m_document->preview()->readDocumentSettings();
}


void CropToSelectionCommand::undo()
{
    QDataStream stream(&m_originalPattern, QIODevice::ReadOnly);
    stream >> m_document->pattern()->stitches();
    m_originalPattern.clear();

    m_document->editor()->readDocumentSettings();
    m_document->preview()->readDocumentSettings();
}


InsertColumnsCommand::InsertColumnsCommand(Document *document, const QRect &selectionArea)
    :   QUndoCommand(i18n("Insert Columns")),
        m_document(document),
        m_selectionArea(selectionArea)
{
}


InsertColumnsCommand::~InsertColumnsCommand()
{
}


void InsertColumnsCommand::redo()
{
    m_document->pattern()->stitches().insertColumns(m_selectionArea.left(), m_selectionArea.width());

    QListIterator<BackgroundImage *> backgroundImageIterator = m_document->backgroundImages().backgroundImages();

    while (backgroundImageIterator.hasNext()) {
        BackgroundImage *backgroundImage = backgroundImageIterator.next();

        if (backgroundImage->location().left() >= m_selectionArea.left()) {
            backgroundImage->setLocation(backgroundImage->location().translated(m_selectionArea.width(), 0));
        }
    }

    m_document->editor()->readDocumentSettings();
    m_document->preview()->readDocumentSettings();
}


void InsertColumnsCommand::undo()
{
    m_document->pattern()->stitches().removeColumns(m_selectionArea.left(), m_selectionArea.width());

    QListIterator<BackgroundImage *> backgroundImageIterator = m_document->backgroundImages().backgroundImages();

    while (backgroundImageIterator.hasNext()) {
        BackgroundImage *backgroundImage = backgroundImageIterator.next();

        if (backgroundImage->location().left() > m_selectionArea.left()) {
            backgroundImage->setLocation(backgroundImage->location().translated(-m_selectionArea.width(), 0));
        }
    }

    m_document->editor()->readDocumentSettings();
    m_document->preview()->readDocumentSettings();
}


InsertRowsCommand::InsertRowsCommand(Document *document, const QRect &selectionArea)
    :   QUndoCommand(i18n("Insert Rows")),
        m_document(document),
        m_selectionArea(selectionArea)
{
}


InsertRowsCommand::~InsertRowsCommand()
{
}


void InsertRowsCommand::redo()
{
    m_document->pattern()->stitches().insertRows(m_selectionArea.top(), m_selectionArea.height());

    QListIterator<BackgroundImage *> backgroundImageIterator = m_document->backgroundImages().backgroundImages();

    while (backgroundImageIterator.hasNext()) {
        BackgroundImage *backgroundImage = backgroundImageIterator.next();

        if (backgroundImage->location().top() >= m_selectionArea.top()) {
            backgroundImage->setLocation(backgroundImage->location().translated(0, m_selectionArea.height()));
        }
    }

    m_document->editor()->readDocumentSettings();
    m_document->preview()->readDocumentSettings();
}


void InsertRowsCommand::undo()
{
    m_document->pattern()->stitches().removeRows(m_selectionArea.top(), m_selectionArea.height());

    QListIterator<BackgroundImage *> backgroundImageIterator = m_document->backgroundImages().backgroundImages();

    while (backgroundImageIterator.hasNext()) {
        BackgroundImage *backgroundImage = backgroundImageIterator.next();

        if (backgroundImage->location().top() > m_selectionArea.top()) {
            backgroundImage->setLocation(backgroundImage->location().translated(0, -m_selectionArea.height()));
        }
    }

    m_document->editor()->readDocumentSettings();
    m_document->preview()->readDocumentSettings();
}


ExtendPatternCommand::ExtendPatternCommand(Document *document, int top, int left, int right, int bottom)
    :   QUndoCommand(i18n("Extend Pattern")),
        m_document(document),
        m_top(top),
        m_left(left),
        m_right(right),
        m_bottom(bottom)
{
}


ExtendPatternCommand::~ExtendPatternCommand()
{
}


void ExtendPatternCommand::redo()
{
    StitchData &stitchData = m_document->pattern()->stitches();
    stitchData.resize(stitchData.width() + m_left + m_right, stitchData.height() + m_top + m_bottom);
    stitchData.movePattern(m_left, m_top);
    QListIterator<BackgroundImage *> backgroundImageIterator = m_document->backgroundImages().backgroundImages();

    while (backgroundImageIterator.hasNext()) {
        BackgroundImage *backgroundImage = backgroundImageIterator.next();
        backgroundImage->setLocation(backgroundImage->location().translated(m_left, m_top));
    }

    m_document->editor()->readDocumentSettings();
    m_document->preview()->readDocumentSettings();
}


void ExtendPatternCommand::undo()
{
    StitchData &stitchData = m_document->pattern()->stitches();
    stitchData.movePattern(-m_left, -m_top);
    stitchData.resize(stitchData.width() - m_left - m_right, stitchData.height() - m_top - m_bottom);
    QListIterator<BackgroundImage *> backgroundImageIterator = m_document->backgroundImages().backgroundImages();

    while (backgroundImageIterator.hasNext()) {
        BackgroundImage *backgroundImage = backgroundImageIterator.next();
        backgroundImage->setLocation(backgroundImage->location().translated(-m_left, -m_top));
    }

    m_document->editor()->readDocumentSettings();
    m_document->preview()->readDocumentSettings();
}


CentrePatternCommand::CentrePatternCommand(Document *document)
    :   QUndoCommand(i18n("Center Pattern")),
        m_document(document)
{
}


CentrePatternCommand::~CentrePatternCommand()
{
}


void CentrePatternCommand::redo()
{
    QRect extents = m_document->pattern()->stitches().extents();

    m_xOffset = ((m_document->pattern()->stitches().width() - extents.width()) / 2) - extents.left();
    m_yOffset = ((m_document->pattern()->stitches().height() - extents.height()) / 2) - extents.top();

    if (m_xOffset || m_yOffset) {
        m_document->pattern()->stitches().movePattern(m_xOffset, m_yOffset);

        m_document->editor()->drawContents();
        m_document->preview()->drawContents();
    }
}



void CentrePatternCommand::undo()
{
    if (m_xOffset || m_yOffset) {
        m_document->pattern()->stitches().movePattern(-m_xOffset, -m_yOffset);

        m_document->editor()->drawContents();
        m_document->preview()->drawContents();
    }
}


UpdateDocumentPaletteCommand::UpdateDocumentPaletteCommand(Document *document, const DocumentPalette &palette)
    :   QUndoCommand(i18n("Update Palette")),
        m_document(document),
        m_palette(palette)
{
}


UpdateDocumentPaletteCommand::~UpdateDocumentPaletteCommand()
{
}


void UpdateDocumentPaletteCommand::redo()
{
    DocumentPalette palette = m_document->pattern()->palette();
    m_document->pattern()->palette() = m_palette;
    m_palette = palette;

    m_document->editor()->drawContents();
    m_document->preview()->drawContents();
    m_document->palette()->update();
}


void UpdateDocumentPaletteCommand::undo()
{
    redo(); // swaps the palette back
}


ChangeSchemeCommand::ChangeSchemeCommand(Document *document, const QString &schemeName, QUndoCommand *parent)
    :   QUndoCommand(i18n("Change Floss Scheme"), parent),
        m_document(document),
        m_schemeName(schemeName)
{
}


ChangeSchemeCommand::~ChangeSchemeCommand()
{
}


void ChangeSchemeCommand::redo()
{
    QDataStream stream(&m_originalPalette, QIODevice::WriteOnly);
    stream << m_document->pattern()->palette();
    m_document->pattern()->palette().setSchemeName(m_schemeName);

    m_document->editor()->drawContents();
    m_document->preview()->drawContents();
    m_document->palette()->update();
}


void ChangeSchemeCommand::undo()
{
    QDataStream stream(&m_originalPalette, QIODevice::ReadOnly);
    stream >> m_document->pattern()->palette();
    m_originalPalette.clear();

    m_document->editor()->drawContents();
    m_document->preview()->drawContents();
    m_document->palette()->update();
}


EditorReadDocumentSettingsCommand::EditorReadDocumentSettingsCommand(Editor *editor)
    :   QUndoCommand(),
        m_editor(editor)
{
}


EditorReadDocumentSettingsCommand::~EditorReadDocumentSettingsCommand()
{
}

void EditorReadDocumentSettingsCommand::redo()
{
    m_editor->readDocumentSettings();
}


void EditorReadDocumentSettingsCommand::undo()
{
    redo(); // same code required
}


PreviewReadDocumentSettingsCommand::PreviewReadDocumentSettingsCommand(Preview *preview)
    :   QUndoCommand(),
        m_preview(preview)
{
}


PreviewReadDocumentSettingsCommand::~PreviewReadDocumentSettingsCommand()
{
}


void PreviewReadDocumentSettingsCommand::redo()
{
    m_preview->readDocumentSettings();
}


void PreviewReadDocumentSettingsCommand::undo()
{
    redo(); // same code required
}


PaletteReplaceColorCommand::PaletteReplaceColorCommand(Document *document, int originalIndex, int replacementIndex)
    :   QUndoCommand(i18n("Replace Color")),
        m_document(document),
        m_originalIndex(originalIndex),
        m_replacementIndex(replacementIndex)
{
}


PaletteReplaceColorCommand::~PaletteReplaceColorCommand()
{
}


void PaletteReplaceColorCommand::redo()
{
    if (m_stitches.count() || m_backstitches.count() || m_knots.count()) {
        // populated from a previous redo call
        // iterator over the existing pointers
        QListIterator<Stitch *> stitchIterator(m_stitches);

        while (stitchIterator.hasNext()) {
            stitchIterator.next()->colorIndex = m_replacementIndex;
        }

        QListIterator<Backstitch *> backstitchIterator(m_backstitches);

        while (backstitchIterator.hasNext()) {
            backstitchIterator.next()->colorIndex = m_replacementIndex;
        }

        QListIterator<Knot *> knotIterator(m_knots);

        while (knotIterator.hasNext()) {
            knotIterator.next()->colorIndex = m_replacementIndex;
        }
    } else {
        // search the stitch data for stitches of the required color
        StitchData &stitchData = m_document->pattern()->stitches();

        for (int row = 0 ; row < stitchData.height() ; ++row) {
            for (int col = 0 ; col < stitchData.width() ; ++col) {
                StitchQueue *queue = stitchData.stitchQueueAt(QPoint(col, row));

                if (queue) {
                    QListIterator<Stitch *> stitchIterator(*queue);

                    while (stitchIterator.hasNext()) {
                        Stitch *stitch = stitchIterator.next();

                        if (stitch->colorIndex == m_originalIndex) {
                            m_stitches.append(stitch);
                            stitch->colorIndex = m_replacementIndex;
                        }
                    }
                }
            }
        }

        QListIterator<Backstitch *> backstitchIterator = stitchData.backstitchIterator();

        while (backstitchIterator.hasNext()) {
            Backstitch *backstitch = backstitchIterator.next();

            if (backstitch->colorIndex == m_originalIndex) {
                m_backstitches.append(backstitch);
                backstitch->colorIndex = m_replacementIndex;
            }
        }

        QListIterator<Knot *> knotIterator = stitchData.knotIterator();

        while (knotIterator.hasNext()) {
            Knot *knot = knotIterator.next();

            if (knot->colorIndex == m_originalIndex) {
                m_knots.append(knot);
                knot->colorIndex = m_replacementIndex;
            }
        }
    }

    m_document->editor()->drawContents();
    m_document->preview()->drawContents();
    m_document->palette()->update();
}


void PaletteReplaceColorCommand::undo()
{
    QListIterator<Stitch *> stitchIterator(m_stitches);

    while (stitchIterator.hasNext()) {
        stitchIterator.next()->colorIndex = m_originalIndex;
    }

    QListIterator<Backstitch *> backstitchIterator(m_backstitches);

    while (backstitchIterator.hasNext()) {
        backstitchIterator.next()->colorIndex = m_originalIndex;
    }

    QListIterator<Knot *> knotIterator(m_knots);

    while (knotIterator.hasNext()) {
        knotIterator.next()->colorIndex = m_originalIndex;
    }

    m_document->editor()->drawContents();
    m_document->preview()->drawContents();
    m_document->palette()->update();
}


PaletteSwapColorCommand::PaletteSwapColorCommand(Document *document, int originalIndex, int swappedIndex)
    :   QUndoCommand(i18n("Swap Colors")),
        m_document(document),
        m_originalIndex(originalIndex),
        m_swappedIndex(swappedIndex)
{
}


PaletteSwapColorCommand::~PaletteSwapColorCommand()
{
}


void PaletteSwapColorCommand::redo()
{
    m_document->pattern()->palette().swap(m_originalIndex, m_swappedIndex);
    m_document->editor()->drawContents();
    m_document->preview()->drawContents();
    m_document->palette()->update();
}


void PaletteSwapColorCommand::undo()
{
    redo();
}


UpdatePrinterConfigurationCommand::UpdatePrinterConfigurationCommand(Document *document, const PrinterConfiguration &printerConfiguration)
    :   QUndoCommand(i18n("Update Printer Configuration")),
        m_document(document),
        m_printerConfiguration(printerConfiguration)
{
}


UpdatePrinterConfigurationCommand::~UpdatePrinterConfigurationCommand()
{
}


void UpdatePrinterConfigurationCommand::redo()
{
    PrinterConfiguration original = m_document->printerConfiguration();
    m_document->setPrinterConfiguration(m_printerConfiguration);
    m_printerConfiguration = original;
}


void UpdatePrinterConfigurationCommand::undo()
{
    redo();
}


EditCutCommand::EditCutCommand(Document *document, const QRect &selectionArea, int colorMask, const QList<Stitch::Type> &stitchMasks, bool excludeBackstitches, bool excludeKnots)
    :   QUndoCommand("Cut"),
        m_document(document),
        m_selectionArea(selectionArea),
        m_colorMask(colorMask),
        m_stitchMasks(stitchMasks),
        m_excludeBackstitches(excludeBackstitches),
        m_excludeKnots(excludeKnots),
        m_originalPattern(0)
{
}


EditCutCommand::~EditCutCommand()
{
    delete m_originalPattern;
}


void EditCutCommand::redo()
{
    m_originalPattern = m_document->pattern()->cut(m_selectionArea, m_colorMask, m_stitchMasks, m_excludeBackstitches, m_excludeKnots);

    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream << *m_originalPattern;

    QMimeData *mimeData = new QMimeData();
    mimeData->setData("application/kxstitch", data);

    QApplication::clipboard()->setMimeData(mimeData);

    m_document->editor()->drawContents();
    m_document->preview()->drawContents();
}


void EditCutCommand::undo()
{
    m_document->pattern()->paste(m_originalPattern, m_selectionArea.topLeft(), true);
    delete m_originalPattern;
    m_originalPattern = 0;

    m_document->editor()->drawContents();
    m_document->preview()->drawContents();
}


EditPasteCommand::EditPasteCommand(Document *document, Pattern *pattern, const QPoint &cell, bool merge, const QString &source)
    :   QUndoCommand(source),
        m_document(document),
        m_pastePattern(pattern),
        m_cell(cell),
        m_merge(merge)
{
}


EditPasteCommand::~EditPasteCommand()
{
}


void EditPasteCommand::redo()
{
    QDataStream stream(&m_originalPattern, QIODevice::WriteOnly);
    stream << *(m_document->pattern());
    m_document->pattern()->paste(m_pastePattern, m_cell, m_merge);

    m_document->editor()->drawContents();
    m_document->preview()->drawContents();
    m_document->palette()->update();
}


void EditPasteCommand::undo()
{
    QDataStream stream(&m_originalPattern, QIODevice::ReadOnly);
    m_document->pattern()->clear();
    stream >> *(m_document->pattern());
    m_originalPattern.clear();

    m_document->editor()->drawContents();
    m_document->preview()->drawContents();
    m_document->palette()->update();
}


MirrorSelectionCommand::MirrorSelectionCommand(Document *document, const QRect &selectionArea, int colorMask, const QList<Stitch::Type> &stitchMasks, bool excludeBackstitches, bool excludeKnots, Qt::Orientation orientation, bool copies, const QByteArray &originalPatternData, Pattern *invertedPattern, const QPoint &pasteCell, bool merge)
    :   QUndoCommand(i18n("Mirror Selection")),
        m_document(document),
        m_selectionArea(selectionArea),
        m_colorMask(colorMask),
        m_stitchMasks(stitchMasks),
        m_excludeBackstitches(excludeBackstitches),
        m_excludeKnots(excludeKnots),
        m_orientation(orientation),
        m_copies(copies),
        m_originalPatternData(originalPatternData),
        m_invertedPattern(invertedPattern),
        m_pasteCell(pasteCell),
        m_merge(merge)
{
}


MirrorSelectionCommand::~MirrorSelectionCommand()
{
    delete m_invertedPattern;
}


void MirrorSelectionCommand::redo()
{
    if (!m_copies) {
        delete m_document->pattern()->cut(m_selectionArea, m_colorMask, m_stitchMasks, m_excludeBackstitches, m_excludeKnots);
    }

    m_document->pattern()->paste(m_invertedPattern, m_pasteCell, m_merge);

    m_document->editor()->drawContents();
    m_document->preview()->drawContents();
}


void MirrorSelectionCommand::undo()
{
    m_document->pattern()->stitches().clear();
    QDataStream stream(&m_originalPatternData, QIODevice::ReadOnly);
    stream >> m_document->pattern()->stitches();

    m_document->editor()->drawContents();
    m_document->preview()->drawContents();
}


RotateSelectionCommand::RotateSelectionCommand(Document *document, const QRect &selectionArea, int colorMask, const QList<Stitch::Type> &stitchMasks, bool excludeBackstitches, bool excludeKnots, StitchData::Rotation rotation, bool copies, const QByteArray &originalPatternData, Pattern *rotatedPattern, const QPoint &pasteCell, bool merge)
    :   QUndoCommand(i18n("Rotate Selection")),
        m_document(document),
        m_selectionArea(selectionArea),
        m_colorMask(colorMask),
        m_stitchMasks(stitchMasks),
        m_excludeBackstitches(excludeBackstitches),
        m_excludeKnots(excludeKnots),
        m_rotation(rotation),
        m_copies(copies),
        m_originalPatternData(originalPatternData),
        m_rotatedPattern(rotatedPattern),
        m_pasteCell(pasteCell),
        m_merge(merge)
{
}


RotateSelectionCommand::~RotateSelectionCommand()
{
    delete m_rotatedPattern;
}


void RotateSelectionCommand::redo()
{
    if (!m_copies) {
        delete m_document->pattern()->cut(m_selectionArea, m_colorMask, m_stitchMasks, m_excludeBackstitches, m_excludeKnots);
    }

    m_document->pattern()->paste(m_rotatedPattern, m_pasteCell, m_merge);

    m_document->editor()->drawContents();
    m_document->preview()->drawContents();
}


void RotateSelectionCommand::undo()
{
    m_document->pattern()->stitches().clear();
    QDataStream stream(&m_originalPatternData, QIODevice::ReadOnly);
    stream >> m_document->pattern()->stitches();

    m_document->editor()->drawContents();
    m_document->preview()->drawContents();
}


AlphabetCommand::AlphabetCommand(Document *document)
    :   QUndoCommand(i18n("Alphabet")),
        m_document(document)
{
}


AlphabetCommand::~AlphabetCommand()
{
    qDeleteAll(m_children);
}


void AlphabetCommand::redo()
{
    for (int i = 0 ; i < m_children.size() ; ++i) {
        m_children.at(i)->redo();
    }
}


void AlphabetCommand::undo()
{
    for (int i = m_children.size() - 1 ; i >= 0 ; --i) {
        m_children.at(i)->undo();
    }
}


void AlphabetCommand::push(QUndoCommand *child)
{
    m_children.append(child);
    child->redo();
}


QUndoCommand *AlphabetCommand::pop()
{
    if (m_children.isEmpty()) {
        return 0;
    }

    m_children.last()->undo();
    return m_children.takeLast();
}


int AlphabetCommand::childCount() const
{
    return m_children.count();
}


ConfigurationCommand::ConfigurationCommand(MainWindow *mainWindow)
    :   QUndoCommand(i18n("Configure KXStitch")),
        m_mainWindow(mainWindow)
{
}


ConfigurationCommand::~ConfigurationCommand()
{
}


void ConfigurationCommand::redo()
{
    QUndoCommand::redo();
    m_mainWindow->loadSettings();
}


void ConfigurationCommand::undo()
{
    QUndoCommand::undo();
    m_mainWindow->loadSettings();
}
