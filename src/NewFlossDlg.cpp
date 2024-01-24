/*
 * Copyright (C) 2010-2015 by Stephen Allewell
 * steve.allewell@gmail.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include "NewFlossDlg.h"

#include <KConfigGroup>
#include <KHelpClient>
#include <KLocalizedString>
#include <KMessageBox>
#include <KSharedConfig>
#include <kwidgetsaddons_version.h>

#include "FlossScheme.h"
#include "SchemeManager.h"

NewFlossDlg::NewFlossDlg(QWidget *parent, FlossScheme *flossScheme)
    : QDialog(parent)
    , m_flossScheme(flossScheme)
    , m_floss(nullptr)
{
    setWindowTitle(i18n("New Floss"));
    ui.setupUi(this);

    ui.SchemeName->setText(m_flossScheme->schemeName());
    ui.DialogButtonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
}

Floss *NewFlossDlg::floss()
{
    return m_floss;
}

void NewFlossDlg::hideEvent(QHideEvent *event)
{
    KConfigGroup(KSharedConfig::openConfig(), QStringLiteral("DialogSizes")).writeEntry(QStringLiteral("NewFlossDlg"), size());

    QDialog::hideEvent(event);
}

void NewFlossDlg::showEvent(QShowEvent *event)
{
    QDialog::showEvent(event);

    if (KConfigGroup(KSharedConfig::openConfig(), QStringLiteral("DialogSizes")).hasKey(QStringLiteral("NewFlossDlg"))) {
        resize(KConfigGroup(KSharedConfig::openConfig(), QStringLiteral("DialogSizes")).readEntry(QStringLiteral("NewFlossDlg"), QSize()));
    }
}

void NewFlossDlg::on_FlossName_textEdited(const QString &text)
{
    if (!text.isEmpty() && !ui.FlossDescription->text().isEmpty()) {
        ui.DialogButtonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
    } else {
        ui.DialogButtonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
    }
}

void NewFlossDlg::on_FlossDescription_textEdited(const QString &text)
{
    if (!text.isEmpty() && !ui.FlossName->text().isEmpty()) {
        ui.DialogButtonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
    } else {
        ui.DialogButtonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
    }
}

void NewFlossDlg::on_DialogButtonBox_accepted()
{
    if (!m_flossScheme->find(ui.FlossName->text()) ||
#if KWIDGETSADDONS_VERSION >= QT_VERSION_CHECK(5, 100, 0)
        KMessageBox::questionTwoActions(this,
#else
        KMessageBox::questionYesNo(this,
#endif
                                        i18n("The floss name %1 is already used.\nOverwrite with the description and color selected.", ui.FlossName->text()),
                                        i18n("Overwrite"),
                                        KStandardGuiItem::overwrite(),
                                        KGuiItem(i18nc("@action:button", "Do Not Overwrite"), QStringLiteral("dialog-cancel")))
#if KWIDGETSADDONS_VERSION >= QT_VERSION_CHECK(5, 100, 0)
            == KMessageBox::PrimaryAction) {
#else
            == KMessageBox::Yes) {
#endif
        m_floss = new Floss(ui.FlossName->text(), ui.FlossDescription->text(), ui.ColorButton->color());
        m_flossScheme->addFloss(m_floss);
        SchemeManager::writeScheme(m_flossScheme->schemeName());
    }

    accept();
}

void NewFlossDlg::on_DialogButtonBox_rejected()
{
    reject();
}

void NewFlossDlg::on_DialogButtonBox_helpRequested()
{
    KHelpClient::invokeHelp(QStringLiteral("NewFlossDialog"), QStringLiteral("kxstitch"));
}

#include "moc_NewFlossDlg.cpp"
