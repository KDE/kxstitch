/*
 * Copyright (C) 2010-2015 by Stephen Allewell
 * steve.allewell@gmail.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include "PrinterConfiguration.h"

#include <KLocalizedString>

#include "Element.h"
#include "Exceptions.h"

PrinterConfiguration::PrinterConfiguration()
{
}

PrinterConfiguration::PrinterConfiguration(const PrinterConfiguration &other)
{
    *this = other;
}

PrinterConfiguration::~PrinterConfiguration()
{
    qDeleteAll(m_pages);
}

bool PrinterConfiguration::isEmpty() const
{
    foreach (Page *page, m_pages) {
        if (!page->elements().isEmpty()) {
            return false;
        }
    }

    return true;
}

void PrinterConfiguration::addPage(Page *page)
{
    m_pages.append(page);
    updatePageNumbers();
}

void PrinterConfiguration::insertPage(int position, Page *page)
{
    m_pages.insert(position, page);
    updatePageNumbers();
}

void PrinterConfiguration::removePage(Page *page)
{
    m_pages.takeAt(m_pages.indexOf(page));
    updatePageNumbers();
}

QList<Page *> PrinterConfiguration::pages() const
{
    return m_pages;
}

PrinterConfiguration &PrinterConfiguration::operator=(const PrinterConfiguration &other)
{
    if (this != &other) {
        qDeleteAll(m_pages);
        m_pages.clear();

        QListIterator<Page *> pageIterator(other.m_pages);

        while (pageIterator.hasNext()) {
            m_pages.append(new Page(*pageIterator.next()));
        }
    }

    return *this;
}

QDataStream &operator<<(QDataStream &stream, const PrinterConfiguration &printerConfiguration)
{
    stream << qint32(printerConfiguration.version);

    stream << printerConfiguration.m_pages.count();
    QListIterator<Page *> pageIterator(printerConfiguration.m_pages);

    while (pageIterator.hasNext()) {
        stream << *pageIterator.next();
    }

    if (stream.status() != QDataStream::Ok) {
        throw FailedWriteFile(stream.status());
    }

    return stream;
}

QDataStream &operator>>(QDataStream &stream, PrinterConfiguration &printerConfiguration)
{
    qint32 version;
    qint32 pages;
    Page *page;

    stream >> version;

    switch (version) {
    case 100:
        stream >> pages;

        while (pages--) {
            page = new Page;
            stream >> *page;
            printerConfiguration.addPage(page);
        }

        break;

    default:
        throw InvalidFileVersion(QString(i18n("Printer configuration %1", version)));
        break;
    }

    if (stream.status() != QDataStream::Ok) {
        throw FailedReadFile(stream.status());
    }

    return stream;
}

void PrinterConfiguration::updatePageNumbers()
{
    int p = 1;
    QListIterator<Page *> pageIterator(m_pages);

    while (pageIterator.hasNext()) {
        pageIterator.next()->setPageNumber(p++);
    }
}
