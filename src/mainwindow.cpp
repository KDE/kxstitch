/****************************************************************************
 *	Copyright (C) 2010 by Stephen Allewell									*
 *	stephen@mirramar.adsl24.co.uk											*
 *																			*
 *	This program is free software; you can redistribute it and/or modify	*
 *	it under the terms of the GNU General Public License as published by	*
 *	the Free Software Foundation; either version 2 of the License, or		*
 *	(at your option) any later version.										*
 ****************************************************************************/


/**
	@file
	Implement the MainWindow class to provide the applications top level window.  This is a
	subclass of the KXmlGuiWindow and creates an Editor with scroll bars as the central widget.
	It also provides the menu bar, tool bar and status bar along with dockable areas for the
	Palette and Preview windows.

	All the applications actions are created here and assigned to the menus and tool bars via
	the kxstitchui.rc file.
	*/

#include <QDockWidget>
#include <QGridLayout>

#include <KAction>
#include <KActionCollection>
#include <KApplication>
#include <KConfigDialog>
#include <KDebug>
#include <KFileDialog>
#include <KLocale>
#include <KMessageBox>
#include <KRecentFilesAction>
#include <KSelectAction>
#include <KStandardAction>
#include <KStatusBar>

#include "configuration.h"
#include "document.h"
#include "editor.h"
#include "mainwindow.h"
#include "palette.h"
#include "preview.h"
#include "scale.h"


const int ID_STATUS_MSG      = 1; /**< ID for the status message area of the status bar. The is used for general status messages. */
const int ID_LOCATION        = 2; /**< ID for the location area of the status bar. This is used for location and size information. */
const int ID_SCALE_FORMAT    = 3; /**< ID for the scale format flag area of the status bar. This is used for showing the current scale format. */
const int ID_MASKS_ENABLED   = 4; /**< ID for the masks enabled area of the status bar. This is used to show which masks are currently in use. */

/**
	@class MainWindow
	@brief Application main window class.

	The MainWindow class provides the main window for the application.  It
	defines the child windows for the Preview, Palette and Editor and creates
	all the actions for the menus, toolbars and context menus.  It also creates
	and initialises the status bar.

	@sa Editor, Preview, Palette
	*/


/**
	Instantiate an instance of a main window that has been restored from a session.
	@todo Investigate how the session management system works and what functions need to be
	implemented in KXStitch.
	*/
MainWindow::MainWindow() : KXmlGuiWindow()
{
	kDebug() << "Restored from session";
}


/**
	Instantiate an instance of a main window.

	Create the Editor and editor Scales and organise them into a layout.  Apply the layout
	to the MainWindow centralWidget.

	Setup actions, statusbar and dock windows.

	Create an instance of a Document using the url to read from and assign it to the various views.
	The url may be empty which indicates that a new document is to be created based on the default
	properties.  The display properties of the Editor, Preview and Palette are read from the Document
	when assigned to the view widgets.

	Update the background image action lists from the Document.

	Finally call the setupGUI to create the main window layout.

	@param url the url of the file to be opened which may be empty.
	*/
MainWindow::MainWindow(const KUrl &url) : KXmlGuiWindow()
{
	resize(Configuration::application_Width(), Configuration::application_Height());
	setObjectName("MainWindow#");

	m_editor = new Editor();
	m_horizontalScale = m_editor->horizontalScale();
	m_verticalScale = m_editor->verticalScale();

	m_scrollArea = new QScrollArea();
	m_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	m_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	m_scrollArea->setWidget(m_editor);

	/**
		The Editor embedded into a QScrollArea is layed out in a QGridLayout
		with the horizontal scale above and the vertical scale to the left.
		This layout is then applied to a widget which is set as the central
		widget of the MainWindow.
		@todo Check if this multi layers approach is the best way as there is an
		warning during compilation that a layout is being applied to a widget that
		already has one.
		*/
	QGridLayout *gridLayout = new QGridLayout(this);
	gridLayout->addWidget(m_horizontalScale, 0, 1);
	gridLayout->addWidget(m_verticalScale, 1, 0);
	gridLayout->addWidget(m_scrollArea, 1, 1);

	QWidget *layout = new QWidget();
	layout->setLayout(gridLayout);

	setCentralWidget(layout);

	setupActions();

	/**
		Add the actions required to implement a context menu to the scales.
		@todo Check if this is necessary as it may be simpler to implement a factory
		from the ui file.
		*/
	m_horizontalScale->addAction(m_formatScalesAsStitches);
	m_horizontalScale->addAction(m_formatScalesAsCM);
	m_horizontalScale->addAction(m_formatScalesAsInches);

	m_verticalScale->addAction(m_formatScalesAsStitches);
	m_verticalScale->addAction(m_formatScalesAsCM);
	m_verticalScale->addAction(m_formatScalesAsInches);

	setupStatusBar();

	setupDockWindows();

	/**
		Create the document using the url and assign it to the views.  The views will
		then read any properties required from the document.
		*/
	m_document = new Document();
	m_editor->setDocument(m_document);
	m_preview->setDocument(m_document);
	m_palette->setDocument(m_document);

	setAutoSaveSettings();

	setupGUI();
}


/**
	Setup the actions for the application and assign to the action collection.

	Actions are assigned to pointers and connected to slots either in the MainWindow
	or in the child windows.
	*/
void MainWindow::setupActions()
{
	KActionCollection *ac = actionCollection();
	// File menu actions
	KStandardAction::openNew(this, SLOT(openNew()), ac);
	KStandardAction::open(this, SLOT(open()), ac);
	KStandardAction::openRecent(this, SLOT(openRecent(const KUrl &)), ac);
	// ----------
	KStandardAction::save(this, SLOT(save()), ac);
	KStandardAction::saveAs(this, SLOT(saveAs()), ac);
	KStandardAction::revert(this, SLOT(revert()), ac);
	// ----------
	KStandardAction::print(this, SLOT(print()), ac);
	KStandardAction::printPreview(this, SLOT(printPreview()), ac);
	// ----------
	m_addBackgroundImage = new KAction(this);
	m_addBackgroundImage->setText(i18n("Add Background Image..."));
	connect(m_addBackgroundImage, SIGNAL(triggered()), this, SLOT(addBackgroundImage()));
	ac->addAction("addBackgroundImage", m_addBackgroundImage);

	m_fileProperties = new KAction(this);
	m_fileProperties->setText(i18n("File Properties..."));
	connect(m_fileProperties, SIGNAL(triggered()), this, SLOT(fileProperties()));
	ac->addAction("fileProperties", m_fileProperties);

	m_fileImport = new KAction(this);
	m_fileImport->setText(i18n("Import Image..."));
	connect(m_fileImport, SIGNAL(triggered()), this, SLOT(fileImport()));
	ac->addAction("fileImport", m_fileImport);
	// ----------
	KStandardAction::close(this, SLOT(close()), ac);
	// ----------
	KStandardAction::quit(this, SLOT(closeAllWindows()), ac);

	// Edit Menu
	KStandardAction::undo(this, SLOT(undo()), ac);
	KStandardAction::redo(this, SLOT(redo()), ac);
	// ----------
	KStandardAction::cut(this, SLOT(cut()), ac);
	KStandardAction::copy(this, SLOT(copy()), ac);
	KStandardAction::paste(this, SLOT(paste()), ac);
	KStandardAction::clear(this, SLOT(clear()), ac);
	// ----------
	KStandardAction::selectAll(this, SLOT(selectAll()), ac);
	KStandardAction::deselect(this, SLOT(deselect()), ac);
	// ----------
	// Selection mask sub menu
		m_maskStitch = new KAction(this);
		m_maskStitch->setText(i18n("Stitch Mask"));
		m_maskStitch->setCheckable(true);
		connect(m_maskStitch, SIGNAL(triggered()), m_editor, SLOT(setMaskStitch()));
		ac->addAction("maskStitch", m_maskStitch);

		m_maskColor = new KAction(this);
		m_maskColor->setText(i18n("Color Mask"));
		m_maskColor->setCheckable(true);
		connect(m_maskColor, SIGNAL(triggered()), m_editor, SLOT(setMaskColor()));
		ac->addAction("maskColor", m_maskColor);

		m_maskBackstitch = new KAction(this);
		m_maskBackstitch->setText(i18n("Exclude Backstitches"));
		m_maskBackstitch->setCheckable(true);
		connect(m_maskBackstitch, SIGNAL(triggered()), m_editor, SLOT(setMaskBackstitch()));
		ac->addAction("maskBackstitch", m_maskBackstitch);

		m_maskKnot = new KAction(this);
		m_maskKnot->setText(i18n("Exclude Knots"));
		m_maskKnot->setCheckable(true);
		connect(m_maskKnot, SIGNAL(triggered()), m_editor, SLOT(setMaskKnot()));
		ac->addAction("maskKnot", m_maskKnot);
	// ----------
	m_copyMirrorRotate = new KAction(this);
	m_copyMirrorRotate->setText(i18n("Mirror/Rotate Copies"));
	m_copyMirrorRotate->setCheckable(true);
	connect(m_copyMirrorRotate, SIGNAL(triggered()), m_editor, SLOT(setCopyMirrorRotate()));
	ac->addAction("copyMirrorRotate", m_copyMirrorRotate);
	// Mirror sub menu
		m_mirrorVertical = new KAction(this);
		m_mirrorVertical->setText(i18n("Vertically"));
		connect(m_mirrorVertical, SIGNAL(triggered()), m_editor, SLOT(mirrorVertical()));
		ac->addAction("mirrorVertical", m_mirrorVertical);

		m_mirrorHorizontal = new KAction(this);
		m_mirrorHorizontal->setText(i18n("Horizontally"));
		connect(m_mirrorVertical, SIGNAL(triggered()), m_editor, SLOT(mirrorHorizontal()));
		ac->addAction("mirrorHorizontal", m_mirrorHorizontal);
	// Rotate sub menu
		m_rotate90 = new KAction(this);
		m_rotate90->setText(i18n("Rotate 90"));
		connect(m_rotate90, SIGNAL(triggered()), m_editor, SLOT(rotate90()));
		ac->addAction("rotate90", m_rotate90);

		m_rotate180 = new KAction(this);
		m_rotate180->setText(i18n("Rotate 180"));
		connect(m_rotate180, SIGNAL(triggered()), m_editor, SLOT(rotate180()));
		ac->addAction("rotate180", m_rotate180);

		m_rotate270 = new KAction(this);
		m_rotate270->setText(i18n("Rotate 270"));
		connect(m_rotate270, SIGNAL(triggered()), m_editor, SLOT(rotate270()));
		ac->addAction("rotate270", m_rotate270);

	// View Menu
	KStandardAction::actualSize(m_editor, SLOT(actualSize()), ac);
	KStandardAction::fitToPage(m_editor, SLOT(fitToPage()), ac);
	KStandardAction::fitToWidth(m_editor, SLOT(fitToWidth()), ac);
	KStandardAction::fitToHeight(m_editor, SLOT(fitToHeight()), ac);
	// ----------
	m_zoomGroup = new QActionGroup(this);
	m_zoomGroup->setExclusive(true);

	KAction *action = new KAction(this);
	action->setText("25%");
	action->setData(0.25);
	action->setCheckable(true);
	connect(action, SIGNAL(triggered()), m_editor, SLOT(zoom()));
	m_zoomGroup->addAction(action);
	ac->addAction("zoom25", action);

	action = new KAction(this);
	action->setText("50%");
	action->setData(0.5);
	action->setCheckable(true);
	connect(action, SIGNAL(triggered()), m_editor, SLOT(zoom()));
	m_zoomGroup->addAction(action);
	ac->addAction("zoom50", action);

	action = new KAction(this);
	action->setText("100%");
	action->setData(1.0);
	action->setCheckable(true);
	connect(action, SIGNAL(triggered()), m_editor, SLOT(zoom()));
	m_zoomGroup->addAction(action);
	ac->addAction("zoom100", action);
	action->setChecked(true);

	action = new KAction(this);
	action->setText("200%");
	action->setData(2.0);
	action->setCheckable(true);
	connect(action, SIGNAL(triggered()), m_editor, SLOT(zoom()));
	m_zoomGroup->addAction(action);
	ac->addAction("zoom200", action);

	action = new KAction(this);
	action->setText("400%");
	action->setData(4.0);
	action->setCheckable(true);
	connect(action, SIGNAL(triggered()), m_editor, SLOT(zoom()));
	m_zoomGroup->addAction(action);
	ac->addAction("zoom400", action);

	// Stitches Menu
	m_stitchGroup = new QActionGroup(this);
	m_stitchGroup->setExclusive(true);

	m_stitchQuarter = new KAction(this);
	m_stitchQuarter->setText(i18n("Quarter Stitch"));
	m_stitchQuarter->setIcon(KIcon("quarter"));
	m_stitchQuarter->setCheckable(true);
	connect(m_stitchQuarter, SIGNAL(triggered()), m_editor, SLOT(selectStitchQuarter()));
	ac->addAction("stitchQuarter", m_stitchQuarter);
	m_stitchGroup->addAction(m_stitchQuarter);

	m_stitchHalf = new KAction(this);
	m_stitchHalf->setText(i18n("Half Stitch"));
	m_stitchHalf->setIcon(KIcon("half"));
	m_stitchHalf->setCheckable(true);
	connect(m_stitchHalf, SIGNAL(triggered()), m_editor, SLOT(selectStitchHalf()));
	ac->addAction("stitchHalf", m_stitchHalf);
	m_stitchGroup->addAction(m_stitchHalf);

	m_stitch3Quarter = new KAction(this);
	m_stitch3Quarter->setText(i18n("3 Quarter Stitch"));
	m_stitch3Quarter->setIcon(KIcon("3quarter"));
	m_stitch3Quarter->setCheckable(true);
	connect(m_stitch3Quarter, SIGNAL(triggered()), m_editor, SLOT(selectStitch3Quarter()));
	ac->addAction("stitch3Quarter", m_stitch3Quarter);
	m_stitchGroup->addAction(m_stitch3Quarter);

	m_stitchFull = new KAction(this);
	m_stitchFull->setText(i18n("Full Stitch"));
	m_stitchFull->setIcon(KIcon("full"));
	m_stitchFull->setCheckable(true);
	connect(m_stitchFull, SIGNAL(triggered()), m_editor, SLOT(selectStitchFull()));
	ac->addAction("stitchFull", m_stitchFull);
	m_stitchGroup->addAction(m_stitchFull);

	m_stitchSmallHalf = new KAction(this);
	m_stitchSmallHalf->setText(i18n("Small Half Stitch"));
	m_stitchSmallHalf->setIcon(KIcon("smallhalf"));
	m_stitchSmallHalf->setCheckable(true);
	connect(m_stitchSmallHalf, SIGNAL(triggered()), m_editor, SLOT(selectStitchSmallHalf()));
	ac->addAction("stitchSmallHalf", m_stitchSmallHalf);
	m_stitchGroup->addAction(m_stitchSmallHalf);

	m_stitchSmallFull = new KAction(this);
	m_stitchSmallFull->setText(i18n("Small Full Stitch"));
	m_stitchSmallFull->setIcon(KIcon("smallfull"));
	m_stitchSmallFull->setCheckable(true);
	connect(m_stitchSmallFull, SIGNAL(triggered()), m_editor, SLOT(selectStitchSmallFull()));
	ac->addAction("stitchSmallFull", m_stitchSmallFull);
	m_stitchGroup->addAction(m_stitchSmallFull);

	m_stitchFrenchKnot = new KAction(this);
	m_stitchFrenchKnot->setText(i18n("French Knot"));
	m_stitchFrenchKnot->setIcon(KIcon("frenchknot"));
	m_stitchFrenchKnot->setCheckable(true);
	connect(m_stitchFrenchKnot, SIGNAL(triggered()), m_editor, SLOT(selectStitchFrenchKnot()));
	ac->addAction("stitchFrenchKnot", m_stitchFrenchKnot);
	m_stitchGroup->addAction(m_stitchFrenchKnot);

	m_stitchFull->setChecked(true);
	m_editor->selectStitchFull();

	// Tools Menu
	m_toolsGroup = new QActionGroup(this);
	m_toolsGroup->setExclusive(true);

	m_toolPaint = new KAction(this);
	m_toolPaint->setText(i18n("Paint"));
	m_toolPaint->setIcon(KIcon("draw-brush"));
	m_toolPaint->setCheckable(true);
	m_toolPaint->setChecked(true);
	connect(m_toolPaint, SIGNAL(triggered()), m_editor, SLOT(selectPaintTool()));
	ac->addAction("toolPaint", m_toolPaint);
	m_toolsGroup->addAction(m_toolPaint);

	m_toolDraw = new KAction(this);
	m_toolDraw->setText(i18n("Draw"));
	m_toolDraw->setIcon(KIcon("draw-freehand"));
	m_toolDraw->setCheckable(true);
	connect(m_toolDraw, SIGNAL(triggered()), m_editor, SLOT(selectDrawTool()));
	ac->addAction("toolDraw", m_toolDraw);
	m_toolsGroup->addAction(m_toolDraw);

	m_toolErase = new KAction(this);
	m_toolErase->setText(i18n("Erase"));
	m_toolErase->setIcon(KIcon("draw-eraser"));
	m_toolErase->setCheckable(true);
	connect(m_toolErase, SIGNAL(triggered()), m_editor, SLOT(selectEraseTool()));
	ac->addAction("toolErase", m_toolErase);
	m_toolsGroup->addAction(m_toolErase);

	m_toolRectangle = new KAction(this);
	m_toolRectangle->setText(i18n("Draw Rectangle"));
	m_toolRectangle->setIcon(KIcon("o_rect"));
	m_toolRectangle->setCheckable(true);
	connect(m_toolRectangle, SIGNAL(triggered()), m_editor, SLOT(selectRectangleTool()));
	ac->addAction("toolRectangle", m_toolRectangle);
	m_toolsGroup->addAction(m_toolRectangle);

	m_toolFillRectangle = new KAction(this);
	m_toolFillRectangle->setText(i18n("Fill Rectangle"));
	m_toolFillRectangle->setIcon(KIcon("f_rect"));
	m_toolFillRectangle->setCheckable(true);
	connect(m_toolFillRectangle, SIGNAL(triggered()), m_editor, SLOT(selectFillRectangleTool()));
	ac->addAction("toolFillRectangle", m_toolFillRectangle);
	m_toolsGroup->addAction(m_toolFillRectangle);

	m_toolEllipse = new KAction(this);
	m_toolEllipse->setText(i18n("Draw Ellipse"));
	m_toolEllipse->setIcon(KIcon("o_ellipse"));
	m_toolEllipse->setCheckable(true);
	connect(m_toolEllipse, SIGNAL(triggered()), m_editor, SLOT(selectEllipseTool()));
	ac->addAction("toolEllipse", m_toolEllipse);
	m_toolsGroup->addAction(m_toolEllipse);

	m_toolFillEllipse = new KAction(this);
	m_toolFillEllipse->setText(i18n("Fill Ellipse"));
	m_toolFillEllipse->setIcon(KIcon("f_ellipse"));
	m_toolFillEllipse->setCheckable(true);
	connect(m_toolFillEllipse, SIGNAL(triggered()), m_editor, SLOT(selectFillEllipseTool()));
	ac->addAction("toolFillEllipse", m_toolFillEllipse);
	m_toolsGroup->addAction(m_toolFillEllipse);

	m_toolFillPolyline = new KAction(this);
	m_toolFillPolyline->setText(i18n("Fill Polyline"));
	m_toolFillPolyline->setIcon(KIcon("polyline"));
	kDebug() << KIcon("polyline").themeSearchPaths();
	m_toolFillPolyline->setCheckable(true);
	connect(m_toolFillPolyline, SIGNAL(triggered()), m_editor, SLOT(selectFillPolylineTool()));
	ac->addAction("toolFillPolyline", m_toolFillPolyline);
	m_toolsGroup->addAction(m_toolFillPolyline);

	m_toolText = new KAction(this);
	m_toolText->setText(i18n("Text"));
	m_toolText->setIcon(KIcon("insert-text"));
	m_toolText->setCheckable(true);
	connect(m_toolText, SIGNAL(triggered()), m_editor, SLOT(selectTextTool()));
	ac->addAction("toolText", m_toolText);
	m_toolsGroup->addAction(m_toolText);

	m_toolSelectRectangle = new KAction(this);
	m_toolSelectRectangle->setText(i18n("Select"));
	m_toolSelectRectangle->setIcon(KIcon("s_rect"));
	m_toolSelectRectangle->setCheckable(true);
	connect(m_toolSelectRectangle, SIGNAL(triggered()), m_editor, SLOT(selectSelectTool()));
	ac->addAction("toolSelectRectangle", m_toolSelectRectangle);
	m_toolsGroup->addAction(m_toolSelectRectangle);

	m_toolBackstitch = new KAction(this);
	m_toolBackstitch->setText(i18n("Backstitch"));
	m_toolBackstitch->setIcon(KIcon("backstitch"));
	kDebug() << KIconLoader::global()->iconPath("backstitch", KIconLoader::Toolbar);
	m_toolBackstitch->setCheckable(true);
	connect(m_toolBackstitch, SIGNAL(triggered()), m_editor, SLOT(selectBackstitchTool()));
	ac->addAction("toolBackstitch", m_toolBackstitch);
	m_toolsGroup->addAction(m_toolBackstitch);

	// Palette Menu
	m_paletteManager = new KAction(this);
	m_paletteManager->setText(i18n("Palette Manager..."));
	m_paletteManager->setIcon(KIcon("palette-manager"));
	connect(m_paletteManager, SIGNAL(triggered()), this, SLOT(paletteManager()));
	ac->addAction("paletteManager", m_paletteManager);

	m_paletteShowSymbols = new KAction(this);
	m_paletteShowSymbols->setText(i18n("Show Symbols"));
	m_paletteShowSymbols->setCheckable(true);
	connect(m_paletteShowSymbols, SIGNAL(triggered()), this, SLOT(paletteShowSymbols()));
	ac->addAction("paletteShowSymbols", m_paletteShowSymbols);

	m_paletteColorPicker = new KAction(this);
	m_paletteColorPicker->setText(i18n("Color Picker"));
	m_paletteColorPicker->setIcon(KIcon("color-picker"));
	connect(m_paletteColorPicker, SIGNAL(triggered()), this, SLOT(paletteColorPicker()));
	ac->addAction("paletteColorPicker", m_paletteColorPicker);

	m_paletteSave = new KAction(this);
	m_paletteSave->setText(i18n("Save Palette"));
	connect(m_paletteSave, SIGNAL(triggered()), this, SLOT(paletteSave()));
	ac->addAction("paletteSave", m_paletteSave);

	m_paletteLoad = new KAction(this);
	m_paletteLoad->setText(i18n("Load Palette"));
	connect(m_paletteLoad, SIGNAL(triggered()), this, SLOT(paletteLoad()));
	ac->addAction("paletteLoad", m_paletteLoad);

	m_paletteCreate = new KAction(this);
	m_paletteCreate->setText(i18n("Create Palette"));
	connect(m_paletteCreate, SIGNAL(triggered()), this, SLOT(paletteCreate()));
	ac->addAction("paletteCreate", m_paletteCreate);

	m_paletteClearUnused = new KAction(this);
	m_paletteClearUnused->setText(i18n("Clear Unused"));
	connect(m_paletteClearUnused, SIGNAL(triggered()), this, SLOT(paletteClearUnused()));
	ac->addAction("paletteClearUnused", m_paletteClearUnused);

	m_paletteSwapColors = new KAction(this);
	m_paletteSwapColors->setText(i18n("Swap Colors"));
	connect(m_paletteSwapColors, SIGNAL(triggered()), this, SLOT(paletteSwapColors()));
	ac->addAction("paletteSwapColors", m_paletteSwapColors);

	m_paletteReplaceColor = new KAction(this);
	m_paletteReplaceColor->setText(i18n("Replace Color"));
	connect(m_paletteReplaceColor, SIGNAL(triggered()), this, SLOT(paletteReplaceColor()));
	ac->addAction("paletteReplaceColor", m_paletteReplaceColor);

	m_paletteCalibrateScheme = new KAction(this);
	m_paletteCalibrateScheme->setText(i18n("Calibrate Scheme..."));
	connect(m_paletteCalibrateScheme, SIGNAL(triggered()), this, SLOT(paletteCalibrateScheme()));
	ac->addAction("paletteCalibrateScheme", m_paletteCalibrateScheme);

	// Pattern Menu
	m_patternExtend = new KAction(this);
	m_patternExtend->setText(i18n("Extend Pattern..."));
	connect(m_patternExtend, SIGNAL(triggered()), this, SLOT(patternExtend()));
	ac->addAction("patternExtend", m_patternExtend);

	m_patternCenter = new KAction(this);
	m_patternCenter->setText(i18n("Center Pattern"));
	connect(m_patternCenter, SIGNAL(triggered()), this, SLOT(patternCenter()));
	ac->addAction("patternCenter", m_patternCenter);
	// ----------
	m_patternCropCanvasToSelection = new KAction(this);
	m_patternCropCanvasToSelection->setText(i18n("Crop Canvas to Selection"));
	connect(m_patternCropCanvasToSelection, SIGNAL(triggered()), this, SLOT(patternCropCanvasToSelection()));
	ac->addAction("patternCropCanvasToSelection", m_patternCropCanvasToSelection);

	m_patternCropCanvasToPattern = new KAction(this);
	m_patternCropCanvasToPattern->setText(i18n("Crop Canvas to Pattern"));
	connect(m_patternCropCanvasToPattern, SIGNAL(triggered()), this, SLOT(patternCropCanvasToPattern()));
	ac->addAction("patternCropCanvasToPattern", m_patternCropCanvasToPattern);
	// ----------
	// Fit background to selection menu

	// Library Menu
	m_libraryManager = new KAction(this);
	m_libraryManager->setText(i18n("Library Manager..."));
	connect(m_libraryManager, SIGNAL(triggered()), this, SLOT(libraryManager()));
	ac->addAction("libraryManager", m_libraryManager);

	// Settings Menu
	// FormatScalesAs
	m_formatScalesAs = new QActionGroup(this);
	m_formatScalesAs->setExclusive(true);

	m_formatScalesAsStitches = new KAction(this);
	m_formatScalesAsStitches->setText(i18n("Stitches"));
	m_formatScalesAsStitches->setCheckable(true);
	m_formatScalesAsStitches->setChecked(true);
	connect(m_formatScalesAsStitches, SIGNAL(triggered()), m_editor, SLOT(formatScalesAsStitches()));
	ac->addAction("formatScalesAsStitches", m_formatScalesAsStitches);
	m_formatScalesAs->addAction(m_formatScalesAsStitches);

	m_formatScalesAsCM = new KAction(this);
	m_formatScalesAsCM->setText(i18n("CM"));
	m_formatScalesAsCM->setCheckable(true);
	connect(m_formatScalesAsCM, SIGNAL(triggered()), m_editor, SLOT(formatScalesAsCM()));
	ac->addAction("formatScalesAsCM", m_formatScalesAsCM);
	m_formatScalesAs->addAction(m_formatScalesAsCM);

	m_formatScalesAsInches = new KAction(this);
	m_formatScalesAsInches->setText(i18n("Inches"));
	m_formatScalesAsInches->setCheckable(true);
	connect(m_formatScalesAsInches, SIGNAL(triggered()), m_editor, SLOT(formatScalesAsInches()));
	ac->addAction("formatScalesAsInches", m_formatScalesAsInches);
	m_formatScalesAs->addAction(m_formatScalesAsInches);

	// ShowStitchesAs
	m_showStitchesAs = new QActionGroup(this);
	m_showStitchesAs->setExclusive(true);

	m_showStitchesAsRegularStitches = new KAction(this);
	m_showStitchesAsRegularStitches->setText(i18n("Regular Stitches"));
	m_showStitchesAsRegularStitches->setCheckable(true);
	m_showStitchesAsRegularStitches->setChecked(true);
	connect(m_showStitchesAsRegularStitches, SIGNAL(triggered()), m_editor, SLOT(showStitchesAsRegularStitches()));
	ac->addAction("showStitchesAsRegularStitches", m_showStitchesAsRegularStitches);
	m_showStitchesAs->addAction(m_showStitchesAsRegularStitches);

	m_showStitchesAsBlackWhiteSymbols = new KAction(this);
	m_showStitchesAsBlackWhiteSymbols->setText(i18n("Black & White Symbols"));
	m_showStitchesAsBlackWhiteSymbols->setCheckable(true);
	connect(m_showStitchesAsBlackWhiteSymbols, SIGNAL(triggered()), m_editor, SLOT(showStitchesAsBlackWhiteSymbols()));
	ac->addAction("showStitchesAsBlackWhiteSymbols", m_showStitchesAsBlackWhiteSymbols);
	m_showStitchesAs->addAction(m_showStitchesAsBlackWhiteSymbols);

	m_showStitchesAsColorSymbols = new KAction(this);
	m_showStitchesAsColorSymbols->setText(i18n("Color Symbols"));
	m_showStitchesAsColorSymbols->setCheckable(true);
	connect(m_showStitchesAsColorSymbols, SIGNAL(triggered()), m_editor, SLOT(showStitchesAsColorSymbols()));
	ac->addAction("showStitchesAsColorSymbols", m_showStitchesAsColorSymbols);
	m_showStitchesAs->addAction(m_showStitchesAsColorSymbols);

	m_showStitchesAsColorBlocks = new KAction(this);
	m_showStitchesAsColorBlocks->setText(i18n("Color Blocks"));
	m_showStitchesAsColorBlocks->setCheckable(true);
	connect(m_showStitchesAsColorBlocks, SIGNAL(triggered()), m_editor, SLOT(showStitchesAsColorBlocks()));
	ac->addAction("showStitchesAsColorBlocks", m_showStitchesAsColorBlocks);
	m_showStitchesAs->addAction(m_showStitchesAsColorBlocks);

	m_showStitchesAsColorBlocksSymbols = new KAction(this);
	m_showStitchesAsColorBlocksSymbols->setText(i18n("Color Blocks & Symbols"));
	m_showStitchesAsColorBlocksSymbols->setCheckable(true);
	connect(m_showStitchesAsColorBlocksSymbols, SIGNAL(triggered()), m_editor, SLOT(showStitchesAsColorBlocksSymbols()));
	ac->addAction("showStitchesAsColorBlocksSymbols", m_showStitchesAsColorBlocksSymbols);
	m_showStitchesAs->addAction(m_showStitchesAsColorBlocksSymbols);

	m_showStitchesAsColorHilight = new KAction(this);
	m_showStitchesAsColorHilight->setText(i18n("Color Hilight"));
	m_showStitchesAsColorHilight->setCheckable(true);
	connect(m_showStitchesAsColorHilight, SIGNAL(triggered()), m_editor, SLOT(showStitchesAsColorHilight()));
	ac->addAction("showStitchesAsColorHilight", m_showStitchesAsColorHilight);
	m_showStitchesAs->addAction(m_showStitchesAsColorHilight);

	// ShowBackstitchesAs
	m_showBackstitchesAs = new QActionGroup(this);
	m_showBackstitchesAs->setExclusive(true);

	m_showBackstitchesAsColorLines = new KAction(this);
	m_showBackstitchesAsColorLines->setText(i18n("Color Lines"));
	m_showBackstitchesAsColorLines->setCheckable(true);
	m_showBackstitchesAsColorLines->setChecked(true);
	connect(m_showBackstitchesAsColorLines, SIGNAL(triggered()), m_editor, SLOT(showBackstitchesAsColorLines()));
	ac->addAction("showBackstitchesAsColorLines", m_showBackstitchesAsColorLines);
	m_showBackstitchesAs->addAction(m_showBackstitchesAsColorLines);

	m_showBackstitchesAsBlackWhiteSymbols = new KAction(this);
	m_showBackstitchesAsBlackWhiteSymbols->setText(i18n("Black & White Symbols"));
	m_showBackstitchesAsBlackWhiteSymbols->setCheckable(true);
	connect(m_showBackstitchesAsBlackWhiteSymbols, SIGNAL(triggered()), m_editor, SLOT(showBackstitchesAsBlackWhiteSymbols()));
	ac->addAction("showBackstitchesAsBlackWhiteSymbols", m_showBackstitchesAsBlackWhiteSymbols);
	m_showBackstitchesAs->addAction(m_showBackstitchesAsBlackWhiteSymbols);

	m_showStitches = new KAction(this);
	m_showStitches->setText(i18n("Show Stitches"));
	m_showStitches->setCheckable(true);
	connect(m_showStitches, SIGNAL(triggered()), m_editor, SLOT(setShowStitches()));
	ac->addAction("showStitches", m_showStitches);

	m_showBackstitches = new KAction(this);
	m_showBackstitches->setText(i18n("Show Backstitches"));
	m_showBackstitches->setCheckable(true);
	connect(m_showBackstitches, SIGNAL(triggered()), m_editor, SLOT(setShowBackstitches()));
	ac->addAction("showBackstitches", m_showBackstitches);

	m_showGrid = new KAction(this);
	m_showGrid->setText(i18n("Show Grid"));
	m_showGrid->setCheckable(true);
	connect(m_showGrid, SIGNAL(triggered()), m_editor, SLOT(setShowGrid()));
	ac->addAction("showGrid", m_showGrid);

	m_showFrenchKnots = new KAction(this);
	m_showFrenchKnots->setText(i18n("Show French Knots"));
	m_showFrenchKnots->setCheckable(true);
	connect(m_showFrenchKnots, SIGNAL(triggered()), m_editor, SLOT(setShowFrenchKnots()));
	ac->addAction("showFrenchKnots", m_showFrenchKnots);

	KStandardAction::showMenubar(this, SLOT(showMenubar()), ac);
	KStandardAction::showStatusbar(this, SLOT(showStatusbar()), ac);
	KStandardAction::preferences(this, SLOT(preferences()), ac);

	// Help Menu
}


/**
	The status bar consists of a message area and some fixed width status flags.
	The style sheet used to display the status bar is updated to show a border around
	the flags to show separation.
	*/
void MainWindow::setupStatusBar()
{
	statusBar()->insertItem("", ID_STATUS_MSG, 1);
	statusBar()->insertFixedItem("", ID_LOCATION);
	statusBar()->insertFixedItem("", ID_SCALE_FORMAT);
	statusBar()->insertFixedItem("", ID_MASKS_ENABLED);

	statusBar()->setItemAlignment(ID_STATUS_MSG, Qt::AlignLeft);

	statusBar()->setItemFixed(ID_LOCATION, 80);
	statusBar()->setItemFixed(ID_SCALE_FORMAT, 30);
	statusBar()->setItemFixed(ID_MASKS_ENABLED, 80);

	statusBar()->changeItem("ST", ID_SCALE_FORMAT);
	statusBar()->changeItem("SCBKL", ID_MASKS_ENABLED);

	statusBar()->setStyleSheet("::item {border: 1px solid grey; border-radius: 3px; margin-right: 2px;}");
}


/**
	Create the dockable windows for the Preview and Palette windows.

	These are initially added to a QDockWidget on the left hand side but can be moved to any location
	along any side of the application window.  The dockable windows can be tiled or tabbed.
	*/
void MainWindow::setupDockWindows()
{
	QDockWidget *dock = new QDockWidget(i18n("Preview"), this);
	dock->setAllowedAreas(Qt::AllDockWidgetAreas);
	m_preview = new Preview();
	dock->setWidget(m_preview);
	addDockWidget(Qt::LeftDockWidgetArea, dock);
	actionCollection()->addAction("showPreviewDockWidget", dock->toggleViewAction());

	dock = new QDockWidget(i18n("Palette"), this);
	dock->setAllowedAreas(Qt::AllDockWidgetAreas);
	m_palette = new Palette();
	dock->setWidget(m_palette);
	addDockWidget(Qt::LeftDockWidgetArea, dock);
	actionCollection()->addAction("showPaletteDockWidget", dock->toggleViewAction());
}


/**
	Load the MainWindow settings.
	*/
void MainWindow::loadSettings()
{
}


/**
	Save the MainWindow settings.
	*/
void MainWindow::saveSettings()
{
}


/**
	Read the MainWindow properties.
	*/
void MainWindow::readProperties(const KConfigGroup &configGroup)
{
}


/**
	Save the MainWindow properties.
	*/
void MainWindow::saveProperties(KConfigGroup &configGroup)
{
}


/**
	Query if it is ok to close the current MainWindow.

	This should check to see if the document needs to be saved and if so ask the user to save it.
	The user may cancel the operation which will also cancel closing the MainWindow
	@return true if ok to close, false otherwise
	*/
bool MainWindow::queryClose()
{
	if (m_document->isModified())
	{
		switch (KMessageBox::warningYesNoCancel(this, i18n("Save changes to document?")))
		{
			case KMessageBox::Yes :
				// save the document, if failed, return false;
				return false;
			case KMessageBox::No :
				return true;
			case KMessageBox::Cancel :
				return false;
		}
	}
	return true;
}


/**
	Query if ok the exit the application.
	@return true if ok false otherwise.
	*/
bool MainWindow::queryExit()
{
	return true;
}


/**
	Updates the actions associated with the background images.

	User needs to be able to remove background image, turn them on or off and
	be able to position them to a selected area.

	The actions are added to action lists which are plugged into the place holders
	created in the ui description.
	*/
void MainWindow::updateBackgroundImageActionLists()
{
	QList<struct Document::BACKGROUND_IMAGE> backgroundImages = m_document->backgroundImages();

	unplugActionList("removeBackgroundImageActions");
	unplugActionList("patternFitBackgroundImageActions");
	unplugActionList("showBackgroundImageActions");

	m_removeBackgroundImageActions.clear();
	m_fitBackgroundImageActions.clear();
	m_showBackgroundImageActions.clear();

	for (int i = 0 ; i < backgroundImages.count() ; i++)
	{
		KAction *action = new KAction(backgroundImages.at(i).imageURL.fileName(), this);
		action->setData(backgroundImages.at(i).imageURL.pathOrUrl());
		action->setIcon(backgroundImages.at(i).imageIcon);
		connect(action, SIGNAL(triggered()), this, SLOT(removeBackgroundImage()));
		m_removeBackgroundImageActions.append(action);

		action = new KAction(backgroundImages.at(i).imageURL.fileName(), this);
		action->setData(backgroundImages.at(i).imageURL.pathOrUrl());
		action->setIcon(backgroundImages.at(i).imageIcon);
		connect(action, SIGNAL(triggered()), this, SLOT(fitBackgroundImage()));
		m_fitBackgroundImageActions.append(action);

		action = new KAction(backgroundImages.at(i).imageURL.fileName(), this);
		action->setData(backgroundImages.at(i).imageURL.pathOrUrl());
		action->setIcon(backgroundImages.at(i).imageIcon);
		action->setCheckable(true);
		action->setChecked(backgroundImages.at(i).imageVisible);
		connect(action, SIGNAL(triggered()), this, SLOT(showBackgroundImage()));
		m_showBackgroundImageActions.append(action);
	}

	plugActionList("removeBackgroundImageActions", m_removeBackgroundImageActions);
	plugActionList("patternFitBackgroundImageActions", m_fitBackgroundImageActions);
	plugActionList("showBackgroundImageActions", m_showBackgroundImageActions);
}


/**
	Remove the selected background image.

	All remove background image actions are connected to this slot.
	The specific action is found using the sender function which can then
	be used to determine the path of the image to be removed.

	The updateBackgroundImageActionLists function is called to update the list
	once the selected image is removed from the document.
	The Editor needs to be updated to remove the image.
	*/
void MainWindow::removeBackgroundImage()
{
	KAction *action = qobject_cast<KAction *>(sender());
	m_document->removeBackgroundImage(action->data().toString());
	updateBackgroundImageActionLists();
	m_editor->update();
}


/**
	Fit the selected background image to the selected area.

	All fit background image actions are connected to this slot.
	The specific action is found using the sender function which can then
	be used to determine the path of the image to be fitted.  The selected
	area is retrieved from the Editor class.

	The Editor needs to be updated to display the new location of the image.
	*/
void MainWindow::fitBackgroundImage()
{
	KAction *action = qobject_cast<KAction *>(sender());
	m_document->fitBackgroundImage(action->data().toString(), m_editor->selectionArea());
	m_editor->update();
}


/**
	Show the selected background image.

	All show background image actions are connected to this slot.
	The specific action is found using the sender function which can then
	be used to determine the path of the image to be shown.  The checked state
	of the action determines whether the image is shown or hidden.

	The Editor needs to be updated to show or hide the image.
	*/
void MainWindow::showBackgroundImage()
{
	KAction *action = qobject_cast<KAction *>(sender());
	m_document->showBackgroundImage(action->data().toString(), action->isChecked());
	m_editor->update();
}


/**
	Create a new instance of the MainWindow.

	Creates a new MainWindow with a blank document.  Resizes it to the default size
	and shows it on the desktop.
	*/
void MainWindow::openNew()
{
	MainWindow *window = new MainWindow();
	window->resize(Configuration::application_Width(), Configuration::application_Height());
	window->show();
}


/**
	Open an existing document.

	Query for a url to be opened and if valid the current Document is deleted and a new Document
	using the url as the source is created.  The url is added to the recent documents list.
	*/
void MainWindow::open()
{
	statusMessage(i18n("Opening file"));
	KUrl url = KFileDialog::getOpenUrl(KUrl("kfiledialog:///"), QString("*.kxs|KXStitch patterns\n*.pat|PC Stitch patterns\n*|All files"), this);
	if (!url.isEmpty())
	{
		delete m_document;
		m_document = new Document(url);
		m_editor->setDocument(m_document);
		m_preview->setDocument(m_document);
		m_palette->setDocument(m_document);
		setCaption(url.fileName(), false);
		m_fileOpenRecent->addUrl(url);
	}
	statusMessage(i18n("Ready"));
}


/**
	Open an existing document.

	Open a document where the url is already known, for example if supplied on the command line.
	@param url reference to the KUrl.
	*/
void MainWindow::open(const KUrl &url)
{
}


/**
	Open a recent document.

	Open a document on the recent document list.
	@param url reference to the KUrl.
	*/
void MainWindow::openRecent(const KUrl &url)
{
	open(url);
}


/**
	Save the current document.

	Check if the document has a url, if not call the saveAs function,
	*/
void MainWindow::save()
{
}


/**
	Save the document with a new name.

	Query for a url to save the document as.
	*/
void MainWindow::saveAs()
{
}


/**
	Revert the document to the last saved state.
	*/
void MainWindow::revert()
{
}


/**
	Print the document.

	Display the print dialog configured with the options specified in the document.
	On acceptance print the document using the options from the print dialog.
	*/
void MainWindow::print()
{
}


/**
	Display a preview of the document for printing.
	*/
void MainWindow::printPreview()
{
}


/**
	Add a background image to the document.

	Query for an image url and if valid read the current selection area.
	Add the image url to the document using the selection area then call the
	updateBackgroundImageActionLists and update the display.
	*/
void MainWindow::addBackgroundImage()
{
	KUrl url = KFileDialog::getImageOpenUrl(KUrl(), this, i18n("Background Image"));
	if (!url.path().isNull())
	{
		QRect r;
		QRect s = m_editor->selectionArea();
		m_document->addBackgroundImage(url, (s.isValid()?s:r));
		updateBackgroundImageActionLists();
		m_editor->update();
	}
}


/**
	Show the file properties dialog.

	Show the dialog initialised from the document properties.  Allow changes to be made which if accepted
	are reflected in the Document.
	*/
void MainWindow::fileProperties()
{
}


/**
	Import an image.

	Query for an image url and if valid import the image converting it to stitches based on the new document
	default properties.
	*/
void MainWindow::fileImport()
{
}


/**
	Close all the application windows.
	*/
void MainWindow::closeAllWindows()
{
	close();
}


/**
	Undo the last action.
	*/
void MainWindow::undo()
{
}


/**
	Redo the last undone action.
	*/
void MainWindow::redo()
{
}


/**
	Cut the current selection.
	*/
void MainWindow::cut()
{
}


/**
	Copy the current selection.
	*/
void MainWindow::copy()
{
}


/**
	Paste the contents of the clipboard.

	Only valid if the clipboard contents provide a application/kxstitch mime source.
	*/
void MainWindow::paste()
{
}


/**
	Clear the selected area.
	*/
void MainWindow::clear()
{
}


/**
	Select everything
	*/
void MainWindow::selectAll()
{
}


/**
	Deselect the selected area.
	*/
void MainWindow::deselect()
{
}


/**
	Show the palette manager dialog.

	Allow changes to the current palette.  Allows assignment of symbols and strands.
	This dialog also allows calibration of the colors and creation of new colors.

	If changes have been made, the views need to be updated to show the updated information.
	*/
void MainWindow::paletteManager()
{
	if (m_document->paletteManager())
	{
		m_editor->repaint();
		m_palette->repaint();
		m_preview->repaint();
	}
}


/**
	Show the palette symbols.

	The checked status of the action determines the enabled state of the toggle.

	The palette needs to be updated to show or hide the symbols.
	*/
void MainWindow::paletteShowSymbols()
{
	m_document->setProperty("showPaletteSymbols", m_paletteShowSymbols->isChecked());
	m_palette->update();
}


/**
	Allow selection of the current color by picking from the Editor.
	*/
void MainWindow::paletteColorPicker()
{
}


/**
	Save the current palette.
	*/
void MainWindow::paletteSave()
{
}


/**
	Load a palette.
	*/
void MainWindow::paletteLoad()
{
}


/**
	Create a new palette.
	*/
void MainWindow::paletteCreate()
{
}


/**
	Clear the unused colors from the palette.

	The palette needs to be updated to show only the used colors.
	*/
void MainWindow::paletteClearUnused()
{
}


/**
	Allow swapping of the currently selected with another.

	Swaps the next selected color with the currently selected color.
	*/
void MainWindow::paletteSwapColors()
{
}


/**
	Replace the current color.
	*/
void MainWindow::paletteReplaceColor()
{
}


/**
	Calibrate a scheme.
	*/
void MainWindow::paletteCalibrateScheme()
{
}


/**
	Extend the pattern.

	Show the extend pattern dialog.  If accepted, extend the pattern by the amounts given
	in the dialog.
	*/
void MainWindow::patternExtend()
{
}


/**
	Center the pattern.

	Center the pattern within the canvas.
	*/
void MainWindow::patternCenter()
{
}


/**
	Crop the canvas to the selection area.

	Crop the canvas to the current selection area.  Any stitches outside this area will
	be deleted.
	*/
void MainWindow::patternCropCanvasToSelection()
{
}


/**
	Crop the canvas to the pattern.

	Crop the canvas to the extents of the pattern.
	*/
void MainWindow::patternCropCanvasToPattern()
{
}


/**
	Show the library manager.

	The library manager is a non modal dialog for using libraries of patterns.
	*/
void MainWindow::libraryManager()
{
}


/**
	Show the menu bar.

	The checked status of the action determines the visibility of the menu bar.
	*/
void MainWindow::showMenubar()
{
}


/**
	Show the status bar.

	The checked status of the action determines the visibility of the status bar.
	*/
void MainWindow::showStatusbar()
{
}


/**
	Show the preferences dialog.

	Allows configuration of the document and application.
	*/
void MainWindow::preferences()
{
	if (KConfigDialog::showDialog("settings"))
		return;

	KConfigDialog *dialog = new KConfigDialog(this, "settings", Configuration::self());
	dialog->setFaceType(KPageDialog::List);
	QWidget *general = new QWidget;
	QWidget *appearance = new QWidget;
	QWidget *colors = new QWidget;
	QWidget *fonts = new QWidget;

	ui_general.setupUi(general);
	ui_appearance.setupUi(appearance);
	ui_colors.setupUi(colors);
	ui_fonts.setupUi(fonts);

	dialog->addPage(general, i18n("General"), "preferences-desktop");
	dialog->addPage(appearance, i18n("Appearance"), "ksnapshot");
	dialog->addPage(colors, i18n("Colors"), "preferences-desktop-color");
	dialog->addPage(fonts, i18n("Fonts"), "preferences-desktop-font");

	connect(dialog, SIGNAL(settingsChanged(const QString&)), this, SLOT(loadSettings()));
	connect(dialog, SIGNAL(settingsChanged(const QString&)), m_preview, SLOT(loadSettings()));
	connect(dialog, SIGNAL(settingsChanged(const QString&)), m_palette, SLOT(loadSettings()));
	connect(dialog, SIGNAL(settingsChanged(const QString&)), m_editor, SLOT(loadSettings()));

	dialog->show();
}


/**
	Display a status message.

	Display a text string in the status message area of the status bar.
	@param text reference to a QString text message
	*/
void MainWindow::statusMessage(const QString &text)
{
	statusBar()->changeItem(text, ID_STATUS_MSG);
}
