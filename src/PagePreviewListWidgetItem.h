/*
 * Copyright (C) 2010-2015 by Stephen Allewell
 * steve.allewell@gmail.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#ifndef PagePreviewListWidgetItem_H
#define PagePreviewListWidgetItem_H


#include <QListWidgetItem>
#include <QPrinter>


class Document;
class Page;


class PagePreviewListWidgetItem : public QListWidgetItem
{
public:
    PagePreviewListWidgetItem(Document *, Page *);
    virtual ~PagePreviewListWidgetItem() = default;

    QPageSize pageSize() const;
    QPageLayout::Orientation orientation() const;
    int paperWidth() const;
    int paperHeight() const;

    void setPageSize(QPageSize);
    void setOrientation(QPageLayout::Orientation);

    Page *page() const;
    void generatePreviewIcon();

private:

    Document    *m_document;
    Page        *m_page;
    int         m_paperWidth;
    int         m_paperHeight;
};


#endif // PagePreviewListWidgetItem_H
