/*
 * Copyright (C) 2010 by Stephen Allewell
 * stephen@mirramar.adsl24.co.uk
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

#include <KDebug>
#include <KLocale>

#include "FlossScheme.h"
#include "Floss.h"


/**
    Valid tags that can exist in the file.
    */
const char *validTags[] = {
    "flossScheme",  /**< The enclosing tags for the file */
    "title",    /**< The title of the scheme */
    "floss",    /**< Enclosing tags for a floss definition */
    "name",     /**< The name of the floss */
    "description",  /**< The descriptive text for the floss */
    "color",    /**< Enclosing tags for a color definition */
    "red",      /**< Red color value */
    "green",    /**< Green color value */
    "blue",     /**< Blue color value */
    0       /**< List termination value */
};


/**
    Error messages.
    */
const char *errorMessages[] = {
    "No error",
    "Floss not completely defined",
    "Unmatched element tags",
    "Unknown element tag",
    "Color value is invalid",
    "Unknown error code"
};


/**
    Constructor.  Initialises pointers to 0 and set the m_errorCode value to 0.
    */
SchemeParser::SchemeParser()
    :   m_errorCode(0),
        m_scheme(0)
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

    int tag = 0;

    while (validTags[tag] && name != validTags[tag]) {
        tag++;
    }

    if (validTags[tag]) {
        m_elements.push(new QString(name));
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

    if (*m_elements.top() == "title") {
        m_scheme->setSchemeName(data);
    } else {
        m_flossMap.insert(*m_elements.top(), data);
    }

    if (*m_elements.top() == "red" || *m_elements.top() == "green" || *m_elements.top() == "blue") {
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
    QString *s = m_elements.pop();

    if (*s == name) {
        if (name == "floss") {
            if (m_flossMap.contains("name") && m_flossMap.contains("description") && m_flossMap.contains("red") && m_flossMap.contains("green") && m_flossMap.contains("blue")) {
                m_scheme->addFloss(new Floss(m_flossMap["name"], m_flossMap["description"], QColor(m_flossMap["red"].toInt(), m_flossMap["green"].toInt(), m_flossMap["blue"].toInt())));
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

    delete s;

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
    return QString(errorMessages[m_errorCode]);
}
