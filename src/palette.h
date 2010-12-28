/****************************************************************************
 *	Copyright (C) 2010 by Stephen Allewell									*
 *	stephen@mirramar.adsl24.co.uk											*
 *																			*
 *	This program is free software; you can redistribute it and/or modify	*
 *	it under the terms of the GNU General Public License as published by	*
 *	the Free Software Foundation; either version 2 of the License, or		*
 *	(at your option) any later version.										*
 ****************************************************************************/


#ifndef PALETTEVIEW_H
#define PALETTEVIEW_H


#include <QFrame>


class Document;


class Palette : public QFrame
{
	Q_OBJECT

	public:
		Palette();
		~Palette();

		virtual QSize sizeHint() const;
		void setDocument(Document *document);

	public slots:
		void loadSettings();

	protected:
		void paintEvent(QPaintEvent *);
		void mousePressEvent(QMouseEvent *);

	private:
		Document *m_document;
		int m_cols;
		int m_rows;
		int m_width;
		int m_height;
		int m_flosses;
		bool m_symbols;
		QVector<int> m_paletteIndex;
};


#endif
