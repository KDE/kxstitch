/********************************************************************************
 *	Copyright (C) 2010 by Stephen Allewell					*
 *	stephen@mirramar.adsl24.co.uk						*
 *										*
 *	This program is free software; you can redistribute it and/or modify	*
 *	it under the terms of the GNU General Public License as published by	*
 *	the Free Software Foundation; either version 2 of the License, or	*
 *	(at your option) any later version.					*
 ********************************************************************************/


#ifndef Element_H
#define Element_H


#include <QMargins>
#include <QRect>

#include "configuration.h"


class QPainter;

class Document;
class Page;
class Renderer;


class Element
{
	public:
		enum Type {Text, Pattern, Plan, Key};

		Element(Page *, Element::Type, const QRect &);
		Element(const Element &);
		virtual ~Element();

		virtual Element *clone() const = 0;
		virtual void render(Document *, QPainter *, double) const = 0;

		Page *parent() const;
		Element::Type type() const;
		const QRect &rectangle() const;
		bool isVisible() const;

		void setParent(Page *);
		void setRectangle(const QRect &);
		void setVisible(bool);
		void move(const QPoint &);

		friend QDataStream &operator<<(QDataStream &, const Element &);
		friend QDataStream &operator>>(QDataStream &, Element &);

	protected:
		virtual QDataStream &streamOut(QDataStream &) const = 0;
		virtual QDataStream &streamIn(QDataStream &) = 0;

		static const int version = 100;

		Page		*m_parent;
		Element::Type	m_type;
		QRect		m_rectangle; // area to be used on the page, in mm, top, left, width, height
		bool		m_visible;
};


class KeyElement : public Element
{
	public:
		KeyElement(Page *, const QRect &);
		KeyElement(const KeyElement &);
		virtual ~KeyElement();

		virtual KeyElement *clone() const;
		virtual void render(Document *, QPainter *painter, double) const;

		friend class KeyElementDlg;

	protected:
		virtual QDataStream &streamOut(QDataStream &) const;
		virtual QDataStream &streamIn(QDataStream &);

	private:
		static const int version = 100;

		bool		m_showBorder;
		QColor		m_borderColor;
		int		m_borderThickness;
		bool		m_fillBackground;
		QColor		m_backgroundColor;
		int		m_backgroundTransparency;
		QMargins	m_margins;
		QColor		m_textColor;
		QFont		m_textFont;
		bool		m_symbolColumn;
		bool		m_flossNameColumn;
		bool		m_strandsColumn;
		bool		m_flossDescriptionColumn;
		bool		m_stitchesColumn;
		bool		m_stitchBreakdownColumn;
		bool		m_lengthColumn;
		bool		m_skeinsColumn;
		bool		m_totalStitches;
};


class PlanElement : public Element
{
	public:
		PlanElement(Page *, const QRect &);
		PlanElement(const PlanElement &);
		virtual ~PlanElement();

		QRect patternRect() const;
		void setPatternRect(const QRect &);

		virtual PlanElement *clone() const;
		virtual void render(Document *, QPainter *painter, double) const;

	protected:
		virtual QDataStream &streamOut(QDataStream &) const;
		virtual QDataStream &streamIn(QDataStream &);

	private:
		static const int version = 100;

		QRect	m_patternRect;
};


class PatternElement : public Element
{
	public:
		PatternElement(Page *, const QRect &);
		PatternElement(const PatternElement &);
		virtual ~PatternElement();

		virtual PatternElement *clone() const;
		virtual void render(Document *, QPainter *painter, double) const;

		QRect patternRect() const;
		bool showScales() const;
		bool showPlan() const;
		Configuration::EnumRenderer_RenderStitchesAs::type renderStitchesAs() const;
		Configuration::EnumRenderer_RenderBackstitchesAs::type renderBackstitchesAs() const;
		Configuration::EnumRenderer_RenderKnotsAs::type renderKnotsAs() const;
		bool showGrid() const;
		bool showStitches() const;
		bool showBackstitches() const;
		bool showKnots() const;

		void setPatternRect(const QRect &);
		void setShowScales(bool);
		void setShowPlan(bool);
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
		static const int version = 101;

		QRect	m_patternRect;
		bool	m_showScales;
		bool	m_showPlan;

		Configuration::EnumEditor_FormatScalesAs::type		m_formatScalesAs;

		Configuration::EnumRenderer_RenderStitchesAs::type	m_renderStitchesAs;
		Configuration::EnumRenderer_RenderBackstitchesAs::type	m_renderBackstitchesAs;
		Configuration::EnumRenderer_RenderKnotsAs::type		m_renderKnotsAs;

		bool		m_showGrid;
		bool		m_showStitches;
		bool		m_showBackstitches;
		bool		m_showKnots;

		Renderer	*m_renderer;
		PlanElement	*m_planElement;
};


class TextElement : public Element
{
	public:
		TextElement(Page *, const QRect &);
		TextElement(const TextElement &);
		virtual ~TextElement();

		virtual TextElement *clone() const;
		virtual void render(Document *, QPainter *painter, double) const;

		friend class TextElementDlg;

	protected:
		virtual QDataStream &streamOut(QDataStream &) const;
		virtual QDataStream &streamIn(QDataStream &);

	private:
		QString convertedText(Document *) const;

		static const int version = 100;

		bool		m_showBorder;
		QColor		m_borderColor;
		int		m_borderThickness;
		bool		m_fillBackground;
		QColor		m_backgroundColor;
		int		m_backgroundTransparency;
		QMargins	m_margins;
		QFont		m_textFont;
		QColor		m_textColor;
		Qt::Alignment	m_alignment;
		QString		m_text;
};


#endif // Element_H
