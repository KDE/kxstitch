/********************************************************************************
 *	Copyright (C) 2010 by Stephen Allewell					*
 *	stephen@mirramar.adsl24.co.uk						*
 *										*
 *	This program is free software; you can redistribute it and/or modify	*
 *	it under the terms of the GNU General Public License as published by	*
 *	the Free Software Foundation; either version 2 of the License, or	*
 *	(at your option) any later version.					*
 ********************************************************************************/


#include "NewFlossDlg.h"

#include <KDebug>
#include <KMessageBox>

#include "FlossScheme.h"


NewFlossDlg::NewFlossDlg(QWidget *parent, FlossScheme *flossScheme)
	:	KDialog(parent),
		m_flossScheme(flossScheme),
		m_floss(0)
{
	setCaption(i18n("New Floss"));
	setButtons(KDialog::Ok | KDialog::Cancel | KDialog::Help);
	setHelp("NewColorDialog");
	QWidget *widget = new QWidget(this);
	ui.setupUi(widget);
	QMetaObject::connectSlotsByName(this);
	ui.SchemeName->setText(m_flossScheme->schemeName());
	enableButtonOk(false);
	setMainWidget(widget);
}


NewFlossDlg::~NewFlossDlg()
{
}


Floss *NewFlossDlg::floss()
{
	return m_floss;
}


void NewFlossDlg::slotButtonClicked(int button)
{
	if (button == KDialog::Ok)
	{
		if (!m_flossScheme->find(ui.FlossName->text()) || KMessageBox::questionYesNo(this, i18n("The floss name %1 is already used.\nOverwrite with the description and color selected.", ui.FlossName->text()),
												i18n("Overwrite")) == KDialog::Yes)
		{
			m_floss = new Floss(ui.FlossName->text(), ui.FlossDescription->text(), ui.ColorButton->color());
			m_flossScheme->addFloss(m_floss);
			accept();
		}
	}
	else
		KDialog::slotButtonClicked(button);
}


void NewFlossDlg::on_FlossName_textEdited(const QString &text)
{
	if (!text.isEmpty() && !ui.FlossDescription->text().isEmpty())
		enableButtonOk(true);
	else
		enableButtonOk(false);
}


void NewFlossDlg::on_FlossDescription_textEdited(const QString &text)
{
	if (!text.isEmpty() && !ui.FlossName->text().isEmpty())
		enableButtonOk(true);
	else
		enableButtonOk(false);
}
