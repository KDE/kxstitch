/*
 * Copyright (C) 2012-2015 by Stephen Allewell
 * steve.allewell@gmail.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#include "ConfigurationDialogs.h"

#include "SchemeManager.h"
#include "SymbolManager.h"


EditorConfigPage::EditorConfigPage(QWidget *parent, const QString &name)
    :   QWidget(parent)
{
    setObjectName(name);
    setupUi(this);
}


PatternConfigPage::PatternConfigPage(QWidget *parent, const QString &name)
    :   QWidget(parent)
{
    setObjectName(name);
    setupUi(this);

    m_currentDocumentUnitsIndex = Configuration::document_UnitsFormat();
    m_currentClothCountUnitsIndex = Configuration::editor_ClothCountUnits();
    
    // change the Default list item for cloth count measurement to append with the users locale measurement
    QString localeUnits = (QLocale::system().measurementSystem() == QLocale::MetricSystem) ? i18n("Default (Centimeters)") : i18n("Default (Inches)");
    kcfg_Editor_ClothCountUnits->setItemText(kcfg_Editor_ClothCountUnits->findText(i18n("Default")), localeUnits);
    
    setPatternSizePrecision();
    setClothCountPrecision();
}


void PatternConfigPage::on_kcfg_Editor_ClothCountUnits_activated(int index)
{
    Configuration::EnumEditor_ClothCountUnits::type clothCountTypeSelected = static_cast<Configuration::EnumEditor_ClothCountUnits::type>(index);
    
    if (clothCountTypeSelected == Configuration::EnumEditor_ClothCountUnits::Default) {
        clothCountTypeSelected = (QLocale::system().measurementSystem() == QLocale::MetricSystem) ? Configuration::EnumEditor_ClothCountUnits::Centimeters : Configuration::EnumEditor_ClothCountUnits::Inches;
    }

    if (m_currentClothCountUnitsIndex != clothCountTypeSelected) {
        m_currentClothCountUnitsIndex = clothCountTypeSelected;

        double horizontalClothCount = kcfg_Editor_HorizontalClothCount->value();
        double verticalClothCount = kcfg_Editor_VerticalClothCount->value();
        
        if (clothCountTypeSelected == Configuration::EnumEditor_ClothCountUnits::Inches) {
            horizontalClothCount *= 2.54;
            verticalClothCount *= 2.54;
        } else {
            horizontalClothCount /= 2.54;
            verticalClothCount /= 2.54;
        }

        setClothCountPrecision();
        
        kcfg_Editor_HorizontalClothCount->setValue(horizontalClothCount);
        kcfg_Editor_VerticalClothCount->setValue(verticalClothCount);
    }
}


void PatternConfigPage::on_kcfg_Document_UnitsFormat_activated(int index)
{
    Configuration::EnumDocument_UnitsFormat::type documentUnitsTypeSelected = static_cast<Configuration::EnumDocument_UnitsFormat::type>(index);

    if (m_currentDocumentUnitsIndex != documentUnitsTypeSelected) {
        int originalDocumentUnitsIndex = m_currentDocumentUnitsIndex;
        m_currentDocumentUnitsIndex = documentUnitsTypeSelected;

        double documentWidth = kcfg_Document_Width->value();
        double documentHeight = kcfg_Document_Height->value();

        double horizontalClothCount = kcfg_Editor_HorizontalClothCount->value();
        double verticalClothCount = kcfg_Editor_VerticalClothCount->value();

        switch (originalDocumentUnitsIndex) {
        case Configuration::EnumDocument_UnitsFormat::Stitches:
            documentWidth /= horizontalClothCount;
            documentHeight /= verticalClothCount;

            switch (documentUnitsTypeSelected) {
            case Configuration::EnumDocument_UnitsFormat::Inches:
                if (m_currentClothCountUnitsIndex == Configuration::EnumEditor_ClothCountUnits::Centimeters) { // from stitches to inches with cloth count in centimeters
                    documentWidth /= 2.54;
                    documentHeight /= 2.54;
                }

                break;

            case Configuration::EnumDocument_UnitsFormat::Centimeters:
                if (m_currentClothCountUnitsIndex == Configuration::EnumEditor_ClothCountUnits::Inches) { // from stitches to centimeters with cloth count in inches
                    documentWidth *= 2.54;
                    documentHeight *= 2.54;
                }

                break;

            default:
                // Do not need to do anything
                break;
            }

            break;

        case Configuration::EnumDocument_UnitsFormat::Inches:
            switch (documentUnitsTypeSelected) {
            case Configuration::EnumDocument_UnitsFormat::Stitches:
                documentWidth *= horizontalClothCount;
                documentHeight *= verticalClothCount;

                if (m_currentClothCountUnitsIndex == Configuration::EnumEditor_ClothCountUnits::Centimeters) { // from inches to stitches with cloth count in centimeters
                    documentWidth *= 2.54;
                    documentHeight *= 2.54;
                }

                break;

            case Configuration::EnumDocument_UnitsFormat::Centimeters: // from inches to centimeters with cloch count in inches
                documentWidth *= 2.54;
                documentHeight *= 2.54;
                break;

            default:
                // do not need to do anything
                break;
            }

            break;

        case Configuration::EnumDocument_UnitsFormat::Centimeters:
            switch (documentUnitsTypeSelected) {
            case Configuration::EnumDocument_UnitsFormat::Stitches:
                documentWidth *= horizontalClothCount;
                documentHeight *= verticalClothCount;

                if (m_currentClothCountUnitsIndex == Configuration::EnumEditor_ClothCountUnits::Inches) { // from centimeters to stitches with cloth count in inches
                    documentWidth /= 2.54;
                    documentHeight /= 2.54;
                }

                break;

            case Configuration::EnumDocument_UnitsFormat::Inches: // from centimeters to inches with cloth count in centimeters
                documentWidth /= 2.54;
                documentHeight /= 2.54;
                break;

            default:
                // do not need to do anything
                break;
            }

            break;
        }

        setPatternSizePrecision();
        
        kcfg_Document_Width->setValue(documentWidth);
        kcfg_Document_Height->setValue(documentHeight);
    }
}


void PatternConfigPage::on_kcfg_Editor_ClothCountLink_toggled(bool checked)
{
    kcfg_Editor_ClothCountLink->setIcon((checked) ? QIcon::fromTheme(QStringLiteral("object-locked")) : QIcon::fromTheme(QStringLiteral("object-unlocked")));
    kcfg_Editor_VerticalClothCount->setEnabled(!checked);
    kcfg_Editor_VerticalClothCount->setValue(kcfg_Editor_HorizontalClothCount->value());
}


void PatternConfigPage::on_kcfg_Editor_HorizontalClothCount_valueChanged(double value)
{
    if (kcfg_Editor_ClothCountLink->isChecked()) {
        kcfg_Editor_VerticalClothCount->setValue(value);
    }
}


void PatternConfigPage::setPatternSizePrecision()
{
    switch (m_currentDocumentUnitsIndex) {
    case Configuration::EnumDocument_UnitsFormat::Stitches:
        kcfg_Document_Width->setDecimals(0);
        kcfg_Document_Height->setDecimals(0);
        kcfg_Document_Width->setSingleStep(1);
        kcfg_Document_Height->setSingleStep(1);
        break;
        
    case Configuration::EnumDocument_UnitsFormat::Inches:
        kcfg_Document_Width->setDecimals(2);
        kcfg_Document_Height->setDecimals(2);
        kcfg_Document_Width->setSingleStep(0.01);
        kcfg_Document_Height->setSingleStep(0.01);
        break;

    case Configuration::EnumDocument_UnitsFormat::Centimeters:
        kcfg_Document_Width->setDecimals(2);
        kcfg_Document_Height->setDecimals(2);
        kcfg_Document_Width->setSingleStep(0.01);
        kcfg_Document_Height->setSingleStep(0.01);
        break;
        
    default:
        break;
    }

    kcfg_Document_Width->setRange(Configuration::document_Min_Width(), Configuration::document_Max_Width());
    kcfg_Document_Height->setRange(Configuration::document_Min_Height(), Configuration::document_Max_Height());
}
     
     
void PatternConfigPage::setClothCountPrecision()
{
    if (m_currentClothCountUnitsIndex == Configuration::EnumEditor_ClothCountUnits::Default) {
        m_currentClothCountUnitsIndex = (QLocale::system().measurementSystem() == QLocale::MetricSystem) ? Configuration::EnumEditor_ClothCountUnits::Centimeters : Configuration::EnumEditor_ClothCountUnits::Inches;
    }
    
    switch (m_currentClothCountUnitsIndex) {
    case Configuration::EnumEditor_ClothCountUnits::Inches:
        kcfg_Editor_HorizontalClothCount->setDecimals(0);
        kcfg_Editor_HorizontalClothCount->setSingleStep(1.0);
        kcfg_Editor_HorizontalClothCount->setSuffix(i18nc("Per inch measurements", "/in"));
        kcfg_Editor_VerticalClothCount->setDecimals(0);
        kcfg_Editor_VerticalClothCount->setSingleStep(1.0);
        kcfg_Editor_VerticalClothCount->setSuffix(i18nc("Per inch measurements", "/in"));
        break;

    case Configuration::EnumEditor_ClothCountUnits::Centimeters:
        kcfg_Editor_HorizontalClothCount->setDecimals(1);
        kcfg_Editor_HorizontalClothCount->setSingleStep(0.1);
        kcfg_Editor_HorizontalClothCount->setSuffix(i18nc("Per centimeter measurements", "/cm"));
        kcfg_Editor_VerticalClothCount->setDecimals(1);
        kcfg_Editor_VerticalClothCount->setSingleStep(0.1);
        kcfg_Editor_VerticalClothCount->setSuffix(i18nc("Per centimeter measurements", "/cm"));
        break;

    default:
        break;
    }
}


PaletteConfigPage::PaletteConfigPage(QWidget *parent, const QString &name)
    :   QWidget(parent)
{
    setObjectName(name);
    setupUi(this);
    // KConfigXT will read the currentText through the kcfg_property
    kcfg_Palette_DefaultScheme->setProperty("kcfg_property", QByteArray("currentText"));
    kcfg_Palette_DefaultSymbolLibrary->setProperty("kcfg_property", QByteArray("currentText"));
    kcfg_Palette_StitchStrands->setProperty("kcfg_property", QByteArray("currentText"));
    kcfg_Palette_BackstitchStrands->setProperty("kcfg_property", QByteArray("currentText"));
    kcfg_Palette_DefaultScheme->insertItems(0, SchemeManager::schemes());
    kcfg_Palette_DefaultSymbolLibrary->insertItems(0, SymbolManager::libraries());
    // KConfigXT can't write the currentText, so this needs to be set manually
    kcfg_Palette_DefaultScheme->setCurrentItem(Configuration::palette_DefaultScheme());
    kcfg_Palette_DefaultSymbolLibrary->setCurrentItem(Configuration::palette_DefaultSymbolLibrary());
    kcfg_Palette_StitchStrands->setCurrentIndex(Configuration::palette_StitchStrands() - 1);
    kcfg_Palette_BackstitchStrands->setCurrentIndex(Configuration::palette_BackstitchStrands() - 1);
}


void PaletteConfigPage::defaultClicked()
{
    // Clicking Default button doesn't work with the KComboBoxes with text defaults, set them manually
    kcfg_Palette_DefaultScheme->setCurrentItem(Configuration::defaultPalette_DefaultSchemeValue());
    kcfg_Palette_DefaultSymbolLibrary->setCurrentItem(Configuration::defaultPalette_DefaultSymbolLibraryValue());
    kcfg_Palette_StitchStrands->setCurrentIndex(Configuration::defaultPalette_StitchStrandsValue() - 1);
    kcfg_Palette_BackstitchStrands->setCurrentIndex(Configuration::defaultPalette_BackstitchStrandsValue() - 1);
}


ImportConfigPage::ImportConfigPage(QWidget *parent, const QString &name)
    :   QWidget(parent)
{
    setObjectName(name);
    setupUi(this);
}


LibraryConfigPage::LibraryConfigPage(QWidget *parent, const QString &name)
    :   QWidget(parent)
{
    setObjectName(name);
    setupUi(this);
}


PrinterConfigPage::PrinterConfigPage(QWidget *parent, const QString &name)
    :   QWidget(parent)
{
    setObjectName(name);
    setupUi(this);
}
