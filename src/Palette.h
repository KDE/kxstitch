/*
 * Copyright (C) 2010-2014 by Stephen Allewell
 * steve.allewell@gmail.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#ifndef Palette_H
#define Palette_H


#include <QFrame>


class Document;


class Palette : public QFrame
{
    Q_OBJECT

public:
    enum Mode {Select, Replace, Swap};

    explicit Palette(QWidget *);
    virtual ~Palette();

    virtual QSize sizeHint() const;
    Document *document() const;

    void setDocument(Document *);

public slots:
    void showSymbols(bool);
    void swapColors();
    void replaceColor();

    void loadSettings();

signals:
    void colorSelected(int);
    void swapColors(int, int);
    void replaceColor(int, int);
    void signalStateChanged(QString, bool);

protected:
    virtual bool event(QEvent *);
    virtual void paintEvent(QPaintEvent *);
    virtual void mousePressEvent(QMouseEvent *);

private:
    Document    *m_document;
    bool        m_showSymbols;
    int         m_cols;
    int         m_rows;
    int         m_width;
    int         m_height;
    int         m_flosses;
    QVector<int>    m_paletteIndex;

    Mode        m_mode;
};


#endif // Palette_H
