/********************************************************************************
 *	Copyright (C) 2010 by Stephen Allewell					*
 *	stephen@mirramar.adsl24.co.uk						*
 *										*
 *	This program is free software; you can redistribute it and/or modify	*
 *	it under the terms of the GNU General Public License as published by	*
 *	the Free Software Foundation; either version 2 of the License, or	*
 *	(at your option) any later version.					*
 ********************************************************************************/


#ifndef __FilePropertiesDlg_H
#define __FilePropertiesDlg_H


class FilePropertiesDlg : public KDialog
{
	Q_OBJECT

	public:
		FilePropertiesDlg();
		~FilePropertiesDlg();

	protected slots:
		void slotButtonClicked(int);

	private slots:
		void on_widget_action(...);

	private:
		Ui::FileProperties	ui;
};


#endif
