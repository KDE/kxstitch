/********************************************************************************
 *	Copyright (C) 2010 by Stephen Allewell					*
 *	stephen@mirramar.adsl24.co.uk						*
 *										*
 *	This program is free software; you can redistribute it and/or modify	*
 *	it under the terms of the GNU General Public License as published by	*
 *	the Free Software Foundation; either version 2 of the License, or	*
 *	(at your option) any later version.					*
 ********************************************************************************/


#include "Editor.h"

#include <QAction>
#include <QApplication>
#include <QBitmap>
#include <QClipboard>
#include <QContextMenuEvent>
#include <QMenu>
#include <QMouseEvent>
#include <QPainter>
#include <QRubberBand>
#include <QScrollArea>
#include <QStyleOptionRubberBand>
#include <QToolTip>
#include <QX11Info>

#include <KAction>
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
#include "Renderer.h"
#include "Scale.h"
#include "SchemeManager.h"
#include "TextToolDlg.h"
#include "XKeyLock.h"


const Editor::keyPressCallPointer Editor::keyPressCallPointers[] =
{
	0,					// Paint
	0,					// Draw
	0,					// Erase
	0,					// Rectangle
	0,					// Fill Rectangle
	0,					// Ellipse
	0,					// Fill Ellipse
	&Editor::keyPressPolygon,		// Fill Polygon
	0,					// Text
	&Editor::keyPressAlphabet,		// Alphabet
	0,					// Select
	0,					// Backstitch
	0,					// Color Picker
	&Editor::keyPressPaste,			// Paste
	&Editor::keyPressMirror,		// Mirror
	&Editor::keyPressRotate			// Rotate
};

const Editor::toolInitCallPointer Editor::toolInitCallPointers[] =
{
	0,					// Paint
	0,					// Draw
	0,					// Erase
	0,					// Rectangle
	0,					// Fill Rectangle
	0,					// Ellipse
	0,					// Fill Ellipse
	&Editor::toolInitPolygon,		// Fill Polygon
	&Editor::toolInitText,			// Text
	&Editor::toolInitAlphabet,		// Alphabet
	0,					// Select
	0,					// Backstitch
	0,					// Color Picker
	0,					// Paste
	0,					// Mirror
	0					// Rotate
};


const Editor::toolCleanupCallPointer Editor::toolCleanupCallPointers[] =
{
	0,					// Paint
	0,					// Draw
	0,					// Erase
	0,					// Rectangle
	0,					// Fill Rectangle
	0,					// Ellipse
	0,					// Fill Ellipse
	&Editor::toolCleanupPolygon,		// Fill Polygon
	0,					// Text
	&Editor::toolCleanupAlphabet,		// Alphabet
	&Editor::toolCleanupSelect,		// Select
	0,					// Backstitch
	0,					// Color Picker
	0,					// Paste
	&Editor::toolCleanupMirror,		// Mirror
	&Editor::toolCleanupRotate		// Rotate
};


const Editor::mouseEventCallPointer Editor::mousePressEventCallPointers[] =
{
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

const Editor::mouseEventCallPointer Editor::mouseMoveEventCallPointers[] =
{
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

const Editor::mouseEventCallPointer Editor::mouseReleaseEventCallPointers[] =
{
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

const Editor::renderToolSpecificGraphicsCallPointer Editor::renderToolSpecificGraphics[] =
{
	0,					// Paint
	&Editor::renderRubberBandLine,		// Draw
	0,					// Erase
	&Editor::renderRubberBandRectangle,	// Rectangle
	&Editor::renderRubberBandRectangle,	// Fill Rectangle
	&Editor::renderRubberBandEllipse,	// Ellipse
	&Editor::renderRubberBandEllipse,	// Fill Ellipse
	&Editor::renderFillPolygon,		// Fill Polygon
	0,					// Text
	&Editor::renderAlphabetCursor,		// Alphabet
	&Editor::renderRubberBandRectangle,	// Select
	&Editor::renderRubberBandLine,		// Backstitch
	0,					// Color Picker
	&Editor::renderPasteImage,		// Paste
	&Editor::renderPasteImage,		// Mirror - Paste performs the same functions
	&Editor::renderPasteImage		// Rotate - Paste performs the same functions
};


const Stitch::Type stitchMap[][4] =
{
	{
		Stitch::TLQtr,
		Stitch::TRQtr,
		Stitch::BLQtr,
		Stitch::BRQtr
	},
	{
		Stitch::TBHalf,
		Stitch::BTHalf,
		Stitch::BTHalf,
		Stitch::TBHalf
	},
	{
		Stitch::TL3Qtr,
		Stitch::TR3Qtr,
		Stitch::BL3Qtr,
		Stitch::BR3Qtr
	},
	{
		Stitch::Full,
		Stitch::Full,
		Stitch::Full,
		Stitch::Full
	},
	{
		Stitch::TLSmallHalf,
		Stitch::TRSmallHalf,
		Stitch::BLSmallHalf,
		Stitch::BRSmallHalf
	},
	{
		Stitch::TLSmallFull,
		Stitch::TRSmallFull,
		Stitch::BLSmallFull,
		Stitch::BRSmallFull
	}
};


Editor::Editor(QWidget *parent)
	:	QWidget(parent),
		m_libraryManagerDlg(0),
		m_activeCommand(0),
		m_horizontalScale(new Scale(Qt::Horizontal)),
		m_verticalScale(new Scale(Qt::Vertical)),
		m_renderer(new Renderer()),
		m_toolMode(ToolPaint),
		m_zoomFactor(1.0)
{
	setAcceptDrops(true);
}


Editor::~Editor()
{
//	delete m_libraryManagerDlg;
//	delete m_horizontalScale;
//	delete m_verticalScale;
//	delete m_renderer;
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
	m_cellHorizontalGrouping = m_document->property("cellHorizontalGrouping").toInt();
	m_cellVerticalGrouping = m_document->property("cellVerticalGrouping").toInt();

	m_horizontalClothCount = m_document->property("horizontalClothCount").toDouble();
	m_verticalClothCount = m_document->property("verticalClothCount").toDouble();

	m_horizontalScale->setCellGrouping(m_cellHorizontalGrouping);
	m_horizontalScale->setCellCount(m_document->pattern()->stitches().width());
	m_horizontalScale->setClothCount(m_horizontalClothCount);
	m_horizontalScale->setUnits(static_cast<Configuration::EnumEditor_FormatScalesAs::type>(m_document->property("formatScalesAs").toInt()));

	m_verticalScale->setCellGrouping(m_cellVerticalGrouping);
	m_verticalScale->setCellCount(m_document->pattern()->stitches().height());
	m_verticalScale->setClothCount(m_verticalClothCount);
	m_verticalScale->setUnits(static_cast<Configuration::EnumEditor_FormatScalesAs::type>(m_document->property("formatScalesAs").toInt()));

	m_renderer->setPatternRect(QRect(0, 0, m_document->pattern()->stitches().width(), m_document->pattern()->stitches().height()));

	zoom(m_zoomFactor);
}


void Editor::libraryManager()
{
	if (m_libraryManagerDlg == 0)
	{
		m_libraryManagerDlg = new LibraryManagerDlg(this);
		m_libraryManagerDlg->setCellSize(m_cellWidth, m_cellHeight);
	}
	m_libraryManagerDlg->show();
}


void Editor::previewClicked(const QPoint &cell)
{
	QRect contentsRect = parentWidget()->contentsRect();
	dynamic_cast<QScrollArea *>(parentWidget()->parentWidget())->ensureVisible(cell.x()*m_cellWidth, cell.y()*m_cellHeight, contentsRect.width()/2, contentsRect.height()/2);
}


void Editor::previewClicked(const QRect &cells)
{
	int documentWidth = m_document->pattern()->stitches().width();
	int documentHeight = m_document->pattern()->stitches().height();
	int left = cells.left();
	int top = cells.top();
	int right = std::min(cells.right(), documentWidth-1);
	int bottom = std::min(cells.bottom(), documentHeight-1);

	QRect visibleArea = parentWidget()->contentsRect();
	double visibleWidth = visibleArea.width();
	double visibleHeight = visibleArea.height();
	bool clothCountUnitsInches = (static_cast<Configuration::EnumEditor_ClothCountUnits::type>(m_document->property("clothCountUnits").toInt()) == Configuration::EnumEditor_ClothCountUnits::Inches);
	double widthScaleFactor = visibleWidth / (right-left) * ((clothCountUnitsInches)?m_horizontalClothCount:m_horizontalClothCount*2.54) / physicalDpiX();
	double heightScaleFactor = visibleHeight / (bottom-top) * ((clothCountUnitsInches)?m_verticalClothCount:m_verticalClothCount*2.54) / physicalDpiY();

	zoom(std::min(widthScaleFactor, heightScaleFactor));

	previewClicked(cells.center());
}


void Editor::zoom(double factor)
{
	m_zoomFactor = factor;

	double dpiX = physicalDpiX();
	double dpiY = physicalDpiY();

	bool clothCountUnitsInches = (static_cast<Configuration::EnumEditor_ClothCountUnits::type>(m_document->property("clothCountUnits").toInt()) == Configuration::EnumEditor_ClothCountUnits::Inches);

	m_cellWidth = dpiX * factor / ((clothCountUnitsInches)?m_horizontalClothCount:m_horizontalClothCount*2.54);
	m_cellHeight = dpiY * factor / ((clothCountUnitsInches)?m_verticalClothCount:m_verticalClothCount*2.54);

	m_horizontalScale->setCellSize(m_cellWidth);
	m_verticalScale->setCellSize(m_cellHeight);

	m_horizontalScale->setOffset(pos().x());
	m_verticalScale->setOffset(pos().y());

	m_renderer->setCellSize(m_cellWidth, m_cellHeight);
	if (m_libraryManagerDlg)
		m_libraryManagerDlg->setCellSize(m_cellWidth, m_cellHeight);

	this->resize(m_document->pattern()->stitches().width()*m_cellWidth, m_document->pattern()->stitches().height()*m_cellHeight);
	m_renderer->setPaintDeviceArea(QRect(0, 0, width(), height()));

	emit changedVisibleCells(visibleCells());
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
	bool clothCountUnitsInches = (static_cast<Configuration::EnumEditor_ClothCountUnits::type>(m_document->property("clothCountUnits").toInt()) == Configuration::EnumEditor_ClothCountUnits::Inches);
	double widthScaleFactor = visibleWidth / documentWidth * ((clothCountUnitsInches)?m_horizontalClothCount:m_horizontalClothCount*2.54) / physicalDpiX();
	double heightScaleFactor = visibleHeight / m_document->pattern()->stitches().height() * ((clothCountUnitsInches)?m_verticalClothCount:m_verticalClothCount*2.54) / physicalDpiY();

	zoom(std::min(widthScaleFactor, heightScaleFactor));
}


void Editor::fitToWidth()
{
	int documentWidth = m_document->pattern()->stitches().width();
	double visibleWidth = parentWidget()->contentsRect().width();
	bool clothCountUnitsInches = (static_cast<Configuration::EnumEditor_ClothCountUnits::type>(m_document->property("clothCountUnits").toInt()) == Configuration::EnumEditor_ClothCountUnits::Inches);
	double widthScaleFactor = visibleWidth / documentWidth * ((clothCountUnitsInches)?m_horizontalClothCount:m_horizontalClothCount*2.54) / physicalDpiX();

	zoom(widthScaleFactor);
}


void Editor::fitToHeight()
{
	int documentHeight = m_document->pattern()->stitches().height();
	double visibleHeight = parentWidget()->contentsRect().height();
	bool clothCountUnitsInches = (static_cast<Configuration::EnumEditor_ClothCountUnits::type>(m_document->property("clothCountUnits").toInt()) == Configuration::EnumEditor_ClothCountUnits::Inches);
	double heightScaleFactor = visibleHeight / documentHeight * ((clothCountUnitsInches)?m_verticalClothCount:m_verticalClothCount*2.54) / physicalDpiY();

	zoom(heightScaleFactor);
}


QList<Stitch::Type> Editor::maskStitches() const
{
	QList<Stitch::Type> maskStitches;
	if (m_maskStitch)
	{
		if (m_currentStitchType == StitchFull)
			maskStitches << Stitch::Full;
		else
		{
			for (int i = 0 ; i < 4 ; ++i)
				maskStitches << stitchMap[m_currentStitchType][i];
		}
	}
	else
	{
		maskStitches << Stitch::TLQtr << Stitch::TRQtr << Stitch::BLQtr << Stitch::BTHalf << Stitch::TL3Qtr << Stitch::BRQtr
			<< Stitch::TBHalf << Stitch::TR3Qtr << Stitch::BL3Qtr << Stitch::BR3Qtr << Stitch::Full << Stitch::TLSmallHalf
			<< Stitch::TRSmallHalf << Stitch::BLSmallHalf << Stitch::BRSmallHalf << Stitch::TLSmallFull << Stitch::TRSmallFull
			<< Stitch::BLSmallFull << Stitch::BRSmallFull;
	}

	return maskStitches;
}


void Editor::editCut()
{
	m_document->undoStack().push(new EditCutCommand(m_document, m_selectionArea, (m_maskColor)?m_document->pattern()->palette().currentIndex():-1, maskStitches(), m_maskBackstitch, m_maskKnot));
	m_selectionArea = QRect();
}


void Editor::editCopy()
{
	Pattern *pattern = m_document->pattern()->copy(m_selectionArea, (m_maskColor)?m_document->pattern()->palette().currentIndex():-1, maskStitches(), m_maskBackstitch, m_maskKnot);

	QByteArray data;
	QDataStream stream(&data, QIODevice::WriteOnly);
	stream << *pattern;
	
	QMimeData *mimeData = new QMimeData();
	mimeData->setData("application/kxstitch", data);
	
	QApplication::clipboard()->setMimeData(mimeData);

	m_selectionArea = QRect();
	update();
}


void Editor::editPaste()
{
	m_pasteData = QApplication::clipboard()->mimeData()->data("application/kxstitch");
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
	if (rect().contains(pos))
		m_cellStart = m_cellTracking = m_cellEnd = contentsToCell(pos);

	update();
	
	setFocus(Qt::OtherFocusReason);
}


void Editor::mirrorSelection()
{
	m_orientation = static_cast<Qt::Orientation>(qobject_cast<KAction *>(sender())->data().toInt());

	QDataStream stream(&m_pasteData, QIODevice::WriteOnly);
	stream << m_document->pattern()->stitches();
	if (m_makesCopies)
		m_pastePattern = m_document->pattern()->copy(m_selectionArea, (m_maskColor)?m_document->pattern()->palette().currentIndex():-1, maskStitches(), m_maskBackstitch, m_maskKnot);
	else
		m_pastePattern = m_document->pattern()->cut(m_selectionArea, (m_maskColor)?m_document->pattern()->palette().currentIndex():-1, maskStitches(), m_maskBackstitch, m_maskKnot);
	
	m_pastePattern->stitches().mirror(m_orientation);
	pastePattern(ToolMirror);
}


void Editor::rotateSelection()
{
	m_rotation = static_cast<StitchData::Rotation>(qobject_cast<KAction *>(sender())->data().toInt());

	QDataStream stream(&m_pasteData, QIODevice::WriteOnly);
	stream << m_document->pattern()->stitches();
	if (m_makesCopies)
		m_pastePattern = m_document->pattern()->copy(m_selectionArea, (m_maskColor)?m_document->pattern()->palette().currentIndex():-1, maskStitches(), m_maskBackstitch, m_maskKnot);
	else
		m_pastePattern = m_document->pattern()->cut(m_selectionArea, (m_maskColor)?m_document->pattern()->palette().currentIndex():-1, maskStitches(), m_maskBackstitch, m_maskKnot);
	
	m_pastePattern->stitches().rotate(m_rotation);
	pastePattern(ToolRotate);
}


void Editor::renderStitches(bool show)
{
	m_renderStitches = show;
	update();
}


void Editor::renderBackstitches(bool show)
{
	m_renderBackstitches = show;
	update();
}


void Editor::renderFrenchKnots(bool show)
{
	m_renderFrenchKnots = show;
	update();
}


void Editor::renderGrid(bool show)
{
	m_renderGrid = show;
	update();
}


void Editor::renderBackgroundImages(bool show)
{
	m_renderBackgroundImages = show;
	update();
}


void Editor::renderStitchesAs()
{
	m_renderStitchesAs = static_cast<Configuration::EnumRenderer_RenderStitchesAs::type>(qobject_cast<QAction *>(sender())->data().toInt());
	m_renderer->setRenderStitchesAs(m_renderStitchesAs);
	update();
}


void Editor::renderBackstitchesAs()
{
	m_renderBackstitchesAs = static_cast<Configuration::EnumRenderer_RenderBackstitchesAs::type>(qobject_cast<QAction *>(sender())->data().toInt());
	m_renderer->setRenderBackstitchesAs(m_renderBackstitchesAs);
	update();
}


void Editor::renderKnotsAs()
{
	m_renderKnotsAs = static_cast<Configuration::EnumRenderer_RenderKnotsAs::type>(qobject_cast<QAction *>(sender())->data().toInt());
	m_renderer->setRenderKnotsAs(m_renderKnotsAs);
	update();
}


void Editor::colorHilight(bool set)
{
	m_colorHilight = set;
	update();
}


void Editor::selectTool()
{
	selectTool(static_cast<Editor::ToolMode>(qobject_cast<QAction *>(sender())->data().toInt()));
}


void Editor::selectTool(ToolMode toolMode)
{
	if (toolCleanupCallPointers[m_toolMode])
		(this->*toolCleanupCallPointers[m_toolMode])();
	
	m_toolMode = toolMode;

	if (toolInitCallPointers[m_toolMode])
		(this->*toolInitCallPointers[m_toolMode])();
}


void Editor::selectStitch()
{
	m_currentStitchType = static_cast<Editor::SelectedStitchType>(qobject_cast<QAction *>(sender())->data().toInt());
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


bool Editor::event(QEvent *e)
{
	if ((e->type() == QEvent::ToolTip) && (m_toolMode == ToolColorPicker))
	{
		QHelpEvent *helpEvent = static_cast<QHelpEvent *>(e);
		int colorIndex = -1;
		QPoint cell = contentsToCell(helpEvent->pos());
		int zone = contentsToZone(helpEvent->pos());
		StitchQueue *queue = m_document->pattern()->stitches().stitchQueueAt(cell);
		if (queue)
		{
			Stitch::Type type = stitchMap[0][zone];
			QListIterator<Stitch *> q(*queue);
			while (q.hasNext())
			{
				Stitch *stitch = q.next();
				if (stitch->type & type)
				{
					colorIndex = stitch->colorIndex;
					break;
				}
			}
		}
		if (colorIndex != -1)
		{
			DocumentFloss *documentFloss = m_document->pattern()->palette().floss(colorIndex);
			Floss *floss = SchemeManager::scheme(m_document->pattern()->palette().schemeName())->find(documentFloss->flossName());
			QToolTip::showText(helpEvent->globalPos(), QString("%1 %2").arg(floss->name()).arg(floss->description()));
		}
		else
		{
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
	QMenu *context = static_cast<QMenu *>(mainWindow->guiFactory()->container("EditorPopup", mainWindow));
	context->popup(e->globalPos());
	e->accept();
}


void Editor::dragEnterEvent(QDragEnterEvent *e)
{
	if (e->mimeData()->hasFormat("application/kxstitch"))
		e->accept();
}


void Editor::dragMoveEvent(QDragMoveEvent *e)
{
	if (e->mimeData()->hasFormat("application/kxstitch"))
		e->accept();
}


void Editor::dragLeaveEvent(QDragLeaveEvent*)
{
	// dont need to do anything here
}


void Editor::dropEvent(QDropEvent *e)
{
	m_pasteData = e->mimeData()->data("application/kxstitch");
	m_pastePattern = new Pattern;
	QDataStream stream(&m_pasteData, QIODevice::ReadOnly);
	stream >> *m_pastePattern;
	m_document->undoStack().push(new EditPasteCommand(m_document, m_pastePattern, contentsToCell(e->pos()), e->keyboardModifiers() & Qt::ShiftModifier, i18n("Drag")));
	m_pastePattern = 0;
	m_pasteData.clear();
	e->accept();
}


void Editor::keyPressEvent(QKeyEvent *e)
{
	if (keyPressCallPointers[m_toolMode])
		(this->*keyPressCallPointers[m_toolMode])(e);
	else
		e->ignore();
}


void Editor::keyReleaseEvent(QKeyEvent*)
{
}


void Editor::keyPressPolygon(QKeyEvent *e)
{
	switch (e->key())
	{
		case Qt::Key_Backspace:
			if (!m_polygon.isEmpty())
			{
				QRect updateArea = polygonToContents(m_polygon);
				m_polygon.pop_back();
				update(updateArea.adjusted(-5,-5,5,5));
			}
			e->accept();
			break;
			
		case Qt::Key_Escape:
			if (!m_polygon.isEmpty())
			{
				QRect updateArea = polygonToContents(m_polygon);
				m_polygon.clear();
				update(updateArea.adjusted(-5,-5,5,5));
			}
			e->accept();
			break;
			
		default:
			e->ignore();
			break;
	}
}


void Editor::keyPressAlphabet(QKeyEvent *e)
{
	XKeyLock keylock(QX11Info::display());
	Qt::KeyboardModifiers modifiers = e->modifiers();
	if (keylock.getCapsLock() & Configuration::alphabet_UseCapsLock())
		modifiers = static_cast<Qt::KeyboardModifiers>(modifiers ^ Qt::ShiftModifier);
	int width = m_document->pattern()->stitches().width();
	int height = m_document->pattern()->stitches().height();
	LibraryPattern *libraryPattern = 0;
	
	if (m_libraryManagerDlg->currentLibrary())
	{

		switch (e->key())
		{
			case Qt::Key_Backspace:
				if (m_cursorStack.count() > 1)
				{
					m_cellEnd = m_cursorStack.pop();
					m_cursorCommands.remove(m_cursorStack.count());
					update(cellToRect(m_cellEnd));
					update(cellToRect(m_cursorStack.top()));
					int commandsToUndo = m_cursorCommands[m_cursorStack.count()-1];
					while (commandsToUndo--)
					{
						QUndoCommand *cmd = static_cast<AlphabetCommand *>(m_activeCommand)->pop();
						m_cursorCommands[m_cursorStack.count()-1]--;
						delete cmd;
					}
				}
				e->accept();
				break;
				
			case Qt::Key_Return:
			case Qt::Key_Enter:
				m_cellTracking = QPoint(m_cursorStack.at(0).x(), m_cursorStack.top().y()+m_libraryManagerDlg->currentLibrary()->maxHeight()+Configuration::alphabet_LineSpacing());
				if (m_cellTracking.y() >= height)
				{
					static_cast<AlphabetCommand *>(m_activeCommand)->push(new ExtendPatternCommand(m_document, 0, 0, 0, m_cellTracking.y()-height+Configuration::alphabet_ExtendPatternHeight()));
					m_cursorCommands[m_cursorStack.count()-1]++;
				}
				m_cellEnd = m_cursorStack.top();
				m_cursorStack.push(m_cellTracking);
				update(cellToRect(m_cellEnd));
				update(cellToRect(m_cellTracking));
				e->accept();
				break;
				
			default:
				libraryPattern = m_libraryManagerDlg->currentLibrary()->findCharacter(e->key(), modifiers);
				if (libraryPattern)
				{
					if ((m_cursorStack.top()+QPoint(libraryPattern->pattern()->stitches().width(), 0)).x() >= width)
					{
						if (m_cursorCommands[m_cursorStack.count()-2])
						{
							static_cast<AlphabetCommand *>(m_activeCommand)->push(new ExtendPatternCommand(m_document, 0, 0, m_cursorStack.top().x()+libraryPattern->pattern()->stitches().width()-width+Configuration::alphabet_ExtendPatternWidth(), 0));
							m_cursorCommands[m_cursorStack.count()-1]++;
						}
						else
						{
							m_cellTracking = QPoint(m_cursorStack.at(0).x(), m_cursorStack.top().y()+m_libraryManagerDlg->currentLibrary()->maxHeight()+Configuration::alphabet_LineSpacing());
							if (m_cellTracking.y() >= height)
							{
								static_cast<AlphabetCommand *>(m_activeCommand)->push(new ExtendPatternCommand(m_document, 0, 0, 0, m_cellTracking.y()-height+Configuration::alphabet_ExtendPatternHeight()));
								m_cursorCommands[m_cursorStack.count()-1]++;
							}
							m_cellEnd = m_cursorStack.top();
							m_cursorStack.push(m_cellTracking);
							update(cellToRect(m_cellEnd));
							update(cellToRect(m_cellTracking));
						}
					}
					QPoint insertionPoint = m_cursorStack.top() - QPoint(0, libraryPattern->pattern()->stitches().height()-1-libraryPattern->baseline());
					static_cast<AlphabetCommand *>(m_activeCommand)->push(new EditPasteCommand(m_document, libraryPattern->pattern(), insertionPoint, true, i18n("Add Character")));
					m_cursorCommands[m_cursorStack.count()-1]++;
					m_cursorStack.push(m_cursorStack.top() + QPoint(libraryPattern->pattern()->stitches().width()+1, 0));
				}
				else
				{
					if (e->key() == Qt::Key_Space)
					{
						m_cellTracking = m_cursorStack.top() + QPoint(Configuration::alphabet_SpaceWidth(), 0);
						if (m_cellTracking.x() >= width)
						{
							if (Configuration::alphabet_WordWrap())
							{
								m_cellTracking = QPoint(m_cursorStack.at(0).x(), m_cellTracking.y() + m_libraryManagerDlg->currentLibrary()->maxHeight()+Configuration::alphabet_LineSpacing());
								if (m_cellTracking.y() >= height)
								{
									static_cast<AlphabetCommand *>(m_activeCommand)->push(new ExtendPatternCommand(m_document, 0, 0, 0, m_cellTracking.y()-height+Configuration::alphabet_ExtendPatternHeight()));
									m_cursorCommands[m_cursorStack.count()-1]++;
								}
							}
							else
							{
								static_cast<AlphabetCommand *>(m_activeCommand)->push(new ExtendPatternCommand(m_document, 0, 0, m_cellTracking.x()-width+Configuration::alphabet_ExtendPatternWidth(), 0));
								m_cursorCommands[m_cursorStack.count()-1]++;
							}
						}
						m_cursorStack.push(m_cellTracking);
						update(cellToRect(m_cursorStack.at(m_cursorStack.size()-2)));
						update(cellToRect(m_cellTracking));
					}
				}
				e->accept();
				break;
		}
	}
	QPoint contentPoint = cellToRect(m_cursorStack.top()).center();
	dynamic_cast<QScrollArea *>(parentWidget()->parentWidget())->ensureVisible(contentPoint.x(), contentPoint.y());
}


void Editor::keyPressPaste(QKeyEvent *e)
{
	switch (e->key())
	{
		case Qt::Key_Return:
		case Qt::Key_Enter:
			m_document->undoStack().push(new EditPasteCommand(m_document, m_pastePattern, m_cellEnd, (e->modifiers() & Qt::ShiftModifier), i18n("Paste")));
			m_pastePattern = 0;
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
	switch (e->key())
	{
		case Qt::Key_Return:
		case Qt::Key_Enter:
			m_document->undoStack().push(new MirrorSelectionCommand(m_document, m_selectionArea, (m_maskColor)?m_document->pattern()->palette().currentIndex():-1, maskStitches(), m_maskBackstitch, m_maskKnot, m_orientation, m_makesCopies, m_pasteData, m_pastePattern, contentsToCell(m_cellEnd), (e->modifiers() & Qt::ShiftModifier)));
			m_pastePattern = 0;
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
	switch (e->key())
	{
		case Qt::Key_Return:
		case Qt::Key_Enter:
			m_document->undoStack().push(new RotateSelectionCommand(m_document, m_selectionArea, (m_maskColor)?m_document->pattern()->palette().currentIndex():-1, maskStitches(), m_maskBackstitch, m_maskKnot, m_rotation, m_makesCopies, m_pasteData, m_pastePattern, contentsToCell(m_cellEnd), (e->modifiers() & Qt::ShiftModifier)));
			m_pastePattern = 0;
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
	switch (e->key())
	{
		case Qt::Key_Escape:
			e->accept();
			selectTool(m_oldToolMode);
			break;
			
		case Qt::Key_Up:
			m_cellTracking = QPoint(m_cellEnd.x(), std::max(m_cellEnd.y()-1, 0));
			if (m_cellTracking != m_cellEnd)
			{
				m_cellEnd = m_cellTracking;
				update();
			}
			e->accept();
			break;
			
		case Qt::Key_Down:
			m_cellTracking = QPoint(m_cellEnd.x(), std::min(m_cellEnd.y()+1, m_document->pattern()->stitches().height()-1));
			if (m_cellTracking != m_cellEnd)
			{
				m_cellEnd = m_cellTracking;
				update();
			}
			e->accept();
			break;
			
		case Qt::Key_Left:
			m_cellTracking = QPoint(std::max(m_cellEnd.x()-1, 0), m_cellEnd.y());
			if (m_cellTracking != m_cellEnd)
			{
				m_cellEnd = m_cellTracking;
				update();
			}
			e->accept();
			break;
			
		case Qt::Key_Right:
			m_cellTracking = QPoint(std::min(m_cellEnd.x()+1, m_document->pattern()->stitches().width()-1), m_cellEnd.y());
			if (m_cellTracking != m_cellEnd)
			{
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
	if (textToolDlg->exec())
	{
		m_pastePattern = new Pattern;
		QImage image = textToolDlg->image();

		m_pastePattern->palette().setSchemeName(m_document->pattern()->palette().schemeName());
		int currentIndex = m_document->pattern()->palette().currentIndex();
		m_pastePattern->palette().add(currentIndex, new DocumentFloss(m_document->pattern()->palette().currentFloss()));
		m_pastePattern->stitches().resize(image.width(), image.height());
		
		int stitchesAdded = 0;
		for (int row = 0 ; row < image.height() ; ++row)
		{
			for (int col = 0 ; col < image.width() ; ++col)
			{
				if (image.pixelIndex(col, row) == 1)
				{
					QPoint cell(col, row);
					m_pastePattern->stitches().addStitch(cell, Stitch::Full, currentIndex);
					++stitchesAdded;
				}
			}
		}
		
		if (stitchesAdded)
			pastePattern(ToolText);
		else
		{
			delete m_pastePattern;
			m_pastePattern = 0;
		}
	}
}


void Editor::toolInitAlphabet()
{
	libraryManager();
}


void Editor::toolCleanupPolygon()
{
	m_polygon.clear();
	m_document->editor()->update();
}


void Editor::toolCleanupAlphabet()
{
	m_activeCommand = 0;
	m_cursorStack.clear();
	m_cursorCommands.clear();
}


void Editor::toolCleanupSelect()
{
	if (m_rubberBand.isValid())
	{
		QRect r = m_rubberBand;
		m_rubberBand = QRect();
		update(r.adjusted(-5,-5,5,5));
	}
	emit(selectionMade(false));
}


void Editor::toolCleanupMirror()
{
	delete m_pastePattern;
	m_pastePattern = 0;
	
	if (!m_pasteData.isEmpty())
	{
		m_document->pattern()->stitches().clear();
		QDataStream stream(&m_pasteData, QIODevice::ReadOnly);
		stream >> m_document->pattern()->stitches();
		m_pasteData.clear();
	}
	
	m_document->editor()->update();
	m_document->preview()->update();
}


void Editor::toolCleanupRotate()
{
	delete m_pastePattern;
	m_pastePattern = 0;
	
	if (!m_pasteData.isEmpty())
	{
		m_document->pattern()->stitches().clear();
		QDataStream stream(&m_pasteData, QIODevice::ReadOnly);
		stream >> m_document->pattern()->stitches();
		m_pasteData.clear();
	}
	
	m_document->editor()->update();
	m_document->preview()->update();
}


void Editor::mousePressEvent(QMouseEvent *e)
{
	if ((e->buttons() & Qt::LeftButton) && ((m_document->pattern()->palette().currentIndex() != -1) || (m_toolMode == Editor::ToolSelect) || (m_toolMode == Editor::ToolAlphabet)))
		(this->*mousePressEventCallPointers[m_toolMode])(e);
}


void Editor::mouseMoveEvent(QMouseEvent *e)
{
	if ((e->buttons() & Qt::LeftButton) && ((m_document->pattern()->palette().currentIndex() != -1) || (m_toolMode == Editor::ToolSelect) || (m_toolMode == Editor::ToolAlphabet)))
		(this->*mouseMoveEventCallPointers[m_toolMode])(e);
}


void Editor::mouseReleaseEvent(QMouseEvent *e)
{
	if ((m_document->pattern()->palette().currentIndex() != -1) || (m_toolMode == Editor::ToolSelect) || (m_toolMode == Editor::ToolAlphabet))
		(this->*mouseReleaseEventCallPointers[m_toolMode])(e);
}


void Editor::paintEvent(QPaintEvent *e)
{
	static QPoint oldpos = pos();

	QPainter *painter = new QPainter();
	painter->begin(this);

	painter->fillRect(e->rect(), m_document->property("fabricColor").value<QColor>());

	if (m_renderBackgroundImages)
		renderBackgroundImages(painter, e->rect());
	
	m_renderer->render(painter,
			   m_document->pattern(),
			   e->rect(),
			   m_renderGrid,
			   m_renderStitches,
			   m_renderBackstitches,
			   m_renderFrenchKnots,
			   (m_colorHilight)?m_document->pattern()->palette().currentIndex():-1);
	
	if (renderToolSpecificGraphics[m_toolMode])
		(this->*renderToolSpecificGraphics[m_toolMode])(painter, e->rect());

	painter->end();
	delete painter;

	if (pos() != oldpos)
	{
		m_horizontalScale->setOffset(pos().x());
		m_verticalScale->setOffset(pos().y());
		oldpos = pos();
	}

	emit changedVisibleCells(visibleCells());
}


void Editor::renderBackgroundImages(QPainter *painter, QRect updateRectangle)
{
	QListIterator<BackgroundImage *> backgroundImages = m_document->backgroundImages().backgroundImages();
	while (backgroundImages.hasNext())
	{
		BackgroundImage *background = backgroundImages.next();
		if (background->isVisible())
		{
			QRect r = background->location();
			r.moveTo(r.left()*m_cellWidth, r.top()*m_cellHeight);
			r.setWidth(r.width()*m_cellWidth);
			r.setHeight(r.height()*m_cellHeight);
			if (r.intersects(updateRectangle))
				painter->drawImage(r, background->image());
		}
	}
}


void Editor::renderRubberBandLine(QPainter *painter, QRect)
{
	painter->save();
	if (m_rubberBand.isValid())
	{
		QPen pen(m_document->pattern()->palette().currentFloss()->flossColor());
		pen.setWidth(4);
		if (m_toolMode == ToolBackstitch)
			painter->drawLine(snapToContents(m_cellStart), snapToContents(m_cellEnd));
		else
			painter->drawLine(cellToRect(m_cellStart).center(), cellToRect(m_cellEnd).center());
	}
	painter->restore();
}


void Editor::renderRubberBandRectangle(QPainter *painter, QRect)
{
	painter->save();
	if (m_rubberBand.isValid())
	{
		QStyleOptionRubberBand opt;
		opt.initFrom(this);
		opt.shape = QRubberBand::Rectangle;
		opt.opaque = false;
		opt.rect = m_rubberBand;
		style()->drawControl(QStyle::CE_RubberBand, &opt, painter);
	}
	painter->restore();
}


void Editor::renderRubberBandEllipse(QPainter *painter, QRect)
{
	painter->save();
	if (m_rubberBand.isValid())
	{
		QPainterPath path;
		path.addEllipse(m_rubberBand);
		painter->setBrush(QColor(Qt::cyan));
		painter->setPen(Qt::black);
		painter->drawPath(path);
	}
	painter->restore();
}


void Editor::renderFillPolygon(QPainter *painter, QRect rect)
{
	QPolygon polyline;
	painter->save();
	QVector<QPoint>::const_iterator i;
	for (i = m_polygon.constBegin() ; i != m_polygon.constEnd() ; ++i)
	{
		QRect cellRect = cellToRect(*i);
		painter->drawEllipse(cellRect);
		polyline.append(cellRect.center());
	}
	painter->drawPolyline(polyline);
	painter->restore();
}


void Editor::renderAlphabetCursor(QPainter *painter, QRect rect)
{
	if (m_cursorStack.isEmpty())
		return;
	
	painter->save();
	painter->setPen(Qt::red);
	painter->fillRect(cellToRect(m_cursorStack.top()), Qt::red);
	painter->restore();
}


void Editor::renderPasteImage(QPainter *painter, QRect rect)
{
	painter->save();
	m_renderer->render(painter,
			   m_pastePattern,	// the pattern data to render
			   rect,		// update rectangle
			   false,		// dont render the grid
			   true,		// render stitches
			   true,		// render backstitches
			   true,		// render knots
			   -1,			// all colors
			   m_cellEnd);		// offset to the position of the mouse
	painter->restore();
}


void Editor::mousePressEvent_Paint(QMouseEvent *e)
{
	QPoint p = e->pos();
	QRect rect;

	if (m_currentStitchType == StitchFrenchKnot)
	{
		if (QRect(0, 0, m_document->pattern()->stitches().width()*m_cellWidth, m_document->pattern()->stitches().height()*m_cellHeight).contains(p))
		{
			m_cellStart = m_cellTracking = m_cellEnd = contentsToSnap(p);
			m_activeCommand = new PaintKnotsCommand(m_document);
			new AddKnotCommand(m_document, m_cellStart, m_document->pattern()->palette().currentIndex(), m_activeCommand);
			m_document->undoStack().push(m_activeCommand);
			rect = QRect(snapToContents(m_cellStart)-(QPoint(m_cellWidth, m_cellHeight)/2), QSize(m_cellWidth, m_cellHeight)).adjusted(-5,-5,5,5);
		}
	}
	else
	{
		m_cellStart = m_cellTracking = m_cellEnd = contentsToCell(p);
		m_zoneStart = m_zoneTracking = m_zoneEnd = contentsToZone(p);
		Stitch::Type stitchType = stitchMap[m_currentStitchType][m_zoneStart];
		m_activeCommand = new PaintStitchesCommand(m_document);
		new AddStitchCommand(m_document, m_cellStart, stitchType, m_document->pattern()->palette().currentIndex(), m_activeCommand);
		m_document->undoStack().push(m_activeCommand);
		rect = cellToRect(m_cellStart).adjusted(-5,-5,5,5);
	}
	update(rect);
}


void Editor::mouseMoveEvent_Paint(QMouseEvent *e)
{
	QPoint p = e->pos();
	QRect rect;

	if (m_currentStitchType == StitchFrenchKnot)
	{
		if (QRect(0, 0, m_document->pattern()->stitches().width()*m_cellWidth, m_document->pattern()->stitches().height()*m_cellHeight).contains(p))
		{
			m_cellTracking = contentsToSnap(p);
			if (m_cellTracking != m_cellStart)
			{
				m_cellStart = m_cellTracking;
				QUndoCommand *cmd = new AddKnotCommand(m_document, m_cellStart, m_document->pattern()->palette().currentIndex(), m_activeCommand);
				cmd->redo();
				rect = QRect(snapToContents(m_cellStart)-(QPoint(m_cellWidth, m_cellHeight)/2), QSize(m_cellWidth, m_cellHeight)).adjusted(-5,-5,5,5);
			}
		}
	}
	else
	{
		m_cellTracking = contentsToCell(p);
		m_zoneTracking = contentsToZone(p);
		if ((m_cellTracking != m_cellStart) || (m_zoneTracking != m_zoneStart))
		{
			m_cellStart = m_cellTracking;
			m_zoneStart = m_zoneTracking;
			Stitch::Type stitchType = stitchMap[m_currentStitchType][m_zoneStart];
			QUndoCommand *cmd = new AddStitchCommand(m_document, m_cellStart, stitchType, m_document->pattern()->palette().currentIndex(), m_activeCommand);
			cmd->redo();
			rect = cellToRect(m_cellStart).adjusted(-5,-5,5,5);
		}
	}
	update(rect);
}


void Editor::mouseReleaseEvent_Paint(QMouseEvent*)
{
	m_activeCommand = 0;
	m_preview->update();
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
	QRect updateArea = cellToRect(m_cellStart).united(cellToRect(m_cellEnd)).normalized();	
	m_cellEnd = m_cellTracking;
	m_rubberBand = cellToRect(m_cellStart).united(cellToRect(m_cellEnd)).normalized();
	update(updateArea.united(m_rubberBand).adjusted(-5,-5,5,5));
}


void Editor::mouseReleaseEvent_Draw(QMouseEvent*)
{
	QBitmap canvas(m_document->pattern()->stitches().width(), m_document->pattern()->stitches().height());
	QPainter painter;

	m_rubberBand = QRect();
	if (m_cellStart != m_cellEnd)
	{
		canvas.fill(Qt::color0);
		painter.begin(&canvas);
		painter.setPen(QPen(Qt::color1));
		painter.drawLine(m_cellStart, m_cellEnd);
		painter.end();

		QUndoCommand *cmd = new DrawLineCommand(m_document);
		processBitmap(cmd, canvas);
		
		m_document->undoStack().push(cmd);
	}
}


void Editor::mousePressEvent_Erase(QMouseEvent *e)
{
	QPoint p = e->pos();
	QRect rect;
	QUndoCommand *cmd;

	if (e->modifiers() & Qt::ControlModifier)
	{
		// Erase a backstitch
		m_cellStart = m_cellTracking = m_cellEnd = contentsToSnap(p);
	}
	else
	{
		m_activeCommand = new EraseStitchesCommand(m_document);
		m_document->undoStack().push(m_activeCommand);
		if (e->modifiers() & Qt::ShiftModifier)
		{
			// Delete french knots
			m_cellStart = m_cellTracking = m_cellEnd = contentsToSnap(p);
			cmd = new DeleteKnotCommand(m_document, m_cellStart, (m_maskColor)?m_document->pattern()->palette().currentIndex():-1, m_activeCommand);
			cmd->redo();
			rect = QRect(snapToContents(m_cellStart)-QPoint(m_cellWidth/2, m_cellHeight/2), QSize(m_cellWidth, m_cellHeight)).adjusted(-5,-5,5,5);
		}
		else
		{
			m_cellStart = m_cellTracking = m_cellEnd = contentsToCell(p);
			m_zoneStart = m_zoneTracking = m_zoneEnd = contentsToZone(p);
			cmd = new DeleteStitchCommand(m_document, m_cellStart, m_maskStitch?stitchMap[m_currentStitchType][m_zoneStart]:Stitch::Delete, m_maskColor?m_document->pattern()->palette().currentIndex():-1, m_activeCommand);
			cmd->redo();
			rect = cellToRect(m_cellStart).adjusted(-5,-5,5,5);
		}
		update(rect);
	}
}


void Editor::mouseMoveEvent_Erase(QMouseEvent *e)
{
	QPoint p = e->pos();
	QRect rect;
	QUndoCommand *cmd;

	if (e->modifiers() & Qt::ControlModifier)
	{
		// Erasing a backstitch
		// Don't need to do anything here
	}
	else
	{
		if (e->modifiers() & Qt::ShiftModifier)
		{
			// Delete french knots
			m_cellTracking = contentsToSnap(p);
			if (m_cellTracking != m_cellStart)
			{
				m_cellStart = m_cellTracking;
				cmd = new DeleteKnotCommand(m_document, m_cellStart, (m_maskColor)?m_document->pattern()->palette().currentIndex():-1, m_activeCommand);
				cmd->redo();
				rect = QRect(snapToContents(m_cellStart)-QPoint(m_cellWidth/2, m_cellHeight/2), QSize(m_cellWidth, m_cellHeight)).adjusted(-5,-5,5,5);
			}
		}
		else
		{
			m_cellTracking = contentsToCell(p);
			m_zoneTracking = contentsToZone(p);
			if ((m_cellTracking != m_cellStart) || (m_zoneTracking != m_zoneStart))
			{
				m_cellStart = m_cellTracking;
				m_zoneStart = m_zoneTracking;
				cmd = new DeleteStitchCommand(m_document, m_cellStart, m_maskStitch?stitchMap[m_currentStitchType][m_zoneStart]:Stitch::Delete, m_maskColor?m_document->pattern()->palette().currentIndex():-1, m_activeCommand);
				cmd->redo();
				rect = cellToRect(m_cellStart).adjusted(-5,-5,5,5);
			}
		}
		update(rect);
	}
}


void Editor::mouseReleaseEvent_Erase(QMouseEvent *e)
{
	if (e->modifiers() & Qt::ControlModifier)
	{
		// Erase a backstitch
		m_cellEnd = contentsToSnap(e->pos());			
		QListIterator<Backstitch *> backstitches = m_document->pattern()->stitches().backstitchIterator();
		while (backstitches.hasNext())
		{
			Backstitch *backstitch = backstitches.next();
			if (backstitch->contains(m_cellStart) && backstitch->contains(m_cellEnd))
			{
				m_document->undoStack().push(new DeleteBackstitchCommand(m_document, backstitch->start, backstitch->end, m_maskColor?m_document->pattern()->palette().currentIndex():-1));
				update(QRect(snapToContents(backstitch->start), snapToContents(backstitch->end)).normalized().adjusted(-5,-5,5,5));
				break;
			}
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
	QRect updateArea = cellToRect(m_cellStart).united(cellToRect(m_cellEnd)).normalized();
	m_cellEnd = m_cellTracking;
	m_rubberBand = cellToRect(m_cellStart).united(cellToRect(m_cellEnd)).normalized();
	update(updateArea.united(m_rubberBand).adjusted(-5,-5,5,5));
}


void Editor::mouseReleaseEvent_Rectangle(QMouseEvent*)
{
	m_selectionArea = QRect(m_cellStart, m_cellEnd).normalized();
	
	int x = m_selectionArea.left();
	int y = m_selectionArea.top();
	QPoint cell(x, y);

	QUndoCommand *cmd = new DrawRectangleCommand(m_document);
	while (++x <= m_selectionArea.right())
	{
		new AddStitchCommand(m_document, cell, Stitch::Full, m_document->pattern()->palette().currentIndex(), cmd);
		cell.setX(x);
	}
	while (++y <= m_selectionArea.bottom())
	{
		new AddStitchCommand(m_document, cell, Stitch::Full, m_document->pattern()->palette().currentIndex(), cmd);
		cell.setY(y);
	}
	while (--x >= m_selectionArea.left())
	{
		new AddStitchCommand(m_document, cell, Stitch::Full, m_document->pattern()->palette().currentIndex(), cmd);
		cell.setX(x);
	}
	while (--y >= m_selectionArea.top())
	{
		new AddStitchCommand(m_document, cell, Stitch::Full, m_document->pattern()->palette().currentIndex(), cmd);
		cell.setY(y);
	}

	m_selectionArea = QRect();	// this will clear the selection area rectangle on the next repaint
	m_rubberBand = QRect();		// this will clear the rubber band rectangle on the next repaint

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
	QRect updateArea = cellToRect(m_cellStart).united(cellToRect(m_cellEnd)).normalized();
	m_cellEnd = m_cellTracking;
	m_rubberBand = cellToRect(m_cellStart).united(cellToRect(m_cellEnd)).normalized();
	update(updateArea.united(m_rubberBand).adjusted(-5,-5,5,5));
}


void Editor::mouseReleaseEvent_FillRectangle(QMouseEvent *e)
{
	m_selectionArea = QRect(m_cellStart, m_cellEnd).normalized();

	int x = m_selectionArea.left();
	int y = m_selectionArea.top();
	QPoint cell(x, y);

	QUndoCommand *cmd = new FillRectangleCommand(m_document);
	for (int y = m_selectionArea.top() ; y <= m_selectionArea.bottom() ; y++)
	{
		for (int x = m_selectionArea.left() ; x <= m_selectionArea.right() ; x++)
		{
			QPoint cell(x, y);
			new AddStitchCommand(m_document, cell, Stitch::Full, m_document->pattern()->palette().currentIndex(), cmd);
		}
	}

	m_selectionArea = QRect();	// this will clear the selection area rectangle on the next repaint
	m_rubberBand = QRect();		// this will clear the rubber band rectangle on the next repaint

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
	QRect updateArea = cellToRect(m_cellStart).united(cellToRect(m_cellEnd)).normalized();
	m_cellEnd = m_cellTracking;
	m_rubberBand = cellToRect(m_cellStart).united(cellToRect(m_cellEnd)).normalized();
	update(updateArea.united(m_rubberBand).adjusted(-5,-5,5,5));
}


void Editor::mouseReleaseEvent_Ellipse(QMouseEvent*)
{
	QBitmap canvas(m_document->pattern()->stitches().width(), m_document->pattern()->stitches().height());
	QPainter painter;
	
	canvas.fill(Qt::color0);
	painter.begin(&canvas);
	painter.setRenderHint(QPainter::Antialiasing, true);
	painter.setPen(QPen(Qt::color1));
	painter.setBrush(Qt::NoBrush);
	painter.drawEllipse(QRect(m_cellStart, m_cellEnd).normalized());
	painter.end();

	QUndoCommand *cmd = new DrawEllipseCommand(m_document);
	processBitmap(cmd, canvas);

	m_rubberBand = QRect();
	m_selectionArea = QRect();

	m_document->undoStack().push(cmd);
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
	QRect updateArea = cellToRect(m_cellStart).united(cellToRect(m_cellEnd)).normalized();
	m_cellEnd = m_cellTracking;
	m_rubberBand = cellToRect(m_cellStart).united(cellToRect(m_cellEnd)).normalized();
	update(updateArea.united(m_rubberBand).adjusted(-5,-5,5,5));
}


void Editor::mouseReleaseEvent_FillEllipse(QMouseEvent*)
{
	QBitmap canvas(m_document->pattern()->stitches().width(), m_document->pattern()->stitches().height());
	QPainter painter;
	
	canvas.fill(Qt::color0);
	painter.begin(&canvas);
	painter.setRenderHint(QPainter::Antialiasing, true);
	painter.setPen(QPen(Qt::color1));
	painter.setBrush(Qt::color1);
	painter.drawEllipse(QRect(m_cellStart, m_cellEnd).normalized());
	painter.end();

	QUndoCommand *cmd = new FillEllipseCommand(m_document);
	processBitmap(cmd, canvas);

	m_rubberBand = QRect();
	m_selectionArea = QRect();

	m_document->undoStack().push(cmd);
}


void Editor::mousePressEvent_FillPolygon(QMouseEvent *e)
{
	m_cellStart = m_cellTracking = m_cellEnd = contentsToCell(e->pos());
	m_polygon.append(m_cellStart);
	update(polygonToContents(m_polygon).adjusted(-5,-5,5,5));
}


void Editor::mouseMoveEvent_FillPolygon(QMouseEvent *e)
{
	m_cellTracking = contentsToCell(e->pos());
	if (m_cellTracking != m_cellStart)
	{
		m_polygon.append(m_cellTracking);
		m_cellStart = m_cellTracking;
	}
	update(polygonToContents(m_polygon).adjusted(-5,-5,5,5));
}


void Editor::mouseReleaseEvent_FillPolygon(QMouseEvent *e)
{
	m_cellEnd = contentsToCell(e->pos());
	if ((m_cellEnd == m_polygon.point(0)) && (m_polygon.count() > 2))
	{
		QBitmap canvas(m_document->pattern()->stitches().width(), m_document->pattern()->stitches().height());
		QPainter painter;
		
		canvas.fill(Qt::color0);
		painter.begin(&canvas);
		painter.setPen(QPen(Qt::color1));
		painter.setBrush(Qt::color1);
		painter.drawPolygon(m_polygon);
		painter.end();

		QUndoCommand *cmd = new FillPolygonCommand(m_document);
		processBitmap(cmd, canvas);

		m_document->undoStack().push(cmd);
		
		m_polygon.clear();
	}
	update(polygonToContents(m_polygon).adjusted(-5,-5,5,5));
	setFocus(Qt::OtherFocusReason);
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
	m_document->undoStack().push(new EditPasteCommand(m_document, m_pastePattern, contentsToCell(e->pos()), (e->modifiers() & Qt::ShiftModifier), i18n("Text")));
	m_pastePattern = 0;
	selectTool(m_oldToolMode);
}


void Editor::mousePressEvent_Alphabet(QMouseEvent *e)
{
	// nothing to do
}


void Editor::mouseMoveEvent_Alphabet(QMouseEvent *e)
{
	// nothing to do
}


void Editor::mouseReleaseEvent_Alphabet(QMouseEvent *e)
{
	if (m_activeCommand == 0)
	{
		m_activeCommand = new AlphabetCommand(m_document);
		m_document->undoStack().push(m_activeCommand);
	}
	else if ((m_activeCommand->text() == i18n("Alphabet")) && static_cast<AlphabetCommand *>(m_activeCommand)->childCount())
	{
		toolCleanupAlphabet();
		m_activeCommand = new AlphabetCommand(m_document);
		m_document->undoStack().push(m_activeCommand);
	}
	
	m_cellStart = m_cellTracking = m_cellEnd = contentsToCell(e->pos());
	m_cursorStack.push(m_cellEnd);
	update(cellToRect(m_cellEnd));
	setFocus(Qt::OtherFocusReason);
}


void Editor::mousePressEvent_Select(QMouseEvent *e)
{
	if (m_rubberBand.isValid())
	{
		QRect r = m_rubberBand;
		m_rubberBand = QRect();
		update(r.adjusted(-5,-5,5,5));
	}

	m_cellStart = m_cellTracking = m_cellEnd = contentsToCell(e->pos());
	m_rubberBand = QRect();
}


void Editor::mouseMoveEvent_Select(QMouseEvent *e)
{
	QPoint p = e->pos();
	
	dynamic_cast<QScrollArea *>(parentWidget()->parentWidget())->ensureVisible(p.x(), p.y());
	
	m_cellTracking = contentsToCell(p);
	QRect updateArea = cellToRect(m_cellStart).united(cellToRect(m_cellEnd)).normalized();
	m_cellEnd = m_cellTracking;
	m_rubberBand = cellToRect(m_cellStart).united(cellToRect(m_cellEnd)).normalized();
	update(updateArea.united(m_rubberBand).adjusted(-5,-5,5,5));
}


void Editor::mouseReleaseEvent_Select(QMouseEvent*)
{
	m_selectionArea = QRect(m_cellStart, m_cellEnd).normalized();
	m_rubberBand = QRect();
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
	QRect updateArea = QRect(snapToContents(m_cellStart), snapToContents(m_cellEnd)).normalized();
	m_cellEnd = m_cellTracking;
	m_rubberBand = QRect(snapToContents(m_cellStart), snapToContents(m_cellEnd)).normalized();
	update(updateArea.united(m_rubberBand));
}


void Editor::mouseReleaseEvent_Backstitch(QMouseEvent *e)
{
	m_rubberBand = QRect();
	if (m_cellStart != m_cellEnd)
		m_document->undoStack().push(new AddBackstitchCommand(m_document, m_cellStart, m_cellEnd, m_document->pattern()->palette().currentIndex()));
}


void Editor::mousePressEvent_ColorPicker(QMouseEvent *e)
{
	// nothing to be done
}


void Editor::mouseMoveEvent_ColorPicker(QMouseEvent *e)
{
	// nothing to be done
}


void Editor::mouseReleaseEvent_ColorPicker(QMouseEvent *e)
{
	int colorIndex = -1;
	StitchQueue *queue = m_document->pattern()->stitches().stitchQueueAt(contentsToCell(e->pos()));
	if (queue)
	{
		Stitch::Type type = stitchMap[0][m_zoneStart];
		QListIterator<Stitch *> q(*queue);
		while (q.hasNext())
		{
			Stitch *stitch = q.next();
			if (stitch->type & type)
			{
				colorIndex = stitch->colorIndex;
				break;
			}
		}
		if (colorIndex != -1)
		{
			m_document->pattern()->palette().setCurrentIndex(colorIndex);
			m_document->palette()->update();
		}
	}

}


void Editor::mousePressEvent_Paste(QMouseEvent *e)
{
	m_cellStart = m_cellTracking = m_cellEnd = contentsToCell(e->pos());
	update();
}


void Editor::mouseMoveEvent_Paste(QMouseEvent *e)
{
	QPoint p = e->pos();
	
	dynamic_cast<QScrollArea *>(parentWidget()->parentWidget())->ensureVisible(p.x(), p.y());
	
	m_cellTracking = contentsToCell(p);
	if (m_cellTracking != m_cellEnd)
	{
		m_cellEnd = m_cellTracking;
		update();
	}
}


void Editor::mouseReleaseEvent_Paste(QMouseEvent *e)
{
	m_document->undoStack().push(new EditPasteCommand(m_document, m_pastePattern, contentsToCell(e->pos()), (e->modifiers() & Qt::ShiftModifier), i18n("Paste")));
	m_pasteData.clear();
	m_pastePattern = 0;
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
	m_document->undoStack().push(new MirrorSelectionCommand(m_document, m_selectionArea, (m_maskColor)?m_document->pattern()->palette().currentIndex():-1, maskStitches(), m_maskBackstitch, m_maskKnot, m_orientation, m_makesCopies, m_pasteData, m_pastePattern, contentsToCell(e->pos()), (e->modifiers() & Qt::ShiftModifier)));
	m_pasteData.clear();
	m_pastePattern = 0;
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
	m_document->undoStack().push(new RotateSelectionCommand(m_document, m_selectionArea, (m_maskColor)?m_document->pattern()->palette().currentIndex():-1, maskStitches(), m_maskBackstitch, m_maskKnot, m_rotation, m_makesCopies, m_pasteData, m_pastePattern, contentsToCell(e->pos()), (e->modifiers() & Qt::ShiftModifier)));
	m_pastePattern = 0;
	m_pasteData.clear();
	selectTool(m_oldToolMode);
}


QPoint Editor::contentsToCell(const QPoint &p) const
{
	return QPoint(p.x()/m_cellWidth, p.y()/m_cellHeight);
}


int Editor::contentsToZone(const QPoint &p) const
{
	QPoint cell(p.x()*2/m_cellWidth, p.y()*2/m_cellHeight);
	int zone = (cell.y()%2)*2 + (cell.x()%2);
	return zone;
}


QPoint Editor::contentsToSnap(const QPoint &p) const
{
	int w = m_document->pattern()->stitches().width()*2;
	int h = m_document->pattern()->stitches().height()*2;

	int x = (int)(round((double)(w*p.x())/width()));
	int y = (int)(round((double)(h*p.y())/height()));

	return QPoint(x, y);
}


QPoint Editor::snapToContents(const QPoint &p) const
{
	int x = m_cellWidth*p.x()/2;
	int y = m_cellHeight*p.y()/2;

	return QPoint(x, y);
}


QRect Editor::cellToRect(QPoint cell)
{
	int x = cell.x()*m_cellWidth;
	int y = cell.y()*m_cellHeight;
	return QRect(x, y, m_cellWidth, m_cellHeight);
}


QRect Editor::polygonToContents(const QPolygon &polygon)
{
	QRect updateArea = polygon.boundingRect();
	return QRect(updateArea.left()*m_cellWidth, updateArea.top()*m_cellHeight, updateArea.width()*m_cellWidth, updateArea.height()*m_cellHeight);
}


void Editor::processBitmap(QUndoCommand *parent, QBitmap &canvas)
{
	QImage image;
	image = canvas.toImage();
			
	for (int y = 0 ; y < image.height() ; y++)
	{
		for (int x = 0 ; x < image.width() ; x++)
		{
			if (image.pixelIndex(x, y) == 1)
			{
				QPoint cell(x, y);
				new AddStitchCommand(m_document, cell, Stitch::Full, m_document->pattern()->palette().currentIndex(), parent);
			}
		}
	}
}


QRect Editor::selectionArea()
{
	return m_selectionArea;
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
	QPoint bottomRight = contentsToCell(QPoint(left+viewportWidth, top+viewportHeight));
	cells.setCoords(topLeft.x(), topLeft.y(), std::min(bottomRight.x(), documentWidth), std::min(bottomRight.y(), documentHeight));

	return cells;
}
