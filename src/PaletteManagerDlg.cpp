/*
 * Copyright (C) 2010-2015 by Stephen Allewell
 * steve.allewell@gmail.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include "PaletteManagerDlg.h"

#include <QBitmap>
#include <QPainter>
#include <QScreen>
#include <QWidget>

#include <KCharSelect>
#include <KHelpClient>
#include <KLocalizedString>
#include <KMessageBox>

#include "CalibrateFlossDlg.h"
#include "Commands.h"
#include "Floss.h"
#include "FlossScheme.h"
#include "MainWindow.h"
#include "NewFlossDlg.h"
#include "SchemeManager.h"
#include "SymbolLibrary.h"
#include "SymbolManager.h"
#include "SymbolSelectorDlg.h"
#include "configuration.h"

const uchar pickColorCursorMask[] = {
    0x18, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x18, 0x00,
    0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff,
    0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff,
    0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff,
    0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff,
};

PaletteManagerDlg::PaletteManagerDlg(QWidget *parent, Document *document)
    : QDialog(parent)
    , m_document(document)
    , m_dialogPalette(m_document->pattern()->palette())
    , m_flossUsage(document->pattern()->stitches().flossUsage())
    , m_scheme(SchemeManager::scheme(m_dialogPalette.schemeName()))
    , m_symbolSelectorDlg(nullptr)
{
    setWindowTitle(i18n("Palette Manager"));
    ui.setupUi(this);

    ui.SymbolLibrary->insertItems(0, SymbolManager::libraries());
    ui.SymbolLibrary->setCurrentItem(m_dialogPalette.symbolLibrary());

    fillLists();
}

PaletteManagerDlg::~PaletteManagerDlg()
{
    delete m_symbolSelectorDlg;
}

const DocumentPalette &PaletteManagerDlg::palette() const
{
    return m_dialogPalette;
}

void PaletteManagerDlg::hideEvent(QHideEvent *event)
{
    KConfigGroup(KSharedConfig::openConfig(), QStringLiteral("DialogSizes")).writeEntry(QStringLiteral("PaletteManagerDlg"), size());

    QDialog::hideEvent(event);
}

void PaletteManagerDlg::showEvent(QShowEvent *event)
{
    QDialog::showEvent(event);

    if (KConfigGroup(KSharedConfig::openConfig(), QStringLiteral("DialogSizes")).hasKey(QStringLiteral("PaletteManagerDlg"))) {
        resize(KConfigGroup(KSharedConfig::openConfig(), QStringLiteral("DialogSizes")).readEntry(QStringLiteral("PaletteManagerDlg"), QSize()));
    }
}

void PaletteManagerDlg::on_ColorList_currentRowChanged(int currentRow)
{
    ui.AddFloss->setEnabled((currentRow != -1) && symbolsAvailable());
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
        Symbol symbol = SymbolManager::library(m_dialogPalette.symbolLibrary())->symbol(m_dialogPalette.flosses().value(i)->stitchSymbol());
        ui.StitchSymbol->setIcon(SymbolListWidget::createIcon(symbol, 22));
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

    if (symbolsAvailable()) {
        ui.AddFloss->setEnabled(ui.ColorList->currentRow() != -1);
        ui.AddFloss->setToolTip(QString());
    } else {
        ui.AddFloss->setEnabled(false);
        ui.AddFloss->setToolTip(QString(i18n("There are no more symbols available.")));
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

void PaletteManagerDlg::on_SymbolLibrary_currentTextChanged(const QString &library)
{
    m_dialogPalette.setSymbolLibrary(library);

    if (library != m_dialogPalette.symbolLibrary()) {
        // Can't change the library because there aren't enough symbols available
        // Show a warning and reset the current library in the selection
        KMessageBox::information(this, QString(i18n("The selected symbol library does not have enough symbols for the flosses in the palette.")));
        ui.SymbolLibrary->setCurrentItem(m_dialogPalette.symbolLibrary());
    } else if (m_symbolSelectorDlg) {
        delete m_symbolSelectorDlg;
        m_symbolSelectorDlg = new SymbolSelectorDlg(this, library);
    }

    on_CurrentList_currentRowChanged(ui.CurrentList->currentRow());
}

void PaletteManagerDlg::on_StitchSymbol_clicked(bool)
{
    int i = paletteIndex(ui.CurrentList->currentItem()->data(Qt::UserRole).toString());

    if (m_symbolSelectorDlg == nullptr) {
        m_symbolSelectorDlg = new SymbolSelectorDlg(this, m_dialogPalette.symbolLibrary());
    }

    m_symbolSelectorDlg->setSelectedSymbol(m_dialogPalette.flosses().value(i)->stitchSymbol(), m_dialogPalette.usedSymbols());

    if (m_symbolSelectorDlg->exec() == QDialog::Accepted) {
        m_dialogPalette.floss(i)->setStitchSymbol(m_symbolSelectorDlg->selectedSymbol());
        ui.StitchSymbol->setIcon(
            SymbolListWidget::createIcon(SymbolManager::library(m_dialogPalette.symbolLibrary())->symbol(m_dialogPalette.flosses().value(i)->stitchSymbol()),
                                         22));
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
            listWidgetItem->setText(QString::fromLatin1("%1 %2").arg(floss->name(), floss->description()));
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
    for (int row = 0; row < ui.CurrentList->count();) {
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

void PaletteManagerDlg::on_PickColor_clicked()
{
    setMouseTracking(true);
    setCursor(Qt::CrossCursor);
    grabMouse();
}

void PaletteManagerDlg::mouseMoveEvent(QMouseEvent *event)
{
    QPoint hotSpot = event->globalPosition().toPoint();
    QColor color(QGuiApplication::primaryScreen()->grabWindow(0, hotSpot.x(), hotSpot.y(), 1, 1).toImage().pixel(0, 0));
    QPixmap pixmap(32, 32);
    pixmap.fill(color);

    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing, false);
    painter.drawLine(3, 0, 3, 7);
    painter.drawLine(4, 0, 4, 7);
    painter.drawLine(0, 3, 7, 3);
    painter.drawLine(0, 4, 7, 4);
    painter.drawRect(8, 8, 23, 23);
    painter.end();

    pixmap.setMask(QBitmap().fromData(QSize(32, 32), pickColorCursorMask, QImage::Format_Mono));
    setCursor(QCursor(pixmap, 4, 4));
}

void PaletteManagerDlg::mouseReleaseEvent(QMouseEvent *event)
{
    releaseMouse();
    setCursor(Qt::ArrowCursor);
    setMouseTracking(false);

    QColor color(QGuiApplication::primaryScreen()->grabWindow(0).toImage().pixel(event->globalPosition().toPoint()));
    Floss *floss = m_scheme->convert(color);

    QList<QListWidgetItem *> foundItems = ui.CurrentList->findItems(floss->name(), Qt::MatchStartsWith);

    if (foundItems.count()) {
        ui.CurrentList->setCurrentItem(foundItems.at(0));
        ui.CurrentList->setFocus();
    } else {
        foundItems = ui.ColorList->findItems(floss->name(), Qt::MatchStartsWith);

        if (foundItems.count()) {
            ui.ColorList->setCurrentItem(foundItems.at(0));
            ui.ColorList->setFocus();
        }
    }
}

void PaletteManagerDlg::on_DialogButtonBox_accepted()
{
    accept();
}

void PaletteManagerDlg::on_DialogButtonBox_rejected()
{
    reject();
}

void PaletteManagerDlg::on_DialogButtonBox_helpRequested()
{
    KHelpClient::invokeHelp(QStringLiteral("PaletteManagerDialog"), QStringLiteral("kxstitch"));
}

void PaletteManagerDlg::fillLists()
{
    ui.ColorList->clear();
    ui.CurrentList->clear();
    ui.CurrentScheme->setText(m_scheme->schemeName());

    foreach (const Floss *floss, m_scheme->flosses()) {
        QListWidgetItem *listWidgetItem = new QListWidgetItem;
        listWidgetItem->setText(QString::fromLatin1("%1 %2").arg(floss->name(), floss->description()));
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

    for (int row = 0; row < rows; row++) {
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
    for (QMap<int, DocumentFloss *>::const_iterator i = m_dialogPalette.flosses().constBegin(); i != m_dialogPalette.flosses().constEnd(); ++i) {
        if (flossName == i.value()->flossName()) {
            return true;
        }
    }

    return false;
}

int PaletteManagerDlg::paletteIndex(const QString &flossName) const
{
    for (QMap<int, DocumentFloss *>::const_iterator i = m_dialogPalette.flosses().constBegin(); i != m_dialogPalette.flosses().constEnd(); ++i) {
        if (flossName == i.value()->flossName()) {
            return i.key();
        }
    }

    return -1;
}

bool PaletteManagerDlg::symbolsAvailable() const
{
    return (SymbolManager::library(m_dialogPalette.symbolLibrary())->indexes().count() > m_dialogPalette.flosses().count());
}

#include "moc_PaletteManagerDlg.cpp"
