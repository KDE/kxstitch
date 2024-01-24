/*
 * Copyright (C) 2010-2015 by Stephen Allewell
 * steve.allewell@gmail.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

/**
    @file
    Implements the scheme parser to read an XML formatted scheme file.
    */

#include "SchemeParser.h"

#include <KLocalizedString>

#include "Floss.h"
#include "FlossScheme.h"

/**
    Valid tags that can exist in the file.
    */
const QStringList validTags = {
    QStringLiteral("flossScheme"), /**< The enclosing tags for the file */
    QStringLiteral("title"), /**< The title of the scheme */
    QStringLiteral("floss"), /**< Enclosing tags for a floss definition */
    QStringLiteral("name"), /**< The name of the floss */
    QStringLiteral("description"), /**< The descriptive text for the floss */
    QStringLiteral("color"), /**< Enclosing tags for a color definition */
    QStringLiteral("red"), /**< Red color value */
    QStringLiteral("green"), /**< Green color value */
    QStringLiteral("blue") /**< Blue color value */
};

/**
    Error messages.
    */
const char *errorMessages[] = {I18N_NOOP("No error"),
                               I18N_NOOP("Floss not completely defined"),
                               I18N_NOOP("Unmatched element tags"),
                               I18N_NOOP("Unknown element tag"),
                               I18N_NOOP("Color value is invalid"),
                               I18N_NOOP("Unknown error code")};

/**
    Constructor.  Initialises pointers to 0 and set the m_errorCode value to 0.
    */
SchemeParser::SchemeParser()
    : m_errorCode(0)
    , m_scheme(nullptr)
{
}

/**
    Get a pointer to the read FlossScheme.
    @return pointer to FlossScheme class.
    */
FlossScheme *SchemeParser::flossScheme()
{
    return m_scheme;
}

/**
    Start a new XML document, creating an instance of a FlossScheme.
    @return true if the scheme is created, false otherwise.
    */
bool SchemeParser::startDocument()
{
    m_scheme = new FlossScheme;
    return (m_scheme) ? true : false;
}

/**
    Start a new element. Reimplemented from QXmlDefaultHandler.
    @param namespaceURI not used
    @param localName not used
    @param qName name of the tag.
    @param qxmlAttributes not used
    @return true if the tag is valid, false otherwise.
    */
bool SchemeParser::startElement(const QString &namespaceURI, const QString &localName, const QString &qName, const QXmlAttributes &qxmlAttributes)
{
    Q_UNUSED(namespaceURI);
    Q_UNUSED(localName);
    Q_UNUSED(qxmlAttributes);

    QString name = qName.simplified();

    if (validTags.contains(name)) {
        m_elements.push(name);
        return true;
    }

    // invalid tag found
    m_errorCode = 3;
    return false;
}

/**
    Process a string of characters.  Reimplemented from QXmlDefaultHandler.
    @param d QString reference of the characters read from the file.
    @return true if the data is acceptable, false otherwise.
    */
bool SchemeParser::characters(const QString &d)
{
    QString data = d.simplified();

    if (data.isEmpty()) {
        return true;
    }

    if (m_elements.top() == QLatin1String("title")) {
        m_scheme->setSchemeName(data);
    } else {
        m_flossMap.insert(m_elements.top(), data);
    }

    if (m_elements.top() == QLatin1String("red") || m_elements.top() == QLatin1String("green") || m_elements.top() == QLatin1String("blue")) {
        if (data.toInt() > 255) {
            // color value is not valid
            m_errorCode = 4;
            return false;
        }
    }

    return true;
}

/**
    End an element. Reimplemented from QXmlDefaultHandler.
    @param namespaceURI not used
    @param localName not used
    @param qName name of the tag.
    @return true if the tag is valid, false otherwise.
    */
bool SchemeParser::endElement(const QString &namespaceURI, const QString &localName, const QString &qName)
{
    Q_UNUSED(namespaceURI);
    Q_UNUSED(localName);

    QString name = qName.simplified();
    QString s = m_elements.pop();

    if (s == name) {
        if (name == QLatin1String("floss")) {
            if (m_flossMap.contains(QLatin1String("name")) && m_flossMap.contains(QLatin1String("description")) && m_flossMap.contains(QLatin1String("red"))
                && m_flossMap.contains(QLatin1String("green")) && m_flossMap.contains(QLatin1String("blue"))) {
                m_scheme->addFloss(new Floss(
                    m_flossMap[QLatin1String("name")],
                    m_flossMap[QLatin1String("description")],
                    QColor(m_flossMap[QLatin1String("red")].toInt(), m_flossMap[QLatin1String("green")].toInt(), m_flossMap[QLatin1String("blue")].toInt())));
                m_flossMap.clear();
            } else {
                // not all elements defined for a floss
                m_errorCode = 1;
            }
        }
    } else
    // mismatched tags
    {
        m_errorCode = 2;
    }

    if (m_errorCode) {
        return false;
    } else {
        return true;
    }
}

/**
    Return an error string related to the last error code encountered which is stored in m_errorCode.
    @return QString containing the error message.
    */
QString SchemeParser::errorString() const
{
    return i18n(errorMessages[m_errorCode]);
}
