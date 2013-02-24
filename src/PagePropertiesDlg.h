/*
 * Copyright (C) 2010 by Stephen Allewell
 * stephen@mirramar.adsl24.co.uk
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#ifndef PagePropertiesDlg_H
#define PagePropertiesDlg_H


#include "ui_PageProperties.h"


class PagePreviewListWidgetItem;


class PagePropertiesDlg : public KDialog
{
    Q_OBJECT

public:
    PagePropertiesDlg(QWidget *, PagePreviewListWidgetItem *);
    ~PagePropertiesDlg();

protected slots:
    void slotButtonClicked(int);

private:
    PagePreviewListWidgetItem   *m_pagePreview;

    Ui::PageProperties  ui;
};


#endif // PagePropertiesDlg_H
