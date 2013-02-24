/*
 * Copyright (C) 2010 by Stephen Allewell
 * stephen@mirramar.adsl24.co.uk
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#include "PaletteManagerDlg.h"

#include <QWidget>

#include <KCharSelect>

#include "configuration.h"
#include "CalibrateFlossDlg.h"
#include "CharSelectorDlg.h"
#include "Commands.h"
#include "Floss.h"
#include "FlossScheme.h"
#include "MainWindow.h"
#include "NewFlossDlg.h"
#include "SchemeManager.h"


PaletteManagerDlg::PaletteManagerDlg(QWidget *parent, Document *document)
    :   KDialog(parent),
        m_document(document),
        m_dialogPalette(m_document->pattern()->palette()),
        m_flossUsage(document->pattern()->stitches().flossUsage()),
        m_scheme(SchemeManager::scheme(m_dialogPalette.schemeName())),
        m_charSelectorDlg(0)
{
    setCaption(i18n("Palette Manager"));
    setButtons(KDialog::Ok | KDialog::Cancel | KDialog::Help);
    setHelp("PaletteManagerDialog");
    QWidget *widget = new QWidget(this);
    ui.setupUi(widget);
    QMetaObject::connectSlotsByName(this);
    setMainWidget(widget);

    fillLists();
}


PaletteManagerDlg::~PaletteManagerDlg()
{
    delete m_charSelectorDlg;
}


const DocumentPalette &PaletteManagerDlg::palette() const
{
    return m_dialogPalette;
}


void PaletteManagerDlg::slotButtonClicked(int button)
{
    if (button == KDialog::Ok) {
        accept();
    } else {
        KDialog::slotButtonClicked(button);
    }
}


void PaletteManagerDlg::on_ColorList_currentRowChanged(int currentRow)
{
    ui.AddFloss->setEnabled(currentRow != -1);
}


int mapStyleToIndex(Qt::PenStyle style)
{
    int index = 0;

    if (style == Qt::DashLine) {
        index = 1;
    }

    if (style == Qt::DotLine) {
        index = 2;
    }

    if (style == Qt::DashDotLine) {
        index = 3;
    }

    if (style == Qt::DashDotDotLine) {
        index = 4;
    }

    return index;
}


void PaletteManagerDlg::on_CurrentList_currentRowChanged(int currentRow)
{
    if (currentRow != -1) {
        int i = paletteIndex(ui.CurrentList->currentItem()->data(Qt::UserRole).toString());
        const DocumentFloss *floss = m_dialogPalette.flosses().value(i);
        ui.StitchStrands->setCurrentIndex(floss->stitchStrands() - 1);
        ui.BackstitchStrands->setCurrentIndex(floss->backstitchStrands() - 1);
        ui.StitchSymbol->setText(floss->stitchSymbol());
        ui.BackstitchSymbol->setCurrentIndex(mapStyleToIndex(floss->backstitchSymbol()));
        ui.StitchStrands->setEnabled(true);
        ui.BackstitchStrands->setEnabled(true);
        ui.StitchSymbol->setEnabled(true);
        ui.BackstitchSymbol->setEnabled(true);
        ui.ClearUnused->setEnabled(true);

        if (m_flossUsage.contains(i) && m_flossUsage[i].totalStitches() != 0) {
            ui.RemoveFloss->setEnabled(false);
        } else {
            ui.RemoveFloss->setEnabled(true);
        }
    } else {
        ui.RemoveFloss->setEnabled(false);
        ui.StitchStrands->setEnabled(false);
        ui.BackstitchStrands->setEnabled(false);
        ui.StitchSymbol->setEnabled(false);
        ui.BackstitchSymbol->setEnabled(false);
        ui.ClearUnused->setEnabled(false);
    }
}


void PaletteManagerDlg::on_AddFloss_clicked(bool)
{
    QListWidgetItem *listWidgetItem = ui.ColorList->takeItem(ui.ColorList->currentRow());
    m_dialogPalette.add(listWidgetItem->data(Qt::DecorationRole).value<QColor>());
    insertListWidgetItem(ui.CurrentList, listWidgetItem);
    ui.CurrentList->scrollToItem(listWidgetItem, QAbstractItemView::PositionAtCenter);
    ui.CurrentList->setCurrentItem(listWidgetItem);
}


void PaletteManagerDlg::on_RemoveFloss_clicked(bool)
{
    QListWidgetItem *listWidgetItem = ui.CurrentList->takeItem(ui.CurrentList->currentRow());
    int i = paletteIndex(listWidgetItem->data(Qt::UserRole).toString());
    m_dialogPalette.remove(i);

    insertListWidgetItem(ui.ColorList, listWidgetItem);
    ui.ColorList->scrollToItem(listWidgetItem, QAbstractItemView::PositionAtCenter);
    ui.ColorList->setCurrentItem(listWidgetItem);
}


void PaletteManagerDlg::on_StitchStrands_activated(int index)
{
    m_dialogPalette.floss(paletteIndex(ui.CurrentList->currentItem()->data(Qt::UserRole).toString()))->setStitchStrands(index + 1);
}


void PaletteManagerDlg::on_BackstitchStrands_activated(int index)
{
    m_dialogPalette.floss(paletteIndex(ui.CurrentList->currentItem()->data(Qt::UserRole).toString()))->setBackstitchStrands(index + 1);
}


void PaletteManagerDlg::on_StitchSymbol_clicked(bool)
{
    int i = paletteIndex(ui.CurrentList->currentItem()->data(Qt::UserRole).toString());

    if (m_charSelectorDlg == 0) {
        m_charSelectorDlg = new CharSelectorDlg(this, m_dialogPalette.usedSymbols());
    }

    m_charSelectorDlg->setSelectedChar(m_dialogPalette.flosses().value(i)->stitchSymbol());

    if (m_charSelectorDlg->exec() == QDialog::Accepted) {
        QChar c = m_charSelectorDlg->selectedChar();
        m_dialogPalette.floss(i)->setStitchSymbol(c);
        ui.StitchSymbol->setText(c);
    }
}


void PaletteManagerDlg::on_BackstitchSymbol_activated(int index)
{
    Qt::PenStyle style = Qt::SolidLine;

    if (index == 1) {
        style = Qt::DashLine;
    } else if (index == 2) {
        style = Qt::DotLine;
    } else if (index == 3) {
        style = Qt::DashDotLine;
    } else if (index == 4) {
        style = Qt::DashDotDotLine;
    }

    m_dialogPalette.floss(paletteIndex(ui.CurrentList->currentItem()->data(Qt::UserRole).toString()))->setBackstitchSymbol(style);
}


void PaletteManagerDlg::on_NewFloss_clicked(bool)
{
    QPointer<NewFlossDlg> newFlossDlg = new NewFlossDlg(this, m_scheme);

    if (newFlossDlg->exec()) {
        Floss *floss = newFlossDlg->floss();

        if (floss) {
            QListWidgetItem *listWidgetItem = new QListWidgetItem;
            listWidgetItem->setText(QString("%1 %2").arg(floss->name()).arg(floss->description()));
            listWidgetItem->setData(Qt::DecorationRole, QColor(floss->color()));
            listWidgetItem->setData(Qt::UserRole, QString(floss->name()));

            if (contains(floss->name())) {
                insertListWidgetItem(ui.CurrentList, listWidgetItem);

                if (m_flossUsage.contains(paletteIndex(floss->name())) && m_flossUsage[paletteIndex(floss->name())].totalStitches()) {
                    listWidgetItem->setForeground(QBrush(Qt::gray));
                }
            } else {
                insertListWidgetItem(ui.ColorList, listWidgetItem);
            }
        }
    }

    delete newFlossDlg;
}


void PaletteManagerDlg::on_ClearUnused_clicked(bool)
{
    for (int row = 0 ; row < ui.CurrentList->count() ;) {
        ui.CurrentList->setCurrentRow(row);
        QListWidgetItem *listWidgetItem = ui.CurrentList->currentItem();
        int i = paletteIndex(listWidgetItem->data(Qt::UserRole).toString());

        if (!m_flossUsage.contains(i) || m_flossUsage[i].totalStitches() == 0) {
            on_RemoveFloss_clicked(true);
        } else {
            row++;
        }
    }
}


void PaletteManagerDlg::on_Calibrate_clicked(bool)
{
    QPointer<CalibrateFlossDlg> calibrateFlossDlg = new CalibrateFlossDlg(this, m_dialogPalette.schemeName());

    if (calibrateFlossDlg->exec() == QDialog::Accepted) {
        fillLists();
    }
}


void PaletteManagerDlg::fillLists()
{
    ui.ColorList->clear();
    ui.CurrentList->clear();

    foreach (const Floss * floss, m_scheme->flosses()) {
        QListWidgetItem *listWidgetItem = new QListWidgetItem;
        listWidgetItem->setText(QString("%1 %2").arg(floss->name()).arg(floss->description()));
        listWidgetItem->setData(Qt::DecorationRole, QColor(floss->color()));
        listWidgetItem->setData(Qt::UserRole, QString(floss->name()));

        if (contains(floss->name())) {
            insertListWidgetItem(ui.CurrentList, listWidgetItem);

            if (m_flossUsage.contains(paletteIndex(floss->name())) && m_flossUsage[paletteIndex(floss->name())].totalStitches()) {
                listWidgetItem->setForeground(QBrush(Qt::gray));
            }
        } else {
            insertListWidgetItem(ui.ColorList, listWidgetItem);
        }
    }

    /* The following should have worked, but setting the currentRow to -1 doesn't emit
        the signal and consequently doesn't call the slot

    ui.CurrentList->setCurrentRow(ui.CurrentList->count()?0:-1);
    ui.ColorList->setCurrentRow(ui.ColorList->count()?0:-1);
    */

    if (ui.CurrentList->count()) {
        ui.CurrentList->setCurrentRow(0);
    } else {
        on_CurrentList_currentRowChanged(-1);
    }

    if (ui.ColorList->count()) {
        ui.ColorList->setCurrentRow(0);
    } else {
        on_ColorList_currentRowChanged(-1);
    }
}


void PaletteManagerDlg::insertListWidgetItem(QListWidget *listWidget, QListWidgetItem *listWidgetItem)
{
    int rows = listWidget->count();

    for (int row = 0 ; row < rows ; row++) {
        if (listWidgetItem->data(Qt::UserRole).toInt() < listWidget->item(row)->data(Qt::UserRole).toInt()) {
            do {
                QListWidgetItem *oldListWidgetItem = listWidget->item(row);
                listWidget->insertItem(row, listWidgetItem);
                listWidgetItem = oldListWidgetItem;
            } while (++row < rows);
        }
    }

    listWidget->addItem(listWidgetItem);
}


bool PaletteManagerDlg::contains(const QString &flossName) const
{
    for (QMap<int, DocumentFloss*>::const_iterator i = m_dialogPalette.flosses().constBegin() ; i != m_dialogPalette.flosses().constEnd() ; ++i) {
        if (flossName == i.value()->flossName()) {
            return true;
        }
    }

    return false;
}


int PaletteManagerDlg::paletteIndex(const QString &flossName) const
{
    for (QMap<int, DocumentFloss*>::const_iterator i = m_dialogPalette.flosses().constBegin() ; i != m_dialogPalette.flosses().constEnd() ; ++i) {
        if (flossName == i.value()->flossName()) {
            return i.key();
        }
    }

    return -1;
}
