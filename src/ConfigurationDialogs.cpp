/*
 * Copyright (C) 2012 by Stephen Allewell
 * sallewell@users.sourceforge.net
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#include "ConfigurationDialogs.h"

#include <KConfigDialogManager>

#include "configuration.h"
#include "SchemeManager.h"


EditorConfigPage::EditorConfigPage(QWidget *parent, const char *name)
    :   QWidget(parent)
{
    setObjectName(name);
    setupUi(this);
}


PatternConfigPage::PatternConfigPage(QWidget *parent, const char *name)
    :   QWidget(parent)
{
    setObjectName(name);
    setupUi(this);
    kcfg_Palette_DefaultScheme->insertItems(0, SchemeManager::schemes());

    QMetaObject::connectSlotsByName(this);

    m_currentDocumentUnitsIndex = Configuration::document_UnitsFormat();
    m_currentClothCountUnitsIndex = Configuration::editor_ClothCountUnits();
    kcfg_Editor_VerticalClothCount->setEnabled(!Configuration::editor_ClothCountLink());

    setPrecision();
}


void PatternConfigPage::on_kcfg_Editor_ClothCountUnits_activated(int index)
{
    if (m_currentClothCountUnitsIndex != index) {
        m_currentClothCountUnitsIndex = index;

        setPrecision();

        double horizontalClothCount = kcfg_Editor_HorizontalClothCount->value();
        double verticalClothCount = kcfg_Editor_VerticalClothCount->value();

        if (index == 0) {
            horizontalClothCount *= 2.54;
            verticalClothCount *= 2.54;
        } else {
            horizontalClothCount /= 2.54;
            verticalClothCount /= 2.54;
        }

        kcfg_Editor_HorizontalClothCount->setValue(horizontalClothCount);
        kcfg_Editor_VerticalClothCount->setValue(verticalClothCount);
    }

}


void PatternConfigPage::on_kcfg_Document_UnitsFormat_activated(int index)
{
    if (m_currentDocumentUnitsIndex != index) {
        int originalDocumentUnitsIndex = m_currentDocumentUnitsIndex;
        m_currentDocumentUnitsIndex = index;
        setPrecision();

        double documentWidth = kcfg_Document_Width->value();
        double documentHeight = kcfg_Document_Height->value();

        double horizontalClothCount = kcfg_Editor_HorizontalClothCount->value();
        double verticalClothCount = kcfg_Editor_VerticalClothCount->value();

        switch (originalDocumentUnitsIndex) {
        case 0: // Stitches
            documentWidth /= horizontalClothCount;
            documentHeight /= verticalClothCount;

            switch (index) {
            case 1: // Inches
                if (m_currentClothCountUnitsIndex == 1) {
                    documentWidth /= 2.54;
                    documentHeight /= 2.54;
                }

                break;

            case 2: // CM
                if (m_currentClothCountUnitsIndex == 0) {
                    documentWidth *= 2.54;
                    documentHeight *= 2.54;
                }

                break;
            }

            break;

        case 1: // Inches
            switch (index) {
            case 0: // Stitches
                documentWidth *= horizontalClothCount;
                documentHeight *= verticalClothCount;

                if (m_currentClothCountUnitsIndex == 1) {
                    documentWidth *= 2.54;
                    documentHeight *= 2.54;
                }

                break;

            case 2: // CM
                documentWidth *= 2.54;
                documentHeight *= 2.54;
                break;
            }

            break;

        case 2: // CM
            switch (index) {
            case 0: // Stitches
                documentWidth *= horizontalClothCount;
                documentHeight *= verticalClothCount;

                if (m_currentClothCountUnitsIndex == 0) {
                    documentWidth /= 2.54;
                    documentHeight /= 2.54;
                }

                break;

            case 1: // Inches
                documentWidth /= 2.54;
                documentHeight /= 2.54;
                break;
            }

            break;
        }

        kcfg_Document_Width->setValue(documentWidth);
        kcfg_Document_Height->setValue(documentHeight);
    }
}


void PatternConfigPage::on_kcfg_Editor_ClothCountLink_toggled(bool checked)
{
    kcfg_Editor_ClothCountLink->setIcon((checked) ? KIcon("link") : KIcon("link_break"));
    kcfg_Editor_VerticalClothCount->setEnabled(!checked);
    kcfg_Editor_VerticalClothCount->setValue(kcfg_Editor_HorizontalClothCount->value());
}


void PatternConfigPage::on_kcfg_Editor_HorizontalClothCount_valueChanged(double value)
{
    if (kcfg_Editor_ClothCountLink->isChecked()) {
        kcfg_Editor_VerticalClothCount->setValue(value);
    }
}


void PatternConfigPage::setPrecision()
{
    switch (m_currentDocumentUnitsIndex) {
    case 0: // Stitches
        kcfg_Document_Width->setDecimals(0);
        kcfg_Document_Width->setSingleStep(1.0);
        kcfg_Document_Height->setDecimals(0);
        kcfg_Document_Height->setSingleStep(1.0);
        break;

    case 1: // Inches
        kcfg_Document_Width->setDecimals(2);
        kcfg_Document_Width->setSingleStep(0.01);
        kcfg_Document_Height->setDecimals(2);
        kcfg_Document_Height->setSingleStep(0.01);
        break;

    case 2: // CM
        kcfg_Document_Width->setDecimals(2);
        kcfg_Document_Width->setSingleStep(0.01);
        kcfg_Document_Height->setDecimals(2);
        kcfg_Document_Height->setSingleStep(0.01);
        break;
    }

    switch (m_currentClothCountUnitsIndex) {
    case 0: // Inches
        kcfg_Editor_HorizontalClothCount->setDecimals(0);
        kcfg_Editor_HorizontalClothCount->setSingleStep(1.0);
        kcfg_Editor_VerticalClothCount->setDecimals(0);
        kcfg_Editor_VerticalClothCount->setSingleStep(1.0);
        break;

    case 1: // CM
        kcfg_Editor_HorizontalClothCount->setDecimals(2);
        kcfg_Editor_HorizontalClothCount->setSingleStep(0.01);
        kcfg_Editor_VerticalClothCount->setDecimals(2);
        kcfg_Editor_VerticalClothCount->setSingleStep(0.01);
        break;
    }
}


ImportConfigPage::ImportConfigPage(QWidget *parent, const char *name)
    :   QWidget(parent)
{
    setObjectName(name);
    setupUi(this);
}


LibraryConfigPage::LibraryConfigPage(QWidget *parent, const char *name)
    :   QWidget(parent)
{
    setObjectName(name);
    setupUi(this);
}


PrinterConfigPage::PrinterConfigPage(QWidget *parent, const char *name)
    :   QWidget(parent)
{
    setObjectName(name);
    setupUi(this);
}
