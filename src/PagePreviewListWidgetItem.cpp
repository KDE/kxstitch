/*
 * Copyright (C) 2010 by Stephen Allewell
 * sallewell@users.sourceforge.net
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#include "PagePreviewListWidgetItem.h"

#include <algorithm>

#include <QPainter>
#include <QPixmap>

#include <KDebug>
#include <KLocale>

#include "Page.h"
#include "PaperSizes.h"


PagePreviewListWidgetItem::PagePreviewListWidgetItem(Document *document, Page *page)
    :   QListWidgetItem(0, QListWidgetItem::UserType),
        m_document(document),
        m_page(page)
{
    generatePreviewIcon();
}


PagePreviewListWidgetItem::~PagePreviewListWidgetItem()
{
}


QPrinter::PaperSize PagePreviewListWidgetItem::paperSize() const
{
    return m_page->paperSize();
}


QPrinter::Orientation PagePreviewListWidgetItem::orientation() const
{
    return m_page->orientation();
}


int PagePreviewListWidgetItem::paperWidth() const
{
    return m_paperWidth;
}


int PagePreviewListWidgetItem::paperHeight() const
{
    return m_paperHeight;
}


void PagePreviewListWidgetItem::setPaperSize(QPrinter::PaperSize paperSize)
{
    m_page->setPaperSize(paperSize);
    generatePreviewIcon();
}


void PagePreviewListWidgetItem::setOrientation(QPrinter::Orientation orientation)
{
    m_page->setOrientation(orientation);
    generatePreviewIcon();
}


Page *PagePreviewListWidgetItem::page() const
{
    return m_page;
}


void PagePreviewListWidgetItem::generatePreviewIcon()
{
    m_paperWidth = PaperSizes::width(m_page->paperSize(), m_page->orientation());
    m_paperHeight = PaperSizes::height(m_page->paperSize(), m_page->orientation());

    QPixmap pixmap(m_paperWidth, m_paperHeight);
    pixmap.fill(Qt::white);

    QPainter painter;
    painter.begin(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing, true);
    m_page->render(m_document, &painter);
    painter.end();
    setIcon(pixmap);
}
