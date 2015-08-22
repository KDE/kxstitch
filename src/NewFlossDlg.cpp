/*
 * Copyright (C) 2010-2014 by Stephen Allewell
 * steve.allewell@gmail.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#include "NewFlossDlg.h"

#include <KHelpClient>
#include <KLocalizedString>
#include <KMessageBox>

#include "FlossScheme.h"


NewFlossDlg::NewFlossDlg(QWidget *parent, FlossScheme *flossScheme)
    :   QDialog(parent),
        m_flossScheme(flossScheme),
        m_floss(0)
{
    setWindowTitle(i18n("New Floss"));
    QWidget *widget = new QWidget(this);
    ui.setupUi(widget);
    QMetaObject::connectSlotsByName(this);
    ui.SchemeName->setText(m_flossScheme->schemeName());
    ui.DialogButtonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
}


NewFlossDlg::~NewFlossDlg()
{
}


Floss *NewFlossDlg::floss()
{
    return m_floss;
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
        KMessageBox::questionYesNo(this, i18n("The floss name %1 is already used.\nOverwrite with the description and color selected.", ui.FlossName->text()), i18n("Overwrite")) == KMessageBox::Yes) {
        m_floss = new Floss(ui.FlossName->text(), ui.FlossDescription->text(), ui.ColorButton->color());
        m_flossScheme->addFloss(m_floss);
    }

    accept();
}


void NewFlossDlg::on_DialogButtonBox_rejected()
{
    reject();
}


void NewFlossDlg::on_DialogButtonBox_helpRequested()
{
    KHelpClient::invokeHelp("NewFlossDialog", "kxstitch");
}
