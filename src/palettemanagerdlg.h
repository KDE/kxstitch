/****************************************************************************
 *	Copyright (C) 2010 by Stephen Allewell									*
 *	stephen@mirramar.adsl24.co.uk											*
 *																			*
 *	This program is free software; you can redistribute it and/or modify	*
 *	it under the terms of the GNU General Public License as published by	*
 *	the Free Software Foundation; either version 2 of the License, or		*
 *	(at your option) any later version.										*
 ****************************************************************************/


#ifndef __PALETTE_MANAGER_DLG
#define __PALETTE_MANAGER_DLG


#include <KDialog>

#include "document.h"
#include "ui_palettemanager.h"


class SchemeManager;

class DocumentFloss;


class PaletteManagerDlg : public KDialog
{
	Q_OBJECT

	public:
		PaletteManagerDlg(SchemeManager *, const QString &, QMap<int, DocumentFloss *> &, QMap<int, int> &);
		~PaletteManagerDlg();

	protected slots:
		virtual void slotButtonClicked(int button);

	private slots:
		void on_ColorList_currentRowChanged(int);
		void on_CurrentList_currentRowChanged(int);
		void on_AddFloss_clicked(bool);
		void on_RemoveFloss_clicked(bool);
		void on_StitchStrands_activated(int);
		void on_BackstitchStrands_activated(int);
		void on_FlossSymbol_clicked(bool);
		void on_NewFloss_clicked(bool);
		void on_ClearUnused_clicked(bool);
		void on_Calibrate_clicked(bool);

	private:
		void fillLists();
		void insertListWidgetItem(QListWidget *, QListWidgetItem *);
		bool contains(const Floss *);
		int paletteIndex(QString);
		int paletteIndex(Floss *);
		QChar freeSymbol();
		int freeIndex();

		Ui::PaletteManager ui;

		SchemeManager *m_schemeManager;
		QString m_schemeName;
		QString m_font;
		QChar m_selectedChar;
		QMap<int, DocumentFloss *> &m_documentPalette;
		QMap<int, DocumentFloss *> m_dialogPalette;
		QMap<int, int> &m_usedFlosses;
//		CharacterSelectDlg *m_characterSelectDlg;
};


#endif
