/***************************************************************************
 *   Copyright (C) 2012 by Stephen Allewell                                *
 *   stephen@mirramar.fsnet.co.uk                                          *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/


#ifndef CharSelectorDlg_H
#define CharSelectorDlg_H


#include <KDialog>

#include "ui_CharSelector.h"


class CharSelectorDlg : public KDialog
{
    Q_OBJECT

    public:
        CharSelectorDlg(QWidget *parent, QList<QChar> usedSymbols);

        void setSelectedChar(const QChar &);
        QChar selectedChar();

    protected slots:
        void on_CharSelect_currentCharChanged(const QChar &);

    private:
        Ui::CharSelector    ui;

        QList<QChar>    m_usedSymbols;
        QChar           m_currentChar;
};


#endif // CharSelectorDlg_H
