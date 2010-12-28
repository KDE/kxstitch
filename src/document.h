/****************************************************************************
 *	Copyright (C) 2010 by Stephen Allewell									*
 *	stephen@mirramar.adsl24.co.uk											*
 *																			*
 *	This program is free software; you can redistribute it and/or modify	*
 *	it under the terms of the GNU General Public License as published by	*
 *	the Free Software Foundation; either version 2 of the License, or		*
 *	(at your option) any later version.										*
 ****************************************************************************/


#ifndef DOCUMENT_H
#define DOCUMENT_H


#include <QColor>
#include <QIcon>
#include <QImage>
#include <QList>
#include <QMap>
#include <QRect>
#include <QStack>
#include <QString>
#include <QVector>

#include <KUrl>

#include "stitch.h"


class Floss;
class SchemeManager;


/**
	Document consists of
		pattern properties
			width in stitches		Q_INT32
			height in stitches		Q_INT32
			size display units		enum
			cloth count units		enum
			cloth count size		float
			title					QString
			author					QString
			copyright				QString
			fabric					QString
			fabric colour			QColor
			scheme name				QString
			Instructions			QString ? rich text
			background image list	QList
				filename			QString
				location rectangle	QRect
				visible flag		bool
		print properties
			front page				bool
			instructions			bool
			pattern					bool
			grid squares per inch	Q_INT16
			minimum spill over		Q_INT16
			fit single page			bool
			grid lines				bool
			print stitches as		enum
			print backstitches as	enum
			colour key				bool
			floss usage				bool
			stitch counts			bool
			stitch totals			bool
			seperate backstitching	bool
			combine fractionals		bool
			stitch summary			bool
		palette
			2 dimensional array of stitchqueues
			list of flosses with associated symbols and floss strands for stitches and backstitches
*/


class Document
{
	public:
		Document();
		Document(KUrl &);
		~Document();

		struct FLOSS
		{
			Floss     *floss;
			QChar     symbol;
			unsigned  stitchStrands;
			unsigned  backstitchStrands;
		};

		struct BACKGROUND_IMAGE
		{
			KUrl   imageURL;
			QRect  imageLocation;
			bool   imageVisible;
			QImage image;
			QIcon  imageIcon;
		};

		unsigned  width() const;
		unsigned  height() const;
		bool isModified() const;
		QVariant property(QString) const;
		void setProperty(QString, QVariant);
		Stitch::Queue *stitchAt(QPoint) const;
		Floss *floss(int) const;
		int currentFlossIndex() const;
		void setCurrentFlossIndex(int);

		bool addStitch(Stitch::Type, QPoint &);
		bool addBackstitch(QPoint &, QPoint &);
		bool addFrenchKnot(QPoint &);
		void selectFloss(int);
		QMap<int, Document::FLOSS> &palette();
		QList<struct BACKGROUND_IMAGE> backgroundImages() const;
		bool paletteManager();
		void addBackgroundImage(KUrl, QRect &);
		void removeBackgroundImage(QString);
		void fitBackgroundImage(QString, QRect);
		void showBackgroundImage(QString, bool);

	private:
		unsigned int canvasIndex(QPoint &) const;
		bool validateCell(QPoint &) const;
		bool validateSnap(QPoint &) const;

		// Document properties
		QVariantMap				m_properties;
		unsigned				m_width;
		unsigned				m_height;
		QString					m_title;
		QString					m_author;
		QString					m_copyright;
		QString					m_fabric;
		QColor					m_fabricColor;
		QString					m_instructions;
		bool					m_documentModified;
		KUrl					m_documentURL;
		QList<struct BACKGROUND_IMAGE>	m_backgroundImages;
		QStack<QByteArray>		m_undoStack;
		QStack<QByteArray>		m_redoStack;

		// Palette properties
		SchemeManager			*m_schemeManager;
		QString					m_flossSchemeName;
		QMap<int, FLOSS>		m_palette;
		int						m_currentFlossIndex;

		// Canvas properties
		QMap<unsigned int, Stitch::Queue *>	m_canvasStitches;
		QMap<int, int>			m_usedFlosses;
		QList<Knot *>			m_canvasKnots;
		QList<Backstitch *>		m_canvasBackstitches;
};


#endif
