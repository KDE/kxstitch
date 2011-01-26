/****************************************************************************
 *	Copyright (C) 2010 by Stephen Allewell									*
 *	stephen@mirramar.fsnet.co.uk											*
 *																			*
 *	This program is free software; you can redistribute it and/or modify	*
 *	it under the terms of the GNU General Public License as published by	*
 *	the Free Software Foundation; either version 2 of the License, or		*
 *	(at your option) any later version.										*
 ****************************************************************************/


#ifndef FLOSS_H
#define FLOSS_H


#include <QColor>
#include <QString>
#include <QVariant>


class Floss
{
	public:
		Floss(const QString &, const QString &, const QColor &);
		~Floss();

		const QString &name() const;
		void setName(const QString &);
		const QString &description() const;
		void setDescription(const QString &);
		const QColor &color() const;
		void setColor(const QColor &);

	private:
		QString	m_name;
		QString	m_description;
		QColor	m_color;
};


class DocumentFloss
{
	public:
		DocumentFloss(Floss *, QChar, unsigned, unsigned);
		DocumentFloss(DocumentFloss *);
		~DocumentFloss();

		const Floss *floss() const;
		void setFloss(Floss *);
		const QChar symbol() const;
		void setSymbol(QChar);
		unsigned stitchStrands() const;
		void setStitchStrands(unsigned);
		unsigned backstitchStrands() const;
		void setBackstitchStrands(unsigned);

		bool operator==(const DocumentFloss &);
		bool operator!=(const DocumentFloss &);

	private:
		Floss		*m_floss;
		QChar		m_symbol;
		unsigned	m_stitchStrands;
		unsigned	m_backstitchStrands;
};


#endif
