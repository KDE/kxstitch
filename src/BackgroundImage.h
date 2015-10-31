/*
 * Copyright (C) 2010-2015 by Stephen Allewell
 * steve.allewell@gmail.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#ifndef BackgroundImage_H
#define BackgroundImage_H


#include <QDataStream>
#include <QIcon>
#include <QImage>
#include <QRect>
#include <QUrl>


class BackgroundImage
{
public:
    BackgroundImage();
    BackgroundImage(const QUrl &, const QRect &);

    const QUrl &url() const;
    const QRect &location() const;
    bool isVisible() const;
    bool isValid() const;
    const QImage &image() const;
    const QIcon &icon() const;

    void setLocation(const QRect &);
    void setVisible(bool);

    friend QDataStream &operator<<(QDataStream &, const BackgroundImage &);
    friend QDataStream &operator>>(QDataStream &, BackgroundImage &);

private:
    void generateIcon();

    static const int version = 101; // no longer store m_icon, generate it on loading

    QUrl    m_url;
    QRect   m_location;
    bool    m_visible;
    bool    m_status;
    QImage  m_image;
    QIcon   m_icon;
};


QDataStream &operator<<(QDataStream &, const BackgroundImage &);
QDataStream &operator>>(QDataStream &, BackgroundImage &);


#endif // BackgroundImage_H
