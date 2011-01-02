/****************************************************************************
 *	Copyright (C) 2010 by Stephen Allewell									*
 *	stephen@mirramar.adsl24.co.uk											*
 *																			*
 *	This program is free software; you can redistribute it and/or modify	*
 *	it under the terms of the GNU General Public License as published by	*
 *	the Free Software Foundation; either version 2 of the License, or		*
 *	(at your option) any later version.										*
 ****************************************************************************/


#include <QWidget>

#include "configuration.h"
#include "floss.h"
#include "flossscheme.h"
#include "palettemanagerdlg.h"
#include "schememanager.h"


PaletteManagerDlg::PaletteManagerDlg(SchemeManager *schemeManager, QString &schemeName, QMap<int, Document::FLOSS> &palette, QMap<int, int> &usedFlosses)
  : KDialog(0),
//	m_characterSelectDlg(0),
	m_schemeManager(schemeManager),
	m_schemeName(schemeName),
	m_documentPalette(palette),
	m_dialogPalette(palette),
	m_usedFlosses(usedFlosses)
{
	setCaption(i18n("Palette Manager"));
	setButtons(KDialog::Ok | KDialog::Cancel | KDialog::Help);
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


void PaletteManagerDlg::slotButtonClicked(int button)
{
	if (button == KDialog::Ok)
	{
		m_documentPalette.clear();
		QMapIterator<int, Document::FLOSS> i(m_dialogPalette);
		while (i.hasNext())
		{
			i.next();
			m_documentPalette.insert(i.key(), i.value());
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
		ui.StitchStrands->setCurrentIndex(m_dialogPalette[i].stitchStrands-1);
		ui.BackstitchStrands->setCurrentIndex(m_dialogPalette[i].backstitchStrands-1);
		ui.FlossSymbol->setText(m_dialogPalette[i].symbol);
		ui.StitchStrands->setEnabled(true);
		ui.BackstitchStrands->setEnabled(true);
		ui.FlossSymbol->setEnabled(true);
		ui.ClearUnused->setEnabled(true);
		if (m_usedFlosses.contains(i) && m_usedFlosses[i] != 0)
			ui.RemoveFloss->setEnabled(false);
		else
			ui.RemoveFloss->setEnabled(true);
	}
	else
	{
		ui.RemoveFloss->setEnabled(false);
		ui.StitchStrands->setEnabled(false);
		ui.BackstitchStrands->setEnabled(false);
		ui.FlossSymbol->setEnabled(false);
		ui.ClearUnused->setEnabled(false);
	}
}


void PaletteManagerDlg::on_AddFloss_clicked(bool)
{
	QListWidgetItem *listWidgetItem = ui.ColorList->takeItem(ui.ColorList->currentRow());

	int i = freeIndex();
	Document::FLOSS floss = {
		m_schemeManager->scheme(m_schemeName)->find(listWidgetItem->data(Qt::UserRole).toString()),
		freeSymbol(),
		Configuration::palette_StitchStrands(),
		Configuration::palette_BackstitchStrands()
	};
	m_dialogPalette.insert(i, floss);

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
	m_dialogPalette[paletteIndex(ui.CurrentList->currentItem()->data(Qt::UserRole).toString())].stitchStrands = index+1;
}


void PaletteManagerDlg::on_BackstitchStrands_activated(int index)
{
	m_dialogPalette[paletteIndex(ui.CurrentList->currentItem()->data(Qt::UserRole).toString())].backstitchStrands = index+1;
}


void PaletteManagerDlg::on_FlossSymbol_clicked(bool)
{
}


void PaletteManagerDlg::on_NewFloss_clicked(bool)
{
  #if 0
	Floss *floss = NewFlossDlg::getFloss(m_scheme);
	if (floss)
		new FlossListWidgetItem(ColorList, floss);
  #endif
}


void PaletteManagerDlg::on_ClearUnused_clicked(bool)
{
	for (int row = 0 ; row < ui.CurrentList->count() ; )
	{
		ui.CurrentList->setCurrentRow(row);
		QListWidgetItem *listWidgetItem = ui.CurrentList->currentItem();
		int i = paletteIndex(listWidgetItem->data(Qt::UserRole).toString());
		if (!m_usedFlosses.contains(i) || m_usedFlosses[i] == 0)
			on_RemoveFloss_clicked(true);
		else
			row++;
	}
}


void PaletteManagerDlg::on_Calibrate_clicked(bool)
{
#if 0
	CalibrateDlg calibrateDlg = new CalibrateDlg(this);
	if (dlg->exec() == QDialog::Accepted)
	{
		fillFlossList();
		fillCurrentList();
	}
#endif
}


void PaletteManagerDlg::fillLists()
{
	ui.ColorList->clear();
	ui.CurrentList->clear();

	QListIterator<Floss *> i = m_schemeManager->scheme(m_schemeName)->flossIterator();
	while (i.hasNext())
	{
		Floss *floss = i.next();

		QListWidgetItem *listWidgetItem = new QListWidgetItem;
		listWidgetItem->setText(QString("%1 %2").arg(floss->name).arg(floss->description));
		listWidgetItem->setData(Qt::DecorationRole, QColor(floss->color));
		listWidgetItem->setData(Qt::UserRole, QString(floss->name));

		if (contains(floss))
		{
			insertListWidgetItem(ui.CurrentList, listWidgetItem);
			if (m_usedFlosses.contains(paletteIndex(floss->name)) && m_usedFlosses[paletteIndex(floss->name)])
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


bool PaletteManagerDlg::contains(const Floss *floss)
{
	QMapIterator<int, Document::FLOSS> i(m_dialogPalette);
	while (i.hasNext())
	{
		if (i.next().value().floss == floss)
		return true;
	}
	return false;
}


int PaletteManagerDlg::paletteIndex(const QString name)
{
	QMapIterator<int, Document::FLOSS> i(m_dialogPalette);
	while (i.hasNext())
	{
		if (i.next().value().floss->name == name)
		return i.key();
	}

	return -1;
}


int PaletteManagerDlg::paletteIndex(Floss *floss)
{
	QMapIterator<int, Document::FLOSS> i(m_dialogPalette);
	while (i.hasNext())
	{
		if (i.next().value().floss == floss)
		return i.key();
	}

	return -1;
}


QChar PaletteManagerDlg::freeSymbol()
{
	int c = -1;
	bool found = false;

	while (!found)
	{
		QChar symbol(++c);
		if (symbol.isPrint() && !symbol.isSpace() && !symbol.isPunct())
		{
			found = true;
			QMapIterator<int, Document::FLOSS> i(m_dialogPalette);
			while (i.hasNext() && found)
			{
				if (i.next().value().symbol == symbol)
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
