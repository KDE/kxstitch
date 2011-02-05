/****************************************************************************
 *	Copyright (C) 2010 by Stephen Allewell									*
 *	stephen@mirramar.adsl24.co.uk											*
 *																			*
 *	This program is free software; you can redistribute it and/or modify	*
 *	it under the terms of the GNU General Public License as published by	*
 *	the Free Software Foundation; either version 2 of the License, or		*
 *	(at your option) any later version.										*
 ****************************************************************************/


#include <KDebug>
#include <KLocale>

#include "backgroundimage.h"
#include "commands.h"
#include "document.h"
#include "floss.h"


AddStitchCommand::AddStitchCommand(Document *document, const QPoint &cell, Stitch::Type type, int floss)
	:	QUndoCommand(),
		m_document(document),
		m_cell(cell),
		m_type(type),
		m_floss(floss),
		m_original(0)
{
}


AddStitchCommand::~AddStitchCommand()
{
	delete m_original;
}


void AddStitchCommand::redo()
{
	m_original = m_document->stitchAt(m_cell);
	if (m_original)
		m_document->replaceStitchAt(m_cell, new StitchQueue(m_original));
	m_document->addStitch(m_cell, m_type, m_floss);
}


void AddStitchCommand::undo()
{
	delete m_document->replaceStitchAt(m_cell, m_original);
	m_original = 0;
}


DeleteStitchCommand::DeleteStitchCommand(Document *document, const QPoint &cell, Stitch::Type type, int floss)
	:	QUndoCommand(),
		m_document(document),
		m_cell(cell),
		m_type(type),
		m_floss(floss),
		m_original(0)
{
}


DeleteStitchCommand::~DeleteStitchCommand()
{
	delete m_original;
}


void DeleteStitchCommand::redo()
{
	m_original = m_document->stitchAt(m_cell);
	if (m_original)
		m_document->replaceStitchAt(m_cell, new StitchQueue(m_original));
	m_document->deleteStitch(m_cell, m_type, m_floss);
}


void DeleteStitchCommand::undo()
{
	delete m_document->replaceStitchAt(m_cell, m_original);
	m_original = 0;
}


AddBackstitchCommand::AddBackstitchCommand(Document *document, const QPoint &start, const QPoint &end, int floss)
	:	QUndoCommand(i18n("Add Backstitch")),
		m_document(document),
		m_start(start),
		m_end(end),
		m_floss(floss)
{
}


AddBackstitchCommand::~AddBackstitchCommand()
{
}


void AddBackstitchCommand::redo()
{
	m_document->addBackstitch(m_start, m_end, m_floss);
}


void AddBackstitchCommand::undo()
{
	m_document->deleteBackstitch(m_start, m_end, m_floss);
}


DeleteBackstitchCommand::DeleteBackstitchCommand(Document *document, const QPoint &start, const QPoint &end, int floss)
	:	QUndoCommand(i18n("Delete Backstitch")),
		m_document(document),
		m_start(start),
		m_end(end),
		m_floss(floss)
{
}


DeleteBackstitchCommand::~DeleteBackstitchCommand()
{
}


void DeleteBackstitchCommand::redo()
{
	m_document->deleteBackstitch(m_start, m_end, m_floss);
}


void DeleteBackstitchCommand::undo()
{
	m_document->addBackstitch(m_start, m_end, m_floss);
}


AddKnotCommand::AddKnotCommand(Document *document, const QPoint &snap, int floss)
	:	QUndoCommand(),
		m_document(document),
		m_snap(snap),
		m_floss(floss)
{
}


AddKnotCommand::~AddKnotCommand()
{
}


void AddKnotCommand::redo()
{
	m_document->addFrenchKnot(m_snap, m_floss);
}


void AddKnotCommand::undo()
{
	m_document->deleteFrenchKnot(m_snap, m_floss);
}


DeleteKnotCommand::DeleteKnotCommand(Document *document, const QPoint &snap, int floss)
	:	QUndoCommand(),
		m_document(document),
		m_snap(snap),
		m_floss(floss)
{
}


DeleteKnotCommand::~DeleteKnotCommand()
{
}


void DeleteKnotCommand::redo()
{
}


void DeleteKnotCommand::undo()
{
}


SetPropertyCommand::SetPropertyCommand(Document *document, const QString &name, const QVariant &value)
	:	QUndoCommand(i18n("Set Property")),
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


AddBackgroundImageCommand::AddBackgroundImageCommand(Document *document, BackgroundImage *backgroundImage)
	:	QUndoCommand(i18n("Add Background Image")),
		m_document(document),
		m_backgroundImage(backgroundImage)
{
}


AddBackgroundImageCommand::~AddBackgroundImageCommand()
{
}


void AddBackgroundImageCommand::redo()
{
	m_document->addBackgroundImage(m_backgroundImage);
}


void AddBackgroundImageCommand::undo()
{
	m_document->removeBackgroundImage(m_backgroundImage);
}


FitBackgroundImageCommand::FitBackgroundImageCommand(Document *document, const QString &path, const QRect &rect)
	:	QUndoCommand(i18n("Fit Background to Selection")),
		m_document(document),
		m_path(path),
		m_rect(rect)
{
}


FitBackgroundImageCommand::~FitBackgroundImageCommand()
{
}


void FitBackgroundImageCommand::redo()
{
	m_rect = m_document->fitBackgroundImage(m_path, m_rect);
}


void FitBackgroundImageCommand::undo()
{
	m_rect = m_document->fitBackgroundImage(m_path, m_rect);
}


ShowBackgroundImageCommand::ShowBackgroundImageCommand(Document *document, const QString &path, bool visible)
	:	QUndoCommand(i18n("Show Background Image")),
		m_document(document),
		m_path(path),
		m_visible(visible)
{
}


ShowBackgroundImageCommand::~ShowBackgroundImageCommand()
{
}


void ShowBackgroundImageCommand::redo()
{
	m_visible = m_document->showBackgroundImage(m_path, m_visible);
}


void ShowBackgroundImageCommand::undo()
{
	m_document->showBackgroundImage(m_path, m_visible);
}


RemoveBackgroundImageCommand::RemoveBackgroundImageCommand(Document *document, const QString &path)
	:	QUndoCommand(i18n("Remove Background Image")),
		m_document(document),
		m_path(path),
		m_backgroundImage(0)
{
}


RemoveBackgroundImageCommand::~RemoveBackgroundImageCommand()
{
	delete m_backgroundImage;
}


void RemoveBackgroundImageCommand::redo()
{
	m_backgroundImage = m_document->removeBackgroundImage(m_path);
}


void RemoveBackgroundImageCommand::undo()
{
	m_document->addBackgroundImage(m_backgroundImage);
}


AddFlossCommand::AddFlossCommand(Document *document, unsigned key, DocumentFloss *documentFloss)
	:	QUndoCommand(),
		m_document(document),
		m_key(key),
		m_documentFloss(documentFloss)
{
}


AddFlossCommand::~AddFlossCommand()
{
}


void AddFlossCommand::redo()
{
	m_document->addFloss(m_key, m_documentFloss);
}


void AddFlossCommand::undo()
{
	m_document->removeFloss(m_key);
}


RemoveFlossCommand::RemoveFlossCommand(Document *document, unsigned key, DocumentFloss *documentFloss)
	:	QUndoCommand(),
		m_document(document),
		m_key(key),
		m_documentFloss(documentFloss)
{
}


RemoveFlossCommand::~RemoveFlossCommand()
{
	delete m_documentFloss;
}


void RemoveFlossCommand::redo()
{
	m_documentFloss = m_document->removeFloss(m_key);
}


void RemoveFlossCommand::undo()
{
	m_document->addFloss(m_key, m_documentFloss);
	m_documentFloss = 0;
}


ChangeFlossCommand::ChangeFlossCommand(Document *document, int key, DocumentFloss *documentFloss)
	:	QUndoCommand(),
		m_document(document),
		m_key(key),
		m_documentFloss(documentFloss)
{
}


ChangeFlossCommand::~ChangeFlossCommand()
{
	delete m_documentFloss;
}


void ChangeFlossCommand::redo()
{
	m_documentFloss = m_document->changeFloss(m_key, m_documentFloss);
}


void ChangeFlossCommand::undo()
{
	m_documentFloss = m_document->changeFloss(m_key, m_documentFloss);
}


ResizeDocumentCommand::ResizeDocumentCommand(Document *document, int width, int height)
	:	QUndoCommand(),
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
	m_originalWidth = m_document->width();
	m_originalHeight = m_document->height();
	QRect extents = m_document->extents();
	int minx = std::min(extents.left(), m_width - extents.width());
	m_xOffset = minx - extents.left();
	int miny = std::min(extents.top(), m_height - extents.height());
	m_yOffset = miny - extents.top();
	m_document->movePattern(m_xOffset, m_yOffset, m_width);
	m_document->resizeDocument(m_width, m_height);
}


void ResizeDocumentCommand::undo()
{
	m_document->movePattern(-m_xOffset, -m_yOffset, m_originalWidth);
	m_document->resizeDocument(m_originalWidth, m_originalHeight);
}


ChangeFlossColorCommand::ChangeFlossColorCommand(Document *document, DocumentFloss *documentFloss, Floss *floss)
	:	QUndoCommand(),
		m_document(document),
		m_documentFloss(documentFloss),
		m_floss(floss)
{
}


ChangeFlossColorCommand::~ChangeFlossColorCommand()
{
}


void ChangeFlossColorCommand::redo()
{
	const Floss *floss = m_documentFloss->floss();
	m_documentFloss->setFloss(m_floss);
	m_floss = floss;
}


void ChangeFlossColorCommand::undo()
{
	const Floss *floss = m_documentFloss->floss();
	m_documentFloss->setFloss(m_floss);
	m_floss = floss;
}
