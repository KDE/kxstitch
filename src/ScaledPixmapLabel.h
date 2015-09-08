/*
 * Copyright (C) 2015 by Stephen Allewell
 * steve.allewell@gmail.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#ifndef ScaledPixmapLabel_H
#define ScaledPixmapLabel_H


#include <QLabel>
#include <QPixmap>


class ScaledPixmapLabel : public QLabel
{
    Q_OBJECT

public:
    explicit ScaledPixmapLabel(QWidget *parent = 0);

    virtual int heightForWidth(int width) const;
    virtual QSize sizeHint() const;

public slots:
    void setPixmap(const QPixmap &);
    void resizeEvent(QResizeEvent *);

private:
    QPixmap m_pixmap;
};


#endif // ScaledPixmapLabel_H
