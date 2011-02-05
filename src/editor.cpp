/****************************************************************************
 *	Copyright (C) 2010 by Stephen Allewell									*
 *	stephen@mirramar.adsl24.co.uk											*
 *																			*
 *	This program is free software; you can redistribute it and/or modify	*
 *	it under the terms of the GNU General Public License as published by	*
 *	the Free Software Foundation; either version 2 of the License, or		*
 *	(at your option) any later version.										*
 ****************************************************************************/


#include <QAction>
#include <QActionGroup>
#include <QContextMenuEvent>
#include <QEvent>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QPoint>
#include <QMenu>
#include <QRubberBand>
#include <QScrollArea>
#include <QStyle>
#include <QStyleOptionRubberBand>

#include <KAction>
#include <KActionCollection>
#include <KApplication>
#include <KDebug>
#include <KXMLGUIFactory>
#include <KMessageBox>

#include <math.h>

#include "backgroundimage.h"
#include "commands.h"
#include "configuration.h"
#include "document.h"
#include "editor.h"
#include "floss.h"
#include "mainwindow.h"
#include "scale.h"
#include "stitch.h"


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


/**
	Constructor.
	Initialises some essential variables and flags from the configuration object.
	Creates horizontal and vertical scales.
	Selects the default tool, stitch format and backstitch format.
	*/
Editor::Editor() : QWidget()
{
	m_cellWidth = Configuration::editor_CellWidth();
	m_cellHeight = Configuration::editor_CellHeight();
	m_horizontalScale = new Scale(Qt::Horizontal);
	m_verticalScale = new Scale(Qt::Vertical);

	loadSettings();

	selectPaintTool();
	showStitchesAsRegularStitches();
	showBackstitchesAsColorLines();
	showKnotsAsColorBlocks();
	m_maskStitch = false;
	m_maskColor = false;
	m_maskBackstitch = false;
	m_maskKnot = false;
	m_copyMirrorRotate = false;
}


/**
	Destructor
	*/
Editor::~Editor()
{
}


/**
	Set the document associated with this editor.
	The editor is then configured to display the document using the settings stored
	within the documents properties.
	*/
void Editor::setDocument(Document *document)
{
	m_document = document;
	// read the document properties that will be used to display it
	m_showBackgroundImages = m_document->property("paintBackgroundImages").toBool();
	m_showGrid = m_document->property("paintGrid").toBool();
	m_showStitches = m_document->property("paintStitches").toBool();
	m_showBackstitches = m_document->property("paintBackstitches").toBool();
	m_showFrenchKnots = m_document->property("paintFrenchKnots").toBool();

	resizeEditor();
}


/**
	Resize the document.
	Resize the editor widget and update the scales
	@param width the new width
	@param height the new height
	*/
void Editor::resizeEditor()
{
	m_cellWidth = m_document->property("cellWidth").toUInt();
	m_cellHeight = m_document->property("cellHeight").toUInt();
	m_cellHorizontalGrouping = m_document->property("cellHorizontalGrouping").toUInt();
	m_cellVerticalGrouping = m_document->property("cellVerticalGrouping").toUInt();

	m_horizontalScale->setCellSize(m_cellWidth);
	m_horizontalScale->setCellGrouping(m_cellHorizontalGrouping);
	m_horizontalScale->setCellCount(m_document->width());
	m_horizontalScale->setClothCount(m_document->property("horizontalClothCount").toDouble());
	m_horizontalScale->setUnits(static_cast<Configuration::EnumEditor_FormatScalesAs::type>(m_document->property("formatScalesAs").toInt()));

	m_verticalScale->setCellSize(m_cellHeight);
	m_verticalScale->setCellGrouping(m_cellVerticalGrouping);
	m_verticalScale->setCellCount(m_document->height());
	m_verticalScale->setClothCount(m_document->property("verticalClothCount").toDouble());
	m_verticalScale->setUnits(static_cast<Configuration::EnumEditor_FormatScalesAs::type>(m_document->property("formatScalesAs").toInt()));

	m_horizontalScale->setOffset(pos().x());
	m_verticalScale->setOffset(pos().y());

	this->resize(m_document->width()*m_cellWidth + 1, m_document->height()*m_cellHeight + 1);
}


/**
	Get a pointer for the editors horizontal scale.
	@return pointer to a Scale.
	*/
Scale *Editor::horizontalScale() const
{
	return m_horizontalScale;
}


/**
	Get a pointer for the editors vertical scale.
	@return pointer to a Scale.
	*/
Scale *Editor::verticalScale() const
{
	return m_verticalScale;
}


/**
	Get the selection area.
	@return a QRect representing the selected area in cell coordinates.
	*/
QRect Editor::selectionArea()
{
	return m_selectionArea;
}


/**
	Mirror the selection vertically.
	*/
void Editor::mirrorVertical()
{
}


/**
	Mirror the selection horizontally.
	*/
void Editor::mirrorHorizontal()
{
}


/**
	Rotate the selection 90 degrees anti clockwise.
	*/
void Editor::rotate90()
{
}


/**
	Rotate the selection 180 degrees.
	*/
void Editor::rotate180()
{
}


/**
	Rotate the selection 270 degrees anti clockwise.
	*/
void Editor::rotate270()
{
}


/**
	Scale the display to represent a true size of the grid.
	*/
void Editor::actualSize()
{
}


/**
	Scale the display so that the whole canvas fits within the visible area.
	*/
void Editor::fitToPage()
{
	int documentWidth = m_document->width();
	int documentHeight = m_document->height();
	int documentCellWidth = m_document->property("cellWidth").toInt();
	int documentCellHeight = m_document->property("cellHeight").toInt();
	QRect visibleArea = parentWidget()->contentsRect();
	int visibleWidth = visibleArea.width();
	int visibleHeight = visibleArea.height();

	m_cellWidth = visibleWidth / documentWidth;    // divide the visible width by the number of horizontal cells
	m_cellHeight = visibleHeight / documentHeight; // divide the visible height by the number of vertical cells

	if (m_cellWidth < 2 || m_cellHeight < 2)
	{
		KMessageBox::information(this, "Pattern is to large to display full size.");
		m_cellWidth = documentCellWidth;
		m_cellHeight = documentCellHeight;
	}
	else
	{
		int ratioWidth  = m_cellHeight * documentCellWidth / documentCellHeight;
		int ratioHeight = m_cellWidth * documentCellHeight / documentCellWidth;

		if (ratioHeight > ratioWidth)
			m_cellWidth = ratioWidth;
		else
			m_cellHeight = ratioHeight;

		m_horizontalScale->setCellSize(m_cellWidth);
		m_verticalScale->setCellSize(m_cellHeight);

		this->resize(documentWidth*m_cellWidth + 1, documentHeight*m_cellHeight + 1);
	}
}


/**
	Scale the display so that the canvas width fits within the visible area.
	*/
void Editor::fitToWidth()
{
	int documentWidth = m_document->width();
	int documentHeight = m_document->height();
	int documentCellWidth = m_document->property("cellWidth").toInt();
	int documentCellHeight = m_document->property("cellHeight").toInt();

	m_cellWidth = parentWidget()->contentsRect().width() / documentWidth;
	m_cellHeight = m_cellWidth * m_document->property("cellHeight").toInt() / m_document->property("cellWidth").toInt();

	if (m_cellHeight < 2 || m_cellWidth < 2)
	{
		KMessageBox::information(this, "Pattern is to large to display full Width.");
		m_cellWidth = documentCellWidth;
		m_cellHeight = documentCellHeight;
	}
	else
	{
		m_horizontalScale->setCellSize(m_cellWidth);
		m_verticalScale->setCellSize(m_cellHeight);

		this->resize(documentWidth*m_cellWidth + 1, documentHeight*m_cellHeight + 1);
	}
}


/**
	Scale the display so that the canvas height fits within the visible area.
	*/
void Editor::fitToHeight()
{
	int documentWidth = m_document->width();
	int documentHeight = m_document->height();
	int documentCellWidth = m_document->property("cellWidth").toInt();
	int documentCellHeight = m_document->property("cellHeight").toInt();

	m_cellHeight = parentWidget()->contentsRect().height() / documentHeight;
	m_cellWidth = m_cellHeight * documentCellWidth / documentCellHeight;

	if (m_cellHeight < 2 || m_cellWidth < 2)
	{
		KMessageBox::information(this, "Pattern is to large to display full height.");
		m_cellWidth = documentCellWidth;
		m_cellHeight = documentCellHeight;
	}
	else
	{
		m_horizontalScale->setCellSize(m_cellWidth);
		m_verticalScale->setCellSize(m_cellHeight);

		this->resize(documentWidth*m_cellWidth + 1, documentHeight*m_cellHeight + 1);
	}
}


/**
	Load any additional settings.
	*/
void Editor::loadSettings()
{
}


/**
	Select the paint tool.
	*/
void Editor::selectPaintTool()
{
	m_mousePressCallPointer = &Editor::mousePressEvent_Paint;
	m_mouseMoveCallPointer = &Editor::mouseMoveEvent_Paint;
	m_mouseReleaseCallPointer = &Editor::mouseReleaseEvent_Paint;
	m_paintToolSpecificGraphics = 0;
	m_toolMode = ToolPaint;
}


/**
	Select the draw tool.
	*/
void Editor::selectDrawTool()
{
	m_mousePressCallPointer = &Editor::mousePressEvent_Draw;
	m_mouseMoveCallPointer = &Editor::mouseMoveEvent_Draw;
	m_mouseReleaseCallPointer = &Editor::mouseReleaseEvent_Draw;
	m_paintToolSpecificGraphics = &Editor::paintRubberBandLine;
	m_toolMode = ToolDraw;
}


/**
	Select the erase tool.
	*/
void Editor::selectEraseTool()
{
	m_mousePressCallPointer = &Editor::mousePressEvent_Erase;
	m_mouseMoveCallPointer = &Editor::mouseMoveEvent_Erase;
	m_mouseReleaseCallPointer = &Editor::mouseReleaseEvent_Erase;
	m_paintToolSpecificGraphics = 0;
	m_toolMode = ToolErase;
}


/**
	Select the rectangle tool.
	*/
void Editor::selectRectangleTool()
{
	m_mousePressCallPointer = &Editor::mousePressEvent_Rectangle;
	m_mouseMoveCallPointer = &Editor::mouseMoveEvent_Rectangle;
	m_mouseReleaseCallPointer = &Editor::mouseReleaseEvent_Rectangle;
	m_paintToolSpecificGraphics = &Editor::paintRubberBandRectangle;
	m_toolMode = ToolRectangle;
}


/**
	Select the fill rectangle tool.
	*/
void Editor::selectFillRectangleTool()
{
	m_mousePressCallPointer = &Editor::mousePressEvent_FillRectangle;
	m_mouseMoveCallPointer = &Editor::mouseMoveEvent_FillRectangle;
	m_mouseReleaseCallPointer = &Editor::mouseReleaseEvent_FillRectangle;
	m_paintToolSpecificGraphics = &Editor::paintRubberBandRectangle;
	m_toolMode = ToolFillRectangle;
}


/**
	Select the ellipse tool.
	*/
void Editor::selectEllipseTool()
{
	m_mousePressCallPointer = &Editor::mousePressEvent_Ellipse;
	m_mouseMoveCallPointer = &Editor::mouseMoveEvent_Ellipse;
	m_mouseReleaseCallPointer = &Editor::mouseReleaseEvent_Ellipse;
	m_paintToolSpecificGraphics = 0;
	m_toolMode = ToolEllipse;
}


/**
	Select the fill ellipse tool.
	*/
void Editor::selectFillEllipseTool()
{
	m_mousePressCallPointer = &Editor::mousePressEvent_FillEllipse;
	m_mouseMoveCallPointer = &Editor::mouseMoveEvent_FillEllipse;
	m_mouseReleaseCallPointer = &Editor::mouseReleaseEvent_FillEllipse;
	m_paintToolSpecificGraphics = &Editor::paintRubberBandEllipse;
	m_toolMode = ToolFillEllipse;
}


/**
	Select the fill polyline tool.
	*/
void Editor::selectFillPolylineTool()
{
	m_mousePressCallPointer = &Editor::mousePressEvent_FillPolyline;
	m_mouseMoveCallPointer = &Editor::mouseMoveEvent_FillPolyline;
	m_mouseReleaseCallPointer = &Editor::mouseReleaseEvent_FillPolyline;
	m_paintToolSpecificGraphics = 0;
	m_toolMode = ToolFillPolyline;
}


/**
	Select the text tool.
	*/
void Editor::selectTextTool()
{
	m_mousePressCallPointer = &Editor::mousePressEvent_Text;
	m_mouseMoveCallPointer = &Editor::mouseMoveEvent_Text;
	m_mouseReleaseCallPointer = &Editor::mouseReleaseEvent_Text;
	m_paintToolSpecificGraphics = 0;
	m_toolMode = ToolText;
}


/**
	Select the selection tool.
	*/
void Editor::selectSelectTool()
{
	m_mousePressCallPointer = &Editor::mousePressEvent_Select;
	m_mouseMoveCallPointer = &Editor::mouseMoveEvent_Select;
	m_mouseReleaseCallPointer = &Editor::mouseReleaseEvent_Select;
	m_paintToolSpecificGraphics = &Editor::paintRubberBandRectangle;
	m_toolMode = ToolSelect;
}


/**
	Select the backstitch tool.
	*/
void Editor::selectBackstitchTool()
{
	m_mousePressCallPointer = &Editor::mousePressEvent_Backstitch;
	m_mouseMoveCallPointer = &Editor::mouseMoveEvent_Backstitch;
	m_mouseReleaseCallPointer = &Editor::mouseReleaseEvent_Backstitch;
	m_paintToolSpecificGraphics = &Editor::paintRubberBandLine;
	m_toolMode = ToolBackstitch;
}


/**
	Set the current stitch type to quarter stitch.
	*/
void Editor::selectStitchQuarter()
{
	m_currentStitchType = Editor::stitchQuarter;
}


/**
	Set the current stitch type to half stitch.
	*/
void Editor::selectStitchHalf()
{
	m_currentStitchType = Editor::stitchHalf;
}


/**
	Set the current stitch type to 3 quarter stitch.
	*/
void Editor::selectStitch3Quarter()
{
	m_currentStitchType = Editor::stitch3Quarter;
}


/**
	Set the current stitch type to full stitch.
	*/
void Editor::selectStitchFull()
{
	m_currentStitchType = Editor::stitchFull;
}


/**
	Set the current stitch type to small half stitch.
	*/
void Editor::selectStitchSmallHalf()
{
	m_currentStitchType = Editor::stitchSmallHalf;
}


/**
	Set the current stitch type to small full stitch.
	*/
void Editor::selectStitchSmallFull()
{
	m_currentStitchType = Editor::stitchSmallFull;
}


/**
	Set the current stitch type to french knots.
	*/
void Editor::selectStitchFrenchKnot()
{
	m_currentStitchType = Editor::stitchFrenchKnot;
}


/**
	Toggle the show stitches flag.
	*/
void Editor::setShowStitches()
{
	m_showStitches = qobject_cast<KAction *>(sender())->isChecked();
	update();
}


/**
	Toggle the show backstitches flag
	*/
void Editor::setShowBackstitches()
{
	m_showBackstitches = qobject_cast<KAction *>(sender())->isChecked();
	update();
}


/**
	Toggle the show french knots flag
	*/
void Editor::setShowFrenchKnots()
{
	m_showFrenchKnots = qobject_cast<KAction *>(sender())->isChecked();
	update();
}


/**
	Toggle the show grid flag
	*/
void Editor::setShowGrid()
{
	m_showGrid = qobject_cast<KAction *>(sender())->isChecked();
	update();
}


/**
	Toggle the copy mirror / rotates flag
	*/
void Editor::setCopyMirrorRotate()
{
	m_copyMirrorRotate = qobject_cast<KAction *>(sender())->isChecked();
}


/**
	Toggle the stitch mask
	*/
void Editor::setMaskStitch()
{
	m_maskStitch = qobject_cast<KAction *>(sender())->isChecked();
}


/**
	Toggle the color mask
	*/
void Editor::setMaskColor()
{
	m_maskColor = qobject_cast<KAction *>(sender())->isChecked();
}


/**
	Toggle the backstitch mask
	*/
void Editor::setMaskBackstitch()
{
	m_maskBackstitch = qobject_cast<KAction *>(sender())->isChecked();
}


/**
	Toggle the knot mask
	*/
void Editor::setMaskKnot()
{
	m_maskKnot = qobject_cast<KAction *>(sender())->isChecked();
}


/**
	Set the stitch representation to lined stitches
	*/
void Editor::showStitchesAsRegularStitches()
{
	m_paintStitchCallPointer = &Editor::paintStitchesAsRegularStitches;
}


/**
	Set the stitch representation to black and white symbols
	*/
void Editor::showStitchesAsBlackWhiteSymbols()
{
	m_paintStitchCallPointer = &Editor::paintStitchesAsBlackWhiteSymbols;
}


/**
	Set the stitch representation to color symbols
	*/
void Editor::showStitchesAsColorSymbols()
{
	m_paintStitchCallPointer = &Editor::paintStitchesAsColorSymbols;
}


/**
	Set the stitch representation to color blocks
	*/
void Editor::showStitchesAsColorBlocks()
{
	m_paintStitchCallPointer = &Editor::paintStitchesAsColorBlocks;
}


/**
	Set the stitch representation to color blocks with symbols
	*/
void Editor::showStitchesAsColorBlocksSymbols()
{
	m_paintStitchCallPointer = &Editor::paintStitchesAsColorBlocksSymbols;
}


/**
	Set the stitch representation to color highlight.  This shows stitches of the current
	color in color and all other stitches in light gray.
	*/
void Editor::showStitchesAsColorHilight()
{
	m_paintStitchCallPointer = &Editor::paintStitchesAsColorHilight;
}


/**
	Set the backstitch representation to color lines
	*/
void Editor::showBackstitchesAsColorLines()
{
	m_paintBackstitchCallPointer = &Editor::paintBackstitchesAsColorLines;
}


/**
	Set the backstitch representation to black and white symbols
	*/
void Editor::showBackstitchesAsBlackWhiteSymbols()
{
	m_paintBackstitchCallPointer = &Editor::paintBackstitchesAsBlackWhiteSymbols;
}


/**
	Set the backstitch representation to color hilight
	*/
void Editor::showBackstitchesAsColorHilight()
{
	m_paintBackstitchCallPointer = &Editor::paintBackstitchesAsColorHilight;
}


/**
	Set the knot representation to color blocks
	*/
void Editor::showKnotsAsColorBlocks()
{
	m_paintKnotCallPointer = &Editor::paintKnotsAsColorBlocks;
}


/**
	Set the knot representation to color symbols
	*/
void Editor::showKnotsAsColorSymbols()
{
	m_paintKnotCallPointer = &Editor::paintKnotsAsColorSymbols;
}


/**
	Set the knot representation to black & white symbols
	*/
void Editor::showKnotsAsBlackWhiteSymbols()
{
	m_paintKnotCallPointer = &Editor::paintKnotsAsBlackWhiteSymbols;
}


/**
	Set the knot representation to color hilight
	*/
void Editor::showKnotsAsColorHilight()
{
	m_paintKnotCallPointer = &Editor::paintKnotsAsColorHilight;
}


/**
	Display a context menu
	@param e pointer to a QContextMenuEvent.
	*/
void Editor::contextMenuEvent(QContextMenuEvent *e)
{
	MainWindow *mainWindow = qobject_cast<MainWindow *>(topLevelWidget());
	QMenu *context = static_cast<QMenu *>(mainWindow->guiFactory()->container("editorPopup", mainWindow));
	context->popup(e->globalPos());
	e->accept();
}


/**
	Called when something is dragged onto the editor window.
	Various items can be accepted such as KXStitch specific patterns and images.
	A visual representation of the object will be displayed.
	*/
void Editor::dragEnterEvent(QDragEnterEvent*)
{
}


/**
	Called when something is dragged out of the editor window.
	If a visual representation of the object was displayed, it will now be removed.
	*/
void Editor::dragLeaveEvent(QDragLeaveEvent*)
{
}


/**
	Called when the dragged item is moved within the editor window.
	*/
void Editor::dragMoveEvent(QDragMoveEvent*)
{
}


/**
	Called when an item is dropped on the editor window.
	Check that the item is valid and create a stitched representation of it to add to the pattern.
	*/
void Editor::dropEvent(QDropEvent*)
{
}


/**
	Called when a key is pressed.
	*/
void Editor::keyPressEvent(QKeyEvent*)
{
}


/**
	Called when a key is released.
	*/
void Editor::keyReleaseEvent(QKeyEvent*)
{
}


/**
	Called when a mouse button is pressed.
	@param e pointer to a QMouseEvent.
	*/
void Editor::mousePressEvent(QMouseEvent *e)
{
	if ((e->buttons() & Qt::LeftButton) && m_document->currentFlossIndex() != -1)
		(this->*m_mousePressCallPointer)(e);
}


/**
	Called when the mouse is moved and the window is set to receive
	mouse tracking events.
	@param e pointer to a QMouseEvent.
	*/
void Editor::mouseMoveEvent(QMouseEvent *e)
{
	if ((e->buttons() & Qt::LeftButton) && m_document->currentFlossIndex() != -1)
		(this->*m_mouseMoveCallPointer)(e);
}


/**
	Called when a mouse button is released.
	@param e pointer to a QMouseEvent.
	*/
void Editor::mouseReleaseEvent(QMouseEvent *e)
{
	if (m_document->currentFlossIndex() != -1)
		(this->*m_mouseReleaseCallPointer)(e);
}



/**
	The main paint event.
	Creates a QPainter on the Editor widget and for each element that can be painted,
	if the element is enabled then call the relevant paint function.
	If the paint event was caused by a scroll then update the scales.
	@param e pointer to a QPaintEvent.
	*/
void Editor::paintEvent(QPaintEvent *e)
{
	static QPoint oldpos = pos();

	QPainter *painter = new QPainter();
	painter->begin(this);

	if (m_showBackgroundImages) paintBackgroundImages(painter, e->rect());
	if (m_showGrid) paintGrid(painter, e->rect());
	if (m_showStitches) paintStitches(painter, e->rect());
	if (m_showBackstitches) paintBackstitches(painter, e->rect());
	if (m_showFrenchKnots) paintFrenchKnots(painter, e->rect());
	if (m_paintToolSpecificGraphics) (this->*m_paintToolSpecificGraphics)(painter, e->rect());

	painter->end();
	delete painter;

	if (pos() != oldpos)
	{
		m_horizontalScale->setOffset(pos().x());
		m_verticalScale->setOffset(pos().y());
		oldpos = pos();
	}
}


/**
	Paint the background images.
	@param painter pointer to a QPainter to draw the backstitches on.
	@param updateRectangle a QRect defining the update rectangle.
	*/
void Editor::paintBackgroundImages(QPainter *painter, QRect updateRectangle)
{
	QListIterator<BackgroundImage *> backgroundImages = m_document->backgroundImages();
	while (backgroundImages.hasNext())
	{
		BackgroundImage *background = backgroundImages.next();
		if (background->isVisible())
		{
			QRect r = background->location();
			r.moveTo(r.left()*m_cellWidth, r.top()*m_cellHeight);
			r.setWidth(r.width()*m_cellWidth);
			r.setHeight(r.height()*m_cellHeight);
			painter->drawImage(r, background->image());
		}
	}
}


/**
	Paint the grid.
	@param painter pointer to a QPainter to draw the backstitches on.
	@param updateRectangle a QRect defining the update rectangle.
	*/
void Editor::paintGrid(QPainter *painter, QRect updateRectangle)
{
	painter->save();

	int gridMaxX = std::min(updateRectangle.right(),(int)(m_document->width()*m_cellWidth))+1;
	int gridMaxY = std::min(updateRectangle.bottom(),(int)(m_document->height()*m_cellHeight))+1;

	QPen thickLine;
	QPen thinLine;
	thickLine.setWidth(m_document->property("thickLineWidth").toInt());
	thickLine.setColor(m_document->property("thickLineColor").value<QColor>());
	thinLine.setWidth(m_document->property("thinLineWidth").toInt());
	thinLine.setColor(m_document->property("thinLineColor").value<QColor>());

	for (int x = updateRectangle.left() ; x <= gridMaxX ; x++)
	{
		for (int y = updateRectangle.top() ; y <= gridMaxY ; y++)
		{
			if ((y % m_cellHeight) == 0)
			{
				// draw a line
				if (y % (m_cellHeight*m_cellVerticalGrouping))
				painter->setPen(thinLine);
				else
				painter->setPen(thickLine);
				painter->drawLine(updateRectangle.left(), y, gridMaxX, y);
			}
		}
		if ((x % m_cellWidth) == 0)
		{
			// draw a line
			if (x % (m_cellWidth*m_cellHorizontalGrouping))
				painter->setPen(thinLine);
			else
				painter->setPen(thickLine);
			painter->drawLine(x, updateRectangle.top(), x, gridMaxY);
		}
	}
	painter->restore();
}


/**
	Paint the stitches.
	@param painter pointer to a QPainter to draw the backstitches on.
	@param updateRectangle a QRect defining the update rectangle.
	*/
void Editor::paintStitches(QPainter *painter, QRect updateRectangle)
{
	painter->save();
	unsigned width = m_document->width();
	unsigned height = m_document->height();

	int firstX = updateRectangle.left() / m_cellWidth;
	int lastX = std::min(updateRectangle.right()/m_cellWidth, width);
	int firstY = updateRectangle.top() / m_cellHeight;
	int lastY = std::min(updateRectangle.bottom()/m_cellHeight, height);

	for (int y = firstY ; y <= lastY ; y++)
	{
		for (int x = firstX ; x <= lastX ; x++)
		{
			StitchQueue *stitchQueue = m_document->stitchAt(QPoint(x, y));
			if (stitchQueue)
			{
				(this->*m_paintStitchCallPointer)(painter, x*m_cellWidth, y*m_cellHeight, m_cellWidth, m_cellHeight, stitchQueue);
			}
		}
	}
	painter->restore();
}


/**
	Paint the backstitches.
	@param painter pointer to a QPainter to draw the backstitches on.
	@param updateRectangle a QRect defining the update rectangle.
	*/
void Editor::paintBackstitches(QPainter *painter, QRect updateRectangle)
{
	painter->save();
	QListIterator<Backstitch *> backstitches = m_document->backstitches();
	while (backstitches.hasNext())
	{
		(this->*m_paintBackstitchCallPointer)(painter, backstitches.next());
	}
	painter->restore();
}


/**
	Paint the french knots.
	@param painter pointer to a QPainter to draw the french knots on.
	@param updateRectangle a QRect defining the update rectangle.
	*/
void Editor::paintFrenchKnots(QPainter *painter, QRect updateRectangle)
{
	painter->save();
	QListIterator<Knot *> knots = m_document->knots();
	while (knots.hasNext())
	{
		(this->*m_paintKnotCallPointer)(painter, knots.next());
	}
	painter->restore();
}


/**
	Paint a rubber band line if it is valid during a screen repaint.
	TODO try and style this in the same way as the rectangle.
	*/
void Editor::paintRubberBandLine(QPainter *painter, QRect updateRectangle)
{
	painter->save();
	if (m_rubberBand.isValid())
	{
		QPen pen(Qt::black);
		pen.setWidth(4);
		if (m_toolMode == ToolBackstitch)
			painter->drawLine(snapToContents(m_cellStart), snapToContents(m_cellEnd));
		else
			painter->drawLine(cellToRect(m_cellStart).center(), cellToRect(m_cellEnd).center());
	}
	painter->restore();
}


/**
	Paint a rubber band rectangle if it is valid during a screen repaint.
	This uses a standard styling for rubber band rectangles.
	*/
void Editor::paintRubberBandRectangle(QPainter *painter, QRect updateRectangle)
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


/**
	Paint a rubber band ellipse if it is valid during a screen repaint.
	TODO try and style this in the same way as the rectangle.
	*/
void Editor::paintRubberBandEllipse(QPainter *painter, QRect updateRectangle)
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


/**
	Called from the mousePressEvent when in paint mode.
	*/
void Editor::mousePressEvent_Paint(QMouseEvent *e)
{
	QPoint p = e->pos();
	QRect rect;

	if (m_currentStitchType == stitchFrenchKnot)
	{
		if (QRect(0, 0, m_document->width()*m_cellWidth, m_document->height()*m_cellHeight).contains(p))
		{
			m_cellStart = m_cellTracking = m_cellEnd = contentsToSnap(p);
			m_document->undoStack().beginMacro("Paint knots");
			m_document->undoStack().push(new AddKnotCommand(m_document, m_cellStart, m_document->currentFlossIndex()));
			rect = QRect(snapToContents(m_cellStart)-(QPoint(m_cellWidth, m_cellHeight)/2), QSize(m_cellWidth, m_cellHeight));
		}
	}
	else
	{
		m_cellStart = m_cellTracking = m_cellEnd = contentsToCell(p);
		m_zoneStart = m_zoneTracking = m_zoneEnd = contentsToZone(p);
		Stitch::Type stitchType = stitchMap[m_currentStitchType][m_zoneStart];
		m_document->undoStack().beginMacro("Paint stitches");
		m_document->undoStack().push(new AddStitchCommand(m_document, m_cellStart, stitchType, m_document->currentFlossIndex()));
		rect = cellToRect(m_cellStart);
	}
	update(rect);
}


/**
	Called from the mouseMoveEvent when in paint mode.
	*/
void Editor::mouseMoveEvent_Paint(QMouseEvent *e)
{
	QPoint p = e->pos();
	QRect rect;

	if (m_currentStitchType == stitchFrenchKnot)
	{
		if (QRect(0, 0, m_document->width()*m_cellWidth, m_document->height()*m_cellHeight).contains(p))
		{
			m_cellTracking = contentsToSnap(p);
			if (m_cellTracking != m_cellStart)
			{
				m_cellStart = m_cellTracking;
				m_document->undoStack().push(new AddKnotCommand(m_document, m_cellStart, m_document->currentFlossIndex()));
				rect = QRect(snapToContents(m_cellStart)-(QPoint(m_cellWidth, m_cellHeight)/2), QSize(m_cellWidth, m_cellHeight));
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
			m_document->undoStack().push(new AddStitchCommand(m_document, m_cellStart, stitchType, m_document->currentFlossIndex()));
			rect = cellToRect(m_cellStart);
		}
	}
	update(rect);
}


/**
	Called from the mouseReleaseEvent when in paint mode.
	*/
void Editor::mouseReleaseEvent_Paint(QMouseEvent*)
{
	m_document->undoStack().endMacro();
}


/**
	Called from the mousePressEvent when in draw mode.
	*/
void Editor::mousePressEvent_Draw(QMouseEvent *e)
{
	m_cellStart = m_cellTracking = m_cellEnd = contentsToCell(e->pos());
	m_rubberBand = QRect();
}


/**
	Called from the mouseMoveEvent when in draw mode.
	*/
void Editor::mouseMoveEvent_Draw(QMouseEvent *e)
{
	QPoint p = e->pos();
	QRect updateArea;

	//  qobject_cast<QScrollArea *>(parent())->ensureVisible(p.x(), p.y());

	m_cellTracking = contentsToCell(p);
	updateArea = (cellToRect(m_cellStart).united(cellToRect(m_cellEnd))).normalized();
	m_rubberBand = QRect();
	update(updateArea.adjusted(-m_cellWidth, -m_cellHeight, m_cellWidth, m_cellHeight));

	m_cellEnd = m_cellTracking;
	updateArea = (cellToRect(m_cellStart).united(cellToRect(m_cellEnd))).normalized();
	m_rubberBand = updateArea;
	update(updateArea.adjusted(-m_cellWidth, -m_cellHeight, m_cellWidth, m_cellHeight));
}


/**
	Called from the mouseReleaseEvent when in draw mode.
	*/
void Editor::mouseReleaseEvent_Draw(QMouseEvent*)
{
	QBitmap canvas(m_document->width(), m_document->height());
	QRect rect;
	QPainter painter;

	if (m_cellStart != m_cellEnd)
	{
		canvas.fill(Qt::color0);
		painter.begin(&canvas);
		painter.setPen(QPen(Qt::color1));
		painter.drawLine(m_cellStart, m_cellEnd);
		painter.drawPoint(m_cellStart); // TODO see if this is still necessary, possible controlled by end cap
		painter.drawPoint(m_cellEnd); // TODO see if this is still necessary, possible controlled by end cap
		painter.end();
		processBitmap(canvas, "Draw line");
	}
	m_rubberBand = QRect();
	update();
}


/**
	Called from the mousePressEvent when in erase mode.
	*/
void Editor::mousePressEvent_Erase(QMouseEvent *e)
{
	QPoint p = e->pos();
	QRect rect;

	if (e->modifiers() & Qt::ControlModifier)
	{
		// Erase a backstitch
		m_cellStart = m_cellTracking = m_cellEnd = contentsToSnap(p);
	}
	else
	{
		if (e->modifiers() & Qt::ShiftModifier)
		{
			// Delete french knots
			m_cellStart = m_cellTracking = m_cellEnd = contentsToSnap(p);
			m_document->undoStack().beginMacro("Delete knots");
			m_document->undoStack().push(new DeleteKnotCommand(m_document, m_cellStart, (m_maskColor)?m_document->currentFlossIndex():-1));
			rect = QRect(snapToContents(m_cellStart)-QPoint(m_cellWidth/2, m_cellHeight/2), QSize(m_cellWidth, m_cellHeight));
		}
		else
		{
			m_cellStart = m_cellTracking = m_cellEnd = contentsToCell(p);
			m_zoneStart = m_zoneTracking = m_zoneEnd = contentsToZone(p);
			m_document->undoStack().beginMacro("Delete stitches");
			m_document->undoStack().push(new DeleteStitchCommand(m_document, m_cellStart, m_maskStitch?stitchMap[m_currentStitchType][m_zoneStart]:Stitch::Delete, m_maskColor?m_document->currentFlossIndex():-1));
			rect = cellToRect(m_cellStart);
		}
		update(rect);
	}
}


/**
	Called from the mouseMoveEvent when in erase mode.
	*/
void Editor::mouseMoveEvent_Erase(QMouseEvent *e)
{
	QPoint p = e->pos();
	QRect rect;

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
				m_document->undoStack().push(new DeleteKnotCommand(m_document, m_cellStart, (m_maskColor)?m_document->currentFlossIndex():-1));
				rect = QRect(snapToContents(m_cellStart)-QPoint(m_cellWidth/2, m_cellHeight/2), QSize(m_cellWidth, m_cellHeight));
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
				m_document->undoStack().push(new DeleteStitchCommand(m_document, m_cellStart, m_maskStitch?stitchMap[m_currentStitchType][m_zoneStart]:Stitch::Delete, m_maskColor?m_document->currentFlossIndex():-1));
				rect = cellToRect(m_cellStart);
			}
		}
		update(rect);
	}
}


/**
	Called from the mouseReleaseEvent when in erase mode.
	*/
void Editor::mouseReleaseEvent_Erase(QMouseEvent *e)
{
	QPoint p = e->pos();
	QRect rect;

	if (e->modifiers() & Qt::ControlModifier)
	{
		// Erase a backstitch
		m_cellEnd = contentsToSnap(p);
		QListIterator<Backstitch *> backstitches = m_document->backstitches();
		while (backstitches.hasNext())
		{
			Backstitch *backstitch = backstitches.next();
			if (backstitch->contains(m_cellStart) && backstitch->contains(m_cellEnd))
			{
				m_document->undoStack().push(new DeleteBackstitchCommand(m_document, backstitch->start(), backstitch->end(), m_maskColor?m_document->currentFlossIndex():-1));
				update(QRect(snapToContents(backstitch->start()), snapToContents(backstitch->end())).normalized().adjusted(-2, -2, 2, 2));
				break;
			}
		}
	}
	else
		m_document->undoStack().endMacro();
	// Nothing needs to be done for french knots or stitches which are handled in mouseMoveEvent_Erase
}


/**
	Called from the mousePressEvent when in rectangle mode
	*/
void Editor::mousePressEvent_Rectangle(QMouseEvent *event)
{
	if (m_rubberBand.isValid())
	{
		QRect r = m_rubberBand;
		m_rubberBand = QRect();
		repaint(r);
	}

	m_cellStart = m_cellTracking = m_cellEnd = contentsToCell(event->pos());
	QRect updateArea = cellToRect(m_cellStart);
	m_rubberBand = updateArea;
	repaint(updateArea.adjusted(-m_cellWidth, -m_cellHeight, m_cellWidth, m_cellHeight));
}


/**
	Called from the mouseMoveEvent when in rectangle mode.
	*/
void Editor::mouseMoveEvent_Rectangle(QMouseEvent *event)
{
	m_cellTracking = contentsToCell(event->pos());
	if (m_cellTracking != m_cellEnd)
	{
		m_cellEnd = m_cellTracking;
		QRect updateArea = (cellToRect(m_cellStart).united(cellToRect(m_cellEnd))).normalized();
		m_rubberBand = updateArea;
		repaint(updateArea.adjusted(-m_cellWidth, -m_cellHeight, m_cellWidth, m_cellHeight));
	}
}


/**
	Called from the mouseReleaseEvent when in fill rectangle mode.
	*/
void Editor::mouseReleaseEvent_Rectangle(QMouseEvent*)
{
	m_selectionArea.setTopLeft(m_cellStart);
	m_selectionArea.setBottomRight(m_cellEnd);
	m_selectionArea = m_selectionArea.normalized();

	int x = m_selectionArea.left();
	int y = m_selectionArea.top();
	QPoint cell(x, y);

	m_document->undoStack().beginMacro("Draw rectangle");
	while (++x <= m_selectionArea.right())
	{
		m_document->undoStack().push(new AddStitchCommand(m_document, cell, Stitch::Full, m_document->currentFlossIndex()));
		cell.setX(x);
	}
	while (++y <= m_selectionArea.bottom())
	{
		m_document->undoStack().push(new AddStitchCommand(m_document, cell, Stitch::Full, m_document->currentFlossIndex()));
		cell.setY(y);
	}
	while (--x >= m_selectionArea.left())
	{
		m_document->undoStack().push(new AddStitchCommand(m_document, cell, Stitch::Full, m_document->currentFlossIndex()));
		cell.setX(x);
	}
	while (--y >= m_selectionArea.top())
	{
		m_document->undoStack().push(new AddStitchCommand(m_document, cell, Stitch::Full, m_document->currentFlossIndex()));
		cell.setY(y);
	}
	m_document->undoStack().endMacro();

	QRect updateArea = (cellToRect(m_cellStart).united(cellToRect(m_cellEnd))).normalized();
	m_selectionArea = QRect();	// this will clear the selection area rectangle on the next repaint
	m_rubberBand = QRect();		// this will clear the rubber band rectangle on the next repaint
	update(updateArea.adjusted(-m_cellWidth, -m_cellHeight, m_cellWidth, m_cellHeight));
}


/**
	Called from the mousePressEvent when in fill rectangle mode.
	*/
void Editor::mousePressEvent_FillRectangle(QMouseEvent *event)
{
	if (m_rubberBand.isValid())
	{
		QRect r = m_rubberBand;
		m_rubberBand = QRect();
		repaint(r);
	}

	m_cellStart = m_cellTracking = m_cellEnd = contentsToCell(event->pos());
	QRect updateArea = cellToRect(m_cellStart);
	m_rubberBand = updateArea;
	repaint(updateArea.adjusted(-m_cellWidth, -m_cellHeight, m_cellWidth, m_cellHeight));
}


/**
	Called from the mouseMoveEvent when in fill rectangle mode.
	*/
void Editor::mouseMoveEvent_FillRectangle(QMouseEvent *event)
{
	m_cellTracking = contentsToCell(event->pos());
	if (m_cellTracking != m_cellEnd)
	{
		m_cellEnd = m_cellTracking;
		QRect updateArea = (cellToRect(m_cellStart).united(cellToRect(m_cellEnd))).normalized();
		m_rubberBand = updateArea;
		repaint(updateArea.adjusted(-m_cellWidth, -m_cellHeight, m_cellWidth, m_cellHeight));
	}
}


/**
	Called from the mouseReleaseEvent when in fill rectangle mode.
	*/
void Editor::mouseReleaseEvent_FillRectangle(QMouseEvent *event)
{
	m_selectionArea.setTopLeft(m_cellStart);
	m_selectionArea.setBottomRight(m_cellEnd);
	m_selectionArea = m_selectionArea.normalized();

	int x = m_selectionArea.left();
	int y = m_selectionArea.top();
	QPoint cell(x, y);

	m_document->undoStack().beginMacro("Fill rectangle");
	for (int y = m_selectionArea.top() ; y <= m_selectionArea.bottom() ; y++)
	{
		for (int x = m_selectionArea.left() ; x <= m_selectionArea.right() ; x++)
		{
			QPoint cell(x, y);
			m_document->undoStack().push(new AddStitchCommand(m_document, cell, Stitch::Full, m_document->currentFlossIndex()));
		}
	}
	m_document->undoStack().endMacro();

	QRect updateArea = (cellToRect(m_cellStart).united(cellToRect(m_cellEnd))).normalized();
	m_selectionArea = QRect();	// this will clear the selection area rectangle on the next repaint
	m_rubberBand = QRect();		// this will clear the rubber band rectangle on the next repaint
	update(updateArea.adjusted(-m_cellWidth, -m_cellHeight, m_cellWidth, m_cellHeight));
}


/**
	Called from the mousePressEvent when in ellipse mode.
	*/
void Editor::mousePressEvent_Ellipse(QMouseEvent*)
{
}


/**
	Called from the mouseMoveEvent when in ellipse mode.
	*/
void Editor::mouseMoveEvent_Ellipse(QMouseEvent*)
{
}


/**
	Called from the mouseReleaseEvent when in ellipse mode
	*/
void Editor::mouseReleaseEvent_Ellipse(QMouseEvent*)
{
}


/**
	Called from the mousePressEvent when in fill ellipse mode.
	*/
void Editor::mousePressEvent_FillEllipse(QMouseEvent *event)
{
	if (m_rubberBand.isValid())
	{
		QRect r = m_rubberBand;
		m_rubberBand = QRect(0, 0, 0, 0);
		repaint(r);
	}

	m_cellStart = m_cellTracking = m_cellEnd = contentsToCell(event->pos());
	QRect updateArea = cellToRect(m_cellStart);
	m_rubberBand = updateArea;
	repaint(updateArea.adjusted(-m_cellWidth, -m_cellHeight, m_cellWidth, m_cellHeight));
}


/**
	Called from the mouseMoveEvent when in fill ellipse mode.
	*/
void Editor::mouseMoveEvent_FillEllipse(QMouseEvent *event)
{
	m_cellTracking = contentsToCell(event->pos());
	if (m_cellTracking != m_cellEnd)
	{
		m_cellEnd = m_cellTracking;
		QRect updateArea = (cellToRect(m_cellStart).united(cellToRect(m_cellEnd))).normalized();
		m_rubberBand = updateArea;
		repaint(updateArea.adjusted(-m_cellWidth, -m_cellHeight, m_cellWidth, m_cellHeight));
	}
}


/**
	Called from the mouseReleaseEvent when in fill ellipse mode
	TODO Although this is the same code from KDE3/QT3, it doesn't produce
	the same result.  Needs work.
	*/
void Editor::mouseReleaseEvent_FillEllipse(QMouseEvent*)
{
	QBitmap canvas(m_document->width()*2, m_document->height()*2);
	QRect rect;
	QPainter painter;

	QPainterPath path;
	QPoint s(m_cellStart.x()*2, m_cellStart.y()*2);
	QPoint e(m_cellEnd.x()*2, m_cellEnd.y()*2);
	path.addEllipse(QRect(s, e).normalized());
	canvas.fill(Qt::color0);
	painter.begin(&canvas);
	painter.setRenderHint(QPainter::Antialiasing, true);
	painter.setPen(QPen(Qt::color1));
	painter.setBrush(Qt::color1);
	painter.drawPath(path);
	painter.end();
	processBitmap(canvas, "Fill ellipse");

	m_rubberBand = QRect();
	m_selectionArea = QRect();

	update();
}


/**
	Called from the mousePressEvent when in fill polyline mode.
	*/
void Editor::mousePressEvent_FillPolyline(QMouseEvent*)
{
}


/**
	Called from the mouseMoveEvent when in fill polyline mode.
	*/
void Editor::mouseMoveEvent_FillPolyline(QMouseEvent*)
{
}


/**
	Called from the mouseReleaseEvent when in fill polyline mode.
	*/
void Editor::mouseReleaseEvent_FillPolyline(QMouseEvent*)
{
}


/**
	Called from the mousePressEvent when in text mode.
	*/
void Editor::mousePressEvent_Text(QMouseEvent*)
{
}


/**
	Called from the mouseMoveEvent when in text mode.
	*/
void Editor::mouseMoveEvent_Text(QMouseEvent*)
{
}


/**
	Called from the mouseReleaseEvent when in text mode.
	*/
void Editor::mouseReleaseEvent_Text(QMouseEvent*)
{
}


/**
	Called from the mousePressEvent when in select mode.
	*/
void Editor::mousePressEvent_Select(QMouseEvent *event)
{
	if (m_rubberBand.isValid())
	{
		QRect r = m_rubberBand;
		m_rubberBand = QRect(0, 0, 0, 0);
		repaint(r);
	}

	m_cellStart = m_cellTracking = m_cellEnd = contentsToCell(event->pos());
	QRect updateArea = cellToRect(m_cellStart);
	m_rubberBand = updateArea;
	repaint(updateArea.adjusted(-m_cellWidth, -m_cellHeight, m_cellWidth, m_cellHeight));
}


/**
	Called from the mouseMoveEvent when in select mode.
	*/
void Editor::mouseMoveEvent_Select(QMouseEvent *event)
{
	m_cellTracking = contentsToCell(event->pos());
	if (m_cellTracking != m_cellEnd)
	{
		m_cellEnd = m_cellTracking;
		QRect updateArea = (cellToRect(m_cellStart).united(cellToRect(m_cellEnd))).normalized();
		m_rubberBand = updateArea;
		repaint(updateArea.adjusted(-m_cellWidth, -m_cellHeight, m_cellWidth, m_cellHeight));
	}
}


/**
	Called from the mouseReleaseEvent when in select mode
	*/
void Editor::mouseReleaseEvent_Select(QMouseEvent*)
{
	m_selectionArea.setTopLeft(m_cellStart);
	m_selectionArea.setBottomRight(m_cellEnd);
	m_selectionArea = m_selectionArea.normalized();
}


/**
	Called from the mousePressEvent when in backstitch mode.
	*/
void Editor::mousePressEvent_Backstitch(QMouseEvent *e)
{
	m_cellStart = m_cellTracking = m_cellEnd = contentsToSnap(e->pos());
	m_rubberBand = QRect();
}


/**
	Called from the mouseMoveEvent when in backstitch mode.
	*/
void Editor::mouseMoveEvent_Backstitch(QMouseEvent *e)
{
	QRect updateArea;

	m_cellTracking = contentsToSnap(e->pos());
	updateArea = QRect(snapToContents(m_cellStart), snapToContents(m_cellEnd)).normalized();
	m_rubberBand = QRect();
	update(updateArea.adjusted(-m_cellWidth, -m_cellHeight, m_cellWidth, m_cellHeight));

	m_cellEnd = m_cellTracking;
	updateArea = QRect(snapToContents(m_cellStart), snapToContents(m_cellEnd)).normalized();
	m_rubberBand = updateArea;
	update(updateArea.adjusted(-m_cellWidth, -m_cellHeight, m_cellWidth, m_cellHeight));
}


/**
	Called from the mouseReleaseEvent when in backstitch mode.
	*/
void Editor::mouseReleaseEvent_Backstitch(QMouseEvent *e)
{
	m_rubberBand = QRect();
	update();
	m_document->undoStack().push(new AddBackstitchCommand(m_document, m_cellStart, m_cellEnd, m_document->currentFlossIndex()));
}


/**
	Paint stitches using the standard mode.
	Stitches are displayed as color lines.
	@param painter a pointer to a QPainter used to paint the stitches on.
	@param x coordinate of the top left of the update rectangle.
	@param y coordinate of the top left of the update rectangle.
	@param w width of the update rectangle.
	@param h height of the update rectangle.
	@param stitchQueue pointer to the queue of stitches to be painted.
	*/
void Editor::paintStitchesAsRegularStitches(QPainter *painter, int x, int y, int w, int h, StitchQueue *stitchQueue)
{
	StitchQueue::const_iterator it;
	for (it = stitchQueue->begin() ; it != stitchQueue->end() ; ++it)
	{
		Stitch *stitch = *it;
		const Floss *floss = m_document->floss(stitch->floss());
		QPen pen(floss->color());
		pen.setWidth(4);
		painter->setPen(pen);

		QRect rect(x, y, w, h);

		switch (stitch->type())
		{
			case Stitch::Delete:
				break;

			case Stitch::TLQtr:
				painter->drawLine(rect.topLeft(), rect.center());
				break;

			case Stitch::TRQtr:
				painter->drawLine(rect.topRight(), rect.center());
				break;

			case Stitch::BLQtr:
				painter->drawLine(rect.bottomLeft(), rect.center());
				break;

			case Stitch::BTHalf:
				painter->drawLine(rect.bottomLeft(), rect.topRight());
				break;

			case Stitch::TL3Qtr:
				painter->drawLine(rect.bottomLeft(), rect.topRight());
				painter->drawLine(rect.topLeft(), rect.center());
				break;

			case Stitch::BRQtr:
				painter->drawLine(rect.bottomRight(), rect.center());
				break;

			case Stitch::TBHalf:
				painter->drawLine(rect.topLeft(), rect.bottomRight());
				break;

			case Stitch::TR3Qtr:
				painter->drawLine(rect.topLeft(), rect.bottomRight());
				painter->drawLine(rect.topRight(), rect.center());
				break;

			case Stitch::BL3Qtr:
				painter->drawLine(rect.topLeft(), rect.bottomRight());
				painter->drawLine(rect.bottomLeft(), rect.center());
				break;

			case Stitch::BR3Qtr:
				painter->drawLine(rect.bottomLeft(), rect.topRight());
				painter->drawLine(rect.bottomRight(), rect.center());
				break;

			case Stitch::Full:
				painter->drawLine(rect.topLeft(), rect.bottomRight());
				painter->drawLine(rect.bottomLeft(), rect.topRight());
				break;

			case Stitch::TLSmallHalf:
				painter->drawLine(rect.left(), rect.center().y(), rect.center().x(), rect.top());
				break;

			case Stitch::TRSmallHalf:
				painter->drawLine(rect.center().x(), rect.top(), rect.right(), rect.center().y());
				break;

			case Stitch::BLSmallHalf:
				painter->drawLine(rect.left(), rect.center().y(), rect.center().x(), rect.bottom());
				break;

			case Stitch::BRSmallHalf:
				painter->drawLine(rect.center().x(), rect.bottom(), rect.right(), rect.center().y());
				break;

			case Stitch::TLSmallFull:
				painter->drawLine(rect.left(), rect.center().y(), rect.center().x(), rect.top());
				painter->drawLine(rect.topLeft(), rect.center());
				break;

			case Stitch::TRSmallFull:
				painter->drawLine(rect.center().x(), rect.top(), rect.right(), rect.center().y());
				painter->drawLine(rect.topRight(), rect.center());
				break;

			case Stitch::BLSmallFull:
				painter->drawLine(rect.left(), rect.center().y(), rect.center().x(), rect.bottom());
				painter->drawLine(rect.bottomLeft(), rect.center());
				break;

			case Stitch::BRSmallFull:
				painter->drawLine(rect.center().x(), rect.bottom(), rect.right(), rect.center().y());
				painter->drawLine(rect.bottomRight(), rect.center());
				break;
		}
	}
}


/**
	Paint stitches using the black and white symbols mode.
	Stitches are displayed with a black or white symbol depending on the background color
	to maintain the contrast.
	@param painter a pointer to a QPainter used to paint the stitches on.
	@param x coordinate of the top left of the update rectangle.
	@param y coordinate of the top left of the update rectangle.
	@param w width of the update rectangle.
	@param h height of the update rectangle.
	@param stitchQueue pointer to the queue of stitches to be painted.
	*/
void Editor::paintStitchesAsBlackWhiteSymbols(QPainter *painter, int x, int y, int w, int h, StitchQueue *stitchQueue)
{
}


/**
	Paint stitches using the color symbols mode.
	Stitches are displayed with a color symbol.
	@param painter a pointer to a QPainter used to paint the stitches on.
	@param x coordinate of the top left of the update rectangle.
	@param y coordinate of the top left of the update rectangle.
	@param w width of the update rectangle.
	@param h height of the update rectangle.
	@param stitchQueue pointer to the queue of stitches to be painted.
	*/
void Editor::paintStitchesAsColorSymbols(QPainter *painter, int x, int y, int w, int h, StitchQueue *stitchQueue)
{
}


/**
	Paint stitches using the color block mode.
	Stitches are displayed with a color block.
	@param painter a pointer to a QPainter used to paint the stitches on.
	@param x coordinate of the top left of the update rectangle.
	@param y coordinate of the top left of the update rectangle.
	@param w width of the update rectangle.
	@param h height of the update rectangle.
	@param stitchQueue pointer to the queue of stitches to be painted.
	*/
void Editor::paintStitchesAsColorBlocks(QPainter *painter, int x, int y, int w, int h, StitchQueue *stitchQueue)
{
}


/**
	Paint stitches using the color block with symbols mode.
	Stitches are displayed with a color block with either a black or white symbol depending
	on the color of the block to maintain contrast.
	@param painter a pointer to a QPainter used to paint the stitches on.
	@param x coordinate of the top left of the update rectangle.
	@param y coordinate of the top left of the update rectangle.
	@param w width of the update rectangle.
	@param h height of the update rectangle.
	@param stitchQueue pointer to the queue of stitches to be painted.
	*/
void Editor::paintStitchesAsColorBlocksSymbols(QPainter *painter, int x, int y, int w, int h, StitchQueue *stitchQueue)
{
}


/**
	Paint stitches using the color hilight mode.
	Stitches of the currently selected color are displayed normally, others are displayed in a lightGray color.
	@param painter a pointer to a QPainter used to paint the stitches on.
	@param x coordinate of the top left of the update rectangle.
	@param y coordinate of the top left of the update rectangle.
	@param w width of the update rectangle.
	@param h height of the update rectangle.
	@param stitchQueue pointer to the queue of stitches to be painted.
	*/
void Editor::paintStitchesAsColorHilight(QPainter *painter, int x, int y, int w, int h, StitchQueue *stitchQueue)
{
	int currentFlossIndex = m_document->currentFlossIndex();

	StitchQueue::const_iterator it;
	for (it = stitchQueue->begin() ; it != stitchQueue->end() ; ++it)
	{
		Stitch *stitch = *it;
		const Floss *floss = m_document->floss(stitch->floss());
		QPen pen(floss->color());
		if (stitch->floss() != currentFlossIndex)
		pen.setColor(Qt::lightGray);;
		pen.setWidth(2);
		painter->setPen(pen);

		QRect rect(x, y, w, h);

		switch (stitch->type())
		{
			case Stitch::Delete:
				break;

			case Stitch::TLQtr:
				painter->drawLine(rect.topLeft(), rect.center());
				break;

			case Stitch::TRQtr:
				painter->drawLine(rect.topRight(), rect.center());
				break;

			case Stitch::BLQtr:
				painter->drawLine(rect.bottomLeft(), rect.center());
				break;

			case Stitch::BTHalf:
				painter->drawLine(rect.bottomLeft(), rect.topRight());
				break;

			case Stitch::TL3Qtr:
				painter->drawLine(rect.bottomLeft(), rect.topRight());
				painter->drawLine(rect.topLeft(), rect.center());
				break;

			case Stitch::BRQtr:
				painter->drawLine(rect.bottomRight(), rect.center());
				break;

			case Stitch::TBHalf:
				painter->drawLine(rect.topLeft(), rect.bottomRight());
				break;

			case Stitch::TR3Qtr:
				painter->drawLine(rect.topLeft(), rect.bottomRight());
				painter->drawLine(rect.topRight(), rect.center());
				break;

			case Stitch::BL3Qtr:
				painter->drawLine(rect.topLeft(), rect.bottomRight());
				painter->drawLine(rect.bottomLeft(), rect.center());
				break;

			case Stitch::BR3Qtr:
				painter->drawLine(rect.bottomLeft(), rect.topRight());
				painter->drawLine(rect.bottomRight(), rect.center());
				break;

			case Stitch::Full:
				painter->drawLine(rect.topLeft(), rect.bottomRight());
				painter->drawLine(rect.bottomLeft(), rect.topRight());
				break;

			case Stitch::TLSmallHalf:
				painter->drawLine(rect.left(), rect.center().y(), rect.center().x(), rect.top());
				break;

			case Stitch::TRSmallHalf:
				painter->drawLine(rect.center().x(), rect.top(), rect.right(), rect.center().y());
				break;

			case Stitch::BLSmallHalf:
				painter->drawLine(rect.left(), rect.center().y(), rect.center().x(), rect.bottom());
				break;

			case Stitch::BRSmallHalf:
				painter->drawLine(rect.center().x(), rect.bottom(), rect.right(), rect.center().y());
				break;

			case Stitch::TLSmallFull:
				painter->drawLine(rect.left(), rect.center().y(), rect.center().x(), rect.top());
				painter->drawLine(rect.topLeft(), rect.center());
				break;

			case Stitch::TRSmallFull:
				painter->drawLine(rect.center().x(), rect.top(), rect.right(), rect.center().y());
				painter->drawLine(rect.topRight(), rect.center());
				break;

			case Stitch::BLSmallFull:
				painter->drawLine(rect.left(), rect.center().y(), rect.center().x(), rect.bottom());
				painter->drawLine(rect.bottomLeft(), rect.center());
				break;

			case Stitch::BRSmallFull:
				painter->drawLine(rect.center().x(), rect.bottom(), rect.right(), rect.center().y());
				painter->drawLine(rect.bottomRight(), rect.center());
				break;
		}
	}
}


/**
	Paint a backstitch as a color line.
	@param painter pointer to a QPainter to draw on.
	@param backstitch pointer to a backstitch to draw.
	*/
void Editor::paintBackstitchesAsColorLines(QPainter *painter, Backstitch *backstitch)
{
	const Floss *floss = m_document->floss(backstitch->floss());
	QPen pen(floss->color());
	pen.setWidth(4);
	painter->setPen(pen);
	painter->drawLine(snapToContents(backstitch->start()), snapToContents(backstitch->end()));
}


/**
	Paint a backstitch as a black & white symbol.
	@param painter pointer to a QPainter to draw on.
	@param backstitch pointer to a backstitch to draw.
	*/
void Editor::paintBackstitchesAsBlackWhiteSymbols(QPainter *painter, Backstitch *backstitch)
{
}


/**
	Paint a backstitch as a color hilight.
	@param painter pointer to a QPainter to draw on.
	@param backstitch pointer to a backstitch to draw.
	*/
void Editor::paintBackstitchesAsColorHilight(QPainter *painter, Backstitch *backstitch)
{
	const Floss *floss = m_document->floss(backstitch->floss());
	QPen pen(floss->color());
	if (backstitch->floss() != m_document->currentFlossIndex())
		pen.setColor(Qt::lightGray);
	pen.setWidth(4);
	painter->setPen(pen);
	painter->drawLine(snapToContents(backstitch->start()), snapToContents(backstitch->end()));
}


/**
	Paint a knot as a color block.
	@param painter pointer to a QPainter to draw on.
	@param knot pointer to a knot to draw.
	*/
void Editor::paintKnotsAsColorBlocks(QPainter *painter, Knot *knot)
{
}


/**
	Paint a knot as a color symbol.
	@param painter pointer to a QPainter to draw on.
	@param knot pointer to a knot to draw.
	*/
void Editor::paintKnotsAsColorSymbols(QPainter *painter, Knot *knot)
{
}


/**
	Paint a knot as a black & white symbol.
	@param painter pointer to a QPainter to draw on.
	@param knot pointer to a knot to draw.
	*/
void Editor::paintKnotsAsBlackWhiteSymbols(QPainter *painter, Knot *knot)
{
}


/**
	Paint a knot as a color hilight.
	@param painter pointer to a QPainter to draw on.
	@param knot pointer to a knot to draw.
	*/
void Editor::paintKnotsAsColorHilight(QPainter *painter, Knot *knot)
{
}


/**
	Convert an editor coordinate to a cell coordinate.
	No error checking is done to see if the coordinates are outside
	the canvas area.
	@param p reference to a QPoint representing the editor coordinate.
	@return a QPoint representing the cell coordinate.
	*/
QPoint Editor::contentsToCell(const QPoint &p) const
{
	return QPoint(p.x()/m_cellHeight, p.y()/m_cellWidth);
}


/**
	Convert an editor coordinate to a cell zone.
	@param p reference to a QPoint representing the editor coordinate.
	@return a number representing the zone of the cell ranging from 1 to 4,
	top-left, top-right, bottom-left, bottom-right.
	*/
int Editor::contentsToZone(const QPoint &p) const
{
	QPoint cell(p.x()*2/m_cellHeight, p.y()*2/m_cellWidth);
	int zone = (cell.y()%2)*2 + (cell.x()%2);
	return zone;
}


/**
	Convert an editor coordinate to a snap point.
	No error checking is done to see if the coordinates are outside
	the canvas area.
	@param p reference to a QPoint representing the editor coordinates.
	@return a QPoint representing the snap coordinates
	*/
QPoint Editor::contentsToSnap(const QPoint &p) const
{
	int w = m_document->width()*2;
	int h = m_document->height()*2;

	int x = (int)(round((double)(w*p.x())/width()));
	int y = (int)(round((double)(h*p.y())/height()));

	return QPoint(x, y);
}


/**
	Convert a snap coordinate to a point mapped t
	the editor window.
	@param p reference to a QPoint representing the snap coordinates.
	@return a QPoint representing the x,y coordinates on the editor.
	*/
QPoint Editor::snapToContents(const QPoint &p) const
{
	int x = m_cellWidth*p.x()/2;
	int y = m_cellHeight*p.y()/2;

	return QPoint(x, y);
}


/**
	Convert a cell coordinate to a rectangle mapped to
	the editor window.
	@param cell QPoint containing the x,y coordinates of the cell.
	@return a QRect representing the x,y,width,height of the editor coordinates
	*/
QRect Editor::cellToRect(QPoint cell)
{
	int x = cell.x()*m_cellWidth;
	int y = cell.y()*m_cellHeight;
	return QRect(x, y, m_cellWidth, m_cellHeight);
}


/**
	Format the editor scales as stitches.
	*/
void Editor::formatScalesAsStitches()
{
	m_horizontalScale->setUnits(Configuration::EnumEditor_FormatScalesAs::Stitches);
	m_verticalScale->setUnits(Configuration::EnumEditor_FormatScalesAs::Stitches);
}


/**
	Format the editor scales as CM.
	*/
void Editor::formatScalesAsCM()
{
	m_horizontalScale->setUnits(Configuration::EnumEditor_FormatScalesAs::CM);
	m_verticalScale->setUnits(Configuration::EnumEditor_FormatScalesAs::CM);
}


/**
	Format the editor scales as inches.
	*/
void Editor::formatScalesAsInches()
{
	m_horizontalScale->setUnits(Configuration::EnumEditor_FormatScalesAs::Inches);
	m_verticalScale->setUnits(Configuration::EnumEditor_FormatScalesAs::Inches);
}


/**
	Set the editor zoom factor.
	*/
void Editor::zoom()
{
	KAction *action = qobject_cast<KAction *>(sender());
	QVariant v = action->data();
	double factor = v.toDouble();
	m_cellWidth = (int)((m_document->property("cellWidth").toDouble())*factor);
	m_cellHeight = (int)((m_document->property("cellHeight").toDouble())*factor);
	m_horizontalScale->setCellSize(m_cellWidth);
	m_verticalScale->setCellSize(m_cellHeight);
	this->resize(m_document->width()*m_cellWidth + 1, m_document->height()*m_cellHeight + 1);
}


void Editor::processBitmap(QBitmap &canvas, const QString &string)
{
	QImage image;
	image = canvas.toImage();
	m_document->undoStack().beginMacro(string);
	for (int y = 0 ; y < image.height() ; y++)
	{
		for (int x = 0 ; x < image.width() ; x++)
		{
			if (image.pixelIndex(x, y) == 1)
			{
				QPoint cell(x, y);
				m_document->undoStack().push(new AddStitchCommand(m_document, cell, Stitch::Full, m_document->currentFlossIndex()));
			}
		}
	}
	m_document->undoStack().endMacro();
}
