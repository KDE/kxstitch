/*
 * Copyright (C) 2012-2015 by Stephen Allewell
 * steve.allewell@gmail.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


/**
 * @file
 * Implement the SymbolSelectorDlg class.
 */


/**
 * @page symbolSelectorDlg Symbol Selector Dialog
 * This dialog allows the user to select a symbol to be associated with a
 * DocumentFloss. Any currently used flosses are shown grayed out and are
 * not selectable. The selection of a new symbol is made by clicking on the
 * required symbol, depending on the system settings for KDE, this will
 * require a single or double click.
 */


#include "SymbolSelectorDlg.h"

#include <QListWidget>

#include <KHelpClient>
#include <KLocalizedString>

#include "configuration.h"
#include "Symbol.h"
#include "SymbolLibrary.h"
#include "SymbolManager.h"


/**
 * Constructor.
 *
 * @param parent a pointer to a QWidget parent object
 * @param symbolLibrary the name of the symbol library to use
 */
SymbolSelectorDlg::SymbolSelectorDlg(QWidget *parent, const QString &symbolLibrary)
    :   QDialog(parent)
{
    setWindowTitle(i18n("Symbol Selector"));
    ui.setupUi(this);

    SymbolLibrary *library = SymbolManager::library(symbolLibrary);

    if (library == nullptr) {
        library = SymbolManager::library(QStringLiteral("kxstitch"));
    }

    ui.SymbolTable->loadFromLibrary(library);
}


/**
 * Set the current symbol and the used symbols to correctly display the
 * symbols from the library.
 *
 * @param symbol the current symbol index
 * @param used a const reference to a QList representing the indexes of used symbols
 */
void SymbolSelectorDlg::setSelectedSymbol(qint16 symbol, const QList<qint16> &used)
{
    m_currentSymbol = symbol;
    m_usedSymbols = used;

    foreach (qint16 index, m_usedSymbols) {
        ui.SymbolTable->disableItem(index);
    }

    ui.SymbolTable->setCurrent(m_currentSymbol);
}


/**
 * Get the selected symbol.
 *
 * @return a qint16 representing the selected symbol
 */
qint16 SymbolSelectorDlg::selectedSymbol()
{
    return m_currentSymbol;
}


/**
 * Override hideEvent to save the dialogs size.
 *
 * @param event a pointer to a QHideEvent
 */
void SymbolSelectorDlg::hideEvent(QHideEvent *event)
{
    KConfigGroup(KSharedConfig::openConfig(), QStringLiteral("DialogSizes")).writeEntry(QStringLiteral("SymbolSelectorDlg"), size());

    QDialog::hideEvent(event);
}


/**
 * Override showEvent to restore the dialogs size.
 *
 * @param event a pointer to a QShowEvent
 */
void SymbolSelectorDlg::showEvent(QShowEvent *event)
{
    QDialog::showEvent(event);

    if (KConfigGroup(KSharedConfig::openConfig(), QStringLiteral("DialogSizes")).hasKey(QStringLiteral("SymbolSelectorDlg"))) {
        resize(KConfigGroup(KSharedConfig::openConfig(), QStringLiteral("DialogSizes")).readEntry(QStringLiteral("SymbolSelectorDlg"), QSize()));
    }
}


/**
 * Set the selected symbol from the selected icon.
 * This slot is connected to the SymbolListWidget executed signal.
 * The index is stored as a data item in the QListWidgetItem.
 *
 * @param item a pointer to the QListWidgetItem that was selected
 */
void SymbolSelectorDlg::on_SymbolTable_itemClicked(QListWidgetItem *item)
{
    if (item->flags() & Qt::ItemIsEnabled) {
        m_usedSymbols.removeOne(m_currentSymbol);
        ui.SymbolTable->enableItem(m_currentSymbol);
        m_currentSymbol = static_cast<qint16>(item->data(Qt::UserRole).toInt());
        ui.SymbolTable->disableItem(m_currentSymbol);
        m_usedSymbols.append(m_currentSymbol);
        accept();
    }
}


/**
 * Called when the dialog Close button is pressed.
 */
void SymbolSelectorDlg::on_DialogButtonBox_rejected()
{
    reject();
}


/**
 * Called when the dialog Help button is pressed.
 */
void SymbolSelectorDlg::on_DialogButtonBox_helpRequested()
{
    KHelpClient::invokeHelp(QStringLiteral("SymbolSelectorDialog"), QStringLiteral("kxstitch"));
}
