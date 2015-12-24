/*
 * Copyright (C) 2010-2015 by Stephen Allewell
 * steve.allewell@gmail.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#ifndef ExtendPatternDlg_H
#define ExtendPatternDlg_H


#include "ui_ExtendPattern.h"

#include <QDialog>


class QHideEvent;
class QShowEvent;


class ExtendPatternDlg : public QDialog
{
    Q_OBJECT

public:
    explicit ExtendPatternDlg(QWidget *);
    virtual ~ExtendPatternDlg() = default;

    int top() const;
    int left() const;
    int right() const;
    int bottom() const;

protected:
    virtual void hideEvent(QHideEvent *) Q_DECL_OVERRIDE;
    virtual void showEvent(QShowEvent *) Q_DECL_OVERRIDE;

private slots:
    void on_DialogButtonBox_accepted();
    void on_DialogButtonBox_rejected();
    void on_DialogButtonBox_helpRequested();

private:
    Ui::ExtendPattern   ui;
};


#endif // ExtendPatternDlg_H
