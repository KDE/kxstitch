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

#include <KConfigGroup>
#include <KHelpClient>
#include <KLocalizedString>
#include <KSharedConfig>


ExtendPatternDlg::ExtendPatternDlg(QWidget *parent)
    :   QDialog(parent)
{
    setWindowTitle(i18n("Extend Pattern"));
    ui.setupUi(this);
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


void ExtendPatternDlg::hideEvent(QHideEvent *event)
{
    KConfigGroup(KSharedConfig::openConfig(), QStringLiteral("DialogSizes")).writeEntry(QStringLiteral("ExtendPatternDlg"), size());

    QDialog::hideEvent(event);
}


void ExtendPatternDlg::showEvent(QShowEvent *event)
{
    QDialog::showEvent(event);

    if (KConfigGroup(KSharedConfig::openConfig(), QStringLiteral("DialogSizes")).hasKey(QStringLiteral("ExtendPatternDlg"))) {
        resize(KConfigGroup(KSharedConfig::openConfig(), QStringLiteral("DialogSizes")).readEntry(QStringLiteral("ExtendPatternDlg"), QSize()));
    }
}


void ExtendPatternDlg::on_DialogButtonBox_accepted()
{
    accept();
}


void ExtendPatternDlg::on_DialogButtonBox_rejected()
{
    reject();
}


void ExtendPatternDlg::on_DialogButtonBox_helpRequested()
{
    KHelpClient::invokeHelp(QStringLiteral("ExtendPatternDialog"), QStringLiteral("kxstitch"));
}
