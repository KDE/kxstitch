/********************************************************************************
 *	Copyright (C) 2010 by Stephen Allewell					*
 *	stephen@mirramar.adsl24.co.uk						*
 *										*
 *	This program is free software; you can redistribute it and/or modify	*
 *	it under the terms of the GNU General Public License as published by	*
 *	the Free Software Foundation; either version 2 of the License, or	*
 *	(at your option) any later version.					*
 ********************************************************************************/


#include <QBitmap>
#include <QPainter>
#include <QPen>

#include "TextToolDlg.h"


TextToolDlg::TextToolDlg(QWidget *parent)
	:	KDialog(parent)
{
	setCaption(i18n("File Properties"));
	setButtons(KDialog::Ok | KDialog::Cancel | KDialog::Help);
	QWidget *widget = new QWidget(this);
	ui.setupUi(widget);
	QMetaObject::connectSlotsByName(this);
	setMainWidget(widget);
}


TextToolDlg::~TextToolDlg()
{
}


void TextToolDlg::slotButtonClicked(int button)
{
	if (button == KDialog::Ok)
	{
		m_font = ui.TextToolFont->currentFont();
		m_text = ui.TextToolText->text();
		m_size = ui.TextToolSize->value();
		accept();
	}
	else
		KDialog::slotButtonClicked(button);

}


QByteArray TextToolDlg::pattern()
{
	QBitmap bitmap(1000, 100);
	QPainter painter;

	painter.begin(&bitmap);
	QFont font = painter.font();
	font.setFamily(m_font.family());
	font.setPixelSize(m_size);
	painter.setFont(font);
	painter.setPen(QPen(Qt::color0));
	QRect boundingRect = painter.boundingRect(0, 0, 1000, 100, Qt::AlignLeft | Qt::AlignTop, m_text);
	m_boundingWidth = boundingRect.width();
	m_boundingHeight = boundingRect.height();
	painter.end();

	bitmap = bitmap.copy(0, 0, m_boundingWidth, m_boundingHeight); // a resize
	painter.begin(&bitmap);
	painter.setFont(font);
	bitmap.fill(Qt::color0);
	painter.setPen(QPen(Qt::color1));
	painter.drawText(0, 0, m_boundingWidth, m_boundingHeight, Qt::AlignLeft | Qt::AlignTop, m_text);
	painter.end();

	QImage image;
	QByteArray a(m_boundingWidth*m_boundingHeight, '\0');
	image = bitmap.toImage();
	for (int y = 0 ; y < m_boundingHeight ; y++)
		for (int x = 0 ; x < m_boundingWidth ; x++)
			a[y*m_boundingWidth+x] = ((image.pixelIndex(x, y))?1:0);
	return a;
}


int TextToolDlg::boundingWidth() const
{
	return m_boundingWidth;
}


int TextToolDlg::boundingHeight() const
{
	return m_boundingHeight;
}


void TextToolDlg::on_TextToolFont_currentFontChanged(const QFont &font)
{
	QFont f = ui.TextToolText->font();
	f.setFamily(font.family());
	ui.TextToolText->setFont(f);
}


void TextToolDlg::on_TextToolSize_valueChanged(int s)
{
	QFont f = ui.TextToolText->font();
	f.setPointSize(s);
	ui.TextToolText->setFont(f);
}


void TextToolDlg::on_TextToolText_textChanged(const QString &s)
{
	enableButtonOk(!s.isEmpty());
}
