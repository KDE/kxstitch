/*
 * Copyright (C) 2010-2014 by Stephen Allewell
 * steve.allewell@gmail.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#ifndef TextToolDlg_H
#define TextToolDlg_H


#include <QDialog>
#include <QFont>
#include <QString>

#include "ui_TextTool.h"


class TextToolDlg : public QDialog
{
    Q_OBJECT

public:
    explicit TextToolDlg(QWidget *);
    ~TextToolDlg();

    QImage image();

private slots:
    void on_TextToolFont_currentFontChanged(const QFont &);
    void on_TextToolSize_valueChanged(int);
    void on_TextToolText_textChanged(const QString &);
    void on_DialogButtonBox_accepted();
    void on_DialogButtonBox_rejected();
    void on_DialogButtonBox_helpRequested();

private:
    Ui::TextTool    ui;

    QFont   m_font;
    int     m_size;
    QString m_text;
};


#endif // TextToolDlg_H

