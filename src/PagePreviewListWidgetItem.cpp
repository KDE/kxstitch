/*
 * Copyright (C) 2010-2015 by Stephen Allewell
 * steve.allewell@gmail.com
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

#include <KLocalizedString>

#include "Page.h"
#include "PaperSizes.h"


PagePreviewListWidgetItem::PagePreviewListWidgetItem(Document *document, Page *page)
    :   QListWidgetItem(0, QListWidgetItem::UserType),
        m_document(document),
        m_page(page)
{
    generatePreviewIcon();
}


QPageSize PagePreviewListWidgetItem::pageSize() const
{
    return m_page->pageSize();
}


QPageLayout::Orientation PagePreviewListWidgetItem::orientation() const
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


void PagePreviewListWidgetItem::setPageSize(QPageSize pageSize)
{
    m_page->setPageSize(pageSize);
    generatePreviewIcon();
}


void PagePreviewListWidgetItem::setOrientation(QPageLayout::Orientation orientation)
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
    m_paperWidth = PageSizes::width(m_page->pageSize().id(), m_page->orientation());
    m_paperHeight = PageSizes::height(m_page->pageSize().id(), m_page->orientation());

    QPixmap pixmap(m_paperWidth, m_paperHeight);
    pixmap.fill(Qt::white);

    QPainter painter;
    painter.begin(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing, true);
    m_page->render(m_document, &painter);
    painter.end();
    setIcon(pixmap);
}
