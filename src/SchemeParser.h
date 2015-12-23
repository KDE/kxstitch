/*
 * Copyright (C) 2010-2015 by Stephen Allewell
 * steve.allewell@gmail.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#ifndef SchemeParser_H
#define SchemeParser_H


#include <QXmlDefaultHandler>
#include <QString>
#include <QMap>
#include <QStack>


class FlossScheme;


class SchemeParser : public QXmlDefaultHandler
{
public:
    SchemeParser();

    FlossScheme *flossScheme();
    virtual bool startDocument() Q_DECL_OVERRIDE;
    virtual bool startElement(const QString &namespaceURI, const QString &localName, const QString &, const QXmlAttributes &qxmlAttributes) Q_DECL_OVERRIDE;
    virtual bool characters(const QString &d) Q_DECL_OVERRIDE;
    virtual bool endElement(const QString &namespaceURI, const QString &localName, const QString &qName) Q_DECL_OVERRIDE;
    virtual QString errorString() const Q_DECL_OVERRIDE;

private:
    int         m_errorCode;
    FlossScheme     *m_scheme;
    QMap<QString, QString>   m_flossMap;
    QStack<QString>     m_elements;
};


#endif // SchemeParser_H
