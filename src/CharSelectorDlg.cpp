/*
 * Copyright (C) 2012 by Stephen Allewell
 * stephen@mirramar.adsl24.co.uk
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#include "CharSelectorDlg.h"

#include <KCharSelect>
#include <KLocale>

#include "configuration.h"


CharSelectorDlg::CharSelectorDlg(QWidget *parent, QList<QChar> usedSymbols)
    :   KDialog(parent),
        m_usedSymbols(usedSymbols)
{
    setCaption(i18n("Character Selector"));
    setButtons(KDialog::Ok | KDialog::Cancel | KDialog::Help);
    setHelp("CharacterSelectorDialog");
    QWidget *widget = new QWidget(this);
    ui.setupUi(widget);
    // Although the ui should be set up at this point, the constructor used for KCharSelect does not
    // give the correct arrangement.  It can not be modified in the Qt Designer so it is necessary
    // to replace the KCharSelect created with a new one with the correct constructor.
    // The following is similar to the generated file ui_CharSelector.h but using the correct constructor.
    ui.gridLayout->removeWidget(ui.CharSelect);
    delete ui.CharSelect;
    ui.CharSelect = new KCharSelect(widget, 0, KCharSelect::CharacterTable);
    ui.CharSelect->setObjectName(QString::fromUtf8("CharSelect"));
    ui.gridLayout->addWidget(ui.CharSelect, 0, 0, 1, 1);
    QMetaObject::connectSlotsByName(this);
    setMainWidget(widget);
    ui.CharSelect->setCurrentFont(Configuration::editor_SymbolFont());
}

void CharSelectorDlg::setSelectedChar(const QChar &c)
{
    ui.CharSelect->setCurrentChar(c);
    m_currentChar = c;
}


QChar CharSelectorDlg::selectedChar()
{
    return m_currentChar;
}


void CharSelectorDlg::on_CharSelect_currentCharChanged(const QChar &c)
{
    if (!c.isPrint() || c.isSpace() || m_usedSymbols.contains(c)) {
        ui.CharSelect->setCurrentChar(m_currentChar);
        return;
    }

    m_currentChar = c;
}
