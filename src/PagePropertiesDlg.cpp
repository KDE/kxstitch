/*
 * Copyright (C) 2010 by Stephen Allewell
 * stephen@mirramar.adsl24.co.uk
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#include "PagePropertiesDlg.h"

#include <KDebug>

#include "Page.h"
#include "PageLayoutEditor.h"
#include "PagePreviewListWidgetItem.h"


PagePropertiesDlg::PagePropertiesDlg(QWidget *parent, const QMargins &margins, bool showGrid, int gridSize)
    :   KDialog(parent)
{
    setCaption(i18n("Page Properties"));
    setButtons(KDialog::Ok | KDialog::Cancel);
    setHelp("PrinterDialog");
    QWidget *widget = new QWidget(this);
    ui.setupUi(widget);
    QMetaObject::connectSlotsByName(this);

    ui.MarginTop->setValue(margins.top());
    ui.MarginLeft->setValue(margins.left());
    ui.MarginRight->setValue(margins.right());
    ui.MarginBottom->setValue(margins.bottom());

    ui.ShowGrid->setChecked(showGrid);
    ui.GridSize->setValue(gridSize);

    setMainWidget(widget);
}


PagePropertiesDlg::~PagePropertiesDlg()
{
}


void PagePropertiesDlg::slotButtonClicked(int button)
{
    if (button == KDialog::Ok) {
        accept();
    } else {
        KDialog::slotButtonClicked(button);
    }
}


QMargins PagePropertiesDlg::margins() const
{
    return QMargins(ui.MarginLeft->value(), ui.MarginTop->value(), ui.MarginRight->value(), ui.MarginBottom->value());
}


bool PagePropertiesDlg::showGrid() const
{
    return ui.ShowGrid->isChecked();
}


int PagePropertiesDlg::gridSize() const
{
    return ui.GridSize->value();
}
