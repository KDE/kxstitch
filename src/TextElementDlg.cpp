/********************************************************************************
 *	Copyright (C) 2010 by Stephen Allewell					*
 *	stephen@mirramar.adsl24.co.uk						*
 *										*
 *	This program is free software; you can redistribute it and/or modify	*
 *	it under the terms of the GNU General Public License as published by	*
 *	the Free Software Foundation; either version 2 of the License, or	*
 *	(at your option) any later version.					*
 ********************************************************************************/


#include "TextElementDlg.h"
#include "Element.h"


TextElementDlg::TextElementDlg(QWidget *parent, TextElement *textElement)
	:	KDialog(parent),
		m_textElement(textElement)
{
	setCaption(i18n("Text Element Properties"));
	setButtons(KDialog::Ok | KDialog::Cancel | KDialog::Help);
	setHelp("TextElement");
	QWidget *widget = new QWidget(this);
	ui.setupUi(widget);
	QMetaObject::connectSlotsByName(this);

	ui.AlignLeft->setIcon(KIcon("format-justify-left"));
	ui.AlignHCenter->setIcon(KIcon("format-justify-center"));
	ui.AlignJustify->setIcon(KIcon("format-justify-fill"));
	ui.AlignRight->setIcon(KIcon("format-justify-right"));

	ui.MarginLeft->setValue(m_textElement->m_margins.left());
	ui.MarginTop->setValue(m_textElement->m_margins.top());
	ui.MarginRight->setValue(m_textElement->m_margins.right());
	ui.MarginBottom->setValue(m_textElement->m_margins.bottom());

	ui.ShowBorder->setChecked(m_textElement->m_showBorder);
	ui.BorderColor->setColor(m_textElement->m_borderColor);
	ui.BorderThickness->setValue(double(m_textElement->m_borderThickness)/10);
	ui.FillBackground->setChecked(m_textElement->m_fillBackground);
	ui.BackgroundColor->setColor(m_textElement->m_backgroundColor);
	ui.BackgroundTransparency->setValue(m_textElement->m_backgroundTransparency);
	ui.Text->setText(m_textElement->m_text);
	switch (m_textElement->m_alignment)
	{
		case Qt::AlignLeft:
			ui.AlignLeft->setChecked(true);
			break;

		case Qt::AlignHCenter:
			ui.AlignHCenter->setChecked(true);
			break;

		case Qt::AlignJustify:
			ui.AlignJustify->setChecked(true);
			break;

		case Qt::AlignRight:
			ui.AlignRight->setChecked(true);
			break;
	}
	ui.TextColor->setColor(m_textElement->m_textColor);
	ui.TextFont->setFont(m_textElement->m_textFont);

	setMainWidget(widget);
}


TextElementDlg::~TextElementDlg()
{
}


void TextElementDlg::slotButtonClicked(int button)
{
	if (button == KDialog::Ok)
	{
		m_textElement->m_margins = QMargins(ui.MarginLeft->value(), ui.MarginTop->value(), ui.MarginRight->value(), ui.MarginBottom->value());

		m_textElement->m_showBorder = ui.ShowBorder->isChecked();
		m_textElement->m_borderColor = ui.BorderColor->color();
		m_textElement->m_borderThickness = int(ui.BorderThickness->value()*10);
		m_textElement->m_fillBackground = ui.FillBackground->isChecked();
		m_textElement->m_backgroundColor = ui.BackgroundColor->color();
		m_textElement->m_backgroundTransparency = ui.BackgroundTransparency->value();
		m_textElement->m_text = ui.Text->toPlainText();
		if (ui.AlignLeft->isChecked()) m_textElement->m_alignment = Qt::AlignLeft;
		if (ui.AlignHCenter->isChecked()) m_textElement->m_alignment = Qt::AlignHCenter;
		if (ui.AlignJustify->isChecked()) m_textElement->m_alignment = Qt::AlignJustify;
		if (ui.AlignRight->isChecked()) m_textElement->m_alignment = Qt::AlignRight;
		m_textElement->m_textColor = ui.TextColor->color();
		m_textElement->m_textFont = ui.TextFont->font();
		accept();
	}
	else
		KDialog::slotButtonClicked(button);
}


void TextElementDlg::on_BackgroundTransparency_valueChanged(int value)
{
	ui.Text->setWindowOpacity(value);
}
