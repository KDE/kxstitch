/*
 * Copyright (C) 2010 by Stephen Allewell
 * sallewell@users.sourceforge.net
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#include "Palette.h"

#include <QBitmap>
#include <QMouseEvent>
#include <QPainter>
#include <QToolTip>

#include <KLocale>
#include <KXMLGUIClient>

#include "configuration.h"
#include "Document.h"
#include "Floss.h"
#include "FlossScheme.h"
#include "SchemeManager.h"
#include "SymbolLibrary.h"
#include "SymbolManager.h"


const uchar swapCursor[] = {
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x08, 0x00,
    0x00, 0x00, 0x18, 0x00,
    0x00, 0x00, 0x3f, 0x80,
    0x00, 0x00, 0x7f, 0xc0,
    0x3f, 0xff, 0x3f, 0xe0,
    0x3f, 0xff, 0x19, 0xe0,
    0x3f, 0xff, 0x08, 0xe0,
    0x3f, 0xff, 0x00, 0xe0,
    0x3f, 0xff, 0x00, 0xe0,
    0x3f, 0xff, 0x03, 0xf8,
    0x3f, 0xff, 0x01, 0xf0,
    0x3f, 0xff, 0x00, 0xe0,
    0x3f, 0xff, 0x00, 0x40,
    0x3f, 0xff, 0x00, 0x00,
    0x3f, 0xff, 0xff, 0x80,
    0x3f, 0xff, 0x00, 0x80,
    0x3f, 0xff, 0x00, 0x80,
    0x3f, 0xff, 0x00, 0x80,
    0x00, 0x10, 0x00, 0x80,
    0x00, 0x10, 0x00, 0x80,
    0x00, 0x10, 0x00, 0x80,
    0x00, 0x10, 0x00, 0x80,
    0x00, 0x10, 0x00, 0x80,
    0x00, 0x10, 0x00, 0x80,
    0x00, 0x10, 0x00, 0x80,
    0x00, 0x10, 0x00, 0x80,
    0x00, 0x10, 0x00, 0x80,
    0x00, 0x1f, 0xff, 0x80,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00
};


const uchar swapCursorMask[] = {
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x1c, 0x00,
    0x00, 0x00, 0x3c, 0x00,
    0x00, 0x00, 0x7f, 0xc0,
    0x00, 0x00, 0xff, 0xe0,
    0x7f, 0xff, 0xff, 0xf0,
    0x7f, 0xff, 0xff, 0xf0,
    0x7f, 0xff, 0xff, 0xf0,
    0x7f, 0xff, 0xbf, 0xf0,
    0x7f, 0xff, 0x9d, 0xf0,
    0x7f, 0xff, 0x87, 0xfc,
    0x7f, 0xff, 0x87, 0xfc,
    0x7f, 0xff, 0x87, 0xfc,
    0x7f, 0xff, 0x83, 0xf8,
    0x7f, 0xff, 0x81, 0xf0,
    0x7f, 0xff, 0xff, 0xe0,
    0x7f, 0xff, 0xff, 0xc0,
    0x7f, 0xff, 0xff, 0xc0,
    0x7f, 0xff, 0xff, 0xc0,
    0x7f, 0xff, 0xff, 0xc0,
    0x7f, 0xff, 0xff, 0xc0,
    0x00, 0x3f, 0xff, 0xc0,
    0x00, 0x3f, 0xff, 0xc0,
    0x00, 0x3f, 0xff, 0xc0,
    0x00, 0x3f, 0xff, 0xc0,
    0x00, 0x3f, 0xff, 0xc0,
    0x00, 0x3f, 0xff, 0xc0,
    0x00, 0x3f, 0xff, 0xc0,
    0x00, 0x3f, 0xff, 0xc0,
    0x00, 0x3f, 0xff, 0xc0,
    0x00, 0x3f, 0xff, 0xc0,
    0x00, 0x00, 0x00, 0x00
};


const uchar replaceCursor[] = {
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x08, 0x00,
    0x00, 0x00, 0x18, 0x00,
    0x00, 0x00, 0x3f, 0x80,
    0x00, 0x00, 0x7f, 0xc0,
    0x3f, 0xff, 0x3f, 0xe0,
    0x3f, 0xff, 0x19, 0xe0,
    0x3f, 0xff, 0x08, 0xe0,
    0x3f, 0xff, 0x00, 0xe0,
    0x3f, 0xff, 0x00, 0xe0,
    0x3f, 0xff, 0x00, 0xe0,
    0x3f, 0xff, 0x00, 0xe0,
    0x3f, 0xff, 0x00, 0xe0,
    0x3f, 0xff, 0x00, 0xe0,
    0x3f, 0xff, 0x00, 0x00,
    0x3f, 0xff, 0xff, 0x80,
    0x3f, 0xff, 0x00, 0x80,
    0x3f, 0xff, 0x00, 0x80,
    0x3f, 0xff, 0x00, 0x80,
    0x00, 0x10, 0x00, 0x80,
    0x00, 0x10, 0x00, 0x80,
    0x00, 0x10, 0x00, 0x80,
    0x00, 0x10, 0x00, 0x80,
    0x00, 0x10, 0x00, 0x80,
    0x00, 0x10, 0x00, 0x80,
    0x00, 0x10, 0x00, 0x80,
    0x00, 0x10, 0x00, 0x80,
    0x00, 0x10, 0x00, 0x80,
    0x00, 0x1f, 0xff, 0x80,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00
};


const uchar replaceCursorMask[] = {
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x1c, 0x00,
    0x00, 0x00, 0x3c, 0x00,
    0x00, 0x00, 0x7f, 0xc0,
    0x00, 0x00, 0xff, 0xe0,
    0x7f, 0xff, 0xff, 0xf0,
    0x7f, 0xff, 0xff, 0xf0,
    0x7f, 0xff, 0xff, 0xf0,
    0x7f, 0xff, 0xbf, 0xf0,
    0x7f, 0xff, 0x9d, 0xf0,
    0x7f, 0xff, 0x81, 0xf0,
    0x7f, 0xff, 0x81, 0xf0,
    0x7f, 0xff, 0x81, 0xf0,
    0x7f, 0xff, 0x81, 0xf0,
    0x7f, 0xff, 0x81, 0xf0,
    0x7f, 0xff, 0xff, 0xe0,
    0x7f, 0xff, 0xff, 0xc0,
    0x7f, 0xff, 0xff, 0xc0,
    0x7f, 0xff, 0xff, 0xc0,
    0x7f, 0xff, 0xff, 0xc0,
    0x7f, 0xff, 0xff, 0xc0,
    0x00, 0x3f, 0xff, 0xc0,
    0x00, 0x3f, 0xff, 0xc0,
    0x00, 0x3f, 0xff, 0xc0,
    0x00, 0x3f, 0xff, 0xc0,
    0x00, 0x3f, 0xff, 0xc0,
    0x00, 0x3f, 0xff, 0xc0,
    0x00, 0x3f, 0xff, 0xc0,
    0x00, 0x3f, 0xff, 0xc0,
    0x00, 0x3f, 0xff, 0xc0,
    0x00, 0x3f, 0xff, 0xc0,
    0x00, 0x00, 0x00, 0x00
};


Palette::Palette(QWidget *parent)
    :   QFrame(parent),
        m_document(0),
        m_showSymbols(Configuration::palette_ShowSymbols()),
        m_cols(0),
        m_rows(0),
        m_width(0),
        m_height(0),
        m_flosses(0),
        m_mode(Select)
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
}


Document *Palette::document() const
{
    return m_document;
}


void Palette::showSymbols(bool show)
{
    m_showSymbols = show;
    update();
}


void Palette::swapColors()
{
    if (m_paletteIndex.count() > 1) { // can't swap if there is less than two colors
        m_mode = Swap;
        setCursor(QCursor(QBitmap().fromData(QSize(32, 32), swapCursor, QImage::Format_Mono), QBitmap().fromData(QSize(32, 32), swapCursorMask, QImage::Format_Mono)));
    }
}


void Palette::replaceColor()
{
    if (m_paletteIndex.count() > 1) { // can't replace if there is less than two colors
        m_mode = Replace;
        setCursor(QCursor(QBitmap().fromData(QSize(32, 32), replaceCursor, QImage::Format_Mono), QBitmap().fromData(QSize(32, 32), replaceCursorMask, QImage::Format_Mono)));
    }
}


void Palette::loadSettings()
{
    update();
}


bool Palette::event(QEvent *event)
{
    if (event->type() == QEvent::ToolTip) {
        QHelpEvent *helpEvent = static_cast<QHelpEvent *>(event);

        if (m_paletteIndex.count()) {
            QPoint local = helpEvent->pos();
            int xCell = local.x() / m_width;
            int yCell = local.y() / m_height;
            int i = yCell * m_cols + xCell;

            if (i < m_paletteIndex.count()) {
                DocumentFloss *documentFloss = m_document->pattern()->palette().flosses()[m_paletteIndex[i]];
                FlossScheme *flossScheme = SchemeManager::scheme(m_document->pattern()->palette().schemeName());
                Floss *floss = flossScheme->find(documentFloss->flossName());
                FlossUsage flossUsage = m_document->pattern()->stitches().flossUsage()[m_paletteIndex[i]];
                QString tip = i18ncp("%1 is the number of stitches of a particular floss, %2 is the floss name and %3 the floss description", "%2 %3\n%1 Stitch", "%2 %3\n%1 Stitches", flossUsage.stitchCount() + flossUsage.backstitchCount, floss->name(), floss->description());
                QToolTip::showText(helpEvent->globalPos(), tip);
            } else {
                QToolTip::hideText();
                event->ignore();
            }
        } else {
            QToolTip::showText(helpEvent->globalPos(), i18n("There are no flosses in the palette.\nAdd them with the Palette Manager\nwhich will then enable the drawing tools."));
        }

        return true;
    }

    return QWidget::event(event);
}


void Palette::paintEvent(QPaintEvent *)
{
    if (m_document == 0) {
        return;
    }

    QMap<int, DocumentFloss *> palette = m_document->pattern()->palette().flosses();
    m_flosses = palette.count();
    m_paletteIndex = m_document->pattern()->palette().sortedFlosses();

    int currentFlossIndex = m_document->pattern()->palette().currentIndex();

    SymbolLibrary *library = SymbolManager::library(m_document->pattern()->palette().symbolLibrary());

    QPainter painter;

    if (m_flosses) {
        emit signalStateChanged("palette_empty", KXMLGUIClient::StateReverse);

        m_cols = 5;

        while (true) {
            while ((m_width = contentsRect().width() / m_cols) > 40) {
                m_cols++;
            }

            m_rows = m_flosses / m_cols;

            if (m_flosses % m_cols) {
                m_rows++;
            }

            m_height = std::min(contentsRect().height() / m_rows, m_width);

            if ((m_width - m_height) > m_height) {
                m_cols++;
            } else {
                break;
            }
        }

        QTransform scale = QTransform::fromScale(m_width, m_height);

        painter.begin(this);
        painter.setRenderHint(QPainter::Antialiasing, Configuration::palette_SymbolsAntialiased());

        for (int flossIndex = 0 ; flossIndex < m_flosses ; flossIndex++) {
            QColor color = palette[m_paletteIndex[flossIndex]]->flossColor();
            int x = (flossIndex % m_cols) * m_width;
            int y = (flossIndex / m_cols) * m_height;
            QRect rect(x, y, m_width, m_height);

            painter.fillRect(rect, color);

            if (currentFlossIndex == -1) {
                m_document->pattern()->palette().setCurrentIndex(m_paletteIndex[flossIndex]);
                currentFlossIndex = m_paletteIndex[flossIndex];
            }

            if (m_paletteIndex[flossIndex] == currentFlossIndex) {
                painter.setPen(Qt::black);
                painter.drawLine(rect.topLeft(), rect.topRight());
                painter.drawLine(rect.topLeft(), rect.bottomLeft());
                painter.setPen(Qt::white);
                painter.drawLine(rect.topRight(), rect.bottomRight());
                painter.drawLine(rect.bottomLeft(), rect.bottomRight());
            } else {
                painter.setPen(Qt::black);
                painter.drawLine(rect.topRight(), rect.bottomRight());
                painter.drawLine(rect.bottomLeft(), rect.bottomRight());
                painter.setPen(Qt::white);
                painter.drawLine(rect.topLeft(), rect.topRight());
                painter.drawLine(rect.topLeft(), rect.bottomLeft());
            }

            if (m_showSymbols) {
                QTransform transform = scale * QTransform::fromTranslate(x, y);
                painter.setTransform(transform);

                Symbol symbol = library->symbol(palette[m_paletteIndex[flossIndex]]->stitchSymbol());
                QPen pen = symbol.pen();
                QBrush brush = symbol.brush();

                if (qGray(color.rgb()) < 128) {
                    pen.setColor(Qt::white);
                    brush.setColor(Qt::white);
                }

                painter.setPen(pen);
                painter.setBrush(brush);
                painter.drawPath(symbol.path(Stitch::BTHalf));
                painter.setTransform(QTransform());
            }
        }

        painter.end();
    } else {
        emit signalStateChanged("palette_empty", KXMLGUIClient::StateNoReverse);
    }
}


void Palette::mousePressEvent(QMouseEvent *event)
{
    if ((event->button() & Qt::LeftButton) && m_flosses) {
        QPoint p = event->pos();
        int x = (p.x() / m_width);
        int y = (p.y() / m_height);
        int i = y * m_cols + x;
        int selectedIndex = m_paletteIndex[i];

        if (i < m_flosses) {
            if (m_mode == Swap) {
                int currentIndex = m_document->pattern()->palette().currentIndex();
                emit(swapColors(currentIndex, selectedIndex));
                setCursor(Qt::ArrowCursor);
                m_mode = Select;
                selectedIndex = currentIndex;
            }

            if (m_mode == Replace) {
                emit(replaceColor(m_document->pattern()->palette().currentIndex(), selectedIndex));
                setCursor(Qt::ArrowCursor);
                m_mode = Select;
            }

            if (m_mode == Select) {
                m_document->pattern()->palette().setCurrentIndex(selectedIndex);
                emit colorSelected(i);
            }
        }

        update();
    }
}
