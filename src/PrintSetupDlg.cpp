/********************************************************************************
 *	Copyright (C) 2010 by Stephen Allewell					*
 *	stephen@mirramar.adsl24.co.uk						*
 *										*
 *	This program is free software; you can redistribute it and/or modify	*
 *	it under the terms of the GNU General Public License as published by	*
 *	the Free Software Foundation; either version 2 of the License, or	*
 *	(at your option) any later version.					*
 ********************************************************************************/


#include "PrintSetupDlg.h"

#include <QFontDialog>
#include <QInputDialog>
#include <QMenu>
#include <QPainter>
#include <QPrinter>
#include <QShowEvent>

#include <KColorDialog>
#include <KDebug>

#include "Document.h"
#include "Element.h"
#include "KeyElementDlg.h"
#include "Page.h"
#include "PageLayoutEditor.h"
#include "PagePropertiesDlg.h"
#include "PaperSizes.h"
#include "PatternElementDlg.h"
#include "TextElementDlg.h"


static const double zoomFactors[] = {0.25, 0.5, 1.0, 1.5, 2.0, 4.0, 8.0, 16.0};


PrintSetupDlg::PrintSetupDlg(QWidget *parent, Document *document, QPrinter *printer)
	:	KDialog(parent),
		m_elementUnderCursor(0),
		m_document(document),
		m_printer(printer),
		m_printerConfiguration(document->printerConfiguration())
{
	setCaption(i18n("Print Setup"));
	setButtons(KDialog::Ok | KDialog::Cancel | KDialog::Help);
	setHelp("PrinterDialog");

	QWidget *widget = new QWidget(this);
	ui.setupUi(widget);
	m_pageLayoutEditor = new PageLayoutEditor(ui.PagePreview, m_document);
	ui.PagePreview->setWidget(m_pageLayoutEditor);

	ui.AddPage->setIcon(KIcon("document-new"));
	ui.InsertPage->setIcon(KIcon("document-import"));
	ui.DeletePage->setIcon(KIcon("document-close"));
	ui.SelectElement->setIcon(KIcon("edit-select"));
	ui.TextElement->setIcon(KIcon("insert-text"));
	ui.PatternElement->setIcon(KIcon("insert-table"));
	ui.KeyElement->setIcon(KIcon("documentation"));

	m_buttonGroup.addButton(ui.SelectElement);
	m_buttonGroup.addButton(ui.TextElement);
	m_buttonGroup.addButton(ui.PatternElement);
	m_buttonGroup.addButton(ui.KeyElement);
	m_buttonGroup.setExclusive(true);

	QMetaObject::connectSlotsByName(this);
	connect(m_pageLayoutEditor, SIGNAL(selectionMade(const QRect &)), this, SLOT(selectionMade(const QRect &)));
	connect(m_pageLayoutEditor, SIGNAL(elementGeometryChanged()), this, SLOT(elementGeometryChanged()));
	connect(m_pageLayoutEditor, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(previewContextMenuRequested(const QPoint &)));

	ui.PaperSize->setCurrentIndex(Configuration::page_Size());
	ui.Orientation->setCurrentIndex(Configuration::page_Orientation());
	ui.Zoom->setCurrentIndex(2);		// 100%
	ui.SelectElement->click();		// select mode

	setMainWidget(widget);

	m_printerConfiguration.detach();
}


PrintSetupDlg::~PrintSetupDlg()
{
	delete m_pageLayoutEditor;
}


PrinterConfiguration PrintSetupDlg::printerConfiguration() const
{
	return m_printerConfiguration;
}


void PrintSetupDlg::showEvent(QShowEvent *event)
{
	KDialog::showEvent(event);

	if (ui.Pages->count() == 0)
		initialiseFromConfig();

	if (ui.Pages->count())
		ui.Pages->setCurrentRow(0);
}


void PrintSetupDlg::on_PaperSize_currentIndexChanged(const QString &paperSize)
{
	if (ui.Pages->count() && ui.Pages->currentItem())
	{
		PagePreviewListWidgetItem *pagePreview = static_cast<PagePreviewListWidgetItem *>(ui.Pages->currentItem());
		pagePreview->setPaperSize(PaperSizes::size(paperSize));
		m_pageLayoutEditor->updatePagePreview();
		ui.Pages->repaint();
	}
}


void PrintSetupDlg::on_Orientation_currentIndexChanged(int orientation)
{
	if (ui.Pages->count() && ui.Pages->currentItem())
	{
		PagePreviewListWidgetItem *pagePreview = static_cast<PagePreviewListWidgetItem *>(ui.Pages->currentItem());
		pagePreview->setOrientation((orientation == 0)?QPrinter::Portrait:QPrinter::Landscape);
		m_pageLayoutEditor->updatePagePreview();
		ui.Pages->repaint();
	}
}


void PrintSetupDlg::on_Zoom_currentIndexChanged(int zoomIndex)
{
	if (ui.Pages->currentRow() != -1)
	{
		PagePreviewListWidgetItem *pagePreview = static_cast<PagePreviewListWidgetItem *>(ui.Pages->currentItem());
		pagePreview->setZoomFactor(zoomFactors[zoomIndex]);
		m_pageLayoutEditor->updatePagePreview();
	}
}


void PrintSetupDlg::on_Pages_currentItemChanged(QListWidgetItem *current, QListWidgetItem *)
{
	if (current)
	{
		PagePreviewListWidgetItem *pagePreview = static_cast<PagePreviewListWidgetItem *>(current);
		m_pageLayoutEditor->setPagePreview(pagePreview);
		ui.PaperSize->setCurrentItem(PaperSizes::name(pagePreview->paperSize()));
		ui.Orientation->setCurrentIndex((pagePreview->orientation() == QPrinter::Portrait)?0:1);
		for (int i = 0 ; i < 6 ; i++)
		{
			if (zoomFactors[i] == m_pageLayoutEditor->zoomFactor())
			{
				ui.Zoom->setCurrentIndex(i);
				break;
			}
		}
		ui.InsertPage->setEnabled(true);
		ui.DeletePage->setEnabled(true);
	}
	else
	{
		m_pageLayoutEditor->setPagePreview(0);
		ui.InsertPage->setEnabled(false);
		ui.DeletePage->setEnabled(false);
	}
}


void PrintSetupDlg::on_AddPage_clicked()
{
	Page *page = new Page(selectedPaperSize(), selectedOrientation(), selectedZoom());
	m_printerConfiguration.addPage(page);
	addPage(ui.Pages->count(), page);
}


void PrintSetupDlg::on_InsertPage_clicked()
{
	Page *page = new Page(selectedPaperSize(), selectedOrientation(), selectedZoom());
	m_printerConfiguration.insertPage(ui.Pages->currentRow(), page);
	addPage(ui.Pages->currentRow(), page);
}


void PrintSetupDlg::on_DeletePage_clicked()
{
	if (ui.Pages->count() && ui.Pages->currentItem())
	{
		int currentRow = ui.Pages->row(ui.Pages->currentItem());
		PagePreviewListWidgetItem *pagePreview = static_cast<PagePreviewListWidgetItem *>(ui.Pages->takeItem(currentRow));
		m_printerConfiguration.removePage(pagePreview->page());
		delete pagePreview->page();
		delete pagePreview;
		if (int rows = ui.Pages->count())
			ui.Pages->setCurrentRow(std::min(currentRow, rows-1));
		else
			on_Pages_currentItemChanged(0, pagePreview);
	}
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


void PrintSetupDlg::on_KeyElement_clicked()
{
	m_elementMode = Key;
	m_pageLayoutEditor->setSelecting(false);
}


void PrintSetupDlg::selectionMade(const QRect &rectangle)
{
	PagePreviewListWidgetItem *pagePreview = static_cast<PagePreviewListWidgetItem *>(ui.Pages->currentItem());
	Page *page = pagePreview->page();

	PatternElement *patternElement;

	switch (m_elementMode)
	{
		case Text:
			page->addElement(new TextElement(page, rectangle));
			break;

		case Pattern:
			patternElement = new PatternElement(page, rectangle);
			page->addElement(patternElement);
			patternElement->setPatternRect(QRect(0, 0, m_document->pattern()->stitches().width(), m_document->pattern()->stitches().height()));
			break;

		case Key:
			page->addElement(new KeyElement(page, rectangle));
			break;
	}

	pagePreview->generatePreviewIcon();
	m_pageLayoutEditor->update();
}


void PrintSetupDlg::elementGeometryChanged()
{
	if (ui.Pages->currentItem())
	{
		PagePreviewListWidgetItem *pagePreview = static_cast<PagePreviewListWidgetItem *>(ui.Pages->currentItem());
		pagePreview->generatePreviewIcon();
	}
}


void PrintSetupDlg::previewContextMenuRequested(const QPoint &pos)
{
	PagePreviewListWidgetItem *pagePreview = static_cast<PagePreviewListWidgetItem *>(ui.Pages->currentItem());
	if (pagePreview)
	{
		m_elementUnderCursor = pagePreview->page()->itemAt(m_pageLayoutEditor->unscale(pos));
		QMenu *contextMenu = new QMenu(this);
		contextMenu->addAction(i18n("Properties"), this, SLOT(properties()));
		if (m_elementUnderCursor)
		{
//			contextMenu->addAction(i18n("Hide"), this, SLOT(hideElement()));
			contextMenu->addSeparator();
			contextMenu->addAction(i18n("Delete Element"), this, SLOT(deleteElement()));
		}
		contextMenu->popup(m_pageLayoutEditor->mapToGlobal(pos));
	}
}


void PrintSetupDlg::properties()
{
	PagePreviewListWidgetItem *pagePreview = static_cast<PagePreviewListWidgetItem *>(ui.Pages->currentItem());

	bool updatePreviews = false;

	if (m_elementUnderCursor)
	{
		if (m_elementUnderCursor->type() == Element::Text)
		{
			QPointer<TextElementDlg> textElementDlg = new TextElementDlg(this, static_cast<TextElement *>(m_elementUnderCursor));
			updatePreviews = (textElementDlg->exec() == QDialog::Accepted);
			delete textElementDlg;
		}

		if (m_elementUnderCursor->type() == Element::Pattern)
		{
			QList<QRect> patternRects;
			QListIterator<Page *> pageIterator(m_printerConfiguration.pages());
			while (pageIterator.hasNext())
			{
				Page *page = pageIterator.next();
				QListIterator<Element *> elementIterator(page->elements());
				while (elementIterator.hasNext())
				{
					Element *element = elementIterator.next();
					if (element->type() == Element::Pattern)
					{
						patternRects.append(static_cast<PatternElement *>(element)->patternRect());
					}
				}
			}
			QPointer<PatternElementDlg> patternElementDlg = new PatternElementDlg(this, static_cast<PatternElement *>(m_elementUnderCursor), m_document, patternRects);
			if (patternElementDlg->exec() == QDialog::Accepted)
			{
				updatePreviews = true;
				if (patternElementDlg->showPlan() && !pagePreview->page()->elements().contains(patternElementDlg->planElement()))
					pagePreview->page()->addElement(patternElementDlg->planElement());
				else if (!patternElementDlg->showPlan() && pagePreview->page()->elements().contains(patternElementDlg->planElement()))
					pagePreview->page()->removeElement(patternElementDlg->planElement());
			}
			delete patternElementDlg;
		}

		if (m_elementUnderCursor->type() == Element::Key)
		{
			QPointer<KeyElementDlg> keyElementDlg = new KeyElementDlg(this, static_cast<KeyElement *>(m_elementUnderCursor));
			updatePreviews = (keyElementDlg->exec() == QDialog::Accepted);
			delete keyElementDlg;
		}
	}
	else
	{
		QPointer<PagePropertiesDlg> pagePropertiesDlg = new PagePropertiesDlg(this, pagePreview);
		updatePreviews = (pagePropertiesDlg->exec() == QDialog::Accepted);
	}

	if (updatePreviews)
	{
//		PagePreviewListWidgetItem *pagePreview = static_cast<PagePreviewListWidgetItem *>(ui.Pages->currentItem());
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
	while (pageIterator.hasNext())
		addPage(ui.Pages->count(), pageIterator.next());
}


void PrintSetupDlg::addPage(int position, Page *page)
{
	PagePreviewListWidgetItem *pagePreview = new PagePreviewListWidgetItem(m_document, page);
	ui.Pages->insertItem(position, pagePreview);
	ui.Pages->setCurrentRow(position);
}


QPrinter::PaperSize PrintSetupDlg::selectedPaperSize()
{
	return PaperSizes::size(ui.PaperSize->currentText());
}


QPrinter::Orientation PrintSetupDlg::selectedOrientation()
{
	return (ui.Orientation->currentIndex() == 0)?QPrinter::Portrait:QPrinter::Landscape;
}


double PrintSetupDlg::selectedZoom()
{
	return zoomFactors[ui.Zoom->currentIndex()];
}
