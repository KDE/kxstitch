/********************************************************************************
 *	Copyright (C) 2010 by Stephen Allewell					*
 *	stephen@mirramar.adsl24.co.uk						*
 *										*
 *	This program is free software; you can redistribute it and/or modify	*
 *	it under the terms of the GNU General Public License as published by	*
 *	the Free Software Foundation; either version 2 of the License, or	*
 *	(at your option) any later version.					*
 ********************************************************************************/


#ifndef __PatternMimeData_H
#define __PatternMimeData_H


#include <QMimeData>
#include <QString>
#include <QStringList>
#include <QVariant>


class PatternMimeData : public QMimeData
{
	Q_OBJECT

	public:
		PatternMimeData();
		~PatternMimeData();

		virtual QStringList formats() const;
		virtual bool hasFormat(const QString &) const;
		virtual QVariant retrieveData(const QString &, QVariant::Type) const;

		void setScale(int, int);

	private:
		int		m_scaleWidth;
		int		m_scaleHeight;
		QStringList	m_outputFormats;
};


#endif
