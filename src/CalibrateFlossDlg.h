/*
 * Copyright (C) 2010-2015 by Stephen Allewell
 * steve.allewell@gmail.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#ifndef CalibrateFlossDlg_H
#define CalibrateFlossDlg_H


#include <QColor>
#include <QDialog>
#include <QMap>
#include <QString>

#include "ui_CalibrateFloss.h"


class QListWidgetItem;
class QPixmap;
class SchemeManager;


class CalibrateFlossDlg : public QDialog
{
    Q_OBJECT

public:
    CalibrateFlossDlg(QWidget *, const QString &);
    ~CalibrateFlossDlg();

private slots:
    void on_SchemeList_currentIndexChanged(const QString &);
    void on_ColorList_currentItemChanged(QListWidgetItem *);
    void on_RedSlider_valueChanged(int);
    void on_GreenSlider_valueChanged(int);
    void on_BlueSlider_valueChanged(int);
    void on_ResetColor_clicked();
    void on_DialogButtonBox_accepted();
    void on_DialogButtonBox_rejected();
    void on_DialogButtonBox_helpRequested();

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
