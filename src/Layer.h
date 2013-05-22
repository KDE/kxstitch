/*
 * Copyright (C) 2010 by Stephen Allewell
 * sallewell@users.sourceforge.net
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#ifndef Layer_H
#define Layer_H


#include <QDataStream>
#include <QString>


class Layer
{
public:
    Layer();
    Layer(int, const QString &);

    int number() const;
    const QString &name() const;

    void setNumber(int);
    void setName(const QString &);

    friend QDataStream &operator<<(QDataStream &, const Layer &);
    friend QDataStream &operator>>(QDataStream &, Layer &);

private:
    static const int version = 100;

    int     m_number;
    QString m_name;
};


QDataStream &operator<<(QDataStream &, const Layer &);
QDataStream &operator>>(QDataStream &, Layer &);


#endif // Layer_H
