/********************************************************************************
 *	Copyright (C) 2010 by Stephen Allewell					*
 *	stephen@mirramar.adsl24.co.uk						*
 *										*
 *	This program is free software; you can redistribute it and/or modify	*
 *	it under the terms of the GNU General Public License as published by	*
 *	the Free Software Foundation; either version 2 of the License, or	*
 *	(at your option) any later version.					*
 ********************************************************************************/


#include "FilePropertiesDlg.h"


FilePropertiesDlg::FilePropertiesDlg(QWidget *parent)
	:	KDialog(parent)
{
	setCaption(i18n("File Properties"));
	setButtons(KDialog::Ok | KDialog::Cancel | KDialog::Help);
	QWidget *widget = new QWidget(this);
	ui.setupUi(widget);
	QMetaObject::connectSlotsByName(this);
	setMainWidget(widget);
}


FilePropertiesDlg::~FilePropertiesDlg()
{
}


void FilePropertiesDlg::slotButtonClicked(int button)
{
}


