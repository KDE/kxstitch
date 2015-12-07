/*
 * Copyright (C) 2010-2015 by Stephen Allewell
 * steve.allewell@gmail.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#include "CalibrateFlossDlg.h"

#include <KHelpClient>
#include <KLocalizedString>

#include "Floss.h"
#include "FlossScheme.h"
#include "SchemeManager.h"


CalibrateFlossDlg::CalibrateFlossDlg(QWidget *parent, const QString &schemeName)
    :   QDialog(parent),
        m_schemeName(schemeName),
        m_item(nullptr),
        m_sample(nullptr)
{
    setWindowTitle(i18n("Calibrate Floss"));
    ui.setupUi(this);

    fillSchemeList();
    ui.SchemeList->setCurrentIndex(ui.SchemeList->findText(schemeName));
}


CalibrateFlossDlg::~CalibrateFlossDlg()
{
    delete m_sample;
}


void CalibrateFlossDlg::fillSchemeList()
{
    ui.SchemeList->insertItems(0, SchemeManager::schemes());
}


void CalibrateFlossDlg::fillColorList()
{
    m_item = nullptr;
    ui.ColorList->clear();
    m_schemeName = ui.SchemeList->currentText();
    FlossScheme *scheme = SchemeManager::scheme(m_schemeName);
    QListIterator<Floss *> flossIterator(scheme->flosses());

    while (flossIterator.hasNext()) {
        Floss *floss = flossIterator.next();
        QListWidgetItem *listWidgetItem = new QListWidgetItem;
        listWidgetItem->setText(QString("%1 %2").arg(floss->name()).arg(floss->description()));
        listWidgetItem->setData(Qt::DecorationRole, QColor(floss->color()));
        listWidgetItem->setData(Qt::UserRole, QString(floss->name()));
        listWidgetItem->setData(Qt::UserRole + 1, QString(floss->description()));
        listWidgetItem->setData(Qt::CheckStateRole, Qt::Unchecked);
        ui.ColorList->addItem(listWidgetItem);
    }

    if (ui.ColorList->count()) {
        ui.ColorList->setCurrentItem(ui.ColorList->item(0));
    }
}


void CalibrateFlossDlg::updateSample()
{
    if (!m_sample) {
        m_sample = new QPixmap(ui.ExampleColor->size());
    }

    m_sample->fill(m_sampleColor);
    ui.ExampleColor->setPixmap(*m_sample);
    ui.RedSlider->setValue(m_sampleColor.red());
    ui.GreenSlider->setValue(m_sampleColor.green());
    ui.BlueSlider->setValue(m_sampleColor.blue());
}


void CalibrateFlossDlg::updateName(bool modified)
{
    KLocalizedString str = modified ? ki18nc("%1 is floss name, %2 is floss description", "%1-%2 (Modified)") : ki18nc("%1 is floss name, %2 is floss description", "%1-%2");
    ui.SelectedColorName->setText(str.subs(m_item->data(Qt::UserRole).toString()).subs(m_item->data(Qt::UserRole + 1).toString()).toString());
}


void CalibrateFlossDlg::commitColor()
{
    if (m_item && (m_item->data(Qt::DecorationRole).value<QColor>() != m_sampleColor)) {
        m_calibratedColors[m_schemeName][m_item->data(Qt::UserRole).toString()] = m_sampleColor;
        m_item->setData(Qt::CheckStateRole, Qt::Checked);
    }
}


void CalibrateFlossDlg::on_SchemeList_currentIndexChanged(const QString &)
{
    commitColor();
    fillColorList();
}


void CalibrateFlossDlg::on_ColorList_currentItemChanged(QListWidgetItem *item)
{
    if (m_item) {
        commitColor();
    }

    m_item = item;

    if (item) {
        if (m_calibratedColors[m_schemeName].contains(m_item->data(Qt::UserRole).toString())) {
            m_sampleColor = m_calibratedColors[m_schemeName][m_item->data(Qt::UserRole).toString()];
        } else {
            m_sampleColor = m_item->data(Qt::DecorationRole).value<QColor>();
        }

        updateSample();
        updateName(m_calibratedColors[m_schemeName].contains(m_item->data(Qt::UserRole).toString()));
    }
}


void CalibrateFlossDlg::on_RedSlider_valueChanged(int red)
{
    m_sampleColor.setRgb(red, m_sampleColor.green(), m_sampleColor.blue());
    updateSample();
    updateName(true);
}


void CalibrateFlossDlg::on_GreenSlider_valueChanged(int green)
{
    m_sampleColor.setRgb(m_sampleColor.red(), green, m_sampleColor.blue());
    updateSample();
    updateName(true);
}


void CalibrateFlossDlg::on_BlueSlider_valueChanged(int blue)
{
    m_sampleColor.setRgb(m_sampleColor.red(), m_sampleColor.green(), blue);
    updateSample();
    updateName(true);
}


void CalibrateFlossDlg::on_ResetColor_clicked()
{
    m_sampleColor = m_item->data(Qt::DecorationRole).value<QColor>();

    if (m_calibratedColors[m_schemeName].contains(m_item->data(Qt::UserRole).toString())) {
        m_calibratedColors[m_schemeName].remove(m_item->data(Qt::UserRole).toString());
    }

    updateSample();
    updateName(false);
    m_item->setData(Qt::CheckStateRole, Qt::Unchecked);
}


void CalibrateFlossDlg::on_DialogButtonBox_accepted()
{
    commitColor();

    QMapIterator<QString, ChangedColors> mapIterator(m_calibratedColors);

    while (mapIterator.hasNext()) {
        mapIterator.next();

        if (mapIterator.value().count()) {
            FlossScheme *scheme = SchemeManager::scheme(mapIterator.key());
            QListIterator<Floss *> flossIterator(scheme->flosses());

            while (flossIterator.hasNext()) {
                Floss *f = flossIterator.next();

                if (mapIterator.value().contains(f->name())) {
                    f->setColor(mapIterator.value()[f->name()]);
                }
            }

            SchemeManager::writeScheme(mapIterator.key());
        }
    }

    accept();
}


void CalibrateFlossDlg::on_DialogButtonBox_rejected()
{
    reject();
}


void CalibrateFlossDlg::on_DialogButtonBox_helpRequested()
{
    KHelpClient::invokeHelp("CalibrateDialog", "kxstitch");
}
