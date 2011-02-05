/****************************************************************************
 *	Copyright (C) 2010 by Stephen Allewell									*
 *	stephen@mirramar.adsl24.co.uk											*
 *																			*
 *	This program is free software; you can redistribute it and/or modify	*
 *	it under the terms of the GNU General Public License as published by	*
 *	the Free Software Foundation; either version 2 of the License, or		*
 *	(at your option) any later version.										*
 ****************************************************************************/


#include <QList>
#include <QMap>
#include <QWidget>

#include <KDebug>

#include "commands.h"
#include "filepropertiesdlg.h"
#include "floss.h"
#include "flossscheme.h"
#include "schememanager.h"


FilePropertiesDlg::FilePropertiesDlg(Document *document)
  : KDialog(0),
	m_document(document)
{
	setCaption(i18n("File Properties"));
	setButtons(KDialog::Ok | KDialog::Cancel | KDialog::Help);

	QWidget *widget = new QWidget(this);
	ui.setupUi(widget);

	QRect extents = m_document->extents();
	m_minWidth = extents.width();
	m_minHeight = extents.height();
	m_width = m_document->width();
	m_height = m_document->height();
	m_horizontalClothCount = m_document->property("horizontalClothCount").toDouble();
	m_verticalClothCount = m_document->property("verticalClothCount").toDouble();
	m_clothCountLink = m_document->property("clothCountLink").toBool();
	m_clothCountUnits = static_cast<Configuration::EnumEditor_ClothCountUnits::type>(m_document->property("clothCountUnits").toInt());
	m_unitsFormat = static_cast<Configuration::EnumDocument_UnitsFormat::type>(m_document->property("unitsFormat").toInt());
	ui.UnitsFormat->setCurrentIndex(m_unitsFormat);
	updatePatternSizes();
	ui.PatternWidth->setMinimum(m_minWidth);
	ui.PatternHeight->setMinimum(m_minHeight);
	ui.PatternTitle->setText(m_document->property("title").toString());
	ui.PatternAuthor->setText(m_document->property("author").toString());
	ui.PatternCopyright->setText(m_document->property("copyright").toString());
	ui.PatternFabric->setText(m_document->property("fabric").toString());
	ui.FabricColor->setColor(m_document->property("fabricColor").value<QColor>());
	ui.FlossScheme->addItems(m_document->schemeManager()->schemes());
	ui.FlossScheme->setCurrentItem(m_document->property("flossSchemeName").toString());
	ui.Instructions->setPlainText(m_document->property("instructions").toString());
	ui.ClothCountLink->setChecked(m_clothCountLink);
	on_ClothCountLink_clicked(m_clothCountLink);

	QMetaObject::connectSlotsByName(this);
	setMainWidget(widget);
}


FilePropertiesDlg::~FilePropertiesDlg()
{
}


void FilePropertiesDlg::slotButtonClicked(int button)
{
	if (button == KDialog::Ok)
	{
		QList<QUndoCommand *> changes;
		if ((m_width != m_document->width()) || (m_height != m_document->height()))
			changes.append(new ResizeDocumentCommand(m_document, m_width, m_height));
		if (m_unitsFormat != static_cast<Configuration::EnumDocument_UnitsFormat::type>(m_document->property("unitsFormat").toInt()))
			changes.append(new SetPropertyCommand(m_document, "unitsFormat", QVariant(m_unitsFormat)));
		if (m_horizontalClothCount != m_document->property("horizontalClothCount").toDouble())
			changes.append(new SetPropertyCommand(m_document, "horizontalClothCount", QVariant(m_horizontalClothCount)));
		if (ui.ClothCountLink->isChecked() != m_document->property("clothCountLink").toBool())
			changes.append(new SetPropertyCommand(m_document, "clothCountLink", QVariant(ui.ClothCountLink->isChecked())));
		if (m_verticalClothCount != m_document->property("verticalClothCount").toDouble())
			changes.append(new SetPropertyCommand(m_document, "verticalClothCount", QVariant(m_verticalClothCount)));
		if (m_clothCountUnits != static_cast<Configuration::EnumEditor_ClothCountUnits::type>(m_document->property("clothCountUnits").toInt()))
			changes.append(new SetPropertyCommand(m_document, "clothCountUnits", QVariant(m_clothCountUnits)));
		if (ui.PatternTitle->text() != m_document->property("title").toString())
			changes.append(new SetPropertyCommand(m_document, "title", QVariant(ui.PatternTitle->text())));
		if (ui.PatternAuthor->text() != m_document->property("author").toString())
			changes.append(new SetPropertyCommand(m_document, "author", QVariant(ui.PatternAuthor->text())));
		if (ui.PatternCopyright->text() != m_document->property("copyright").toString())
			changes.append(new SetPropertyCommand(m_document, "copyright", QVariant(ui.PatternCopyright->text())));
		if (ui.PatternFabric->text() != m_document->property("fabric").toString())
			changes.append(new SetPropertyCommand(m_document, "fabric", QVariant(ui.PatternFabric->text())));
		if (ui.FabricColor->color() != m_document->property("fabricColor").value<QColor>())
			changes.append(new SetPropertyCommand(m_document, "fabricColor", QVariant(ui.FabricColor->color())));
		if (ui.Instructions->toPlainText() != m_document->property("instructions").toString())
			changes.append(new SetPropertyCommand(m_document, "instructions", QVariant(ui.Instructions->toPlainText())));

		if (ui.FlossScheme->currentText() != m_document->property("flossSchemeName").toString())
		{
			changes.append(new SetPropertyCommand(m_document, "flossSchemeName", QVariant(ui.FlossScheme->currentText())));
			QMapIterator<int, DocumentFloss *> it(m_document->palette());
			SchemeManager *manager = m_document->schemeManager();
			FlossScheme *scheme = manager->scheme(ui.FlossScheme->currentText());	// get a pointer to the new scheme selected
			while (it.hasNext())
			{
				it.next();
				DocumentFloss *documentFloss = it.value();
				Floss *floss = scheme->convert(documentFloss->floss()->color());
				changes.append(new ChangeFlossColorCommand(m_document, documentFloss, floss));
			}
		}

		if (!changes.isEmpty())
		{
			QListIterator<QUndoCommand *> it(changes);
			m_document->undoStack().beginMacro(i18n("File Properties"));
			while (it.hasNext())
				m_document->undoStack().push(it.next());
			m_document->undoStack().endMacro();
		}

		accept();
	}
	else
		KDialog::slotButtonClicked(button);
}


void FilePropertiesDlg::on_UnitsFormat_activated(int index)
{
	m_unitsFormat = static_cast<Configuration::EnumDocument_UnitsFormat::type>(index);
	switch (m_unitsFormat)
	{
		case Configuration::EnumDocument_UnitsFormat::Stitches:
			break;

		case Configuration::EnumDocument_UnitsFormat::Inches:
			if (m_clothCountUnits == Configuration::EnumEditor_ClothCountUnits::CM)
			{
				m_horizontalClothCount *= 2.54;
				m_verticalClothCount *= 2.54;
			}
			m_clothCountUnits = Configuration::EnumEditor_ClothCountUnits::Inches;
			break;

		case Configuration::EnumDocument_UnitsFormat::CM:
			if (m_clothCountUnits == Configuration::EnumEditor_ClothCountUnits::Inches)
			{
				m_horizontalClothCount /= 2.54;
				m_verticalClothCount /= 2.54;
			}
			m_clothCountUnits = Configuration::EnumEditor_ClothCountUnits::CM;
			break;
	}
	updatePatternSizes();
}


void FilePropertiesDlg::on_PatternWidth_valueChanged(double d)
{
	if (m_unitsFormat != Configuration::EnumDocument_UnitsFormat::Stitches)
		d *= m_horizontalClothCount;
	m_width = (int)d;
}


void FilePropertiesDlg::on_PatternHeight_valueChanged(double d)
{
	if (m_unitsFormat != Configuration::EnumDocument_UnitsFormat::Stitches)
		d *= m_horizontalClothCount;
	m_height = (int)d;
}


void FilePropertiesDlg::on_HorizontalClothCount_valueChanged(double d)
{
	m_horizontalClothCount = d;
	if (ui.ClothCountLink->isChecked())
		ui.VerticalClothCount->setValue(d);
	if (m_unitsFormat != Configuration::EnumDocument_UnitsFormat::Stitches)
		ui.PatternWidth->setValue(m_width / m_horizontalClothCount);
}


void FilePropertiesDlg::on_VerticalClothCount_valueChanged(double d)
{
	m_verticalClothCount = d;
	if (m_unitsFormat != Configuration::EnumDocument_UnitsFormat::Stitches)
		ui.PatternHeight->setValue(m_height / m_verticalClothCount);
}


void FilePropertiesDlg::on_ClothCountLink_clicked(bool checked)
{
	ui.ClothCountLink->setIcon((checked)?KIcon("link"):KIcon("link_break"));
	if (checked)
	{
		ui.VerticalClothCount->setValue(ui.HorizontalClothCount->value());
		ui.VerticalClothCount->setEnabled(false);
	}
	else
		ui.VerticalClothCount->setEnabled(true);
}

void FilePropertiesDlg::updatePatternSizes()
{
	ui.UnitsFormat->setCurrentIndex(m_unitsFormat);
	double horizontalScale = (m_unitsFormat == Configuration::EnumDocument_UnitsFormat::Stitches)?1:m_horizontalClothCount;
	double verticalScale = (m_unitsFormat == Configuration::EnumDocument_UnitsFormat::Stitches)?1:m_verticalClothCount;
	ui.PatternWidth->setValue(m_width/horizontalScale);
	ui.PatternHeight->setValue(m_height/verticalScale);
	ui.HorizontalClothCount->setValue(m_horizontalClothCount);
	ui.VerticalClothCount->setValue(m_verticalClothCount);
	QString suffix = ((m_clothCountUnits == Configuration::EnumEditor_ClothCountUnits::CM)?"/cm":"/in");
	ui.HorizontalClothCount->setSuffix(suffix);
	ui.VerticalClothCount->setSuffix(suffix);
	double step = ((m_unitsFormat == Configuration::EnumDocument_UnitsFormat::Stitches)?1:0.01);
	ui.PatternWidth->setSingleStep(step);
	ui.PatternHeight->setSingleStep(step);
}
