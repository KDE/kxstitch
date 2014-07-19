/*
 * Copyright (C) 2010-2014 by Stephen Allewell
 * steve.allewell@gmail.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#ifndef Layers_H
#define Layers_H


#include <QDataStream>
#include <QList>
#include <QStringList>

#include "Layer.h"


class Layers
{
public:
    Layers();

    void clear();

    const QList<Layer> &layers() const;
    QStringList layerNames() const;
    QList<int> layerNumbers() const;
    int currentLayer() const;

    void addLayer(int, const QString &);
    void removeLayer(int);
    void removeLayer(const QString &);
    void setCurrentLayer(int);

    friend QDataStream &operator<<(QDataStream &, const Layers &);
    friend QDataStream &operator>>(QDataStream &, Layers &);

private:
    static const int version = 100;

    int             m_currentLayer;
    QList<Layer>    m_layers;
};


QDataStream &operator<<(QDataStream &, const Layers &);
QDataStream &operator>>(QDataStream &, Layers &);


#endif // Layers_H
