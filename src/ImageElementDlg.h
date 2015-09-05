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


#include <QRect>

#include <KDialog>

#include "ui_ImageElement.h"


class Document;
class ImageElement;
class SelectArea;


class ImageElementDlg : public KDialog
{
    Q_OBJECT

public:
    ImageElementDlg(QWidget *, ImageElement *, Document *);
    ~ImageElementDlg();

protected slots:
    void slotButtonClicked(int);

private slots:

private:
    SelectArea      *m_selectArea;

    ImageElement    *m_imageElement;
    Document        *m_document;

    QRect           m_patternRect;

    Ui::ImageElement  ui;
};


#endif // ImageElementDlg_H
