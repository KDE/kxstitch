/********************************************************************************
 *	Copyright (C) 2010 by Stephen Allewell					*
 *	stephen@mirramar.adsl24.co.uk						*
 *										*
 *	This program is free software; you can redistribute it and/or modify	*
 *	it under the terms of the GNU General Public License as published by	*
 *	the Free Software Foundation; either version 2 of the License, or	*
 *	(at your option) any later version.					*
 ********************************************************************************/


#include "PrinterConfiguration.h"

#include <QSharedData>

#include "Element.h"


PrinterConfigurationPrivate::PrinterConfigurationPrivate()
	:	QSharedData()
{
}


PrinterConfigurationPrivate::PrinterConfigurationPrivate(const PrinterConfigurationPrivate &other)
{
	QListIterator<Page *> pageIterator(other.m_pages);
	while (pageIterator.hasNext())
	{
		Page *page = pageIterator.next();
		m_pages.append(new Page(*page));
	}
}


PrinterConfigurationPrivate::~PrinterConfigurationPrivate()
{
	clear();
}


void PrinterConfigurationPrivate::clear()
{
	qDeleteAll(m_pages);
}


PrinterConfiguration::PrinterConfiguration()
	:	d(new PrinterConfigurationPrivate)
{
}


PrinterConfiguration::PrinterConfiguration(const PrinterConfiguration &other)
	:	d(other.d)
{
}


PrinterConfiguration::~PrinterConfiguration()
{
}


void PrinterConfiguration::clear()
{
	d->clear();
}


void PrinterConfiguration::detach()
{
	d.detach();
}


void PrinterConfiguration::addPage(Page *page)
{
	d->m_pages.append(page);
	updatePageNumbers();
}


void PrinterConfiguration::insertPage(int position, Page *page)
{
	d->m_pages.insert(position, page);
	updatePageNumbers();
}


void PrinterConfiguration::removePage(Page *page)
{
	d->m_pages.takeAt(d->m_pages.indexOf(page));
	updatePageNumbers();
}


QList<Page *> &PrinterConfiguration::pages()
{
	return d->m_pages;
}


QDataStream &operator<<(QDataStream &stream, const PrinterConfiguration &printerConfiguration)
{
	stream << qint32(printerConfiguration.version);

	stream << printerConfiguration.d->m_pages.count();
	QListIterator<Page *> pageIterator(printerConfiguration.d->m_pages);
	while (pageIterator.hasNext())
	{
		Page *page = pageIterator.next();
		stream << *page;
	}

	return stream;
}


QDataStream &operator>>(QDataStream &stream, PrinterConfiguration &printerConfiguration)
{
	qint32 version;
	qint32 pages;
	Page *page;

	stream >> version;
	switch (version)
	{
		case 100:
			stream >> pages;
			while (pages--)
			{
				page = new Page;
				stream >> *page;
				printerConfiguration.addPage(page);
			}
			break;

		default:
			// not supported
			// throw exception
			break;
	}

	return stream;
}


void PrinterConfiguration::updatePageNumbers()
{
	int p = 1;
	QListIterator<Page *> pageIterator(d->m_pages);
	while (pageIterator.hasNext())
	{
		pageIterator.next()->setPageNumber(p++);
	}
}
