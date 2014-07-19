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


LibraryFilePathsDlg::LibraryFilePathsDlg(QWidget *parent, const QString&, QStringList paths)
    :   KDialog(parent)
{
    setCaption(i18n("Library File Paths"));
    setButtons(KDialog::Ok | KDialog::Help);
    QWidget *widget = new QWidget(this);
    ui.setupUi(widget);
    QMetaObject::connectSlotsByName(this);

    QStringListIterator pathIterator(paths);

    while (pathIterator.hasNext()) {
        ui.PathList->addItem(pathIterator.next());
    }

    setMainWidget(widget);
}


LibraryFilePathsDlg::~LibraryFilePathsDlg()
{
}


void LibraryFilePathsDlg::slotButtonClicked(int button)
{
    KDialog::slotButtonClicked(button);
}
