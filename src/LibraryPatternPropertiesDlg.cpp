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

#include <KConfigGroup>
#include <KHelpClient>
#include <KLocalizedString>
#include <KSharedConfig>


LibraryPatternPropertiesDlg::LibraryPatternPropertiesDlg(QWidget *parent, qint32 key, Qt::KeyboardModifiers modifiers, qint16 baseline, const QString &scheme, int width, int height, const QIcon &icon)
    :   QDialog(parent)
{
    setWindowTitle(i18n("Library Pattern Properties"));
    ui.setupUi(this);

    ui.FlossScheme->setText(scheme);
    ui.Width->setText(QString().setNum(width));
    ui.Height->setText(QString().setNum(height));
    ui.Baseline->setValue(baseline);
    ui.KeyCode->setKeyModifiers(key, modifiers);
    ui.Icon->setPixmap(icon.pixmap(ui.Icon->size()));
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


void LibraryPatternPropertiesDlg::hideEvent(QHideEvent *event)
{
    KConfigGroup(KSharedConfig::openConfig(), QStringLiteral("DialogSizes")).writeEntry(QStringLiteral("LibraryPatternPropertiesDlg"), size());

    QDialog::hideEvent(event);
}


void LibraryPatternPropertiesDlg::showEvent(QShowEvent *event)
{
    QDialog::showEvent(event);

    if (KConfigGroup(KSharedConfig::openConfig(), QStringLiteral("DialogSizes")).hasKey(QStringLiteral("LibraryPatternPropertiesDlg"))) {
        resize(KConfigGroup(KSharedConfig::openConfig(), QStringLiteral("DialogSizes")).readEntry(QStringLiteral("LibraryPatternPropertiesDlg"), QSize()));
    }
}


void LibraryPatternPropertiesDlg::on_DialogButtonBox_accepted()
{
    accept();
}


void LibraryPatternPropertiesDlg::on_DialogButtonBox_rejected()
{
    reject();
}


void LibraryPatternPropertiesDlg::on_DialogButtonBox_helpRequested()
{
    KHelpClient::invokeHelp("PatternLibraryDialog", "kxstitch");
}
