/****************************************************************************
 *	Copyright (C) 2010 by Stephen Allewell									*
 *	stephen@mirramar.adsl24.co.uk											*
 *																			*
 *	This program is free software; you can redistribute it and/or modify	*
 *	it under the terms of the GNU General Public License as published by	*
 *	the Free Software Foundation; either version 2 of the License, or		*
 *	(at your option) any later version.										*
 ****************************************************************************/


#ifndef EDITOR_H
#define EDITOR_H


#include <QBitmap>
#include <QWidget>
#include <QPoint>

#include "stitch.h"


class QRubberBand;

class KActionCollection;
class KAction;

class Document;
class Scale;


class Editor : public QWidget
{
	Q_OBJECT

	public:
		enum SelectedStitchType {
			stitchQuarter,
			stitchHalf,
			stitch3Quarter,
			stitchFull,
			stitchSmallHalf,
			stitchSmallFull,
			stitchFrenchKnot
		};

		Editor();
		~Editor();

		void setDocument(Document *);
		Scale *horizontalScale() const;
		Scale *verticalScale() const;
		QRect selectionArea();

	public slots:
		void mirrorVertical();
		void mirrorHorizontal();

		void rotate90();
		void rotate180();
		void rotate270();

		void actualSize();
		void fitToPage();
		void fitToWidth();
		void fitToHeight();

		void zoom();
		void loadSettings();

		void formatScalesAsStitches();
		void formatScalesAsCM();
		void formatScalesAsInches();

		void selectPaintTool();
		void selectDrawTool();
		void selectEraseTool();
		void selectRectangleTool();
		void selectFillRectangleTool();
		void selectEllipseTool();
		void selectFillEllipseTool();
		void selectFillPolylineTool();
		void selectTextTool();
		void selectSelectTool();
		void selectBackstitchTool();

		void selectStitchQuarter();
		void selectStitchHalf();
		void selectStitch3Quarter();
		void selectStitchFull();
		void selectStitchSmallHalf();
		void selectStitchSmallFull();
		void selectStitchFrenchKnot();

		void setShowStitches();
		void setShowBackstitches();
		void setShowFrenchKnots();
		void setShowGrid();
		void setCopyMirrorRotate();

		void setMaskStitch();
		void setMaskColor();
		void setMaskBackstitch();
		void setMaskKnot();

		void showStitchesAsRegularStitches();
		void showStitchesAsBlackWhiteSymbols();
		void showStitchesAsColorSymbols();
		void showStitchesAsColorBlocks();
		void showStitchesAsColorBlocksSymbols();
		void showStitchesAsColorHilight();
		void showBackstitchesAsColorLines();
		void showBackstitchesAsBlackWhiteSymbols();
		void showBackstitchesAsColorHilight();
		void showKnotsAsColorBlocks();
		void showKnotsAsBlackWhiteSymbols();
		void showKnotsAsColorHilight();

	protected:
		void contextMenuEvent(QContextMenuEvent*);
		void dragEnterEvent(QDragEnterEvent*);
		void dragLeaveEvent(QDragLeaveEvent*);
		void dragMoveEvent(QDragMoveEvent*);
		void dropEvent(QDropEvent*);
		void keyPressEvent(QKeyEvent*);
		void keyReleaseEvent(QKeyEvent*);
		void mousePressEvent(QMouseEvent*);
		void mouseMoveEvent(QMouseEvent*);
		void mouseReleaseEvent(QMouseEvent*);
		void paintEvent(QPaintEvent*);

	private:
		enum ToolMode {
			ToolPaint,
			ToolDraw,
			ToolErase,
			ToolRectangle,
			ToolFillRectangle,
			ToolEllipse,
			ToolFillEllipse,
			ToolFillPolyline,
			ToolText,
			ToolSelect,
			ToolBackstitch,
			ToolPaste
		};

		void paintBackgroundImages(QPainter *, QRect);
		void paintGrid(QPainter *, QRect);
		void paintStitches(QPainter *, QRect);
		void paintBackstitches(QPainter *, QRect);
		void paintFrenchKnots(QPainter *, QRect);

		void paintRubberBandLine(QPainter *, QRect);
		void paintRubberBandRectangle(QPainter *, QRect);
		void paintRubberBandEllipse(QPainter *, QRect);

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

		void mousePressEvent_FillPolyline(QMouseEvent*);
		void mouseMoveEvent_FillPolyline(QMouseEvent*);
		void mouseReleaseEvent_FillPolyline(QMouseEvent*);

		void mousePressEvent_Text(QMouseEvent*);
		void mouseMoveEvent_Text(QMouseEvent*);
		void mouseReleaseEvent_Text(QMouseEvent*);

		void mousePressEvent_Select(QMouseEvent*);
		void mouseMoveEvent_Select(QMouseEvent*);
		void mouseReleaseEvent_Select(QMouseEvent*);

		void mousePressEvent_Backstitch(QMouseEvent*);
		void mouseMoveEvent_Backstitch(QMouseEvent*);
		void mouseReleaseEvent_Backstitch(QMouseEvent*);

		void paintStitchesAsRegularStitches(QPainter *, int, int, int, int, Stitch::Queue *);
		void paintStitchesAsBlackWhiteSymbols(QPainter *, int, int, int, int, Stitch::Queue *);
		void paintStitchesAsColorSymbols(QPainter *, int, int, int, int, Stitch::Queue *);
		void paintStitchesAsColorBlocks(QPainter *, int, int, int, int, Stitch::Queue *);
		void paintStitchesAsColorBlocksSymbols(QPainter *, int, int, int, int, Stitch::Queue *);
		void paintStitchesAsColorHilight(QPainter *, int, int, int, int, Stitch::Queue *);

		void paintBackstitchesAsColorLines(QPainter *, Backstitch *);
		void paintBackstitchesAsBlackWhiteSymbols(QPainter *, Backstitch *);
		void paintBackstitchesAsColorHilight(QPainter *, Backstitch *);

		void paintKnotsAsColorBlocks(QPainter *, Knot *);
		void paintKnotsAsBlackWhiteSymbols(QPainter *, Knot *);
		void paintKnotsAsColorHilight(QPainter *, Knot *);

		QPoint contentsToCell(const QPoint &) const;
		int contentsToZone(const QPoint &) const;
		QPoint contentsToSnap(const QPoint &) const;
		QPoint snapToContents(const QPoint &) const;
		QRect cellToRect(QPoint);
		void processBitmap(QBitmap &);

		typedef void (Editor::*mouseEventCallPointer)(QMouseEvent*);
		typedef void (Editor::*paintStitchCallPointer)(QPainter *, int, int, int, int, Stitch::Queue *);
		typedef void (Editor::*paintBackstitchCallPointer)(QPainter *, Backstitch *);
		typedef void (Editor::*paintKnotCallPointer)(QPainter *, Knot *);
		typedef void (Editor::*paintToolSpecificGraphicsCallPointer)(QPainter *, QRect);

		mouseEventCallPointer m_mousePressCallPointer;
		mouseEventCallPointer m_mouseMoveCallPointer;
		mouseEventCallPointer m_mouseReleaseCallPointer;
		enum ToolMode m_toolMode;

		paintStitchCallPointer m_paintStitchCallPointer;
		paintBackstitchCallPointer m_paintBackstitchCallPointer;
		paintKnotCallPointer m_paintKnotCallPointer;
		paintToolSpecificGraphicsCallPointer m_paintToolSpecificGraphics;

		unsigned m_cellWidth;  /*	Width in pixels of the visible cell
									Initialised from configuration object
									Affected by zoom functions
								*/
		unsigned m_cellHeight; /* 	Height in pixels of the visible cell
									Initialised from configuration object
									Affected by zoom functions
								*/
		unsigned m_cellHorizontalGrouping;
		unsigned m_cellVerticalGrouping;

		Document *m_document;  /* 	Pointer to the Document object
									Initialised from setDocument function
								*/

		bool m_showBackgroundImages;
		bool m_showGrid;
		bool m_showStitches;
		bool m_showBackstitches;
		bool m_showFrenchKnots;
		bool m_copyMirrorRotate;
		bool m_maskStitch;
		bool m_maskColor;
		bool m_maskBackstitch;
		bool m_maskKnot;

		QPoint m_cellStart;
		QPoint m_cellTracking;
		QPoint m_cellEnd;
		int m_zoneStart;
		int m_zoneTracking;
		int m_zoneEnd;

		QPoint m_origin;
		QRect m_rubberBand;
		QRect m_selectionArea;
		enum SelectedStitchType m_currentStitchType;

		Scale *m_horizontalScale;
		Scale *m_verticalScale;
};


#endif
