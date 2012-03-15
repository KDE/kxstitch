/********************************************************************************
 *	Copyright (C) 2010 by Stephen Allewell					*
 *	stephen@mirramar.adsl24.co.uk						*
 *										*
 *	This program is free software; you can redistribute it and/or modify	*
 *	it under the terms of the GNU General Public License as published by	*
 *	the Free Software Foundation; either version 2 of the License, or	*
 *	(at your option) any later version.					*
 ********************************************************************************/


#include "MainWindow.h"

#include <QActionGroup>
#include <QClipboard>
#include <QDataStream>
#include <QDockWidget>
#include <QGridLayout>
#include <QPainter>
#include <QProgressDialog>
#include <QPrintDialog>
#include <QPrinter>
#include <QPrintPreviewDialog>
#include <QScrollArea>
#include <QTemporaryFile>
#include <QUndoView>

#include <KAction>
#include <KActionCollection>
#include <KFileDialog>
#include <KGlobalSettings>
#include <KIO/NetAccess>
#include <KLocale>
#include <KMessageBox>
#include <KRecentFilesAction>
#include <KSelectAction>
#include <KUrl>

#include "BackgroundImage.h"
#include "configuration.h"
#include "Commands.h"
#include "Document.h"
#include "Editor.h"
#include "ExtendPatternDlg.h"
#include "FilePropertiesDlg.h"
#include "Floss.h"
#include "FlossScheme.h"
#include "ImportImageDlg.h"
#include "LibraryManagerDlg.h"
#include "Palette.h"
#include "PaletteManagerDlg.h"
#include "PatternMimeData.h"
#include "Preview.h"
#include "PrintSetupDlg.h"
#include "QVariantPtr.h"
#include "Scale.h"
#include "SchemeManager.h"


MainWindow::MainWindow()
	:	m_libraryManagerDlg(0),
		m_printer(new QPrinter(QPrinter::HighResolution))
{
	setupActions();
}


MainWindow::MainWindow(const KUrl &url)
	:	m_libraryManagerDlg(0),
		m_printer(new QPrinter(QPrinter::HighResolution))
{
	setupMainWindow();
	setupLayout();
	setupDockWindows();
	setupActions();
	setupDocument();
	fileOpen(url);
	setupActionDefaults();
	setupActionsFromDocument();
	setCaption(m_document->url().fileName(), !m_document->undoStack().isClean());
}


MainWindow::MainWindow(const Magick::Image &image)
	:	m_libraryManagerDlg(0),
		m_printer(new QPrinter(QPrinter::HighResolution))
{
	setupMainWindow();
	setupLayout();
	setupDockWindows();
	setupActions();
	setupDocument();
	convertImage(image);
	setupActionDefaults();
	setupActionsFromDocument();
	setCaption(m_document->url().fileName(), !m_document->undoStack().isClean());
}


void MainWindow::setupMainWindow()
{
	setObjectName("MainWindow#");
	setAutoSaveSettings();
}


void MainWindow::setupLayout()
{
	QScrollArea *scrollArea = new QScrollArea();
	scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	m_editor = new Editor(scrollArea);
	scrollArea->setWidget(m_editor);

	m_horizontalScale = m_editor->horizontalScale();
	m_verticalScale = m_editor->verticalScale();

	QGridLayout *gridLayout = new QGridLayout(this);
	gridLayout->addWidget(m_horizontalScale, 0, 1);
	gridLayout->addWidget(m_verticalScale, 1, 0);
	gridLayout->addWidget(scrollArea, 1, 1);

	QWidget *layout = new QWidget();
	layout->setLayout(gridLayout);

	setCentralWidget(layout);
}


void MainWindow::setupDocument()
{
	KActionCollection *actions = actionCollection();

	m_document = new Document();

	connect(&(m_document->undoStack()), SIGNAL(canUndoChanged(bool)), actions->action("edit_undo"), SLOT(setEnabled(bool)));
	connect(&(m_document->undoStack()), SIGNAL(canUndoChanged(bool)), actions->action("file_revert"), SLOT(setEnabled(bool)));
	connect(&(m_document->undoStack()), SIGNAL(canRedoChanged(bool)), actions->action("edit_redo"), SLOT(setEnabled(bool)));
	connect(QApplication::clipboard(), SIGNAL(dataChanged()), this, SLOT(clipboardDataChanged()));
	connect(m_editor, SIGNAL(selectionMade(bool)), actionCollection()->action("edit_cut"), SLOT(setEnabled(bool)));
	connect(m_editor, SIGNAL(selectionMade(bool)), actionCollection()->action("edit_copy"), SLOT(setEnabled(bool)));
	connect(m_editor, SIGNAL(selectionMade(bool)), actionCollection()->action("mirrorHorizontal"), SLOT(setEnabled(bool)));
	connect(m_editor, SIGNAL(selectionMade(bool)), actionCollection()->action("mirrorVertical"), SLOT(setEnabled(bool)));
	connect(m_editor, SIGNAL(selectionMade(bool)), actionCollection()->action("patternCropToSelection"), SLOT(setEnabled(bool)));
	connect(&(m_document->undoStack()), SIGNAL(undoTextChanged(const QString &)), this, SLOT(undoTextChanged(const QString &)));
	connect(&(m_document->undoStack()), SIGNAL(redoTextChanged(const QString &)), this, SLOT(redoTextChanged(const QString &)));
	connect(&(m_document->undoStack()), SIGNAL(cleanChanged(bool)), this, SLOT(documentModified(bool)));
	connect(m_palette, SIGNAL(colorSelected(int)), m_editor, SLOT(update()));
	connect(m_palette, SIGNAL(swapColors(int, int)), this, SLOT(paletteSwapColors(int, int)));
	connect(m_palette, SIGNAL(replaceColor(int, int)), this, SLOT(paletteReplaceColor(int, int)));
	connect(m_editor, SIGNAL(changedVisibleCells(const QRect &)), m_preview, SLOT(setVisibleCells(const QRect &)));
	connect(m_preview, SIGNAL(clicked(const QPoint &)), m_editor, SLOT(previewClicked(const QPoint &)));
	connect(m_preview, SIGNAL(clicked(const QRect &)), m_editor, SLOT(previewClicked(const QRect &)));

	m_editor->setDocument(m_document);
	m_editor->setPreview(m_preview);
	m_palette->setDocument(m_document);
	m_preview->setDocument(m_document);
	m_history->setStack(&(m_document->undoStack()));
	
	m_document->addView(m_editor);
	m_document->addView(m_preview);
	m_document->addView(m_palette);
}


void MainWindow::setupActionDefaults()
{
	KActionCollection *actions = actionCollection();

	actions->action("toolPaint")->trigger();	// Select paint tool
	actions->action("stitchFull")->trigger();	// Select full stitch

	clipboardDataChanged();
}


MainWindow::~MainWindow()
{
	delete m_libraryManagerDlg;
	delete m_printer;
}


Editor *MainWindow::editor()
{
	return m_editor;
}


Preview *MainWindow::preview()
{
	return m_preview;
}


Palette *MainWindow::palette()
{
	return m_palette;
}


bool MainWindow::queryClose()
{
	if (m_document->undoStack().isClean())
		return true;
	while (true)
	{
		int messageBoxResult = KMessageBox::warningYesNoCancel(this, i18n("Save changes to document?\nSelecting No discards changes."));
		switch (messageBoxResult)
		{
			case KMessageBox::Yes :
				fileSave();
				if (m_document->undoStack().isClean())
					return true;
				else
					KMessageBox::error(this, i18n("Unable to save the file"));
				break;

			case KMessageBox::No :
				return true;

			case KMessageBox::Cancel :
				return false;
		}
	}
}


bool MainWindow::queryExit()
{
	return true;
}


void MainWindow::setupActionsFromDocument()
{
	KActionCollection *actions = actionCollection();

	actions->action("file_revert")->setEnabled(!m_document->undoStack().isClean());
	actions->action("edit_undo")->setEnabled(m_document->undoStack().canUndo());
	actions->action("edit_redo")->setEnabled(m_document->undoStack().canRedo());

	updateBackgroundImageActionLists();

	switch (static_cast<Configuration::EnumEditor_FormatScalesAs::type>(m_document->property("formatScalesAs").toInt()))
	{
		case Configuration::EnumEditor_FormatScalesAs::Stitches:
			actions->action("formatScalesAsStitches")->trigger();
			break;

		case Configuration::EnumEditor_FormatScalesAs::Inches:
			actions->action("formatScalesAsInches")->trigger();
			break;

		case Configuration::EnumEditor_FormatScalesAs::CM:
			actions->action("formatScalesAsCM")->trigger();
			break;
	}

	switch (static_cast<Configuration::EnumRenderer_RenderStitchesAs::type>(m_document->property("renderStitchesAs").toInt()))
	{
		case Configuration::EnumRenderer_RenderStitchesAs::Stitches:
			actions->action("renderStitchesAsRegularStitches")->trigger();
			break;

		case Configuration::EnumRenderer_RenderStitchesAs::BlackWhiteSymbols:
			actions->action("renderStitchesAsBlackWhiteSymbols")->trigger();
			break;

		case Configuration::EnumRenderer_RenderStitchesAs::ColorSymbols:
			actions->action("renderStitchesAsColorSymbols")->trigger();
			break;

		case Configuration::EnumRenderer_RenderStitchesAs::ColorBlocks:
			actions->action("renderStitchesAsColorBlocks")->trigger();
			break;

		case Configuration::EnumRenderer_RenderStitchesAs::ColorBlocksSymbols:
			actions->action("renderStitchesAsColorBlocksSymbols")->trigger();
			break;
	}

	switch (static_cast<Configuration::EnumRenderer_RenderBackstitchesAs::type>(m_document->property("renderBackstitchesAs").toInt()))
	{
		case Configuration::EnumRenderer_RenderBackstitchesAs::ColorLines:
			actions->action("renderBackstitchesAsColorLines")->trigger();
			break;

		case Configuration::EnumRenderer_RenderBackstitchesAs::BlackWhiteSymbols:
			actions->action("renderBackstitchesAsBlackWhiteSymbols")->trigger();
			break;
	}

	switch (static_cast<Configuration::EnumRenderer_RenderKnotsAs::type>(m_document->property("renderKnotsAs").toInt()))
	{
		case Configuration::EnumRenderer_RenderKnotsAs::ColorBlocks:
			actions->action("renderKnotsAsColorBlocks")->trigger();
			break;

		case Configuration::EnumRenderer_RenderKnotsAs::ColorSymbols:
			actions->action("renderKnotsAsColorSymbols")->trigger();
			break;

		case Configuration::EnumRenderer_RenderKnotsAs::BlackWhiteSymbols:
			actions->action("renderKnotsAsBlackWhiteSymbols")->trigger();
			break;
	}

	actions->action("colorHilight")->setChecked(m_document->property("colorHilight").toBool());
	m_editor->colorHilight(m_document->property("colorHilight").toBool());

	m_editor->setMaskStitch(m_document->property("maskStitch").toBool());
	actions->action("maskStitch")->setChecked(m_document->property("maskStitch").toBool());
	m_editor->setMaskColor(m_document->property("maskColor").toBool());
	actions->action("maskColor")->setChecked(m_document->property("maskColor").toBool());
	m_editor->setMaskBackstitch(m_document->property("maskBackstitch").toBool());
	actions->action("maskBackstitch")->setChecked(m_document->property("maskBackstitch").toBool());
	m_editor->setMaskKnot(m_document->property("maskKnot").toBool());
	actions->action("maskKnot")->setChecked(m_document->property("maskKnot").toBool());

	actions->action("renderStitches")->setChecked(m_document->property("renderStitches").toBool());
	actions->action("renderBackstitches")->setChecked(m_document->property("renderBackstitches").toBool());
	actions->action("renderFrenchKnots")->setChecked(m_document->property("renderFrenchKnots").toBool());
	actions->action("renderGrid")->setChecked(m_document->property("renderGrid").toBool());
	actions->action("renderBackgroundImages")->setChecked(m_document->property("renderBackgroundImages").toBool());
}


void MainWindow::fileNew()
{
	MainWindow *window = new MainWindow(KUrl());
	window->show();
}


void MainWindow::fileOpen()
{
	fileOpen(KFileDialog::getOpenUrl(KUrl("kfiledialog:///"), i18n("*.kxs|Cross Stitch Patterns\n*.pat|PC Stitch patterns\n*|All files"), this));
}


void MainWindow::fileOpen(const KUrl &url)
{
	MainWindow *window;
	bool docEmpty = ((m_document->undoStack().isClean()) && (m_document->url() == i18n("Untitled")));
	if (url.isValid())
	{
		if (docEmpty)
		{
			if (m_document->load(url))
			{
				setupActionsFromDocument();
				m_editor->readDocumentSettings();
				m_palette->readDocumentSettings();
				m_preview->readDocumentSettings();
				documentModified(true); // this is the clean value true
				KRecentFilesAction *action = static_cast<KRecentFilesAction *>(actionCollection()->action("file_open_recent"));
				action->addUrl(url);
				action->saveEntries(KConfigGroup(KGlobal::config(), "RecentFiles"));
			}
		}
		else
		{
			window = new MainWindow(url);
			window->show();
		}
	}
}


void MainWindow::fileSave()
{
	if (m_document->url() == i18n("Untitled"))
		fileSaveAs();
	else
		m_document->save();
}


void MainWindow::fileSaveAs()
{
	KUrl url = KFileDialog::getSaveUrl(QString("::%1").arg(KGlobalSettings::documentPath()), i18n("*.kxs|Cross Stitch Patterns"), this, i18n("Save As..."));
	if (url.isValid())
	{
		if (KIO::NetAccess::exists(url, false, 0))
		{
			if (KMessageBox::warningYesNo(this, i18n("This file already exists\nDo you want to overwrite it?")) == KMessageBox::No)
			{
				return;
			}
		}
		m_document->setUrl(url);
		m_document->save();
		KRecentFilesAction *action = static_cast<KRecentFilesAction *>(actionCollection()->action("file_open_recent"));
		action->addUrl(url);
		action->saveEntries(KConfigGroup(KGlobal::config(), "RecentFiles"));
	}
}


void MainWindow::fileRevert()
{
	if (!m_document->undoStack().isClean())
	{
		if (KMessageBox::warningYesNo(this, i18n("Revert changes to document?")) == KMessageBox::Yes)
			m_document->undoStack().setIndex(m_document->undoStack().cleanIndex());
	}
}


void MainWindow::filePrintSetup()
{
	QPointer<PrintSetupDlg> printSetupDlg = new PrintSetupDlg(this, m_document, m_printer);
	if (printSetupDlg->exec() == QDialog::Accepted)
	{
		m_document->undoStack().push(new UpdatePrinterConfigurationCommand(m_document, printSetupDlg->printerConfiguration()));
	}
	delete printSetupDlg;
}


void MainWindow::filePrint()
{
	m_printer->setFullPage(true);
	m_printer->setResolution(300);
	m_printer->setPrintRange(QPrinter::AllPages);
	m_printer->setFromTo(1, m_document->printerConfiguration().pages().count());

	QListIterator<Page *> pageIterator(m_document->printerConfiguration().pages());
	if (pageIterator.hasNext())
	{
		Page *page = pageIterator.next();
		m_printer->setPaperSize(page->paperSize());
		m_printer->setOrientation(page->orientation());

		QPointer<QPrintDialog> printDialog = new QPrintDialog(m_printer, this);
		if (printDialog->exec() == QDialog::Accepted)
		{
			printPages(m_printer);
		}
		delete printDialog;
	}
	else
	{
		KMessageBox::information(this, i18n("There is nothing to print"));
	}
}


void MainWindow::printPages(QPrinter *printer)
{
	QList<Page *> pages = m_document->printerConfiguration().pages();
	int totalPages = pages.count();

	QPainter painter;
	painter.begin(printer);

	Page *page = pages.at(0);
	for (int p = 0 ; p < totalPages ;)
	{
		// for the first page, the orientation and size will have been set
		page->render(m_document, &painter);
		if (++p < totalPages)
		{
			page = pages.at(p);
			printer->setPaperSize(page->paperSize());
			printer->setOrientation(page->orientation());
			printer->newPage();
		}

	}

	painter.end();

#if 0
#if QT_VERSION >= 0x040700
	int copies = m_printer->copyCount();
	bool supportsMultipleCopies = m_printer->supportsMultipleCopies();
#else
	int copies = m_printer->numCopies();
	bool supportsMultipleCopies = false;
#endif
	bool collateCopies = m_printer->collateCopies();

	int fromPage = m_printer->fromPage();
	int toPage = m_printer->toPage();
	QString outputFileName = m_printer->outputFileName();
	QPrinter::OutputFormat outputFormat = m_printer->outputFormat();
	QPrinter::PageOrder pageOrder = m_printer->pageOrder();
	QRect paperRect = m_printer->paperRect();
	QPrinter::PaperSize paperSize = m_printer->paperSize();
	QPrinter::PrintRange printRange = m_printer->printRange();
	if (printRange == QPrinter::AllPages)
	{
		fromPage = 1;
		toPage = m_document->printerConfiguration().pages().count();
	}
	int resolution = m_printer->resolution();

	QPainter painter;
	painter.begin(printer);

	if (collateCopies)
	{
		for (int c = 0 ; c < copies ; ++c)
		{
			if (pageOrder == QPrinter::FirstPageFirst)
			{
				for (int p = fromPage ; p <= toPage ; ++p)
				{
					Page *page = pages.at(p-1);
					printer->setPaperSize(page->paperSize());
					printer->setOrientation(page->orientation());
					page->render(m_document, &painter);
					if (p != toPage)
						m_printer->newPage();
				}
			}
			else
			{
				for (int p = toPage ; p >= fromPage ; --p)
				{
					Page *page = pages.at(p-1);
					printer->setPaperSize(page->paperSize());
					printer->setOrientation(page->orientation());
					page->render(m_document, &painter);
					if (p != fromPage)
						m_printer->newPage();
				}
			}
		}
	}
	else
	{
		if (pageOrder == QPrinter::FirstPageFirst)
		{
			for (int p = fromPage ; p <= toPage ; )
			{
				Page *page = pages.at(p-1);
				printer->setPaperSize(page->paperSize());
				printer->setOrientation(page->orientation());
				for (int c = 0 ; c < copies ; )
				{
					page->render(m_document, &painter);
					if (++c < copies)
						m_printer->newPage();
				}
				if (++p <= toPage)
					m_printer->newPage();
			}
		}
		else
		{
			for (int p = toPage ; p >= fromPage ; )
			{
				Page *page = pages.at(p-1);
				printer->setPaperSize(page->paperSize());
				printer->setOrientation(page->orientation());
				for (int c = 0 ; c < copies ; )
				{
					page->render(m_document, &painter);
					if (++c < copies)
						m_printer->newPage();
				}
				if (--p >= fromPage)
					m_printer->newPage();
			}
		}
	}

	painter.end();
#endif
}


void MainWindow::fileImportImage()
{
	MainWindow *window;
	bool docEmpty = ((m_document->undoStack().isClean()) && (m_document->url() == i18n("Untitled")));
	KUrl url = KFileDialog::getImageOpenUrl(KUrl(), this, i18n("Import Image"));
	if (!url.path().isNull())
	{
		if (docEmpty)
			convertImage(Magick::Image(url.pathOrUrl().toStdString()));
		else
		{
			window = new MainWindow(Magick::Image(url.pathOrUrl().toStdString()));
			window->show();
		}
	}
}


void MainWindow::convertImage(const Magick::Image &image)
{
	QMap<int, QColor> documentFlosses;
	QMap<int, QChar> flossSymbols;

	QPointer<ImportImageDlg> importImageDlg = new ImportImageDlg(this, image);
	if (importImageDlg->exec())
	{
		Magick::Image convertedImage = importImageDlg->convertedImage();

		int imageWidth = convertedImage.columns();
		int imageHeight = convertedImage.rows();
		int documentWidth = imageWidth;
		int documentHeight = imageHeight;

		bool useFractionals = importImageDlg->useFractionals();

		bool ignoreColor = importImageDlg->ignoreColor();
		Magick::Color ignoreColorValue = importImageDlg->ignoreColorValue();

		int pixelCount = imageWidth*imageHeight;

		if (useFractionals)
		{
			documentWidth /= 2;
			documentHeight /= 2;
		}
		m_document->pattern()->stitches().resize(documentWidth, documentHeight);

		QString schemeName = importImageDlg->flossScheme();
		FlossScheme *flossScheme = SchemeManager::scheme(schemeName);
		m_document->pattern()->palette().setSchemeName(schemeName);

		QProgressDialog progress(i18n("Converting to stitches"), i18n("Cancel"), 0, pixelCount, this);
		progress.setWindowModality(Qt::WindowModal);
		Magick::Pixels cache(convertedImage);
		const Magick::PixelPacket *pixels = cache.getConst(0, 0, imageWidth, imageHeight);

		for (int dy = 0 ; dy < imageHeight ; dy++)
		{
			progress.setValue(dy*imageWidth);
			QApplication::processEvents();
			if (progress.wasCanceled())
				break;
			for (int dx = 0 ; dx < imageWidth ; dx++)
			{
				Magick::PixelPacket packet = *pixels++;
				if (!(packet.opacity))
				{
					if (!(ignoreColor && packet == ignoreColorValue))
					{
						int flossIndex;
						QColor color(packet.red/257, packet.green/257, packet.blue/257);
						for (flossIndex = 0 ; flossIndex < documentFlosses.count() ; flossIndex++)
						{
							if (documentFlosses[flossIndex] == color)
								break;
						}
						if (flossIndex == documentFlosses.count()) // reached the end of the list
						{
							int c = -1;
							bool found = false;

							while (!found)
							{
								QChar symbol(++c);
								if (symbol.isPrint() && !symbol.isSpace() && !symbol.isPunct())
								{
									found = true;
									QMapIterator<int, QChar> i(flossSymbols);
									while (i.hasNext() && found)
									{
										i.next();
										QChar existingSymbol = i.value();
										if (symbol == existingSymbol)
											found = false;
									}
								}
							}

							QChar stitchSymbol(c);
							Qt::PenStyle backstitchSymbol(Qt::SolidLine);

							DocumentFloss *documentFloss = new DocumentFloss(flossScheme->find(color), stitchSymbol, backstitchSymbol, Configuration::palette_StitchStrands(), Configuration::palette_BackstitchStrands());
							documentFloss->setFlossColor(color);
							m_document->pattern()->palette().add(flossIndex, documentFloss);
							documentFlosses.insert(flossIndex, color);
							flossSymbols.insert(flossIndex, stitchSymbol);
						}
						// at this point
						//   flossIndex will be the index for the found colorHilight
						if (useFractionals)
						{
							if (dx%2)
							{
								if (dy%2)
									m_document->pattern()->stitches().addStitch(QPoint(dx/2, dy/2), Stitch::BRQtr, flossIndex);
								else
									m_document->pattern()->stitches().addStitch(QPoint(dx/2, dy/2), Stitch::TRQtr, flossIndex);
							}
							else
							{
								if (dx%2)
									m_document->pattern()->stitches().addStitch(QPoint(dx/2, dy/2), Stitch::BLQtr, flossIndex);
								else
									m_document->pattern()->stitches().addStitch(QPoint(dx/2, dy/2), Stitch::TLQtr, flossIndex);
							}
						}
						else
						{
							m_document->pattern()->stitches().addStitch(QPoint(dx, dy), Stitch::Full, flossIndex);
						}
					}
				}
			}
		}
		m_document->setProperty("horizontalClothCount", importImageDlg->horizontalClothCount());
		m_document->setProperty("verticalClothCount", importImageDlg->verticalClothCount());
		m_editor->readDocumentSettings();
		m_palette->readDocumentSettings();
		m_preview->readDocumentSettings();
		m_editor->update();
		m_palette->update();
		m_preview->update();
	}
	delete importImageDlg;
}


void MainWindow::fileProperties()
{
	QPointer<FilePropertiesDlg> filePropertiesDlg = new FilePropertiesDlg(this, m_document);
	if (filePropertiesDlg->exec())
	{
		QUndoCommand *cmd = new FilePropertiesCommand(m_document);
		if ((filePropertiesDlg->documentWidth() != m_document->pattern()->stitches().width()) || (filePropertiesDlg->documentHeight() != m_document->pattern()->stitches().height()))
			new ResizeDocumentCommand(m_document, filePropertiesDlg->documentWidth(), filePropertiesDlg->documentHeight(), cmd);
		if (filePropertiesDlg->unitsFormat() != static_cast<Configuration::EnumDocument_UnitsFormat::type>(m_document->property("unitsFormat").toInt()))
			new SetPropertyCommand(m_document, "unitsFormat", QVariant(filePropertiesDlg->unitsFormat()), cmd);
		if (filePropertiesDlg->horizontalClothCount() != m_document->property("horizontalClothCount").toDouble())
			new SetPropertyCommand(m_document, "horizontalClothCount", QVariant(filePropertiesDlg->horizontalClothCount()), cmd);
		if (filePropertiesDlg->clothCountLink() != m_document->property("clothCountLink").toBool())
			new SetPropertyCommand(m_document, "clothCountLink", QVariant(filePropertiesDlg->clothCountLink()), cmd);
		if (filePropertiesDlg->verticalClothCount() != m_document->property("verticalClothCount").toDouble())
			new SetPropertyCommand(m_document, "verticalClothCount", QVariant(filePropertiesDlg->verticalClothCount()), cmd);
		if (filePropertiesDlg->clothCountUnits() != static_cast<Configuration::EnumEditor_ClothCountUnits::type>(m_document->property("clothCountUnits").toInt()))
			new SetPropertyCommand(m_document, "clothCountUnits", QVariant(filePropertiesDlg->clothCountUnits()), cmd);
		if (filePropertiesDlg->title() != m_document->property("title").toString())
			new SetPropertyCommand(m_document, "title", QVariant(filePropertiesDlg->title()), cmd);
		if (filePropertiesDlg->author() != m_document->property("author").toString())
			new SetPropertyCommand(m_document, "author", QVariant(filePropertiesDlg->author()), cmd);
		if (filePropertiesDlg->copyright() != m_document->property("copyright").toString())
			new SetPropertyCommand(m_document, "copyright", QVariant(filePropertiesDlg->copyright()), cmd);
		if (filePropertiesDlg->fabric() != m_document->property("fabric").toString())
			new SetPropertyCommand(m_document, "fabric", QVariant(filePropertiesDlg->fabric()), cmd);
		if (filePropertiesDlg->fabricColor() != m_document->property("fabricColor").value<QColor>())
			new SetPropertyCommand(m_document, "fabricColor", QVariant(filePropertiesDlg->fabricColor()), cmd);
		if (filePropertiesDlg->instructions() != m_document->property("instructions").toString())
			new SetPropertyCommand(m_document, "instructions", QVariant(filePropertiesDlg->instructions()), cmd);
		if (filePropertiesDlg->flossScheme() != m_document->pattern()->palette().schemeName())
			new ChangeSchemeCommand(m_document, filePropertiesDlg->flossScheme(), cmd);

		if (cmd->childCount())
			m_document->undoStack().push(cmd);
		else
			delete cmd;
	}
	delete filePropertiesDlg;
}


void MainWindow::fileAddBackgroundImage()
{
	KUrl url = KFileDialog::getImageOpenUrl(KUrl(), this, i18n("Background Image"));
	if (!url.path().isNull())
	{
		QRect patternArea(0, 0, m_document->pattern()->stitches().width(), m_document->pattern()->stitches().height());
		QRect selectionArea = m_editor->selectionArea();
		BackgroundImage *backgroundImage = new BackgroundImage(url, (selectionArea.isValid()?selectionArea:patternArea));
		if (backgroundImage->isValid())
		{
			m_document->undoStack().push(new AddBackgroundImageCommand(m_document, backgroundImage, this));
		}
		else
			delete backgroundImage;
	}
}


void MainWindow::fileRemoveBackgroundImage()
{
	KAction *action = qobject_cast<KAction *>(sender());
	m_document->undoStack().push(new RemoveBackgroundImageCommand(m_document, QVariantPtr<BackgroundImage>::asPtr(action->data()), this));
}


void MainWindow::fileClose()
{
	if (queryClose())
	{
		m_document->initialiseNew();
		setupActionsFromDocument();
		m_editor->readDocumentSettings();
		m_palette->readDocumentSettings();
		m_preview->readDocumentSettings();
	}
	close();
}


void MainWindow::fileQuit()
{
	close();
}


void MainWindow::editUndo()
{
	m_document->undoStack().undo();
}


void MainWindow::editRedo()
{
	m_document->undoStack().redo();
}


void MainWindow::undoTextChanged(const QString &text)
{
	actionCollection()->action("edit_undo")->setText(i18n("Undo %1", text));
}


void MainWindow::redoTextChanged(const QString &text)
{
	actionCollection()->action("edit_redo")->setText(i18n("Redo %1", text));
}


void MainWindow::clipboardDataChanged()
{
	actionCollection()->action("edit_paste")->setEnabled(QApplication::clipboard()->mimeData()->hasFormat("application/kxstitch"));
}


void MainWindow::paletteManager()
{
	QPointer<PaletteManagerDlg> paletteManagerDlg = new PaletteManagerDlg(this, m_document);
	if (paletteManagerDlg->exec())
	{
		QList<QUndoCommand *> changes = paletteManagerDlg->changes();
		if (!changes.isEmpty())
		{
			m_document->undoStack().beginMacro(i18n("Update Palette"));
			m_document->undoStack().push(new UpdatePaletteCommand(m_palette));
			m_document->undoStack().push(new UpdatePreviewCommand(m_preview));
			m_document->undoStack().push(new UpdateEditorCommand(m_editor));
			QListIterator<QUndoCommand *> it(changes);
			while (it.hasNext())
			{
				m_document->undoStack().push(it.next());
			}
			m_document->undoStack().endMacro();
		}

		m_palette->update();
		m_preview->update();
		m_editor->update();
	}
	delete paletteManagerDlg;
}


void MainWindow::paletteShowSymbols(bool show)
{
	m_palette->showSymbols(show);
	m_document->pattern()->palette().setShowSymbols(show);
}


void MainWindow::paletteClearUnused()
{
	QList<QUndoCommand *> changes;

	QMap<int, FlossUsage> flossUsage = m_document->pattern()->stitches().flossUsage();
	QMapIterator<int, DocumentFloss *> flosses(m_document->pattern()->palette().flosses());
	while (flosses.hasNext())
	{
		flosses.next();
		if (flossUsage[flosses.key()].totalStitches() == 0)
			changes.append(new RemoveDocumentFlossCommand(m_document, flosses.key(), flosses.value()));
	}

	if (!changes.isEmpty())
	{
		m_document->undoStack().beginMacro(i18n("Clear Unused Flosses"));
		QListIterator<QUndoCommand *> it(changes);
		while (it.hasNext())
		{
			m_document->undoStack().push(it.next());
		}
		m_document->undoStack().endMacro();
	}
}


void MainWindow::paletteCalibrateScheme()
{
}


void MainWindow::paletteSwapColors(int originalIndex, int replacementIndex)
{
	if (originalIndex != replacementIndex)
	{
		m_document->undoStack().push(new PaletteSwapColorCommand(m_document, originalIndex, replacementIndex));
	}
}


void MainWindow::paletteReplaceColor(int originalIndex, int replacementIndex)
{
	if (originalIndex != replacementIndex)
	{
		m_document->undoStack().push(new PaletteReplaceColorCommand(m_document, originalIndex, replacementIndex));
	}
}


void MainWindow::viewFitBackgroundImage()
{
	KAction *action = qobject_cast<KAction *>(sender());
	m_document->undoStack().push(new FitBackgroundImageCommand(m_document, QVariantPtr<BackgroundImage>::asPtr(action->data()), m_editor->selectionArea()));
}


void MainWindow::viewShowBackgroundImage()
{
	KAction *action = qobject_cast<KAction *>(sender());
	m_document->undoStack().push(new ShowBackgroundImageCommand(m_document, QVariantPtr<BackgroundImage>::asPtr(action->data()), action->isChecked()));
}


void MainWindow::libraryManager()
{
	if (m_libraryManagerDlg == 0)
		m_libraryManagerDlg = new LibraryManagerDlg(this);
	m_libraryManagerDlg->show();
}


void MainWindow::patternExtend()
{
	QPointer<ExtendPatternDlg> extendPatternDlg = new ExtendPatternDlg(this);
	if (extendPatternDlg->exec())
	{
		int top = extendPatternDlg->top();
		int left = extendPatternDlg->left();
		int right = extendPatternDlg->right();
		int bottom = extendPatternDlg->bottom();

		if (top || left || right || bottom)
		{
			m_document->undoStack().push(new ExtendPatternCommand(m_document, top, left, right, bottom));
		}
	}
	delete extendPatternDlg;
}


void MainWindow::patternCentre()
{
	m_document->undoStack().push(new CentrePatternCommand(m_document));
}


void MainWindow::patternCrop()
{
	m_document->undoStack().push(new CropToPatternCommand(m_document));
}


void MainWindow::patternCropToSelection()
{
	m_document->undoStack().push(new CropToSelectionCommand(m_document, m_editor->selectionArea()));
}


void MainWindow::formatScalesAsStitches()
{
	m_horizontalScale->setUnits(Configuration::EnumEditor_FormatScalesAs::Stitches);
	m_verticalScale->setUnits(Configuration::EnumEditor_FormatScalesAs::Stitches);
}


void MainWindow::formatScalesAsCM()
{
	m_horizontalScale->setUnits(Configuration::EnumEditor_FormatScalesAs::CM);
	m_verticalScale->setUnits(Configuration::EnumEditor_FormatScalesAs::CM);
}


void MainWindow::formatScalesAsInches()
{
	m_horizontalScale->setUnits(Configuration::EnumEditor_FormatScalesAs::Inches);
	m_verticalScale->setUnits(Configuration::EnumEditor_FormatScalesAs::Inches);
}


void MainWindow::documentModified(bool clean)
{
	setCaption(m_document->url().fileName(), !clean);
}


void MainWindow::setupActions()
{
	KAction *action;
	QActionGroup *actionGroup;

	KActionCollection *actions = actionCollection();

	// File menu
	KStandardAction::openNew(this, SLOT(fileNew()), actions);
	KStandardAction::open(this, SLOT(fileOpen()), actions);
	KStandardAction::openRecent(this, SLOT(fileOpen(const KUrl &)), actions)->loadEntries(KConfigGroup(KGlobal::config(), "RecentFiles"));
	KStandardAction::save(this, SLOT(fileSave()), actions);
	KStandardAction::saveAs(this, SLOT(fileSaveAs()), actions);
	KStandardAction::revert(this, SLOT(fileRevert()), actions);

	action = new KAction(this);
	action->setText(i18n("Print Setup..."));
	connect(action, SIGNAL(triggered()), this, SLOT(filePrintSetup()));
	actions->addAction("filePrintSetup", action);

	KStandardAction::print(this, SLOT(filePrint()), actions);

	action = new KAction(this);
	action->setText(i18n("Import Image"));
	connect(action, SIGNAL(triggered()), this, SLOT(fileImportImage()));
	actions->addAction("fileImportImage", action);

	action = new KAction(this);
	action->setText(i18n("File Properties"));
	connect(action, SIGNAL(triggered()), this, SLOT(fileProperties()));
	actions->addAction("fileProperties", action);

	action = new KAction(this);
	action->setText(i18n("Add Background Image..."));
	connect(action, SIGNAL(triggered()), this, SLOT(fileAddBackgroundImage()));
	actions->addAction("fileAddBackgroundImage", action);

	KStandardAction::close(this, SLOT(fileClose()), actions);
	KStandardAction::quit(this, SLOT(fileQuit()), actions);


	// Edit menu
	KStandardAction::undo(this, SLOT(editUndo()), actions);
	KStandardAction::redo(this, SLOT(editRedo()), actions);
	KStandardAction::cut(m_editor, SLOT(editCut()), actions);
	actions->action("edit_cut")->setEnabled(false);
	KStandardAction::copy(m_editor, SLOT(editCopy()), actions);
	actions->action("edit_copy")->setEnabled(false);
	KStandardAction::paste(m_editor, SLOT(editPaste()), actions);

	action = new KAction(this);
	action->setText(i18n("Mirror/Rotate makes copies"));
	action->setCheckable(true);
	connect(action, SIGNAL(triggered(bool)), m_editor, SLOT(setMakesCopies(bool)));
	actions->addAction("makesCopies", action);
	
	action = new KAction(this);
	action->setText(i18n("Horizontally"));
	action->setData(Qt::Horizontal);
	connect(action, SIGNAL(triggered()), m_editor, SLOT(mirrorSelection()));
	action->setEnabled(false);
	actions->addAction("mirrorHorizontal", action);
	
	action = new KAction(this);
	action->setText(i18n("Vertically"));
	action->setData(Qt::Vertical);
	connect(action, SIGNAL(triggered()), m_editor, SLOT(mirrorSelection()));
	action->setEnabled(false);
	actions->addAction("mirrorVertical", action);
	
	// Selection mask sub menu
	action = new KAction(this);
	action->setText(i18n("Stitch Mask"));
	action->setCheckable(true);
	connect(action, SIGNAL(triggered(bool)), m_editor, SLOT(setMaskStitch(bool)));
	actions->addAction("maskStitch", action);

	action = new KAction(this);
	action->setText(i18n("Color Mask"));
	action->setCheckable(true);
	connect(action, SIGNAL(triggered(bool)), m_editor, SLOT(setMaskColor(bool)));
	actions->addAction("maskColor", action);

	action = new KAction(this);
	action->setText(i18n("Exclude Backstitches"));
	action->setCheckable(true);
	connect(action, SIGNAL(triggered(bool)), m_editor, SLOT(setMaskBackstitch(bool)));
	actions->addAction("maskBackstitch", action);

	action = new KAction(this);
	action->setText(i18n("Exclude Knots"));
	action->setCheckable(true);
	connect(action, SIGNAL(triggered(bool)), m_editor, SLOT(setMaskKnot(bool)));
	actions->addAction("maskKnot", action);


	// View menu
	KStandardAction::zoomIn(m_editor, SLOT(zoomIn()), actions);
	KStandardAction::zoomOut(m_editor, SLOT(zoomOut()), actions);
	KStandardAction::actualSize(m_editor, SLOT(actualSize()), actions);
	action = KStandardAction::fitToPage(m_editor, SLOT(fitToPage()), actions);
	action->setIcon(KIcon("zoom-fit-best"));
	action = KStandardAction::fitToWidth(m_editor, SLOT(fitToWidth()), actions);
	action->setIcon(KIcon("zoom-fit-width"));
	action = KStandardAction::fitToHeight(m_editor, SLOT(fitToHeight()), actions);
	action->setIcon(KIcon("zoom-fit-height"));

	// Entries for Show/Hide Preview and Palette dock windows are added dynamically
	// Entries for Show/Hide and Remove background images are added dynamically


	// Stitches Menu
	actionGroup = new QActionGroup(this);
	actionGroup->setExclusive(true);

	action = new KAction(this);
	action->setText(i18n("Quarter Stitch"));
	action->setData(Editor::StitchQuarter);
	action->setIcon(KIcon("quarter"));
	action->setCheckable(true);
	connect(action, SIGNAL(triggered()), m_editor, SLOT(selectStitch()));
	actions->addAction("stitchQuarter", action);
	actionGroup->addAction(action);

	action = new KAction(this);
	action->setText(i18n("Half Stitch"));
	action->setData(Editor::StitchHalf);
	action->setIcon(KIcon("half"));
	action->setCheckable(true);
	connect(action, SIGNAL(triggered()), m_editor, SLOT(selectStitch()));
	actions->addAction("stitchHalf", action);
	actionGroup->addAction(action);

	action = new KAction(this);
	action->setText(i18n("3 Quarter Stitch"));
	action->setData(Editor::Stitch3Quarter);
	action->setIcon(KIcon("3quarter"));
	action->setCheckable(true);
	connect(action, SIGNAL(triggered()), m_editor, SLOT(selectStitch()));
	actions->addAction("stitch3Quarter", action);
	actionGroup->addAction(action);

	action = new KAction(this);
	action->setText(i18n("Full Stitch"));
	action->setData(Editor::StitchFull);
	action->setIcon(KIcon("full"));
	action->setCheckable(true);
	connect(action, SIGNAL(triggered()), m_editor, SLOT(selectStitch()));
	actions->addAction("stitchFull", action);
	actionGroup->addAction(action);

	action = new KAction(this);
	action->setText(i18n("Small Half Stitch"));
	action->setData(Editor::StitchSmallHalf);
	action->setIcon(KIcon("smallhalf"));
	action->setCheckable(true);
	connect(action, SIGNAL(triggered()), m_editor, SLOT(selectStitch()));
	actions->addAction("stitchSmallHalf", action);
	actionGroup->addAction(action);

	action = new KAction(this);
	action->setText(i18n("Small Full Stitch"));
	action->setData(Editor::StitchSmallFull);
	action->setIcon(KIcon("smallfull"));
	action->setCheckable(true);
	connect(action, SIGNAL(triggered()), m_editor, SLOT(selectStitch()));
	actions->addAction("stitchSmallFull", action);
	actionGroup->addAction(action);

	action = new KAction(this);
	action->setText(i18n("French Knot"));
	action->setData(Editor::StitchFrenchKnot);
	action->setIcon(KIcon("frenchknot"));
	action->setCheckable(true);
	connect(action, SIGNAL(triggered()), m_editor, SLOT(selectStitch()));
	actions->addAction("stitchFrenchKnot", action);
	actionGroup->addAction(action);


	// Tools Menu
	actionGroup = new QActionGroup(this);
	actionGroup->setExclusive(true);

	action = new KAction(this);
	action->setText(i18n("Paint"));
	action->setData(Editor::ToolPaint);
	action->setIcon(KIcon("draw-brush"));
	action->setCheckable(true);
	connect(action, SIGNAL(triggered()), m_editor, SLOT(selectTool()));
	actions->addAction("toolPaint", action);
	actionGroup->addAction(action);

	action = new KAction(this);
	action->setText(i18n("Draw"));
	action->setData(Editor::ToolDraw);
	action->setIcon(KIcon("draw-freehand"));
	action->setCheckable(true);
	connect(action, SIGNAL(triggered()), m_editor, SLOT(selectTool()));
	actions->addAction("toolDraw", action);
	actionGroup->addAction(action);

	action = new KAction(this);
	action->setText(i18n("Erase"));
	action->setData(Editor::ToolErase);
	action->setIcon(KIcon("draw-eraser"));
	action->setCheckable(true);
	connect(action, SIGNAL(triggered()), m_editor, SLOT(selectTool()));
	actions->addAction("toolErase", action);
	actionGroup->addAction(action);

	action = new KAction(this);
	action->setText(i18n("Draw Rectangle"));
	action->setData(Editor::ToolRectangle);
	action->setIcon(KIcon("o_rect"));
	action->setCheckable(true);
	connect(action, SIGNAL(triggered()), m_editor, SLOT(selectTool()));
	actions->addAction("toolRectangle", action);
	actionGroup->addAction(action);

	action = new KAction(this);
	action->setText(i18n("Fill Rectangle"));
	action->setData(Editor::ToolFillRectangle);
	action->setIcon(KIcon("f_rect"));
	action->setCheckable(true);
	connect(action, SIGNAL(triggered()), m_editor, SLOT(selectTool()));
	actions->addAction("toolFillRectangle", action);
	actionGroup->addAction(action);

	action = new KAction(this);
	action->setText(i18n("Draw Ellipse"));
	action->setData(Editor::ToolEllipse);
	action->setIcon(KIcon("o_ellipse"));
	action->setCheckable(true);
	connect(action, SIGNAL(triggered()), m_editor, SLOT(selectTool()));
	actions->addAction("toolEllipse", action);
	actionGroup->addAction(action);

	action = new KAction(this);
	action->setText(i18n("Fill Ellipse"));
	action->setData(Editor::ToolFillEllipse);
	action->setIcon(KIcon("f_ellipse"));
	action->setCheckable(true);
	connect(action, SIGNAL(triggered()), m_editor, SLOT(selectTool()));
	actions->addAction("toolFillEllipse", action);
	actionGroup->addAction(action);

	action = new KAction(this);
	action->setText(i18n("Fill Polygon"));
	action->setData(Editor::ToolFillPolygon);
	action->setIcon(KIcon("polygon"));
	action->setCheckable(true);
	connect(action, SIGNAL(triggered()), m_editor, SLOT(selectTool()));
	actions->addAction("toolFillPolygon", action);
	actionGroup->addAction(action);

	action = new KAction(this);
	action->setText(i18n("Text"));
	action->setData(Editor::ToolText);
	action->setIcon(KIcon("insert-text"));
	action->setCheckable(true);
	connect(action, SIGNAL(triggered()), m_editor, SLOT(selectTool()));
	actions->addAction("toolText", action);
	actionGroup->addAction(action);

	action = new KAction(this);
	action->setText(i18nc("Select an area of the pattern", "Select"));
	action->setData(Editor::ToolSelect);
	action->setIcon(KIcon("s_rect"));
	action->setCheckable(true);
	connect(action, SIGNAL(triggered()), m_editor, SLOT(selectTool()));
	actions->addAction("toolSelectRectangle", action);
	actionGroup->addAction(action);

	action = new KAction(this);
	action->setText(i18n("Backstitch"));
	action->setData(Editor::ToolBackstitch);
	action->setIcon(KIcon("backstitch"));
	action->setCheckable(true);
	connect(action, SIGNAL(triggered()), m_editor, SLOT(selectTool()));
	actions->addAction("toolBackstitch", action);
	actionGroup->addAction(action);
	
	action = new KAction(this);
	action->setText(i18n("Color Picker"));
	action->setData(Editor::ToolColorPicker);
	action->setIcon(KIcon("color-picker"));
	action->setCheckable(true);
	connect(action, SIGNAL(triggered()), m_editor, SLOT(selectTool()));
	actions->addAction("toolColorPicker", action);
	actionGroup->addAction(action);


	// Palette Menu
	action = new KAction(this);
	action->setText(i18n("Palette Manager..."));
	action->setIcon(KIcon("palette-manager"));
	connect(action, SIGNAL(triggered()), this, SLOT(paletteManager()));
	actions->addAction("paletteManager", action);

	action = new KAction(this);
	action->setText(i18n("Show Symbols"));
	action->setCheckable(true);
	connect(action, SIGNAL(toggled(bool)), this, SLOT(paletteShowSymbols(bool)));
	actions->addAction("paletteShowSymbols", action);

	action = new KAction(this);
	action->setText(i18n("Clear Unused"));
	connect(action, SIGNAL(triggered()), this, SLOT(paletteClearUnused()));
	actions->addAction("paletteClearUnused", action);

	action = new KAction(this);
	action->setText(i18n("Calibrate Scheme..."));
	connect(action, SIGNAL(triggered()), this, SLOT(paletteCalibrateScheme()));
	actions->addAction("paletteCalibrateScheme", action);

	action = new KAction(this);
	action->setText(i18n("Swap Colors"));
	connect(action, SIGNAL(triggered()), m_palette, SLOT(swapColors()));
	actions->addAction("paletteSwapColors", action);

	action = new KAction(this);
	action->setText(i18n("Replace Colors"));
	connect(action, SIGNAL(triggered()), m_palette, SLOT(replaceColor()));
	actions->addAction("paletteReplaceColor", action);


	// Pattern Menu
	action = new KAction(this);
	action->setText(i18n("Extend Pattern..."));
	action->setIcon(KIcon("extpattern"));
	connect(action, SIGNAL(triggered()), this, SLOT(patternExtend()));
	actions->addAction("patternExtend", action);
	
	action = new KAction(this);
	action->setText(i18n("Centre Pattern"));
	action->setIcon(KIcon("centerpattern"));
	connect(action, SIGNAL(triggered()), this, SLOT(patternCentre()));
	actions->addAction("patternCentre", action);
	
	action = new KAction(this);
	action->setText(i18n("Crop Canvas to Pattern"));
	connect(action, SIGNAL(triggered()), this, SLOT(patternCrop()));
	actions->addAction("patternCrop", action);
	
	action = new KAction(this);
	action->setText(i18n("Crop Canvas to Selection"));
	connect(action, SIGNAL(triggered()), this, SLOT(patternCropToSelection()));
	action->setEnabled(false);
	actions->addAction("patternCropToSelection", action);


	// Library Menu
	action = new KAction(this);
	action->setText(i18n("Library Manager..."));
	connect(action, SIGNAL(triggered()), this, SLOT(libraryManager()));
	actions->addAction("libraryManager", action);

	// Settings Menu
	// formatScalesAs
	actionGroup = new QActionGroup(this);
	actionGroup->setExclusive(true);

	action = new KAction(this);
	action->setText(i18n("Stitches"));
	action->setCheckable(true);
	connect(action, SIGNAL(triggered()), this, SLOT(formatScalesAsStitches()));
	actions->addAction("formatScalesAsStitches", action);
	actionGroup->addAction(action);

	action = new KAction(this);
	action->setText(i18n("CM"));
	action->setCheckable(true);
	connect(action, SIGNAL(triggered()), this, SLOT(formatScalesAsCM()));
	actions->addAction("formatScalesAsCM", action);
	actionGroup->addAction(action);

	action = new KAction(this);
	action->setText(i18n("Inches"));
	action->setCheckable(true);
	connect(action, SIGNAL(triggered()), this, SLOT(formatScalesAsInches()));
	actions->addAction("formatScalesAsInches", action);
	actionGroup->addAction(action);

	// ShowStitchesAs
	actionGroup = new QActionGroup(this);
	actionGroup->setExclusive(true);

	action = new KAction(this);
	action->setText(i18n("Regular Stitches"));
	action->setData(Configuration::EnumRenderer_RenderStitchesAs::Stitches);
	action->setCheckable(true);
	action->setChecked(true);
	connect(action, SIGNAL(triggered()), m_editor, SLOT(renderStitchesAs()));
	actions->addAction("renderStitchesAsRegularStitches", action);
	actionGroup->addAction(action);

	action = new KAction(this);
	action->setText(i18n("Black & White Symbols"));
	action->setData(Configuration::EnumRenderer_RenderStitchesAs::BlackWhiteSymbols);
	action->setCheckable(true);
	connect(action, SIGNAL(triggered()), m_editor, SLOT(renderStitchesAs()));
	actions->addAction("renderStitchesAsBlackWhiteSymbols", action);
	actionGroup->addAction(action);

	action = new KAction(this);
	action->setText(i18n("Color Symbols"));
	action->setData(Configuration::EnumRenderer_RenderStitchesAs::ColorSymbols);
	action->setCheckable(true);
	connect(action, SIGNAL(triggered()), m_editor, SLOT(renderStitchesAs()));
	actions->addAction("renderStitchesAsColorSymbols", action);
	actionGroup->addAction(action);

	action = new KAction(this);
	action->setText(i18n("Color Blocks"));
	action->setData(Configuration::EnumRenderer_RenderStitchesAs::ColorBlocks);
	action->setCheckable(true);
	connect(action, SIGNAL(triggered()), m_editor, SLOT(renderStitchesAs()));
	actions->addAction("renderStitchesAsColorBlocks", action);
	actionGroup->addAction(action);

	action = new KAction(this);
	action->setText(i18n("Color Blocks & Symbols"));
	action->setData(Configuration::EnumRenderer_RenderStitchesAs::ColorBlocksSymbols);
	action->setCheckable(true);
	connect(action, SIGNAL(triggered()), m_editor, SLOT(renderStitchesAs()));
	actions->addAction("renderStitchesAsColorBlocksSymbols", action);
	actionGroup->addAction(action);

	// ShowBackstitchesAs
	actionGroup = new QActionGroup(this);
	actionGroup->setExclusive(true);

	action = new KAction(this);
	action->setText(i18n("Color Lines"));
	action->setData(Configuration::EnumRenderer_RenderBackstitchesAs::ColorLines);
	action->setCheckable(true);
	action->setChecked(true);
	connect(action, SIGNAL(triggered()), m_editor, SLOT(renderBackstitchesAs()));
	actions->addAction("renderBackstitchesAsColorLines", action);
	actionGroup->addAction(action);

	action = new KAction(this);
	action->setText(i18n("Black & White Symbols"));
	action->setData(Configuration::EnumRenderer_RenderBackstitchesAs::BlackWhiteSymbols);
	action->setCheckable(true);
	connect(action, SIGNAL(triggered()), m_editor, SLOT(renderBackstitchesAs()));
	actions->addAction("renderBackstitchesAsBlackWhiteSymbols", action);
	actionGroup->addAction(action);

	// ShowKnotsAs
	actionGroup = new QActionGroup(this);
	actionGroup->setExclusive(true);

	action = new KAction(this);
	action->setText(i18n("Color Blocks"));
	action->setData(Configuration::EnumRenderer_RenderKnotsAs::ColorBlocks);
	action->setCheckable(true);
	action->setChecked(true);
	connect(action, SIGNAL(triggered()), m_editor, SLOT(renderKnotsAs()));
	actions->addAction("renderKnotsAsColorBlocks", action);
	actionGroup->addAction(action);

	action = new KAction(this);
	action->setText(i18n("Color Symbols"));
	action->setData(Configuration::EnumRenderer_RenderKnotsAs::ColorSymbols);
	action->setCheckable(true);
	connect(action, SIGNAL(triggered()), m_editor, SLOT(renderKnotsAs()));
	actions->addAction("renderKnotsAsColorSymbols", action);
	actionGroup->addAction(action);

	action = new KAction(this);
	action->setText(i18n("Black & White Symbols"));
	action->setData(Configuration::EnumRenderer_RenderKnotsAs::BlackWhiteSymbols);
	action->setCheckable(true);
	connect(action, SIGNAL(triggered()), m_editor, SLOT(renderKnotsAs()));
	actions->addAction("renderKnotsAsBlackWhiteSymbols", action);
	actionGroup->addAction(action);


	action = new KAction(this);
	action->setText(i18n("Color Hilight"));
	action->setCheckable(true);
	connect(action, SIGNAL(toggled(bool)), m_editor, SLOT(colorHilight(bool)));
	actions->addAction("colorHilight", action);

	action = new KAction(this);
	action->setText(i18n("Show Stitches"));
	action->setCheckable(true);
	connect(action, SIGNAL(toggled(bool)), m_editor, SLOT(renderStitches(bool)));
	actions->addAction("renderStitches", action);

	action = new KAction(this);
	action->setText(i18n("Show Backstitches"));
	action->setCheckable(true);
	connect(action, SIGNAL(toggled(bool)), m_editor, SLOT(renderBackstitches(bool)));
	actions->addAction("renderBackstitches", action);

	action = new KAction(this);
	action->setText(i18n("Show French Knots"));
	action->setCheckable(true);
	connect(action, SIGNAL(toggled(bool)), m_editor, SLOT(renderFrenchKnots(bool)));
	actions->addAction("renderFrenchKnots", action);

	action = new KAction(this);
	action->setText(i18n("Show Grid"));
	action->setCheckable(true);
	connect(action, SIGNAL(toggled(bool)), m_editor, SLOT(renderGrid(bool)));
	actions->addAction("renderGrid", action);

	action = new KAction(this);
	action->setText(i18n("Show Background Images"));
	action->setCheckable(true);
	connect(action, SIGNAL(toggled(bool)), m_editor, SLOT(renderBackgroundImages(bool)));
	actions->addAction("renderBackgroundImages", action);

	m_horizontalScale->addAction(actions->action("formatScalesAsStitches"));
	m_horizontalScale->addAction(actions->action("formatScalesAsCM"));
	m_horizontalScale->addAction(actions->action("formatScalesAsInches"));

	m_verticalScale->addAction(actions->action("formatScalesAsStitches"));
	m_verticalScale->addAction(actions->action("formatScalesAsCM"));
	m_verticalScale->addAction(actions->action("formatScalesAsInches"));

	setupGUI(KXmlGuiWindow::Default, "kxstitchui.rc");
}


void MainWindow::updateBackgroundImageActionLists()
{
	QListIterator<BackgroundImage *> backgroundImages = m_document->backgroundImages().backgroundImages();

	unplugActionList("removeBackgroundImageActions");
	unplugActionList("fitBackgroundImageActions");
	unplugActionList("showBackgroundImageActions");

	QList<QAction *> removeBackgroundImageActions;
	QList<QAction *> fitBackgroundImageActions;
	QList<QAction *> showBackgroundImageActions;

	while (backgroundImages.hasNext())
	{
		BackgroundImage *background = backgroundImages.next();

		KAction *action = new KAction(background->url().fileName(), this);
		action->setData(QVariantPtr<BackgroundImage>::asQVariant(background));
		action->setIcon(background->icon());
		connect(action, SIGNAL(triggered()), this, SLOT(fileRemoveBackgroundImage()));
		removeBackgroundImageActions.append(action);

		action = new KAction(background->url().fileName(), this);
		action->setData(QVariantPtr<BackgroundImage>::asQVariant(background));
		action->setIcon(background->icon());
		connect(action, SIGNAL(triggered()), this, SLOT(viewFitBackgroundImage()));
		fitBackgroundImageActions.append(action);

		action = new KAction(background->url().fileName(), this);
		action->setData(QVariantPtr<BackgroundImage>::asQVariant(background));
		action->setIcon(background->icon());
		action->setCheckable(true);
		action->setChecked(background->isVisible());
		connect(action, SIGNAL(triggered()), this, SLOT(viewShowBackgroundImage()));
		showBackgroundImageActions.append(action);
	}

	plugActionList("removeBackgroundImageActions", removeBackgroundImageActions);
	plugActionList("fitBackgroundImageActions", fitBackgroundImageActions);
	plugActionList("showBackgroundImageActions", showBackgroundImageActions);
}


void MainWindow::setupDockWindows()
{
	QDockWidget *dock = new QDockWidget(i18n("Preview"), this);
	dock->setAllowedAreas(Qt::AllDockWidgetAreas);
	QScrollArea *scrollArea = new QScrollArea();
	m_preview = new Preview(scrollArea);
	scrollArea->setWidget(m_preview);
	scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	scrollArea->setMinimumSize(std::min(300, m_preview->width()), std::min(400, m_preview->height()));
	dock->setWidget(scrollArea);
	addDockWidget(Qt::LeftDockWidgetArea, dock);
	actionCollection()->addAction("showPreviewDockWidget", dock->toggleViewAction());

	dock = new QDockWidget(i18n("Palette"), this);
	dock->setAllowedAreas(Qt::AllDockWidgetAreas);
	m_palette = new Palette(this);
	dock->setWidget(m_palette);
	addDockWidget(Qt::LeftDockWidgetArea, dock);
	actionCollection()->addAction("showPaletteDockWidget", dock->toggleViewAction());
	
	dock = new QDockWidget(i18n("History"), this);
	dock->setAllowedAreas(Qt::AllDockWidgetAreas);
	m_history = new QUndoView(this);
	dock->setWidget(m_history);
	addDockWidget(Qt::LeftDockWidgetArea, dock);
	actionCollection()->addAction("showHistoryDockWidget", dock->toggleViewAction());
}
