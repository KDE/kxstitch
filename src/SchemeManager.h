/*
 * Copyright (C) 2010-2015 by Stephen Allewell
 * steve.allewell@gmail.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#ifndef SchemeManager_H
#define SchemeManager_H

#include <QColor>
#include <QList>
#include <QStringList>

#include <KDirWatch>

class FlossScheme;

class SchemeManager : private QObject
{
    Q_OBJECT

public:
    ~SchemeManager();

    static FlossScheme *createScheme(QString schemeName);
    static FlossScheme *readScheme(QString name);
    static bool writeScheme(QString name);
    static QStringList schemes();
    static FlossScheme *scheme(QString name);

private:
    static SchemeManager &self();
    SchemeManager();

    void refresh();

    static SchemeManager *schemeManager;
    typedef QMap<QString, QColor> CalibratedColor;
    QList<FlossScheme *> m_flossSchemes;
};

#endif // SchemeManager_H
