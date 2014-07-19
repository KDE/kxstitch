/*
 * Copyright (C) 2012-2014 by Stephen Allewell
 * steve.allewell@gmail.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#ifndef ConfigurationDialogs_H
#define ConfigurationDialogs_H


#include "ui_EditorConfigPage.h"
#include "ui_PatternConfigPage.h"
#include "ui_PaletteConfigPage.h"
#include "ui_ImportConfigPage.h"
#include "ui_LibraryConfigPage.h"
#include "ui_PrinterConfigPage.h"


class EditorConfigPage : public QWidget, public Ui::EditorConfigPage
{
    Q_OBJECT

public:
    explicit EditorConfigPage(QWidget *parent = 0, const char *name = 0);
};


class PatternConfigPage : public QWidget, public Ui::PatternConfigPage
{
    Q_OBJECT

public:
    explicit PatternConfigPage(QWidget *parent = 0, const char *name = 0);

protected slots:
    void on_kcfg_Editor_ClothCountUnits_activated(int);
    void on_kcfg_Document_UnitsFormat_activated(int);
    void on_kcfg_Editor_ClothCountLink_toggled(bool);
    void on_kcfg_Editor_HorizontalClothCount_valueChanged(double);

private:
    void setPrecision();

    int m_currentDocumentUnitsIndex;
    int m_currentClothCountUnitsIndex;
};


class PaletteConfigPage : public QWidget, public Ui::PaletteConfigPage
{
    Q_OBJECT

public:
    explicit PaletteConfigPage(QWidget *parent = 0, const char *name = 0);

public slots:
    void defaultClicked();
};


class ImportConfigPage : public QWidget, public Ui::ImportConfigPage
{
    Q_OBJECT

public:
    explicit ImportConfigPage(QWidget *parent = 0, const char *name = 0);
};


class LibraryConfigPage : public QWidget, public Ui::LibraryConfigPage
{
    Q_OBJECT

public:
    explicit LibraryConfigPage(QWidget *parent = 0, const char *name = 0);
};


class PrinterConfigPage : public QWidget, public Ui::PrinterConfigPage
{
    Q_OBJECT

public:
    explicit PrinterConfigPage(QWidget *parent = 0, const char *name = 0);
};


#endif // ConfigurationDialogs_H
