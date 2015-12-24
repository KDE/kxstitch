/*
 * Copyright (C) 2010-2015 by Stephen Allewell
 * steve.allewell@gmail.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#include "PatternElementDlg.h"

#include <KHelpClient>
#include <KLocalizedString>

#include "Document.h"
#include "Element.h"
#include "SelectArea.h"


PatternElementDlg::PatternElementDlg(QWidget *parent, PatternElement *patternElement, Document *document, const QMap<int, QList<QRect> > &patternRects)
    :   QDialog(parent),
        m_patternElement(patternElement),
        m_document(document),
        m_patternRects(patternRects)
{
    setWindowTitle(i18n("Pattern Element Properties"));

    ui.setupUi(this);

    ui.RenderStitchesAs->setItemData(0, Configuration::EnumRenderer_RenderStitchesAs::Stitches);
    ui.RenderStitchesAs->setItemData(1, Configuration::EnumRenderer_RenderStitchesAs::BlackWhiteSymbols);
    ui.RenderStitchesAs->setItemData(2, Configuration::EnumRenderer_RenderStitchesAs::ColorSymbols);
    ui.RenderStitchesAs->setItemData(3, Configuration::EnumRenderer_RenderStitchesAs::ColorBlocks);
    ui.RenderStitchesAs->setItemData(4, Configuration::EnumRenderer_RenderStitchesAs::ColorBlocksSymbols);

    ui.RenderBackstitchesAs->setItemData(0, Configuration::EnumRenderer_RenderBackstitchesAs::ColorLines);
    ui.RenderBackstitchesAs->setItemData(1, Configuration::EnumRenderer_RenderBackstitchesAs::BlackWhiteSymbols);

    ui.RenderKnotsAs->setItemData(0, Configuration::EnumRenderer_RenderKnotsAs::ColorBlocks);
    ui.RenderKnotsAs->setItemData(1, Configuration::EnumRenderer_RenderKnotsAs::ColorSymbols);
    ui.RenderKnotsAs->setItemData(2, Configuration::EnumRenderer_RenderKnotsAs::BlackWhiteSymbols);

    ui.FormatScalesAs->setItemData(0, Configuration::EnumEditor_FormatScalesAs::Stitches);
    ui.FormatScalesAs->setItemData(1, Configuration::EnumEditor_FormatScalesAs::Inches);
    ui.FormatScalesAs->setItemData(2, Configuration::EnumEditor_FormatScalesAs::CM);

    m_selectArea = new SelectArea(ui.PreviewFrame, patternElement, document, patternRects);
    ui.PreviewFrame->setWidget(m_selectArea);

    m_selectArea->setPatternRect(patternElement->m_patternRect);
    ui.ShowStitches->setChecked(patternElement->m_showStitches);
    ui.ShowBackstitches->setChecked(patternElement->m_showBackstitches);
    ui.ShowKnots->setChecked(patternElement->m_showKnots);
    ui.ShowGrid->setChecked(patternElement->m_showGrid);
    ui.ShowScales->setChecked(patternElement->m_showScales);
    ui.ShowPlan->setChecked(patternElement->m_showPlan);
    ui.FormatScalesAs->setCurrentIndex(ui.FormatScalesAs->findData(patternElement->m_formatScalesAs));
    ui.RenderStitchesAs->setCurrentIndex(ui.RenderStitchesAs->findData(patternElement->m_renderStitchesAs));
    ui.RenderBackstitchesAs->setCurrentIndex(ui.RenderBackstitchesAs->findData(patternElement->m_renderBackstitchesAs));
    ui.RenderKnotsAs->setCurrentIndex(ui.RenderKnotsAs->findData(patternElement->m_renderKnotsAs));
}


bool PatternElementDlg::showPlan() const
{
    return ui.ShowPlan->isChecked();
}


PlanElement *PatternElementDlg::planElement() const
{
    return m_patternElement->m_planElement;
}


void PatternElementDlg::hideEvent(QHideEvent *event)
{
    KConfigGroup(KSharedConfig::openConfig(), QStringLiteral("DialogSizes")).writeEntry(QStringLiteral("PatternElementDlg"), size());

    QDialog::hideEvent(event);
}


void PatternElementDlg::showEvent(QShowEvent *event)
{
    QDialog::showEvent(event);

    if (KConfigGroup(KSharedConfig::openConfig(), QStringLiteral("DialogSizes")).hasKey(QStringLiteral("PatternElementDlg"))) {
        resize(KConfigGroup(KSharedConfig::openConfig(), QStringLiteral("DialogSizes")).readEntry(QStringLiteral("PatternElementDlg"), QSize()));
    }
}


void PatternElementDlg::on_DialogButtonBox_accepted()
{
    m_patternElement->setPatternRect(m_selectArea->patternRect());
    m_patternElement->m_showScales = ui.ShowScales->isChecked();

    if (ui.ShowPlan->isChecked()) {
        if (m_patternElement->m_planElement == nullptr) {
            m_patternElement->m_planElement = new PlanElement(m_patternElement->parent(), QRect(m_patternElement->rectangle().topLeft(), QSize(25, 25)));
        }
    } else {
        if (m_patternElement->m_planElement) {
            delete m_patternElement->m_planElement;
            m_patternElement->m_planElement = nullptr;
         }
    }

    if (m_patternElement->m_planElement) {
        m_patternElement->m_planElement->setPatternRect(m_patternElement->patternRect());
    }

    m_patternElement->m_showPlan = ui.ShowPlan->isChecked();
    m_patternElement->m_formatScalesAs = static_cast<Configuration::EnumEditor_FormatScalesAs::type>(ui.FormatScalesAs->itemData(ui.FormatScalesAs->currentIndex()).toInt());
    m_patternElement->setRenderStitchesAs(static_cast<Configuration::EnumRenderer_RenderStitchesAs::type>(ui.RenderStitchesAs->itemData(ui.RenderStitchesAs->currentIndex()).toInt()));
    m_patternElement->setRenderBackstitchesAs(static_cast<Configuration::EnumRenderer_RenderBackstitchesAs::type>(ui.RenderBackstitchesAs->itemData(ui.RenderBackstitchesAs->currentIndex()).toInt()));
    m_patternElement->setRenderKnotsAs(static_cast<Configuration::EnumRenderer_RenderKnotsAs::type>(ui.RenderKnotsAs->itemData(ui.RenderKnotsAs->currentIndex()).toInt()));
    m_patternElement->m_showStitches = ui.ShowStitches->isChecked();
    m_patternElement->m_showBackstitches = ui.ShowBackstitches->isChecked();
    m_patternElement->m_showKnots = ui.ShowKnots->isChecked();
    m_patternElement->m_showGrid = ui.ShowGrid->isChecked();

    accept();
}


void PatternElementDlg::on_DialogButtonBox_rejected()
{
    reject();
}


void PatternElementDlg::on_DialogButtonBox_helpRequested()
{
    KHelpClient::invokeHelp(QStringLiteral("PatternElement"), QStringLiteral("kxstitch"));
}
