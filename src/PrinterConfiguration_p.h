/********************************************************************************
 *	Copyright (C) 2010 by Stephen Allewell					*
 *	stephen@mirramar.adsl24.co.uk						*
 *										*
 *	This program is free software; you can redistribute it and/or modify	*
 *	it under the terms of the GNU General Public License as published by	*
 *	the Free Software Foundation; either version 2 of the License, or	*
 *	(at your option) any later version.					*
 ********************************************************************************/


#ifndef PrinterConfigurationPrivate_H
#define PrinterConfigurationPrivate_H


#include <QSharedData>


class PrinterConfigurationPrivate : public QSharedData
{
	public:
		PrinterConfigurationPrivate();
		PrinterConfigurationPrivate(const PrinterConfigurationPrivate &);
		~PrinterConfigurationPrivate();

		void clear();

//		friend class PrinterConfiguration;

		QList<Page *>	m_pages;
};


#endif // PrinterConfigurationPrivate_H
