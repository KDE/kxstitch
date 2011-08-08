/********************************************************************************
 *	Copyright (C) 2010 by Stephen Allewell					*
 *	stephen@mirramar.adsl24.co.uk						*
 *										*
 *	This program is free software; you can redistribute it and/or modify	*
 *	it under the terms of the GNU General Public License as published by	*
 *	the Free Software Foundation; either version 2 of the License, or	*
 *	(at your option) any later version.					*
 ********************************************************************************/


#ifndef ExtendPatternDlg_H
#define ExtendPatternDlg_H


#include "ui_ExtendPattern.h"


class ExtendPatternDlg : public KDialog
{
	Q_OBJECT

	public:
		ExtendPatternDlg(QWidget *);
		~ExtendPatternDlg();

		int top() const;
		int left() const;
		int right() const;
		int bottom() const;

	private:
		Ui::ExtendPattern	ui;
};


#endif // ExtendPatternDlg_H
