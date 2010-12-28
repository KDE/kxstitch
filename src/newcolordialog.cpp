/****************************************************************************
 *	Copyright (C) 2003 by Stephen Allewell									*
 *	stephen@mirramar.fsnet.co.uk											*
 *																			*
 *	This program is free software; you can redistribute it and/or modify	*
 *	it under the terms of the GNU General Public License as published by	*
 *	the Free Software Foundation; either version 2 of the License, or		*
 *	(at your option) any later version.										*
 ****************************************************************************/


#include <QLabel>
#include <QPushButton>

#include <KApplication>
#include <KColorButton>
#include <KLineEdit>
#include <KLocale>
#include <KMessageBox>

#include "floss.h"
#include "flossscheme.h"
#include "kxstitch.h"
#include "newcolordialog.h"
#include "schememanager.h"


NewColorDialog::NewColorDialog(QWidget *parent, QString schemeName)
  : NewColorDlg(parent, "NewColorDialog", true)
{
	SchemeName->setText(schemeName);
	connect(FlossName, SIGNAL(textChanged(const QString &)), this, SLOT(slotTextChanged(const QString &)));
	connect(FlossDescription, SIGNAL(textChanged(const QString &)), this, SLOT(slotTextChanged(const QString &)));
	connect(HelpButton, SIGNAL(clicked()), this, SLOT(slotContextHelp()));
	OKButton->setEnabled(false);
}


void NewColorDialog::slotContextHelp()
{
	kapp->invokeHelp("NewColorDialog");
}


void NewColorDialog::slotTextChanged(const QString &)
{
	if (FlossName->text().isEmpty() || FlossDescription->text().isEmpty())
		OKButton->setEnabled(false);
	else
		OKButton->setEnabled(true);
}


Floss *NewColorDialog::getFloss(QString schemeName)
{
	Floss *floss = 0;
	NewColorDialog *dialog = new NewColorDialog(0, schemeName);
	if (dialog->exec() == QDialog::Accepted)
	{
		FlossScheme *scheme = static_cast<KXStitchApplication*>(kapp)->scheme(schemeName);
		if (!scheme->find(dialog->FlossName->text()))
		{
			floss = new Floss(dialog->FlossName->text(), dialog->FlossDescription->text(), dialog->ColorButton->color());
			scheme->addFloss(floss);
		}
		else
			KMessageBox::sorry(0, i18n("The floss name specified has already been used."), i18n("Floss Name Used"), false);
	}
	delete dialog;
	return floss;
}
