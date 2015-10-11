/*
 * Copyright (C) 2010-2015 by Stephen Allewell
 * steve.allewell@gmail.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#include "LibraryPatternPropertiesDlg.h"


LibraryPatternPropertiesDlg::LibraryPatternPropertiesDlg(QWidget *parent, qint32 key, Qt::KeyboardModifiers modifiers, qint16 baseline, const QString &scheme, int width, int height, const QIcon &icon)
    :   KDialog(parent)
{
    setCaption(i18n("Library Pattern Properties"));
    setButtons(KDialog::Ok | KDialog::Cancel | KDialog::Help);
    setHelp("PatternLibraryDialog");
    QWidget *widget = new QWidget(this);
    ui.setupUi(widget);
    QMetaObject::connectSlotsByName(this);
    ui.FlossScheme->setText(scheme);
    ui.Width->setText(QString().setNum(width));
    ui.Height->setText(QString().setNum(height));
    ui.Baseline->setValue(baseline);
    ui.KeyCode->setKeyModifiers(key, modifiers);
    ui.Icon->setPixmap(icon.pixmap(ui.Icon->size()));
    setMainWidget(widget);
}


LibraryPatternPropertiesDlg::~LibraryPatternPropertiesDlg()
{
}


qint32 LibraryPatternPropertiesDlg::key() const
{
    return ui.KeyCode->key();
}


Qt::KeyboardModifiers LibraryPatternPropertiesDlg::modifiers() const
{
    return ui.KeyCode->modifiers();
}


qint16 LibraryPatternPropertiesDlg::baseline() const
{
    return ui.Baseline->value();
}
