/*
 * Copyright (C) 2013-2015 by Stephen Allewell
 * steve.allewell@gmail.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#ifndef ImageElementDlg_H
#define ImageElementDlg_H


#include <QDialog>
#include <QRect>

#include "ui_ImageElement.h"


class Document;
class ImageElement;
class SelectArea;
class QHideEvent;
class QShowEvent;


class ImageElementDlg : public QDialog
{
    Q_OBJECT

public:
    ImageElementDlg(QWidget *, ImageElement *, Document *);
    ~ImageElementDlg();

protected:
    virtual void hideEvent(QHideEvent *) Q_DECL_OVERRIDE;
    virtual void showEvent(QShowEvent *) Q_DECL_OVERRIDE;

private slots:
    void on_DialogButtonBox_accepted();
    void on_DialogButtonBox_rejected();
    void on_DialogButtonBox_helpRequested();

private:
    SelectArea      *m_selectArea;

    ImageElement    *m_imageElement;
    Document        *m_document;

    QRect           m_patternRect;

    Ui::ImageElement  ui;
};


#endif // ImageElementDlg_H
