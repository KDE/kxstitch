/*
 * Copyright (C) 2010 by Stephen Allewell
 * sallewell@users.sourceforge.net
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#ifndef PaletteManagerDlg_H
#define PaletteManagerDlg_H


#include <KDialog>

#include "Document.h"
#include "FlossScheme.h"

#include "ui_PaletteManager.h"


class Document;
class DocumentFloss;
class SchemeManager;
class SymbolSelectorDlg;


class PaletteManagerDlg : public KDialog
{
    Q_OBJECT

public:
    PaletteManagerDlg(QWidget *, Document *);
    ~PaletteManagerDlg();

    const DocumentPalette &palette() const;

protected slots:
    virtual void slotButtonClicked(int button);

private slots:
    void on_ColorList_currentRowChanged(int);
    void on_CurrentList_currentRowChanged(int);
    void on_AddFloss_clicked(bool);
    void on_RemoveFloss_clicked(bool);
    void on_StitchStrands_activated(int);
    void on_BackstitchStrands_activated(int);
    void on_StitchSymbol_clicked(bool);
    void on_BackstitchSymbol_activated(int);
    void on_NewFloss_clicked(bool);
    void on_ClearUnused_clicked(bool);
    void on_Calibrate_clicked(bool);

private:
    void fillLists();
    void insertListWidgetItem(QListWidget *, QListWidgetItem *);
    bool contains(const QString &) const;
    int paletteIndex(const QString &) const;
    bool symbolsAvailable() const;

    Ui::PaletteManager  ui;

    Document                *m_document;
    DocumentPalette         m_dialogPalette;
    QMap<int, FlossUsage>   m_flossUsage;
    FlossScheme             *m_scheme;
    SymbolSelectorDlg       *m_symbolSelectorDlg;
};


#endif // PaletteManagerDlg_H
