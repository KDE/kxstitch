/*
 * Copyright (C) 2010 by Stephen Allewell
 * stephen@mirramar.adsl24.co.uk
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#include "Page.h"

#include <QPainter>
#include <QPrinterInfo>

#include <KDebug>
#include <KLocale>

#include "Document.h"
#include "Element.h"
#include "Exceptions.h"
#include "PaperSizes.h"


Page::Page(QPrinter::PaperSize paperSize, QPrinter::Orientation orientation)
    :   m_pageNumber(0),
        m_paperSize(paperSize),
        m_orientation(orientation),
        m_margins(QMargins(Configuration::page_MarginLeft(), Configuration::page_MarginTop(), Configuration::page_MarginRight(), Configuration::page_MarginBottom()))
{
}


Page::Page(const Page &other)
{
    *this = other;
}


Page::~Page()
{
    qDeleteAll(m_elements);
}


Page &Page::operator=(const Page &other)
{
    if (this != &other) {
        qDeleteAll(m_elements);
        m_elements.clear();

        m_pageNumber = other.m_pageNumber;
        m_paperSize = other.m_paperSize;
        m_orientation = other.m_orientation;
        m_margins = other.m_margins;

        QListIterator<Element *> elementIterator(other.m_elements);

        while (elementIterator.hasNext()) {
            Element *element = elementIterator.next();

            if (element->type() != Element::Plan) {
                Element *cloned = element->clone();
                cloned->setParent(this);
                m_elements.append(cloned);

                if (cloned->type() == Element::Pattern) {
                    if (dynamic_cast<PatternElement *>(cloned)->showPlan()) {
                        cloned = dynamic_cast<PatternElement *>(cloned)->planElement();
                        cloned->setParent(this);
                        m_elements.append(cloned);
                    }
                }
            }
        }
    }

    return *this;
}


int Page::pageNumber() const
{
    return m_pageNumber;
}


QPrinter::PaperSize Page::paperSize() const
{
    return m_paperSize;
}


QPrinter::Orientation Page::orientation() const
{
    return m_orientation;
}


const QMargins &Page::margins() const
{
    return m_margins;
}


const QList<Element *> Page::elements() const
{
    return m_elements;
}


void Page::setPageNumber(int pageNumber)
{
    m_pageNumber = pageNumber;
}


void Page::setPaperSize(QPrinter::PaperSize paperSize)
{
    m_paperSize = paperSize;
}


void Page::setOrientation(QPrinter::Orientation orientation)
{
    m_orientation = orientation;
}


void Page::setMargins(const QMargins &margins)
{
    m_margins = margins;
}


void Page::addElement(Element *element)
{
    m_elements.append(element);
}


void Page::removeElement(Element *element)
{
    m_elements.removeOne(element);
}


void Page::render(Document *document, QPainter *painter) const
{
    painter->save();
    painter->drawRect(0, 0, painter->window().width(), painter->window().height());

    QListIterator<Element *> elementIterator(m_elements);

    while (elementIterator.hasNext()) {
        Element *element = elementIterator.next();
        element->render(document, painter);
    }

    painter->restore();
}


Element *Page::itemAt(const QPoint &pos) const
{
    Element *element = 0;

    if (m_elements.count()) {
        QListIterator<Element *> elementIterator(m_elements);
        elementIterator.toBack();  // start from the end of the list

        while (elementIterator.hasPrevious()) {
            Element *testElement = elementIterator.previous();

            if (testElement->rectangle().contains(pos)) {
                element = testElement;
                break;
            }
        }
    }

    return element; // will be the element under the cursor or 0
}


QDataStream &operator<<(QDataStream &stream, const Page &page)
{
    stream << qint32(page.version);

    stream  << qint32(page.m_pageNumber)
            << qint32(page.m_paperSize)
            << qint32(page.m_orientation)
            << qint32(page.m_margins.left())
            << qint32(page.m_margins.top())
            << qint32(page.m_margins.right())
            << qint32(page.m_margins.bottom())
            << qint32(page.m_elements.count());

    QListIterator<Element *> elementIterator(page.m_elements);

    while (elementIterator.hasNext()) {
        const Element *element = elementIterator.next();
        stream  << qint32(element->type());

        if (element->type() != Element::Plan) {
            stream << *element;
        }
    }

    if (stream.status() != QDataStream::Ok) {
        throw FailedWriteFile(stream.status());
    }

    return stream;
}


QDataStream &operator>>(QDataStream &stream, Page &page)
{
    qint32 version;
    qint32 pageNumber;
    qint32 paperSize;
    qint32 orientation;
    qint32 left;
    qint32 top;
    qint32 right;
    qint32 bottom;
    qint32 showGrid;
    qint32 gridX;
    qint32 gridY;
    qreal zoomFactor;

    stream >> version;

    switch (version) {
    case 102:
        stream  >> pageNumber
                >> paperSize
                >> orientation
                >> left
                >> top
                >> right
                >> bottom;
        page.m_pageNumber = pageNumber;
        page.m_paperSize = static_cast<QPrinter::PaperSize>(paperSize);
        page.m_orientation = static_cast<QPrinter::Orientation>(orientation);
        page.m_margins = QMargins(left, top, right, bottom);

        page.readElements(stream);
        break;

    case 101:
        stream  >> pageNumber
                >> paperSize
                >> orientation
                >> left
                >> top
                >> right
                >> bottom
                >> showGrid
                >> gridX
                >> gridY;
        page.m_pageNumber = pageNumber;
        page.m_paperSize = static_cast<QPrinter::PaperSize>(paperSize);
        page.m_orientation = static_cast<QPrinter::Orientation>(orientation);
        page.m_margins = QMargins(left, top, right, bottom);

        page.readElements(stream);

        stream >> zoomFactor;
        break;

    case 100:
        stream  >> paperSize
                >> orientation
                >> left
                >> top
                >> right
                >> bottom;
        page.m_paperSize = static_cast<QPrinter::PaperSize>(paperSize);
        page.m_orientation = static_cast<QPrinter::Orientation>(orientation);
        page.m_margins = QMargins(left, top, right, bottom);

        page.readElements(stream);

        stream >> zoomFactor;
        break;

    default:
        throw InvalidFileVersion(QString(i18n("Page version %1", version)));
        break;
    }

    return stream;
}


void Page::readElements(QDataStream &stream)
{
    qint32 elements;
    qint32 type;
    Element *element;

    stream >> elements;

    while (elements--) {
        stream >> type;

        switch (static_cast<Element::Type>(type)) {
        case Element::Text:
            element = new TextElement(this, QRect());
            break;

        case Element::Pattern:
            element = new PatternElement(this, QRect());
            break;

        case Element::Plan:
            element = 0; // handled by the PatternElement
            break;

        case Element::Key:
            element = new KeyElement(this, QRect());
            break;

        case Element::Image:
            element = new ImageElement(this, QRect());
            break;

        default:
            throw FailedReadFile(QString(i18n("Invalid element type")));
            break;
        }

        if (element) {
            stream >> *element;
            m_elements.append(element);
            if (element->type() == Element::Pattern) {
                if (dynamic_cast<PatternElement *>(element)->showPlan()) {
                    m_elements.append(dynamic_cast<PatternElement *>(element)->planElement());
                }
            }
        }
    }
}
