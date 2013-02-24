/*
 * Copyright (C) 2010 by Stephen Allewell
 * stephen@mirramar.adsl24.co.uk
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#include "Floss.h"


Floss::Floss(const QString &name, const QString &description, const QColor &color)
    :   m_name(name),
        m_description(description),
        m_color(color)
{
}


const QString &Floss::name() const
{
    return m_name;
}


const QString &Floss::description() const
{
    return m_description;
}


const QColor &Floss::color() const
{
    return m_color;
}


void Floss::setName(const QString &name)
{
    m_name = name;
}


void Floss::setDescription(const QString &description)
{
    m_description = description;
}


void Floss::setColor(const QColor &color)
{
    m_color = color;
}
