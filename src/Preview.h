/********************************************************************************
 *	Copyright (C) 2010 by Stephen Allewell					*
 *	stephen@mirramar.adsl24.co.uk						*
 *										*
 *	This program is free software; you can redistribute it and/or modify	*
 *	it under the terms of the GNU General Public License as published by	*
 *	the Free Software Foundation; either version 2 of the License, or	*
 *	(at your option) any later version.					*
 ********************************************************************************/


#ifndef __Preview_H
#define __Preview_H


#include <QWidget>


class Renderer;


class Preview : public QWidget
{
	Q_OBJECT

	public:
		Preview(QWidget *);
		~Preview();

		void setDocument(Document *);
		Document *document();

		void readDocumentSettings();

	public slots:
		void setVisibleCells(const QRect &);

	signals:
		void clicked(QPoint);

	protected:
		virtual void mousePressEvent(QMouseEvent *);
		virtual void mouseMoveEvent(QMouseEvent *);
		virtual void mouseReleaseEvent(QMouseEvent *);
		virtual void paintEvent(QPaintEvent *);

		Document	*m_document;
		Renderer	*m_renderer;
		QRect		m_visible;
		QPoint		m_point;
		bool		m_moved;
		int		m_cellWidth;
		int		m_cellHeight;
		int		m_previewWidth;
		int		m_previewHeight;
};


#endif
