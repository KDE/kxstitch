/*
 * Copyright (C) 2010-2014 by Stephen Allewell
 * steve.allewell@gmail.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * Some of the concepts and code behind the rich text editor have been
 * copied / modified from the TextEdit example program from here:
 * http://qt-project.org/doc/qt-4.8/demos-textedit.html
 */


#include "TextElementDlg.h"

#include <QColorDialog>

#include <KHelpClient>
#include <KLocalizedString>

#include "Element.h"


TextElementDlg::TextElementDlg(QWidget *parent, TextElement *textElement)
    :   QDialog(parent),
        m_textElement(textElement)
{
    setWindowTitle(i18n("Text Element Properties"));

    QWidget *widget = new QWidget(this);
    ui.setupUi(widget);
    QMetaObject::connectSlotsByName(this);

    ui.BoldButton->setIcon(QIcon::fromTheme("format-text-bold"));
    ui.UnderlineButton->setIcon(QIcon::fromTheme("format-text-underline"));
    ui.ItalicButton->setIcon(QIcon::fromTheme("format-text-italic"));
    ui.AlignLeft->setIcon(QIcon::fromTheme("format-justify-left"));
    ui.AlignHCenter->setIcon(QIcon::fromTheme("format-justify-center"));
    ui.AlignRight->setIcon(QIcon::fromTheme("format-justify-right"));
    ui.AlignJustify->setIcon(QIcon::fromTheme("format-justify-fill"));

    QButtonGroup *group = new QButtonGroup(this);
    group->addButton(ui.AlignLeft);
    group->addButton(ui.AlignHCenter);
    group->addButton(ui.AlignRight);
    group->addButton(ui.AlignJustify);
    group->setExclusive(true);
    connect(group, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(textAlign(QAbstractButton*)));
    alignmentChanged(m_textElement->m_alignment);

    ui.MarginLeft->setValue(m_textElement->m_margins.left());
    ui.MarginTop->setValue(m_textElement->m_margins.top());
    ui.MarginRight->setValue(m_textElement->m_margins.right());
    ui.MarginBottom->setValue(m_textElement->m_margins.bottom());

    ui.ShowBorder->setChecked(m_textElement->m_showBorder);
    ui.BorderColor->setColor(m_textElement->m_borderColor);
    ui.BorderThickness->setValue(double(m_textElement->m_borderThickness) / 10);

    ui.BackgroundColor->setColor(m_textElement->m_backgroundColor);
    ui.BackgroundTransparency->setValue(m_textElement->m_backgroundTransparency);
    ui.FillBackground->setChecked(m_textElement->m_fillBackground);

    fontChanged(m_textElement->m_textFont);
    colorChanged(m_textElement->m_textColor);

    if (!m_textElement->m_text.isEmpty()) {
        ui.Text->setHtml(m_textElement->m_text);
    }
}


TextElementDlg::~TextElementDlg()
{
}


void TextElementDlg::on_FillBackground_toggled(bool checked)
{
    ui.Text->setAutoFillBackground(checked);
    on_BackgroundColor_activated((checked) ? ui.BackgroundColor->color() : QWidget().palette().color(QPalette::Window));
}


void TextElementDlg::on_BackgroundColor_activated(const QColor &color)
{
    QPalette pal = ui.Text->palette();
    pal.setColor(QPalette::Base, color);
    ui.Text->setPalette(pal);
}


void TextElementDlg::on_BackgroundTransparency_valueChanged(int value)
{
    ui.Text->setWindowOpacity(value);
}


void TextElementDlg::on_BoldButton_clicked()
{
    QTextCharFormat fmt;
    fmt.setFontWeight(ui.BoldButton->isChecked() ? QFont::Bold : QFont::Normal);
    mergeFormatOnWordOrSelection(fmt);
}


void TextElementDlg::on_UnderlineButton_clicked()
{
    QTextCharFormat fmt;
    fmt.setFontUnderline(ui.UnderlineButton->isChecked());
    mergeFormatOnWordOrSelection(fmt);
}


void TextElementDlg::on_ItalicButton_clicked()
{
    QTextCharFormat fmt;
    fmt.setFontItalic(ui.ItalicButton->isChecked());
    mergeFormatOnWordOrSelection(fmt);
}


void TextElementDlg::on_FontFamily_currentFontChanged(const QFont &font)
{
    QTextCharFormat fmt;
    fmt.setFontFamily(font.family());
    mergeFormatOnWordOrSelection(fmt);
}


void TextElementDlg::on_PointSize_valueChanged(int size)
{
    QTextCharFormat fmt;
    fmt.setFontPointSize(double(size));
    mergeFormatOnWordOrSelection(fmt);
}


void TextElementDlg::on_TextColor_clicked()
{
    QColor color = QColorDialog::getColor(ui.Text->textColor(), this);

    if (color.isValid()) {
        QTextCharFormat fmt;
        fmt.setForeground(color);
        mergeFormatOnWordOrSelection(fmt);
        colorChanged(color);
    }
}


void TextElementDlg::textAlign(QAbstractButton *button)
{
    if (button == ui.AlignLeft) {
        ui.Text->setAlignment(Qt::AlignLeft | Qt::AlignAbsolute);
    } else if (button == ui.AlignHCenter) {
        ui.Text->setAlignment(Qt::AlignHCenter);
    } else if (button == ui.AlignRight) {
        ui.Text->setAlignment(Qt::AlignRight | Qt::AlignAbsolute);
    } else if (button == ui.AlignJustify) {
        ui.Text->setAlignment(Qt::AlignJustify);
    }
}


void TextElementDlg::on_Text_currentCharFormatChanged(const QTextCharFormat &format)
{
    fontChanged(format.font());
    colorChanged(format.foreground().color());
}


void TextElementDlg::on_Text_cursorPositionChanged()
{
    alignmentChanged(ui.Text->alignment());
}


void TextElementDlg::on_DialogButtonBox_accepted()
{
    m_textElement->m_margins = QMargins(ui.MarginLeft->value(), ui.MarginTop->value(), ui.MarginRight->value(), ui.MarginBottom->value());
    m_textElement->m_showBorder = ui.ShowBorder->isChecked();
    m_textElement->m_borderColor = ui.BorderColor->color();
    m_textElement->m_borderThickness = int(ui.BorderThickness->value() * 10);
    m_textElement->m_fillBackground = ui.FillBackground->isChecked();
    m_textElement->m_backgroundColor = ui.BackgroundColor->color();
    m_textElement->m_backgroundTransparency = ui.BackgroundTransparency->value();
    m_textElement->m_text = ui.Text->toHtml();

    if (ui.AlignLeft->isChecked()) {
        m_textElement->m_alignment = Qt::AlignLeft;
    } else if (ui.AlignHCenter->isChecked()) {
        m_textElement->m_alignment = Qt::AlignHCenter;
    } else if (ui.AlignRight->isChecked()) {
        m_textElement->m_alignment = Qt::AlignRight;
    } else if (ui.AlignJustify->isChecked()) {
        m_textElement->m_alignment = Qt::AlignJustify;
    }

    m_textElement->m_textFont = ui.Text->font();

    accept();
}


void TextElementDlg::on_DialogButtonBox_rejected()
{
    reject();
}


void TextElementDlg::on_DialogButtonBox_helpRequested()
{
    KHelpClient::invokeHelp("TextElement", "kxstitch");
}


void TextElementDlg::mergeFormatOnWordOrSelection(const QTextCharFormat &format)
{
    QTextCursor cursor = ui.Text->textCursor();

    if (!cursor.hasSelection()) {
        cursor.select(QTextCursor::WordUnderCursor);
    }

    cursor.mergeCharFormat(format);
    ui.Text->mergeCurrentCharFormat(format);
}


void TextElementDlg::fontChanged(const QFont &f)
{
    ui.FontFamily->setCurrentFont(QFontInfo(f).family());
    ui.PointSize->setValue(f.pointSize());
    ui.BoldButton->setChecked(f.bold());
    ui.ItalicButton->setChecked(f.italic());
    ui.UnderlineButton->setChecked(f.underline());
}


void TextElementDlg::colorChanged(const QColor &color)
{
    QPixmap pix(22, 22);
    pix.fill(color);
    ui.TextColor->setIcon(pix);
}


void TextElementDlg::alignmentChanged(Qt::Alignment align)
{
    if (align & Qt::AlignLeft) {
        ui.AlignLeft->setChecked(true);
    } else if (align & Qt::AlignHCenter) {
        ui.AlignHCenter->setChecked(true);
    } else if (align & Qt::AlignRight) {
        ui.AlignRight->setChecked(true);
    } else if (align & Qt::AlignJustify) {
        ui.AlignJustify->setChecked(true);
    }
}
