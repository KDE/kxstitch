/****************************************************************************
 *	Copyright (C) 2010 by Stephen Allewell									*
 *	stephen@mirramar.adsl24.co.uk											*
 *																			*
 *	This program is free software; you can redistribute it and/or modify	*
 *	it under the terms of the GNU General Public License as published by	*
 *	the Free Software Foundation; either version 2 of the License, or		*
 *	(at your option) any later version.										*
 ****************************************************************************/


#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include <KXmlGuiWindow>

#include "ui_appearance.h"
#include "ui_general.h"
#include "ui_colors.h"
#include "ui_fonts.h"


class QScrollArea;

class KAction;
class KRecentFilesAction;
class KSelectAction;
class KToggleAction;

class Document;
class Editor;
class Palette;
class Preview;
class Scale;


class MainWindow : public KXmlGuiWindow
{
	Q_OBJECT

	public:
		MainWindow();
		MainWindow(const KUrl &);

	public slots:
		void loadSettings();
		void saveSettings();
		void statusMessage(const QString &);

	protected:
		virtual bool queryClose();
		virtual bool queryExit();
		virtual void readProperties(const KConfigGroup &);
		virtual void saveProperties(KConfigGroup &);

	private slots:
		void openNew();
		void open();
		void open(const KUrl &);
		void save();
		void saveAs();
		void revert();
		void print();
		void printPreview();

		void addBackgroundImage();
		void removeBackgroundImage();

		void fileProperties();
		void fileImport();

		void undo();
		void redo();
		void cut();
		void copy();
		void paste();
		void clear();
		void selectAll();
		void deselect();

		void fitBackgroundImage();
		void showBackgroundImage();
		void closeAllWindows();
		void paletteManager();
		void paletteShowSymbols();
		void paletteColorPicker();
		void paletteSave();
		void paletteLoad();
		void paletteCreate();
		void paletteClearUnused();
		void paletteSwapColors();
		void paletteReplaceColor();
		void paletteCalibrateScheme();
		void patternExtend();
		void patternCenter();
		void patternCropCanvasToSelection();
		void patternCropCanvasToPattern();
		void libraryManager();
		void showMenubar();
		void showStatusbar();
		void preferences();

	private:
		void setupActions();
		void setActionsFromDocument();
		void setupStatusBar();
		void setupDockWindows();

		void updateBackgroundImageActionLists();

		Document			*m_document;
		Editor				*m_editor;
		Palette				*m_palette;
		Preview				*m_preview;
		QScrollArea			*m_scrollArea;
		Scale				*m_horizontalScale;
		Scale				*m_verticalScale;

		KRecentFilesAction	*m_fileOpenRecent;
		KAction				*m_addBackgroundImage;
		QList<QAction *>	m_removeBackgroundImageActions;
		QList<QAction *>	m_showBackgroundImageActions;
		KAction				*m_fileProperties;
		KAction				*m_fileImport;

		KAction				*m_maskStitch;
		KAction				*m_maskColor;
		KAction				*m_maskBackstitch;
		KAction				*m_maskKnot;

		QActionGroup		*m_zoomGroup;
		QList<QAction *>	m_zoomActions;
		KAction				*m_copyMirrorRotate;
		KAction				*m_mirrorVertical;
		KAction				*m_mirrorHorizontal;
		KAction				*m_rotate90;
		KAction				*m_rotate180;
		KAction				*m_rotate270;

		QActionGroup		*m_toolsGroup;
		KAction				*m_toolPaint;
		KAction				*m_toolDraw;
		KAction				*m_toolErase;
		KAction				*m_toolRectangle;
		KAction				*m_toolFillRectangle;
		KAction				*m_toolEllipse;
		KAction				*m_toolFillEllipse;
		KAction				*m_toolFillPolyline;
		KAction				*m_toolText;
		KAction				*m_toolSelectRectangle;
		KAction				*m_toolBackstitch;

		KAction				*m_paletteManager;
		KAction				*m_paletteShowSymbols;
		KAction				*m_paletteColorPicker;
		KAction				*m_paletteSave;
		KAction				*m_paletteLoad;
		KAction				*m_paletteCreate;
		KAction				*m_paletteClearUnused;
		KAction				*m_paletteSwapColors;
		KAction				*m_paletteReplaceColor;
		KAction				*m_paletteCalibrateScheme;

		QActionGroup		*m_stitchGroup;
		KAction				*m_stitchQuarter;
		KAction				*m_stitchHalf;
		KAction				*m_stitch3Quarter;
		KAction				*m_stitchFull;
		KAction				*m_stitchSmallFull;
		KAction				*m_stitchSmallHalf;
		KAction				*m_stitchFrenchKnot;

		KAction				*m_patternExtend;
		KAction				*m_patternCenter;
		KAction				*m_patternCropCanvasToSelection;
		KAction				*m_patternCropCanvasToPattern;
		QList<QAction *>	m_fitBackgroundImageActions;

		KAction				*m_libraryManager;

		KAction				*m_showStitches;
		KAction				*m_showBackstitches;
		KAction				*m_showFrenchKnots;
		KAction				*m_showGrid;
		KAction				*m_showBackgroundImages;

		QActionGroup		*m_showStitchesAs;
		KAction				*m_showStitchesAsRegularStitches;
		KAction				*m_showStitchesAsBlackWhiteSymbols;
		KAction				*m_showStitchesAsColorSymbols;
		KAction				*m_showStitchesAsColorBlocks;
		KAction				*m_showStitchesAsColorBlocksSymbols;
		KAction				*m_showStitchesAsColorHilight;

		QActionGroup		*m_showBackstitchesAs;
		KAction				*m_showBackstitchesAsColorLines;
		KAction				*m_showBackstitchesAsBlackWhiteSymbols;
		KAction				*m_showBackstitchesAsColorHilight;

		QActionGroup		*m_showKnotsAs;
		KAction				*m_showKnotsAsColorBlocks;
		KAction				*m_showKnotsAsColorSymbols;
		KAction				*m_showKnotsAsBlackWhiteSymbols;
		KAction				*m_showKnotsAsColorHilight;

		QActionGroup		*m_formatScalesAs;
		KAction				*m_formatScalesAsStitches;
		KAction				*m_formatScalesAsCM;
		KAction				*m_formatScalesAsInches;

		QAction				*m_showPreviewDockWidget;
		QAction				*m_showPaletteDockWidget;

		Ui::General			ui_general;
		Ui::Appearance		ui_appearance;
		Ui::Colors			ui_colors;
		Ui::Fonts			ui_fonts;
};


#endif
