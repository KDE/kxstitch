/*
 * Copyright (C) 2010-2015 by Stephen Allewell
 * steve.allewell@gmail.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#ifndef PatternElementDlg_H
#define PatternElementDlg_H


#include <QDialog>
#include <QList>
#include <QRect>

#include "ui_PatternElement.h"


class Document;
class PlanElement;
class PatternElement;
class SelectArea;
class QHideEvent;
class QShowEvent;


class PatternElementDlg : public QDialog
{
    Q_OBJECT

public:
    PatternElementDlg(QWidget *, PatternElement *, Document *, const QMap<int, QList<QRect> > &);
    virtual ~PatternElementDlg() = default;

    bool showPlan() const;
    PlanElement *planElement() const;

protected:
    virtual void hideEvent(QHideEvent *) Q_DECL_OVERRIDE;
    virtual void showEvent(QShowEvent *) Q_DECL_OVERRIDE;

private slots:
    void on_DialogButtonBox_accepted();
    void on_DialogButtonBox_rejected();
    void on_DialogButtonBox_helpRequested();

private:
    SelectArea  *m_selectArea;

    PatternElement  *m_patternElement;
    Document    *m_document;

    QRect                       m_patternRect;
    QMap<int, QList<QRect> >    m_patternRects;

    Ui::PatternElement  ui;
};


#endif // PatternElementDlg_H
