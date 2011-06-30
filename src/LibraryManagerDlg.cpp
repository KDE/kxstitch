/********************************************************************************
 *	Copyright (C) 2010 by Stephen Allewell					*
 *	stephen@mirramar.adsl24.co.uk						*
 *										*
 *	This program is free software; you can redistribute it and/or modify	*
 *	it under the terms of the GNU General Public License as published by	*
 *	the Free Software Foundation; either version 2 of the License, or	*
 *	(at your option) any later version.					*
 ********************************************************************************/


#include <QClipboard>
#include <QDir>
#include <QFileInfoList>
#include <QMimeData>

#include <KStandardDirs>

#include "LibraryManagerDlg.h"
#include "LibraryListWidgetItem.h"
#include "LibraryTreeWidgetItem.h"


LibraryManagerDlg::LibraryManagerDlg(QWidget *parent)
	:	KDialog(parent)
{
	setCaption(i18n("Library Manager"));
	setButtons(KDialog::Close | KDialog::Help);
	setContextMenuPolicy(Qt::CustomContextMenu);
	QWidget *widget = new QWidget(this);
	ui.setupUi(widget);
	QMetaObject::connectSlotsByName(this);
	setMainWidget(widget);
}


LibraryManagerDlg::~LibraryManagerDlg()
{
}


void LibraryManagerDlg::slotButtonClicked(int button)
{
	KDialog::slotButtonClicked(button);
}


void LibraryManagerDlg::on_LibraryTree_customContextMenuRequested(const QPoint &position)
{
	m_contextMenu.clear();
	m_contextMenu.addAction(i18n("New Catagory"), this, SLOT(newTreeCatagory()));
	if (ui.LibraryTree->itemAt(position))
	{
		m_contextMenu.addAction(i18n("Add to Export List"), this, SLOT(addLibraryToExportList()));
		m_contextMenu.addAction(i18n("Properties..."), this, SLOT(patternLibraryProperties()));
		if (QApplication::clipboard()->mimeData()->hasFormat("application/kxstitch"))
		{
			m_contextMenu.addAction(i18n("Paste"), this, SLOT(pasteFromClipboard()));
		}
	}
	m_contextMenu.popup(position);
}


void LibraryManagerDlg::on_LibraryIcons_customContextMenuRequested(const QPoint &position)
{
	m_contextMenu.clear();
	if (ui.LibraryIcons->itemAt(position))
	{
		m_contextMenu.addAction(i18n("Properties..."), this, SLOT(libraryPatternProperties()));
		m_contextMenu.addAction(i18n("Add to Export List"), this, SLOT(addLibraryPatternToExportList()));
		m_contextMenu.addAction(i18n("Copy"), this, SLOT(copyToClipboard()));
		m_contextMenu.addAction(i18n("Delete"), this, SLOT(deleteLibraryPattern()));
		m_contextMenu.popup(position);
	}
	else
	{
		if (QApplication::clipboard()->mimeData()->hasFormat("application/kxstitch") && ui.LibraryTree->selectedItems().count() == 1)
		{
			m_contextMenu.addAction(i18n("Paste"), this, SLOT(pasteFromClipboard()));
			m_contextMenu.popup(position);
		}
	}
}


void LibraryManagerDlg::on_LibraryTree_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *)
{
	ui.LibraryIcons->clear();
	LibraryTreeWidgetItem *libraryTreeWidgetItem = dynamic_cast<LibraryTreeWidgetItem *>(current);
	for (LibraryPattern *libraryPattern = libraryTreeWidgetItem->first() ; libraryPattern ; libraryPattern = libraryTreeWidgetItem->next())
	{
		LibraryListWidgetItem *libraryListWidgetItem = new LibraryListWidgetItem(ui.LibraryIcons, libraryPattern);
		libraryPattern->setLibraryListWidgetItem(libraryListWidgetItem);
	}
}


void LibraryManagerDlg::refreshLibraries()
{
	ui.LibraryTree->clear();
	QStringList libraryDirectories = KGlobal::dirs()->findDirs("appdata", "library");
	QStringListIterator libraryDirectoriesIterator(libraryDirectories);
	while (libraryDirectoriesIterator.hasNext())
	{
		recurseLibraryDirectory(0, libraryDirectoriesIterator.next());
	}
}


void LibraryManagerDlg::recurseLibraryDirectory(LibraryTreeWidgetItem *parent, const QString &path)
{
	LibraryTreeWidgetItem *libraryTreeWidgetItem = 0;
	QDir directory(path);
	const QFileInfoList directoryEntries = directory.entryInfoList();
	QListIterator<QFileInfo> fileInfoListIterator(directoryEntries);
	while (fileInfoListIterator.hasNext())
	{
		QFileInfo fileInfo = fileInfoListIterator.next();
		if (fileInfo.isDir() && fileInfo.fileName() != "." && fileInfo.fileName() != "..")
		{
			QString subPath = QString("%1%2/").arg(path).arg(fileInfo.fileName());
			if (parent)
			{
				int children = parent->childCount();
				int childIndex = 0;
				while (childIndex < children)
				{
					libraryTreeWidgetItem = dynamic_cast<LibraryTreeWidgetItem *>(parent->child(childIndex));
					if (libraryTreeWidgetItem->text(0) == fileInfo.fileName())
						break;
					childIndex++;
				}
				if (childIndex == children)
					libraryTreeWidgetItem = 0;
			}
			else
			{
				// this is a root node
				QList<QTreeWidgetItem *> rootNodes = ui.LibraryTree->findItems(fileInfo.fileName(), Qt::MatchExactly, 0);
				if (!rootNodes.isEmpty())
				{
					libraryTreeWidgetItem = dynamic_cast<LibraryTreeWidgetItem *>(rootNodes[0]);
				}
			}

			if (libraryTreeWidgetItem == 0)
			{
				if (parent)
				{
					libraryTreeWidgetItem = new LibraryTreeWidgetItem(parent, fileInfo.fileName());
				}
				else
				{
					libraryTreeWidgetItem = new LibraryTreeWidgetItem(ui.LibraryTree, fileInfo.fileName());
				}
			}

			libraryTreeWidgetItem->addPath(subPath);
			recurseLibraryDirectory(libraryTreeWidgetItem, subPath);
		}
	}
}


