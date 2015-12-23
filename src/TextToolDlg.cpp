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
#include <QPushButton>

#include <KHelpClient>
#include <KLocalizedString>

#include "configuration.h"


TextToolDlg::TextToolDlg(QWidget *parent)
    :   QDialog(parent)
{
    setWindowTitle(i18n("Text Tool"));
    ui.setupUi(this);

    ui.TextToolFont->setCurrentFont(QFont(Configuration::textTool_FontFamily()));
    ui.TextToolSize->setValue(Configuration::textTool_FontSize());
}


TextToolDlg::~TextToolDlg()
{
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


void TextToolDlg::hideEvent(QHideEvent *event)
{
    KConfigGroup(KSharedConfig::openConfig(), QStringLiteral("DialogSizes")).writeEntry(QStringLiteral("TextToolDlg"), size());

    QDialog::hideEvent(event);
}


void TextToolDlg::showEvent(QShowEvent *event)
{
    QDialog::showEvent(event);

    if (KConfigGroup(KSharedConfig::openConfig(), QStringLiteral("DialogSizes")).hasKey(QStringLiteral("PrintSetupDlg"))) {
        resize(KConfigGroup(KSharedConfig::openConfig(), QStringLiteral("DialogSizes")).readEntry(QStringLiteral("TextToolDlg"), QSize()));
    }
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
    ui.DialogButtonBox->button(QDialogButtonBox::Ok)->setEnabled(!s.isEmpty());
}


void TextToolDlg::on_DialogButtonBox_accepted()
{
    m_font = ui.TextToolFont->currentFont();
    m_text = ui.TextToolText->text();
    m_size = ui.TextToolSize->value();

    Configuration::setTextTool_FontFamily(ui.TextToolFont->currentFont().family());
    Configuration::setTextTool_FontSize(ui.TextToolSize->value());

    accept();
}


void TextToolDlg::on_DialogButtonBox_rejected()
{
    reject();
}


void TextToolDlg::on_DialogButtonBox_helpRequested()
{
    KHelpClient::invokeHelp(QStringLiteral("TextDialog"), QStringLiteral("kxstitch"));
}

