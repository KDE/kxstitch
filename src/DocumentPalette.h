/********************************************************************************
 *	Copyright (C) 2010 by Stephen Allewell					*
 *	stephen@mirramar.adsl24.co.uk						*
 *										*
 *	This program is free software; you can redistribute it and/or modify	*
 *	it under the terms of the GNU General Public License as published by	*
 *	the Free Software Foundation; either version 2 of the License, or	*
 *	(at your option) any later version.					*
 ********************************************************************************/


#ifndef DocumentPalette_H
#define DocumentPalette_H


#include <QDataStream>
#include <QList>
#include <QMap>
#include <QString>
#include <QStringList>

#include "DocumentFloss.h"


class DocumentPalette
{
	public:
		DocumentPalette();
		DocumentPalette(const DocumentPalette &);
		~DocumentPalette();

		void clear();

		const QString &schemeName() const;
		QMap<int, DocumentFloss *> &flosses();
		QVector<int> sortedFlosses() const;
		QList<QChar> usedSymbols() const;
		DocumentFloss *currentFloss() const;
		DocumentFloss *floss(int) const;
		int currentIndex() const;
		bool showSymbols() const;

		void setSchemeName(const QString &);
		void setCurrentIndex(int);
		void add(int, DocumentFloss *);
		int add(const QColor &);
		DocumentFloss *remove(int);
		DocumentFloss *replace(int, DocumentFloss *);
		void swap(int, int);
		void setShowSymbols(bool);

		DocumentPalette &operator=(const DocumentPalette &);
		bool operator==(const DocumentPalette &);
		bool operator!=(const DocumentPalette &);
		
		friend QDataStream &operator<<(QDataStream &, const DocumentPalette &);
		friend QDataStream &operator>>(QDataStream &, DocumentPalette &);

	private:
		static const int version = 100;

		QString				m_schemeName;
		int				m_currentIndex;
		QMap<int, DocumentFloss *>	m_documentFlosses;
		bool				m_showSymbols;
};


QDataStream &operator<<(QDataStream &, const DocumentPalette &);
QDataStream &operator>>(QDataStream &, DocumentPalette &);


#endif // DocumentPalette_H
