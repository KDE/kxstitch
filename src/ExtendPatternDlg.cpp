/*
 * Copyright (C) 2010-2015 by Stephen Allewell
 * steve.allewell@gmail.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#include "ExtendPatternDlg.h"


ExtendPatternDlg::ExtendPatternDlg(QWidget *parent)
    :   KDialog(parent)
{
    setCaption(i18n("Extend Pattern"));
    setButtons(KDialog::Ok | KDialog::Cancel | KDialog::Help);
    setHelp("ExtendDialog");
    QWidget *widget = new QWidget(this);
    ui.setupUi(widget);
    setMainWidget(widget);
}


ExtendPatternDlg::~ExtendPatternDlg()
{
}


int ExtendPatternDlg::top() const
{
    return ui.TopMargin->value();
}


int ExtendPatternDlg::left() const
{
    return ui.LeftMargin->value();
}


int ExtendPatternDlg::right() const
{
    return ui.RightMargin->value();
}


int ExtendPatternDlg::bottom() const
{
    return ui.BottomMargin->value();
}
