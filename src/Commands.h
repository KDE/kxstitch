/*
 * Copyright (C) 2010-2015 by Stephen Allewell
 * steve.allewell@gmail.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#ifndef Commands_H
#define Commands_H


#include <QPoint>
#include <QRect>
#include <QString>
#include <QUndoCommand>
#include <QVariant>

#include "DocumentPalette.h"
#include "PrinterConfiguration.h"
#include "Stitch.h"
#include "StitchData.h"


class BackgroundImage;
class Document;
class DocumentFloss;
class Editor;
class Floss;
class MainWindow;
class Palette;
class Pattern;
class Preview;


class FilePropertiesCommand : public QUndoCommand
{
public:
    explicit FilePropertiesCommand(Document *);
    virtual ~FilePropertiesCommand() = default;

    virtual void redo() Q_DECL_OVERRIDE;
    virtual void undo() Q_DECL_OVERRIDE;

private:
    Document    *m_document;
};


class ImportImageCommand : public QUndoCommand
{
public:
    explicit ImportImageCommand(Document *);
    virtual ~ImportImageCommand() = default;

    virtual void redo() Q_DECL_OVERRIDE;
    virtual void undo() Q_DECL_OVERRIDE;

private:
    Document    *m_document;
};


class PaintStitchesCommand : public QUndoCommand
{
public:
    explicit PaintStitchesCommand(Document *);
    virtual ~PaintStitchesCommand() = default;

    virtual void redo() Q_DECL_OVERRIDE;
    virtual void undo() Q_DECL_OVERRIDE;

private:
    Document    *m_document;
};


class PaintKnotsCommand : public QUndoCommand
{
public:
    explicit PaintKnotsCommand(Document *);
    virtual ~PaintKnotsCommand() = default;

    virtual void redo() Q_DECL_OVERRIDE;
    virtual void undo() Q_DECL_OVERRIDE;

private:
    Document    *m_document;
};


class DrawLineCommand : public QUndoCommand
{
public:
    explicit DrawLineCommand(Document *);
    virtual ~DrawLineCommand() = default;

    virtual void redo() Q_DECL_OVERRIDE;
    virtual void undo() Q_DECL_OVERRIDE;

private:
    Document    *m_document;    /**< pointer to the associated Document */
};


class EraseStitchesCommand : public QUndoCommand
{
public:
    explicit EraseStitchesCommand(Document *);
    virtual ~EraseStitchesCommand() = default;

    virtual void redo() Q_DECL_OVERRIDE;
    virtual void undo() Q_DECL_OVERRIDE;

private:
    Document    *m_document;
};


class DrawRectangleCommand : public QUndoCommand
{
public:
    explicit DrawRectangleCommand(Document *document);
    virtual ~DrawRectangleCommand() = default;

    virtual void redo() Q_DECL_OVERRIDE;
    virtual void undo() Q_DECL_OVERRIDE;

private:
    Document    *m_document;    /**< pointer to the associated Document */
};


class FillRectangleCommand : public QUndoCommand
{
public:
    explicit FillRectangleCommand(Document *document);
    virtual ~FillRectangleCommand() = default;

    virtual void redo() Q_DECL_OVERRIDE;
    virtual void undo() Q_DECL_OVERRIDE;

private:
    Document    *m_document;    /**< pointer to the associated Document */
};


class DrawEllipseCommand : public QUndoCommand
{
public:
    explicit DrawEllipseCommand(Document *document);
    virtual ~DrawEllipseCommand() = default;

    virtual void redo() Q_DECL_OVERRIDE;
    virtual void undo() Q_DECL_OVERRIDE;

private:
    Document    *m_document;    /**< pointer to the associated Document */
};


class FillEllipseCommand : public QUndoCommand
{
public:
    explicit FillEllipseCommand(Document *document);
    virtual ~FillEllipseCommand() = default;

    virtual void redo() Q_DECL_OVERRIDE;
    virtual void undo() Q_DECL_OVERRIDE;

private:
    Document    *m_document;    /**< pointer to the associated Document */
};


class FillPolygonCommand : public QUndoCommand
{
public:
    explicit FillPolygonCommand(Document *);
    virtual ~FillPolygonCommand() = default;

    virtual void redo() Q_DECL_OVERRIDE;
    virtual void undo() Q_DECL_OVERRIDE;

private:
    Document    *m_document;
};


class AddStitchCommand : public QUndoCommand
{
public:
    AddStitchCommand(Document *, const QPoint &, Stitch::Type, int, QUndoCommand *);
    virtual ~AddStitchCommand();

    virtual void redo() Q_DECL_OVERRIDE;
    virtual void undo() Q_DECL_OVERRIDE;

private:
    Document        *m_document;
    QPoint          m_cell;
    Stitch::Type    m_type;
    int             m_colorIndex;
    StitchQueue     *m_original;
};


class DeleteStitchCommand : public QUndoCommand
{
public:
    DeleteStitchCommand(Document *, const QPoint &, Stitch::Type, int, QUndoCommand *);
    virtual ~DeleteStitchCommand();

    virtual void redo() Q_DECL_OVERRIDE;
    virtual void undo() Q_DECL_OVERRIDE;

private:
    Document        *m_document;
    QPoint          m_cell;
    Stitch::Type    m_type;
    int             m_colorIndex;
    StitchQueue     *m_original;
};


class AddBackstitchCommand : public QUndoCommand
{
public:
    AddBackstitchCommand(Document *, const QPoint &, const QPoint &, int);
    virtual ~AddBackstitchCommand() = default;

    virtual void redo() Q_DECL_OVERRIDE;
    virtual void undo() Q_DECL_OVERRIDE;

private:
    Document    *m_document;
    QPoint      m_start;
    QPoint      m_end;
    int         m_colorIndex;
};


class DeleteBackstitchCommand : public QUndoCommand
{
public:
    DeleteBackstitchCommand(Document *, const QPoint &, const QPoint &, int);
    virtual ~DeleteBackstitchCommand();

    virtual void redo() Q_DECL_OVERRIDE;
    virtual void undo() Q_DECL_OVERRIDE;

private:
    Document    *m_document;
    QPoint      m_start;
    QPoint      m_end;
    int         m_colorIndex;
    Backstitch  *m_backstitch;
};


class AddKnotCommand : public QUndoCommand
{
public:
    AddKnotCommand(Document *, const QPoint &, int, QUndoCommand *);
    virtual ~AddKnotCommand() = default;

    virtual void redo() Q_DECL_OVERRIDE;
    virtual void undo() Q_DECL_OVERRIDE;

private:
    Document    *m_document;
    QPoint      m_snap;
    int         m_colorIndex;
};


class DeleteKnotCommand : public QUndoCommand
{
public:
    DeleteKnotCommand(Document *, const QPoint &, int, QUndoCommand *);
    virtual ~DeleteKnotCommand();

    virtual void redo() Q_DECL_OVERRIDE;
    virtual void undo() Q_DECL_OVERRIDE;

private:
    Document    *m_document;
    QPoint      m_snap;
    int         m_colorIndex;
    Knot        *m_knot;
};


class SetPropertyCommand : public QUndoCommand
{
public:
    SetPropertyCommand(Document *, const QString &, const QVariant &, QUndoCommand *parent = nullptr);
    virtual ~SetPropertyCommand() = default;

    virtual void redo() Q_DECL_OVERRIDE;
    virtual void undo() Q_DECL_OVERRIDE;

private:
    Document    *m_document;
    QString     m_name;
    QVariant    m_value;
    QVariant    m_oldValue;
};


class AddBackgroundImageCommand : public QUndoCommand
{
public:
    AddBackgroundImageCommand(Document *, QSharedPointer<BackgroundImage>, MainWindow *);
    virtual ~AddBackgroundImageCommand() = default;

    virtual void redo() Q_DECL_OVERRIDE;
    virtual void undo() Q_DECL_OVERRIDE;

private:
    Document        *m_document;
    QSharedPointer<BackgroundImage> m_backgroundImage;
    MainWindow      *m_mainWindow;
};


class FitBackgroundImageCommand : public QUndoCommand
{
public:
    FitBackgroundImageCommand(Document *, QSharedPointer<BackgroundImage>, const QRect &);
    virtual ~FitBackgroundImageCommand() = default;

    virtual void redo() Q_DECL_OVERRIDE;
    virtual void undo() Q_DECL_OVERRIDE;

private:
    Document        *m_document;
    QSharedPointer<BackgroundImage> m_backgroundImage;
    QRect           m_rect;
};


class ShowBackgroundImageCommand : public QUndoCommand
{
public:
    ShowBackgroundImageCommand(Document *, QSharedPointer<BackgroundImage>, bool);
    virtual ~ShowBackgroundImageCommand() = default;

    virtual void redo() Q_DECL_OVERRIDE;
    virtual void undo() Q_DECL_OVERRIDE;

private:
    Document        *m_document;
    QSharedPointer<BackgroundImage> m_backgroundImage;
    bool            m_visible;
};


class RemoveBackgroundImageCommand : public QUndoCommand
{
public:
    RemoveBackgroundImageCommand(Document *, QSharedPointer<BackgroundImage>, MainWindow *);
    virtual ~RemoveBackgroundImageCommand();

    virtual void redo() Q_DECL_OVERRIDE;
    virtual void undo() Q_DECL_OVERRIDE;

private:
    Document        *m_document;
    QSharedPointer<BackgroundImage> m_backgroundImage;
    MainWindow      *m_mainWindow;
};


class AddDocumentFlossCommand : public QUndoCommand
{
public:
    AddDocumentFlossCommand(Document *, int, DocumentFloss *, QUndoCommand *parent = nullptr);
    virtual ~AddDocumentFlossCommand() = default;

    virtual void redo() Q_DECL_OVERRIDE;
    virtual void undo() Q_DECL_OVERRIDE;

private:
    Document        *m_document;
    int             m_key;
    DocumentFloss   *m_documentFloss;
};


class RemoveDocumentFlossCommand : public QUndoCommand
{
public:
    RemoveDocumentFlossCommand(Document *, int, DocumentFloss *, QUndoCommand *parent = nullptr);
    virtual ~RemoveDocumentFlossCommand();

    virtual void redo() Q_DECL_OVERRIDE;
    virtual void undo() Q_DECL_OVERRIDE;

private:
    Document        *m_document;
    int             m_key;
    DocumentFloss   *m_documentFloss;
};


class ReplaceDocumentFlossCommand : public QUndoCommand
{
public:
    ReplaceDocumentFlossCommand(Document *, int, DocumentFloss *);
    virtual ~ReplaceDocumentFlossCommand();

    virtual void redo() Q_DECL_OVERRIDE;
    virtual void undo() Q_DECL_OVERRIDE;

private:
    Document        *m_document;
    int             m_key;
    DocumentFloss   *m_documentFloss;
};


class ClearUnusedFlossesCommand : public QUndoCommand
{
public:
    explicit ClearUnusedFlossesCommand(Document *);
    virtual ~ClearUnusedFlossesCommand() = default;

    virtual void redo() Q_DECL_OVERRIDE;
    virtual void undo() Q_DECL_OVERRIDE;

private:
    Document    *m_document;
};


class ResizeDocumentCommand : public QUndoCommand
{
public:
    ResizeDocumentCommand(Document *, int, int, QUndoCommand *parent = nullptr);
    virtual ~ResizeDocumentCommand() = default;

    void redo() Q_DECL_OVERRIDE;
    void undo() Q_DECL_OVERRIDE;

private:
    Document    *m_document;
    int         m_width;
    int         m_height;
    int         m_originalWidth;
    int         m_originalHeight;
    int         m_xOffset;
    int         m_yOffset;
    QPoint      m_snapOffset;
};


class CropToPatternCommand : public QUndoCommand
{
public:
    explicit CropToPatternCommand(Document *);
    virtual ~CropToPatternCommand() = default;

    void redo() Q_DECL_OVERRIDE;
    void undo() Q_DECL_OVERRIDE;

private:
    Document    *m_document;
    unsigned    m_originalWidth;
    unsigned    m_originalHeight;
    int         m_xOffset;
    int         m_yOffset;
    QRect       m_extents;
};


class CropToSelectionCommand : public QUndoCommand
{
public:
    CropToSelectionCommand(Document *, const QRect &);
    virtual ~CropToSelectionCommand() = default;

    void redo() Q_DECL_OVERRIDE;
    void undo() Q_DECL_OVERRIDE;

private:
    Document    *m_document;
    QRect       m_selectionArea;
    QByteArray  m_originalPattern;
};


class InsertColumnsCommand : public QUndoCommand
{
public:
    InsertColumnsCommand(Document *, const QRect &);
    virtual ~InsertColumnsCommand() = default;

    void redo() Q_DECL_OVERRIDE;
    void undo() Q_DECL_OVERRIDE;

private:
    Document    *m_document;
    QRect       m_selectionArea;
};


class InsertRowsCommand : public QUndoCommand
{
public:
    InsertRowsCommand(Document *, const QRect &);
    virtual ~InsertRowsCommand() = default;

    void redo() Q_DECL_OVERRIDE;
    void undo() Q_DECL_OVERRIDE;

private:
    Document    *m_document;
    QRect       m_selectionArea;
};


class ExtendPatternCommand : public QUndoCommand
{
public:
    ExtendPatternCommand(Document *, int, int, int, int);
    virtual ~ExtendPatternCommand() = default;

    void redo() Q_DECL_OVERRIDE;
    void undo() Q_DECL_OVERRIDE;

private:
    Document    *m_document;
    int         m_top;
    int         m_left;
    int         m_right;
    int         m_bottom;
};


class CentrePatternCommand : public QUndoCommand
{
public:
    explicit CentrePatternCommand(Document *);
    virtual ~CentrePatternCommand() = default;

    void redo() Q_DECL_OVERRIDE;
    void undo() Q_DECL_OVERRIDE;

private:
    Document    *m_document;
    int         m_xOffset;
    int         m_yOffset;
};


class UpdateDocumentPaletteCommand : public QUndoCommand
{
public:
    UpdateDocumentPaletteCommand(Document *, const DocumentPalette &);
    virtual ~UpdateDocumentPaletteCommand() = default;

    void redo() Q_DECL_OVERRIDE;
    void undo() Q_DECL_OVERRIDE;

private:
    Document        *m_document;
    DocumentPalette m_palette;
};


class ChangeSchemeCommand : public QUndoCommand
{
public:
    ChangeSchemeCommand(Document *, const QString &, QUndoCommand *parent = nullptr);
    virtual ~ChangeSchemeCommand() = default;

    void redo() Q_DECL_OVERRIDE;
    void undo() Q_DECL_OVERRIDE;

private:
    Document    *m_document;
    QString     m_schemeName;
    QByteArray  m_originalPalette;
};


class EditorReadDocumentSettingsCommand : public QUndoCommand
{
public:
    explicit EditorReadDocumentSettingsCommand(Editor *);
    virtual ~EditorReadDocumentSettingsCommand() = default;

    void redo() Q_DECL_OVERRIDE;
    void undo() Q_DECL_OVERRIDE;

private:
    Editor  *m_editor;
};


class PreviewReadDocumentSettingsCommand : public QUndoCommand
{
public:
    explicit PreviewReadDocumentSettingsCommand(Preview *);
    virtual ~PreviewReadDocumentSettingsCommand() = default;

    void redo() Q_DECL_OVERRIDE;
    void undo() Q_DECL_OVERRIDE;

private:
    Preview *m_preview;
};


class PaletteReplaceColorCommand : public QUndoCommand
{
public:
    PaletteReplaceColorCommand(Document *document, int, int);
    virtual ~PaletteReplaceColorCommand() = default;

    void redo() Q_DECL_OVERRIDE;
    void undo() Q_DECL_OVERRIDE;

private:
    Document    *m_document;
    int         m_originalIndex;
    int         m_replacementIndex;
    QList<Stitch *>     m_stitches;
    QList<Backstitch *> m_backstitches;
    QList<Knot *>       m_knots;
};


class PaletteSwapColorCommand : public QUndoCommand
{
public:
    PaletteSwapColorCommand(Document *, int, int);
    virtual ~PaletteSwapColorCommand() = default;

    void redo() Q_DECL_OVERRIDE;
    void undo() Q_DECL_OVERRIDE;

private:
    Document    *m_document;
    int         m_originalIndex;
    int         m_swappedIndex;
};


class UpdatePrinterConfigurationCommand : public QUndoCommand
{
public:
    UpdatePrinterConfigurationCommand(Document *, const PrinterConfiguration &);
    virtual ~UpdatePrinterConfigurationCommand() = default;

    void redo() Q_DECL_OVERRIDE;
    void undo() Q_DECL_OVERRIDE;

private:
    Document                *m_document;
    PrinterConfiguration    m_printerConfiguration;
};


class EditCutCommand : public QUndoCommand
{
public:
    EditCutCommand(Document *document, const QRect &selectionArea, int colorMask, const QList<Stitch::Type> &stitchMasks, bool excludeBackstitches, bool excludeKnots);
    virtual ~EditCutCommand();

    void redo() Q_DECL_OVERRIDE;
    void undo() Q_DECL_OVERRIDE;

private:
    Document            *m_document;
    QRect               m_selectionArea;
    int                 m_colorMask;
    QList<Stitch::Type> m_stitchMasks;
    bool                m_excludeBackstitches;
    bool                m_excludeKnots;

    Pattern             *m_originalPattern;
};


class EditPasteCommand : public QUndoCommand
{
public:
    EditPasteCommand(Document *document, Pattern *pattern, const QPoint &cell, bool merge, const QString &);
    virtual ~EditPasteCommand() = default;

    void redo() Q_DECL_OVERRIDE;
    void undo() Q_DECL_OVERRIDE;

private:
    Document    *m_document;
    Pattern     *m_pastePattern;
    QPoint      m_cell;
    bool        m_merge;

    QByteArray  m_originalPattern;
};


class MirrorSelectionCommand : public QUndoCommand
{
public:
    MirrorSelectionCommand(Document *, const QRect &, int, const QList<Stitch::Type> &, bool, bool, Qt::Orientation, bool, const QByteArray &, Pattern *, const QPoint &, bool merge);
    virtual ~MirrorSelectionCommand();

    virtual void redo() Q_DECL_OVERRIDE;
    virtual void undo() Q_DECL_OVERRIDE;

private:
    Document            *m_document;
    QRect               m_selectionArea;
    int                 m_colorMask;
    QList<Stitch::Type> m_stitchMasks;
    bool                m_excludeBackstitches;
    bool                m_excludeKnots;
    Qt::Orientation     m_orientation;
    bool                m_copies;
    QByteArray          m_originalPatternData;
    Pattern             *m_invertedPattern;
    QPoint              m_pasteCell;
    bool                m_merge;
};


class RotateSelectionCommand : public QUndoCommand
{
public:
    RotateSelectionCommand(Document *, const QRect &, int, const QList<Stitch::Type> &, bool, bool, StitchData::Rotation, bool, const QByteArray &, Pattern *, const QPoint &, bool);
    virtual ~RotateSelectionCommand();

    void redo() Q_DECL_OVERRIDE;
    void undo() Q_DECL_OVERRIDE;

private:
    Document                *m_document;
    QRect                   m_selectionArea;
    int                     m_colorMask;
    QList<Stitch::Type>     m_stitchMasks;
    bool                    m_excludeBackstitches;
    bool                    m_excludeKnots;
    StitchData::Rotation    m_rotation;
    bool                    m_copies;
    QByteArray              m_originalPatternData;
    Pattern                 *m_rotatedPattern;
    QPoint                  m_pasteCell;
    bool                    m_merge;
};


class AlphabetCommand : public QUndoCommand
{
public:
    explicit AlphabetCommand(Document *);
    virtual ~AlphabetCommand();

    void redo() Q_DECL_OVERRIDE;
    void undo() Q_DECL_OVERRIDE;

    void push(QUndoCommand *);
    QUndoCommand *pop();

    int childCount() const;

private:
    Document                *m_document;
    QList<QUndoCommand *>   m_children;
};


class ConfigurationCommand : public QUndoCommand
{
public:
    explicit ConfigurationCommand(MainWindow *mainWindow);
    virtual ~ConfigurationCommand() = default;

    void redo() Q_DECL_OVERRIDE;
    void undo() Q_DECL_OVERRIDE;

private:
    MainWindow  *m_mainWindow;
};


#endif // Commands_H
