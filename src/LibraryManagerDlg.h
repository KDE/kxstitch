/*
 * Copyright (C) 2010-2015 by Stephen Allewell
 * steve.allewell@gmail.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#ifndef LibraryManagerDlg_H
#define LibraryManagerDlg_H


#include <QDialog>
#include <QMenu>
#include <QTreeWidgetItem>
#include <QWidget>

#include "ui_LibraryManager.h"


class LibraryListWidgetItem;
class LibraryTreeWidgetItem;
class QHideEvent;
class QShowEvent;


class LibraryManagerDlg : public QDialog
{
    Q_OBJECT

public:
    explicit LibraryManagerDlg(QWidget *parent);
    virtual ~LibraryManagerDlg();

    LibraryTreeWidgetItem *currentLibrary();

protected:
    virtual bool event(QEvent *) Q_DECL_OVERRIDE;
    virtual void hideEvent(QHideEvent *) Q_DECL_OVERRIDE;
    virtual void showEvent(QShowEvent *) Q_DECL_OVERRIDE;

public slots:
    void setCellSize(double, double);

private slots:
    void on_LibraryTree_customContextMenuRequested(const QPoint &);
    void on_LibraryIcons_customContextMenuRequested(const QPoint &);
    void on_LibraryTree_currentItemChanged(QTreeWidgetItem *, QTreeWidgetItem *);
    void on_IconSizeSlider_valueChanged(int);
    void on_DialogButtonBox_rejected();
    void on_DialogButtonBox_helpRequested();

    void newCategory();
    void addLibraryToExportList();
    void libraryProperties();
    void pasteFromClipboard();

    void patternProperties();
    void addPatternToExportList();
    void copyToClipboard();
    void deletePattern();

private:
    void refreshLibraries();
    void recurseLibraryDirectory(LibraryTreeWidgetItem *, const QString &);

    QMenu                   m_contextMenu;
    LibraryTreeWidgetItem   *m_contextTreeItem;
    LibraryListWidgetItem   *m_contextListItem;

    Ui::LibraryManager  ui;
};


#endif // LibraryManagerDlg_H
