/*
 * Copyright (C) 2010-2015 by Stephen Allewell
 * steve.allewell@gmail.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#include "PrintSetupDlg.h"

#include <math.h>

#include <QColorDialog>
#include <QFontDialog>
#include <QHideEvent>
#include <QInputDialog>
#include <QMargins>
#include <QMenu>
#include <QPainter>
#include <QPrinter>
#include <QShowEvent>

#include <KHelpClient>
#include <KLocalizedString>
#include <KMessageBox>

#include "Document.h"
#include "Element.h"
#include "ImageElementDlg.h"
#include "KeyElementDlg.h"
#include "Page.h"
#include "PageLayoutEditor.h"
#include "PagePropertiesDlg.h"
#include "PaperSizes.h"
#include "PatternElementDlg.h"
#include "TextElementDlg.h"


static const double zoomFactors[] = {0.25, 0.5, 1.0, 1.5, 2.0, 4.0, 8.0, 16.0};


PrintSetupDlg::PrintSetupDlg(QWidget *parent, Document *document, QPrinter *printer)
    :   QDialog(parent),
        m_printerConfiguration(document->printerConfiguration()),
        m_elementUnderCursor(0),
        m_document(document),
        m_printer(printer)
{
    setWindowModality(Qt::WindowModal);

    setWindowTitle(i18n("Print Setup"));

    ui.setupUi(this);
    m_pageLayoutEditor = new PageLayoutEditor(ui.PagePreview, m_document);
    ui.PagePreview->setWidget(m_pageLayoutEditor);

    ui.AddPage->setIcon(QIcon::fromTheme(QStringLiteral("document-new")));
    ui.InsertPage->setIcon(QIcon::fromTheme(QStringLiteral("document-import")));
    ui.DeletePage->setIcon(QIcon::fromTheme(QStringLiteral("document-close")));
    ui.SelectElement->setIcon(QIcon::fromTheme(QStringLiteral("edit-select")));
    ui.TextElement->setIcon(QIcon::fromTheme(QStringLiteral("insert-text")));
    ui.PatternElement->setIcon(QIcon::fromTheme(QStringLiteral("insert-table")));
    ui.ImageElement->setIcon(QIcon::fromTheme(QStringLiteral("insert-image")));
    ui.KeyElement->setIcon(QIcon::fromTheme(QStringLiteral("documentation")));

    m_buttonGroup.addButton(ui.SelectElement);
    m_buttonGroup.addButton(ui.TextElement);
    m_buttonGroup.addButton(ui.PatternElement);
    m_buttonGroup.addButton(ui.ImageElement);
    m_buttonGroup.addButton(ui.KeyElement);
    m_buttonGroup.setExclusive(true);

    connect(m_pageLayoutEditor, &PageLayoutEditor::selectionMade, this, &PrintSetupDlg::selectionMade);
    connect(m_pageLayoutEditor, &PageLayoutEditor::elementGeometryChanged, this, &PrintSetupDlg::elementGeometryChanged);
    connect(m_pageLayoutEditor, &PageLayoutEditor::customContextMenuRequested, this, &PrintSetupDlg::previewContextMenuRequested);

    ui.PageSize->setCurrentIndex(Configuration::page_Size());
    ui.Orientation->setCurrentIndex(Configuration::page_Orientation());
    ui.Zoom->setCurrentIndex(4);    // 200%
    ui.SelectElement->click();      // select mode
}


PrintSetupDlg::~PrintSetupDlg()
{
    delete m_pageLayoutEditor;
}


const PrinterConfiguration &PrintSetupDlg::printerConfiguration() const
{
    return m_printerConfiguration;
}


void PrintSetupDlg::hideEvent(QHideEvent *event)
{
    KConfigGroup(KSharedConfig::openConfig(), QStringLiteral("DialogSizes")).writeEntry(QStringLiteral("PrintSetupDlg"), size());

    QDialog::hideEvent(event);
}


void PrintSetupDlg::showEvent(QShowEvent *event)
{
    QDialog::showEvent(event);

    if (KConfigGroup(KSharedConfig::openConfig(), QStringLiteral("DialogSizes")).hasKey(QStringLiteral("PrintSetupDlg"))) {
        resize(KConfigGroup(KSharedConfig::openConfig(), QStringLiteral("DialogSizes")).readEntry(QStringLiteral("PrintSetupDlg"), QSize()));
    }

    if (ui.Pages->count() == 0) {
        initialiseFromConfig();
    }

    if (ui.Pages->count()) {
        ui.Pages->setCurrentRow(0);
    } else {
        on_AddPage_clicked();
    }
}


void PrintSetupDlg::on_PageSize_currentIndexChanged(const QString &pageSize)
{
    if (ui.Pages->count() && ui.Pages->currentItem()) {
        PagePreviewListWidgetItem *pagePreview = static_cast<PagePreviewListWidgetItem *>(ui.Pages->currentItem());
        pagePreview->setPageSize(QPageSize(PageSizes::size(pageSize)));
        m_pageLayoutEditor->updatePagePreview();
        ui.Pages->repaint();
    }
}


void PrintSetupDlg::on_Orientation_currentIndexChanged(int orientation)
{
    if (ui.Pages->count() && ui.Pages->currentItem()) {
        PagePreviewListWidgetItem *pagePreview = static_cast<PagePreviewListWidgetItem *>(ui.Pages->currentItem());
        pagePreview->setOrientation((orientation == 0) ? QPageLayout::Portrait : QPageLayout::Landscape);
        m_pageLayoutEditor->updatePagePreview();
        ui.Pages->repaint();
    }
}


void PrintSetupDlg::on_Zoom_currentIndexChanged(int zoomIndex)
{
    m_pageLayoutEditor->setZoomFactor(zoomFactors[zoomIndex]);
    m_pageLayoutEditor->updatePagePreview();
}


void PrintSetupDlg::on_Pages_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous)
{
    Q_UNUSED(previous);

    if (current) {
        PagePreviewListWidgetItem *pagePreview = static_cast<PagePreviewListWidgetItem *>(current);
        m_pageLayoutEditor->setPagePreview(pagePreview);
        ui.PageSize->setCurrentItem(PageSizes::name(pagePreview->pageSize().id()));
        ui.Orientation->setCurrentIndex((pagePreview->orientation() == QPageLayout::Portrait) ? 0 : 1);
        ui.InsertPage->setEnabled(true);
        ui.DeletePage->setEnabled(true);
    } else {
        m_pageLayoutEditor->setPagePreview(nullptr);
        ui.InsertPage->setEnabled(false);
        ui.DeletePage->setEnabled(false);
    }
}


void PrintSetupDlg::on_AddPage_clicked()
{
    Page *page = new Page(selectedPageSize(), selectedOrientation());
    m_printerConfiguration.addPage(page);
    addPage(ui.Pages->count(), page);
}


void PrintSetupDlg::on_InsertPage_clicked()
{
    Page *page = new Page(selectedPageSize(), selectedOrientation());
    m_printerConfiguration.insertPage(ui.Pages->currentRow(), page);
    addPage(ui.Pages->currentRow(), page);
}


void PrintSetupDlg::on_DeletePage_clicked()
{
    if (ui.Pages->count() && ui.Pages->currentItem()) {
        int currentRow = ui.Pages->row(ui.Pages->currentItem());
        PagePreviewListWidgetItem *pagePreview = static_cast<PagePreviewListWidgetItem *>(ui.Pages->takeItem(currentRow));
        m_printerConfiguration.removePage(pagePreview->page());
        delete pagePreview->page();
        delete pagePreview;

        if (int rows = ui.Pages->count()) {
            ui.Pages->setCurrentRow(std::min(currentRow, rows - 1));
        } else {
            on_Pages_currentItemChanged(0, 0);
        }
    }

    updatePageNumbers();
}


void PrintSetupDlg::on_SelectElement_clicked()
{
    m_elementMode = Select;
    m_pageLayoutEditor->setSelecting(true);
}


void PrintSetupDlg::on_TextElement_clicked()
{
    m_elementMode = Text;
    m_pageLayoutEditor->setSelecting(false);
}


void PrintSetupDlg::on_PatternElement_clicked()
{
    m_elementMode = Pattern;
    m_pageLayoutEditor->setSelecting(false);
}


void PrintSetupDlg::on_ImageElement_clicked()
{
    m_elementMode = Image;
    m_pageLayoutEditor->setSelecting(false);
}


void PrintSetupDlg::on_KeyElement_clicked()
{
    m_elementMode = Key;
    m_pageLayoutEditor->setSelecting(false);
}


void PrintSetupDlg::on_DialogButtonBox_accepted()
{
    accept();
}


void PrintSetupDlg::on_DialogButtonBox_rejected()
{
    reject();
}


void PrintSetupDlg::on_DialogButtonBox_helpRequested()
{
    KHelpClient::invokeHelp(QStringLiteral("PrinterDialog"), QStringLiteral("kxstitch"));
}


void PrintSetupDlg::selectionMade(const QRect &rectangle)
{
    PagePreviewListWidgetItem *pagePreview = static_cast<PagePreviewListWidgetItem *>(ui.Pages->currentItem());
    Page *page = pagePreview->page();

    PatternElement *patternElement;
    ImageElement *imageElement;

    switch (m_elementMode) {
    case Text:
        page->addElement(new TextElement(page, rectangle));
        break;

    case Pattern:
        patternElement = new PatternElement(page, rectangle);
        page->addElement(patternElement);
        patternElement->setPatternRect(QRect(0, 0, m_document->pattern()->stitches().width(), m_document->pattern()->stitches().height()));
        break;

    case Image:
        imageElement = new ImageElement(page, rectangle);
        page->addElement(imageElement);
        imageElement->setPatternRect(QRect(0, 0, m_document->pattern()->stitches().width(), m_document->pattern()->stitches().height()));
        break;

    case Key:
        page->addElement(new KeyElement(page, rectangle));
        break;

    default:
        break;
    }

    pagePreview->generatePreviewIcon();
    m_pageLayoutEditor->update();

    ui.SelectElement->click();      // select mode
}


void PrintSetupDlg::elementGeometryChanged()
{
    if (ui.Pages->currentItem()) {
        PagePreviewListWidgetItem *pagePreview = static_cast<PagePreviewListWidgetItem *>(ui.Pages->currentItem());
        pagePreview->generatePreviewIcon();
    }
}


void PrintSetupDlg::previewContextMenuRequested(const QPoint &pos)
{
    PagePreviewListWidgetItem *pagePreview = static_cast<PagePreviewListWidgetItem *>(ui.Pages->currentItem());

    if (pagePreview) {
        m_elementUnderCursor = pagePreview->page()->itemAt(m_pageLayoutEditor->unscale(pos));
        QMenu *contextMenu = new QMenu(this);

        if (m_elementUnderCursor == nullptr || m_elementUnderCursor->type() != Element::Plan) {
            contextMenu->addAction(i18n("Properties"), this, &PrintSetupDlg::properties);

            if (m_elementUnderCursor) {
                contextMenu->addSeparator();
                contextMenu->addAction(i18n("Delete Element"), this, &PrintSetupDlg::deleteElement);
            }
        }

        if (contextMenu->isEmpty()) {
            delete contextMenu;
        } else {
            contextMenu->popup(m_pageLayoutEditor->mapToGlobal(pos));
        }
    }
}


void PrintSetupDlg::properties()
{
    PagePreviewListWidgetItem *pagePreview = static_cast<PagePreviewListWidgetItem *>(ui.Pages->currentItem());

    bool updatePreviews = false;

    if (m_elementUnderCursor) {
        if (m_elementUnderCursor->type() == Element::Text) {
            QPointer<TextElementDlg> textElementDlg = new TextElementDlg(this, static_cast<TextElement *>(m_elementUnderCursor));
            updatePreviews = (textElementDlg->exec() == QDialog::Accepted);
            delete textElementDlg;
        }

        if (m_elementUnderCursor->type() == Element::Pattern) {
            QMap<int, QList<QRect> > patternRects;
            QListIterator<Page *> pageIterator(m_printerConfiguration.pages());

            while (pageIterator.hasNext()) {
                Page *page = pageIterator.next();
                QListIterator<Element *> elementIterator(page->elements());

                while (elementIterator.hasNext()) {
                    Element *element = elementIterator.next();

                    if (element->type() == Element::Pattern) {
                        patternRects[page->pageNumber()].append(static_cast<PatternElement *>(element)->patternRect());
                    }
                }
            }

            QPointer<PatternElementDlg> patternElementDlg = new PatternElementDlg(this, static_cast<PatternElement *>(m_elementUnderCursor), m_document, patternRects);

            if (patternElementDlg->exec() == QDialog::Accepted) {
                updatePreviews = true;

                if (patternElementDlg->showPlan() && !pagePreview->page()->elements().contains(patternElementDlg->planElement())) {
                    pagePreview->page()->addElement(patternElementDlg->planElement());
                } else if (!patternElementDlg->showPlan() && pagePreview->page()->elements().contains(patternElementDlg->planElement())) {
                    pagePreview->page()->removeElement(patternElementDlg->planElement());
                }
            }

            delete patternElementDlg;
        }

        if (m_elementUnderCursor->type() == Element::Image) {
            QPointer<ImageElementDlg> imageElementDlg = new ImageElementDlg(this, static_cast<ImageElement *>(m_elementUnderCursor), m_document);
            updatePreviews = (imageElementDlg->exec() == QDialog::Accepted);
            delete imageElementDlg;
        }

        if (m_elementUnderCursor->type() == Element::Key) {
            QPointer<KeyElementDlg> keyElementDlg = new KeyElementDlg(this, static_cast<KeyElement *>(m_elementUnderCursor));
            updatePreviews = (keyElementDlg->exec() == QDialog::Accepted);
            delete keyElementDlg;
        }
    } else {
        QPointer<PagePropertiesDlg> pagePropertiesDlg = new PagePropertiesDlg(this, pagePreview->page()->margins().toMargins(), m_pageLayoutEditor->showGrid(), m_pageLayoutEditor->gridSize());

        if (pagePropertiesDlg->exec() == QDialog::Accepted) {
            updatePreviews = true;
            pagePreview->page()->setMargins(pagePropertiesDlg->margins());
            m_pageLayoutEditor->setShowGrid(pagePropertiesDlg->showGrid());
            m_pageLayoutEditor->setGridSize(pagePropertiesDlg->gridSize());
        }
    }

    if (updatePreviews) {
        PagePreviewListWidgetItem *pagePreview = static_cast<PagePreviewListWidgetItem *>(ui.Pages->currentItem());
        m_pageLayoutEditor->update();
        pagePreview->generatePreviewIcon();
    }
}


void PrintSetupDlg::deleteElement()
{
    PagePreviewListWidgetItem *pagePreview = static_cast<PagePreviewListWidgetItem *>(ui.Pages->currentItem());
    pagePreview->page()->removeElement(m_elementUnderCursor);
    m_pageLayoutEditor->update();
    pagePreview->generatePreviewIcon();
}


void PrintSetupDlg::initialiseFromConfig()
{
    QListIterator<Page *> pageIterator(m_printerConfiguration.pages());

    while (pageIterator.hasNext()) {
        addPage(ui.Pages->count(), pageIterator.next());
    }
}


void PrintSetupDlg::addPage(int position, Page *page)
{
    PagePreviewListWidgetItem *pagePreview = new PagePreviewListWidgetItem(m_document, page);
    ui.Pages->insertItem(position, pagePreview);
    ui.Pages->setCurrentRow(position);

    updatePageNumbers();
}


QPageSize PrintSetupDlg::selectedPageSize()
{
    return QPageSize(PageSizes::size(ui.PageSize->currentText()));
}


QPageLayout::Orientation PrintSetupDlg::selectedOrientation()
{
    return (ui.Orientation->currentIndex() == 0) ? QPageLayout::Portrait : QPageLayout::Landscape;
}


double PrintSetupDlg::selectedZoom()
{
    return zoomFactors[ui.Zoom->currentIndex()];
}


void PrintSetupDlg::updatePageNumbers()
{
    for (int i = 0 ; i < ui.Pages->count() ; ++i) {
        PagePreviewListWidgetItem *pagePreview = dynamic_cast<PagePreviewListWidgetItem *>(ui.Pages->item(i));
        pagePreview->setText(i18n("Page %1 of %2", pagePreview->page()->pageNumber(), ui.Pages->count()));
    }
}


void PrintSetupDlg::on_Templates_clicked()
{
//    QString templateName = qobject_cast<QAction *>(sender())->data().toString();

    bool okToCreate = true;

    if (!m_printerConfiguration.isEmpty()) {
        okToCreate = (KMessageBox::questionYesNo(this, i18n("Overwrite the current configuration?"), i18n("Overwrite")) == KMessageBox::Yes);
    }

    if (okToCreate) {
        // TODO this section is currently a hack to allow creation of a printerConfiguration
        // based on the KDE3 version of KXStitch.  It is intended to create a template based
        // functionality so that various templates can be created and selected as required.

        // clear the current configuration
        m_printerConfiguration = PrinterConfiguration();
        ui.Pages->clear();
        // create pages based on template
        // Cover sheet
        QFont titleFont;
        titleFont.setPointSize(16);
        QFont pageFont;
        pageFont.setPointSize(8);

        Page *page = new Page(QPageSize(QPageSize::A4), QPageLayout::Portrait);
        m_printerConfiguration.addPage(page);
        addPage(ui.Pages->count(), page);

        QRect printableArea(page->margins().left(), page->margins().top(), 210 - page->margins().left() - page->margins().right(), 297 - page->margins().top() - page->margins().bottom());
        QRect headerTitleArea(printableArea.topLeft(), QSize(printableArea.width(), 10));
        QRect patternArea(headerTitleArea.bottomLeft() + QPoint(0, 5), QSize(printableArea.width(), printableArea.height() - 10 - 35));
        QRect mapArea(patternArea.bottomRight() - QPoint(30, 5), QSize(30, 30));
        QRect pageNumberArea(printableArea.bottomLeft() - QPoint(0, 10), QSize(printableArea.width(), 10));

        Element *element = new TextElement(page, QRect(printableArea.topLeft(), QSize(printableArea.width(), 30)));
        static_cast<TextElement *>(element)->setTextFont(titleFont);
        static_cast<TextElement *>(element)->setAlignment(Qt::AlignCenter);
        static_cast<TextElement *>(element)->setText(QStringLiteral("${title}"));
        page->addElement(element);

        element = new ImageElement(page, QRect(printableArea.topLeft() + QPoint(0, 60), QSize(printableArea.width(), printableArea.height() / 3)));
        static_cast<ImageElement *>(element)->setPatternRect(QRect(0, 0, m_document->pattern()->stitches().width(), m_document->pattern()->stitches().height()));
        static_cast<ImageElement *>(element)->setShowBorder(true);
        static_cast<ImageElement *>(element)->setBorderThickness(2);
        page->addElement(element);

        element = new TextElement(page, QRect(printableArea.topLeft() + QPoint(20, 60 + printableArea.height() / 3 + 5), QSize(printableArea.width() - 40, 60)));
        static_cast<TextElement *>(element)->setAlignment(Qt::AlignTop | Qt::AlignLeft);
        static_cast<TextElement *>(element)->setText(i18nc("Do not translate elements in ${...} which are substituted by properties", "Created by ${author}\n(C) ${copyright}\n\nPattern Size: ${width.stitches} x ${height.stitches}\nFlosses from the ${scheme} range."));
        page->addElement(element);

        element = new TextElement(page, pageNumberArea);
        static_cast<TextElement *>(element)->setTextFont(pageFont);
        static_cast<TextElement *>(element)->setAlignment(Qt::AlignCenter);
        static_cast<TextElement *>(element)->setText(i18nc("Do not translate elements in ${...} which are substituted by properties", "Page ${page}"));
        page->addElement(element);

        // pattern sheets
        int documentWidth = m_document->pattern()->stitches().width();
        int documentHeight = m_document->pattern()->stitches().height();

        // calculate the aspect ratio and the size of the cells to fit within the rectangle and the overall paint area size
        double cellWidth = Configuration::patternElement_MinimumCellSize(); // mm
        double aspect =  m_document->property(QStringLiteral("horizontalClothCount")).toDouble() / m_document->property(QStringLiteral("verticalClothCount")).toDouble();
        double cellHeight = cellWidth * aspect;

        if (cellHeight < cellWidth) {
            cellHeight = cellWidth;
            cellWidth = cellHeight / aspect;
        }

        // at this point, based on a minimum preferred cell size a physical width and height have been calculated
        // which can be used to calculate the number of pages wide and tall that the pattern needs to occupy
        // based on the size of the pattern area on the page.
        int horizontalCellsPerPage = int(double(patternArea.width()) / cellWidth);
        int verticalCellsPerPage = int(double(patternArea.height()) / cellHeight);

        int pagesWide = (documentWidth / horizontalCellsPerPage) + (((documentWidth % horizontalCellsPerPage) < Configuration::patternElement_MinimumOverflow()) ? 0 : 1);
        int pagesTall = (documentHeight / verticalCellsPerPage) + (((documentHeight % verticalCellsPerPage) < Configuration::patternElement_MinimumOverflow()) ? 0 : 1);

        // the number of pages wide and tall has been calculated based on the minimum cell size.
        // try and make use of the available space across the pages expanding the cell size to fill the space.
        horizontalCellsPerPage = (documentWidth / pagesWide) + ((documentWidth % pagesWide) ? 1 : 0);               // equal number per page
        verticalCellsPerPage = (documentHeight / pagesTall) + ((documentHeight % pagesTall) ? 1 : 0);               // equal number per page

        for (int verticalPage = 0 ; verticalPage < pagesTall ; ++verticalPage) {
            for (int horizontalPage = 0 ; horizontalPage < pagesWide ; ++horizontalPage) {
                page = new Page(QPageSize(QPageSize::A4), QPageLayout::Portrait);
                m_printerConfiguration.addPage(page);
                addPage(ui.Pages->count(), page);

                element = new TextElement(page, headerTitleArea);
                static_cast<TextElement *>(element)->setTextFont(titleFont);
                static_cast<TextElement *>(element)->setAlignment(Qt::AlignCenter);
                static_cast<TextElement *>(element)->setText(QStringLiteral("${title}"));
                page->addElement(element);

                element = new PatternElement(page, patternArea);
                static_cast<PatternElement *>(element)->setPatternRect(QRect(horizontalPage * horizontalCellsPerPage, verticalPage * verticalCellsPerPage, std::min(horizontalCellsPerPage, documentWidth - (horizontalCellsPerPage * horizontalPage)), std::min(verticalCellsPerPage, documentHeight - (verticalCellsPerPage * verticalPage))));
                static_cast<PatternElement *>(element)->setShowScales(true);
                static_cast<PatternElement *>(element)->setRenderStitchesAs(Configuration::EnumRenderer_RenderStitchesAs::BlackWhiteSymbols);
                static_cast<PatternElement *>(element)->setRenderBackstitchesAs(Configuration::EnumRenderer_RenderBackstitchesAs::BlackWhiteSymbols);
                static_cast<PatternElement *>(element)->setRenderKnotsAs(Configuration::EnumRenderer_RenderKnotsAs::BlackWhiteSymbols);
                page->addElement(element);

                element = new TextElement(page, pageNumberArea);
                static_cast<TextElement *>(element)->setTextFont(pageFont);
                static_cast<TextElement *>(element)->setAlignment(Qt::AlignCenter);
                static_cast<TextElement *>(element)->setText(i18nc("Do not translate elements in ${...} which are substituted by properties", "Page ${page}"));
                page->addElement(element);
            }
        }

        // key page
        page = new Page(QPageSize(QPageSize::A4), QPageLayout::Portrait);
        m_printerConfiguration.addPage(page);
        addPage(ui.Pages->count(), page);

        element = new TextElement(page, headerTitleArea);
        static_cast<TextElement *>(element)->setTextFont(titleFont);
        static_cast<TextElement *>(element)->setAlignment(Qt::AlignCenter);
        static_cast<TextElement *>(element)->setText(QStringLiteral("${title}"));
        page->addElement(element);

        element = new KeyElement(page, patternArea.adjusted(20, 0, -20, 0));
        page->addElement(element);

        element = new TextElement(page, pageNumberArea);
        static_cast<TextElement *>(element)->setTextFont(titleFont);
        static_cast<TextElement *>(element)->setAlignment(Qt::AlignCenter);
        static_cast<TextElement *>(element)->setText(i18nc("Do not translate elements in ${...} which are substituted by properties", "Page ${page}"));
        page->addElement(element);

        for (int i = 0 ; i < ui.Pages->count() ; ++i) {
            static_cast<PagePreviewListWidgetItem *>(ui.Pages->item(i))->generatePreviewIcon();
        }
    }
}
