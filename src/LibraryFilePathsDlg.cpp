/*
 * Copyright (C) 2010-2015 by Stephen Allewell
 * steve.allewell@gmail.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#include "LibraryFilePathsDlg.h"

#include <KConfigGroup>
#include <KHelpClient>
#include <KLocalizedString>
#include <KSharedConfig>


LibraryFilePathsDlg::LibraryFilePathsDlg(QWidget *parent, const QString&, QStringList paths)
    :   QDialog(parent)
{
    setWindowTitle(i18n("Library File Paths"));
    ui.setupUi(this);

    QStringListIterator pathIterator(paths);

    while (pathIterator.hasNext()) {
        ui.PathList->addItem(pathIterator.next());
    }
}


LibraryFilePathsDlg::~LibraryFilePathsDlg()
{
}


void LibraryFilePathsDlg::hideEvent(QHideEvent *event)
{
    KConfigGroup(KSharedConfig::openConfig(), QStringLiteral("DialogSizes")).writeEntry(QStringLiteral("LibraryFilePathsDlg"), size());

    QDialog::hideEvent(event);
}


void LibraryFilePathsDlg::showEvent(QShowEvent *event)
{
    QDialog::showEvent(event);

    if (KConfigGroup(KSharedConfig::openConfig(), QStringLiteral("DialogSizes")).hasKey(QStringLiteral("LibraryFilePathsDlg"))) {
        resize(KConfigGroup(KSharedConfig::openConfig(), QStringLiteral("DialogSizes")).readEntry(QStringLiteral("LibraryFilePathsDlg"), QSize()));
    }
}


void LibraryFilePathsDlg::on_DialogButtonBox_accepted()
{
    accept();
}


void LibraryFilePathsDlg::on_DialogButtonBox_helpRequested()
{
    KHelpClient::invokeHelp(QStringLiteral("LibraryPaths"), QStringLiteral("kxstitch"));
}
