/*
 * Copyright (C) 2010-2015 by Stephen Allewell
 * steve.allewell@gmail.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#include "Layer.h"


Layer::Layer()
{
}


Layer::Layer(int number, const QString &name)
    :   m_number(number),
        m_name(name)
{
}


int Layer::number() const
{
    return m_number;
}


const QString &Layer::name() const
{
    return m_name;
}


void Layer::setNumber(int number)
{
    number = number;
}


void Layer::setName(const QString &name)
{
    m_name = name;
}


QDataStream &operator<<(QDataStream &stream, const Layer &layer)
{
    stream << qint32(layer.version);
    stream << qint32(layer.m_number);
    stream << layer.m_name;

    return stream;
}


QDataStream &operator>>(QDataStream &stream, Layer &layer)
{
    qint32 version;
    qint32 number;

    stream >> version;

    switch (version) {
    case 100:
        stream >> number;
        layer.m_number = number;
        stream >> layer.m_name;
        break;

    default:
        // not supported
        // throw exception
        break;
    }

    return stream;
}
