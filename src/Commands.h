/********************************************************************************
 *	Copyright (C) 2010 by Stephen Allewell					*
 *	stephen@mirramar.adsl24.co.uk						*
 *										*
 *	This program is free software; you can redistribute it and/or modify	*
 *	it under the terms of the GNU General Public License as published by	*
 *	the Free Software Foundation; either version 2 of the License, or	*
 *	(at your option) any later version.					*
 ********************************************************************************/


#ifndef Commands_H
#define Commands_H


#include <QPoint>
#include <QRect>
#include <QString>
#include <QUndoCommand>
#include <QVariant>

#include <KUrl>

#include "Stitch.h"


class BackgroundImage;
class Document;
class DocumentCrossStitch;
class DocumentFloss;
class Editor;
class Floss;
class MainWindow;
class Palette;
class Preview;


class AddStitchCommand : public QUndoCommand
{
	public:
		AddStitchCommand(Document *, int, const QPoint &, Stitch::Type, int);
		~AddStitchCommand();

		virtual void redo();
		virtual void undo();

	private:
		Document	*m_document;
		int		m_layer;
		QPoint		m_cell;
		Stitch::Type	m_type;
		int		m_colorIndex;
		StitchQueue	*m_original;
};


class DeleteStitchCommand : public QUndoCommand
{
	public:
		DeleteStitchCommand(Document *, int, const QPoint &, Stitch::Type, int);
		~DeleteStitchCommand();

		virtual void redo();
		virtual void undo();

	private:
		Document	*m_document;
		int		m_layer;
		QPoint		m_cell;
		Stitch::Type	m_type;
		int		m_colorIndex;
		StitchQueue	*m_original;
};


class AddBackstitchCommand : public QUndoCommand
{
	public:
		AddBackstitchCommand(Document *, int, const QPoint &, const QPoint &, int);
		~AddBackstitchCommand();

		virtual void redo();
		virtual void undo();

	private:
		Document	*m_document;
		int		m_layer;
		QPoint		m_start;
		QPoint		m_end;
		int		m_colorIndex;
};


class DeleteBackstitchCommand : public QUndoCommand
{
	public:
		DeleteBackstitchCommand(Document *, int, const QPoint &, const QPoint &, int);
		~DeleteBackstitchCommand();

		virtual void redo();
		virtual void undo();

	private:
		Document	*m_document;
		int		m_layer;
		QPoint		m_start;
		QPoint		m_end;
		int		m_colorIndex;
		Backstitch	*m_backstitch;
};


class AddKnotCommand : public QUndoCommand
{
	public:
		AddKnotCommand(Document *, int, const QPoint &, int);
		~AddKnotCommand();

		virtual void redo();
		virtual void undo();

	private:
		Document	*m_document;
		int		m_layer;
		QPoint		m_snap;
		int		m_colorIndex;
};


class DeleteKnotCommand : public QUndoCommand
{
	public:
		DeleteKnotCommand(Document *, int, const QPoint &, int);
		~DeleteKnotCommand();

		virtual void redo();
		virtual void undo();

	private:
		Document	*m_document;
		int		m_layer;
		QPoint		m_snap;
		int		m_colorIndex;
		Knot		*m_knot;
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
		AddBackgroundImageCommand(Document *, BackgroundImage *, MainWindow *);
		~AddBackgroundImageCommand();

		virtual void redo();
		virtual void undo();

	private:
		Document	*m_document;
		BackgroundImage	*m_backgroundImage;
		MainWindow	*m_mainWindow;
};


class FitBackgroundImageCommand : public QUndoCommand
{
	public:
		FitBackgroundImageCommand(Document *, BackgroundImage *, const QRect &);
		~FitBackgroundImageCommand();

		virtual void redo();
		virtual void undo();

	private:
		Document	*m_document;
		BackgroundImage	*m_backgroundImage;
		QRect		m_rect;
};


class ShowBackgroundImageCommand : public QUndoCommand
{
	public:
		ShowBackgroundImageCommand(Document *, BackgroundImage *, bool);
		~ShowBackgroundImageCommand();

		virtual void redo();
		virtual void undo();

	private:
		Document	*m_document;
		BackgroundImage	*m_backgroundImage;
		bool		m_visible;
};


class RemoveBackgroundImageCommand : public QUndoCommand
{
	public:
		RemoveBackgroundImageCommand(Document *, BackgroundImage *, MainWindow *);
		~RemoveBackgroundImageCommand();

		virtual void redo();
		virtual void undo();

	private:
		Document	*m_document;
		BackgroundImage	*m_backgroundImage;
		MainWindow	*m_mainWindow;
};


class AddDocumentFlossCommand : public QUndoCommand
{
	public:
		AddDocumentFlossCommand(Document *, int, DocumentFloss *);
		~AddDocumentFlossCommand();

		virtual void redo();
		virtual void undo();

	private:
		Document	*m_document;
		int		m_key;
		DocumentFloss	*m_documentFloss;
};


class RemoveDocumentFlossCommand : public QUndoCommand
{
	public:
		RemoveDocumentFlossCommand(Document *, int, DocumentFloss *);
		~RemoveDocumentFlossCommand();

		virtual void redo();
		virtual void undo();

	private:
		Document	*m_document;
		int		m_key;
		DocumentFloss	*m_documentFloss;
};


class ReplaceDocumentFlossCommand : public QUndoCommand
{
	public:
		ReplaceDocumentFlossCommand(Document *, int, DocumentFloss *);
		~ReplaceDocumentFlossCommand();

		virtual void redo();
		virtual void undo();

	private:
		Document	*m_document;
		int		m_key;
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
		int		m_width;
		int		m_height;
		int		m_originalWidth;
		int		m_originalHeight;
		int		m_xOffset;
		int		m_yOffset;
		QPoint		m_snapOffset;
};


#if 0
class ChangeFlossColorCommand : public QUndoCommand
{
	public:
		ChangeFlossColorCommand(Document *, DocumentFloss *, Floss *);
		~ChangeFlossColorCommand();

		void redo();
		void undo();

	private:
		Document	*m_document;
		DocumentFloss	*m_documentFloss;
		Floss		*m_floss;
};
#endif


class CropToPatternCommand : public QUndoCommand
{
	public:
		CropToPatternCommand(Document *);
		~CropToPatternCommand();

		void redo();
		void undo();

	private:
		Document	*m_document;
		unsigned	m_originalWidth;
		unsigned	m_originalHeight;
		int		m_xOffset;
		int		m_yOffset;
		QRect		m_extents;
};


class ExtendPatternCommand : public QUndoCommand
{
	public:
		ExtendPatternCommand(Document *, int, int, int, int);
		~ExtendPatternCommand();

		void redo();
		void undo();

	private:
		Document	*m_document;
		int		m_top;
		int		m_left;
		int		m_right;
		int		m_bottom;
};


class UpdateEditorCommand : public QUndoCommand
{
	public:
		UpdateEditorCommand(Editor *);
		~UpdateEditorCommand();

		void redo();
		void undo();

	private:
		Editor	*m_editor;
};


class UpdatePaletteCommand : public QUndoCommand
{
	public:
		UpdatePaletteCommand(Palette *);
		~UpdatePaletteCommand();

		void redo();
		void undo();

	private:
		Palette	*m_palette;
};


class UpdatePreviewCommand : public QUndoCommand
{
	public:
		UpdatePreviewCommand(Preview *);
		~UpdatePreviewCommand();

		void redo();
		void undo();

	private:
		Preview	*m_preview;
};


class ChangeSchemeCommand : public QUndoCommand
{
	public:
		ChangeSchemeCommand(Document *, const QString &);
		~ChangeSchemeCommand();

		void redo();
		void undo();

	private:
		Document			*m_document;
		QString				m_schemeName;
		QMap<int, DocumentFloss *>	m_flosses;
};


class EditorReadDocumentSettingsCommand : public QUndoCommand
{
	public:
		EditorReadDocumentSettingsCommand(Editor *);
		~EditorReadDocumentSettingsCommand();

		void redo();
		void undo();

	private:
		Editor	*m_editor;
};


class PaletteReadDocumentSettingsCommand : public QUndoCommand
{
	public:
		PaletteReadDocumentSettingsCommand(Palette *);
		~PaletteReadDocumentSettingsCommand();

		void redo();
		void undo();

	private:
		Palette	*m_palette;
};


class PreviewReadDocumentSettingsCommand : public QUndoCommand
{
	public:
		PreviewReadDocumentSettingsCommand(Preview *);
		~PreviewReadDocumentSettingsCommand();

		void redo();
		void undo();

	private:
		Preview	*m_preview;
};


#endif // Commands_H
