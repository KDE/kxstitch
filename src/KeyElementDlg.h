/*
 * Copyright (C) 2010-2015 by Stephen Allewell
 * steve.allewell@gmail.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#ifndef KeyElementDlg_H
#define KeyElementDlg_H

#include <QDialog>

#include "ui_KeyElement.h"

class KeyElement;
class QHideEvent;
class QShowEvent;

class KeyElementDlg : public QDialog
{
    Q_OBJECT

public:
    KeyElementDlg(QWidget *, KeyElement *);
    virtual ~KeyElementDlg() = default;

protected:
    virtual void hideEvent(QHideEvent *) Q_DECL_OVERRIDE;
    virtual void showEvent(QShowEvent *) Q_DECL_OVERRIDE;

private slots:
    void on_DialogButtonBox_accepted();
    void on_DialogButtonBox_rejected();
    void on_DialogButtonBox_helpRequested();

private:
    KeyElement *m_keyElement;

    Ui::KeyElement ui;
};

#endif // KeyElementDlg_H
