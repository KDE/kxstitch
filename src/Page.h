/*
 * Copyright (C) 2010 by Stephen Allewell
 * stephen@mirramar.adsl24.co.uk
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


class Page
{
public:
    Page(QPrinter::PaperSize paperSize = QPrinter::A4, QPrinter::Orientation orientation = QPrinter::Portrait);
    Page(const Page &);
    ~Page();

    Page &operator=(const Page &);

    int pageNumber() const;
    QPrinter::PaperSize paperSize() const;
    QPrinter::Orientation orientation() const;
    const QMargins &margins() const;
    const QList<Element *> elements() const;

    void setPageNumber(int);
    void setPaperSize(QPrinter::PaperSize);
    void setOrientation(QPrinter::Orientation);
    void setMargins(const QMargins &);

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

    QPrinter::PaperSize     m_paperSize;
    QPrinter::Orientation   m_orientation;

    QMargins    m_margins;

    QList<Element *>  m_elements;
};


#endif // Page_H
