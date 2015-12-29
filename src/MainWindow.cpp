/*
 * Copyright (C) 2010-2015 by Stephen Allewell
 * steve.allewell@gmail.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#include "MainWindow.h"

#include <QAction>
#include <QActionGroup>
#include <QClipboard>
#include <QDataStream>
#include <QDockWidget>
#include <QFileDialog>
#include <QGridLayout>
#include <QMenu>
#include <QMimeData>
#include <QPainter>
#include <QPaintEngine>
#include <QProgressDialog>
#include <QPrintDialog>
#include <QPrinter>
#include <QPrintEngine>
#include <QPrintPreviewDialog>
#include <QSaveFile>
#include <QScrollArea>
#include <QTemporaryFile>
#include <QUndoView>
#include <QUrl>

#include <KActionCollection>
#include <KConfigDialog>
#include <KIO/FileCopyJob>
#include <KIO/StatJob>
#include <KLocalizedString>
#include <KMessageBox>
#include <KRecentFilesAction>
#include <KSelectAction>
#include <KXMLGUIFactory>

#include "BackgroundImage.h"
#include "configuration.h"
#include "ConfigurationDialogs.h"
#include "Commands.h"
#include "Document.h"
#include "Editor.h"
#include "ExtendPatternDlg.h"
#include "FilePropertiesDlg.h"
#include "Floss.h"
#include "FlossScheme.h"
#include "ImportImageDlg.h"
#include "Palette.h"
#include "PaletteManagerDlg.h"
#include "PaperSizes.h"
#include "Preview.h"
#include "PrintSetupDlg.h"
#include "QVariantPtr.h"
#include "Scale.h"
#include "ScaledPixmapLabel.h"
#include "SchemeManager.h"
#include "SymbolLibrary.h"
#include "SymbolManager.h"


MainWindow::MainWindow()
    :   m_printer(nullptr)
{
    setupActions();
}


MainWindow::MainWindow(const QUrl &url)
    :   m_printer(nullptr)
{
    setupMainWindow();
    setupLayout();
    setupDockWindows();
    setupActions();
    setupDocument();
    setupConnections();
    setupActionDefaults();
    loadSettings();
    fileOpen(url);
    setupActionsFromDocument();
    setCaption(m_document->url().fileName(), !m_document->undoStack().isClean());
    this->findChild<QDockWidget *>(QStringLiteral("ImportedImage#"))->hide();
}


MainWindow::MainWindow(const QString &source)
    :   m_printer(nullptr)
{
    setupMainWindow();
    setupLayout();
    setupDockWindows();
    setupActions();
    setupDocument();
    setupConnections();
    setupActionDefaults();
    loadSettings();
    convertImage(source);
    convertPreview(source);
    setupActionsFromDocument();
    setCaption(m_document->url().fileName(), !m_document->undoStack().isClean());
    this->findChild<QDockWidget *>(QStringLiteral("ImportedImage#"))->show();
}


void MainWindow::setupMainWindow()
{
    setObjectName(QStringLiteral("MainWindow#"));
    setAutoSaveSettings();
}


void MainWindow::setupLayout()
{
    QScrollArea *scrollArea = new QScrollArea();
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    m_editor = new Editor(scrollArea);
    scrollArea->installEventFilter(m_editor);
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
    m_document = new Document();

    m_editor->setDocument(m_document);
    m_editor->setPreview(m_preview);
    m_palette->setDocument(m_document);
    m_preview->setDocument(m_document);
    m_history->setStack(&(m_document->undoStack()));

    m_document->addView(m_editor);
    m_document->addView(m_preview);
    m_document->addView(m_palette);
}


void MainWindow::setupConnections()
{
    KActionCollection *actions = actionCollection();

    connect(&(m_document->undoStack()), SIGNAL(canUndoChanged(bool)), actions->action(QStringLiteral("edit_undo")), SLOT(setEnabled(bool)));
    connect(&(m_document->undoStack()), SIGNAL(canUndoChanged(bool)), actions->action(QStringLiteral("file_revert")), SLOT(setEnabled(bool)));
    connect(&(m_document->undoStack()), SIGNAL(canRedoChanged(bool)), actions->action(QStringLiteral("edit_redo")), SLOT(setEnabled(bool)));
    connect(QApplication::clipboard(), SIGNAL(dataChanged()), this, SLOT(clipboardDataChanged()));
    connect(m_editor, SIGNAL(selectionMade(bool)), actionCollection()->action(QStringLiteral("edit_cut")), SLOT(setEnabled(bool)));
    connect(m_editor, SIGNAL(selectionMade(bool)), actionCollection()->action(QStringLiteral("edit_copy")), SLOT(setEnabled(bool)));
    connect(m_editor, SIGNAL(selectionMade(bool)), actionCollection()->action(QStringLiteral("mirrorHorizontal")), SLOT(setEnabled(bool)));
    connect(m_editor, SIGNAL(selectionMade(bool)), actionCollection()->action(QStringLiteral("mirrorVertical")), SLOT(setEnabled(bool)));
    connect(m_editor, SIGNAL(selectionMade(bool)), actionCollection()->action(QStringLiteral("rotate90")), SLOT(setEnabled(bool)));
    connect(m_editor, SIGNAL(selectionMade(bool)), actionCollection()->action(QStringLiteral("rotate180")), SLOT(setEnabled(bool)));
    connect(m_editor, SIGNAL(selectionMade(bool)), actionCollection()->action(QStringLiteral("rotate270")), SLOT(setEnabled(bool)));
    connect(m_editor, SIGNAL(selectionMade(bool)), actionCollection()->action(QStringLiteral("patternCropToSelection")), SLOT(setEnabled(bool)));
    connect(m_editor, SIGNAL(selectionMade(bool)), actionCollection()->action(QStringLiteral("insertColumns")), SLOT(setEnabled(bool)));
    connect(m_editor, SIGNAL(selectionMade(bool)), actionCollection()->action(QStringLiteral("insertRows")), SLOT(setEnabled(bool)));
    connect(&(m_document->undoStack()), SIGNAL(undoTextChanged(QString)), this, SLOT(undoTextChanged(QString)));
    connect(&(m_document->undoStack()), SIGNAL(redoTextChanged(QString)), this, SLOT(redoTextChanged(QString)));
    connect(&(m_document->undoStack()), SIGNAL(cleanChanged(bool)), this, SLOT(documentModified(bool)));
    connect(m_palette, SIGNAL(colorSelected(int)), m_editor, SLOT(drawContents()));
    connect(m_palette, SIGNAL(swapColors(int,int)), this, SLOT(paletteSwapColors(int,int)));
    connect(m_palette, SIGNAL(replaceColor(int,int)), this, SLOT(paletteReplaceColor(int,int)));
    connect(m_palette, SIGNAL(signalStateChanged(QString,bool)), this, SLOT(slotStateChanged(QString,bool)));
    connect(m_palette, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(paletteContextMenu(QPoint)));
    connect(m_editor, SIGNAL(changedVisibleCells(QRect)), m_preview, SLOT(setVisibleCells(QRect)));
    connect(m_preview, SIGNAL(clicked(QPoint)), m_editor, SLOT(previewClicked(QPoint)));
    connect(m_preview, SIGNAL(clicked(QRect)), m_editor, SLOT(previewClicked(QRect)));
}


void MainWindow::setupActionDefaults()
{
    KActionCollection *actions = actionCollection();

    actions->action(QStringLiteral("maskStitch"))->setChecked(false);
    actions->action(QStringLiteral("maskColor"))->setChecked(false);
    actions->action(QStringLiteral("maskBackstitch"))->setChecked(false);
    actions->action(QStringLiteral("maskKnot"))->setChecked(false);

    actions->action(QStringLiteral("stitchFull"))->trigger();   // Select full stitch

    actions->action(QStringLiteral("toolPaint"))->trigger();    // Select paint tool

    clipboardDataChanged();
}


MainWindow::~MainWindow()
{
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
    if (m_document->undoStack().isClean()) {
        return true;
    }

    while (true) {
        int messageBoxResult = KMessageBox::warningYesNoCancel(this, i18n("Save changes to document?\nSelecting No discards changes."));

        switch (messageBoxResult) {
        case KMessageBox::Yes :
            fileSave();

            if (m_document->undoStack().isClean()) {
                return true;
            } else {
                KMessageBox::error(this, i18n("Unable to save the file"));
            }

            break;

        case KMessageBox::No :
            return true;

        case KMessageBox::Cancel :
            return false;
        }
    }
}


void MainWindow::setupActionsFromDocument()
{
    KActionCollection *actions = actionCollection();

    actions->action(QStringLiteral("file_revert"))->setEnabled(!m_document->undoStack().isClean());
    actions->action(QStringLiteral("edit_undo"))->setEnabled(m_document->undoStack().canUndo());
    actions->action(QStringLiteral("edit_redo"))->setEnabled(m_document->undoStack().canRedo());

    updateBackgroundImageActionLists();
}


void MainWindow::fileNew()
{
    MainWindow *window = new MainWindow(QUrl());
    window->show();
}


void MainWindow::fileOpen()
{
    fileOpen(QFileDialog::getOpenFileUrl(this, i18n("Open file"), QUrl::fromLocalFile(QDir::homePath()), i18n("KXStitch Patterns (*.kxs);;PC Stitch Patterns (*.pat);;All Files (*)")));
}


void MainWindow::fileOpen(const QUrl &url)
{
    MainWindow *window;
    bool docEmpty = (m_document->undoStack().isClean() && (m_document->url() == i18n("Untitled")));

    if (url.isValid()) {
        if (docEmpty) {
            QTemporaryFile tmpFile;

            if (tmpFile.open()) {
                KIO::FileCopyJob *job = KIO::file_copy(url, QUrl::fromLocalFile(tmpFile.fileName()), -1, KIO::Overwrite);

                if (job->exec()) {
                    QDataStream stream(&tmpFile);

                    try {
                        m_document->readKXStitch(stream);
                        m_document->setUrl(url);
                        KRecentFilesAction *action = static_cast<KRecentFilesAction *>(actionCollection()->action(QStringLiteral("file_open_recent")));
                        action->addUrl(url);
                        action->saveEntries(KConfigGroup(KSharedConfig::openConfig(), QStringLiteral("RecentFiles")));
                    } catch (const InvalidFile &e) {
                        stream.device()->seek(0);

                        try {
                            m_document->readPCStitch(stream);
                        } catch (const InvalidFile &e) {
                            KMessageBox::sorry(nullptr, i18n("The file does not appear to be a recognized cross stitch file."));
                        }
                    } catch (const InvalidFileVersion &e) {
                        KMessageBox::sorry(nullptr, i18n("This version of the file is not supported.\n%1", e.version));
                    } catch (const FailedReadFile &e) {
                        KMessageBox::error(nullptr, i18n("Failed to read the file.\n%1.", e.status));
                        m_document->initialiseNew();
                    }

                    setupActionsFromDocument();
                    m_editor->readDocumentSettings();
                    m_preview->readDocumentSettings();
                    m_palette->update();
                    documentModified(true); // this is the clean value true
                } else {
                    KMessageBox::error(nullptr, job->errorString());
                }
            } else {
                KMessageBox::error(nullptr, tmpFile.errorString());
            }
        } else {
            window = new MainWindow(url);
            window->show();
        }
    }
}


void MainWindow::fileSave()
{
    QUrl url = m_document->url();

    if (url == i18n("Untitled")) {
        fileSaveAs();
    } else {
        QSaveFile file(url.path());

        if (file.open(QIODevice::WriteOnly)) {
            QDataStream stream(&file);

            try {
                m_document->write(stream);

                if (!file.commit()) {
                    throw FailedWriteFile(stream.status());
                }

                m_document->undoStack().setClean();
            } catch (const FailedWriteFile &e) {
                KMessageBox::error(nullptr, QString(i18n("Failed to save the file.\n%1", file.errorString())));
                file.cancelWriting();
            }
        } else {
            KMessageBox::error(nullptr, QString(i18n("Failed to open the file.\n%1", file.errorString())));
        }
    }

}


void MainWindow::fileSaveAs()
{
    QUrl url = QFileDialog::getSaveFileUrl(this, i18n("Save As..."), QUrl::fromLocalFile(QDir::homePath()), i18n("Cross Stitch Patterns (*.kxs)"));

    if (url.isValid()) {
        KIO::StatJob *statJob = KIO::stat(url, KIO::StatJob::DestinationSide, 0);

        if (statJob->exec()) {
            if (KMessageBox::warningYesNo(this, i18n("This file already exists\nDo you want to overwrite it?")) == KMessageBox::No) {
                return;
            }
        }

        m_document->setUrl(url);
        fileSave();
        KRecentFilesAction *action = static_cast<KRecentFilesAction *>(actionCollection()->action(QStringLiteral("file_open_recent")));
        action->addUrl(url);
        action->saveEntries(KConfigGroup(KSharedConfig::openConfig(), QStringLiteral("RecentFiles")));
    }
}


void MainWindow::fileRevert()
{
    if (!m_document->undoStack().isClean()) {
        if (KMessageBox::warningYesNo(this, i18n("Revert changes to document?")) == KMessageBox::Yes) {
            m_document->undoStack().setIndex(m_document->undoStack().cleanIndex());
        }
    }
}


void MainWindow::filePrintSetup()
{
    if (m_printer == nullptr) {
        m_printer = new QPrinter();
    }

    QPointer<PrintSetupDlg> printSetupDlg = new PrintSetupDlg(this, m_document, m_printer);

    if (printSetupDlg->exec() == QDialog::Accepted) {
        m_document->undoStack().push(new UpdatePrinterConfigurationCommand(m_document, printSetupDlg->printerConfiguration()));
    }

    delete printSetupDlg;
}


void MainWindow::filePrint()
{
    if (m_printer == nullptr) {
        filePrintSetup();
    }

    if (!m_document->printerConfiguration().pages().isEmpty()) {
        m_printer->setFullPage(true);
        m_printer->setPrintRange(QPrinter::AllPages);
        m_printer->setFromTo(1, m_document->printerConfiguration().pages().count());

        QPointer<QPrintDialog> printDialog = new QPrintDialog(m_printer, this);

        if (printDialog->exec() == QDialog::Accepted) {
            printPages();
        }

        delete printDialog;
    } else {
        KMessageBox::information(this, i18n("There is nothing to print"));
    }
}


void MainWindow::printPages()
{
    QList<Page *> pages = m_document->printerConfiguration().pages();

    int fromPage = 1;
    int toPage = pages.count();

    if (m_printer->printRange() == QPrinter::PageRange) {
        fromPage = m_printer->fromPage();
        toPage = m_printer->toPage();
    }

    while (toPage < pages.count()) pages.removeLast();
    while (--fromPage) pages.removeFirst();

    int totalPages = pages.count();

    const Page *page = (m_printer->pageOrder() == QPrinter::FirstPageFirst)?pages.takeFirst():pages.takeLast();

    m_printer->setPaperSize(page->paperSize());             // DEPRECATED
    m_printer->setOrientation(page->orientation());         // DEPRECATED

    QPainter painter;
    painter.begin(m_printer);
    painter.setRenderHint(QPainter::Antialiasing, true);

    for (int p = 0 ; p < totalPages ;) {
        int paperWidth = PaperSizes::width(page->paperSize(), page->orientation());
        int paperHeight = PaperSizes::height(page->paperSize(), page->orientation());

        painter.setWindow(0, 0, paperWidth, paperHeight);

        page->render(m_document, &painter);

        if (++p < totalPages) {
            page = (m_printer->pageOrder() == QPrinter::FirstPageFirst)?pages.takeFirst():pages.takeLast();

            m_printer->setPaperSize(page->paperSize());     // DEPRECATED
            m_printer->setOrientation(page->orientation()); // DEPRECATED

            m_printer->newPage();
        }
    }

    painter.end();
}


void MainWindow::fileImportImage()
{
    MainWindow *window;
    bool docEmpty = ((m_document->undoStack().isClean()) && (m_document->url() == i18n("Untitled")));
    QUrl url = QFileDialog::getOpenFileUrl(this, i18n("Import Image"), QUrl(QStandardPaths::writableLocation(QStandardPaths::HomeLocation)), i18n("Images (*.bmp *.gif *.jpg *.png *.pbm *.pgm *.ppm *.xbm *.xpm *.svg)"));

    if (url.isValid()) {
        QTemporaryFile tmpFile;

        if (tmpFile.open()) {
            KIO::FileCopyJob *job = KIO::file_copy(url, QUrl::fromLocalFile(tmpFile.fileName()), -1, KIO::Overwrite);

            if (job->exec()) {
                if (docEmpty) {
                    convertImage(tmpFile.fileName());
                    convertPreview(tmpFile.fileName());
                    this->findChild<QDockWidget *>(QStringLiteral("ImportedImage#"))->show();
                } else {
                    window = new MainWindow(tmpFile.fileName());
                    window->show();
                }
            } else {
                KMessageBox::error(nullptr, job->errorString());
            }
        }
    }
}


void MainWindow::convertImage(const QString &source)
{
    Magick::Image image(source.toStdString());
    QMap<int, QColor> documentFlosses;
    QList<qint16> symbolIndexes = SymbolManager::library(Configuration::palette_DefaultSymbolLibrary())->indexes();

    QPointer<ImportImageDlg> importImageDlg = new ImportImageDlg(this, image);

    if (importImageDlg->exec()) {
        Magick::Image convertedImage = importImageDlg->convertedImage();

        int imageWidth = convertedImage.columns();
        int imageHeight = convertedImage.rows();
        int documentWidth = imageWidth;
        int documentHeight = imageHeight;

        bool useFractionals = importImageDlg->useFractionals();

        bool ignoreColor = importImageDlg->ignoreColor();
        Magick::Color ignoreColorValue = importImageDlg->ignoreColorValue();

        int pixelCount = imageWidth * imageHeight;

        if (useFractionals) {
            documentWidth /= 2;
            documentHeight /= 2;
        }

        QString schemeName = importImageDlg->flossScheme();
        FlossScheme *flossScheme = SchemeManager::scheme(schemeName);

        QUndoCommand *importImageCommand = new ImportImageCommand(m_document);
        new ResizeDocumentCommand(m_document, documentWidth, documentHeight, importImageCommand);
        new ChangeSchemeCommand(m_document, schemeName, importImageCommand);

        QProgressDialog progress(i18n("Converting to stitches"), i18n("Cancel"), 0, pixelCount, this);
        progress.setWindowModality(Qt::WindowModal);
        Magick::Pixels cache(convertedImage);
        const Magick::PixelPacket *pixels = cache.getConst(0, 0, imageWidth, imageHeight);
        bool colorNotFound = false;

        for (int dy = 0 ; dy < imageHeight ; dy++) {
            progress.setValue(dy * imageWidth);
            QApplication::processEvents();

            if (progress.wasCanceled()) {
                delete importImageDlg;
                delete importImageCommand;
                return;
            }

            for (int dx = 0 ; dx < imageWidth ; dx++) {
                Magick::PixelPacket packet = *pixels++;

                if (!(packet.opacity)) {
                    if (!(ignoreColor && Magick::Color(packet) == ignoreColorValue)) {
                        int flossIndex;
#if MAGICKCORE_QUANTUM_DEPTH == 8
                        QColor color(packet.red, packet.green, packet.blue);
#else
                        QColor color(packet.red / 256, packet.green / 256, packet.blue / 256);
#endif

                        for (flossIndex = 0 ; flossIndex < documentFlosses.count() ; ++flossIndex) {
                            if (documentFlosses[flossIndex] == color) {
                                break;
                            }
                        }

                        if (flossIndex == documentFlosses.count()) { // reached the end of the list
                            qint16 stitchSymbol = symbolIndexes.takeFirst();
                            Qt::PenStyle backstitchSymbol(Qt::SolidLine);
                            QString foundName = flossScheme->find(color);

                            if (foundName.isEmpty()) {
                                colorNotFound = true;
                            }

                            DocumentFloss *documentFloss = new DocumentFloss(foundName, stitchSymbol, backstitchSymbol, Configuration::palette_StitchStrands(), Configuration::palette_BackstitchStrands());
                            documentFloss->setFlossColor(color);
                            new AddDocumentFlossCommand(m_document, flossIndex, documentFloss, importImageCommand);
                            documentFlosses.insert(flossIndex, color);
                        }

                        // at this point
                        //   flossIndex will be the index for the found color
                        if (useFractionals) {
                            int zone = (dy % 2) * 2 + (dx % 2);
                            new AddStitchCommand(m_document, QPoint(dx / 2, dy / 2), stitchMap[0][zone], flossIndex, importImageCommand);
                        } else {
                            new AddStitchCommand(m_document, QPoint(dx, dy), Stitch::Full, flossIndex, importImageCommand);
                        }
                    }
                }
            }
        }

        if (colorNotFound) {
            // Examples of imported images have missing color names
            // This will fix those that are found by changing the scheme to something else and then back to the required one
            // A fix has been introduced, but this is a final catch if there are any still found
            qDebug() << "Found a missing color name and attempting to fix";

            if (schemeName == QLatin1String("DMC")) {
                new ChangeSchemeCommand(m_document, QStringLiteral("Anchor"), importImageCommand);
            } else {
                new ChangeSchemeCommand(m_document, QStringLiteral("DMC"), importImageCommand);
            }

            new ChangeSchemeCommand(m_document, schemeName, importImageCommand);
        }

        new SetPropertyCommand(m_document, QStringLiteral("horizontalClothCount"), importImageDlg->horizontalClothCount(), importImageCommand);
        new SetPropertyCommand(m_document, QStringLiteral("verticalClothCount"), importImageDlg->verticalClothCount(), importImageCommand);
        m_document->undoStack().push(importImageCommand);
    }

    delete importImageDlg;
}


void MainWindow::convertPreview(const QString &source)
{
    QPixmap pixmap;
    pixmap.load(source);
    m_imageLabel->setPixmap(pixmap);
}


void MainWindow::fileProperties()
{
    QPointer<FilePropertiesDlg> filePropertiesDlg = new FilePropertiesDlg(this, m_document);

    if (filePropertiesDlg->exec()) {
        QUndoCommand *cmd = new FilePropertiesCommand(m_document);

        if ((filePropertiesDlg->documentWidth() != m_document->pattern()->stitches().width()) || (filePropertiesDlg->documentHeight() != m_document->pattern()->stitches().height())) {
            new ResizeDocumentCommand(m_document, filePropertiesDlg->documentWidth(), filePropertiesDlg->documentHeight(), cmd);
        }

        if (filePropertiesDlg->unitsFormat() != static_cast<Configuration::EnumDocument_UnitsFormat::type>(m_document->property(QStringLiteral("unitsFormat")).toInt())) {
            new SetPropertyCommand(m_document, QStringLiteral("unitsFormat"), QVariant(filePropertiesDlg->unitsFormat()), cmd);
        }

        if (filePropertiesDlg->horizontalClothCount() != m_document->property(QStringLiteral("horizontalClothCount")).toDouble()) {
            new SetPropertyCommand(m_document, QStringLiteral("horizontalClothCount"), QVariant(filePropertiesDlg->horizontalClothCount()), cmd);
        }

        if (filePropertiesDlg->clothCountLink() != m_document->property(QStringLiteral("clothCountLink")).toBool()) {
            new SetPropertyCommand(m_document, QStringLiteral("clothCountLink"), QVariant(filePropertiesDlg->clothCountLink()), cmd);
        }

        if (filePropertiesDlg->verticalClothCount() != m_document->property(QStringLiteral("verticalClothCount")).toDouble()) {
            new SetPropertyCommand(m_document, QStringLiteral("verticalClothCount"), QVariant(filePropertiesDlg->verticalClothCount()), cmd);
        }

        if (filePropertiesDlg->clothCountUnits() != static_cast<Configuration::EnumEditor_ClothCountUnits::type>(m_document->property(QStringLiteral("clothCountUnits")).toInt())) {
            new SetPropertyCommand(m_document, QStringLiteral("clothCountUnits"), QVariant(filePropertiesDlg->clothCountUnits()), cmd);
        }

        if (filePropertiesDlg->title() != m_document->property(QStringLiteral("title")).toString()) {
            new SetPropertyCommand(m_document, QStringLiteral("title"), QVariant(filePropertiesDlg->title()), cmd);
        }

        if (filePropertiesDlg->author() != m_document->property(QStringLiteral("author")).toString()) {
            new SetPropertyCommand(m_document, QStringLiteral("author"), QVariant(filePropertiesDlg->author()), cmd);
        }

        if (filePropertiesDlg->copyright() != m_document->property(QStringLiteral("copyright")).toString()) {
            new SetPropertyCommand(m_document, QStringLiteral("copyright"), QVariant(filePropertiesDlg->copyright()), cmd);
        }

        if (filePropertiesDlg->fabric() != m_document->property(QStringLiteral("fabric")).toString()) {
            new SetPropertyCommand(m_document, QStringLiteral("fabric"), QVariant(filePropertiesDlg->fabric()), cmd);
        }

        if (filePropertiesDlg->fabricColor() != m_document->property(QStringLiteral("fabricColor")).value<QColor>()) {
            new SetPropertyCommand(m_document, QStringLiteral("fabricColor"), QVariant(filePropertiesDlg->fabricColor()), cmd);
        }

        if (filePropertiesDlg->instructions() != m_document->property(QStringLiteral("instructions")).toString()) {
            new SetPropertyCommand(m_document, QStringLiteral("instructions"), QVariant(filePropertiesDlg->instructions()), cmd);
        }

        if (filePropertiesDlg->flossScheme() != m_document->pattern()->palette().schemeName()) {
            new ChangeSchemeCommand(m_document, filePropertiesDlg->flossScheme(), cmd);
        }

        if (cmd->childCount()) {
            m_document->undoStack().push(cmd);
        } else {
            delete cmd;
        }
    }

    delete filePropertiesDlg;
}


void MainWindow::fileAddBackgroundImage()
{
    QUrl url = QFileDialog::getOpenFileUrl(this, i18n("Background Image"), QUrl(QStandardPaths::writableLocation(QStandardPaths::HomeLocation)), i18n("Images (*.bmp *.gif *.jpg *.png *.pbm *.pgm *.ppm *.xbm *.xpm *.svg)"));

    if (!url.path().isNull()) {
        QRect patternArea(0, 0, m_document->pattern()->stitches().width(), m_document->pattern()->stitches().height());
        QRect selectionArea = m_editor->selectionArea();
        QSharedPointer<BackgroundImage> backgroundImage(new BackgroundImage(url, (selectionArea.isValid() ? selectionArea : patternArea)));

        if (backgroundImage->isValid()) {
            m_document->undoStack().push(new AddBackgroundImageCommand(m_document, backgroundImage, this));
        }
    }
}


void MainWindow::fileRemoveBackgroundImage()
{
    QAction *action = qobject_cast<QAction *>(sender());
    m_document->undoStack().push(new RemoveBackgroundImageCommand(m_document, action->data().value<QSharedPointer<BackgroundImage>>(), this));
}


void MainWindow::fileClose()
{
    if (queryClose()) {
        m_document->initialiseNew();
        setupActionsFromDocument();
        m_editor->readDocumentSettings();
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
    actionCollection()->action(QStringLiteral("edit_undo"))->setText(i18n("Undo %1", text));
}


void MainWindow::redoTextChanged(const QString &text)
{
    actionCollection()->action(QStringLiteral("edit_redo"))->setText(i18n("Redo %1", text));
}


void MainWindow::clipboardDataChanged()
{
    actionCollection()->action(QStringLiteral("edit_paste"))->setEnabled(QApplication::clipboard()->mimeData()->hasFormat(QStringLiteral("application/kxstitch")));
}


void MainWindow::paletteManager()
{
    QPointer<PaletteManagerDlg> paletteManagerDlg = new PaletteManagerDlg(this, m_document);

    if (paletteManagerDlg->exec()) {
        DocumentPalette palette = paletteManagerDlg->palette();

        if (palette != m_document->pattern()->palette()) {
            m_document->undoStack().push(new UpdateDocumentPaletteCommand(m_document, palette));
        }
    }

    delete paletteManagerDlg;
}


void MainWindow::paletteShowSymbols(bool show)
{
    m_palette->showSymbols(show);
}


void MainWindow::paletteClearUnused()
{
    QMap<int, FlossUsage> flossUsage = m_document->pattern()->stitches().flossUsage();
    QMapIterator<int, DocumentFloss *> flosses(m_document->pattern()->palette().flosses());
    ClearUnusedFlossesCommand *clearUnusedFlossesCommand = new ClearUnusedFlossesCommand(m_document);

    while (flosses.hasNext()) {
        flosses.next();

        if (flossUsage[flosses.key()].totalStitches() == 0) {
            new RemoveDocumentFlossCommand(m_document, flosses.key(), flosses.value(), clearUnusedFlossesCommand);
        }
    }

    if (clearUnusedFlossesCommand->childCount()) {
        m_document->undoStack().push(clearUnusedFlossesCommand);
    } else {
        delete clearUnusedFlossesCommand;
    }
}


void MainWindow::paletteCalibrateScheme()
{
}


void MainWindow::paletteSwapColors(int originalIndex, int replacementIndex)
{
    if (originalIndex != replacementIndex) {
        m_document->undoStack().push(new PaletteSwapColorCommand(m_document, originalIndex, replacementIndex));
    }
}


void MainWindow::paletteReplaceColor(int originalIndex, int replacementIndex)
{
    if (originalIndex != replacementIndex) {
        m_document->undoStack().push(new PaletteReplaceColorCommand(m_document, originalIndex, replacementIndex));
    }
}


void MainWindow::viewFitBackgroundImage()
{
    QAction *action = qobject_cast<QAction *>(sender());
    m_document->undoStack().push(new FitBackgroundImageCommand(m_document, action->data().value<QSharedPointer<BackgroundImage>>(), m_editor->selectionArea()));
}


void MainWindow::paletteContextMenu(const QPoint &pos)
{
    static_cast<QMenu *>(guiFactory()->container(QStringLiteral("PalettePopup"), this))->popup(qobject_cast<QWidget *>(sender())->mapToGlobal(pos));
}


void MainWindow::viewShowBackgroundImage()
{
    QAction *action = qobject_cast<QAction *>(sender());
    m_document->undoStack().push(new ShowBackgroundImageCommand(m_document, action->data().value<QSharedPointer<BackgroundImage>>(), action->isChecked()));
}


void MainWindow::patternExtend()
{
    QPointer<ExtendPatternDlg> extendPatternDlg = new ExtendPatternDlg(this);

    if (extendPatternDlg->exec()) {
        int top = extendPatternDlg->top();
        int left = extendPatternDlg->left();
        int right = extendPatternDlg->right();
        int bottom = extendPatternDlg->bottom();

        if (top || left || right || bottom) {
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


void MainWindow::insertColumns()
{
    m_document->undoStack().push(new InsertColumnsCommand(m_document, m_editor->selectionArea()));
}


void MainWindow::insertRows()
{
    m_document->undoStack().push(new InsertRowsCommand(m_document, m_editor->selectionArea()));
}


void MainWindow::preferences()
{
    if (KConfigDialog::showDialog(QStringLiteral("preferences"))) {
        return;
    }

    KConfigDialog *dialog = new KConfigDialog(this, QStringLiteral("preferences"), Configuration::self());
//    dialog->setHelp("ConfigurationDialog");
    dialog->setFaceType(KPageDialog::List);

    dialog->addPage(new EditorConfigPage(0, QStringLiteral("EditorConfigPage")), i18nc("The Editor config page", "Editor"), QStringLiteral("preferences-desktop"));
    dialog->addPage(new PatternConfigPage(0, QStringLiteral("PatternConfigPage")), i18n("Pattern"), QStringLiteral("ksnapshot"));
    PaletteConfigPage *paletteConfigPage = new PaletteConfigPage(0, QStringLiteral("PaletteConfigPage"));
    dialog->addPage(paletteConfigPage, i18n("Palette"), QStringLiteral("preferences-desktop-color"));
    connect(dialog, SIGNAL(defaultClicked()), paletteConfigPage, SLOT(defaultClicked()));
    dialog->addPage(new ImportConfigPage(0, QStringLiteral("ImportConfigPage")), i18n("Import"), QStringLiteral("insert-image"));
    dialog->addPage(new LibraryConfigPage(0, QStringLiteral("LibraryConfigPage")), i18n("Library"), QStringLiteral("accessories-dictionary"));
    dialog->addPage(new PrinterConfigPage(0, QStringLiteral("PrinterConfigPage")), i18n("Printer Configuration"), QStringLiteral("preferences-desktop-printer"));

    connect(dialog, SIGNAL(settingsChanged(QString)), this, SLOT(settingsChanged()));

    dialog->show();
}


void MainWindow::settingsChanged()
{
    QList<QUndoCommand *> documentChanges;
    ConfigurationCommand *configurationCommand = new ConfigurationCommand(this);

    if (m_document->property(QStringLiteral("cellHorizontalGrouping")) != Configuration::editor_CellHorizontalGrouping()) {
        documentChanges.append(new SetPropertyCommand(m_document, QStringLiteral("cellHorizontalGrouping"), Configuration::editor_CellHorizontalGrouping(), configurationCommand));
    }

    if (m_document->property(QStringLiteral("cellVerticalGrouping")) != Configuration::editor_CellVerticalGrouping()) {
        documentChanges.append(new SetPropertyCommand(m_document, QStringLiteral("cellVerticalGrouping"), Configuration::editor_CellVerticalGrouping(), configurationCommand));
    }

    if (m_document->property(QStringLiteral("thickLineColor")) != Configuration::editor_ThickLineColor()) {
        documentChanges.append(new SetPropertyCommand(m_document, QStringLiteral("thickLineColor"), Configuration::editor_ThickLineColor(), configurationCommand));
    }

    if (m_document->property(QStringLiteral("thinLineColor")) != Configuration::editor_ThinLineColor()) {
        documentChanges.append(new SetPropertyCommand(m_document, QStringLiteral("thinLineColor"), Configuration::editor_ThinLineColor(), configurationCommand));
    }

    if (documentChanges.count()) {
        m_document->undoStack().push(configurationCommand);
    } else {
        delete configurationCommand;
    }

    loadSettings();
}


void MainWindow::loadSettings()
{
    m_horizontalScale->setMinimumSize(0, Configuration::editor_HorizontalScaleHeight());
    m_verticalScale->setMinimumSize(Configuration::editor_VerticalScaleWidth(), 0);
    m_horizontalScale->setCellGrouping(Configuration::editor_CellHorizontalGrouping());
    m_verticalScale->setCellGrouping(Configuration::editor_CellVerticalGrouping());

    m_editor->loadSettings();
    m_preview->loadSettings();
    m_palette->loadSettings();

    KActionCollection *actions = actionCollection();

    actions->action(QStringLiteral("makesCopies"))->setChecked(Configuration::tool_MakesCopies());

    actions->action(QStringLiteral("colorHighlight"))->setChecked(Configuration::renderer_ColorHilight());

    actions->action(QStringLiteral("renderStitches"))->setChecked(Configuration::renderer_RenderStitches());
    actions->action(QStringLiteral("renderBackstitches"))->setChecked(Configuration::renderer_RenderBackstitches());
    actions->action(QStringLiteral("renderFrenchKnots"))->setChecked(Configuration::renderer_RenderFrenchKnots());
    actions->action(QStringLiteral("renderGrid"))->setChecked(Configuration::renderer_RenderGrid());
    actions->action(QStringLiteral("renderBackgroundImages"))->setChecked(Configuration::renderer_RenderBackgroundImages());

    switch (Configuration::editor_FormatScalesAs()) {
    case Configuration::EnumEditor_FormatScalesAs::Stitches:
        actions->action(QStringLiteral("formatScalesAsStitches"))->trigger();
        break;

    case Configuration::EnumEditor_FormatScalesAs::Inches:
        actions->action(QStringLiteral("formatScalesAsInches"))->trigger();
        break;

    case Configuration::EnumEditor_FormatScalesAs::CM:
        actions->action(QStringLiteral("formatScalesAsCM"))->trigger();
        break;

    default:
        break;
    }

    switch (Configuration::renderer_RenderStitchesAs()) {
    case Configuration::EnumRenderer_RenderStitchesAs::Stitches:
        actions->action(QStringLiteral("renderStitchesAsRegularStitches"))->trigger();
        break;

    case Configuration::EnumRenderer_RenderStitchesAs::BlackWhiteSymbols:
        actions->action(QStringLiteral("renderStitchesAsBlackWhiteSymbols"))->trigger();
        break;

    case Configuration::EnumRenderer_RenderStitchesAs::ColorSymbols:
        actions->action(QStringLiteral("renderStitchesAsColorSymbols"))->trigger();
        break;

    case Configuration::EnumRenderer_RenderStitchesAs::ColorBlocks:
        actions->action(QStringLiteral("renderStitchesAsColorBlocks"))->trigger();
        break;

    case Configuration::EnumRenderer_RenderStitchesAs::ColorBlocksSymbols:
        actions->action(QStringLiteral("renderStitchesAsColorBlocksSymbols"))->trigger();
        break;

    default:
        break;
    }

    switch (Configuration::renderer_RenderBackstitchesAs()) {
    case Configuration::EnumRenderer_RenderBackstitchesAs::ColorLines:
        actions->action(QStringLiteral("renderBackstitchesAsColorLines"))->trigger();
        break;

    case Configuration::EnumRenderer_RenderBackstitchesAs::BlackWhiteSymbols:
        actions->action(QStringLiteral("renderBackstitchesAsBlackWhiteSymbols"))->trigger();
        break;

    default:
        break;
    }

    switch (Configuration::renderer_RenderKnotsAs()) {
    case Configuration::EnumRenderer_RenderKnotsAs::ColorBlocks:
        actions->action(QStringLiteral("renderKnotsAsColorBlocks"))->trigger();
        break;

    case Configuration::EnumRenderer_RenderKnotsAs::ColorBlocksSymbols:
        actions->action(QStringLiteral("renderKnotsAsColorBlocksSymbols"))->trigger();
        break;

    case Configuration::EnumRenderer_RenderKnotsAs::ColorSymbols:
        actions->action(QStringLiteral("renderKnotsAsColorSymbols"))->trigger();
        break;

    case Configuration::EnumRenderer_RenderKnotsAs::BlackWhiteSymbols:
        actions->action(QStringLiteral("renderKnotsAsBlackWhiteSymbols"))->trigger();
        break;

    default:
        break;
    }

    actions->action(QStringLiteral("paletteShowSymbols"))->setChecked(Configuration::palette_ShowSymbols());
}


void MainWindow::documentModified(bool clean)
{
    setCaption(m_document->url().fileName(), !clean);
}


void MainWindow::setupActions()
{
    QAction *action;
    QActionGroup *actionGroup;

    KActionCollection *actions = actionCollection();

    // File menu
    KStandardAction::openNew(this, SLOT(fileNew()), actions);
    KStandardAction::open(this, SLOT(fileOpen()), actions);
    KStandardAction::openRecent(this, SLOT(fileOpen(QUrl)), actions)->loadEntries(KConfigGroup(KSharedConfig::openConfig(), QStringLiteral("RecentFiles")));
    KStandardAction::save(this, SLOT(fileSave()), actions);
    KStandardAction::saveAs(this, SLOT(fileSaveAs()), actions);
    KStandardAction::revert(this, SLOT(fileRevert()), actions);

    action = new QAction(this);
    action->setText(i18n("Print Setup..."));
    connect(action, SIGNAL(triggered()), this, SLOT(filePrintSetup()));
    actions->addAction(QStringLiteral("filePrintSetup"), action);

    KStandardAction::print(this, SLOT(filePrint()), actions);

    action = new QAction(this);
    action->setText(i18n("Import Image"));
    connect(action, SIGNAL(triggered()), this, SLOT(fileImportImage()));
    actions->addAction(QStringLiteral("fileImportImage"), action);

    action = new QAction(this);
    action->setText(i18n("File Properties"));
    connect(action, SIGNAL(triggered()), this, SLOT(fileProperties()));
    actions->addAction(QStringLiteral("fileProperties"), action);

    action = new QAction(this);
    action->setText(i18n("Add Background Image..."));
    connect(action, SIGNAL(triggered()), this, SLOT(fileAddBackgroundImage()));
    actions->addAction(QStringLiteral("fileAddBackgroundImage"), action);

    KStandardAction::close(this, SLOT(fileClose()), actions);
    KStandardAction::quit(this, SLOT(fileQuit()), actions);


    // Edit menu
    KStandardAction::undo(this, SLOT(editUndo()), actions);
    KStandardAction::redo(this, SLOT(editRedo()), actions);
    KStandardAction::cut(m_editor, SLOT(editCut()), actions);
    actions->action(QStringLiteral("edit_cut"))->setEnabled(false);
    KStandardAction::copy(m_editor, SLOT(editCopy()), actions);
    actions->action(QStringLiteral("edit_copy"))->setEnabled(false);
    KStandardAction::paste(m_editor, SLOT(editPaste()), actions);

    action = new QAction(this);
    action->setText(i18n("Mirror/Rotate makes copies"));
    action->setCheckable(true);
    connect(action, SIGNAL(triggered(bool)), m_editor, SLOT(setMakesCopies(bool)));
    actions->addAction(QStringLiteral("makesCopies"), action);

    action = new QAction(this);
    action->setText(i18n("Horizontally"));
    action->setData(Qt::Horizontal);
    connect(action, SIGNAL(triggered()), m_editor, SLOT(mirrorSelection()));
    action->setEnabled(false);
    actions->addAction(QStringLiteral("mirrorHorizontal"), action);

    action = new QAction(this);
    action->setText(i18n("Vertically"));
    action->setData(Qt::Vertical);
    connect(action, SIGNAL(triggered()), m_editor, SLOT(mirrorSelection()));
    action->setEnabled(false);
    actions->addAction(QStringLiteral("mirrorVertical"), action);

    action = new QAction(this);
    action->setText(i18n("90 Degrees"));
    action->setData(StitchData::Rotate90);
    connect(action, SIGNAL(triggered()), m_editor, SLOT(rotateSelection()));
    action->setEnabled(false);
    actions->addAction(QStringLiteral("rotate90"), action);

    action = new QAction(this);
    action->setText(i18n("180 Degrees"));
    action->setData(StitchData::Rotate180);
    connect(action, SIGNAL(triggered()), m_editor, SLOT(rotateSelection()));
    action->setEnabled(false);
    actions->addAction(QStringLiteral("rotate180"), action);

    action = new QAction(this);
    action->setText(i18n("270 Degrees"));
    action->setData(StitchData::Rotate270);
    connect(action, SIGNAL(triggered()), m_editor, SLOT(rotateSelection()));
    action->setEnabled(false);
    actions->addAction(QStringLiteral("rotate270"), action);

    // Selection mask sub menu
    action = new QAction(this);
    action->setText(i18n("Stitch Mask"));
    action->setCheckable(true);
    connect(action, SIGNAL(triggered(bool)), m_editor, SLOT(setMaskStitch(bool)));
    actions->addAction(QStringLiteral("maskStitch"), action);

    action = new QAction(this);
    action->setText(i18n("Color Mask"));
    action->setCheckable(true);
    connect(action, SIGNAL(triggered(bool)), m_editor, SLOT(setMaskColor(bool)));
    actions->addAction(QStringLiteral("maskColor"), action);

    action = new QAction(this);
    action->setText(i18n("Exclude Backstitches"));
    action->setCheckable(true);
    connect(action, SIGNAL(triggered(bool)), m_editor, SLOT(setMaskBackstitch(bool)));
    actions->addAction(QStringLiteral("maskBackstitch"), action);

    action = new QAction(this);
    action->setText(i18n("Exclude Knots"));
    action->setCheckable(true);
    connect(action, SIGNAL(triggered(bool)), m_editor, SLOT(setMaskKnot(bool)));
    actions->addAction(QStringLiteral("maskKnot"), action);


    // View menu
    KStandardAction::zoomIn(m_editor, SLOT(zoomIn()), actions);
    KStandardAction::zoomOut(m_editor, SLOT(zoomOut()), actions);
    KStandardAction::actualSize(m_editor, SLOT(actualSize()), actions);
    action = KStandardAction::fitToPage(m_editor, SLOT(fitToPage()), actions);
    action->setIcon(QIcon::fromTheme(QStringLiteral("zoom-fit-best")));
    action = KStandardAction::fitToWidth(m_editor, SLOT(fitToWidth()), actions);
    action->setIcon(QIcon::fromTheme(QStringLiteral("zoom-fit-width")));
    action = KStandardAction::fitToHeight(m_editor, SLOT(fitToHeight()), actions);
    action->setIcon(QIcon::fromTheme(QStringLiteral("zoom-fit-height")));

    // Entries for Show/Hide Preview and Palette dock windows are added dynamically
    // Entries for Show/Hide and Remove background images are added dynamically


    // Stitches Menu
    actionGroup = new QActionGroup(this);
    actionGroup->setExclusive(true);

    action = new QAction(this);
    action->setText(i18n("Quarter Stitch"));
    action->setData(Editor::StitchQuarter);
    action->setIcon(QIcon::fromTheme(QStringLiteral("kxstitch-quarter-stitch")));
    action->setCheckable(true);
    connect(action, SIGNAL(triggered()), m_editor, SLOT(selectStitch()));
    actions->addAction(QStringLiteral("stitchQuarter"), action);
    actionGroup->addAction(action);

    action = new QAction(this);
    action->setText(i18n("Half Stitch"));
    action->setData(Editor::StitchHalf);
    action->setIcon(QIcon::fromTheme(QStringLiteral("kxstitch-half-stitch")));
    action->setCheckable(true);
    connect(action, SIGNAL(triggered()), m_editor, SLOT(selectStitch()));
    actions->addAction(QStringLiteral("stitchHalf"), action);
    actionGroup->addAction(action);

    action = new QAction(this);
    action->setText(i18n("3 Quarter Stitch"));
    action->setData(Editor::Stitch3Quarter);
    action->setIcon(QIcon::fromTheme(QStringLiteral("kxstitch-3quarter-stitch")));
    action->setCheckable(true);
    connect(action, SIGNAL(triggered()), m_editor, SLOT(selectStitch()));
    actions->addAction(QStringLiteral("stitch3Quarter"), action);
    actionGroup->addAction(action);

    action = new QAction(this);
    action->setText(i18n("Full Stitch"));
    action->setData(Editor::StitchFull);
    action->setIcon(QIcon::fromTheme(QStringLiteral("kxstitch-full-stitch")));
    action->setCheckable(true);
    connect(action, SIGNAL(triggered()), m_editor, SLOT(selectStitch()));
    actions->addAction(QStringLiteral("stitchFull"), action);
    actionGroup->addAction(action);

    action = new QAction(this);
    action->setText(i18n("Small Half Stitch"));
    action->setData(Editor::StitchSmallHalf);
    action->setIcon(QIcon::fromTheme(QStringLiteral("kxstitch-small-half-stitch")));
    action->setCheckable(true);
    connect(action, SIGNAL(triggered()), m_editor, SLOT(selectStitch()));
    actions->addAction(QStringLiteral("stitchSmallHalf"), action);
    actionGroup->addAction(action);

    action = new QAction(this);
    action->setText(i18n("Small Full Stitch"));
    action->setData(Editor::StitchSmallFull);
    action->setIcon(QIcon::fromTheme(QStringLiteral("kxstitch-small-full-stitch")));
    action->setCheckable(true);
    connect(action, SIGNAL(triggered()), m_editor, SLOT(selectStitch()));
    actions->addAction(QStringLiteral("stitchSmallFull"), action);
    actionGroup->addAction(action);

    action = new QAction(this);
    action->setText(i18n("French Knot"));
    action->setData(Editor::StitchFrenchKnot);
    action->setIcon(QIcon::fromTheme(QStringLiteral("kxstitch-frenchknot")));
    action->setCheckable(true);
    connect(action, SIGNAL(triggered()), m_editor, SLOT(selectStitch()));
    actions->addAction(QStringLiteral("stitchFrenchKnot"), action);
    actionGroup->addAction(action);


    // Tools Menu
    actionGroup = new QActionGroup(this);
    actionGroup->setExclusive(true);

    action = new QAction(this);
    action->setText(i18n("Paint"));
    action->setData(Editor::ToolPaint);
    action->setIcon(QIcon::fromTheme(QStringLiteral("draw-brush")));
    action->setCheckable(true);
    connect(action, SIGNAL(triggered()), m_editor, SLOT(selectTool()));
    actions->addAction(QStringLiteral("toolPaint"), action);
    actionGroup->addAction(action);

    action = new QAction(this);
    action->setText(i18n("Draw"));
    action->setData(Editor::ToolDraw);
    action->setIcon(QIcon::fromTheme(QStringLiteral("draw-freehand")));
    action->setCheckable(true);
    connect(action, SIGNAL(triggered()), m_editor, SLOT(selectTool()));
    actions->addAction(QStringLiteral("toolDraw"), action);
    actionGroup->addAction(action);

    action = new QAction(this);
    action->setText(i18n("Erase"));
    action->setData(Editor::ToolErase);
    action->setIcon(QIcon::fromTheme(QStringLiteral("draw-eraser")));
    action->setCheckable(true);
    connect(action, SIGNAL(triggered()), m_editor, SLOT(selectTool()));
    actions->addAction(QStringLiteral("toolErase"), action);
    actionGroup->addAction(action);

    action = new QAction(this);
    action->setText(i18n("Draw Rectangle"));
    action->setData(Editor::ToolRectangle);
    action->setIcon(QIcon::fromTheme(QStringLiteral("draw-rectangle")));
    action->setCheckable(true);
    connect(action, SIGNAL(triggered()), m_editor, SLOT(selectTool()));
    actions->addAction(QStringLiteral("toolRectangle"), action);
    actionGroup->addAction(action);

    action = new QAction(this);
    action->setText(i18n("Fill Rectangle"));
    action->setData(Editor::ToolFillRectangle);
    action->setIcon(QIcon::fromTheme(QStringLiteral("kxstitch-draw-rectangle-filled")));
    action->setCheckable(true);
    connect(action, SIGNAL(triggered()), m_editor, SLOT(selectTool()));
    actions->addAction(QStringLiteral("toolFillRectangle"), action);
    actionGroup->addAction(action);

    action = new QAction(this);
    action->setText(i18n("Draw Ellipse"));
    action->setData(Editor::ToolEllipse);
    action->setIcon(QIcon::fromTheme(QStringLiteral("draw-ellipse")));
    action->setCheckable(true);
    connect(action, SIGNAL(triggered()), m_editor, SLOT(selectTool()));
    actions->addAction(QStringLiteral("toolEllipse"), action);
    actionGroup->addAction(action);

    action = new QAction(this);
    action->setText(i18n("Fill Ellipse"));
    action->setData(Editor::ToolFillEllipse);
    action->setIcon(QIcon::fromTheme(QStringLiteral("kxstitch-draw-ellipse-filled")));
    action->setCheckable(true);
    connect(action, SIGNAL(triggered()), m_editor, SLOT(selectTool()));
    actions->addAction(QStringLiteral("toolFillEllipse"), action);
    actionGroup->addAction(action);

    action = new QAction(this);
    action->setText(i18n("Fill Polygon"));
    action->setData(Editor::ToolFillPolygon);
    action->setIcon(QIcon::fromTheme(QStringLiteral("draw-polyline")));
    action->setCheckable(true);
    connect(action, SIGNAL(triggered()), m_editor, SLOT(selectTool()));
    actions->addAction(QStringLiteral("toolFillPolygon"), action);
    actionGroup->addAction(action);

    action = new QAction(this);
    action->setText(i18n("Text"));
    action->setData(Editor::ToolText);
    action->setIcon(QIcon::fromTheme(QStringLiteral("draw-text")));
    action->setCheckable(true);
    connect(action, SIGNAL(triggered()), m_editor, SLOT(selectTool()));
    actions->addAction(QStringLiteral("toolText"), action);
    actionGroup->addAction(action);

    action = new QAction(this);
    action->setText(i18n("Alphabet"));
    action->setData(Editor::ToolAlphabet);
    action->setIcon(QIcon::fromTheme(QStringLiteral("text-field")));
    action->setCheckable(true);
    connect(action, SIGNAL(triggered()), m_editor, SLOT(selectTool()));
    actions->addAction(QStringLiteral("toolAlphabet"), action);
    actionGroup->addAction(action);

    action = new QAction(this);
    action->setText(i18nc("Select an area of the pattern", "Select"));
    action->setData(Editor::ToolSelect);
    action->setIcon(QIcon::fromTheme(QStringLiteral("select-rectangular")));
    action->setCheckable(true);
    connect(action, SIGNAL(triggered()), m_editor, SLOT(selectTool()));
    actions->addAction(QStringLiteral("toolSelectRectangle"), action);
    actionGroup->addAction(action);

    action = new QAction(this);
    action->setText(i18n("Backstitch"));
    action->setData(Editor::ToolBackstitch);
    action->setIcon(QIcon::fromTheme(QStringLiteral("kxstitch-backstitch")));
    action->setCheckable(true);
    connect(action, SIGNAL(triggered()), m_editor, SLOT(selectTool()));
    actions->addAction(QStringLiteral("toolBackstitch"), action);
    actionGroup->addAction(action);

    action = new QAction(this);
    action->setText(i18n("Color Picker"));
    action->setData(Editor::ToolColorPicker);
    action->setIcon(QIcon::fromTheme(QStringLiteral("color-picker")));
    action->setCheckable(true);
    connect(action, SIGNAL(triggered()), m_editor, SLOT(selectTool()));
    actions->addAction(QStringLiteral("toolColorPicker"), action);
    actionGroup->addAction(action);


    // Palette Menu
    action = new QAction(this);
    action->setText(i18n("Palette Manager..."));
    action->setIcon(QIcon::fromTheme(QStringLiteral("kxstitch-color-add")));
    connect(action, SIGNAL(triggered()), this, SLOT(paletteManager()));
    actions->addAction(QStringLiteral("paletteManager"), action);

    action = new QAction(this);
    action->setText(i18n("Show Symbols"));
    action->setCheckable(true);
    connect(action, SIGNAL(toggled(bool)), this, SLOT(paletteShowSymbols(bool)));
    actions->addAction(QStringLiteral("paletteShowSymbols"), action);

    action = new QAction(this);
    action->setText(i18n("Clear Unused"));
    connect(action, SIGNAL(triggered()), this, SLOT(paletteClearUnused()));
    actions->addAction(QStringLiteral("paletteClearUnused"), action);

    action = new QAction(this);
    action->setText(i18n("Calibrate Scheme..."));
    connect(action, SIGNAL(triggered()), this, SLOT(paletteCalibrateScheme()));
    actions->addAction(QStringLiteral("paletteCalibrateScheme"), action);

    action = new QAction(this);
    action->setText(i18n("Swap Colors"));
    connect(action, SIGNAL(triggered()), m_palette, SLOT(swapColors()));
    actions->addAction(QStringLiteral("paletteSwapColors"), action);

    action = new QAction(this);
    action->setText(i18n("Replace Colors"));
    connect(action, SIGNAL(triggered()), m_palette, SLOT(replaceColor()));
    actions->addAction(QStringLiteral("paletteReplaceColor"), action);


    // Pattern Menu
    action = new QAction(this);
    action->setText(i18n("Extend Pattern..."));
    action->setIcon(QIcon::fromTheme(QStringLiteral("kxstitch-extend-pattern")));
    connect(action, SIGNAL(triggered()), this, SLOT(patternExtend()));
    actions->addAction(QStringLiteral("patternExtend"), action);

    action = new QAction(this);
    action->setText(i18n("Center Pattern"));
    action->setIcon(QIcon::fromTheme(QStringLiteral("kxstitch-center-pattern")));
    connect(action, SIGNAL(triggered()), this, SLOT(patternCentre()));
    actions->addAction(QStringLiteral("patternCentre"), action);

    action = new QAction(this);
    action->setText(i18n("Crop Canvas to Pattern"));
    connect(action, SIGNAL(triggered()), this, SLOT(patternCrop()));
    actions->addAction(QStringLiteral("patternCrop"), action);

    action = new QAction(this);
    action->setText(i18n("Crop Canvas to Selection"));
    action->setIcon(QIcon::fromTheme(QStringLiteral("transform-crop")));
    connect(action, SIGNAL(triggered()), this, SLOT(patternCropToSelection()));
    action->setEnabled(false);
    actions->addAction(QStringLiteral("patternCropToSelection"), action);

    action = new QAction(this);
    action->setText(i18n("Insert Rows"));
    connect(action, SIGNAL(triggered()), this, SLOT(insertRows()));
    action->setEnabled(false);
    actions->addAction(QStringLiteral("insertRows"), action);

    action = new QAction(this);
    action->setText(i18n("Insert Columns"));
    connect(action, SIGNAL(triggered()), this, SLOT(insertColumns()));
    action->setEnabled(false);
    actions->addAction(QStringLiteral("insertColumns"), action);


    // Library Menu
    action = new QAction(this);
    action->setText(i18n("Library Manager..."));
    connect(action, SIGNAL(triggered()), m_editor, SLOT(libraryManager()));
    actions->addAction(QStringLiteral("libraryManager"), action);

    // Settings Menu
    KStandardAction::preferences(this, SLOT(preferences()), actions);
    // formatScalesAs
    actionGroup = new QActionGroup(this);
    actionGroup->setExclusive(true);

    action = new QAction(this);
    action->setText(i18n("Stitches"));
    action->setCheckable(true);
    connect(action, SIGNAL(triggered()), m_editor, SLOT(formatScalesAsStitches()));
    actions->addAction(QStringLiteral("formatScalesAsStitches"), action);
    actionGroup->addAction(action);

    action = new QAction(this);
    action->setText(i18n("CM"));
    action->setCheckable(true);
    connect(action, SIGNAL(triggered()), m_editor, SLOT(formatScalesAsCM()));
    actions->addAction(QStringLiteral("formatScalesAsCM"), action);
    actionGroup->addAction(action);

    action = new QAction(this);
    action->setText(i18n("Inches"));
    action->setCheckable(true);
    connect(action, SIGNAL(triggered()), m_editor, SLOT(formatScalesAsInches()));
    actions->addAction(QStringLiteral("formatScalesAsInches"), action);
    actionGroup->addAction(action);

    // ShowStitchesAs
    actionGroup = new QActionGroup(this);
    actionGroup->setExclusive(true);

    action = new QAction(this);
    action->setText(i18n("Regular Stitches"));
    action->setData(Configuration::EnumRenderer_RenderStitchesAs::Stitches);
    action->setCheckable(true);
    action->setChecked(true);
    connect(action, SIGNAL(triggered()), m_editor, SLOT(renderStitchesAs()));
    actions->addAction(QStringLiteral("renderStitchesAsRegularStitches"), action);
    actionGroup->addAction(action);

    action = new QAction(this);
    action->setText(i18n("Black & White Symbols"));
    action->setData(Configuration::EnumRenderer_RenderStitchesAs::BlackWhiteSymbols);
    action->setCheckable(true);
    connect(action, SIGNAL(triggered()), m_editor, SLOT(renderStitchesAs()));
    actions->addAction(QStringLiteral("renderStitchesAsBlackWhiteSymbols"), action);
    actionGroup->addAction(action);

    action = new QAction(this);
    action->setText(i18n("Color Symbols"));
    action->setData(Configuration::EnumRenderer_RenderStitchesAs::ColorSymbols);
    action->setCheckable(true);
    connect(action, SIGNAL(triggered()), m_editor, SLOT(renderStitchesAs()));
    actions->addAction(QStringLiteral("renderStitchesAsColorSymbols"), action);
    actionGroup->addAction(action);

    action = new QAction(this);
    action->setText(i18n("Color Blocks"));
    action->setData(Configuration::EnumRenderer_RenderStitchesAs::ColorBlocks);
    action->setCheckable(true);
    connect(action, SIGNAL(triggered()), m_editor, SLOT(renderStitchesAs()));
    actions->addAction(QStringLiteral("renderStitchesAsColorBlocks"), action);
    actionGroup->addAction(action);

    action = new QAction(this);
    action->setText(i18n("Color Blocks & Symbols"));
    action->setData(Configuration::EnumRenderer_RenderStitchesAs::ColorBlocksSymbols);
    action->setCheckable(true);
    connect(action, SIGNAL(triggered()), m_editor, SLOT(renderStitchesAs()));
    actions->addAction(QStringLiteral("renderStitchesAsColorBlocksSymbols"), action);
    actionGroup->addAction(action);

    // ShowBackstitchesAs
    actionGroup = new QActionGroup(this);
    actionGroup->setExclusive(true);

    action = new QAction(this);
    action->setText(i18n("Color Lines"));
    action->setData(Configuration::EnumRenderer_RenderBackstitchesAs::ColorLines);
    action->setCheckable(true);
    action->setChecked(true);
    connect(action, SIGNAL(triggered()), m_editor, SLOT(renderBackstitchesAs()));
    actions->addAction(QStringLiteral("renderBackstitchesAsColorLines"), action);
    actionGroup->addAction(action);

    action = new QAction(this);
    action->setText(i18n("Black & White Symbols"));
    action->setData(Configuration::EnumRenderer_RenderBackstitchesAs::BlackWhiteSymbols);
    action->setCheckable(true);
    connect(action, SIGNAL(triggered()), m_editor, SLOT(renderBackstitchesAs()));
    actions->addAction(QStringLiteral("renderBackstitchesAsBlackWhiteSymbols"), action);
    actionGroup->addAction(action);

    // ShowKnotsAs
    actionGroup = new QActionGroup(this);
    actionGroup->setExclusive(true);

    action = new QAction(this);
    action->setText(i18n("Color Blocks"));
    action->setData(Configuration::EnumRenderer_RenderKnotsAs::ColorBlocks);
    action->setCheckable(true);
    action->setChecked(true);
    connect(action, SIGNAL(triggered()), m_editor, SLOT(renderKnotsAs()));
    actions->addAction(QStringLiteral("renderKnotsAsColorBlocks"), action);
    actionGroup->addAction(action);

    action = new QAction(this);
    action->setText(i18n("Color Blocks & Symbols"));
    action->setData(Configuration::EnumRenderer_RenderKnotsAs::ColorBlocksSymbols);
    action->setCheckable(true);
    connect(action, SIGNAL(triggered()), m_editor, SLOT(renderKnotsAs()));
    actions->addAction(QStringLiteral("renderKnotsAsColorBlocksSymbols"), action);
    actionGroup->addAction(action);

    action = new QAction(this);
    action->setText(i18n("Color Symbols"));
    action->setData(Configuration::EnumRenderer_RenderKnotsAs::ColorSymbols);
    action->setCheckable(true);
    connect(action, SIGNAL(triggered()), m_editor, SLOT(renderKnotsAs()));
    actions->addAction(QStringLiteral("renderKnotsAsColorSymbols"), action);
    actionGroup->addAction(action);

    action = new QAction(this);
    action->setText(i18n("Black & White Symbols"));
    action->setData(Configuration::EnumRenderer_RenderKnotsAs::BlackWhiteSymbols);
    action->setCheckable(true);
    connect(action, SIGNAL(triggered()), m_editor, SLOT(renderKnotsAs()));
    actions->addAction(QStringLiteral("renderKnotsAsBlackWhiteSymbols"), action);
    actionGroup->addAction(action);


    action = new QAction(this);
    action->setText(i18n("Color Highlight"));
    action->setCheckable(true);
    connect(action, SIGNAL(toggled(bool)), m_editor, SLOT(colorHighlight(bool)));
    actions->addAction(QStringLiteral("colorHighlight"), action);

    action = new QAction(this);
    action->setText(i18n("Show Stitches"));
    action->setCheckable(true);
    connect(action, SIGNAL(toggled(bool)), m_editor, SLOT(renderStitches(bool)));
    actions->addAction(QStringLiteral("renderStitches"), action);

    action = new QAction(this);
    action->setText(i18n("Show Backstitches"));
    action->setCheckable(true);
    connect(action, SIGNAL(toggled(bool)), m_editor, SLOT(renderBackstitches(bool)));
    actions->addAction(QStringLiteral("renderBackstitches"), action);

    action = new QAction(this);
    action->setText(i18n("Show French Knots"));
    action->setCheckable(true);
    connect(action, SIGNAL(toggled(bool)), m_editor, SLOT(renderFrenchKnots(bool)));
    actions->addAction(QStringLiteral("renderFrenchKnots"), action);

    action = new QAction(this);
    action->setText(i18n("Show Grid"));
    action->setCheckable(true);
    connect(action, SIGNAL(toggled(bool)), m_editor, SLOT(renderGrid(bool)));
    actions->addAction(QStringLiteral("renderGrid"), action);

    action = new QAction(this);
    action->setText(i18n("Show Background Images"));
    action->setCheckable(true);
    connect(action, SIGNAL(toggled(bool)), m_editor, SLOT(renderBackgroundImages(bool)));
    actions->addAction(QStringLiteral("renderBackgroundImages"), action);

    m_horizontalScale->addAction(actions->action(QStringLiteral("formatScalesAsStitches")));
    m_horizontalScale->addAction(actions->action(QStringLiteral("formatScalesAsCM")));
    m_horizontalScale->addAction(actions->action(QStringLiteral("formatScalesAsInches")));

    m_verticalScale->addAction(actions->action(QStringLiteral("formatScalesAsStitches")));
    m_verticalScale->addAction(actions->action(QStringLiteral("formatScalesAsCM")));
    m_verticalScale->addAction(actions->action(QStringLiteral("formatScalesAsInches")));

    setupGUI(KXmlGuiWindow::Default, QStringLiteral("kxstitchui.rc"));
}


void MainWindow::updateBackgroundImageActionLists()
{
    auto backgroundImages = m_document->backgroundImages().backgroundImages();

    unplugActionList(QStringLiteral("removeBackgroundImageActions"));
    unplugActionList(QStringLiteral("fitBackgroundImageActions"));
    unplugActionList(QStringLiteral("showBackgroundImageActions"));

    QList<QAction *> removeBackgroundImageActions;
    QList<QAction *> fitBackgroundImageActions;
    QList<QAction *> showBackgroundImageActions;

    while (backgroundImages.hasNext()) {
        QSharedPointer<BackgroundImage> backgroundImage = backgroundImages.next();

        QAction *action = new QAction(backgroundImage->url().fileName(), this);
        action->setData(QVariant::fromValue(backgroundImage));
        action->setIcon(backgroundImage->icon());
        connect(action, SIGNAL(triggered()), this, SLOT(fileRemoveBackgroundImage()));
        removeBackgroundImageActions.append(action);

        action = new QAction(backgroundImage->url().fileName(), this);
        action->setData(QVariant::fromValue(backgroundImage));
        action->setIcon(backgroundImage->icon());
        connect(action, SIGNAL(triggered()), this, SLOT(viewFitBackgroundImage()));
        fitBackgroundImageActions.append(action);

        action = new QAction(backgroundImage->url().fileName(), this);
        action->setData(QVariant::fromValue(backgroundImage));
        action->setIcon(backgroundImage->icon());
        action->setCheckable(true);
        action->setChecked(backgroundImage->isVisible());
        connect(action, SIGNAL(triggered()), this, SLOT(viewShowBackgroundImage()));
        showBackgroundImageActions.append(action);
    }

    plugActionList(QStringLiteral("removeBackgroundImageActions"), removeBackgroundImageActions);
    plugActionList(QStringLiteral("fitBackgroundImageActions"), fitBackgroundImageActions);
    plugActionList(QStringLiteral("showBackgroundImageActions"), showBackgroundImageActions);
}


void MainWindow::setupDockWindows()
{
    QDockWidget *dock = new QDockWidget(i18n("Preview"), this);
    dock->setObjectName(QStringLiteral("PreviewDock#"));
    dock->setAllowedAreas(Qt::AllDockWidgetAreas);
    QScrollArea *scrollArea = new QScrollArea();
    m_preview = new Preview(scrollArea);
    scrollArea->setWidget(m_preview);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    scrollArea->setMinimumSize(std::min(300, m_preview->width()), std::min(400, m_preview->height()));
    dock->setWidget(scrollArea);
    addDockWidget(Qt::LeftDockWidgetArea, dock);
    actionCollection()->addAction(QStringLiteral("showPreviewDockWidget"), dock->toggleViewAction());

    dock = new QDockWidget(i18n("Palette"), this);
    dock->setObjectName(QStringLiteral("PaletteDock#"));
    dock->setAllowedAreas(Qt::AllDockWidgetAreas);
    m_palette = new Palette(this);
    m_palette->setContextMenuPolicy(Qt::CustomContextMenu);
    dock->setWidget(m_palette);
    addDockWidget(Qt::LeftDockWidgetArea, dock);
    actionCollection()->addAction(QStringLiteral("showPaletteDockWidget"), dock->toggleViewAction());

    dock = new QDockWidget(i18n("History"), this);
    dock->setObjectName(QStringLiteral("HistoryDock#"));
    dock->setAllowedAreas(Qt::AllDockWidgetAreas);
    m_history = new QUndoView(this);
    dock->setWidget(m_history);
    addDockWidget(Qt::LeftDockWidgetArea, dock);
    actionCollection()->addAction(QStringLiteral("showHistoryDockWidget"), dock->toggleViewAction());

    dock = new QDockWidget(i18n("Imported Image"), this);
    dock->setObjectName(QStringLiteral("ImportedImage#"));
    dock->setAllowedAreas(Qt::AllDockWidgetAreas);
    m_imageLabel = new ScaledPixmapLabel(this);
    m_imageLabel->setScaledContents(false);
    dock->setWidget(m_imageLabel);
    addDockWidget(Qt::LeftDockWidgetArea, dock);
    actionCollection()->addAction(QStringLiteral("showImportedDockWidget"), dock->toggleViewAction());
}
