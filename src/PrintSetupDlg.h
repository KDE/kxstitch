/*
 * Copyright (C) 2010-2015 by Stephen Allewell
 * steve.allewell@gmail.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#ifndef PrintSetupDlg_H
#define PrintSetupDlg_H


#include <QButtonGroup>
#include <QDialog>
#include <QListWidgetItem>

#include "PagePreviewListWidgetItem.h"
#include "PrinterConfiguration.h"

#include "ui_PrintSetup.h"


class QHideEvent;
class QResizeEvent;
class QShowEvent;
class QString;

class Document;
class Element;
class PageLayoutEditor;


class PrintSetupDlg : public QDialog
{
    Q_OBJECT

public:
    PrintSetupDlg(QWidget *, Document *, QPrinter *);
    virtual ~PrintSetupDlg();

    const PrinterConfiguration &printerConfiguration() const;

protected:
    virtual void hideEvent(QHideEvent *) Q_DECL_OVERRIDE;
    virtual void showEvent(QShowEvent *) Q_DECL_OVERRIDE;

private slots:
    void on_PageSize_currentIndexChanged(const QString &);
    void on_Orientation_currentIndexChanged(int);
    void on_Zoom_currentIndexChanged(int);
    void on_Pages_currentItemChanged(QListWidgetItem *, QListWidgetItem *);
    void on_Templates_clicked();
    void on_AddPage_clicked();
    void on_InsertPage_clicked();
    void on_DeletePage_clicked();
    void on_SelectElement_clicked();
    void on_TextElement_clicked();
    void on_PatternElement_clicked();
    void on_ImageElement_clicked();
    void on_KeyElement_clicked();
    void on_DialogButtonBox_accepted();
    void on_DialogButtonBox_rejected();
    void on_DialogButtonBox_helpRequested();
    void selectionMade(const QRect &);
    void elementGeometryChanged();
    void previewContextMenuRequested(const QPoint &);

    void properties();
    void deleteElement();

private:
    enum ElementMode {Select, Text, Pattern, Key, Image};

    void initialiseFromConfig();
    void addPage(int, Page *);
    void updatePageNumbers();
//    void selectTemplate();

    QPageSize selectedPageSize();
    QPageLayout::Orientation selectedOrientation();
    double selectedZoom();

    Ui::PrintSetup  ui;

    PrinterConfiguration    m_printerConfiguration;

    double          m_scale;
    QButtonGroup    m_buttonGroup;
    ElementMode     m_elementMode;
    Element         *m_elementUnderCursor;
    Document        *m_document;
    QPrinter        *m_printer;

    PageLayoutEditor    *m_pageLayoutEditor;
};


#endif // PrintSetupDlg_H
