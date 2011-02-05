/****************************************************************************
 *	Copyright (C) 2010 by Stephen Allewell									*
 *	stephen@mirramar.adsl24.co.uk											*
 *																			*
 *	This program is free software; you can redistribute it and/or modify	*
 *	it under the terms of the GNU General Public License as published by	*
 *	the Free Software Foundation; either version 2 of the License, or		*
 *	(at your option) any later version.										*
 ****************************************************************************/


#ifndef COMMANDS_H
#define COMMANDS_H


#include <QPoint>
#include <QRect>
#include <QString>
#include <QUndoCommand>
#include <QVariant>

#include <KUrl>

#include "stitch.h"


class BackgroundImage;
class Document;
class DocumentFloss;
class Floss;


class AddStitchCommand : public QUndoCommand
{
	public:
		AddStitchCommand(Document *, const QPoint &, Stitch::Type, int);
		~AddStitchCommand();

		virtual void redo();
		virtual void undo();

	private:
		Document		*m_document;
		QPoint			m_cell;
		Stitch::Type	m_type;
		int				m_floss;
		StitchQueue		*m_original;
};


class DeleteStitchCommand : public QUndoCommand
{
	public:
		DeleteStitchCommand(Document *, const QPoint &, Stitch::Type, int);
		~DeleteStitchCommand();

		virtual void redo();
		virtual void undo();

	private:
		Document		*m_document;
		QPoint			m_cell;
		Stitch::Type	m_type;
		int				m_floss;
		StitchQueue		*m_original;
};


class AddBackstitchCommand : public QUndoCommand
{
	public:
		AddBackstitchCommand(Document *, const QPoint &, const QPoint &, int);
		~AddBackstitchCommand();

		virtual void redo();
		virtual void undo();

	private:
		Document	*m_document;
		QPoint		m_start;
		QPoint		m_end;
		int			m_floss;
};


class DeleteBackstitchCommand : public QUndoCommand
{
	public:
		DeleteBackstitchCommand(Document *, const QPoint &, const QPoint &, int);
		~DeleteBackstitchCommand();

		virtual void redo();
		virtual void undo();

	private:
		Document	*m_document;
		QPoint		m_start;
		QPoint		m_end;
		int			m_floss;
};


class AddKnotCommand : public QUndoCommand
{
	public:
		AddKnotCommand(Document *, const QPoint &, int);
		~AddKnotCommand();

		virtual void redo();
		virtual void undo();

	private:
		Document	*m_document;
		QPoint		m_snap;
		int			m_floss;
};


class DeleteKnotCommand : public QUndoCommand
{
	public:
		DeleteKnotCommand(Document *, const QPoint &, int);
		~DeleteKnotCommand();

		virtual void redo();
		virtual void undo();

	private:
		Document	*m_document;
		QPoint		m_snap;
		int			m_floss;
};


class SetPropertyCommand : public QUndoCommand
{
	public:
		SetPropertyCommand(Document *, const QString &, const QVariant &);
		~SetPropertyCommand();

		virtual void redo();
		virtual void undo();

	private:
		Document	*m_document;
		QString		m_name;
		QVariant	m_value;
		QVariant	m_oldValue;
};


class AddBackgroundImageCommand : public QUndoCommand
{
	public:
		AddBackgroundImageCommand(Document *, BackgroundImage *);
		~AddBackgroundImageCommand();

		virtual void redo();
		virtual void undo();

	private:
		Document	*m_document;
		BackgroundImage	*m_backgroundImage;
};


class FitBackgroundImageCommand : public QUndoCommand
{
	public:
		FitBackgroundImageCommand(Document *, const QString &, const QRect &);
		~FitBackgroundImageCommand();

		virtual void redo();
		virtual void undo();

	private:
		Document	*m_document;
		QString		m_path;
		QRect		m_rect;
		QRect		m_oldRect;
};


class ShowBackgroundImageCommand : public QUndoCommand
{
	public:
		ShowBackgroundImageCommand(Document *, const QString &, bool);
		~ShowBackgroundImageCommand();

		virtual void redo();
		virtual void undo();

	private:
		Document	*m_document;
		QString		m_path;
		bool		m_visible;
};


class RemoveBackgroundImageCommand : public QUndoCommand
{
	public:
		RemoveBackgroundImageCommand(Document *, const QString &);
		~RemoveBackgroundImageCommand();

		virtual void redo();
		virtual void undo();

	private:
		Document	*m_document;
		QString		m_path;
		BackgroundImage	*m_backgroundImage;
};


class AddFlossCommand : public QUndoCommand
{
	public:
		AddFlossCommand(Document *, unsigned, DocumentFloss *);
		~AddFlossCommand();

		virtual void redo();
		virtual void undo();

	private:
		Document		*m_document;
		unsigned		m_key;
		DocumentFloss	*m_documentFloss;
};


class RemoveFlossCommand : public QUndoCommand
{
	public:
		RemoveFlossCommand(Document *, unsigned, DocumentFloss *);
		~RemoveFlossCommand();

		virtual void redo();
		virtual void undo();

	private:
		Document		*m_document;
		unsigned		m_key;
		DocumentFloss	*m_documentFloss;
};


class ChangeFlossCommand : public QUndoCommand
{
	public:
		ChangeFlossCommand(Document *, int, DocumentFloss *);
		~ChangeFlossCommand();

		virtual void redo();
		virtual void undo();

	private:
		Document		*m_document;
		int				m_key;
		DocumentFloss	*m_documentFloss;
};


class ResizeDocumentCommand : public QUndoCommand
{
	public:
		ResizeDocumentCommand(Document *, int, int);
		~ResizeDocumentCommand();

		void redo();
		void undo();

	private:
		Document	*m_document;
		int			m_width;
		int			m_height;
		int			m_originalWidth;
		int			m_originalHeight;
		int			m_xOffset;
		int			m_yOffset;
		QPoint		m_snapOffset;
};


class ChangeFlossColorCommand : public QUndoCommand
{
	public:
		ChangeFlossColorCommand(Document *, DocumentFloss *, Floss *);
		~ChangeFlossColorCommand();

		void redo();
		void undo();

	private:
		Document		*m_document;
		DocumentFloss	*m_documentFloss;
		const Floss		*m_floss;
};


#endif