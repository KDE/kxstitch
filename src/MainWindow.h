/********************************************************************************
 *	Copyright (C) 2010 by Stephen Allewell					*
 *	stephen@mirramar.adsl24.co.uk						*
 *										*
 *	This program is free software; you can redistribute it and/or modify	*
 *	it under the terms of the GNU General Public License as published by	*
 *	the Free Software Foundation; either version 2 of the License, or	*
 *	(at your option) any later version.					*
 ********************************************************************************/


#ifndef MainWindow_H
#define MainWindow_H


#include <KXmlGuiWindow>

#include <Magick++.h>

class QString;

class KUrl;

class Document;
class Editor;
class LibraryManagerDlg;
class Palette;
class Preview;
class Scale;
class SchemeManager;


class MainWindow : public KXmlGuiWindow
{
	Q_OBJECT

	public:
		MainWindow();
		MainWindow(const KUrl &);
		MainWindow(const Magick::Image &);
		~MainWindow();

		Editor *editor();
		Preview *preview();
		Palette *palette();

		void updateBackgroundImageActionLists();

	protected:
		bool queryClose();
		bool queryExit();

	protected slots:
		// File menu
		void fileNew();
		void fileOpen();
		void fileOpen(const KUrl &);
		void fileSave();
		void fileSaveAs();
		void fileRevert();
		void filePrintSetup();
		void filePrint();
		void printPages(QPrinter *);
		void fileImportImage();
		void fileProperties();
		void fileAddBackgroundImage();
		void fileRemoveBackgroundImage();
		void fileClose();
		void fileQuit();

		// Edit menu
		void editUndo();
		void editRedo();
		void editCut();
		void editCopy();
		void editPaste();
		void undoTextChanged(const QString &);
		void redoTextChanged(const QString &);
		void clipboardDataChanged();

		// Tool menu
		void toolText();

		// Palette menu
		void paletteManager();
		void paletteShowSymbols(bool);
		void paletteClearUnused();
		void paletteCalibrateScheme();
		void paletteSwapColors(int, int);
		void paletteReplaceColor(int, int);

		// View menu
		void viewFitBackgroundImage();
		void viewShowBackgroundImage();

		// Library menu
		void libraryManager();

		// Pattern menu
		void patternExtend();

		// Layer action
		void selectLayer();

		// Settings menu
		void formatScalesAsStitches();
		void formatScalesAsCM();
		void formatScalesAsInches();

		void documentModified(bool);

	private:
		void setupMainWindow();
		void setupLayout();
		void setupDockWindows();
		void setupActions();
		void setupDocument();
		void setupActionDefaults();
		void setupActionsFromDocument();
		void updateLayerActionLists();
		void convertImage(const Magick::Image &);
		QPrinter *printer();

	protected:
		Document	*m_document;
		Editor		*m_editor;
		Palette		*m_palette;
		Preview		*m_preview;

		Scale		*m_horizontalScale;
		Scale		*m_verticalScale;

		LibraryManagerDlg	*m_libraryManagerDlg;

		QPrinter	*m_printer;
};


#endif // MainWindow_H
