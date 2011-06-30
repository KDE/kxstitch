/********************************************************************************
 *	Copyright (C) 2010 by Stephen Allewell					*
 *	stephen@mirramar.fsnet.co.uk						*
 *										*
 *	This program is free software; you can redistribute it and/or modify	*
 *	it under the terms of the GNU General Public License as published by	*
 *	the Free Software Foundation; either version 2 of the License, or	*
 *	(at your option) any later version.					*
 ********************************************************************************/


#ifndef __DocumentFloss_H
#define __DocumentFloss_H


#include <QChar>
#include <QColor>
#include <QDataStream>
#include <QString>


class DocumentFloss
{
	public:
		DocumentFloss();
		DocumentFloss(const QString &, const QChar &, Qt::PenStyle, int, int);
		DocumentFloss(DocumentFloss *);

		QString flossName() const;
		QColor flossColor() const;
		QChar stitchSymbol() const;
		Qt::PenStyle backstitchSymbol() const;
		int stitchStrands() const;
		int backstitchStrands() const;

		void setFlossName(const QString &);
		void setFlossColor(const QColor &);
		void setStitchSymbol(const QChar &);
		void setBackstitchSymbol(Qt::PenStyle);
		void setStitchStrands(int);
		void setBackstitchStrands(int);

		bool operator==(const DocumentFloss &);
		bool operator!=(const DocumentFloss &);

		friend QDataStream &operator<<(QDataStream &, const DocumentFloss &);
		friend QDataStream &operator>>(QDataStream &, DocumentFloss &);

	private:
		static const int version = 100;

		QString		m_flossName;
		QColor		m_flossColor;
		QChar		m_stitchSymbol;
		Qt::PenStyle	m_backstitchSymbol;
		int		m_stitchStrands;
		int		m_backstitchStrands;
};


QDataStream &operator<<(QDataStream &, DocumentFloss *);
QDataStream &operator>>(QDataStream &, DocumentFloss *);


#endif
