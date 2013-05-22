/*
 * Copyright (C) 2010 by Stephen Allewell
 * sallewell@users.sourceforge.net
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#ifndef PrintSetupDlg_H
#define PrintSetupDlg_H


#include <QButtonGroup>
#include <QListWidgetItem>

#include "PagePreviewListWidgetItem.h"
#include "PrinterConfiguration.h"

#include "ui_PrintSetup.h"


class QResizeEvent;
class QShowEvent;
class QString;

class Document;
class Element;
class PageLayoutEditor;


class PrintSetupDlg : public KDialog
{
    Q_OBJECT

public:
    PrintSetupDlg(QWidget *, Document *, QPrinter *);
    ~PrintSetupDlg();

    const PrinterConfiguration &printerConfiguration() const;

protected:
    void showEvent(QShowEvent *);

private slots:
    void on_PaperSize_currentIndexChanged(const QString &);
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

    QPrinter::PaperSize selectedPaperSize();
    QPrinter::Orientation selectedOrientation();
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
