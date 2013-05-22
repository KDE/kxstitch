/*
 * Copyright (C) 2010 by Stephen Allewell
 * sallewell@users.sourceforge.net
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#ifndef PatternElementDlg_H
#define PatternElementDlg_H


#include <QList>
#include <QRect>

#include <KDialog>

#include "ui_PatternElement.h"


class Document;
class PlanElement;
class PatternElement;
class SelectArea;


class PatternElementDlg : public KDialog
{
    Q_OBJECT

public:
    PatternElementDlg(QWidget *, PatternElement *, Document *, const QList<QRect> &);
    ~PatternElementDlg();

    bool showPlan() const;
    PlanElement *planElement() const;

protected slots:
    void slotButtonClicked(int);

private slots:

private:
    SelectArea  *m_selectArea;

    PatternElement  *m_patternElement;
    Document    *m_document;

    QRect       m_patternRect;
    QList<QRect>    m_patternRects;

    Ui::PatternElement  ui;
};


#endif // PatternElementDlg_H
