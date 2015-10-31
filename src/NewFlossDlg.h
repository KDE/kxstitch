/*
 * Copyright (C) 2010-2015 by Stephen Allewell
 * steve.allewell@gmail.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#ifndef NewFlossDlg_H
#define NewFlossDlg_H


#include <QDialog>

#include "ui_NewFloss.h"


class QString;
class QWidget;

class Floss;
class FlossScheme;


class NewFlossDlg : public QDialog
{
    Q_OBJECT

public:
    NewFlossDlg(QWidget *, FlossScheme *flossScheme);
    ~NewFlossDlg();

    Floss *floss();

private slots:
    void on_FlossName_textEdited(const QString &);
    void on_FlossDescription_textEdited(const QString &);
    void on_DialogButtonBox_accepted();
    void on_DialogButtonBox_rejected();
    void on_DialogButtonBox_helpRequested();

private:
    Ui::NewFloss ui;

    FlossScheme *m_flossScheme;
    Floss       *m_floss;
};


#endif // NewFlossDlg_H
