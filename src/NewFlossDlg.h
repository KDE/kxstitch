/********************************************************************************
 *	Copyright (C) 2010 by Stephen Allewell					*
 *	stephen@mirramar.adsl24.co.uk						*
 *										*
 *	This program is free software; you can redistribute it and/or modify	*
 *	it under the terms of the GNU General Public License as published by	*
 *	the Free Software Foundation; either version 2 of the License, or	*
 *	(at your option) any later version.					*
 ********************************************************************************/


#ifndef NewFlossDlg_H
#define NewFlossDlg_H


#include <KDialog>

#include "ui_NewFloss.h"


class QString;
class QWidget;

class Floss;
class FlossScheme;


class NewFlossDlg : public KDialog
{
	Q_OBJECT

	public:
		NewFlossDlg(QWidget *, FlossScheme *flossScheme);
		~NewFlossDlg();

		Floss *floss();

	protected slots:
		virtual void slotButtonClicked(int);

	private slots:
		void on_FlossName_textEdited(const QString &);
		void on_FlossDescription_textEdited(const QString &);

	private:
		Ui::NewFloss ui;

		FlossScheme	*m_flossScheme;
		Floss		*m_floss;
};


#endif // NewFlossDlg_H
