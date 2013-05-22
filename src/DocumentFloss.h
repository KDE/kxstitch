/*
 * Copyright (C) 2010 by Stephen Allewell
 * sallewell@users.sourceforge.net
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#ifndef DocumentFloss_H
#define DocumentFloss_H


#include <QChar>
#include <QColor>
#include <QDataStream>
#include <QString>


class DocumentFloss
{
public:
    DocumentFloss();
    DocumentFloss(const QString &, qint16, Qt::PenStyle, int, int);
    explicit DocumentFloss(const DocumentFloss *);

    QString flossName() const;
    QColor flossColor() const;
    qint16 stitchSymbol() const;
    Qt::PenStyle backstitchSymbol() const;
    int stitchStrands() const;
    int backstitchStrands() const;

    void setFlossName(const QString &);
    void setFlossColor(const QColor &);
    void setStitchSymbol(qint16);
    void setBackstitchSymbol(Qt::PenStyle);
    void setStitchStrands(int);
    void setBackstitchStrands(int);

    bool operator==(const DocumentFloss &) const;
    bool operator!=(const DocumentFloss &) const;

    friend QDataStream &operator<<(QDataStream &, const DocumentFloss &);
    friend QDataStream &operator>>(QDataStream &, DocumentFloss &);

private:
    static const int version = 101;

    QString         m_flossName;
    QColor          m_flossColor;
    qint16          m_stitchSymbol;
    Qt::PenStyle    m_backstitchSymbol;
    int             m_stitchStrands;
    int             m_backstitchStrands;
};


QDataStream &operator<<(QDataStream &, const DocumentFloss &);
QDataStream &operator>>(QDataStream &, DocumentFloss &);


#endif // DocumentFloss_H
