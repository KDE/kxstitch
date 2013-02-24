/*
 * Copyright (C) 2010 by Stephen Allewell
 * stephen@mirramar.adsl24.co.uk
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#ifndef Floss_H
#define Floss_H


#include <QColor>
#include <QString>
#include <QVariant>


class Floss
{
public:
    Floss(const QString &, const QString &, const QColor &);

    const QString &name() const;
    const QString &description() const;
    const QColor &color() const;

    void setName(const QString &);
    void setDescription(const QString &);
    void setColor(const QColor &);

private:
    QString m_name;
    QString m_description;
    QColor  m_color;
};


#endif // Floss_H
