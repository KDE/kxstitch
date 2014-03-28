/*
 * Copyright (C) 2010 by Stephen Allewell
 * sallewell@users.sourceforge.net
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#ifndef MainWindow_H
#define MainWindow_H


#include <KXmlGuiWindow>

#include <Magick++.h>


class QString;
class QUndoView;

class KUrl;

class Document;
class Editor;
class Palette;
class Preview;
class Scale;
class SchemeManager;


class MainWindow : public KXmlGuiWindow
{
    Q_OBJECT

public:
    MainWindow();
    explicit MainWindow(const KUrl &);
    explicit MainWindow(const Magick::Image &);
    ~MainWindow();

    Editor *editor();
    Preview *preview();
    Palette *palette();

    void loadSettings();

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
    void printPages();
    void fileImportImage();
    void fileProperties();
    void fileAddBackgroundImage();
    void fileRemoveBackgroundImage();
    void fileClose();
    void fileQuit();

    // Edit menu
    void editUndo();
    void editRedo();
    void undoTextChanged(const QString &);
    void redoTextChanged(const QString &);
    void clipboardDataChanged();

    // Tool menu

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

    // Pattern menu
    void patternExtend();
    void patternCentre();
    void patternCrop();
    void patternCropToSelection();
    void insertColumns();
    void insertRows();

    // Settings menu
    void preferences();
    void settingsChanged();

    void documentModified(bool);

private slots:
    void paletteContextMenu(const QPoint &);

private:
    void setupMainWindow();
    void setupLayout();
    void setupDockWindows();
    void setupActions();
    void setupDocument();
    void setupConnections();
    void setupActionDefaults();
    void setupActionsFromDocument();
    void convertImage(const Magick::Image &);
    QPrinter *printer();

    Document    *m_document;
    Editor      *m_editor;
    Palette     *m_palette;
    Preview     *m_preview;
    QUndoView   *m_history;

    Scale       *m_horizontalScale;
    Scale       *m_verticalScale;

    QPrinter    *m_printer;
};


#endif // MainWindow_H
