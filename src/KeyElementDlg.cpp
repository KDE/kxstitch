/*
 * Copyright (C) 2010 by Stephen Allewell
 * sallewell@users.sourceforge.net
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#include "KeyElementDlg.h"

#include <QCheckBox>

#include "Element.h"


KeyElementDlg::KeyElementDlg(QWidget *parent, KeyElement *keyElement)
    :   KDialog(parent),
        m_keyElement(keyElement)
{
    setCaption(i18n("Key Element Properties"));
    setButtons(KDialog::Ok | KDialog::Cancel | KDialog::Help);
    setHelp("KeyElement");
    QWidget *widget = new QWidget(this);
    ui.setupUi(widget);

    ui.MarginLeft->setValue(keyElement->m_margins.left());
    ui.MarginTop->setValue(keyElement->m_margins.top());
    ui.MarginRight->setValue(keyElement->m_margins.right());
    ui.MarginBottom->setValue(keyElement->m_margins.bottom());
    ui.ShowBorder->setChecked(keyElement->m_showBorder);
    ui.BorderColor->setColor(keyElement->m_borderColor);
    ui.BorderThickness->setValue(double(keyElement->m_borderThickness) / 10);
    ui.FillBackground->setChecked(keyElement->m_fillBackground);
    ui.BackgroundColor->setColor(keyElement->m_backgroundColor);
    ui.BackgroundTransparency->setValue(keyElement->m_backgroundTransparency);
    ui.KeyFont->setFont(keyElement->m_textFont);
    ui.IndexStart->setValue(keyElement->m_indexStart);
    ui.IndexCount->setValue(keyElement->m_indexCount);
    ui.SymbolColumn->setChecked(keyElement->m_symbolColumn);
    ui.FlossNameColumn->setChecked(keyElement->m_flossNameColumn);
    ui.StrandsColumn->setChecked(m_keyElement->m_strandsColumn);
    ui.FlossDescriptionColumn->setChecked(m_keyElement->m_flossDescriptionColumn);
    ui.StitchesColumn->setChecked(m_keyElement->m_stitchesColumn);
    ui.LengthColumn->setChecked(m_keyElement->m_lengthColumn);
    ui.SkeinsColumn->setChecked(m_keyElement->m_skeinsColumn);

    QMetaObject::connectSlotsByName(this);

    setMainWidget(widget);
}


KeyElementDlg::~KeyElementDlg()
{
}


void KeyElementDlg::slotButtonClicked(int button)
{
    if (button == KDialog::Ok) {
        m_keyElement->m_margins = QMargins(ui.MarginLeft->value(), ui.MarginTop->value(), ui.MarginRight->value(), ui.MarginBottom->value());
        m_keyElement->m_showBorder = ui.ShowBorder->isChecked();
        m_keyElement->m_borderColor = ui.BorderColor->color();
        m_keyElement->m_borderThickness = int(ui.BorderThickness->value() * 10);
        m_keyElement->m_fillBackground = ui.FillBackground->isChecked();
        m_keyElement->m_backgroundColor = ui.BackgroundColor->color();
        m_keyElement->m_backgroundTransparency = ui.BackgroundTransparency->value();
        m_keyElement->m_textFont = ui.KeyFont->font();
        m_keyElement->m_indexStart = ui.IndexStart->value();
        m_keyElement->m_indexCount = ui.IndexCount->value();
        m_keyElement->m_symbolColumn = ui.SymbolColumn->isChecked();
        m_keyElement->m_flossNameColumn = ui.FlossNameColumn->isChecked();
        m_keyElement->m_strandsColumn = ui.StrandsColumn->isChecked();
        m_keyElement->m_flossDescriptionColumn = ui.FlossDescriptionColumn->isChecked();
        m_keyElement->m_stitchesColumn = ui.StitchesColumn->isChecked();
        m_keyElement->m_lengthColumn = ui.LengthColumn->isChecked();
        m_keyElement->m_skeinsColumn = ui.SkeinsColumn->isChecked();
        accept();
    } else {
        KDialog::slotButtonClicked(button);
    }
}
