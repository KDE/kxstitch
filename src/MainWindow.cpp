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

    setStatusBar(nullptr);
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

    connect(&(m_document->undoStack()), &QUndoStack::canUndoChanged, actions->action(QStringLiteral("edit_undo")), &QAction::setEnabled);
    connect(&(m_document->undoStack()), &QUndoStack::canUndoChanged, actions->action(QStringLiteral("file_revert")), &QAction::setEnabled);
    connect(&(m_document->undoStack()), &QUndoStack::canRedoChanged, actions->action(QStringLiteral("edit_redo")), &QAction::setEnabled);
    connect(QApplication::clipboard(),  &QClipboard::dataChanged, this, &MainWindow::clipboardDataChanged);
    connect(m_editor, &Editor::selectionMade, actionCollection()->action(QStringLiteral("edit_cut")), &QAction::setEnabled);
    connect(m_editor, &Editor::selectionMade, actionCollection()->action(QStringLiteral("edit_copy")), &QAction::setEnabled);
    connect(m_editor, &Editor::selectionMade, actionCollection()->action(QStringLiteral("mirrorHorizontal")), &QAction::setEnabled);
    connect(m_editor, &Editor::selectionMade, actionCollection()->action(QStringLiteral("mirrorVertical")), &QAction::setEnabled);
    connect(m_editor, &Editor::selectionMade, actionCollection()->action(QStringLiteral("rotate90")), &QAction::setEnabled);
    connect(m_editor, &Editor::selectionMade, actionCollection()->action(QStringLiteral("rotate180")), &QAction::setEnabled);
    connect(m_editor, &Editor::selectionMade, actionCollection()->action(QStringLiteral("rotate270")), &QAction::setEnabled);
    connect(m_editor, &Editor::selectionMade, actionCollection()->action(QStringLiteral("patternCropToSelection")), &QAction::setEnabled);
    connect(m_editor, &Editor::selectionMade, actionCollection()->action(QStringLiteral("insertColumns")), &QAction::setEnabled);
    connect(m_editor, &Editor::selectionMade, actionCollection()->action(QStringLiteral("insertRows")), &QAction::setEnabled);
    connect(&(m_document->undoStack()), &QUndoStack::undoTextChanged, this, &MainWindow::undoTextChanged);
    connect(&(m_document->undoStack()), &QUndoStack::redoTextChanged, this, &MainWindow::redoTextChanged);
    connect(&(m_document->undoStack()), &QUndoStack::cleanChanged, this, &MainWindow::documentModified);
    connect(m_palette, &Palette::colorSelected, m_editor, static_cast<void (Editor::*)()>(&Editor::drawContents));
    connect(m_palette, static_cast<void (Palette::*)(int, int)>(&Palette::swapColors), this, &MainWindow::paletteSwapColors);
    connect(m_palette, static_cast<void (Palette::*)(int, int)>(&Palette::replaceColor), this, &MainWindow::paletteReplaceColor);
    connect(m_palette, &Palette::signalStateChanged, this, static_cast<void (KXmlGuiWindow::*)(const QString &, bool)>(&KXmlGuiWindow::slotStateChanged));
    connect(m_palette, &Palette::customContextMenuRequested, this, &MainWindow::paletteContextMenu);
    connect(m_editor,  &Editor::changedVisibleCells, m_preview, &Preview::setVisibleCells);
    connect(m_preview, static_cast<void (Preview::*)(QPoint)>(&Preview::clicked), m_editor, static_cast<void (Editor::*)(const QPoint &)>(&Editor::previewClicked));
    connect(m_preview, static_cast<void (Preview::*)(QRect)>(&Preview::clicked), m_editor, static_cast<void (Editor::*)(const QRect &)>(&Editor::previewClicked));
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
    bool docEmpty = (m_document->undoStack().isClean() && (m_document->url().toString() == i18n("Untitled")));

    if (url.isValid()) {
        if (docEmpty) {
            QTemporaryFile tmpFile;

            if (tmpFile.open()) {
                tmpFile.close();

                KIO::FileCopyJob *job = KIO::file_copy(url, QUrl::fromLocalFile(tmpFile.fileName()), -1, KIO::Overwrite);

                if (job->exec()) {
                    /* In earlier versions of KDE/Qt creating a QDataStream on tmpFile allowed reading the data from the copied file.
                     * Somewhere after KDE 5.55.0/Qt 5.9.7 this no longer possible as tmpFile size() is reported with a length of 0
                     * whereas previously tmpFile size() was reported as the size of the copied file.
                     * Therefore open a new QFile on the temporary file after downloading to allow reading.
                     */
                    QFile reader(tmpFile.fileName());
                    if (reader.open(QIODevice::ReadOnly)) {
                        QDataStream stream(&reader);

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

                        reader.close();
                    } else {
                        KMessageBox::error(nullptr, reader.errorString());
                    }
                } else {
                    KMessageBox::error(nullptr, job->errorString());
                }

                tmpFile.close();
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

    if (url.toString() == i18n("Untitled")) {
        fileSaveAs();
    } else {
        // ### Why use QUrl everywhere if this only supports local files?
        QSaveFile file(url.toLocalFile());

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

    m_printer->setPageSize(page->pageSize());
    m_printer->setPageOrientation(page->orientation());

    QPainter painter;
    painter.begin(m_printer);
    painter.setRenderHint(QPainter::Antialiasing, true);

    for (int p = 0 ; p < totalPages ;) {
        int paperWidth = PageSizes::width(page->pageSize().id(), page->orientation());
        int paperHeight = PageSizes::height(page->pageSize().id(), page->orientation());

        painter.setWindow(0, 0, paperWidth, paperHeight);

        page->render(m_document, &painter);

        if (++p < totalPages) {
            page = (m_printer->pageOrder() == QPrinter::FirstPageFirst)?pages.takeFirst():pages.takeLast();

            m_printer->setPageSize(page->pageSize());
            m_printer->setPageOrientation(page->orientation());

            m_printer->newPage();
        }
    }

    painter.end();
}


void MainWindow::fileImportImage()
{
    MainWindow *window;
    bool docEmpty = ((m_document->undoStack().isClean()) && (m_document->url().toString() == i18n("Untitled")));
    QUrl url = QFileDialog::getOpenFileUrl(this, i18n("Import Image"), QUrl::fromLocalFile(QStandardPaths::writableLocation(QStandardPaths::HomeLocation)), i18n("Images (*.bmp *.gif *.jpg *.png *.pbm *.pgm *.ppm *.xbm *.xpm *.svg)"));

    if (url.isValid()) {
        QTemporaryFile tmpFile;

        if (tmpFile.open()) {
            KIO::FileCopyJob *job = KIO::file_copy(url, QUrl::fromLocalFile(tmpFile.fileName()), -1, KIO::Overwrite);

            if (job->exec()) {
                if (docEmpty) {
                    convertImage(tmpFile.fileName());
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

/*
 * ImageMagick prior to V7 used matte (opacity) to determine if an image has transparency.
 * 0.0 for transparent to 1.0 for opaque
 *
 * ImageMagick V7 now uses alpha (transparency).
 * 1.0 for transparent to 0.0 for opaque
 *
 * Access to pixels has changed too, V7 can use pixelColor to access the color of a particular
 * pixel, but although this was available in V6, it doesn't appear to produce the same result
 * and has resulted in black images when importing.
 */
#if MagickLibVersion < 0x700
        bool hasTransparency = convertedImage.matte();
        double transparent = 1.0;
        const Magick::PixelPacket *pixels = convertedImage.getConstPixels(0, 0, imageWidth, imageHeight);
#else
        bool hasTransparency = convertedImage.alpha();
        double transparent = 0.0;
#endif

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

        for (int dy = 0 ; dy < imageHeight ; dy++) {
            progress.setValue(dy * imageWidth);
            QApplication::processEvents();

            if (progress.wasCanceled()) {
                delete importImageDlg;
                delete importImageCommand;
                return;
            }

            for (int dx = 0 ; dx < imageWidth ; dx++) {
#if MagickLibVersion < 0x700
                Magick::ColorRGB rgb = Magick::Color(*pixels++);
#else
                Magick::ColorRGB rgb = convertedImage.pixelColor(dx, dy);
#endif

                if (hasTransparency && (rgb.alpha() == transparent)) {
                    // ignore this pixel as it is transparent
                } else {
                    if (!(ignoreColor && (rgb == ignoreColorValue))) {
                        int flossIndex;
                        QColor color((int)(255*rgb.red()), (int)(255*rgb.green()), (int)(255*rgb.blue()));

                        for (flossIndex = 0 ; flossIndex < documentFlosses.count() ; ++flossIndex) {
                            if (documentFlosses[flossIndex] == color) {
                                break;
                            }
                        }

                        if (flossIndex == documentFlosses.count()) { // reached the end of the list
                            qint16 stitchSymbol = symbolIndexes.takeFirst();
                            Qt::PenStyle backstitchSymbol(Qt::SolidLine);
                            Floss *floss = flossScheme->find(color);

                            DocumentFloss *documentFloss = new DocumentFloss(floss->name(), stitchSymbol, backstitchSymbol, Configuration::palette_StitchStrands(), Configuration::palette_BackstitchStrands());
                            documentFloss->setFlossColor(floss->color());
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

        new SetPropertyCommand(m_document, QStringLiteral("horizontalClothCount"), importImageDlg->horizontalClothCount(), importImageCommand);
        new SetPropertyCommand(m_document, QStringLiteral("verticalClothCount"), importImageDlg->verticalClothCount(), importImageCommand);
        m_document->undoStack().push(importImageCommand);

        convertPreview(source, importImageDlg->croppedArea());
    }

    delete importImageDlg;
}


void MainWindow::convertPreview(const QString &source, const QRect &croppedArea)
{
    QPixmap pixmap;
    pixmap.load(source);
    pixmap = pixmap.copy(croppedArea);
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
    QUrl url = QFileDialog::getOpenFileUrl(this, i18n("Background Image"), QUrl::fromLocalFile(QStandardPaths::writableLocation(QStandardPaths::HomeLocation)), i18n("Images (*.bmp *.gif *.jpg *.png *.pbm *.pgm *.ppm *.xbm *.xpm *.svg)"));

    if (!url.isEmpty()) {
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
    dialog->setFaceType(KPageDialog::List);

    dialog->addPage(new EditorConfigPage(0, QStringLiteral("EditorConfigPage")), i18nc("The Editor config page", "Editor"), QStringLiteral("preferences-desktop"));
    dialog->addPage(new PatternConfigPage(0, QStringLiteral("PatternConfigPage")), i18n("Pattern"), QStringLiteral("ksnapshot"));
    PaletteConfigPage *paletteConfigPage = new PaletteConfigPage(0, QStringLiteral("PaletteConfigPage"));
    dialog->addPage(paletteConfigPage, i18n("Palette"), QStringLiteral("preferences-desktop-color"));
    dialog->addPage(new ImportConfigPage(0, QStringLiteral("ImportConfigPage")), i18n("Import"), QStringLiteral("insert-image"));
    dialog->addPage(new LibraryConfigPage(0, QStringLiteral("LibraryConfigPage")), i18n("Library"), QStringLiteral("accessories-dictionary"));
    dialog->addPage(new PrinterConfigPage(0, QStringLiteral("PrinterConfigPage")), i18n("Printer Configuration"), QStringLiteral("preferences-desktop-printer"));

    connect(dialog, &KConfigDialog::settingsChanged, this, &MainWindow::settingsChanged);

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

    case Configuration::EnumEditor_FormatScalesAs::Centimeters:
        actions->action(QStringLiteral("formatScalesAsCentimeters"))->trigger();
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
    KStandardAction::openNew(this, &MainWindow::fileNew, actions);
    KStandardAction::open(this, static_cast<void (MainWindow::*)()>(&MainWindow::fileOpen), actions);
    KStandardAction::openRecent(this, static_cast<void (MainWindow::*)(const QUrl &)>(&MainWindow::fileOpen), actions)->loadEntries(KConfigGroup(KSharedConfig::openConfig(), QStringLiteral("RecentFiles")));
    KStandardAction::save(this, &MainWindow::fileSave, actions);
    KStandardAction::saveAs(this, &MainWindow::fileSaveAs, actions);
    KStandardAction::revert(this, &MainWindow::fileRevert, actions);

    action = new QAction(this);
    action->setText(i18n("Print Setup..."));
    connect(action, &QAction::triggered, this, &MainWindow::filePrintSetup);
    actions->addAction(QStringLiteral("filePrintSetup"), action);

    KStandardAction::print(this, &MainWindow::filePrint, actions);

    action = new QAction(this);
    action->setText(i18n("Import Image"));
    connect(action, &QAction::triggered, this, &MainWindow::fileImportImage);
    actions->addAction(QStringLiteral("fileImportImage"), action);

    action = new QAction(this);
    action->setText(i18n("File Properties"));
    connect(action, &QAction::triggered, this, &MainWindow::fileProperties);
    actions->addAction(QStringLiteral("fileProperties"), action);

    action = new QAction(this);
    action->setText(i18n("Add Background Image..."));
    connect(action, &QAction::triggered, this, &MainWindow::fileAddBackgroundImage);
    actions->addAction(QStringLiteral("fileAddBackgroundImage"), action);

    KStandardAction::close(this, &MainWindow::fileClose, actions);
    KStandardAction::quit(this, &MainWindow::fileQuit, actions);


    // Edit menu
    KStandardAction::undo(this, &MainWindow::editUndo, actions);
    KStandardAction::redo(this, &MainWindow::editRedo, actions);
    KStandardAction::cut(m_editor, &Editor::editCut, actions);
    actions->action(QStringLiteral("edit_cut"))->setEnabled(false);
    KStandardAction::copy(m_editor, &Editor::editCopy, actions);
    actions->action(QStringLiteral("edit_copy"))->setEnabled(false);
    KStandardAction::paste(m_editor, &Editor::editPaste, actions);

    action = new QAction(this);
    action->setText(i18n("Mirror/Rotate makes copies"));
    action->setCheckable(true);
    connect(action, &QAction::triggered, m_editor, &Editor::setMakesCopies);
    actions->addAction(QStringLiteral("makesCopies"), action);

    action = new QAction(this);
    action->setText(i18n("Horizontally"));
    action->setData(Qt::Horizontal);
    connect(action, &QAction::triggered, m_editor, &Editor::mirrorSelection);
    action->setEnabled(false);
    actions->addAction(QStringLiteral("mirrorHorizontal"), action);

    action = new QAction(this);
    action->setText(i18n("Vertically"));
    action->setData(Qt::Vertical);
    connect(action, &QAction::triggered, m_editor, &Editor::mirrorSelection);
    action->setEnabled(false);
    actions->addAction(QStringLiteral("mirrorVertical"), action);

    action = new QAction(this);
    action->setText(i18n("90 Degrees"));
    action->setData(StitchData::Rotate90);
    connect(action, &QAction::triggered, m_editor, &Editor::rotateSelection);
    action->setEnabled(false);
    actions->addAction(QStringLiteral("rotate90"), action);

    action = new QAction(this);
    action->setText(i18n("180 Degrees"));
    action->setData(StitchData::Rotate180);
    connect(action, &QAction::triggered, m_editor, &Editor::rotateSelection);
    action->setEnabled(false);
    actions->addAction(QStringLiteral("rotate180"), action);

    action = new QAction(this);
    action->setText(i18n("270 Degrees"));
    action->setData(StitchData::Rotate270);
    connect(action, &QAction::triggered, m_editor, &Editor::rotateSelection);
    action->setEnabled(false);
    actions->addAction(QStringLiteral("rotate270"), action);

    // Selection mask sub menu
    action = new QAction(this);
    action->setText(i18n("Stitch Mask"));
    action->setCheckable(true);
    connect(action, &QAction::triggered, m_editor, &Editor::setMaskStitch);
    actions->addAction(QStringLiteral("maskStitch"), action);

    action = new QAction(this);
    action->setText(i18n("Color Mask"));
    action->setCheckable(true);
    connect(action, &QAction::triggered, m_editor, &Editor::setMaskColor);
    actions->addAction(QStringLiteral("maskColor"), action);

    action = new QAction(this);
    action->setText(i18n("Exclude Backstitches"));
    action->setCheckable(true);
    connect(action, &QAction::triggered, m_editor, &Editor::setMaskBackstitch);
    actions->addAction(QStringLiteral("maskBackstitch"), action);

    action = new QAction(this);
    action->setText(i18n("Exclude Knots"));
    action->setCheckable(true);
    connect(action, &QAction::triggered, m_editor, &Editor::setMaskKnot);
    actions->addAction(QStringLiteral("maskKnot"), action);


    // View menu
    KStandardAction::zoomIn(m_editor, &Editor::zoomIn, actions);
    KStandardAction::zoomOut(m_editor, &Editor::zoomOut, actions);
    KStandardAction::actualSize(m_editor, &Editor::actualSize, actions);
    action = KStandardAction::fitToPage(m_editor, &Editor::fitToPage, actions);
    action->setIcon(QIcon::fromTheme(QStringLiteral("zoom-fit-best")));
    action = KStandardAction::fitToWidth(m_editor, &Editor::fitToWidth, actions);
    action->setIcon(QIcon::fromTheme(QStringLiteral("zoom-fit-width")));
    action = KStandardAction::fitToHeight(m_editor, &Editor::fitToHeight, actions);
    action->setIcon(QIcon::fromTheme(QStringLiteral("zoom-fit-height")));

    // Entries for Show/Hide Preview and Palette dock windows are added dynamically
    // Entries for Show/Hide and Remove background images are added dynamically


    // Stitches Menu
    actionGroup = new QActionGroup(this);
    actionGroup->setExclusive(true);

    action = new QAction(this);
    action->setText(i18n("Quarter Stitch"));
    action->setIcon(QIcon::fromTheme(QStringLiteral("kxstitch-quarter-stitch")));
    action->setCheckable(true);
    connect(action, &QAction::triggered, m_editor, [=]() { m_editor->selectStitch(Editor::StitchQuarter); });
    actions->addAction(QStringLiteral("stitchQuarter"), action);
    actionGroup->addAction(action);

    action = new QAction(this);
    action->setText(i18n("Half Stitch"));
    action->setIcon(QIcon::fromTheme(QStringLiteral("kxstitch-half-stitch")));
    action->setCheckable(true);
    connect(action, &QAction::triggered, m_editor, [=]() { m_editor->selectStitch(Editor::StitchHalf); });
    actions->addAction(QStringLiteral("stitchHalf"), action);
    actionGroup->addAction(action);

    action = new QAction(this);
    action->setText(i18n("3 Quarter Stitch"));
    action->setIcon(QIcon::fromTheme(QStringLiteral("kxstitch-3quarter-stitch")));
    action->setCheckable(true);
    connect(action, &QAction::triggered, m_editor, [=]() { m_editor->selectStitch(Editor::Stitch3Quarter); });
    actions->addAction(QStringLiteral("stitch3Quarter"), action);
    actionGroup->addAction(action);

    action = new QAction(this);
    action->setText(i18n("Full Stitch"));
    action->setIcon(QIcon::fromTheme(QStringLiteral("kxstitch-full-stitch")));
    action->setCheckable(true);
    connect(action, &QAction::triggered, m_editor, [=]() { m_editor->selectStitch(Editor::StitchFull); });
    actions->addAction(QStringLiteral("stitchFull"), action);
    actionGroup->addAction(action);

    action = new QAction(this);
    action->setText(i18n("Small Half Stitch"));
    action->setIcon(QIcon::fromTheme(QStringLiteral("kxstitch-small-half-stitch")));
    action->setCheckable(true);
    connect(action, &QAction::triggered, m_editor, [=]() { m_editor->selectStitch(Editor::StitchSmallHalf); });
    actions->addAction(QStringLiteral("stitchSmallHalf"), action);
    actionGroup->addAction(action);

    action = new QAction(this);
    action->setText(i18n("Small Full Stitch"));
    action->setIcon(QIcon::fromTheme(QStringLiteral("kxstitch-small-full-stitch")));
    action->setCheckable(true);
    connect(action, &QAction::triggered, m_editor, [=]() { m_editor->selectStitch(Editor::StitchSmallFull); });
    actions->addAction(QStringLiteral("stitchSmallFull"), action);
    actionGroup->addAction(action);

    action = new QAction(this);
    action->setText(i18n("French Knot"));
    action->setIcon(QIcon::fromTheme(QStringLiteral("kxstitch-frenchknot")));
    action->setCheckable(true);
    connect(action, &QAction::triggered, m_editor, [=]() { m_editor->selectStitch(Editor::StitchFrenchKnot); });
    actions->addAction(QStringLiteral("stitchFrenchKnot"), action);
    actionGroup->addAction(action);


    // Tools Menu
    actionGroup = new QActionGroup(this);
    actionGroup->setExclusive(true);

    action = new QAction(this);
    action->setText(i18n("Paint"));
    action->setIcon(QIcon::fromTheme(QStringLiteral("draw-brush")));
    action->setCheckable(true);
    connect(action, &QAction::triggered, m_editor, [=]() {m_editor->selectTool(Editor::ToolPaint);});
    actions->addAction(QStringLiteral("toolPaint"), action);
    actionGroup->addAction(action);

    action = new QAction(this);
    action->setText(i18n("Draw"));
    action->setIcon(QIcon::fromTheme(QStringLiteral("draw-freehand")));
    action->setCheckable(true);
    connect(action, &QAction::triggered, m_editor, [=]() {m_editor->selectTool(Editor::ToolDraw);});
    actions->addAction(QStringLiteral("toolDraw"), action);
    actionGroup->addAction(action);

    action = new QAction(this);
    action->setText(i18n("Erase"));
    action->setIcon(QIcon::fromTheme(QStringLiteral("draw-eraser")));
    action->setCheckable(true);
    connect(action, &QAction::triggered, m_editor, [=]() {m_editor->selectTool(Editor::ToolErase);});
    actions->addAction(QStringLiteral("toolErase"), action);
    actionGroup->addAction(action);

    action = new QAction(this);
    action->setText(i18n("Draw Rectangle"));
    action->setIcon(QIcon::fromTheme(QStringLiteral("draw-rectangle")));
    action->setCheckable(true);
    connect(action, &QAction::triggered, m_editor, [=]() {m_editor->selectTool(Editor::ToolRectangle);});
    actions->addAction(QStringLiteral("toolRectangle"), action);
    actionGroup->addAction(action);

    action = new QAction(this);
    action->setText(i18n("Fill Rectangle"));
    action->setIcon(QIcon::fromTheme(QStringLiteral("kxstitch-draw-rectangle-filled")));
    action->setCheckable(true);
    connect(action, &QAction::triggered, m_editor, [=]() {m_editor->selectTool(Editor::ToolFillRectangle);});
    actions->addAction(QStringLiteral("toolFillRectangle"), action);
    actionGroup->addAction(action);

    action = new QAction(this);
    action->setText(i18n("Draw Ellipse"));
    action->setIcon(QIcon::fromTheme(QStringLiteral("draw-ellipse")));
    action->setCheckable(true);
    connect(action, &QAction::triggered, m_editor, [=]() {m_editor->selectTool(Editor::ToolEllipse);});
    actions->addAction(QStringLiteral("toolEllipse"), action);
    actionGroup->addAction(action);

    action = new QAction(this);
    action->setText(i18n("Fill Ellipse"));
    action->setIcon(QIcon::fromTheme(QStringLiteral("kxstitch-draw-ellipse-filled")));
    action->setCheckable(true);
    connect(action, &QAction::triggered, m_editor, [=]() {m_editor->selectTool(Editor::ToolFillEllipse);});
    actions->addAction(QStringLiteral("toolFillEllipse"), action);
    actionGroup->addAction(action);

    action = new QAction(this);
    action->setText(i18n("Fill Polygon"));
    action->setIcon(QIcon::fromTheme(QStringLiteral("draw-polyline")));
    action->setCheckable(true);
    connect(action, &QAction::triggered, m_editor, [=]() {m_editor->selectTool(Editor::ToolFillPolygon);});
    actions->addAction(QStringLiteral("toolFillPolygon"), action);
    actionGroup->addAction(action);

    action = new QAction(this);
    action->setText(i18n("Text"));
    action->setIcon(QIcon::fromTheme(QStringLiteral("draw-text")));
    action->setCheckable(true);
    connect(action, &QAction::triggered, m_editor, [=]() {m_editor->selectTool(Editor::ToolText);});
    actions->addAction(QStringLiteral("toolText"), action);
    actionGroup->addAction(action);

    action = new QAction(this);
    action->setText(i18n("Alphabet"));
    action->setIcon(QIcon::fromTheme(QStringLiteral("text-field")));
    action->setCheckable(true);
    connect(action, &QAction::triggered, m_editor, [=]() {m_editor->selectTool(Editor::ToolAlphabet);});
    actions->addAction(QStringLiteral("toolAlphabet"), action);
    actionGroup->addAction(action);

    action = new QAction(this);
    action->setText(i18nc("Select an area of the pattern", "Select"));
    action->setIcon(QIcon::fromTheme(QStringLiteral("select-rectangular")));
    action->setCheckable(true);
    connect(action, &QAction::triggered, m_editor, [=]() {m_editor->selectTool(Editor::ToolSelect);});
    actions->addAction(QStringLiteral("toolSelectRectangle"), action);
    actionGroup->addAction(action);

    action = new QAction(this);
    action->setText(i18n("Backstitch"));
    action->setIcon(QIcon::fromTheme(QStringLiteral("kxstitch-backstitch")));
    action->setCheckable(true);
    connect(action, &QAction::triggered, m_editor, [=]() {m_editor->selectTool(Editor::ToolBackstitch);});
    actions->addAction(QStringLiteral("toolBackstitch"), action);
    actionGroup->addAction(action);

    action = new QAction(this);
    action->setText(i18n("Color Picker"));
    action->setIcon(QIcon::fromTheme(QStringLiteral("color-picker")));
    action->setCheckable(true);
    connect(action, &QAction::triggered, m_editor, [=]() {m_editor->selectTool(Editor::ToolColorPicker);});
    actions->addAction(QStringLiteral("toolColorPicker"), action);
    actionGroup->addAction(action);


    // Palette Menu
    action = new QAction(this);
    action->setText(i18n("Palette Manager..."));
    action->setIcon(QIcon::fromTheme(QStringLiteral("kxstitch-color-add")));
    connect(action, &QAction::triggered, this, &MainWindow::paletteManager);
    actions->addAction(QStringLiteral("paletteManager"), action);

    action = new QAction(this);
    action->setText(i18n("Show Symbols"));
    action->setCheckable(true);
    connect(action, &QAction::toggled, this, &MainWindow::paletteShowSymbols);
    actions->addAction(QStringLiteral("paletteShowSymbols"), action);

    action = new QAction(this);
    action->setText(i18n("Clear Unused"));
    connect(action, &QAction::triggered, this, &MainWindow::paletteClearUnused);
    actions->addAction(QStringLiteral("paletteClearUnused"), action);

    action = new QAction(this);
    action->setText(i18n("Calibrate Scheme..."));
    connect(action, &QAction::triggered, this, &MainWindow::paletteCalibrateScheme);
    actions->addAction(QStringLiteral("paletteCalibrateScheme"), action);

    action = new QAction(this);
    action->setText(i18n("Swap Colors"));
    connect(action, &QAction::triggered, m_palette, static_cast<void (Palette::*)()>(&Palette::swapColors));
    actions->addAction(QStringLiteral("paletteSwapColors"), action);

    action = new QAction(this);
    action->setText(i18n("Replace Colors"));
    connect(action, &QAction::triggered, m_palette, static_cast<void (Palette::*)()>(&Palette::replaceColor));
    actions->addAction(QStringLiteral("paletteReplaceColor"), action);


    // Pattern Menu
    action = new QAction(this);
    action->setText(i18n("Extend Pattern..."));
    action->setIcon(QIcon::fromTheme(QStringLiteral("kxstitch-extend-pattern")));
    connect(action, &QAction::triggered, this, &MainWindow::patternExtend);
    actions->addAction(QStringLiteral("patternExtend"), action);

    action = new QAction(this);
    action->setText(i18n("Center Pattern"));
    action->setIcon(QIcon::fromTheme(QStringLiteral("kxstitch-center-pattern")));
    connect(action, &QAction::triggered, this, &MainWindow::patternCentre);
    actions->addAction(QStringLiteral("patternCentre"), action);

    action = new QAction(this);
    action->setText(i18n("Crop Canvas to Pattern"));
    connect(action, &QAction::triggered, this, &MainWindow::patternCrop);
    actions->addAction(QStringLiteral("patternCrop"), action);

    action = new QAction(this);
    action->setText(i18n("Crop Canvas to Selection"));
    action->setIcon(QIcon::fromTheme(QStringLiteral("transform-crop")));
    connect(action, &QAction::triggered, this, &MainWindow::patternCropToSelection);
    action->setEnabled(false);
    actions->addAction(QStringLiteral("patternCropToSelection"), action);

    action = new QAction(this);
    action->setText(i18n("Insert Rows"));
    connect(action, &QAction::triggered, this, &MainWindow::insertRows);
    action->setEnabled(false);
    actions->addAction(QStringLiteral("insertRows"), action);

    action = new QAction(this);
    action->setText(i18n("Insert Columns"));
    connect(action, &QAction::triggered, this, &MainWindow::insertColumns);
    action->setEnabled(false);
    actions->addAction(QStringLiteral("insertColumns"), action);


    // Library Menu
    action = new QAction(this);
    action->setText(i18n("Library Manager..."));
    connect(action, &QAction::triggered, m_editor, &Editor::libraryManager);
    actions->addAction(QStringLiteral("libraryManager"), action);

    // Settings Menu
    KStandardAction::preferences(this, &MainWindow::preferences, actions);
    // formatScalesAs
    actionGroup = new QActionGroup(this);
    actionGroup->setExclusive(true);

    action = new QAction(this);
    action->setText(i18n("Stitches"));
    action->setCheckable(true);
    connect(action, &QAction::triggered, m_editor, &Editor::formatScalesAsStitches);
    actions->addAction(QStringLiteral("formatScalesAsStitches"), action);
    actionGroup->addAction(action);

    action = new QAction(this);
    action->setText(i18n("Centimeters"));
    action->setCheckable(true);
    connect(action, &QAction::triggered, m_editor, &Editor::formatScalesAsCentimeters);
    actions->addAction(QStringLiteral("formatScalesAsCentimeters"), action);
    actionGroup->addAction(action);

    action = new QAction(this);
    action->setText(i18n("Inches"));
    action->setCheckable(true);
    connect(action, &QAction::triggered, m_editor, &Editor::formatScalesAsInches);
    actions->addAction(QStringLiteral("formatScalesAsInches"), action);
    actionGroup->addAction(action);

    // ShowStitchesAs
    actionGroup = new QActionGroup(this);
    actionGroup->setExclusive(true);

    action = new QAction(this);
    action->setText(i18n("Regular Stitches"));
    action->setCheckable(true);
    action->setChecked(true);
    connect(action, &QAction::triggered, m_editor, [=]() {m_editor->renderStitchesAs(Configuration::EnumRenderer_RenderStitchesAs::Stitches);});
    actions->addAction(QStringLiteral("renderStitchesAsRegularStitches"), action);
    actionGroup->addAction(action);

    action = new QAction(this);
    action->setText(i18n("Black & White Symbols"));
    action->setCheckable(true);
    connect(action, &QAction::triggered, m_editor, [=]() {m_editor->renderStitchesAs(Configuration::EnumRenderer_RenderStitchesAs::BlackWhiteSymbols);});
    actions->addAction(QStringLiteral("renderStitchesAsBlackWhiteSymbols"), action);
    actionGroup->addAction(action);

    action = new QAction(this);
    action->setText(i18n("Color Symbols"));
    action->setCheckable(true);
    connect(action, &QAction::triggered, m_editor, [=]() {m_editor->renderStitchesAs(Configuration::EnumRenderer_RenderStitchesAs::ColorSymbols);});
    actions->addAction(QStringLiteral("renderStitchesAsColorSymbols"), action);
    actionGroup->addAction(action);

    action = new QAction(this);
    action->setText(i18n("Color Blocks"));
    action->setCheckable(true);
    connect(action, &QAction::triggered, m_editor, [=]() {m_editor->renderStitchesAs(Configuration::EnumRenderer_RenderStitchesAs::ColorBlocks);});
    actions->addAction(QStringLiteral("renderStitchesAsColorBlocks"), action);
    actionGroup->addAction(action);

    action = new QAction(this);
    action->setText(i18n("Color Blocks & Symbols"));
    action->setCheckable(true);
    connect(action, &QAction::triggered, m_editor, [=]() {m_editor->renderStitchesAs(Configuration::EnumRenderer_RenderStitchesAs::ColorBlocksSymbols);});
    actions->addAction(QStringLiteral("renderStitchesAsColorBlocksSymbols"), action);
    actionGroup->addAction(action);

    // ShowBackstitchesAs
    actionGroup = new QActionGroup(this);
    actionGroup->setExclusive(true);

    action = new QAction(this);
    action->setText(i18n("Color Lines"));
    action->setCheckable(true);
    action->setChecked(true);
    connect(action, &QAction::triggered, m_editor, [=]() {m_editor->renderBackstitchesAs(Configuration::EnumRenderer_RenderBackstitchesAs::ColorLines);});
    actions->addAction(QStringLiteral("renderBackstitchesAsColorLines"), action);
    actionGroup->addAction(action);

    action = new QAction(this);
    action->setText(i18n("Black & White Symbols"));
    action->setCheckable(true);
    connect(action, &QAction::triggered, m_editor, [=]() {m_editor->renderBackstitchesAs(Configuration::EnumRenderer_RenderBackstitchesAs::BlackWhiteSymbols);});
    actions->addAction(QStringLiteral("renderBackstitchesAsBlackWhiteSymbols"), action);
    actionGroup->addAction(action);

    // ShowKnotsAs
    actionGroup = new QActionGroup(this);
    actionGroup->setExclusive(true);

    action = new QAction(this);
    action->setText(i18n("Color Blocks"));
    action->setCheckable(true);
    action->setChecked(true);
    connect(action, &QAction::triggered, m_editor, [=]() {m_editor->renderKnotsAs(Configuration::EnumRenderer_RenderKnotsAs::ColorBlocks);});
    actions->addAction(QStringLiteral("renderKnotsAsColorBlocks"), action);
    actionGroup->addAction(action);

    action = new QAction(this);
    action->setText(i18n("Color Blocks & Symbols"));
    action->setCheckable(true);
    connect(action, &QAction::triggered, m_editor, [=]() {m_editor->renderKnotsAs(Configuration::EnumRenderer_RenderKnotsAs::ColorBlocksSymbols);});
    actions->addAction(QStringLiteral("renderKnotsAsColorBlocksSymbols"), action);
    actionGroup->addAction(action);

    action = new QAction(this);
    action->setText(i18n("Color Symbols"));
    action->setCheckable(true);
    connect(action, &QAction::triggered, m_editor, [=]() {m_editor->renderKnotsAs(Configuration::EnumRenderer_RenderKnotsAs::ColorSymbols);});
    actions->addAction(QStringLiteral("renderKnotsAsColorSymbols"), action);
    actionGroup->addAction(action);

    action = new QAction(this);
    action->setText(i18n("Black & White Symbols"));
    action->setCheckable(true);
    connect(action, &QAction::triggered, m_editor, [=]() {m_editor->renderKnotsAs(Configuration::EnumRenderer_RenderKnotsAs::BlackWhiteSymbols);});
    actions->addAction(QStringLiteral("renderKnotsAsBlackWhiteSymbols"), action);
    actionGroup->addAction(action);


    action = new QAction(this);
    action->setText(i18n("Color Highlight"));
    action->setCheckable(true);
    connect(action, &QAction::toggled, m_editor, &Editor::colorHighlight);
    actions->addAction(QStringLiteral("colorHighlight"), action);

    action = new QAction(this);
    action->setText(i18n("Show Stitches"));
    action->setCheckable(true);
    connect(action, &QAction::toggled, m_editor, static_cast<void (Editor::*)(bool)>(&Editor::renderStitches));
    actions->addAction(QStringLiteral("renderStitches"), action);

    action = new QAction(this);
    action->setText(i18n("Show Backstitches"));
    action->setCheckable(true);
    connect(action, &QAction::toggled, m_editor, static_cast<void (Editor::*)(bool)>(&Editor::renderBackstitches));
    actions->addAction(QStringLiteral("renderBackstitches"), action);

    action = new QAction(this);
    action->setText(i18n("Show French Knots"));
    action->setCheckable(true);
    connect(action, &QAction::toggled, m_editor, static_cast<void (Editor::*)(bool)>(&Editor::renderFrenchKnots));
    actions->addAction(QStringLiteral("renderFrenchKnots"), action);

    action = new QAction(this);
    action->setText(i18n("Show Grid"));
    action->setCheckable(true);
    connect(action, &QAction::toggled, m_editor, static_cast<void (Editor::*)(bool)>(&Editor::renderGrid));
    actions->addAction(QStringLiteral("renderGrid"), action);

    action = new QAction(this);
    action->setText(i18n("Show Background Images"));
    action->setCheckable(true);
    connect(action, &QAction::toggled, m_editor, static_cast<void (Editor::*)(bool)>(&Editor::renderBackgroundImages));
    actions->addAction(QStringLiteral("renderBackgroundImages"), action);

    m_horizontalScale->addAction(actions->action(QStringLiteral("formatScalesAsStitches")));
    m_horizontalScale->addAction(actions->action(QStringLiteral("formatScalesAsCentimeters")));
    m_horizontalScale->addAction(actions->action(QStringLiteral("formatScalesAsInches")));

    m_verticalScale->addAction(actions->action(QStringLiteral("formatScalesAsStitches")));
    m_verticalScale->addAction(actions->action(QStringLiteral("formatScalesAsCentimeters")));
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
        connect(action, &QAction::triggered, this, &MainWindow::fileRemoveBackgroundImage);
        removeBackgroundImageActions.append(action);

        action = new QAction(backgroundImage->url().fileName(), this);
        action->setData(QVariant::fromValue(backgroundImage));
        action->setIcon(backgroundImage->icon());
        connect(action, &QAction::triggered, this, &MainWindow::viewFitBackgroundImage);
        fitBackgroundImageActions.append(action);

        action = new QAction(backgroundImage->url().fileName(), this);
        action->setData(QVariant::fromValue(backgroundImage));
        action->setIcon(backgroundImage->icon());
        action->setCheckable(true);
        action->setChecked(backgroundImage->isVisible());
        connect(action, &QAction::triggered, this, &MainWindow::viewShowBackgroundImage);
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
