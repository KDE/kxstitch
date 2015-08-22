/*
 * Copyright (C) 20102-2014 by Stephen Allewell
 * steve.allewell@gmail.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#ifndef FilePropertiesDlg_H
#define FilePropertiesDlg_H


#include <QDialog>

#include "configuration.h"
#include "ui_FileProperties.h"


class Document;
class QWidget;


class FilePropertiesDlg : public QDialog
{
    Q_OBJECT

public:
    FilePropertiesDlg(QWidget *, Document *);
    ~FilePropertiesDlg();

    int documentWidth() const;
    int documentHeight() const;
    Configuration::EnumDocument_UnitsFormat::type unitsFormat() const;
    double horizontalClothCount() const;
    bool clothCountLink() const;
    double verticalClothCount() const;
    Configuration::EnumEditor_ClothCountUnits::type clothCountUnits() const;
    QString title() const;
    QString author() const;
    QString copyright() const;
    QString fabric() const;
    QColor fabricColor() const;
    QString instructions() const;
    QString flossScheme() const;

private slots:
    void on_UnitsFormat_activated(int);
    void on_PatternWidth_valueChanged(double);
    void on_PatternHeight_valueChanged(double);
    void on_HorizontalClothCount_valueChanged(double);
    void on_VerticalClothCount_valueChanged(double);
    void on_ClothCountLink_clicked(bool);
    void on_DialogButtonBox_accepted();
    void on_DialogButtonBox_rejected();
    void on_DialogButtonBox_helpRequested();

private:
    void updatePatternSizes();

    Ui::FileProperties  ui;

    Document    *m_document;

    int m_width;
    int m_height;
    int m_minWidth;
    int m_minHeight;
    double  m_horizontalClothCount;
    double  m_verticalClothCount;
    bool    m_clothCountLink;

    Configuration::EnumDocument_UnitsFormat::type   m_unitsFormat;
    Configuration::EnumEditor_ClothCountUnits::type m_clothCountUnits;
};


#endif // FilePropertiesDlg_H
