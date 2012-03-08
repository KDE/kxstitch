/********************************************************************************
 *	Copyright (C) 2010 by Stephen Allewell					*
 *	stephen@mirramar.adsl24.co.uk						*
 *										*
 *	This program is free software; you can redistribute it and/or modify	*
 *	it under the terms of the GNU General Public License as published by	*
 *	the Free Software Foundation; either version 2 of the License, or	*
 *	(at your option) any later version.					*
 ********************************************************************************/


#include "FilePropertiesDlg.h"

#include <QRect>

#include "Commands.h"
#include "Document.h"
#include "Floss.h"
#include "FlossScheme.h"
#include "SchemeManager.h"


FilePropertiesDlg::FilePropertiesDlg(QWidget *parent, Document *document)
	:	KDialog(parent),
		m_document(document)
{
	setCaption(i18n("File Properties"));
	setButtons(KDialog::Ok | KDialog::Cancel | KDialog::Help);
	QWidget *widget = new QWidget(this);
	ui.setupUi(widget);

	QRect extents = m_document->pattern()->stitches().extents();
	m_minWidth = extents.width();
	m_minHeight = extents.height();
	m_width = m_document->pattern()->stitches().width();
	m_height = m_document->pattern()->stitches().height();
	m_horizontalClothCount = m_document->property("horizontalClothCount").toDouble();
	m_verticalClothCount = m_document->property("verticalClothCount").toDouble();
	m_clothCountLink = m_document->property("clothCountLink").toBool();
	m_clothCountUnits = static_cast<Configuration::EnumEditor_ClothCountUnits::type>(m_document->property("clothCountUnits").toInt());
	m_unitsFormat = static_cast<Configuration::EnumDocument_UnitsFormat::type>(m_document->property("unitsFormat").toInt());
	ui.UnitsFormat->setCurrentIndex(m_unitsFormat);
	updatePatternSizes();
	ui.PatternTitle->setText(m_document->property("title").toString());
	ui.PatternAuthor->setText(m_document->property("author").toString());
	ui.PatternCopyright->setText(m_document->property("copyright").toString());
	ui.PatternFabric->setText(m_document->property("fabric").toString());
	ui.FabricColor->setColor(m_document->property("fabricColor").value<QColor>());
	ui.FlossScheme->addItems(SchemeManager::schemes());
	ui.FlossScheme->setCurrentItem(m_document->pattern()->palette().schemeName());
	ui.Instructions->setPlainText(m_document->property("instructions").toString());
	ui.ClothCountLink->setChecked(m_clothCountLink);
	on_ClothCountLink_clicked(m_clothCountLink);

	QMetaObject::connectSlotsByName(this);
	setMainWidget(widget);
}


FilePropertiesDlg::~FilePropertiesDlg()
{
}


int FilePropertiesDlg::documentWidth() const
{
	return m_width;
}


int FilePropertiesDlg::documentHeight() const
{
	return m_height;
}


Configuration::EnumDocument_UnitsFormat::type FilePropertiesDlg::unitsFormat() const
{
	return m_unitsFormat;
}


double FilePropertiesDlg::horizontalClothCount() const
{
	return m_horizontalClothCount;
}


bool FilePropertiesDlg::clothCountLink() const
{
	return ui.ClothCountLink->isChecked();
}


double FilePropertiesDlg::verticalClothCount() const
{
	return m_verticalClothCount;
}


Configuration::EnumEditor_ClothCountUnits::type FilePropertiesDlg::clothCountUnits() const
{
	return m_clothCountUnits;
}


QString FilePropertiesDlg::title() const
{
	return ui.PatternTitle->text();
}


QString FilePropertiesDlg::author() const
{
	return ui.PatternAuthor->text();
}


QString FilePropertiesDlg::copyright() const
{
	return ui.PatternCopyright->text();
}


QString FilePropertiesDlg::fabric() const
{
	return ui.PatternFabric->text();
}


QColor FilePropertiesDlg::fabricColor() const
{
	return ui.FabricColor->color();
}


QString FilePropertiesDlg::instructions() const
{
	return ui.Instructions->toPlainText();
}


QString FilePropertiesDlg::flossScheme() const
{
	return ui.FlossScheme->currentText();
}


void FilePropertiesDlg::slotButtonClicked(int button)
{
	if (button == KDialog::Ok)
	{
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
	m_width = std::max((int)d, m_minWidth);
}


void FilePropertiesDlg::on_PatternHeight_valueChanged(double d)
{
	if (m_unitsFormat != Configuration::EnumDocument_UnitsFormat::Stitches)
		d *= m_horizontalClothCount;
	m_height = std::max((int)d, m_minHeight);
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
	double scaledWidth = m_width/horizontalScale;
	double scaledHeight = m_height/verticalScale;

	ui.PatternWidth->setMinimum(scaledWidth);
	ui.PatternHeight->setMinimum(scaledHeight);
	ui.PatternWidth->setValue(scaledWidth);
	ui.PatternHeight->setValue(scaledHeight);
	ui.HorizontalClothCount->setValue(m_horizontalClothCount);
	ui.VerticalClothCount->setValue(m_verticalClothCount);

	QString suffix = ((m_clothCountUnits == Configuration::EnumEditor_ClothCountUnits::CM)?"/cm":"/in");
	ui.HorizontalClothCount->setSuffix(suffix);
	ui.VerticalClothCount->setSuffix(suffix);

	double step = ((m_unitsFormat == Configuration::EnumDocument_UnitsFormat::Stitches)?1:0.01);
	ui.PatternWidth->setSingleStep(step);
	ui.PatternHeight->setSingleStep(step);
}
