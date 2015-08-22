/*
 * Copyright (C) 2010-2014 by Stephen Allewell
 * steve.allewell@gmail.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#include "LibraryFilePathsDlg.h"

#include <KHelpClient>
#include <KLocalizedString>


LibraryFilePathsDlg::LibraryFilePathsDlg(QWidget *parent, const QString&, QStringList paths)
    :   QDialog(parent)
{
    setWindowTitle(i18n("Library File Paths"));
    QWidget *widget = new QWidget(this);
    ui.setupUi(widget);
    QMetaObject::connectSlotsByName(this);

    QStringListIterator pathIterator(paths);

    while (pathIterator.hasNext()) {
        ui.PathList->addItem(pathIterator.next());
    }
}


LibraryFilePathsDlg::~LibraryFilePathsDlg()
{
}


void LibraryFilePathsDlg::on_DialogButtonBox_accepted()
{
    accept();
}


void LibraryFilePathsDlg::on_DialogButtonBox_helpRequested()
{
    KHelpClient::invokeHelp("LibraryPaths", "kxstitch");
}
