/*
 * Copyright (C) 2010-2015 by Stephen Allewell
 * steve.allewell@gmail.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#ifndef FlossScheme_H
#define FlossScheme_H

#include <QColor>
#include <QList>
#include <QListIterator>
#include <QString>

// wrap include to silence unused-parameter warning from Magick++ include file
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wsuggest-override"
#include <Magick++.h>
#pragma GCC diagnostic pop

#include "Floss.h"

class FlossScheme
{
public:
    FlossScheme();
    ~FlossScheme();

    Floss *convert(const QColor &color);
    Floss *find(const QString &name) const;
    Floss *find(const QColor &color) const;
    QString schemeName() const;
    QString path() const;
    const QList<Floss *> &flosses() const;

    void addFloss(Floss *floss);
    void clearScheme();
    Magick::Image *createImageMap();
    void setSchemeName(const QString &name);
    void setPath(const QString &name);

private:
    QString m_schemeName;
    QString m_path;
    QList<Floss *> m_flosses;
    Magick::Image *m_map;
};

#endif // FlossScheme_H
