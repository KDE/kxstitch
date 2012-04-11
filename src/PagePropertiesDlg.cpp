/********************************************************************************
 *	Copyright (C) 2010 by Stephen Allewell					*
 *	stephen@mirramar.adsl24.co.uk						*
 *										*
 *	This program is free software; you can redistribute it and/or modify	*
 *	it under the terms of the GNU General Public License as published by	*
 *	the Free Software Foundation; either version 2 of the License, or	*
 *	(at your option) any later version.					*
 ********************************************************************************/


#include "PagePropertiesDlg.h"

#include "Page.h"
#include "PagePreviewListWidgetItem.h"


PagePropertiesDlg::PagePropertiesDlg(QWidget *parent, PagePreviewListWidgetItem *pagePreview)
	:	KDialog(parent),
		m_pagePreview(pagePreview)
{
	setCaption(i18n("Page Properties"));
	setButtons(KDialog::Ok | KDialog::Cancel);
	setHelp("PrinterDialog");
	QWidget *widget = new QWidget(this);
	ui.setupUi(widget);
	QMetaObject::connectSlotsByName(this);

	Page *page = pagePreview->page();
	ui.MarginTop->setValue(page->m_margins.top());
	ui.MarginLeft->setValue(page->m_margins.left());
	ui.MarginRight->setValue(page->m_margins.right());
	ui.MarginBottom->setValue(page->m_margins.bottom());

	ui.ShowGrid->setChecked(page->m_showGrid);
	ui.GridX->setValue(page->m_gridX);
	ui.GridY->setValue(page->m_gridY);

	setMainWidget(widget);
}


PagePropertiesDlg::~PagePropertiesDlg()
{
}


void PagePropertiesDlg::slotButtonClicked(int button)
{
	if (button == KDialog::Ok)
	{
		Page *page = m_pagePreview->page();
		page->m_margins = QMargins(ui.MarginTop->value(), ui.MarginLeft->value(), ui.MarginRight->value(), ui.MarginBottom->value());
		page->m_showGrid = ui.ShowGrid->isChecked();
		page->m_gridX = ui.GridX->value();
		page->m_gridY = ui.GridY->value();
		accept();
	}
	else
		KDialog::slotButtonClicked(button);
}


