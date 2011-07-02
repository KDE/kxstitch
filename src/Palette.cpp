/********************************************************************************
 *	Copyright (C) 2010 by Stephen Allewell					*
 *	stephen@mirramar.adsl24.co.uk						*
 *										*
 *	This program is free software; you can redistribute it and/or modify	*
 *	it under the terms of the GNU General Public License as published by	*
 *	the Free Software Foundation; either version 2 of the License, or	*
 *	(at your option) any later version.					*
 ********************************************************************************/


#include <QContextMenuEvent>
#include <QMenu>
#include <QMouseEvent>
#include <QPainter>
#include <QToolTip>

#include <KLocale>
#include <KXMLGUIFactory>

#include "configuration.h"
#include "Document.h"
#include "Floss.h"
#include "FlossScheme.h"
#include "MainWindow.h"
#include "Palette.h"
#include "SchemeManager.h"


Palette::Palette(QWidget *parent)
	:	QFrame(parent)
{
	setObjectName("Palette#");
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
	readDocumentSettings();
}


Document *Palette::document() const
{
	return m_document;
}


void Palette::readDocumentSettings()
{
	m_showSymbols = m_document->documentPalette().showSymbols();
}


void Palette::showSymbols(bool show)
{
	m_showSymbols = show;
	update();
}


bool Palette::event(QEvent *event)
{
	if (event->type() == QEvent::ToolTip)
	{
		QHelpEvent *helpEvent = static_cast<QHelpEvent *>(event);
		QPoint local = helpEvent->pos();
		int xCell = local.x()/m_width;
		int yCell = local.y()/m_height;
		int i = yCell*m_cols+xCell;
		if (m_paletteIndex.count())
		{
			if (i < m_paletteIndex.count())
			{
				DocumentFloss *documentFloss = m_document->documentPalette().flosses()[m_paletteIndex[i]];
				FlossScheme *flossScheme = SchemeManager::scheme(m_document->documentPalette().schemeName());
				Floss *floss = flossScheme->find(documentFloss->flossName());
				QString tip = QString("%1 %2").arg(floss->name()).arg(floss->description());
				QToolTip::showText(helpEvent->globalPos(), tip);
			}
			else
			{
				QToolTip::hideText();
				event->ignore();
			}
		}
		else
			QToolTip::showText(helpEvent->globalPos(), QString(i18n("No colors in palette")));

		return true;
	}
	return QWidget::event(event);
}


void Palette::paintEvent(QPaintEvent *)
{
	if (m_document == 0) return;
	QMap<int, DocumentFloss *> palette = m_document->documentPalette().flosses();
	m_flosses = palette.count();
	m_paletteIndex.resize(m_flosses);
	int currentFlossIndex = m_document->documentPalette().currentIndex();

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

		QMapIterator<int, DocumentFloss *> mapIterator(palette);
		int flossIndex = 0;
		while (mapIterator.hasNext())
		{
			mapIterator.next();
			m_paletteIndex[flossIndex] = mapIterator.key();
			QColor color = mapIterator.value()->flossColor();
			int x = (flossIndex%m_cols)*m_width;
			int y = (flossIndex/m_cols)*m_height;
			QRect rect(x, y, m_width, m_height);

			painter.fillRect(rect, color);

			if (currentFlossIndex == -1)
			{
				m_document->documentPalette().setCurrentIndex(mapIterator.key());
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

			if (m_showSymbols)
			{
				if (qGray(color.rgb()) < 128)
					painter.setPen(Qt::white);
				else
					painter.setPen(Qt::black);
				painter.drawText(rect, Qt::AlignCenter, mapIterator.value()->stitchSymbol());
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


void Palette::contextMenuEvent(QContextMenuEvent *event)
{
	MainWindow *mainwindow = qobject_cast<MainWindow *>(topLevelWidget());
	QMenu *context = static_cast<QMenu *>(mainwindow->guiFactory()->container("PalettePopup", mainwindow));
	context->popup(event->globalPos());
	event->accept();
}


void Palette::mousePressEvent(QMouseEvent *event)
{
	if ((event->button() & Qt::LeftButton) && m_flosses)
	{
		QPoint p = event->pos();
		int x = (p.x()/m_width);
		int y = (p.y()/m_height);
		int i = y*m_cols+x;
		if (i < m_flosses)
		{
			m_document->documentPalette().setCurrentIndex(m_paletteIndex[i]);
			emit colorSelected(i);
		}
		repaint();
	}
}
