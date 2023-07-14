/*
 * Copyright (C) 2010-2015 by Stephen Allewell
 * steve.allewell@gmail.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#include "PagePropertiesDlg.h"

#include <KConfigGroup>
#include <KHelpClient>
#include <KLocalizedString>
#include <KSharedConfig>

#include "Page.h"
#include "PageLayoutEditor.h"
#include "PagePreviewListWidgetItem.h"


PagePropertiesDlg::PagePropertiesDlg(QWidget *parent, const QMargins &margins, bool showGrid, int gridSize)
    :   QDialog(parent)
{
    setWindowTitle(i18n("Page Properties"));
    ui.setupUi(this);

    ui.MarginTop->setValue(margins.top());
    ui.MarginLeft->setValue(margins.left());
    ui.MarginRight->setValue(margins.right());
    ui.MarginBottom->setValue(margins.bottom());

    ui.ShowGrid->setChecked(showGrid);
    ui.GridSize->setValue(gridSize);
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


void PagePropertiesDlg::hideEvent(QHideEvent *event)
{
    KConfigGroup(KSharedConfig::openConfig(), QStringLiteral("DialogSizes")).writeEntry(QStringLiteral("PagePropertiesDlg"), size());

    QDialog::hideEvent(event);
}


void PagePropertiesDlg::showEvent(QShowEvent *event)
{
    QDialog::showEvent(event);

    if (KConfigGroup(KSharedConfig::openConfig(), QStringLiteral("DialogSizes")).hasKey(QStringLiteral("PagePropertiesDlg"))) {
        resize(KConfigGroup(KSharedConfig::openConfig(), QStringLiteral("DialogSizes")).readEntry(QStringLiteral("PagePropertiesDlg"), QSize()));
    }
}


void PagePropertiesDlg::on_DialogButtonBox_accepted()
{
    accept();
}


void PagePropertiesDlg::on_DialogButtonBox_rejected()
{
    reject();
}


void PagePropertiesDlg::on_DialogButtonBox_helpRequested()
{
    KHelpClient::invokeHelp(QStringLiteral("PrinterDialog"), QStringLiteral("kxstitch"));
}

#include "moc_PagePropertiesDlg.cpp"
