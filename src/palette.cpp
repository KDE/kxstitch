/****************************************************************************
 *	Copyright (C) 2010 by Stephen Allewell									*
 *	stephen@mirramar.adsl24.co.uk											*
 *																			*
 *	This program is free software; you can redistribute it and/or modify	*
 *	it under the terms of the GNU General Public License as published by	*
 *	the Free Software Foundation; either version 2 of the License, or		*
 *	(at your option) any later version.										*
 ****************************************************************************/


#include <QMouseEvent>
#include <QPainter>

#include <KDebug>

#include "configuration.h"
#include "document.h"
#include "floss.h"
#include "mainwindow.h"
#include "palette.h"


Palette::Palette() : QFrame(),
	m_document(0)
{
	setObjectName("Palette#");
	loadSettings();
}


Palette::~Palette()
{
}


QSize Palette::sizeHint() const
{
	return QSize(400, 600);
}


void Palette::setDocument(Document *document)
{
	m_document = document;
}


void Palette::loadSettings()
{
}


void Palette::paintEvent(QPaintEvent *)
{
	if (m_document == 0) return;
	QMap<int, Document::FLOSS> &palette = m_document->palette();
	m_flosses = palette.count();
	m_paletteIndex.resize(m_flosses);
	int currentFlossIndex = m_document->currentFlossIndex();
  
	QPainter painter;

	if (m_flosses)
	{
		static_cast<MainWindow *>(topLevelWidget())->slotStateChanged("palette_empty", KXMLGUIClient::StateReverse);

		m_cols = 5;
		while (true)
		{
			while ((m_width = contentsRect().width()/m_cols) > 40)
				m_cols++;
			m_rows = m_flosses/m_cols;
			if (m_flosses%m_cols)
				m_rows++;
			m_height = std::min(contentsRect().height()/m_rows, m_width);
			if ((m_width-m_height) > m_height)
				m_cols++;
			else
				break;
		}

		painter.begin(this);
		QFont font = painter.font();
		font.setFamily(Configuration::editor_SymbolFont());
		font.setPixelSize(std::min(m_width, m_height));
		painter.setFont(font);

		QMapIterator<int, Document::FLOSS> mapIterator(palette);
		int flossIndex = 0;
		while (mapIterator.hasNext())
		{
			mapIterator.next();
			m_paletteIndex[flossIndex] = mapIterator.key();
			Floss *floss = mapIterator.value().floss;
			int x = (flossIndex%m_cols)*m_width;
			int y = (flossIndex/m_cols)*m_height;
			QRect rect(x, y, m_width, m_height);

			painter.fillRect(rect, floss->color);

			if (currentFlossIndex == -1)
			{
				m_document->setCurrentFlossIndex(mapIterator.key());
				currentFlossIndex = mapIterator.key();
			}

			if (mapIterator.key() == currentFlossIndex)
			{
				painter.setPen(Qt::black);
				painter.drawLine(rect.topLeft(), rect.topRight());
				painter.drawLine(rect.topLeft(), rect.bottomLeft());
				painter.setPen(Qt::white);
				painter.drawLine(rect.topRight(), rect.bottomRight());
				painter.drawLine(rect.bottomLeft(), rect.bottomRight());
			}
			else
			{
				painter.setPen(Qt::black);
				painter.drawLine(rect.topRight(), rect.bottomRight());
				painter.drawLine(rect.bottomLeft(), rect.bottomRight());
				painter.setPen(Qt::white);
				painter.drawLine(rect.topLeft(), rect.topRight());
				painter.drawLine(rect.topLeft(), rect.bottomLeft());
			}

			if (m_document->property("showPaletteSymbols").toBool())
			{
				if (qGray(floss->color.rgb()) < 128)
					painter.setPen(Qt::white);
				else
					painter.setPen(Qt::black);
				painter.drawText(rect, Qt::AlignCenter, mapIterator.value().symbol);
			}

		flossIndex++;
		}

		painter.end();
	}
	else
	{
		static_cast<MainWindow *>(topLevelWidget())->slotStateChanged("palette_empty");
	}
}


void Palette::mousePressEvent(QMouseEvent *event)
{
	if (event->button() & Qt::LeftButton)
	{
		QPoint p = event->pos();
		int x = (p.x()/m_width);
		int y = (p.y()/m_height);
		int i = y*m_cols+x;
		if (i < m_flosses)
			m_document->setCurrentFlossIndex(m_paletteIndex[i]);
		repaint();
	}
}
