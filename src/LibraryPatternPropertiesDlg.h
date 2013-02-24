/*
 * Copyright (C) 2010 by Stephen Allewell
 * stephen@mirramar.adsl24.co.uk
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#ifndef LibraryPatternPropertiesDlg_H
#define LibraryPatternPropertiesDlg_H


#include <KDialog>

#include "ui_LibraryPatternProperties.h"


class LibraryPatternPropertiesDlg : public KDialog
{
    Q_OBJECT

public:
    LibraryPatternPropertiesDlg(QWidget *, qint32, Qt::KeyboardModifiers, qint16, const QString &, int, int, const QIcon &);
    ~LibraryPatternPropertiesDlg();

    qint32 key() const;
    Qt::KeyboardModifiers modifiers() const;
    qint16 baseline() const;

private:
    Ui::LibraryPatternProperties    ui;
};


#endif // LibraryPatternPropertiesDlg_H
