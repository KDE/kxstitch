/*
 * Copyright (C) 2010-2015 by Stephen Allewell
 * steve.allewell@gmail.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#ifndef Editor_H
#define Editor_H


#include <QStack>
#include <QWidget>

#include "Stitch.h"

#include "configuration.h"
#include "Renderer.h"
#include "StitchData.h"


class QUndoCommand;

class Document;
class LibraryManagerDlg;
class Pattern;
class Preview;
class Renderer;
class Scale;


class Editor : public QWidget
{
    Q_OBJECT

public:
    enum SelectedStitchType {
        StitchQuarter,
        StitchHalf,
        Stitch3Quarter,
        StitchFull,
        StitchSmallHalf,
        StitchSmallFull,
        StitchFrenchKnot
    };

    enum ToolMode {
        ToolPaint,
        ToolDraw,
        ToolErase,
        ToolRectangle,
        ToolFillRectangle,
        ToolEllipse,
        ToolFillEllipse,
        ToolFillPolygon,
        ToolText,
        ToolAlphabet,
        ToolSelect,
        ToolBackstitch,
        ToolColorPicker,
        ToolPaste,
        ToolMirror,
        ToolRotate
    };

    explicit Editor(QWidget*);
    virtual ~Editor() = default;

    void setDocument(Document*);
    Document *document();

    void setPreview(Preview*);

    void readDocumentSettings();

    Scale *horizontalScale();
    Scale *verticalScale();

    QRect selectionArea();
    void resetSelectionArea();

signals:
    void selectionMade(bool);
    void changedVisibleCells(const QRect&);

public slots:
    void libraryManager();

    void previewClicked(const QPoint&);
    void previewClicked(const QRect&);

    void zoomIn();
    void zoomOut();
    void actualSize();
    void fitToPage();
    void fitToWidth();
    void fitToHeight();

    void editCut();
    void editCopy();
    void editPaste();

    void mirrorSelection();
    void rotateSelection();

    void pastePattern(ToolMode);

    void formatScalesAsStitches();
    void formatScalesAsCM();
    void formatScalesAsInches();

    void renderStitches(bool);
    void renderBackstitches(bool);
    void renderFrenchKnots(bool);
    void renderGrid(bool);
    void renderBackgroundImages(bool);

    void renderStitchesAs();
    void renderBackstitchesAs();
    void renderKnotsAs();

    void colorHighlight(bool);

    void selectTool();
    void selectTool(ToolMode);

    void selectStitch();

    void setMaskStitch(bool);
    void setMaskColor(bool);
    void setMaskBackstitch(bool);
    void setMaskKnot(bool);
    void setMakesCopies(bool);

    void loadSettings();

    void drawContents();
    void drawContents(const QPoint &);
    void drawContents(const QRect &);

protected:
    virtual bool event(QEvent*) Q_DECL_OVERRIDE;
    virtual void contextMenuEvent(QContextMenuEvent*) Q_DECL_OVERRIDE;
    virtual void dragEnterEvent(QDragEnterEvent*) Q_DECL_OVERRIDE;
    virtual void dragLeaveEvent(QDragLeaveEvent*) Q_DECL_OVERRIDE;
    virtual void dragMoveEvent(QDragMoveEvent*) Q_DECL_OVERRIDE;
    virtual void dropEvent(QDropEvent*) Q_DECL_OVERRIDE;
    virtual void keyPressEvent(QKeyEvent*) Q_DECL_OVERRIDE;
    virtual void keyReleaseEvent(QKeyEvent*) Q_DECL_OVERRIDE;
    virtual void mousePressEvent(QMouseEvent*) Q_DECL_OVERRIDE;
    virtual void mouseMoveEvent(QMouseEvent*) Q_DECL_OVERRIDE;
    virtual void mouseReleaseEvent(QMouseEvent*) Q_DECL_OVERRIDE;
    virtual void moveEvent(QMoveEvent*) Q_DECL_OVERRIDE;
    virtual void resizeEvent(QResizeEvent*) Q_DECL_OVERRIDE;
    virtual void paintEvent(QPaintEvent*) Q_DECL_OVERRIDE;
    virtual void wheelEvent(QWheelEvent*) Q_DECL_OVERRIDE;
    virtual bool eventFilter(QObject*, QEvent*) Q_DECL_OVERRIDE;

private:
    bool zoom(double);

    void keyPressPolygon(QKeyEvent*);
    void keyPressText(QKeyEvent*);
    void keyPressAlphabet(QKeyEvent*);
    void keyPressPaste(QKeyEvent*);
    void keyPressMirror(QKeyEvent*);
    void keyPressRotate(QKeyEvent*);
    void keyPressMovePattern(QKeyEvent*);

    void toolInitPolygon();
    void toolInitText();
    void toolInitAlphabet();

    void toolCleanupPolygon();
    void toolCleanupAlphabet();
    void toolCleanupSelect();
    void toolCleanupMirror();
    void toolCleanupRotate();

    void renderBackgroundImages(QPainter &, const QRect&);
    void renderStitches(QPainter*, const QRect&);
    void renderBackstitches(QPainter*, const QRect&);
    void renderFrenchKnots(QPainter*, const QRect&);

    void renderRubberBandLine(QPainter*, const QRect&);
    void renderRubberBandRectangle(QPainter*, const QRect&);
    void renderRubberBandEllipse(QPainter*, const QRect&);
    void renderFillPolygon(QPainter*, const QRect&);
    void renderAlphabetCursor(QPainter*, const QRect&);
    void renderPasteImage(QPainter*, const QRect&);

    void mousePressEvent_Paint(QMouseEvent*);
    void mouseMoveEvent_Paint(QMouseEvent*);
    void mouseReleaseEvent_Paint(QMouseEvent*);

    void mousePressEvent_Draw(QMouseEvent*);
    void mouseMoveEvent_Draw(QMouseEvent*);
    void mouseReleaseEvent_Draw(QMouseEvent*);

    void mousePressEvent_Erase(QMouseEvent*);
    void mouseMoveEvent_Erase(QMouseEvent*);
    void mouseReleaseEvent_Erase(QMouseEvent*);

    void mousePressEvent_Rectangle(QMouseEvent*);
    void mouseMoveEvent_Rectangle(QMouseEvent*);
    void mouseReleaseEvent_Rectangle(QMouseEvent*);

    void mousePressEvent_FillRectangle(QMouseEvent*);
    void mouseMoveEvent_FillRectangle(QMouseEvent*);
    void mouseReleaseEvent_FillRectangle(QMouseEvent*);

    void mousePressEvent_Ellipse(QMouseEvent*);
    void mouseMoveEvent_Ellipse(QMouseEvent*);
    void mouseReleaseEvent_Ellipse(QMouseEvent*);

    void mousePressEvent_FillEllipse(QMouseEvent*);
    void mouseMoveEvent_FillEllipse(QMouseEvent*);
    void mouseReleaseEvent_FillEllipse(QMouseEvent*);

    void mousePressEvent_FillPolygon(QMouseEvent*);
    void mouseMoveEvent_FillPolygon(QMouseEvent*);
    void mouseReleaseEvent_FillPolygon(QMouseEvent*);

    void mousePressEvent_Text(QMouseEvent*);
    void mouseMoveEvent_Text(QMouseEvent*);
    void mouseReleaseEvent_Text(QMouseEvent*);

    void mousePressEvent_Alphabet(QMouseEvent*);
    void mouseMoveEvent_Alphabet(QMouseEvent*);
    void mouseReleaseEvent_Alphabet(QMouseEvent*);

    void mousePressEvent_Select(QMouseEvent*);
    void mouseMoveEvent_Select(QMouseEvent*);
    void mouseReleaseEvent_Select(QMouseEvent*);

    void mousePressEvent_Backstitch(QMouseEvent*);
    void mouseMoveEvent_Backstitch(QMouseEvent*);
    void mouseReleaseEvent_Backstitch(QMouseEvent*);

    void mousePressEvent_ColorPicker(QMouseEvent*);
    void mouseMoveEvent_ColorPicker(QMouseEvent*);
    void mouseReleaseEvent_ColorPicker(QMouseEvent*);

    void mousePressEvent_Paste(QMouseEvent*);
    void mouseMoveEvent_Paste(QMouseEvent*);
    void mouseReleaseEvent_Paste(QMouseEvent*);

    void mousePressEvent_Mirror(QMouseEvent*);
    void mouseMoveEvent_Mirror(QMouseEvent*);
    void mouseReleaseEvent_Mirror(QMouseEvent*);

    void mousePressEvent_Rotate(QMouseEvent*);
    void mouseMoveEvent_Rotate(QMouseEvent*);
    void mouseReleaseEvent_Rotate(QMouseEvent*);

    QPoint contentsToCell(const QPoint&) const;
    int contentsToZone(const QPoint&) const;
    QPoint contentsToSnap(const QPoint&) const;
    QRect snapToCells(const QPoint&) const;
    QRect cellToRect(const QPoint&) const;
    QRect polygonToCells(const QPolygon&) const;
    QRect rectToContents(const QRect&) const;

    void processBitmap(QUndoCommand*, const QBitmap&);
    QRect visibleCells();
    QList<Stitch::Type> maskStitches() const;

    Document    *m_document;
    Preview     *m_preview;

    Renderer    m_renderer;

    Scale       *m_horizontalScale;
    Scale       *m_verticalScale;
    Configuration::EnumEditor_FormatScalesAs::type m_formatScalesAs;

    LibraryManagerDlg   *m_libraryManagerDlg;

    double  m_zoomFactor;
    double  m_cellWidth;
    double  m_cellHeight;
    double  m_horizontalClothCount;
    double  m_verticalClothCount;
    int     m_cellHorizontalGrouping;
    int     m_cellVerticalGrouping;

    enum ToolMode   m_toolMode;
    enum ToolMode   m_oldToolMode;

    bool    m_renderBackgroundImages;
    bool    m_renderGrid;
    bool    m_renderStitches;
    bool    m_renderBackstitches;
    bool    m_renderFrenchKnots;

    bool    m_maskStitch;
    bool    m_maskColor;
    bool    m_maskBackstitch;
    bool    m_maskKnot;
    bool    m_makesCopies;

    Qt::Orientation         m_orientation;
    StitchData::Rotation    m_rotation;

    QPoint  m_cellStart;
    QPoint  m_cellTracking;
    QPoint  m_cellEnd;

    QPoint  m_pasteOffset;

    int m_zoneStart;
    int m_zoneTracking;
    int m_zoneEnd;

    QPolygon    m_polygon;

    QUndoCommand    *m_activeCommand;

    enum SelectedStitchType m_currentStitchType;

    Configuration::EnumRenderer_RenderStitchesAs::type      m_renderStitchesAs;
    Configuration::EnumRenderer_RenderBackstitchesAs::type  m_renderBackstitchesAs;
    Configuration::EnumRenderer_RenderKnotsAs::type         m_renderKnotsAs;
    bool    m_colorHighlight;

    QRect   m_rubberBand;
    QRect   m_selectionArea;

    QByteArray  m_pasteData;
    Pattern     *m_pastePattern;

    QImage      m_cachedContents;

    QStack<QPoint>  m_cursorStack;
    QMap<int, int>  m_cursorCommands;

    typedef void (Editor::*keyPressCallPointer)(QKeyEvent*);
    typedef void (Editor::*toolInitCallPointer)();
    typedef void (Editor::*toolCleanupCallPointer)();
    typedef void (Editor::*mouseEventCallPointer)(QMouseEvent*);
    typedef void (Editor::*renderToolSpecificGraphicsCallPointer)(QPainter*, const QRect&);

    static const keyPressCallPointer    keyPressCallPointers[];

    static const toolInitCallPointer    toolInitCallPointers[];
    static const toolCleanupCallPointer toolCleanupCallPointers[];

    static const mouseEventCallPointer  mousePressEventCallPointers[];
    static const mouseEventCallPointer  mouseMoveEventCallPointers[];
    static const mouseEventCallPointer  mouseReleaseEventCallPointers[];

    static const renderToolSpecificGraphicsCallPointer  renderToolSpecificGraphics[];
};


#endif // Editor_H
