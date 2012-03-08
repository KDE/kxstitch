/********************************************************************************
 *	Copyright (C) 2010 by Stephen Allewell					*
 *	stephen@mirramar.adsl24.co.uk						*
 *										*
 *	This program is free software; you can redistribute it and/or modify	*
 *	it under the terms of the GNU General Public License as published by	*
 *	the Free Software Foundation; either version 2 of the License, or	*
 *	(at your option) any later version.					*
 ********************************************************************************/


#include "PaletteManagerDlg.h"

#include <QWidget>

#include <KDebug>

#include "configuration.h"
#include "CalibrateFlossDlg.h"
#include "Commands.h"
#include "Floss.h"
#include "FlossScheme.h"
#include "NewFlossDlg.h"
#include "SchemeManager.h"


PaletteManagerDlg::PaletteManagerDlg(QWidget *parent, Document *document)
	:	KDialog(parent),
		m_document(document),
		m_schemeName(document->pattern()->palette().schemeName()),
		m_documentPalette(document->pattern()->palette().flosses()),
		m_flossUsage(document->pattern()->stitches().flossUsage()),
		m_scheme(SchemeManager::scheme(m_schemeName))
//		m_characterSelectDlg(0)
{
	setCaption(i18n("Palette Manager"));
	setButtons(KDialog::Ok | KDialog::Cancel | KDialog::Help);
	QMapIterator<int, DocumentFloss *> it(m_documentPalette);
	while (it.hasNext())
	{
		it.next();
		m_dialogPalette.insert(it.key(), new DocumentFloss(it.value()));
	}
	QWidget *widget = new QWidget(this);
	ui.setupUi(widget);
	QMetaObject::connectSlotsByName(this);
	setMainWidget(widget);

	fillLists();
}


PaletteManagerDlg::~PaletteManagerDlg()
{
//	delete m_characterSelectDlg;
}


QList<QUndoCommand *> PaletteManagerDlg::changes()
{
	return m_changes;
}


void PaletteManagerDlg::slotButtonClicked(int button)
{
	if (button == KDialog::Ok)
	{
		QMap<int, DocumentFloss *>::const_iterator it;
		for (it = m_documentPalette.begin() ; it != m_documentPalette.end() ; ++it)
		{
			if (m_dialogPalette.contains(it.key()))
			{
				if (*(it.value()) != *(m_dialogPalette.value(it.key())))
					m_changes.append(new ReplaceDocumentFlossCommand(m_document, it.key(), m_dialogPalette.value(it.key())));
			}
			else
				m_changes.append(new RemoveDocumentFlossCommand(m_document, it.key(), m_dialogPalette.value(it.key())));
		}

		for (it = m_dialogPalette.begin() ; it != m_dialogPalette.end() ; ++it)
		{
			if (!m_documentPalette.contains(it.key()))
				m_changes.append(new AddDocumentFlossCommand(m_document, it.key(), it.value()));
		}

		accept();
	}
	else
		KDialog::slotButtonClicked(button);
}


void PaletteManagerDlg::on_ColorList_currentRowChanged(int currentRow)
{
	ui.AddFloss->setEnabled(currentRow != -1);
}


void PaletteManagerDlg::on_CurrentList_currentRowChanged(int currentRow)
{
	if (currentRow != -1)
	{
		int i = paletteIndex(ui.CurrentList->currentItem()->data(Qt::UserRole).toString());
		ui.StitchStrands->setCurrentIndex(m_dialogPalette[i]->stitchStrands()-1);
		ui.BackstitchStrands->setCurrentIndex(m_dialogPalette[i]->backstitchStrands()-1);
		ui.StitchSymbol->setText(m_dialogPalette[i]->stitchSymbol());
		ui.StitchStrands->setEnabled(true);
		ui.BackstitchStrands->setEnabled(true);
		ui.StitchSymbol->setEnabled(true);
		ui.ClearUnused->setEnabled(true);
		if (m_flossUsage.contains(i) && m_flossUsage[i].totalStitches() != 0)
			ui.RemoveFloss->setEnabled(false);
		else
			ui.RemoveFloss->setEnabled(true);
	}
	else
	{
		ui.RemoveFloss->setEnabled(false);
		ui.StitchStrands->setEnabled(false);
		ui.BackstitchStrands->setEnabled(false);
		ui.StitchSymbol->setEnabled(false);
		ui.ClearUnused->setEnabled(false);
	}
}


void PaletteManagerDlg::on_AddFloss_clicked(bool)
{
	QListWidgetItem *listWidgetItem = ui.ColorList->takeItem(ui.ColorList->currentRow());

	int i = freeIndex();
	DocumentFloss *documentFloss = new DocumentFloss(
		listWidgetItem->data(Qt::UserRole).toString(),
		freeSymbol(),
		Qt::SolidLine,
		Configuration::palette_StitchStrands(),
		Configuration::palette_BackstitchStrands()
	);
	documentFloss->setFlossColor(m_scheme->find(listWidgetItem->data(Qt::UserRole).toString())->color());
	m_dialogPalette.insert(i, documentFloss);

	insertListWidgetItem(ui.CurrentList, listWidgetItem);
	ui.CurrentList->scrollToItem(listWidgetItem, QAbstractItemView::PositionAtCenter);
	ui.CurrentList->setCurrentItem(listWidgetItem);
}


void PaletteManagerDlg::on_RemoveFloss_clicked(bool)
{
	QListWidgetItem *listWidgetItem = ui.CurrentList->takeItem(ui.CurrentList->currentRow());
	int i = paletteIndex(listWidgetItem->data(Qt::UserRole).toString());
	m_dialogPalette.remove(i);

	insertListWidgetItem(ui.ColorList, listWidgetItem);
	ui.ColorList->scrollToItem(listWidgetItem, QAbstractItemView::PositionAtCenter);
	ui.ColorList->setCurrentItem(listWidgetItem);
}


void PaletteManagerDlg::on_StitchStrands_activated(int index)
{
	m_dialogPalette[paletteIndex(ui.CurrentList->currentItem()->data(Qt::UserRole).toString())]->setStitchStrands(index+1);
}


void PaletteManagerDlg::on_BackstitchStrands_activated(int index)
{
	m_dialogPalette[paletteIndex(ui.CurrentList->currentItem()->data(Qt::UserRole).toString())]->setBackstitchStrands(index+1);
}


void PaletteManagerDlg::on_StitchSymbol_clicked(bool)
{
}


void PaletteManagerDlg::on_BackstitchSymbol_clicked(bool)
{
}


void PaletteManagerDlg::on_NewFloss_clicked(bool)
{
	QPointer<NewFlossDlg> newFlossDlg = new NewFlossDlg(this, m_scheme);
	if (newFlossDlg->exec())
	{
		Floss *floss = newFlossDlg->floss();
		if (floss)
		{
			QListWidgetItem *listWidgetItem = new QListWidgetItem;
			listWidgetItem->setText(QString("%1 %2").arg(floss->name()).arg(floss->description()));
			listWidgetItem->setData(Qt::DecorationRole, QColor(floss->color()));
			listWidgetItem->setData(Qt::UserRole, QString(floss->name()));

			if (contains(floss->name()))
			{
				insertListWidgetItem(ui.CurrentList, listWidgetItem);
				if (m_flossUsage.contains(paletteIndex(floss->name())) && m_flossUsage[paletteIndex(floss->name())].totalStitches())
					listWidgetItem->setForeground(QBrush(Qt::gray));
			}
			else
				insertListWidgetItem(ui.ColorList, listWidgetItem);
		}
	}
	delete newFlossDlg;
}


void PaletteManagerDlg::on_ClearUnused_clicked(bool)
{
	for (int row = 0 ; row < ui.CurrentList->count() ; )
	{
		ui.CurrentList->setCurrentRow(row);
		QListWidgetItem *listWidgetItem = ui.CurrentList->currentItem();
		int i = paletteIndex(listWidgetItem->data(Qt::UserRole).toString());
		if (!m_flossUsage.contains(i) || m_flossUsage[i].totalStitches() == 0)
			on_RemoveFloss_clicked(true);
		else
			row++;
	}
}


void PaletteManagerDlg::on_Calibrate_clicked(bool)
{
	QPointer<CalibrateFlossDlg> calibrateFlossDlg = new CalibrateFlossDlg(this, m_schemeName);
	if (calibrateFlossDlg->exec() == QDialog::Accepted)
	{
		fillLists();
	}
}


void PaletteManagerDlg::fillLists()
{
	ui.ColorList->clear();
	ui.CurrentList->clear();

	QListIterator<Floss *> i = m_scheme->flosses();
	while (i.hasNext())
	{
		Floss *floss = i.next();

		QListWidgetItem *listWidgetItem = new QListWidgetItem;
		listWidgetItem->setText(QString("%1 %2").arg(floss->name()).arg(floss->description()));
		listWidgetItem->setData(Qt::DecorationRole, QColor(floss->color()));
		listWidgetItem->setData(Qt::UserRole, QString(floss->name()));

		if (contains(floss->name()))
		{
			insertListWidgetItem(ui.CurrentList, listWidgetItem);
			if (m_flossUsage.contains(paletteIndex(floss->name())) && m_flossUsage[paletteIndex(floss->name())].totalStitches())
				listWidgetItem->setForeground(QBrush(Qt::gray));
		}
		else
			insertListWidgetItem(ui.ColorList, listWidgetItem);
	}

	/* The following should have worked, but setting the currentRow to -1 doesn't emit
		the signal and consequently doesn't call the slot

	ui.CurrentList->setCurrentRow(ui.CurrentList->count()?0:-1);
	ui.ColorList->setCurrentRow(ui.ColorList->count()?0:-1);
	*/

	if (ui.CurrentList->count())
		ui.CurrentList->setCurrentRow(0);
	else
		on_CurrentList_currentRowChanged(-1);

	if (ui.ColorList->count())
		ui.ColorList->setCurrentRow(0);
	else
		on_ColorList_currentRowChanged(-1);
}


void PaletteManagerDlg::insertListWidgetItem(QListWidget *listWidget, QListWidgetItem *listWidgetItem)
{
	int rows = listWidget->count();

	for (int row = 0 ; row < rows ; row++)
	{
		if (listWidgetItem->data(Qt::UserRole).toInt() < listWidget->item(row)->data(Qt::UserRole).toInt())
		{
			do
			{
				QListWidgetItem *oldListWidgetItem = listWidget->item(row);
				listWidget->insertItem(row, listWidgetItem);
				listWidgetItem = oldListWidgetItem;
			} while (++row < rows);
		}
	}
	listWidget->addItem(listWidgetItem);
}


bool PaletteManagerDlg::contains(const QString &flossName)
{
	QMapIterator<int, DocumentFloss *> i(m_dialogPalette);
	while (i.hasNext())
	{
		if (i.next().value()->flossName() == flossName)
			return true;
	}
	return false;
}


int PaletteManagerDlg::paletteIndex(const QString &flossName)
{
	QMapIterator<int, DocumentFloss *> i(m_dialogPalette);
	while (i.hasNext())
	{
		if (i.next().value()->flossName() == flossName)
		return i.key();
	}

	return -1;
}

#if 0
int PaletteManagerDlg::paletteIndex(Floss *floss)
{
	QMapIterator<int, DocumentFloss *> i(m_dialogPalette);
	while (i.hasNext())
	{
		if (i.next().value()->floss() == floss)
		return i.key();
	}

	return -1;
}
#endif

QChar PaletteManagerDlg::freeSymbol()
{
	int c = -1;
	bool found = false;

	while (!found)
	{
		QChar stitchSymbol(++c);
		if (stitchSymbol.isPrint() && !stitchSymbol.isSpace() && !stitchSymbol.isPunct())
		{
			found = true;
			QMapIterator<int, DocumentFloss *> i(m_dialogPalette);
			while (i.hasNext() && found)
			{
				if (i.next().value()->stitchSymbol() == stitchSymbol)
				found = false;
			}
		}
	}

	return QChar(c);
}


int PaletteManagerDlg::freeIndex()
{
	int i = 0;
	while (m_dialogPalette.contains(i)) i++;
	return i;
}
