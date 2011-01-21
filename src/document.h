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
	V9 Document consists of
		KXStitch					QString
		file format version			quint16
		width in stitches			quint32
		height in stitches			quint32
		title						QString
		author						QString
		copyright					QString
		fabric						QString
		fabric colour				QColor
		instructions				QString
		property list				QVariantMap
		scheme name					QString
		floss count					quint32
			floss key				quint32
			floss name				QString
			floss symbol			QChar
			strands / stitch		quint8
			strands / backstitch	quint8
		current floss index			quint32
		used flosses count			quint32
			floss key				quint32
			number of stitches		quint32
		stitches count				quint32
			stitch cell index		quint32
			stitches in the queue	quint8
				stitch type			quint8
				floss index			quint32
		backstitch count			quint32
			start					QPoint
			end						QPoint
			floss key				quint32
		knot count					quint32
			position				QPoint
			floss key				quint32
		background image list	QList
			url						KUrl
			location rectangle		QRect
			visible flag			bool
			image					QImage
			image icon				QIcon
*/


class Document
{
	public:
		Document();
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
		bool loadURL(const KUrl &);
		KUrl URL() const;
		void setURL(const KUrl&);
		bool isNew() const;
		bool isModified() const;
		bool saveDocument();
		QVariant property(QString) const;
		void setProperty(QString, QVariant);
		StitchQueue *stitchAt(QPoint) const;
		Floss *floss(int) const;
		int currentFlossIndex() const;
		void setCurrentFlossIndex(int);

		bool addStitch(Stitch::Type, QPoint &);
		bool deleteStitch(QPoint &, Stitch::Type, int);
		bool addBackstitch(QPoint &, QPoint &);
		bool deleteBackstitch(const QPoint &, const QPoint &, int);
		bool addFrenchKnot(QPoint &);
		bool deleteFrenchKnot(QPoint &, int);

		void selectFloss(int);
		void clearUnusedColors();
		QMap<int, Document::FLOSS> &palette();
		QListIterator<struct BACKGROUND_IMAGE> backgroundImages() const;
		QListIterator<Backstitch *> backstitches() const;
		QListIterator<Knot *> knots() const;
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
		bool					m_documentNew;
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
		QMap<unsigned int, StitchQueue *>	m_canvasStitches;
		QMap<int, int>			m_usedFlosses;
		QList<Knot *>			m_canvasKnots;
		QList<Backstitch *>		m_canvasBackstitches;
};


#endif
