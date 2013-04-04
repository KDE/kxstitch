/*
 * Copyright (C) 2012 by Stephen Allewell
 * stephen@mirramar.adsl24.co.uk
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

#include <KListWidget>
#include <KLocale>

#include "configuration.h"
#include "Symbol.h"
#include "SymbolLibrary.h"
#include "SymbolManager.h"


/**
 * Constructor.
 *
 * @param parent a pointer to a QWidget parent object
 */
SymbolSelectorDlg::SymbolSelectorDlg(QWidget *parent)
    :   KDialog(parent),
        m_library(0)
{
    setCaption(i18n("Symbol Selector"));
    setButtons(KDialog::Cancel | KDialog::Help);
    setHelp("SymbolSelectorDialog");
    QWidget *widget = new QWidget(this);
    ui.setupUi(widget);
    QMetaObject::connectSlotsByName(this);
    setMainWidget(widget);
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
    fillSymbols();
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
 * Set the selected symbol from the selected icon.
 * This slot is connected to the SymbolListWidget executed signal.
 * The index is stored as a data item in the QListWidgetItem.
 *
 * @param item a pointer to the QListWidgetItem that was selected
 */
void SymbolSelectorDlg::on_SymbolTable_executed(QListWidgetItem *item)
{
    m_usedSymbols.removeOne(m_currentSymbol);
    ui.SymbolTable->enableItem(m_symbolItems.value(m_currentSymbol));
    m_currentSymbol = static_cast<qint16>(item->data(Qt::UserRole).toInt());
    ui.SymbolTable->disableItem(m_symbolItems.value(m_currentSymbol));
    m_usedSymbols.append(m_currentSymbol);

    accept();
}


/**
 * Fill the SymbolListWidget with the symbols from the symbol library.
 * Each symbol is added to the widget, the state of the icon is determined
 * by the used state of the symbol.
 */
void SymbolSelectorDlg::fillSymbols()
{
    m_library = SymbolManager::library("kxstitch"); // TODO options for other symbol libraries

    if (m_library) {
        foreach (qint16 index, m_library->indexes()) {
            QListWidgetItem *item = ui.SymbolTable->addSymbol(index, m_library->symbol(index));
            m_symbolItems.insert(index, item);

            if (m_usedSymbols.contains(index)) {
                ui.SymbolTable->disableItem(item);
            }
        }
    }

    ui.SymbolTable->setCurrent(m_currentSymbol);
}
