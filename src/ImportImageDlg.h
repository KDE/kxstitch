/*
 * Copyright (C) 2010-2015 by Stephen Allewell
 * steve.allewell@gmail.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#ifndef ImportImageDlg_H
#define ImportImageDlg_H


#include <QAction>
#include <QDialog>
#include <QPixmap>
#include <QSize>
#include <QTimer>
#include <QWidget>

// wrap include to silence unused-parameter warning from Magick++ include file
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wsuggest-override"
#include <Magick++.h>
#pragma GCC diagnostic pop

#include "AlphaSelect.h"
#include "ui_ImportImage.h"


class SchemeManager;
class QHideEvent;
class QShowEvent;


class ImportImageDlg : public QDialog
{
    Q_OBJECT

public:
    ImportImageDlg(QWidget *, const Magick::Image &);
    virtual ~ImportImageDlg() = default;

    Magick::Image convertedImage() const;
    bool ignoreColor() const;
    Magick::Color ignoreColorValue() const;
    QString flossScheme() const;
    double horizontalClothCount() const;
    double verticalClothCount() const;
    bool useFractionals() const;
    QRect croppedArea() const;

protected:
    virtual void hideEvent(QHideEvent *) Q_DECL_OVERRIDE;
    virtual void showEvent(QShowEvent *) Q_DECL_OVERRIDE;
    virtual void timerEvent(QTimerEvent *) Q_DECL_OVERRIDE;

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
    void on_CropEnabled_toggled(bool);
    void on_CropReset_clicked(bool);
    void imageCropped(const QRectF &rectF);
    void on_UseFractionals_toggled(bool);
    void selectColor(const QPoint &);
    void on_DialogButtonBox_accepted();
    void on_DialogButtonBox_rejected();
    void on_DialogButtonBox_helpRequested();
    void on_DialogButtonBox_clicked(QAbstractButton *);

private:
    void updateWindowTitle();
    void resetImportParameters();
    void clothCountChanged(double, double);
    void calculateSizes();
    void createImageMap();
    void renderPixmap();
    void pickColor();

    Ui::ImportImage ui;

    QPixmap     m_pixmap;
    QSize       m_originalSize;
    QSize       m_preferredSize;
    int         m_timer;
    AlphaSelect *m_alphaSelect;
    Magick::ColorRGB    m_ignoreColorValue;
    Magick::Image       m_originalImage;
    Magick::Image       m_convertedImage;
    Magick::Image       m_colorMap;
    QRect       m_crop;
};


#endif // ImportImageDlg_H
