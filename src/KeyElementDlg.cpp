/*
 * Copyright (C) 2010-2015 by Stephen Allewell
 * steve.allewell@gmail.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#include "KeyElementDlg.h"

#include <QCheckBox>

#include <KHelpClient>
#include <KLocalizedString>

#include "Element.h"


KeyElementDlg::KeyElementDlg(QWidget *parent, KeyElement *keyElement)
    :   QDialog(parent),
        m_keyElement(keyElement)
{
    setWindowTitle(i18n("Key Element Properties"));
    ui.setupUi(this);

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
    ui.SymbolColumnColor->setChecked(keyElement->m_symbolColumnColor);
    ui.FlossNameColumn->setChecked(keyElement->m_flossNameColumn);
    ui.StrandsColumn->setChecked(m_keyElement->m_strandsColumn);
    ui.FlossDescriptionColumn->setChecked(m_keyElement->m_flossDescriptionColumn);
    ui.StitchesColumn->setChecked(m_keyElement->m_stitchesColumn);
    ui.LengthColumn->setChecked(m_keyElement->m_lengthColumn);
    ui.SkeinsColumn->setChecked(m_keyElement->m_skeinsColumn);
}


void KeyElementDlg::hideEvent(QHideEvent *event)
{
    KConfigGroup(KSharedConfig::openConfig(), QStringLiteral("DialogSizes")).writeEntry(QStringLiteral("KeyElementDlg"), size());

    QDialog::hideEvent(event);
}


void KeyElementDlg::showEvent(QShowEvent *event)
{
    QDialog::showEvent(event);

    if (KConfigGroup(KSharedConfig::openConfig(), QStringLiteral("DialogSizes")).hasKey(QStringLiteral("KeyElementDlg"))) {
        resize(KConfigGroup(KSharedConfig::openConfig(), QStringLiteral("DialogSizes")).readEntry(QStringLiteral("KeyElementDlg"), QSize()));
    }
}


void KeyElementDlg::on_DialogButtonBox_accepted()
{
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
    m_keyElement->m_symbolColumnColor = ui.SymbolColumnColor->isChecked();
    m_keyElement->m_flossNameColumn = ui.FlossNameColumn->isChecked();
    m_keyElement->m_strandsColumn = ui.StrandsColumn->isChecked();
    m_keyElement->m_flossDescriptionColumn = ui.FlossDescriptionColumn->isChecked();
    m_keyElement->m_stitchesColumn = ui.StitchesColumn->isChecked();
    m_keyElement->m_lengthColumn = ui.LengthColumn->isChecked();
    m_keyElement->m_skeinsColumn = ui.SkeinsColumn->isChecked();

    accept();
}


void KeyElementDlg::on_DialogButtonBox_rejected()
{
    reject();
}


void KeyElementDlg::on_DialogButtonBox_helpRequested()
{
    KHelpClient::invokeHelp(QStringLiteral("KeyElement"), QStringLiteral("kxstitch"));
}

#include "moc_KeyElementDlg.cpp"
