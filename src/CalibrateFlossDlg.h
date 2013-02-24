/*
 * Copyright (C) 2010 by Stephen Allewell
 * stephen@mirramar.adsl24.co.uk
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#ifndef CalibrateFlossDlg_H
#define CalibrateFlossDlg_H


#include <QColor>
#include <QMap>
#include <QString>

#include <KDialog>

#include "ui_CalibrateFloss.h"


class QListWidgetItem;
class QPixmap;
class SchemeManager;


class CalibrateFlossDlg : public KDialog
{
    Q_OBJECT

public:
    CalibrateFlossDlg(QWidget *, const QString &);
    ~CalibrateFlossDlg();

protected slots:
    virtual void slotButtonClicked(int button);

private slots:
    void on_SchemeList_currentIndexChanged(const QString &);
    void on_ColorList_currentItemChanged(QListWidgetItem *);
    void on_RedSlider_valueChanged(int);
    void on_GreenSlider_valueChanged(int);
    void on_BlueSlider_valueChanged(int);
    void on_ResetColor_clicked();

private:
    void fillSchemeList();
    void fillColorList();
    void updateSample();
    void updateName(bool modified);
    void commitColor();

    Ui::CalibrateFloss  ui;

    QString         m_schemeName;
    QListWidgetItem *m_item;
    QColor          m_sampleColor;
    QPixmap         *m_sample;

    typedef QMap<QString, QColor>   ChangedColors;
    QMap<QString, ChangedColors>    m_calibratedColors;
};


#endif // CalibrateFlossDlg_H
