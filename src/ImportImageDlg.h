/*
 * Copyright (C) 2010 by Stephen Allewell
 * sallewell@users.sourceforge.net
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#ifndef ImportImageDlg_H
#define ImportImageDlg_H


#include <QAction>
#include <QPixmap>
#include <QSize>
#include <QTimer>
#include <QWidget>

#include <KDebug>

#include <Magick++.h>

#include "AlphaSelect.h"
#include "ui_ImportImage.h"


class SchemeManager;


class ImportImageDlg : public KDialog
{
    Q_OBJECT

public:
    ImportImageDlg(QWidget *, const Magick::Image &);
    ~ImportImageDlg();

    Magick::Image convertedImage() const;
    bool ignoreColor() const;
    Magick::Color ignoreColorValue() const;
    QString flossScheme() const;
    double horizontalClothCount() const;
    double verticalClothCount() const;
    bool useFractionals() const;

protected:
    virtual void timerEvent(QTimerEvent *);

protected slots:
    void slotButtonClicked(int);

private slots:
    void on_FlossScheme_currentIndexChanged(const QString &);
    void on_UseMaximumColors_toggled(bool);
    void on_MaximumColors_valueChanged(int);
    void on_IgnoreColor_toggled(bool);
    void on_ColorButton_clicked(bool);
    void on_HorizontalClothCount_valueChanged(double);
    void on_VerticalClothCount_valueChanged(double);
    void on_ClothCountLink_clicked(bool);
    void on_PatternScale_valueChanged(int);
    void on_UseFractionals_toggled(bool);
    void selectColor(const QPoint &);

private:
    void clothCountChanged(double, double);
    void calculateSizes();
    void createImageMap();
    void renderPixmap();
    void pickColor();

    Ui::ImportImage ui;

    QPixmap     m_pixmap;
    QSize       m_originalSize;
    QSize       m_preferredSize;
    bool        m_useMaximumColors;
    bool        m_useFractionals;
    bool        m_ignoreColor;
    int         m_timer;
    AlphaSelect *m_alphaSelect;
    Magick::Color   m_ignoreColorValue;
    Magick::Image   m_originalImage;
    Magick::Image   m_convertedImage;
    Magick::Image   m_colorMap;
};


#endif // ImportImageDlg_H
