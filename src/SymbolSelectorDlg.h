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
 * Header file for the SymbolSelectorDlg class.
 */


#ifndef SymbolSelectorDlg_H
#define SymbolSelectorDlg_H


#include <KDialog>

#include "ui_SymbolSelector.h"


class SymbolLibrary;


/**
 * @brief Provide a dialog to allow the selection of a symbol for the floss.
 *
 * This class implements a dialog that will display the symbols belonging to
 * the symbol set used by the palette. The icons used are displayed reflecting
 * whether the symbol has already been used or not. The user can select a symbol
 * either by single or double clicking depending on the system settings for
 * selections. The new symbol selected is made available to the caller.
 */
class SymbolSelectorDlg : public KDialog
{
    Q_OBJECT

public:
    explicit SymbolSelectorDlg(QWidget *parent);

    void setSelectedSymbol(qint16 symbol, const QList<qint16> &usedSymbols);
    qint16 selectedSymbol();

protected slots:
    void on_SymbolTable_executed(QListWidgetItem *item);

private:
    void fillSymbols();

    Ui::SymbolSelector    ui;           /**< dialog interface description */

    SymbolLibrary   *m_library;         /**< pointer to the SymbolLibrary */
    QList<qint16>   m_usedSymbols;      /**< a list of used symbols indexes */
    QMap<qint16, QListWidgetItem*>  m_symbolItems;  /**< a map of symbol indexes to QListWidgetItem pointers */
    qint16          m_currentSymbol;    /**< the current symbols index */
};


#endif
