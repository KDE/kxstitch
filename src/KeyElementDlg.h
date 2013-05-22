/*
 * Copyright (C) 2010 by Stephen Allewell
 * sallewell@users.sourceforge.net
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#ifndef KeyElementDlg_H
#define KeyElementDlg_H


#include "ui_KeyElement.h"


class KeyElement;


class KeyElementDlg : public KDialog
{
    Q_OBJECT

public:
    KeyElementDlg(QWidget *, KeyElement *);
    ~KeyElementDlg();

protected slots:
    void slotButtonClicked(int);

private:
    KeyElement  *m_keyElement;

    Ui::KeyElement  ui;
};


#endif // KeyElementDlg_H
