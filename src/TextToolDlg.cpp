/*
 * Copyright (C) 2010-2015 by Stephen Allewell
 * steve.allewell@gmail.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#include "TextToolDlg.h"

#include <QBitmap>
#include <QPainter>
#include <QPen>

#include "configuration.h"


TextToolDlg::TextToolDlg(QWidget *parent)
    :   KDialog(parent)
{
    setCaption(i18n("Text Tool"));
    setButtons(KDialog::Ok | KDialog::Cancel | KDialog::Help);
    setHelp("TextDialog");
    QWidget *widget = new QWidget(this);
    ui.setupUi(widget);

    ui.TextToolFont->setCurrentFont(QFont(Configuration::textTool_FontFamily()));
    ui.TextToolSize->setValue(Configuration::textTool_FontSize());

    QMetaObject::connectSlotsByName(this);
    setMainWidget(widget);
}


TextToolDlg::~TextToolDlg()
{
}


void TextToolDlg::slotButtonClicked(int button)
{
    if (button == KDialog::Ok) {
        m_font = ui.TextToolFont->currentFont();
        m_text = ui.TextToolText->text();
        m_size = ui.TextToolSize->value();

        Configuration::setTextTool_FontFamily(ui.TextToolFont->currentFont().family());
        Configuration::setTextTool_FontSize(ui.TextToolSize->value());
        Configuration::self()->writeConfig();

        accept();
    } else {
        KDialog::slotButtonClicked(button);
    }

}


QImage TextToolDlg::image()
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
    painter.end();

    int boundingWidth = boundingRect.width();
    int boundingHeight = boundingRect.height();
    bitmap = bitmap.copy(0, 0, boundingWidth, boundingHeight); // a resize
    painter.begin(&bitmap);
    painter.setFont(font);
    bitmap.fill(Qt::color0);
    painter.setPen(QPen(Qt::color1));
    painter.drawText(0, 0, boundingWidth, boundingHeight, Qt::AlignLeft | Qt::AlignTop, m_text);
    painter.end();

    QImage image = bitmap.toImage();

    return image;
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
