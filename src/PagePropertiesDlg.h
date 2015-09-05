/*
 * Copyright (C) 2010-2015 by Stephen Allewell
 * steve.allewell@gmail.com
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
    PagePropertiesDlg(QWidget *, const QMargins &, bool, int);
    ~PagePropertiesDlg();

    QMargins margins() const;
    bool showGrid() const;
    int gridSize() const;

protected slots:
    void slotButtonClicked(int);

private:
    Ui::PageProperties  ui;
};


#endif // PagePropertiesDlg_H
