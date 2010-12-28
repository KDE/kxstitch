/****************************************************************************
 *	Copyright (C) 2003 by Stephen Allewell									*
 *	stephen@mirramar.fsnet.co.uk											*
 *																			*
 *	This program is free software; you can redistribute it and/or modify	*
 *	it under the terms of the GNU General Public License as published by	*
 *	the Free Software Foundation; either version 2 of the License, or		*
 *	(at your option) any later version.										*
 ****************************************************************************/


#ifndef __NEWCOLORDIALOG_H
#define __NEWCOLORDIALOG_H


#include <qmap.h>
#include <qstring.h>
#include <qcolor.h>

#include "newcolordlg.h"


class FlossListBoxItem;
class Floss;


/**
	Wrapper class to manage the creation of new floss colors
	@author Stephen P Allewell
  */
class NewColorDialog : public NewColorDlg
{
		Q_OBJECT
	public:
		NewColorDialog(QWidget *parent, QString schemeName);

		static Floss* getFloss(QString schemeName);

	protected slots:
		void slotContextHelp();
  
	private slots:
		void slotTextChanged(const QString&);
};


#endif
