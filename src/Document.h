/*
 * Copyright (C) 2010-2015 by Stephen Allewell
 * steve.allewell@gmail.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#ifndef Document_H
#define Document_H

#include <QPolygon>
#include <QUndoStack>
#include <QUrl>

#include "BackgroundImages.h"
#include "Exceptions.h"
#include "Pattern.h"
#include "PrinterConfiguration.h"
#include "configuration.h"

class Editor;
class Palette;
class Preview;

class Document
{
public:
    Document();
    ~Document();

    void initialiseNew();

    void readKXStitch(QDataStream &);
    void readPCStitch(QDataStream &);
    void write(QDataStream &);

    void setUrl(const QUrl &);
    QUrl url() const;

    void addView(Editor *);
    void addView(Palette *);
    void addView(Preview *);

    Editor *editor() const;
    Palette *palette() const;
    Preview *preview() const;

    QVariant property(const QString &) const;
    void setProperty(const QString &, const QVariant &);

    QUndoStack &undoStack();

    BackgroundImages &backgroundImages();
    Pattern *pattern();
    const PrinterConfiguration &printerConfiguration() const;
    void setPrinterConfiguration(const PrinterConfiguration &);

private:
    void readPCStitch5File(QDataStream &);
    void readPCStitch6File(QDataStream &);
    void readPCStitch7File(QDataStream &);
    QString readPCStitchString(QDataStream &);

    void readKXStitchV2File(QDataStream &);
    void readKXStitchV3File(QDataStream &);
    void readKXStitchV4File(QDataStream &);
    void readKXStitchV5File(QDataStream &);
    void readKXStitchV6File(QDataStream &);
    void readKXStitchV7File(QDataStream &);

    static const int version = 104;

    QMap<QString, QVariant> m_properties;

    QUrl m_url;

    QUndoStack m_undoStack;

    Editor *m_editor;
    Palette *m_palette;
    Preview *m_preview;

    BackgroundImages m_backgroundImages;
    Pattern *m_pattern;
    PrinterConfiguration m_printerConfiguration;
};

#endif // Document_H
