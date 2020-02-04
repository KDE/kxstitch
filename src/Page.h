/*
 * Copyright (C) 2010-2015 by Stephen Allewell
 * steve.allewell@gmail.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#ifndef Page_H
#define Page_H


#include <QDataStream>
#include <QList>
#include <QMargins>
#include <QPrinter>


class Document;
class Element;
class QPainter;


class Page : public QPageLayout
{
public:
    explicit Page(QPageSize pageSize = QPageSize(QPageSize::A4), QPageLayout::Orientation orientation = QPageLayout::Portrait);
    Page(const Page &);
    ~Page();

    Page &operator=(const Page &);

    int pageNumber() const;
    const QList<Element *> elements() const;

    void setPageNumber(int);

    void addElement(Element *);
    void removeElement(Element *);

    void render(Document *, QPainter *) const;

    Element *itemAt(const QPoint &) const;

    friend QDataStream &operator<<(QDataStream &, const Page &);
    friend QDataStream &operator>>(QDataStream &, Page &);

    friend class PagePropertiesDlg;

private:
    void readElements(QDataStream &);

    static const int version = 102;

    int     m_pageNumber;

    QList<Element *>  m_elements;
};


#endif // Page_H
