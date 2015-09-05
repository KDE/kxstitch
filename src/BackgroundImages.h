/*
 * Copyright (C) 2010-2015 by Stephen Allewell
 * steve.allewell@gmail.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#ifndef BackgroundImages_H
#define BackgroundImages_H


#include <QDataStream>
#include <QList>
#include <QRect>


class BackgroundImage;


class BackgroundImages
{
public:
    BackgroundImages();
    ~BackgroundImages();

    void clear();

    QListIterator<BackgroundImage *> backgroundImages();

    void addBackgroundImage(BackgroundImage *);
    bool removeBackgroundImage(BackgroundImage *);
    QRect fitBackgroundImage(BackgroundImage *, const QRect &);
    bool showBackgroundImage(BackgroundImage *, bool);

    friend QDataStream &operator<<(QDataStream &, const BackgroundImages &);
    friend QDataStream &operator>>(QDataStream &, BackgroundImages &);

private:
    static const int version = 100;

    QList<BackgroundImage *> m_backgroundImages;
};


QDataStream &operator<<(QDataStream &, const BackgroundImages &);
QDataStream &operator>>(QDataStream &, BackgroundImages &);


#endif // BackgroundImages_H
