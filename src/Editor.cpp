/*
 * Copyright (C) 2010-2015 by Stephen Allewell
 * steve.allewell@gmail.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#include "Editor.h"

#include <QAction>
#include <QApplication>
#include <QBitmap>
#include <QClipboard>
#include <QContextMenuEvent>
#include <QMenu>
#include <QMimeData>
#include <QMouseEvent>
#include <QPainter>
#include <QRubberBand>
#include <QScrollArea>
#include <QStyleOptionRubberBand>
#include <QToolTip>

#include <KLocalizedString>
#include <KMessageBox>
#include <KXMLGUIFactory>

#include <math.h>

#include "BackgroundImage.h"
#include "Commands.h"
#include "Document.h"
#include "Floss.h"
#include "FlossScheme.h"
#include "LibraryManagerDlg.h"
#include "LibraryPattern.h"
#include "LibraryTreeWidgetItem.h"
#include "MainWindow.h"
#include "Palette.h"
#include "Preview.h"
#include "Scale.h"
#include "SchemeManager.h"
#include "TextToolDlg.h"


const Editor::keyPressCallPointer Editor::keyPressCallPointers[] = {
    0,                          // Paint
    0,                          // Draw
    0,                          // Erase
    0,                          // Rectangle
    0,                          // Fill Rectangle
    0,                          // Ellipse
    0,                          // Fill Ellipse
    &Editor::keyPressPolygon,   // Fill Polygon
    &Editor::keyPressText,      // Text
    &Editor::keyPressAlphabet,  // Alphabet
    0,                          // Select
    0,                          // Backstitch
    0,                          // Color Picker
    &Editor::keyPressPaste,     // Paste
    &Editor::keyPressMirror,    // Mirror
    &Editor::keyPressRotate     // Rotate
};

const Editor::toolInitCallPointer Editor::toolInitCallPointers[] = {
    0,                          // Paint
    0,                          // Draw
    0,                          // Erase
    0,                          // Rectangle
    0,                          // Fill Rectangle
    0,                          // Ellipse
    0,                          // Fill Ellipse
    &Editor::toolInitPolygon,   // Fill Polygon
    &Editor::toolInitText,      // Text
    &Editor::toolInitAlphabet,  // Alphabet
    0,                          // Select
    0,                          // Backstitch
    0,                          // Color Picker
    0,                          // Paste
    0,                          // Mirror
    0                           // Rotate
};


const Editor::toolCleanupCallPointer Editor::toolCleanupCallPointers[] = {
    0,                              // Paint
    0,                              // Draw
    0,                              // Erase
    0,                              // Rectangle
    0,                              // Fill Rectangle
    0,                              // Ellipse
    0,                              // Fill Ellipse
    &Editor::toolCleanupPolygon,    // Fill Polygon
    0,                              // Text
    &Editor::toolCleanupAlphabet,   // Alphabet
    &Editor::toolCleanupSelect,     // Select
    0,                              // Backstitch
    0,                              // Color Picker
    0,                              // Paste
    &Editor::toolCleanupMirror,     // Mirror
    &Editor::toolCleanupRotate      // Rotate
};


const Editor::mouseEventCallPointer Editor::mousePressEventCallPointers[] = {
    &Editor::mousePressEvent_Paint,
    &Editor::mousePressEvent_Draw,
    &Editor::mousePressEvent_Erase,
    &Editor::mousePressEvent_Rectangle,
    &Editor::mousePressEvent_FillRectangle,
    &Editor::mousePressEvent_Ellipse,
    &Editor::mousePressEvent_FillEllipse,
    &Editor::mousePressEvent_FillPolygon,
    &Editor::mousePressEvent_Text,
    &Editor::mousePressEvent_Alphabet,
    &Editor::mousePressEvent_Select,
    &Editor::mousePressEvent_Backstitch,
    &Editor::mousePressEvent_ColorPicker,
    &Editor::mousePressEvent_Paste,
    &Editor::mousePressEvent_Mirror,
    &Editor::mousePressEvent_Rotate
};

const Editor::mouseEventCallPointer Editor::mouseMoveEventCallPointers[] = {
    &Editor::mouseMoveEvent_Paint,
    &Editor::mouseMoveEvent_Draw,
    &Editor::mouseMoveEvent_Erase,
    &Editor::mouseMoveEvent_Rectangle,
    &Editor::mouseMoveEvent_FillRectangle,
    &Editor::mouseMoveEvent_Ellipse,
    &Editor::mouseMoveEvent_FillEllipse,
    &Editor::mouseMoveEvent_FillPolygon,
    &Editor::mouseMoveEvent_Text,
    &Editor::mouseMoveEvent_Alphabet,
    &Editor::mouseMoveEvent_Select,
    &Editor::mouseMoveEvent_Backstitch,
    &Editor::mouseMoveEvent_ColorPicker,
    &Editor::mouseMoveEvent_Paste,
    &Editor::mouseMoveEvent_Mirror,
    &Editor::mouseMoveEvent_Rotate
};

const Editor::mouseEventCallPointer Editor::mouseReleaseEventCallPointers[] = {
    &Editor::mouseReleaseEvent_Paint,
    &Editor::mouseReleaseEvent_Draw,
    &Editor::mouseReleaseEvent_Erase,
    &Editor::mouseReleaseEvent_Rectangle,
    &Editor::mouseReleaseEvent_FillRectangle,
    &Editor::mouseReleaseEvent_Ellipse,
    &Editor::mouseReleaseEvent_FillEllipse,
    &Editor::mouseReleaseEvent_FillPolygon,
    &Editor::mouseReleaseEvent_Text,
    &Editor::mouseReleaseEvent_Alphabet,
    &Editor::mouseReleaseEvent_Select,
    &Editor::mouseReleaseEvent_Backstitch,
    &Editor::mouseReleaseEvent_ColorPicker,
    &Editor::mouseReleaseEvent_Paste,
    &Editor::mouseReleaseEvent_Mirror,
    &Editor::mouseReleaseEvent_Rotate
};

const Editor::renderToolSpecificGraphicsCallPointer Editor::renderToolSpecificGraphics[] = {
    0,                                  // Paint
    &Editor::renderRubberBandLine,      // Draw
    0,                                  // Erase
    &Editor::renderRubberBandRectangle, // Rectangle
    &Editor::renderRubberBandRectangle, // Fill Rectangle
    &Editor::renderRubberBandEllipse,   // Ellipse
    &Editor::renderRubberBandEllipse,   // Fill Ellipse
    &Editor::renderFillPolygon,         // Fill Polygon
    &Editor::renderPasteImage,          // Text
    &Editor::renderAlphabetCursor,      // Alphabet
    &Editor::renderRubberBandRectangle, // Select
    &Editor::renderRubberBandLine,      // Backstitch
    0,                                  // Color Picker
    &Editor::renderPasteImage,          // Paste
    &Editor::renderPasteImage,          // Mirror - Paste performs the same functions
    &Editor::renderPasteImage           // Rotate - Paste performs the same functions
};


Editor::Editor(QWidget *parent)
    :   QWidget(parent),
        m_horizontalScale(new Scale(Qt::Horizontal)),
        m_verticalScale(new Scale(Qt::Vertical)),
        m_libraryManagerDlg(nullptr),
        m_zoomFactor(Configuration::editor_DefaultZoomFactor()),
        m_toolMode(ToolPaint),
        m_renderBackgroundImages(Configuration::renderer_RenderBackgroundImages()),
        m_renderGrid(Configuration::renderer_RenderGrid()),
        m_renderStitches(Configuration::renderer_RenderStitches()),
        m_renderBackstitches(Configuration::renderer_RenderBackstitches()),
        m_renderFrenchKnots(Configuration::renderer_RenderFrenchKnots()),
        m_maskStitch(false),
        m_maskColor(false),
        m_maskBackstitch(false),
        m_maskKnot(false),
        m_makesCopies(Configuration::tool_MakesCopies()),
        m_activeCommand(nullptr),
        m_colorHighlight(Configuration::renderer_ColorHilight()),
        m_pastePattern(nullptr)
{
    setAcceptDrops(true);
    setFocusPolicy(Qt::StrongFocus);
    setMouseTracking(true);
}


void Editor::setDocument(Document *document)
{
    m_document = document;
    readDocumentSettings();
}


Document *Editor::document()
{
    return m_document;
}


void Editor::setPreview(Preview *preview)
{
    m_preview = preview;
}


Scale *Editor::horizontalScale()
{
    return m_horizontalScale;
}


Scale *Editor::verticalScale()
{
    return m_verticalScale;
}


void Editor::readDocumentSettings()
{
    m_cellHorizontalGrouping = m_document->property(QStringLiteral("cellHorizontalGrouping")).toInt();
    m_cellVerticalGrouping = m_document->property(QStringLiteral("cellVerticalGrouping")).toInt();

    m_horizontalClothCount = m_document->property(QStringLiteral("horizontalClothCount")).toDouble();
    m_verticalClothCount = m_document->property(QStringLiteral("verticalClothCount")).toDouble();

    m_horizontalScale->setCellGrouping(m_cellHorizontalGrouping);
    m_horizontalScale->setCellCount(m_document->pattern()->stitches().width());
    m_horizontalScale->setClothCount(m_horizontalClothCount);
    m_horizontalScale->setClothCountUnits(static_cast<Configuration::EnumEditor_ClothCountUnits::type>(m_document->property(QStringLiteral("clothCountUnits")).toInt()));
    m_horizontalScale->setUnits(m_formatScalesAs);

    m_verticalScale->setCellGrouping(m_cellVerticalGrouping);
    m_verticalScale->setCellCount(m_document->pattern()->stitches().height());
    m_verticalScale->setClothCount(m_verticalClothCount);
    m_verticalScale->setClothCountUnits(static_cast<Configuration::EnumEditor_ClothCountUnits::type>(m_document->property(QStringLiteral("clothCountUnits")).toInt()));
    m_verticalScale->setUnits(m_formatScalesAs);

    m_renderer.setCellGrouping(m_cellHorizontalGrouping, m_cellVerticalGrouping);
    m_renderer.setGridLineWidths(Configuration::editor_ThinLineWidth(), Configuration::editor_ThickLineWidth());
    m_renderer.setGridLineColors(m_document->property(QStringLiteral("thinLineColor")).value<QColor>(), m_document->property(QStringLiteral("thickLineColor")).value<QColor>());

    zoom(m_zoomFactor);

    drawContents();
}


void Editor::drawContents()
{
    drawContents(visibleCells());
}


void Editor::drawContents(const QPoint &cell)
{
    drawContents(QRect(cell, QSize(1, 1)));
}


void Editor::drawContents(const QRect &cells)
{
    if (!updatesEnabled() || (m_document == nullptr) || m_cachedContents.isNull()) {
        return;
    }

    QPainter painter(&m_cachedContents);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setCompositionMode(QPainter::CompositionMode_Source);
    painter.setWindow(0, 0, m_document->pattern()->stitches().width(), m_document->pattern()->stitches().height());
    painter.fillRect(cells, m_document->property(QStringLiteral("fabricColor")).value<QColor>());

    if (m_renderBackgroundImages) {
        renderBackgroundImages(painter, cells);
    }

    m_renderer.render(&painter,
                      m_document->pattern(),
                      cells,
                      m_renderGrid,
                      m_renderStitches,
                      m_renderBackstitches,
                      m_renderFrenchKnots,
                      (m_colorHighlight) ? m_document->pattern()->palette().currentIndex() : -1);

    painter.end();

    update();
}


void Editor::libraryManager()
{
    if (m_libraryManagerDlg == nullptr) {
        m_libraryManagerDlg = new LibraryManagerDlg(this);
        m_libraryManagerDlg->setCellSize(m_cellWidth, m_cellHeight);
    }

    m_libraryManagerDlg->show();
}


void Editor::previewClicked(const QPoint &cell)
{
    QRect contentsRect = parentWidget()->contentsRect();
    dynamic_cast<QScrollArea *>(parentWidget()->parentWidget())->ensureVisible(cell.x()*m_cellWidth, cell.y()*m_cellHeight, contentsRect.width() / 2, contentsRect.height() / 2);
}


void Editor::previewClicked(const QRect &cells)
{
    int documentWidth = m_document->pattern()->stitches().width();
    int documentHeight = m_document->pattern()->stitches().height();
    int left = cells.left();
    int top = cells.top();
    int right = std::min(cells.right(), documentWidth - 1);
    int bottom = std::min(cells.bottom(), documentHeight - 1);

    QRect visibleArea = parentWidget()->contentsRect();
    double visibleWidth = visibleArea.width();
    double visibleHeight = visibleArea.height();
    bool clothCountUnitsInches = (static_cast<Configuration::EnumEditor_ClothCountUnits::type>(m_document->property(QStringLiteral("clothCountUnits")).toInt()) == Configuration::EnumEditor_ClothCountUnits::Inches);
    double widthScaleFactor = visibleWidth / (right - left) * ((clothCountUnitsInches) ? m_horizontalClothCount : m_horizontalClothCount * 2.54) / physicalDpiX();
    double heightScaleFactor = visibleHeight / (bottom - top) * ((clothCountUnitsInches) ? m_verticalClothCount : m_verticalClothCount * 2.54) / physicalDpiY();

    zoom(std::min(widthScaleFactor, heightScaleFactor));

    previewClicked(cells.center());
}


bool Editor::zoom(double factor)
{
    if (factor < Configuration::editor_MinimumZoomFactor() || factor > Configuration::editor_MaximumZoomFactor()) {
        return false;
    }

    m_zoomFactor = factor;

    double dpiX = logicalDpiX();
    double dpiY = logicalDpiY();

    bool clothCountUnitsInches = (static_cast<Configuration::EnumEditor_ClothCountUnits::type>(m_document->property(QStringLiteral("clothCountUnits")).toInt()) == Configuration::EnumEditor_ClothCountUnits::Inches);

    m_cellWidth = dpiX * factor / ((clothCountUnitsInches) ? m_horizontalClothCount : m_horizontalClothCount * 2.54);
    m_cellHeight = dpiY * factor / ((clothCountUnitsInches) ? m_verticalClothCount : m_verticalClothCount * 2.54);

    m_horizontalScale->setCellSize(m_cellWidth);
    m_verticalScale->setCellSize(m_cellHeight);

    m_horizontalScale->setOffset(pos().x());
    m_verticalScale->setOffset(pos().y());

    if (m_libraryManagerDlg) {
        m_libraryManagerDlg->setCellSize(m_cellWidth, m_cellHeight);
    }

    int cacheWidth = int(m_cellWidth * m_document->pattern()->stitches().width());
    int cacheHeight = int(m_cellHeight * m_document->pattern()->stitches().height());

    this->resize(cacheWidth, cacheHeight);

    emit changedVisibleCells(visibleCells());

    return true;
}


void Editor::zoomIn()
{
    zoom(m_zoomFactor * 1.2);
}


void Editor::zoomOut()
{
    zoom(m_zoomFactor / 1.2);
}


void Editor::actualSize()
{
    zoom(1.0);
}


void Editor::fitToPage()
{
    int documentWidth = m_document->pattern()->stitches().width();
    int documentHeight = m_document->pattern()->stitches().height();
    QRect visibleArea = parentWidget()->contentsRect();
    double visibleWidth = visibleArea.width();
    double visibleHeight = visibleArea.height();
    bool clothCountUnitsInches = (static_cast<Configuration::EnumEditor_ClothCountUnits::type>(m_document->property(QStringLiteral("clothCountUnits")).toInt()) == Configuration::EnumEditor_ClothCountUnits::Inches);
    double widthScaleFactor = visibleWidth / documentWidth * ((clothCountUnitsInches) ? m_horizontalClothCount : m_horizontalClothCount * 2.54) / physicalDpiX();
    double heightScaleFactor = visibleHeight / documentHeight * ((clothCountUnitsInches) ? m_verticalClothCount : m_verticalClothCount * 2.54) / physicalDpiY();

    zoom(std::min(widthScaleFactor, heightScaleFactor));
}


void Editor::fitToWidth()
{
    int documentWidth = m_document->pattern()->stitches().width();
    double visibleWidth = parentWidget()->contentsRect().width();
    bool clothCountUnitsInches = (static_cast<Configuration::EnumEditor_ClothCountUnits::type>(m_document->property(QStringLiteral("clothCountUnits")).toInt()) == Configuration::EnumEditor_ClothCountUnits::Inches);
    double widthScaleFactor = visibleWidth / documentWidth * ((clothCountUnitsInches) ? m_horizontalClothCount : m_horizontalClothCount * 2.54) / physicalDpiX();

    zoom(widthScaleFactor);
}


void Editor::fitToHeight()
{
    int documentHeight = m_document->pattern()->stitches().height();
    double visibleHeight = parentWidget()->contentsRect().height();
    bool clothCountUnitsInches = (static_cast<Configuration::EnumEditor_ClothCountUnits::type>(m_document->property(QStringLiteral("clothCountUnits")).toInt()) == Configuration::EnumEditor_ClothCountUnits::Inches);
    double heightScaleFactor = visibleHeight / documentHeight * ((clothCountUnitsInches) ? m_verticalClothCount : m_verticalClothCount * 2.54) / physicalDpiY();

    zoom(heightScaleFactor);
}


QList<Stitch::Type> Editor::maskStitches() const
{
    QList<Stitch::Type> maskStitches;

    if (m_maskStitch) {
        if (m_currentStitchType == StitchFull) {
            maskStitches << Stitch::Full;
        } else {
            for (int i = 0 ; i < 4 ; ++i) {
                maskStitches << stitchMap[m_currentStitchType][i];
            }
        }
    } else {
        maskStitches << Stitch::TLQtr << Stitch::TRQtr << Stitch::BLQtr << Stitch::BTHalf << Stitch::TL3Qtr << Stitch::BRQtr
                     << Stitch::TBHalf << Stitch::TR3Qtr << Stitch::BL3Qtr << Stitch::BR3Qtr << Stitch::Full << Stitch::TLSmallHalf
                     << Stitch::TRSmallHalf << Stitch::BLSmallHalf << Stitch::BRSmallHalf << Stitch::TLSmallFull << Stitch::TRSmallFull
                     << Stitch::BLSmallFull << Stitch::BRSmallFull;
    }

    return maskStitches;
}


void Editor::editCut()
{
    m_document->undoStack().push(new EditCutCommand(m_document, m_selectionArea, (m_maskColor) ? m_document->pattern()->palette().currentIndex() : -1, maskStitches(), m_maskBackstitch, m_maskKnot));

    toolCleanupSelect();
}


void Editor::editCopy()
{
    Pattern *pattern = m_document->pattern()->copy(m_selectionArea, (m_maskColor) ? m_document->pattern()->palette().currentIndex() : -1, maskStitches(), m_maskBackstitch, m_maskKnot);

    toolCleanupSelect();

    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream << *pattern;

    QMimeData *mimeData = new QMimeData();
    mimeData->setData(QStringLiteral("application/kxstitch"), data);

    QApplication::clipboard()->setMimeData(mimeData);

    update();
}


void Editor::editPaste()
{
    m_pasteData = QApplication::clipboard()->mimeData()->data(QStringLiteral("application/kxstitch"));
    m_pastePattern = new Pattern;
    QDataStream stream(&m_pasteData, QIODevice::ReadOnly);
    stream >> *m_pastePattern;

    pastePattern(ToolPaste);
}


void Editor::pastePattern(ToolMode toolMode)
{
    m_oldToolMode = m_toolMode;
    m_toolMode = toolMode;

    m_cellStart = m_cellTracking = m_cellEnd = QPoint(0, 0);
    QPoint pos = mapFromGlobal(QCursor::pos());

    if (rect().contains(pos)) {
        m_cellStart = m_cellTracking = m_cellEnd = contentsToCell(pos);
    }

    update();
}


void Editor::mirrorSelection()
{
    m_orientation = static_cast<Qt::Orientation>(qobject_cast<QAction *>(sender())->data().toInt());

    QDataStream stream(&m_pasteData, QIODevice::WriteOnly);
    stream << m_document->pattern()->stitches();

    if (m_makesCopies) {
        m_pastePattern = m_document->pattern()->copy(m_selectionArea, (m_maskColor) ? m_document->pattern()->palette().currentIndex() : -1, maskStitches(), m_maskBackstitch, m_maskKnot);
    } else {
        m_pastePattern = m_document->pattern()->cut(m_selectionArea, (m_maskColor) ? m_document->pattern()->palette().currentIndex() : -1, maskStitches(), m_maskBackstitch, m_maskKnot);
    }

    toolCleanupSelect();

    m_pastePattern->stitches().mirror(m_orientation);
    pastePattern(ToolMirror);
}


void Editor::rotateSelection()
{
    m_rotation = static_cast<StitchData::Rotation>(qobject_cast<QAction *>(sender())->data().toInt());

    QDataStream stream(&m_pasteData, QIODevice::WriteOnly);
    stream << m_document->pattern()->stitches();

    if (m_makesCopies) {
        m_pastePattern = m_document->pattern()->copy(m_selectionArea, (m_maskColor) ? m_document->pattern()->palette().currentIndex() : -1, maskStitches(), m_maskBackstitch, m_maskKnot);
    } else {
        m_pastePattern = m_document->pattern()->cut(m_selectionArea, (m_maskColor) ? m_document->pattern()->palette().currentIndex() : -1, maskStitches(), m_maskBackstitch, m_maskKnot);
    }

    toolCleanupSelect();

    m_pastePattern->stitches().rotate(m_rotation);
    pastePattern(ToolRotate);
}


void Editor::formatScalesAsStitches()
{
    m_formatScalesAs = Configuration::EnumEditor_FormatScalesAs::Stitches;
    m_horizontalScale->setUnits(m_formatScalesAs);
    m_verticalScale->setUnits(m_formatScalesAs);
}


void Editor::formatScalesAsCentimeters()
{
    m_formatScalesAs = Configuration::EnumEditor_FormatScalesAs::Centimeters;
    m_horizontalScale->setUnits(m_formatScalesAs);
    m_verticalScale->setUnits(m_formatScalesAs);
}


void Editor::formatScalesAsInches()
{
    m_formatScalesAs = Configuration::EnumEditor_FormatScalesAs::Inches;
    m_horizontalScale->setUnits(m_formatScalesAs);
    m_verticalScale->setUnits(m_formatScalesAs);
}


void Editor::renderStitches(bool show)
{
    m_renderStitches = show;
    drawContents();
}


void Editor::renderBackstitches(bool show)
{
    m_renderBackstitches = show;
    drawContents();
}


void Editor::renderFrenchKnots(bool show)
{
    m_renderFrenchKnots = show;
    drawContents();
}


void Editor::renderGrid(bool show)
{
    m_renderGrid = show;
    drawContents();
}


void Editor::renderBackgroundImages(bool show)
{
    m_renderBackgroundImages = show;
    drawContents();
}


void Editor::renderStitchesAs(Configuration::EnumRenderer_RenderStitchesAs::type type)
{
    m_renderStitchesAs = type;
    m_renderer.setRenderStitchesAs(m_renderStitchesAs);
    drawContents();
}


void Editor::renderBackstitchesAs(Configuration::EnumRenderer_RenderBackstitchesAs::type type)
{
    m_renderBackstitchesAs = type;
    m_renderer.setRenderBackstitchesAs(m_renderBackstitchesAs);
    drawContents();
}


void Editor::renderKnotsAs(Configuration::EnumRenderer_RenderKnotsAs::type type)
{
    m_renderKnotsAs = type;
    m_renderer.setRenderKnotsAs(m_renderKnotsAs);
    drawContents();
}


void Editor::colorHighlight(bool set)
{
    m_colorHighlight = set;
    drawContents();
}


void Editor::selectTool(ToolMode toolMode)
{
    if (toolCleanupCallPointers[m_toolMode]) {
        (this->*toolCleanupCallPointers[m_toolMode])();
    }

    m_toolMode = toolMode;

    if (toolInitCallPointers[m_toolMode]) {
        (this->*toolInitCallPointers[m_toolMode])();
    }
}


void Editor::selectStitch(Editor::SelectedStitchType stitchType)
{
    m_currentStitchType = stitchType;
}


void Editor::setMaskStitch(bool set)
{
    m_maskStitch = set;
}


void Editor::setMaskColor(bool set)
{
    m_maskColor = set;
}


void Editor::setMaskBackstitch(bool set)
{
    m_maskBackstitch = set;
}


void Editor::setMaskKnot(bool set)
{
    m_maskKnot = set;
}


void Editor::setMakesCopies(bool set)
{
    m_makesCopies = set;
}


void Editor::loadSettings()
{
    readDocumentSettings();
}


bool Editor::event(QEvent *e)
{
    if ((e->type() == QEvent::ToolTip) && (m_toolMode == ToolColorPicker)) {
        QHelpEvent *helpEvent = static_cast<QHelpEvent *>(e);
        int colorIndex = -1;
        QPoint cell = contentsToCell(helpEvent->pos());
        int zone = contentsToZone(helpEvent->pos());
        StitchQueue *queue = m_document->pattern()->stitches().stitchQueueAt(cell);

        if (queue) {
            Stitch::Type type = stitchMap[0][zone];
            QListIterator<Stitch *> q(*queue);

            while (q.hasNext()) {
                Stitch *stitch = q.next();

                if (stitch->type & type) {
                    colorIndex = stitch->colorIndex;
                    break;
                }
            }
        }

        if (colorIndex != -1) {
            const DocumentFloss *documentFloss = m_document->pattern()->palette().flosses().value(colorIndex);
            Floss *floss = SchemeManager::scheme(m_document->pattern()->palette().schemeName())->find(documentFloss->flossName());
            QToolTip::showText(helpEvent->globalPos(), QString::fromLatin1("%1 %2").arg(floss->name(), floss->description()));
        } else {
            QToolTip::hideText();
            e->ignore();
        }

        return true;
    }

    return QWidget::event(e);
}


void Editor::contextMenuEvent(QContextMenuEvent *e)
{
    MainWindow *mainWindow = qobject_cast<MainWindow *>(topLevelWidget());
    QMenu *context = static_cast<QMenu *>(mainWindow->guiFactory()->container(QStringLiteral("EditorPopup"), mainWindow));
    context->popup(e->globalPos());
    e->accept();
}


void Editor::dragEnterEvent(QDragEnterEvent *e)
{
    if (e->mimeData()->hasFormat(QStringLiteral("application/kxstitch"))) {
        e->accept();
    }
}


void Editor::dragMoveEvent(QDragMoveEvent *e)
{
    if (e->mimeData()->hasFormat(QStringLiteral("application/kxstitch"))) {
        e->accept();
    }
}


void Editor::dragLeaveEvent(QDragLeaveEvent*)
{
    // don't need to do anything here
}


void Editor::dropEvent(QDropEvent *e)
{
    m_pasteData = e->mimeData()->data(QStringLiteral("application/kxstitch"));
    m_pastePattern = new Pattern;
    QDataStream stream(&m_pasteData, QIODevice::ReadOnly);
    stream >> *m_pastePattern;
    m_document->undoStack().push(new EditPasteCommand(m_document, m_pastePattern, contentsToCell(e->pos()), e->keyboardModifiers() & Qt::ShiftModifier, i18n("Drag")));
    m_pastePattern = nullptr;
    m_pasteData.clear();
    e->accept();
}


void Editor::keyPressEvent(QKeyEvent *e)
{
    if (keyPressCallPointers[m_toolMode]) {
        (this->*keyPressCallPointers[m_toolMode])(e);
    } else {
        e->ignore();
    }
}


void Editor::keyReleaseEvent(QKeyEvent*)
{
}


void Editor::keyPressPolygon(QKeyEvent *e)
{
    switch (e->key()) {
    case Qt::Key_Backspace:
        if (!m_polygon.isEmpty()) {
            m_polygon.pop_back();
            update();
        }

        e->accept();
        break;

    case Qt::Key_Escape:
        if (!m_polygon.isEmpty()) {
            m_polygon.clear();
            update();
        }

        e->accept();
        break;

    default:
        e->ignore();
        break;
    }
}


void Editor::keyPressText(QKeyEvent *e)
{
    switch (e->key()) {
    case Qt::Key_Return:
    case Qt::Key_Enter:
        m_document->undoStack().push(new EditPasteCommand(m_document, m_pastePattern, m_cellEnd, (e->modifiers() & Qt::ShiftModifier), i18n("Text")));
        m_pastePattern = nullptr;
        m_pasteData.clear();
        e->accept();
        selectTool(m_oldToolMode);
        break;

    default:
        keyPressMovePattern(e);
        break;
    }
}


void Editor::keyPressAlphabet(QKeyEvent *e)
{
    Qt::KeyboardModifiers modifiers = e->modifiers();

    if (m_keyInfo.isKeyPressed(Qt::Key_CapsLock) && Configuration::alphabet_UseCapsLock()) {
        modifiers = static_cast<Qt::KeyboardModifiers>(modifiers ^ Qt::ShiftModifier);
    }

    int width = m_document->pattern()->stitches().width();
    int height = m_document->pattern()->stitches().height();
    LibraryPattern *libraryPattern = nullptr;

    if (m_libraryManagerDlg->currentLibrary()) {

        switch (e->key()) {
        case Qt::Key_Backspace:
            if (m_cursorStack.count() > 1) {
                m_cellEnd = m_cursorStack.pop();
                m_cursorCommands.remove(m_cursorStack.count());
                update();
                int commandsToUndo = m_cursorCommands[m_cursorStack.count() - 1];

                while (commandsToUndo--) {
                    QUndoCommand *cmd = static_cast<AlphabetCommand *>(m_activeCommand)->pop();
                    m_cursorCommands[m_cursorStack.count() - 1]--;
                    delete cmd;
                }
            }

            e->accept();
            break;

        case Qt::Key_Return:
        case Qt::Key_Enter:
            m_cellTracking = QPoint(m_cursorStack.at(0).x(), m_cursorStack.top().y() + m_libraryManagerDlg->currentLibrary()->maxHeight() + Configuration::alphabet_LineSpacing());

            if (m_cellTracking.y() >= height) {
                static_cast<AlphabetCommand *>(m_activeCommand)->push(new ExtendPatternCommand(m_document, 0, 0, 0, m_cellTracking.y() - height + Configuration::alphabet_ExtendPatternHeight()));
                m_cursorCommands[m_cursorStack.count() - 1]++;
            }

            m_cellEnd = m_cursorStack.top();
            m_cursorStack.push(m_cellTracking);
            update();
            e->accept();
            break;

        default:
            libraryPattern = m_libraryManagerDlg->currentLibrary()->findCharacter(e->key(), modifiers);

            if (libraryPattern) {
                if ((m_cursorStack.top() + QPoint(libraryPattern->pattern()->stitches().width(), 0)).x() >= width) {
                    if (m_cursorCommands[m_cursorStack.count() - 2]) {
                        static_cast<AlphabetCommand *>(m_activeCommand)->push(new ExtendPatternCommand(m_document, 0, 0, m_cursorStack.top().x() + libraryPattern->pattern()->stitches().width() - width + Configuration::alphabet_ExtendPatternWidth(), 0));
                        m_cursorCommands[m_cursorStack.count() - 1]++;
                    } else {
                        m_cellTracking = QPoint(m_cursorStack.at(0).x(), m_cursorStack.top().y() + m_libraryManagerDlg->currentLibrary()->maxHeight() + Configuration::alphabet_LineSpacing());

                        if (m_cellTracking.y() >= height) {
                            static_cast<AlphabetCommand *>(m_activeCommand)->push(new ExtendPatternCommand(m_document, 0, 0, 0, m_cellTracking.y() - height + Configuration::alphabet_ExtendPatternHeight()));
                            m_cursorCommands[m_cursorStack.count() - 1]++;
                        }

                        m_cellEnd = m_cursorStack.top();
                        m_cursorStack.push(m_cellTracking);
                        update();
                    }
                }

                QPoint insertionPoint = m_cursorStack.top() - QPoint(0, libraryPattern->pattern()->stitches().height() - 1 - libraryPattern->baseline());
                static_cast<AlphabetCommand *>(m_activeCommand)->push(new EditPasteCommand(m_document, libraryPattern->pattern(), insertionPoint, true, i18n("Add Character")));
                m_cursorCommands[m_cursorStack.count() - 1]++;
                m_cursorStack.push(m_cursorStack.top() + QPoint(libraryPattern->pattern()->stitches().width() + 1, 0));
            } else {
                if (e->key() == Qt::Key_Space) {
                    m_cellTracking = m_cursorStack.top() + QPoint(Configuration::alphabet_SpaceWidth(), 0);

                    if (m_cellTracking.x() >= width) {
                        if (Configuration::alphabet_WordWrap()) {
                            m_cellTracking = QPoint(m_cursorStack.at(0).x(), m_cellTracking.y() + m_libraryManagerDlg->currentLibrary()->maxHeight() + Configuration::alphabet_LineSpacing());

                            if (m_cellTracking.y() >= height) {
                                static_cast<AlphabetCommand *>(m_activeCommand)->push(new ExtendPatternCommand(m_document, 0, 0, 0, m_cellTracking.y() - height + Configuration::alphabet_ExtendPatternHeight()));
                                m_cursorCommands[m_cursorStack.count() - 1]++;
                            }
                        } else {
                            static_cast<AlphabetCommand *>(m_activeCommand)->push(new ExtendPatternCommand(m_document, 0, 0, m_cellTracking.x() - width + Configuration::alphabet_ExtendPatternWidth(), 0));
                            m_cursorCommands[m_cursorStack.count() - 1]++;
                        }
                    }

                    m_cursorStack.push(m_cellTracking);
                    update();
                }
            }

            e->accept();
            break;
        }
    }

    QPoint contentPoint = rectToContents(cellToRect(m_cursorStack.top())).center();
    dynamic_cast<QScrollArea *>(parentWidget()->parentWidget())->ensureVisible(contentPoint.x(), contentPoint.y());
}


void Editor::keyPressPaste(QKeyEvent *e)
{
    switch (e->key()) {
    case Qt::Key_Return:
    case Qt::Key_Enter:
        m_document->undoStack().push(new EditPasteCommand(m_document, m_pastePattern, m_cellEnd, (e->modifiers() & Qt::ShiftModifier), i18n("Paste")));
        m_pastePattern = nullptr;
        m_pasteData.clear();
        e->accept();
        selectTool(m_oldToolMode);
        break;

    default:
        keyPressMovePattern(e);
        break;
    }
}


void Editor::keyPressMirror(QKeyEvent *e)
{
    switch (e->key()) {
    case Qt::Key_Return:
    case Qt::Key_Enter:
        m_document->undoStack().push(new MirrorSelectionCommand(m_document, m_selectionArea, (m_maskColor) ? m_document->pattern()->palette().currentIndex() : -1, maskStitches(), m_maskBackstitch, m_maskKnot, m_orientation, m_makesCopies, m_pasteData, m_pastePattern, m_cellEnd, (e->modifiers() & Qt::ShiftModifier)));
        m_pastePattern = nullptr;
        m_pasteData.clear();
        e->accept();
        selectTool(m_oldToolMode);
        break;

    default:
        keyPressMovePattern(e);
        break;
    }
}


void Editor::keyPressRotate(QKeyEvent *e)
{
    switch (e->key()) {
    case Qt::Key_Return:
    case Qt::Key_Enter:
        m_document->undoStack().push(new RotateSelectionCommand(m_document, m_selectionArea, (m_maskColor) ? m_document->pattern()->palette().currentIndex() : -1, maskStitches(), m_maskBackstitch, m_maskKnot, m_rotation, m_makesCopies, m_pasteData, m_pastePattern, m_cellEnd, (e->modifiers() & Qt::ShiftModifier)));
        m_pastePattern = nullptr;
        m_pasteData.clear();
        e->accept();
        selectTool(m_oldToolMode);
        break;

    default:
        keyPressMovePattern(e);
        break;
    }
}


void Editor::keyPressMovePattern(QKeyEvent *e)
{
    switch (e->key()) {
    case Qt::Key_Escape:
        e->accept();
        selectTool(m_oldToolMode);
        update();
        break;

    case Qt::Key_Up:
        m_cellTracking = QPoint(m_cellEnd.x(), std::max(m_cellEnd.y() - 1, 0));

        if (m_cellTracking != m_cellEnd) {
            m_cellEnd = m_cellTracking;
            update();
        }

        e->accept();
        break;

    case Qt::Key_Down:
        m_cellTracking = QPoint(m_cellEnd.x(), std::min(m_cellEnd.y() + 1, m_document->pattern()->stitches().height() - 1));

        if (m_cellTracking != m_cellEnd) {
            m_cellEnd = m_cellTracking;
            update();
        }

        e->accept();
        break;

    case Qt::Key_Left:
        m_cellTracking = QPoint(std::max(m_cellEnd.x() - 1, 0), m_cellEnd.y());

        if (m_cellTracking != m_cellEnd) {
            m_cellEnd = m_cellTracking;
            update();
        }

        e->accept();
        break;

    case Qt::Key_Right:
        m_cellTracking = QPoint(std::min(m_cellEnd.x() + 1, m_document->pattern()->stitches().width() - 1), m_cellEnd.y());

        if (m_cellTracking != m_cellEnd) {
            m_cellEnd = m_cellTracking;
            update();
        }

        e->accept();
        break;

    default:
        e->ignore();
        break;
    }
}


void Editor::toolInitPolygon()
{
    m_polygon.clear();
}


void Editor::toolInitText()
{
    QPointer<TextToolDlg> textToolDlg = new TextToolDlg(this);

    if (textToolDlg->exec()) {
        m_pastePattern = new Pattern;
        QImage image = textToolDlg->image();

        m_pastePattern->palette().setSchemeName(m_document->pattern()->palette().schemeName());
        int currentIndex = m_document->pattern()->palette().currentIndex();
        m_pastePattern->palette().add(currentIndex, new DocumentFloss(m_document->pattern()->palette().currentFloss()));
        m_pastePattern->stitches().resize(image.width(), image.height());

        int stitchesAdded = 0;

        for (int row = 0 ; row < image.height() ; ++row) {
            for (int col = 0 ; col < image.width() ; ++col) {
                if (image.pixelIndex(col, row) == 1) {
                    QPoint cell(col, row);
                    m_pastePattern->stitches().addStitch(cell, Stitch::Full, currentIndex);
                    ++stitchesAdded;
                }
            }
        }

        if (stitchesAdded) {
            pastePattern(ToolText);
        } else {
            delete m_pastePattern;
            m_pastePattern = nullptr;
        }
    } else { // dialog was cancelled, reset tool to paint
        topLevelWidget()->findChild<QAction *>(QStringLiteral("toolPaint"))->trigger();
    }
}


void Editor::toolInitAlphabet()
{
    libraryManager();
}


void Editor::toolCleanupPolygon()
{
    m_polygon.clear();
    drawContents();
}


void Editor::toolCleanupAlphabet()
{
    m_activeCommand = nullptr;
    m_cursorStack.clear();
    m_cursorCommands.clear();
}


void Editor::toolCleanupSelect()
{
    if (m_rubberBand.isValid()) {
        QRect updateArea = m_rubberBand;
        m_rubberBand = QRect();
        drawContents(updateArea.adjusted(-1, -1, 1, 1));
    }

    emit(selectionMade(false));
}


void Editor::toolCleanupMirror()
{
    delete m_pastePattern;
    m_pastePattern = nullptr;

    if (!m_pasteData.isEmpty()) {
        m_document->pattern()->stitches().clear();
        QDataStream stream(&m_pasteData, QIODevice::ReadOnly);
        stream >> m_document->pattern()->stitches();
        m_pasteData.clear();
    }

    drawContents();
}


void Editor::toolCleanupRotate()
{
    delete m_pastePattern;
    m_pastePattern = nullptr;

    if (!m_pasteData.isEmpty()) {
        m_document->pattern()->stitches().clear();
        QDataStream stream(&m_pasteData, QIODevice::ReadOnly);
        stream >> m_document->pattern()->stitches();
        m_pasteData.clear();
    }

    drawContents();
}


void Editor::mousePressEvent(QMouseEvent *e)
{
    if (!rect().contains(e->pos())) {
        return;
    }

    if ((e->buttons() & Qt::LeftButton) && ((m_document->pattern()->palette().currentIndex() != -1) ||
                                            (m_toolMode == Editor::ToolSelect) ||
                                            (m_toolMode == Editor::ToolAlphabet) ||
                                            (m_toolMode == Editor::ToolPaste))) {
        (this->*mousePressEventCallPointers[m_toolMode])(e);
    }
}


void Editor::mouseMoveEvent(QMouseEvent *e)
{
    if (!rect().contains(e->pos())) {
        return;
    }

    if ((e->buttons() & Qt::LeftButton) && ((m_document->pattern()->palette().currentIndex() != -1) ||
                                            (m_toolMode == Editor::ToolSelect) ||
                                            (m_toolMode == Editor::ToolAlphabet) ||
                                            (m_toolMode == Editor::ToolPaste))) {
        (this->*mouseMoveEventCallPointers[m_toolMode])(e);
    } else if (m_toolMode == Editor::ToolPaste ||
               m_toolMode == Editor::ToolMirror ||
               m_toolMode == Editor::ToolRotate ||
               m_toolMode == Editor::ToolText) {
        if (m_pastePattern && rectToContents(m_pastePattern->stitches().extents().translated(m_cellStart)).contains(e->pos())) {
            setCursor(Qt::SizeAllCursor);
        } else {
            setCursor(Qt::ArrowCursor);
        }
    }
}


void Editor::mouseReleaseEvent(QMouseEvent *e)
{
    if (!rect().contains(e->pos())) {
        return;
    }

    if ((m_document->pattern()->palette().currentIndex() != -1) ||
        (m_toolMode == Editor::ToolSelect) ||
        (m_toolMode == Editor::ToolAlphabet) ||
        (m_toolMode == Editor::ToolPaste)) {
        (this->*mouseReleaseEventCallPointers[m_toolMode])(e);
    }
}


void Editor::moveEvent(QMoveEvent *)
{
    drawContents();
}


void Editor::resizeEvent(QResizeEvent *e)
{
    int width = e->size().width();
    int height = e->size().height();

    if (m_cachedContents.isNull()) {
        m_cachedContents = QImage(width, height, QImage::Format_ARGB32_Premultiplied);
    } else {
        m_cachedContents = m_cachedContents.scaled(width, height);
        repaint();
    }

    drawContents();
}


void Editor::paintEvent(QPaintEvent *e)
{
    if (m_cachedContents.isNull()) {
        return;
    }

    static QPoint oldpos = pos();
    QRect dirtyRect = e->rect();

    QPainter painter(this);

    painter.fillRect(dirtyRect, Qt::white);
    painter.drawImage(dirtyRect, m_cachedContents, dirtyRect);
    painter.setWindow(0, 0, m_document->pattern()->stitches().width(), m_document->pattern()->stitches().height());

    if (renderToolSpecificGraphics[m_toolMode]) {
        (this->*renderToolSpecificGraphics[m_toolMode])(&painter, e->rect());
    }

    if (pos() != oldpos) {
        m_horizontalScale->setOffset(pos().x());
        m_verticalScale->setOffset(pos().y());
        oldpos = pos();
    }

    emit changedVisibleCells(visibleCells());
}


void Editor::wheelEvent(QWheelEvent *e)
{
    bool zoomed;
    QPoint p = e->pos();
    QPoint offset = parentWidget()->rect().center() - pos();

    setUpdatesEnabled(false);

    if (e->delta() > 0) {
        zoomed = zoom(m_zoomFactor * 1.2);
        offset -= (p - (p * 1.2));
    } else {
        zoomed = zoom(m_zoomFactor / 1.2);
        offset -= (p - (p / 1.2));
    }

    if (zoomed) {
        int marginX = parentWidget()->width() / 2;
        int marginY = parentWidget()->height() / 2;

        dynamic_cast<QScrollArea *>(parentWidget()->parentWidget())->ensureVisible(static_cast<int>(offset.x()), static_cast<int>(offset.y()), marginX, marginY);
    }

    setUpdatesEnabled(true);
    drawContents();

    e->accept();
}


bool Editor::eventFilter(QObject *object, QEvent *e)
{
    if (object == parentWidget()->parentWidget()) {
        if (e->type() == QEvent::Wheel) {
            wheelEvent(static_cast<QWheelEvent *>(e));
            return true;
        }

        if (e->type() == QEvent::Resize) {
            drawContents();
            // Don't want to intercept this, just act on it to update the editor content
        }
    }

    return false;
}


void Editor::renderBackgroundImages(QPainter &painter, const QRect &updateRectangle)
{
    auto backgroundImages = m_document->backgroundImages().backgroundImages();

    while (backgroundImages.hasNext()) {
        auto backgroundImage = backgroundImages.next();

        if (backgroundImage->isVisible()) {
            if (backgroundImage->location().intersects(updateRectangle)) {
                painter.setClipRect(updateRectangle.x(), updateRectangle.y(), updateRectangle.width(), updateRectangle.height());
                painter.drawImage(backgroundImage->location(), backgroundImage->image());
                painter.setClipping(false);
            }
        }
    }
}


void Editor::renderRubberBandLine(QPainter *painter, const QRect&)
{
    painter->save();

    if (m_rubberBand.isValid()) {
        QPen pen;
        pen.setWidthF(0.5);

        painter->setOpacity(0.5);

        if (m_toolMode == ToolBackstitch) {
            pen.setColor(m_document->pattern()->palette().currentFloss()->flossColor());
            painter->setPen(pen);
            painter->drawLine(QPointF(m_cellStart) / 2, QPointF(m_cellEnd) / 2);
        } else {
            pen.setColor(QColor(200,225,255));
            painter->setPen(pen);
            painter->drawLine(QPointF(m_cellStart) + QPointF(0.5, 0.5), QPointF(m_cellEnd) + QPointF(0.5, 0.5));
        }
    }

    painter->restore();
}


void Editor::renderRubberBandRectangle(QPainter *painter, const QRect&)
{
    painter->save();

    if (m_rubberBand.isValid()) {
        painter->setRenderHint(QPainter::Qt4CompatiblePainting, true);

        QStyleOptionRubberBand opt;
        opt.initFrom(this);
        opt.shape = QRubberBand::Rectangle;
        opt.opaque = false;
        opt.rect = m_rubberBand.adjusted(0, 0, 1, 1);

        style()->drawControl(QStyle::CE_RubberBand, &opt, painter);
    }

    painter->restore();
}


void Editor::renderRubberBandEllipse(QPainter *painter, const QRect&)
{
    painter->save();

    if (m_rubberBand.isValid()) {
        painter->setRenderHint(QPainter::Qt4CompatiblePainting, true);

        painter->setPen(Qt::NoPen);
        painter->setBrush(QColor(200,225,255));
        painter->setOpacity(0.5);
        painter->drawEllipse(m_rubberBand);

        painter->setPen(Qt::darkBlue);
        painter->setBrush(Qt::NoBrush);
        painter->drawEllipse(m_rubberBand);
    }

    painter->restore();
}


void Editor::renderFillPolygon(QPainter *painter, const QRect&)
{
    QPolygonF polyline;
    painter->save();

    painter->setPen(Qt::green);         // use green for the first point
    painter->setBrush(Qt::green);

    QVector<QPoint>::const_iterator i;

    for (i = m_polygon.constBegin() ; i != m_polygon.constEnd() ; ++i) {
        QPointF cell = QPointF(*i) + QPointF(0.5, 0.5);
        painter->drawEllipse(QRectF(-0.5, -0.5, 1, 1).translated(cell));
        painter->setPen(Qt::blue);      // use blue for subsequent points
        painter->setBrush(Qt::blue);
        polyline.append(cell);
    }

    painter->drawPolyline(polyline);
    painter->restore();
}


void Editor::renderAlphabetCursor(QPainter *painter, const QRect&)
{
    if (m_cursorStack.isEmpty()) {
        return;
    }

    painter->save();
    painter->setPen(Qt::red);
    painter->fillRect(QRect(m_cursorStack.top(), QSize(1, 1)), Qt::red);
    painter->restore();
}


void Editor::renderPasteImage(QPainter *painter, const QRect &)
{
    painter->save();

    if (m_pastePattern) {
        QPen outlinePen(Qt::red);
        outlinePen.setCosmetic(true);
        painter->translate(m_cellEnd);
        painter->setPen(outlinePen);
        QRect outline(0, 0, m_pastePattern->stitches().width(), m_pastePattern->stitches().height());
        painter->drawRect(outline);

        m_renderer.render(painter,
                           m_pastePattern,  // the pattern data to render
                           outline,         // update rectangle in cells
                           false,           // don't render the grid
                           true,            // render stitches
                           true,            // render backstitches
                           true,            // render knots
                           -1);             // all colors
    }

    painter->restore();
}


void Editor::mousePressEvent_Paint(QMouseEvent *e)
{
    QPoint p = e->pos();

    if (m_currentStitchType == StitchFrenchKnot) {
        m_cellStart = m_cellTracking = m_cellEnd = contentsToSnap(p);
        m_activeCommand = new PaintKnotsCommand(m_document);
        new AddKnotCommand(m_document, m_cellStart, m_document->pattern()->palette().currentIndex(), m_activeCommand);
        m_document->undoStack().push(m_activeCommand);
        drawContents(snapToCells(m_cellStart));
    } else {
        m_cellStart = m_cellTracking = m_cellEnd = contentsToCell(p);
        m_zoneStart = m_zoneTracking = m_zoneEnd = contentsToZone(p);
        Stitch::Type stitchType = stitchMap[m_currentStitchType][m_zoneStart];
        m_activeCommand = new PaintStitchesCommand(m_document);
        new AddStitchCommand(m_document, m_cellStart, stitchType, m_document->pattern()->palette().currentIndex(), m_activeCommand);
        m_document->undoStack().push(m_activeCommand);
        drawContents(m_cellStart);
    }
}


void Editor::mouseMoveEvent_Paint(QMouseEvent *e)
{
    QPoint p = e->pos();

    if (m_currentStitchType == StitchFrenchKnot) {
        m_cellTracking = contentsToSnap(p);

        if (m_cellTracking != m_cellStart) {
            m_cellStart = m_cellTracking;
            QUndoCommand *cmd = new AddKnotCommand(m_document, m_cellStart, m_document->pattern()->palette().currentIndex(), m_activeCommand);
            cmd->redo();
            drawContents(snapToCells(m_cellStart));
        }
    } else {
        m_cellTracking = contentsToCell(p);
        m_zoneTracking = contentsToZone(p);

        if ((m_cellTracking != m_cellStart) || (m_zoneTracking != m_zoneStart)) {
            m_cellStart = m_cellTracking;
            m_zoneStart = m_zoneTracking;
            Stitch::Type stitchType = stitchMap[m_currentStitchType][m_zoneStart];
            QUndoCommand *cmd = new AddStitchCommand(m_document, m_cellStart, stitchType, m_document->pattern()->palette().currentIndex(), m_activeCommand);
            cmd->redo();
            drawContents(m_cellStart);
        }
    }
}


void Editor::mouseReleaseEvent_Paint(QMouseEvent*)
{
    m_activeCommand = nullptr;
    m_preview->drawContents();
}


void Editor::mousePressEvent_Draw(QMouseEvent *e)
{
    m_cellStart = m_cellTracking = m_cellEnd = contentsToCell(e->pos());
    m_rubberBand = QRect();
}


void Editor::mouseMoveEvent_Draw(QMouseEvent *e)
{
    QPoint p = e->pos();

    dynamic_cast<QScrollArea *>(parentWidget()->parentWidget())->ensureVisible(p.x(), p.y());

    m_cellTracking = contentsToCell(p);
    m_cellEnd = m_cellTracking;
    m_rubberBand = QRect(m_cellStart, m_cellEnd).normalized();
    update();
}


void Editor::mouseReleaseEvent_Draw(QMouseEvent*)
{
    int bitmapWidth = m_document->pattern()->stitches().width();
    int bitmapHeight = m_document->pattern()->stitches().height();
    bool useFractionals = Configuration::toolShapes_UseFractionals();

    if (useFractionals) {
        bitmapWidth *= 2;
        bitmapHeight *= 2;
        m_cellStart *= 2;
        m_cellEnd *= 2;
    }

    if (m_cellStart != m_cellEnd) {
        QBitmap canvas(bitmapWidth, bitmapHeight);
        QPainter painter;

        canvas.fill(Qt::color0);
        painter.begin(&canvas);
        painter.setRenderHint(QPainter::Antialiasing, false);
        painter.setRenderHint(QPainter::Qt4CompatiblePainting, true);
        painter.setPen(QPen(Qt::color1));
        painter.drawLine(m_cellStart, m_cellEnd);
        painter.end();

        QUndoCommand *cmd = new DrawLineCommand(m_document);
        processBitmap(cmd, canvas);

        m_document->undoStack().push(cmd);
    }

    m_rubberBand = QRect();
}


void Editor::mousePressEvent_Erase(QMouseEvent *e)
{
    QPoint p = e->pos();
    QRect rect;
    QUndoCommand *cmd;

    if (e->modifiers() & Qt::ControlModifier) {
        // Erase a backstitch
        m_cellStart = m_cellTracking = m_cellEnd = contentsToSnap(p);
    } else {
        m_activeCommand = new EraseStitchesCommand(m_document);
        m_document->undoStack().push(m_activeCommand);

        if (e->modifiers() & Qt::ShiftModifier) {
            // Delete french knots
            m_cellStart = m_cellTracking = m_cellEnd = contentsToSnap(p);

            if (Knot *knot = m_document->pattern()->stitches().findKnot(m_cellStart, (m_maskColor) ? m_document->pattern()->palette().currentIndex() : -1)) {
                cmd = new DeleteKnotCommand(m_document, knot->position, knot->colorIndex, m_activeCommand);
                cmd->redo();
                drawContents(snapToCells(m_cellStart));
            }
        } else {
            m_cellStart = m_cellTracking = m_cellEnd = contentsToCell(p);
            m_zoneStart = m_zoneTracking = m_zoneEnd = contentsToZone(p);

            if (Stitch *stitch = m_document->pattern()->stitches().findStitch(m_cellStart, m_maskStitch ? stitchMap[m_currentStitchType][m_zoneStart] : Stitch::Delete, m_maskColor ? m_document->pattern()->palette().currentIndex() : -1)) {
                cmd = new DeleteStitchCommand(m_document, m_cellStart, m_maskStitch ? stitchMap[m_currentStitchType][m_zoneStart] : Stitch::Delete, stitch->colorIndex, m_activeCommand);
                cmd->redo();
                drawContents(cellToRect(m_cellStart).adjusted(-1, -1, 1, 1));
            }
        }
    }
}


void Editor::mouseMoveEvent_Erase(QMouseEvent *e)
{
    QPoint p = e->pos();
    QRect rect;
    QUndoCommand *cmd;

    if (e->modifiers() & Qt::ControlModifier) {
        // Erasing a backstitch
        // Don't need to do anything here
    } else {
        if (e->modifiers() & Qt::ShiftModifier) {
            // Delete french knots
            m_cellTracking = contentsToSnap(p);

            if (m_cellTracking != m_cellStart) {
                m_cellStart = m_cellTracking;

                if (Knot *knot = m_document->pattern()->stitches().findKnot(m_cellStart, (m_maskColor) ? m_document->pattern()->palette().currentIndex() : -1)) {
                    cmd = new DeleteKnotCommand(m_document, knot->position, knot->colorIndex, m_activeCommand);
                    cmd->redo();
                    drawContents(snapToCells(m_cellStart));
                }
            }
        } else {
            m_cellTracking = contentsToCell(p);
            m_zoneTracking = contentsToZone(p);

            if ((m_cellTracking != m_cellStart) || (m_zoneTracking != m_zoneStart)) {
                m_cellStart = m_cellTracking;
                m_zoneStart = m_zoneTracking;

                if (Stitch *stitch = m_document->pattern()->stitches().findStitch(m_cellStart, m_maskStitch ? stitchMap[m_currentStitchType][m_zoneStart] : Stitch::Delete, m_maskColor ? m_document->pattern()->palette().currentIndex() : -1)) {
                    cmd = new DeleteStitchCommand(m_document, m_cellStart, m_maskStitch ? stitchMap[m_currentStitchType][m_zoneStart] : Stitch::Delete, stitch->colorIndex, m_activeCommand);
                    cmd->redo();
                    drawContents(cellToRect(m_cellStart).adjusted(-1, -1, 1, 1));
                }
            }
        }
    }
}


void Editor::mouseReleaseEvent_Erase(QMouseEvent *e)
{
    if (e->modifiers() & Qt::ControlModifier) {
        // Erase a backstitch
        m_cellEnd = contentsToSnap(e->pos());

        if (Backstitch *backstitch = m_document->pattern()->stitches().findBackstitch(m_cellStart, m_cellEnd, m_maskColor ? m_document->pattern()->palette().currentIndex() : -1)) {
            m_document->undoStack().push(new DeleteBackstitchCommand(m_document, backstitch->start, backstitch->end, backstitch->colorIndex));
        }
    }

    // Nothing needs to be done for french knots or stitches which are handled in mouseMoveEvent_Erase
}


void Editor::mousePressEvent_Rectangle(QMouseEvent *e)
{
    m_cellStart = m_cellTracking = m_cellEnd = contentsToCell(e->pos());
    m_rubberBand = QRect();
}


void Editor::mouseMoveEvent_Rectangle(QMouseEvent *e)
{
    QPoint p = e->pos();

    dynamic_cast<QScrollArea *>(parentWidget()->parentWidget())->ensureVisible(p.x(), p.y());

    m_cellTracking = contentsToCell(p);
    m_cellEnd = m_cellTracking;
    m_rubberBand = QRect(m_cellStart, m_cellEnd).normalized();
    update();
}


void Editor::mouseReleaseEvent_Rectangle(QMouseEvent*)
{
    int x = m_rubberBand.left();
    int y = m_rubberBand.top();
    QPoint cell(x, y);

    QUndoCommand *cmd = new DrawRectangleCommand(m_document);

    while (++x <= m_rubberBand.right()) {
        new AddStitchCommand(m_document, cell, Stitch::Full, m_document->pattern()->palette().currentIndex(), cmd);
        cell.setX(x);
    }

    while (++y <= m_rubberBand.bottom()) {
        new AddStitchCommand(m_document, cell, Stitch::Full, m_document->pattern()->palette().currentIndex(), cmd);
        cell.setY(y);
    }

    while (--x >= m_rubberBand.left()) {
        new AddStitchCommand(m_document, cell, Stitch::Full, m_document->pattern()->palette().currentIndex(), cmd);
        cell.setX(x);
    }

    while (--y >= m_rubberBand.top()) {
        new AddStitchCommand(m_document, cell, Stitch::Full, m_document->pattern()->palette().currentIndex(), cmd);
        cell.setY(y);
    }

    m_rubberBand = QRect();     // this will clear the rubber band rectangle on the next repaint

    m_document->undoStack().push(cmd);
}


void Editor::mousePressEvent_FillRectangle(QMouseEvent *e)
{
    m_cellStart = m_cellTracking = m_cellEnd = contentsToCell(e->pos());
    m_rubberBand = QRect();
}


void Editor::mouseMoveEvent_FillRectangle(QMouseEvent *e)
{
    QPoint p = e->pos();

    dynamic_cast<QScrollArea *>(parentWidget()->parentWidget())->ensureVisible(p.x(), p.y());

    m_cellTracking = contentsToCell(p);
    m_cellEnd = m_cellTracking;
    m_rubberBand = QRect(m_cellStart, m_cellEnd).normalized();
    update();
}


void Editor::mouseReleaseEvent_FillRectangle(QMouseEvent*)
{
    QUndoCommand *cmd = new FillRectangleCommand(m_document);

    for (int y = m_rubberBand.top() ; y <= m_rubberBand.bottom() ; y++) {
        for (int x = m_rubberBand.left() ; x <= m_rubberBand.right() ; x++) {
            new AddStitchCommand(m_document, QPoint(x, y), Stitch::Full, m_document->pattern()->palette().currentIndex(), cmd);
        }
    }

    m_rubberBand = QRect();     // this will clear the rubber band rectangle on the next repaint

    m_document->undoStack().push(cmd);
}


void Editor::mousePressEvent_Ellipse(QMouseEvent *e)
{
    m_cellStart = m_cellTracking = m_cellEnd = contentsToCell(e->pos());
    m_rubberBand = QRect();
}


void Editor::mouseMoveEvent_Ellipse(QMouseEvent *e)
{
    QPoint p = e->pos();

    dynamic_cast<QScrollArea *>(parentWidget()->parentWidget())->ensureVisible(p.x(), p.y());

    m_cellTracking = contentsToCell(p);
    m_cellEnd = m_cellTracking;
    m_rubberBand = QRect(m_cellStart, m_cellEnd).normalized();
    update();
}


void Editor::mouseReleaseEvent_Ellipse(QMouseEvent*)
{
    int bitmapWidth = m_document->pattern()->stitches().width();
    int bitmapHeight = m_document->pattern()->stitches().height();
    bool useFractionals = Configuration::toolShapes_UseFractionals();

    if (useFractionals) {
        bitmapWidth *= 2;
        bitmapHeight *= 2;
        m_cellStart *= 2;
        m_cellEnd *= 2;
    }

    if (m_cellStart != m_cellEnd) {
        QBitmap canvas(bitmapWidth, bitmapHeight);
        QPainter painter;

        canvas.fill(Qt::color0);
        painter.begin(&canvas);
        painter.setRenderHint(QPainter::Antialiasing, !Configuration::toolShapes_UseFractionals());
        painter.setPen(QPen(Qt::color1));
        painter.setBrush(Qt::NoBrush);
        painter.drawEllipse(m_rubberBand);
        painter.end();

        QUndoCommand *cmd = new DrawEllipseCommand(m_document);
        processBitmap(cmd, canvas);

        m_document->undoStack().push(cmd);
    }

    m_rubberBand = QRect();
}


void Editor::mousePressEvent_FillEllipse(QMouseEvent *e)
{
    m_cellStart = m_cellTracking = m_cellEnd = contentsToCell(e->pos());
    m_rubberBand = QRect();
}


void Editor::mouseMoveEvent_FillEllipse(QMouseEvent *e)
{
    QPoint p = e->pos();

    dynamic_cast<QScrollArea *>(parentWidget()->parentWidget())->ensureVisible(p.x(), p.y());

    m_cellTracking = contentsToCell(p);
    m_cellEnd = m_cellTracking;
    m_rubberBand = QRect(m_cellStart, m_cellEnd).normalized();
    update();
}


void Editor::mouseReleaseEvent_FillEllipse(QMouseEvent*)
{
    int bitmapWidth = m_document->pattern()->stitches().width();
    int bitmapHeight = m_document->pattern()->stitches().height();
    bool useFractionals = Configuration::toolShapes_UseFractionals();

    if (useFractionals) {
        bitmapWidth *= 2;
        bitmapHeight *= 2;
        m_cellStart *= 2;
        m_cellEnd *= 2;
    }

    if (m_cellStart != m_cellEnd) {
        QBitmap canvas(bitmapWidth, bitmapHeight);
        QPainter painter;

        canvas.fill(Qt::color0);
        painter.begin(&canvas);
        painter.setRenderHint(QPainter::Antialiasing, !useFractionals);
        painter.setPen(QPen(Qt::color1));
        painter.setBrush(Qt::color1);
        painter.drawEllipse(QRect(m_cellStart, m_cellEnd).normalized());
        painter.end();

        QUndoCommand *cmd = new FillEllipseCommand(m_document);
        processBitmap(cmd, canvas);

        m_document->undoStack().push(cmd);
    }

    m_rubberBand = QRect();
}


void Editor::mousePressEvent_FillPolygon(QMouseEvent *e)
{
    m_cellStart = m_cellTracking = m_cellEnd = contentsToCell(e->pos());
    m_polygon.append(m_cellStart);
    update();
}


void Editor::mouseMoveEvent_FillPolygon(QMouseEvent *e)
{
    m_cellTracking = contentsToCell(e->pos());

    if (m_cellTracking != m_cellStart) {
        m_polygon.append(m_cellTracking);
        m_cellStart = m_cellTracking;
    }

    update();
}


void Editor::mouseReleaseEvent_FillPolygon(QMouseEvent *e)
{
    int bitmapWidth = m_document->pattern()->stitches().width();
    int bitmapHeight = m_document->pattern()->stitches().height();
    bool useFractionals = Configuration::toolShapes_UseFractionals();

    if (useFractionals) {
        bitmapWidth *= 2;
        bitmapHeight *= 2;
        m_cellStart *= 2;
        m_cellEnd *= 2;
    }

    m_cellEnd = contentsToCell(e->pos());

    if ((m_cellEnd == m_polygon.point(0)) && (m_polygon.count() > 2)) {
        QBitmap canvas(bitmapWidth, bitmapHeight);
        QPainter painter;

        if (useFractionals) {
            for (int i = 0 ; i < m_polygon.size() ; ++i) {
                m_polygon[i] *= 2;
            }
        }

        canvas.fill(Qt::color0);
        painter.begin(&canvas);
        painter.setRenderHint(QPainter::Antialiasing, false);
        painter.setRenderHint(QPainter::Qt4CompatiblePainting, true);
        painter.setPen(QPen(Qt::color1));
        painter.setBrush(Qt::color1);
        painter.drawPolygon(m_polygon);
        painter.end();

        QUndoCommand *cmd = new FillPolygonCommand(m_document);
        processBitmap(cmd, canvas);

        m_document->undoStack().push(cmd);

        m_polygon.clear();
    }
}


void Editor::mousePressEvent_Text(QMouseEvent *e)
{
    mousePressEvent_Paste(e); // performs the required functions
}


void Editor::mouseMoveEvent_Text(QMouseEvent *e)
{
    mouseMoveEvent_Paste(e); // performs the required functions
}


void Editor::mouseReleaseEvent_Text(QMouseEvent *e)
{
    m_document->undoStack().push(new EditPasteCommand(m_document, m_pastePattern, contentsToCell(e->pos()) - m_pasteOffset, (e->modifiers() & Qt::ShiftModifier), i18n("Text")));
    m_pastePattern = nullptr;
    setCursor(Qt::ArrowCursor);
    selectTool(m_oldToolMode);
}


void Editor::mousePressEvent_Alphabet(QMouseEvent*)
{
    // nothing to do
}


void Editor::mouseMoveEvent_Alphabet(QMouseEvent*)
{
    // nothing to do
}


void Editor::mouseReleaseEvent_Alphabet(QMouseEvent *e)
{
    if (m_activeCommand == nullptr) {
        m_activeCommand = new AlphabetCommand(m_document);
        m_document->undoStack().push(m_activeCommand);
    } else if ((m_activeCommand->text() == i18n("Alphabet")) && static_cast<AlphabetCommand *>(m_activeCommand)->childCount()) {
        toolCleanupAlphabet();
        m_activeCommand = new AlphabetCommand(m_document);
        m_document->undoStack().push(m_activeCommand);
    }

    m_cellStart = m_cellTracking = m_cellEnd = contentsToCell(e->pos());
    m_cursorStack.push(m_cellEnd);
    update();
}


void Editor::mousePressEvent_Select(QMouseEvent *e)
{
    if (m_rubberBand.isValid()) {
        m_rubberBand = QRect();
        update();
    }

    m_cellStart = m_cellTracking = m_cellEnd = contentsToCell(e->pos());
    m_rubberBand = QRect(cellToRect(m_cellStart));
}


void Editor::mouseMoveEvent_Select(QMouseEvent *e)
{
    QPoint p = e->pos();

    dynamic_cast<QScrollArea *>(parentWidget()->parentWidget())->ensureVisible(p.x(), p.y());

    m_cellTracking = contentsToCell(p);
    m_cellEnd = m_cellTracking;
    m_rubberBand = QRect(m_cellStart, m_cellEnd).normalized();
    update();

    QToolTip::showText(QCursor::pos(), QString::fromLatin1("%1,%2 %3 x %4").arg(m_rubberBand.left()).arg(m_rubberBand.top()).arg(m_rubberBand.width()).arg(m_rubberBand.height()));
}


void Editor::mouseReleaseEvent_Select(QMouseEvent*)
{
    m_selectionArea = QRect(m_cellStart, m_cellEnd).normalized();
    emit(selectionMade(true));
}


void Editor::mousePressEvent_Backstitch(QMouseEvent *e)
{
    m_cellStart = m_cellTracking = m_cellEnd = contentsToSnap(e->pos());
    m_rubberBand = QRect();
}


void Editor::mouseMoveEvent_Backstitch(QMouseEvent *e)
{
    QPoint p = e->pos();

    dynamic_cast<QScrollArea *>(parentWidget()->parentWidget())->ensureVisible(p.x(), p.y());

    m_cellTracking = contentsToSnap(p);
    m_cellEnd = m_cellTracking;
    m_rubberBand = (snapToCells(m_cellStart).united(snapToCells(m_cellEnd))).normalized();
    update();
}


void Editor::mouseReleaseEvent_Backstitch(QMouseEvent*)
{
    m_rubberBand = QRect();

    if (m_cellStart != m_cellEnd) {
        m_document->undoStack().push(new AddBackstitchCommand(m_document, m_cellStart, m_cellEnd, m_document->pattern()->palette().currentIndex()));
    }
}


void Editor::mousePressEvent_ColorPicker(QMouseEvent*)
{
    // nothing to be done
}


void Editor::mouseMoveEvent_ColorPicker(QMouseEvent*)
{
    // nothing to be done
}


void Editor::mouseReleaseEvent_ColorPicker(QMouseEvent *e)
{
    int colorIndex = -1;
    StitchQueue *queue = m_document->pattern()->stitches().stitchQueueAt(contentsToCell(e->pos()));

    if (queue) {
        Stitch::Type type = stitchMap[0][m_zoneStart];
        QListIterator<Stitch *> q(*queue);

        while (q.hasNext()) {
            Stitch *stitch = q.next();

            if (stitch->type & type) {
                colorIndex = stitch->colorIndex;
                break;
            }
        }

        if (colorIndex != -1) {
            m_document->pattern()->palette().setCurrentIndex(colorIndex);
            m_document->palette()->update();

            if (m_colorHighlight) {
                drawContents();
            }
        }
    }
}


void Editor::mousePressEvent_Paste(QMouseEvent *e)
{
    QPoint cell = contentsToCell(e->pos());

    if (!m_pastePattern->stitches().extents().translated(m_cellStart).contains(cell)) {
        m_cellStart = m_cellTracking = m_cellEnd = cell;
    }

    m_pasteOffset = cell - m_cellStart;

    update();
}


void Editor::mouseMoveEvent_Paste(QMouseEvent *e)
{
    QPoint p = e->pos();

    dynamic_cast<QScrollArea *>(parentWidget()->parentWidget())->ensureVisible(p.x(), p.y());

    m_cellTracking = contentsToCell(p) - m_pasteOffset;

    if (m_cellTracking != m_cellEnd) {
        m_cellEnd = m_cellTracking;
        update();
    }
}


void Editor::mouseReleaseEvent_Paste(QMouseEvent *e)
{
    m_document->undoStack().push(new EditPasteCommand(m_document, m_pastePattern, contentsToCell(e->pos()) - m_pasteOffset, (e->modifiers() & Qt::ShiftModifier), i18n("Paste")));
    m_pasteData.clear();
    m_pastePattern = nullptr;
    setCursor(Qt::ArrowCursor);
    selectTool(m_oldToolMode);
}


void Editor::mousePressEvent_Mirror(QMouseEvent *e)
{
    mousePressEvent_Paste(e);
}


void Editor::mouseMoveEvent_Mirror(QMouseEvent *e)
{
    mouseMoveEvent_Paste(e);
}


void Editor::mouseReleaseEvent_Mirror(QMouseEvent *e)
{
    m_document->undoStack().push(new MirrorSelectionCommand(m_document, m_selectionArea, (m_maskColor) ? m_document->pattern()->palette().currentIndex() : -1, maskStitches(), m_maskBackstitch, m_maskKnot, m_orientation, m_makesCopies, m_pasteData, m_pastePattern, contentsToCell(e->pos()) - m_pasteOffset, (e->modifiers() & Qt::ShiftModifier)));
    m_pasteData.clear();
    m_pastePattern = nullptr;
    setCursor(Qt::ArrowCursor);
    selectTool(m_oldToolMode);
}


void Editor::mousePressEvent_Rotate(QMouseEvent *e)
{
    mousePressEvent_Paste(e);
}


void Editor::mouseMoveEvent_Rotate(QMouseEvent *e)
{
    mouseMoveEvent_Paste(e);
}


void Editor::mouseReleaseEvent_Rotate(QMouseEvent *e)
{
    m_document->undoStack().push(new RotateSelectionCommand(m_document, m_selectionArea, (m_maskColor) ? m_document->pattern()->palette().currentIndex() : -1, maskStitches(), m_maskBackstitch, m_maskKnot, m_rotation, m_makesCopies, m_pasteData, m_pastePattern, contentsToCell(e->pos()) - m_pasteOffset, (e->modifiers() & Qt::ShiftModifier)));
    m_pastePattern = nullptr;
    m_pasteData.clear();
    setCursor(Qt::ArrowCursor);
    selectTool(m_oldToolMode);
}


QPoint Editor::contentsToCell(const QPoint &p) const
{
    return QPoint(p.x() / m_cellWidth, p.y() / m_cellHeight);
}


int Editor::contentsToZone(const QPoint &p) const
{
    QPoint cell(p.x() * 2 / m_cellWidth, p.y() * 2 / m_cellHeight);
    int zone = (cell.y() % 2) * 2 + (cell.x() % 2);

    return zone;
}


QPoint Editor::contentsToSnap(const QPoint &p) const
{
    int w = m_document->pattern()->stitches().width() * 2;
    int h = m_document->pattern()->stitches().height() * 2;

    int x = (int)(round((double)(w * p.x()) / width()));
    int y = (int)(round((double)(h * p.y()) / height()));

    return QPoint(x, y);
}


QRect Editor::snapToCells(const QPoint &p) const
{
    int x = p.x();
    int y = p.y();

    int rectWidth  = (x % 2) ? 1 : 2;
    int rectHeight = (y % 2) ? 1 : 2;
    int rectLeft   = (x - rectWidth) / 2;
    int rectTop    = (y - rectHeight) / 2;

    return QRect(rectLeft, rectTop, rectWidth, rectHeight);
}


QRect Editor::cellToRect(const QPoint &cell) const
{
    return QRect(cell.x(), cell.y(), 1, 1);
}


QRect Editor::rectToContents(const QRect &rect) const
{
    return QRect(rect.left() * m_cellWidth, rect.top() * m_cellHeight, rect.width() * m_cellWidth, rect.height() * m_cellHeight);
}


void Editor::processBitmap(QUndoCommand *parent, const QBitmap &canvas)
{
    QImage image = canvas.toImage();
    int colorIndex = m_document->pattern()->palette().currentIndex();

    for (int y = 0 ; y < image.height() ; y++) {
        for (int x = 0 ; x < image.width() ; x++) {
            if (image.pixelIndex(x, y) == 1) {
                if (Configuration::toolShapes_UseFractionals()) {
                    int zone = (y % 2) * 2 + (x % 2);
                    new AddStitchCommand(m_document, QPoint(x / 2, y / 2), stitchMap[0][zone], colorIndex, parent);
                } else {
                    new AddStitchCommand(m_document, QPoint(x, y), Stitch::Full, colorIndex, parent);
                }
            }
        }
    }
}


QRect Editor::selectionArea()
{
    return m_selectionArea;
}


void Editor::resetSelectionArea()
{
    toolCleanupSelect();
}


QRect Editor::visibleCells()
{
    QRect cells;

    int left = -pos().x();
    int top = -pos().y();
    QSize viewportSize = dynamic_cast<QScrollArea *>(parent()->parent())->viewport()->size();
    int viewportWidth = viewportSize.width();
    int viewportHeight = viewportSize.height();
    int documentWidth = m_document->pattern()->stitches().width();
    int documentHeight = m_document->pattern()->stitches().height();

    QPoint topLeft = contentsToCell(QPoint(left, top));
    QPoint bottomRight = contentsToCell(QPoint(left + viewportWidth, top + viewportHeight));
    cells.setCoords(topLeft.x(), topLeft.y(), std::min(bottomRight.x(), documentWidth), std::min(bottomRight.y(), documentHeight));

    return cells;
}
