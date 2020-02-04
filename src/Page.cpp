/*
 * Copyright (C) 2010-2015 by Stephen Allewell
 * steve.allewell@gmail.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#include "Page.h"

#include <QPainter>
#include <QPrinterInfo>

#include <KLocalizedString>

#include "Document.h"
#include "Element.h"
#include "Exceptions.h"
#include "PaperSizes.h"


Page::Page(QPageSize pageSize, QPageLayout::Orientation orientation)
    :   QPageLayout(pageSize, orientation, QMarginsF(Configuration::page_MarginLeft(), Configuration::page_MarginTop(), Configuration::page_MarginRight(), Configuration::page_MarginBottom()), QPageLayout::Millimeter),
        m_pageNumber(0)
{
}


Page::Page(const Page &other)
    :   QPageLayout(other)
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


const QList<Element *> Page::elements() const
{
    return m_elements;
}


void Page::setPageNumber(int pageNumber)
{
    m_pageNumber = pageNumber;
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

    if (painter->device()->paintEngine() == nullptr) {
        painter->setPen(QPen(Qt::red, 0.05));
        painter->drawRect(painter->window().marginsRemoved(margins().toMargins()));
    }

    QListIterator<Element *> elementIterator(m_elements);

    while (elementIterator.hasNext()) {
        Element *element = elementIterator.next();
        element->render(document, painter);
    }

    painter->restore();
}


Element *Page::itemAt(const QPoint &pos) const
{
    Element *element = nullptr;

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
            << qint32(page.pageSize().id())
            << qint32(page.orientation())
            << qint32(page.margins().left())
            << qint32(page.margins().top())
            << qint32(page.margins().right())
            << qint32(page.margins().bottom())
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
    qint32 pageSizeId;
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
                >> pageSizeId
                >> orientation
                >> left
                >> top
                >> right
                >> bottom;
        page.m_pageNumber = pageNumber;
        page.setPageSize(QPageSize(static_cast<QPageSize::PageSizeId>(pageSizeId)));
        page.setOrientation(static_cast<QPageLayout::Orientation>(orientation));
        page.setMargins(QMarginsF(left, top, right, bottom));

        page.readElements(stream);
        break;

    case 101:
        stream  >> pageNumber
                >> pageSizeId
                >> orientation
                >> left
                >> top
                >> right
                >> bottom
                >> showGrid
                >> gridX
                >> gridY;
        page.m_pageNumber = pageNumber;
        page.setPageSize(QPageSize(static_cast<QPageSize::PageSizeId>(pageSizeId)));
        page.setOrientation(static_cast<QPageLayout::Orientation>(orientation));
        page.setMargins(QMarginsF(left, top, right, bottom));

        page.readElements(stream);

        stream >> zoomFactor;
        break;

    case 100:
        stream  >> pageSizeId
                >> orientation
                >> left
                >> top
                >> right
                >> bottom;
        page.setPageSize(QPageSize(static_cast<QPageSize::PageSizeId>(pageSizeId)));
        page.setOrientation(static_cast<QPageLayout::Orientation>(orientation));
        page.setMargins(QMarginsF(left, top, right, bottom));

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
            element = nullptr; // handled by the PatternElement
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
