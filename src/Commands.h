/*
 * Copyright (C) 2010 by Stephen Allewell
 * sallewell@users.sourceforge.net
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

#include <KUrl>

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
    virtual ~FilePropertiesCommand();

    virtual void redo();
    virtual void undo();

private:
    Document    *m_document;
};


class ImportImageCommand : public QUndoCommand
{
public:
    explicit ImportImageCommand(Document *);
    virtual ~ImportImageCommand();

    virtual void redo();
    virtual void undo();
    
private:
    Document    *m_document;
};


class PaintStitchesCommand : public QUndoCommand
{
public:
    explicit PaintStitchesCommand(Document *);
    virtual ~PaintStitchesCommand();

    virtual void redo();
    virtual void undo();

private:
    Document    *m_document;
};


class PaintKnotsCommand : public QUndoCommand
{
public:
    explicit PaintKnotsCommand(Document *);
    virtual ~PaintKnotsCommand();

    virtual void redo();
    virtual void undo();

private:
    Document    *m_document;
};


class DrawLineCommand : public QUndoCommand
{
public:
    explicit DrawLineCommand(Document *);
    virtual ~DrawLineCommand();

    virtual void redo();
    virtual void undo();

private:
    Document    *m_document;    /**< pointer to the associated Document */
};


class EraseStitchesCommand : public QUndoCommand
{
public:
    explicit EraseStitchesCommand(Document *);
    virtual ~EraseStitchesCommand();

    virtual void redo();
    virtual void undo();

private:
    Document    *m_document;
};


class DrawRectangleCommand : public QUndoCommand
{
public:
    explicit DrawRectangleCommand(Document *document);
    virtual ~DrawRectangleCommand();

    virtual void redo();
    virtual void undo();

private:
    Document    *m_document;    /**< pointer to the associated Document */
};


class FillRectangleCommand : public QUndoCommand
{
public:
    explicit FillRectangleCommand(Document *document);
    virtual ~FillRectangleCommand();

    virtual void redo();
    virtual void undo();

private:
    Document    *m_document;    /**< pointer to the associated Document */
};


class DrawEllipseCommand : public QUndoCommand
{
public:
    explicit DrawEllipseCommand(Document *document);
    virtual ~DrawEllipseCommand();

    virtual void redo();
    virtual void undo();

private:
    Document    *m_document;    /**< pointer to the associated Document */
};


class FillEllipseCommand : public QUndoCommand
{
public:
    explicit FillEllipseCommand(Document *document);
    virtual ~FillEllipseCommand();

    virtual void redo();
    virtual void undo();

private:
    Document    *m_document;    /**< pointer to the associated Document */
};


class FillPolygonCommand : public QUndoCommand
{
public:
    explicit FillPolygonCommand(Document *);
    virtual ~FillPolygonCommand();

    virtual void redo();
    virtual void undo();

private:
    Document    *m_document;
};


class AddStitchCommand : public QUndoCommand
{
public:
    AddStitchCommand(Document *, const QPoint &, Stitch::Type, int, QUndoCommand *);
    ~AddStitchCommand();

    virtual void redo();
    virtual void undo();

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
    ~DeleteStitchCommand();

    virtual void redo();
    virtual void undo();

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
    ~AddBackstitchCommand();

    virtual void redo();
    virtual void undo();

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
    ~DeleteBackstitchCommand();

    virtual void redo();
    virtual void undo();

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
    ~AddKnotCommand();

    virtual void redo();
    virtual void undo();

private:
    Document    *m_document;
    QPoint      m_snap;
    int         m_colorIndex;
};


class DeleteKnotCommand : public QUndoCommand
{
public:
    DeleteKnotCommand(Document *, const QPoint &, int, QUndoCommand *);
    ~DeleteKnotCommand();

    virtual void redo();
    virtual void undo();

private:
    Document    *m_document;
    QPoint      m_snap;
    int         m_colorIndex;
    Knot        *m_knot;
};


class SetPropertyCommand : public QUndoCommand
{
public:
    SetPropertyCommand(Document *, const QString &, const QVariant &, QUndoCommand *parent = 0);
    ~SetPropertyCommand();

    virtual void redo();
    virtual void undo();

private:
    Document    *m_document;
    QString     m_name;
    QVariant    m_value;
    QVariant    m_oldValue;
};


class AddBackgroundImageCommand : public QUndoCommand
{
public:
    AddBackgroundImageCommand(Document *, BackgroundImage *, MainWindow *);
    ~AddBackgroundImageCommand();

    virtual void redo();
    virtual void undo();

private:
    Document        *m_document;
    BackgroundImage *m_backgroundImage;
    MainWindow      *m_mainWindow;
};


class FitBackgroundImageCommand : public QUndoCommand
{
public:
    FitBackgroundImageCommand(Document *, BackgroundImage *, const QRect &);
    ~FitBackgroundImageCommand();

    virtual void redo();
    virtual void undo();

private:
    Document        *m_document;
    BackgroundImage *m_backgroundImage;
    QRect           m_rect;
};


class ShowBackgroundImageCommand : public QUndoCommand
{
public:
    ShowBackgroundImageCommand(Document *, BackgroundImage *, bool);
    ~ShowBackgroundImageCommand();

    virtual void redo();
    virtual void undo();

private:
    Document        *m_document;
    BackgroundImage *m_backgroundImage;
    bool            m_visible;
};


class RemoveBackgroundImageCommand : public QUndoCommand
{
public:
    RemoveBackgroundImageCommand(Document *, BackgroundImage *, MainWindow *);
    ~RemoveBackgroundImageCommand();

    virtual void redo();
    virtual void undo();

private:
    Document        *m_document;
    BackgroundImage *m_backgroundImage;
    MainWindow      *m_mainWindow;
};


class AddDocumentFlossCommand : public QUndoCommand
{
public:
    AddDocumentFlossCommand(Document *, int, DocumentFloss *, QUndoCommand *parent = 0);
    ~AddDocumentFlossCommand();

    virtual void redo();
    virtual void undo();

private:
    Document        *m_document;
    int             m_key;
    DocumentFloss   *m_documentFloss;
};


class RemoveDocumentFlossCommand : public QUndoCommand
{
public:
    RemoveDocumentFlossCommand(Document *, int, DocumentFloss *);
    ~RemoveDocumentFlossCommand();

    virtual void redo();
    virtual void undo();

private:
    Document        *m_document;
    int             m_key;
    DocumentFloss   *m_documentFloss;
};


class ReplaceDocumentFlossCommand : public QUndoCommand
{
public:
    ReplaceDocumentFlossCommand(Document *, int, DocumentFloss *);
    ~ReplaceDocumentFlossCommand();

    virtual void redo();
    virtual void undo();

private:
    Document        *m_document;
    int             m_key;
    DocumentFloss   *m_documentFloss;
};


class ResizeDocumentCommand : public QUndoCommand
{
public:
    ResizeDocumentCommand(Document *, int, int, QUndoCommand *parent = 0);
    ~ResizeDocumentCommand();

    void redo();
    void undo();

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
    ~CropToPatternCommand();

    void redo();
    void undo();

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
    ~CropToSelectionCommand();

    void redo();
    void undo();

private:
    Document    *m_document;
    QRect       m_selectionArea;
    QByteArray  m_originalPattern;
};


class ExtendPatternCommand : public QUndoCommand
{
public:
    ExtendPatternCommand(Document *, int, int, int, int);
    ~ExtendPatternCommand();

    void redo();
    void undo();

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
    ~CentrePatternCommand();

    void redo();
    void undo();

private:
    Document    *m_document;
    int         m_xOffset;
    int         m_yOffset;
};


class UpdateDocumentPaletteCommand : public QUndoCommand
{
public:
    UpdateDocumentPaletteCommand(Document *, const DocumentPalette &);
    ~UpdateDocumentPaletteCommand();

    void redo();
    void undo();

private:
    Document        *m_document;
    DocumentPalette m_palette;
};


class ChangeSchemeCommand : public QUndoCommand
{
public:
    ChangeSchemeCommand(Document *, const QString &, QUndoCommand *parent = 0);
    ~ChangeSchemeCommand();

    void redo();
    void undo();

private:
    Document    *m_document;
    QString     m_schemeName;
    QByteArray  m_originalPalette;
};


class EditorReadDocumentSettingsCommand : public QUndoCommand
{
public:
    explicit EditorReadDocumentSettingsCommand(Editor *);
    ~EditorReadDocumentSettingsCommand();

    void redo();
    void undo();

private:
    Editor  *m_editor;
};


class PaletteReadDocumentSettingsCommand : public QUndoCommand
{
public:
    explicit PaletteReadDocumentSettingsCommand(Palette *);
    ~PaletteReadDocumentSettingsCommand();

    void redo();
    void undo();

private:
    Palette *m_palette;
};


class PreviewReadDocumentSettingsCommand : public QUndoCommand
{
public:
    explicit PreviewReadDocumentSettingsCommand(Preview *);
    ~PreviewReadDocumentSettingsCommand();

    void redo();
    void undo();

private:
    Preview *m_preview;
};


class PaletteReplaceColorCommand : public QUndoCommand
{
public:
    PaletteReplaceColorCommand(Document *document, int, int);
    ~PaletteReplaceColorCommand();

    void redo();
    void undo();

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
    ~PaletteSwapColorCommand();

    void redo();
    void undo();

private:
    Document    *m_document;
    int         m_originalIndex;
    int         m_swappedIndex;
};


class UpdatePrinterConfigurationCommand : public QUndoCommand
{
public:
    UpdatePrinterConfigurationCommand(Document *, const PrinterConfiguration &);
    ~UpdatePrinterConfigurationCommand();

    void redo();
    void undo();

private:
    Document                *m_document;
    PrinterConfiguration    m_printerConfiguration;
};


class EditCutCommand : public QUndoCommand
{
public:
    EditCutCommand(Document *document, const QRect &selectionArea, int colorMask, const QList<Stitch::Type> &stitchMasks, bool excludeBackstitches, bool excludeKnots);
    ~EditCutCommand();

    void redo();
    void undo();

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
    ~EditPasteCommand();

    void redo();
    void undo();

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
    ~MirrorSelectionCommand();

    virtual void redo();
    virtual void undo();

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
    ~RotateSelectionCommand();

    void redo();
    void undo();

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
    ~AlphabetCommand();

    void redo();
    void undo();

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
    ~ConfigurationCommand();

    void redo();
    void undo();

private:
    MainWindow  *m_mainWindow;
};


#endif // Commands_H
