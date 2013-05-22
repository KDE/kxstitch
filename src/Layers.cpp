/*
 * Copyright (C) 2010 by Stephen Allewell
 * sallewell@users.sourceforge.net
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#include "Layers.h"

#include <KDebug>


Layers::Layers()
    :   m_currentLayer(-1)
{
}


void Layers::clear()
{
    m_layers.clear();
    m_currentLayer = -1;
}


const QList<Layer> &Layers::layers() const
{
    return m_layers;
}


QStringList Layers::layerNames() const
{
    QStringList namesList;

    QListIterator<Layer> iterator(m_layers);

    while (iterator.hasNext()) {
        namesList.append(iterator.next().name());
    }

    return namesList;
}


QList<int> Layers::layerNumbers() const
{
    QList<int> numberList;

    QListIterator<Layer> iterator(m_layers);

    while (iterator.hasNext()) {
        numberList.append(iterator.next().number());
    }

    return numberList;
}


int Layers::currentLayer() const
{
    return m_currentLayer;
}


void Layers::addLayer(int layer, const QString &name)
{
    m_layers.append(Layer(layer, name));

    if (m_currentLayer == -1) {
        m_currentLayer = 0;
    }
}


void Layers::removeLayer(int number)
{
    QMutableListIterator<Layer> iterator(m_layers);

    while (iterator.hasNext()) {
        if (iterator.next().number() == number) {
            iterator.remove();
        }
    }

    if (m_layers.count() == 0) {
        m_currentLayer = -1;
    }
}


void Layers::removeLayer(const QString &name)
{
    QMutableListIterator<Layer> iterator(m_layers);

    while (iterator.hasNext()) {
        if (iterator.next().name() == name) {
            iterator.remove();
        }
    }

    if (m_layers.count() == 0) {
        m_currentLayer = -1;
    }
}


void Layers::setCurrentLayer(int layer)
{
    if (m_layers.count() > 0) {
        m_currentLayer = layer;
    }
}


QDataStream &operator<<(QDataStream &stream, const Layers &layers)
{
    stream << qint32(layers.version);
    stream << qint32(layers.m_currentLayer);
    stream << layers.m_layers;

    return stream;
}


QDataStream &operator>>(QDataStream &stream, Layers &layers)
{
    qint32 version;
    qint32 currentLayer;

    stream >> version;

    switch (version) {
    case 100:
        stream >> currentLayer;
        layers.m_currentLayer = currentLayer;
        stream >> layers.m_layers;
        break;

    default:
        // not supported
        // throw exception
        break;
    }

    return stream;
}
