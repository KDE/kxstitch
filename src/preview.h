/****************************************************************************
 *	Copyright (C) 2010 by Stephen Allewell									*
 *	stephen@mirramar.adsl24.co.uk											*
 *																			*
 *	This program is free software; you can redistribute it and/or modify	*
 *	it under the terms of the GNU General Public License as published by	*
 *	the Free Software Foundation; either version 2 of the License, or		*
 *	(at your option) any later version.										*
 ****************************************************************************/


#ifndef PREVIEW_H
#define PREVIEW_H


#include <QScrollArea>
#include <QPoint>


class Document;


class Preview : public QScrollArea
{
	Q_OBJECT

	public:
		Preview();
		~Preview();

		virtual QSize sizeHint() const;
		void  setDocument(Document *document);

	public slots:
		void  loadSettings();
		void  setVisibleArea(QRect visibleArea);

	signals:
		void  clicked(QPoint point);

	protected:
		virtual void contentsMousePressEvent(QMouseEvent *e);
		virtual void contentsMouseMoveEvent(QMouseEvent *e);
		virtual void contentsMouseReleaseEvent(QMouseEvent *e);
		virtual void paintEvent(QPaintEvent *e);

	private:
		Document  *m_document;
		QRect     m_visibleArea;
};


#endif
