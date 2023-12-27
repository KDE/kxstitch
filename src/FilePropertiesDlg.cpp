/*
 * Copyright (C) 2010-2015 by Stephen Allewell
 * steve.allewell@gmail.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include "FilePropertiesDlg.h"

#include <QLocale>
#include <QRect>

#include <KHelpClient>
#include <KLocalizedString>

#include "Commands.h"
#include "Document.h"
#include "Floss.h"
#include "FlossScheme.h"
#include "SchemeManager.h"

FilePropertiesDlg::FilePropertiesDlg(QWidget *parent, Document *document)
    : QDialog(parent)
    , m_document(document)
{
    setWindowTitle(i18n("File Properties"));
    ui.setupUi(this);

    ui.FlossScheme->addItems(SchemeManager::schemes());
    ui.FlossScheme->setCurrentItem(m_document->pattern()->palette().schemeName());

    m_unitsFormat = static_cast<Configuration::EnumDocument_UnitsFormat::type>(m_document->property(QStringLiteral("unitsFormat")).toInt());

    // get the extents of the pattern so that absolute minimum values for
    // the height and width of the pattern can be set.
    QRect extents = m_document->pattern()->stitches().extents();

    // extents will be (-1, -1) if no stitches have been added, set extents to minimum size
    if (extents.width() == -1) {
        extents.setWidth(1);
    }

    if (extents.height() == -1) {
        extents.setHeight(1);
    }

    m_minWidthStitches = extents.width();
    m_minHeightStitches = extents.height();

    // get the current width and height of the pattern in stitches.
    m_widthStitches = m_document->pattern()->stitches().width();
    m_heightStitches = m_document->pattern()->stitches().height();

    m_horizontalClothCount = m_document->property(QStringLiteral("horizontalClothCount")).toDouble();
    m_verticalClothCount = m_document->property(QStringLiteral("verticalClothCount")).toDouble();

    m_clothCountLink = m_document->property(QStringLiteral("clothCountLink")).toBool();
    m_clothCountUnits = static_cast<Configuration::EnumEditor_ClothCountUnits::type>(m_document->property(QStringLiteral("clothCountUnits")).toInt());

    updatePatternSizes();

    ui.ClothCountLink->setChecked(m_clothCountLink);
    on_ClothCountLink_clicked(m_clothCountLink);

    ui.PatternTitle->setText(m_document->property(QStringLiteral("title")).toString());
    ui.PatternAuthor->setText(m_document->property(QStringLiteral("author")).toString());
    ui.PatternCopyright->setText(m_document->property(QStringLiteral("copyright")).toString());
    ui.PatternFabric->setText(m_document->property(QStringLiteral("fabric")).toString());
    ui.FabricColor->setColor(m_document->property(QStringLiteral("fabricColor")).value<QColor>());
    ui.Instructions->setPlainText(m_document->property(QStringLiteral("instructions")).toString());
}

int FilePropertiesDlg::documentWidth() const
{
    return m_widthStitches;
}

int FilePropertiesDlg::documentHeight() const
{
    return m_heightStitches;
}

Configuration::EnumDocument_UnitsFormat::type FilePropertiesDlg::unitsFormat() const
{
    return m_unitsFormat;
}

double FilePropertiesDlg::horizontalClothCount() const
{
    return m_horizontalClothCount;
}

bool FilePropertiesDlg::clothCountLink() const
{
    return ui.ClothCountLink->isChecked();
}

double FilePropertiesDlg::verticalClothCount() const
{
    return m_verticalClothCount;
}

Configuration::EnumEditor_ClothCountUnits::type FilePropertiesDlg::clothCountUnits() const
{
    return m_clothCountUnits;
}

QString FilePropertiesDlg::title() const
{
    return ui.PatternTitle->text();
}

QString FilePropertiesDlg::author() const
{
    return ui.PatternAuthor->text();
}

QString FilePropertiesDlg::copyright() const
{
    return ui.PatternCopyright->text();
}

QString FilePropertiesDlg::fabric() const
{
    return ui.PatternFabric->text();
}

QColor FilePropertiesDlg::fabricColor() const
{
    return ui.FabricColor->color();
}

QString FilePropertiesDlg::instructions() const
{
    return ui.Instructions->toPlainText();
}

QString FilePropertiesDlg::flossScheme() const
{
    return ui.FlossScheme->currentText();
}

void FilePropertiesDlg::hideEvent(QHideEvent *event)
{
    KConfigGroup(KSharedConfig::openConfig(), QStringLiteral("DialogSizes")).writeEntry(QStringLiteral("FilePropertiesDlg"), size());

    QDialog::hideEvent(event);
}

void FilePropertiesDlg::showEvent(QShowEvent *event)
{
    QDialog::showEvent(event);

    if (KConfigGroup(KSharedConfig::openConfig(), QStringLiteral("DialogSizes")).hasKey(QStringLiteral("FilePropertiesDlg"))) {
        resize(KConfigGroup(KSharedConfig::openConfig(), QStringLiteral("DialogSizes")).readEntry(QStringLiteral("FilePropertiesDlg"), QSize()));
    }
}

void FilePropertiesDlg::on_UnitsFormat_activated(int index)
{
    m_unitsFormat = static_cast<Configuration::EnumDocument_UnitsFormat::type>(index);

    updatePatternSizes();
}

void FilePropertiesDlg::on_PatternWidth_valueChanged(double d)
{
    if (m_unitsFormat != Configuration::EnumDocument_UnitsFormat::Stitches) {
        d *= m_horizontalClothCount;
    }

    m_widthStitches = std::max((int)d, m_minWidthStitches);
}

void FilePropertiesDlg::on_PatternHeight_valueChanged(double d)
{
    if (m_unitsFormat != Configuration::EnumDocument_UnitsFormat::Stitches) {
        d *= m_horizontalClothCount;
    }

    m_heightStitches = std::max((int)d, m_minHeightStitches);
}

void FilePropertiesDlg::on_HorizontalClothCount_valueChanged(double d)
{
    m_horizontalClothCount = d;

    if (ui.ClothCountLink->isChecked()) {
        ui.VerticalClothCount->setValue(d);
    }

    updatePatternSizes();
}

void FilePropertiesDlg::on_VerticalClothCount_valueChanged(double d)
{
    m_verticalClothCount = d;

    updatePatternSizes();
}

void FilePropertiesDlg::on_ClothCountLink_clicked(bool checked)
{
    ui.ClothCountLink->setIcon((checked) ? QIcon::fromTheme(QStringLiteral("object-locked")) : QIcon::fromTheme(QStringLiteral("object-unlocked")));

    if (checked) {
        ui.VerticalClothCount->setValue(ui.HorizontalClothCount->value());
        ui.VerticalClothCount->setEnabled(false);
    } else {
        ui.VerticalClothCount->setEnabled(true);
    }
}

void FilePropertiesDlg::on_DialogButtonBox_accepted()
{
    accept();
}

void FilePropertiesDlg::on_DialogButtonBox_rejected()
{
    reject();
}

void FilePropertiesDlg::on_DialogButtonBox_helpRequested()
{
    KHelpClient::invokeHelp(QStringLiteral("PatternPropertiesDialog"), QStringLiteral("kxstitch"));
}

void FilePropertiesDlg::updatePatternSizes()
{
    // block signals from elements to avoid recursive calls
    ui.UnitsFormat->blockSignals(true);
    ui.PatternWidth->blockSignals(true);
    ui.PatternHeight->blockSignals(true);
    ui.HorizontalClothCount->blockSignals(true);
    ui.VerticalClothCount->blockSignals(true);

    ui.UnitsFormat->setCurrentIndex(m_unitsFormat);

    double horizontalScale = 1.0;
    double verticalScale = 1.0;

    switch (m_unitsFormat) {
    case Configuration::EnumDocument_UnitsFormat::Inches:
        if (m_clothCountUnits == Configuration::EnumEditor_ClothCountUnits::Inches) {
            horizontalScale = m_horizontalClothCount;
            verticalScale = m_verticalClothCount;
        } else {
            horizontalScale = m_horizontalClothCount * 2.54;
            verticalScale = m_verticalClothCount * 2.54;
        }

        break;

    case Configuration::EnumDocument_UnitsFormat::Centimeters:
        if (m_clothCountUnits == Configuration::EnumEditor_ClothCountUnits::Centimeters) {
            horizontalScale = m_horizontalClothCount;
            verticalScale = m_verticalClothCount;
        } else {
            horizontalScale = m_horizontalClothCount / 2.54;
            verticalScale = m_verticalClothCount / 2.54;
        }

        break;

    default:
        break;
    }

    double scaledMinWidth = m_minWidthStitches / horizontalScale;
    double scaledMinHeight = m_minHeightStitches / verticalScale;

    double scaledWidth = m_widthStitches / horizontalScale;
    double scaledHeight = m_heightStitches / verticalScale;

    if (m_unitsFormat == Configuration::EnumDocument_UnitsFormat::Stitches) {
        ui.PatternWidth->setDecimals(0);
        ui.PatternHeight->setDecimals(0);
        ui.PatternWidth->setSingleStep(1);
        ui.PatternHeight->setSingleStep(1);
    } else {
        ui.PatternWidth->setDecimals(2);
        ui.PatternHeight->setDecimals(2);
        ui.PatternWidth->setSingleStep(0.01);
        ui.PatternHeight->setSingleStep(0.01);
    }

    ui.PatternWidth->setMinimum(scaledMinWidth);
    ui.PatternHeight->setMinimum(scaledMinHeight);

    ui.PatternWidth->setValue(scaledWidth);
    ui.PatternHeight->setValue(scaledHeight);

    if (m_clothCountUnits == Configuration::EnumEditor_ClothCountUnits::Centimeters) {
        ui.HorizontalClothCount->setSuffix(i18nc("Per centimeter measurements", "/cm"));
        ui.VerticalClothCount->setSuffix(i18nc("Per centimeter measurements", "/cm"));
        ui.HorizontalClothCount->setDecimals(1);
        ui.VerticalClothCount->setDecimals(1);
        ui.HorizontalClothCount->setSingleStep(0.1);
        ui.VerticalClothCount->setSingleStep(0.1);
    } else {
        ui.HorizontalClothCount->setSuffix(i18nc("Per inch measurements", "/in"));
        ui.VerticalClothCount->setSuffix(i18nc("Per inch measurements", "/in"));
        ui.HorizontalClothCount->setDecimals(0);
        ui.VerticalClothCount->setDecimals(0);
        ui.HorizontalClothCount->setSingleStep(1);
        ui.VerticalClothCount->setSingleStep(1);
    }

    ui.HorizontalClothCount->setValue(m_horizontalClothCount);
    ui.VerticalClothCount->setValue(m_verticalClothCount);

    // re-enable signals from elements to avoid recursive calls
    ui.UnitsFormat->blockSignals(false);
    ui.PatternWidth->blockSignals(false);
    ui.PatternHeight->blockSignals(false);
    ui.HorizontalClothCount->blockSignals(false);
    ui.VerticalClothCount->blockSignals(false);
}

#include "moc_FilePropertiesDlg.cpp"
