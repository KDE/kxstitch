/********************************************************************************
 *	Copyright (C) 2010 by Stephen Allewell					*
 *	stephen@mirramar.adsl24.co.uk						*
 *										*
 *	This program is free software; you can redistribute it and/or modify	*
 *	it under the terms of the GNU General Public License as published by	*
 *	the Free Software Foundation; either version 2 of the License, or	*
 *	(at your option) any later version.					*
 ********************************************************************************/


#ifndef __LibraryManagerDlg_H
#define __LibraryManagerDlg_H


#include <QMenu>
#include <QTreeWidgetItem>
#include <QWidget>

#include <KDialog>

#include "ui_LibraryManager.h"


class LibraryTreeWidgetItem;


class LibraryManagerDlg : public KDialog
{
	Q_OBJECT

	public:
		LibraryManagerDlg(QWidget *parent);
		~LibraryManagerDlg();

	protected slots:
		void slotButtonClicked(int);

	private slots:
		void on_LibraryTree_customContextMenuRequested(const QPoint &);
		void on_LibraryIcons_customContextMenuRequested(const QPoint &);
		void on_LibraryTree_currentItemChanged(QTreeWidgetItem *, QTreeWidgetItem *);

	private:
		void refreshLibraries();
		void recurseLibraryDirectory(LibraryTreeWidgetItem *, const QString &);

		QMenu			m_contextMenu;
		Ui::LibraryManager	ui;
};


#endif
