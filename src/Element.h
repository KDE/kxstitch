/*
 * Copyright (C) 2010-2014 by Stephen Allewell
 * steve.allewell@gmail.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#ifndef Element_H
#define Element_H


#include <QMargins>
#include <QRect>

#include "configuration.h"
#include "Renderer.h"


class QPainter;

class Document;
class Page;
class Renderer;


class Element
{
public:
    enum Type {Text, Pattern, Plan, Key, Image};

    Element(Page *, const QRect &, Element::Type);
    Element(const Element &);
    virtual ~Element();

    virtual Element *clone() const = 0;
    virtual void render(Document *, QPainter *) const = 0;

    Page *parent() const;
    Element::Type type() const;
    const QRect &rectangle() const;

    void setParent(Page *);
    void setRectangle(const QRect &);
    void move(const QPoint &);

    friend QDataStream &operator<<(QDataStream &, const Element &);
    friend QDataStream &operator>>(QDataStream &, Element &);

protected:
    virtual QDataStream &streamOut(QDataStream &) const = 0;
    virtual QDataStream &streamIn(QDataStream &) = 0;

    static const int version = 101; // removed m_visible

    Page            *m_parent;
    QRect           m_rectangle; // area to be used on the page, in mm, top, left, width, height
    Element::Type   m_type;
};


class KeyElement : public Element
{
public:
    KeyElement(Page *, const QRect &, Element::Type type = Element::Key);
    KeyElement(const KeyElement &);
    virtual ~KeyElement();

    bool showBorder() const;
    QColor borderColor() const;
    int borderThickness() const;
    bool fillBackground() const;
    QColor backgroundColor() const;
    int backgroundTransparency() const;
    QMargins margins() const;
    QColor textColor() const;
    QFont textFont() const;
    int indexStart() const;
    int indexCount() const;
    bool symbolColoumn() const;
    bool flossNameColumn() const;
    bool strandsColumn() const;
    bool flossDescriptionColumn() const;
    bool stitchesColumn() const;
    bool lengthColumn() const;
    bool skeinsColumn() const;

    void setShowBorder(bool);
    void setBorderColor(const QColor &);
    void setBorderThickness(int);
    void setFillBackground(bool);
    void setBackgroundColor(const QColor &);
    void setBackgroundTransparency(int);
    void setMargins(const QMargins &);
    void setTextColor(const QColor &);
    void setTextFont(const QFont &);
    void setIndexStart(int);
    void setIndexCount(int);
    void setSymbolColoumn(bool);
    void setFlossNameColumn(bool);
    void setStrandsColumn(bool);
    void setFlossDescriptionColumn(bool);
    void setStitchesColumn(bool);
    void setLengthColumn(bool);
    void setSkeinsColumn(bool);

    virtual KeyElement *clone() const;
    virtual void render(Document *, QPainter *painter) const;

    friend class KeyElementDlg;

protected:
    virtual QDataStream &streamOut(QDataStream &) const;
    virtual QDataStream &streamIn(QDataStream &);

private:
    static const int version = 102; // added m_indexStart and m_indexCount

    bool        m_showBorder;
    QColor      m_borderColor;
    int         m_borderThickness;
    bool        m_fillBackground;
    QColor      m_backgroundColor;
    int         m_backgroundTransparency;
    QMargins    m_margins;
    QColor      m_textColor;
    QFont       m_textFont;
    int         m_indexStart;
    int         m_indexCount;
    bool        m_symbolColumn;
    bool        m_flossNameColumn;
    bool        m_strandsColumn;
    bool        m_flossDescriptionColumn;
    bool        m_stitchesColumn;
    bool        m_lengthColumn;
    bool        m_skeinsColumn;
};


class PlanElement : public Element
{
public:
    PlanElement(Page *, const QRect &, Element::Type type = Element::Plan);
    PlanElement(const PlanElement &);
    virtual ~PlanElement();

    QRect patternRect() const;
    void setPatternRect(const QRect &);

    virtual PlanElement *clone() const;
    virtual void render(Document *, QPainter *painter) const;

protected:
    virtual QDataStream &streamOut(QDataStream &) const;
    virtual QDataStream &streamIn(QDataStream &);

private:
    static const int version = 100;

    QRect   m_patternRect;
};


class PatternElement : public Element
{
public:
    PatternElement(Page *, const QRect &, Element::Type type = Element::Pattern);
    PatternElement(const PatternElement &);
    virtual ~PatternElement();

    virtual PatternElement *clone() const;
    virtual void render(Document *, QPainter *painter) const;

    bool showBorder() const;
    QColor borderColor() const;
    int borderThickness() const;
    QRect patternRect() const;
    bool showScales() const;
    bool showPlan() const;
    Element *planElement() const;
    Configuration::EnumEditor_FormatScalesAs::type formatScalesAs() const;
    Configuration::EnumRenderer_RenderStitchesAs::type renderStitchesAs() const;
    Configuration::EnumRenderer_RenderBackstitchesAs::type renderBackstitchesAs() const;
    Configuration::EnumRenderer_RenderKnotsAs::type renderKnotsAs() const;
    bool showGrid() const;
    bool showStitches() const;
    bool showBackstitches() const;
    bool showKnots() const;

    void setShowBorder(bool);
    void setBorderColor(const QColor &);
    void setBorderThickness(int);
    void setPatternRect(const QRect &);
    void setShowScales(bool);
    void setShowPlan(bool);
    void setFormatScalesAs(Configuration::EnumEditor_FormatScalesAs::type);
    void setRenderStitchesAs(Configuration::EnumRenderer_RenderStitchesAs::type);
    void setRenderBackstitchesAs(Configuration::EnumRenderer_RenderBackstitchesAs::type);
    void setRenderKnotsAs(Configuration::EnumRenderer_RenderKnotsAs::type);
    void setShowGrid(bool);
    void setShowStitches(bool);
    void setShowBackstitches(bool);
    void setShowKnots(bool);

    friend class PatternElementDlg;

protected:
    virtual QDataStream &streamOut(QDataStream &) const;
    virtual QDataStream &streamIn(QDataStream &);

private:
    static const int version = 102; // added m_showBorder, m_borderColor, m_borderThickness

    bool    m_showBorder;
    QColor  m_borderColor;
    int     m_borderThickness;

    QRect   m_patternRect;
    bool    m_showScales;
    bool    m_showPlan;

    Configuration::EnumEditor_FormatScalesAs::type      m_formatScalesAs;

    Configuration::EnumRenderer_RenderStitchesAs::type      m_renderStitchesAs;
    Configuration::EnumRenderer_RenderBackstitchesAs::type  m_renderBackstitchesAs;
    Configuration::EnumRenderer_RenderKnotsAs::type         m_renderKnotsAs;

    bool        m_showGrid;
    bool        m_showStitches;
    bool        m_showBackstitches;
    bool        m_showKnots;

//    Renderer    m_renderer;
    PlanElement *m_planElement;
};


class ImageElement : public PatternElement
{
public:
    ImageElement(Page *, const QRect &, Element::Type type = Element::Image);
    ImageElement(const ImageElement &);
    virtual ~ImageElement();

    virtual ImageElement *clone() const;
    virtual void render(Document *, QPainter *painter) const;

    friend class ImageElementDlg;

protected:
    virtual QDataStream &streamOut(QDataStream &) const;
    virtual QDataStream &streamIn(QDataStream &);

private:
    static const int version = 100;
};


class TextElement : public Element
{
public:
    TextElement(Page *, const QRect &, Element::Type type = Element::Text);
    TextElement(const TextElement &);
    virtual ~TextElement();

    bool showBorder() const;
    QColor borderColor() const;
    int borderThickness() const;
    bool fillBackground() const;
    QColor backgroundColor() const;
    int backgroundTransparency() const;
    QMargins margins() const;
    QFont textFont() const;
    QColor textColor() const;
    Qt::Alignment alignment() const;
    QString text() const;

    void setShowBorder(bool);
    void setBorderColor(const QColor &);
    void setBorderThickness(int);
    void setFillBackground(bool);
    void setBackgroundColor(const QColor &);
    void setBackgroundTransparency(int);
    void setMargins(const QMargins &);
    void setTextFont(const QFont &);
    void setTextColor(const QColor &);
    void setAlignment(Qt::Alignment);
    void setText(const QString &);

    virtual TextElement *clone() const;
    virtual void render(Document *, QPainter *painter) const;

    friend class TextElementDlg;

protected:
    virtual QDataStream &streamOut(QDataStream &) const;
    virtual QDataStream &streamIn(QDataStream &);

private:
    QString convertedText(Document *) const;
    QString encodeToHtml(const QString &) const;

    static const int version = 100;

    bool        m_showBorder;
    QColor      m_borderColor;
    int         m_borderThickness;
    bool        m_fillBackground;
    QColor      m_backgroundColor;
    int         m_backgroundTransparency;
    QMargins    m_margins;
    QFont       m_textFont;
    QColor      m_textColor;
    Qt::Alignment   m_alignment;
    QString     m_text;
};


#endif // Element_H
