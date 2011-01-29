/****************************************************************************
 *	Copyright (C) 2010 by Stephen Allewell									*
 *	stephen@mirramar.adsl24.co.uk											*
 *																			*
 *	This program is free software; you can redistribute it and/or modify	*
 *	it under the terms of the GNU General Public License as published by	*
 *	the Free Software Foundation; either version 2 of the License, or		*
 *	(at your option) any later version.										*
 ****************************************************************************/


#include <QFile>
#include <QtGlobal>

#include <KMessageBox>
#include <KIO/NetAccess>

#include "backgroundimage.h"
#include "commands.h"
#include "configuration.h"
#include "document.h"
#include "flossscheme.h"
#include "palettemanagerdlg.h"
#include "schememanager.h"


Document::Document()
{
	m_schemeManager = new SchemeManager();

	initialiseNew();
}


Document::~Document()
{
	qDeleteAll(m_backgroundImages);
	qDeleteAll(m_palette);
	qDeleteAll(m_canvasStitches);
	qDeleteAll(m_canvasKnots);
	qDeleteAll(m_canvasBackstitches);
}


void Document::initialiseNew()
{
	// set all the default properties to display a new document
	// these will be updated by configuring or loading a document

	m_properties["width"] = Configuration::document_Width();
	m_properties["height"] = Configuration::document_Height();
	m_properties["title"] = QString();
	m_properties["author"] = QString();
	m_properties["copyright"] = QString();
	m_properties["fabric"] = QString();
	m_properties["fabricColor"] = QColor();
	m_properties["instructions"] = QString();
	m_properties["flossSchemeName"] = Configuration::palette_DefaultScheme();
	m_properties["currentFlossIndex"] = -1;
	m_properties["cellWidth"] = Configuration::editor_CellWidth();
	m_properties["cellHeight"] = Configuration::editor_CellHeight();
	m_properties["cellHorizontalGrouping"] = Configuration::editor_HorizontalCellGrouping();
	m_properties["cellVerticalGrouping"] = Configuration::editor_VerticalCellGrouping();
	m_properties["horizontalClothCount"] = Configuration::editor_HorizontalClothCount();
	m_properties["verticalClothCount"] = Configuration::editor_VerticalClothCount();
	m_properties["clothCountUnits"] = Configuration::editor_ClothCountUnits();
	m_properties["thickLineWidth"] = Configuration::editor_ThickLineWidth();
	m_properties["thinLineWidth"] = Configuration::editor_ThinLineWidth();
	m_properties["thickLineColor"] = Configuration::editor_ThickLineColor();
	m_properties["thinLineColor"] = Configuration::editor_ThinLineColor();
	m_properties["formatScalesAs"] = Configuration::editor_FormatScalesAs();
	m_properties["showStitchesAs"] = Configuration::editor_ShowStitchesAs();
	m_properties["showBackstitchesAs"] = Configuration::editor_ShowBackstitchesAs();
	m_properties["showKnotsAs"] = Configuration::editor_ShowKnotsAs();
	m_properties["showPaletteSymbols"] = Configuration::palette_ShowSymbols();
	m_properties["paintBackgroundImages"] = true; // Configuration::editor_PaintBackgroundImages();
	m_properties["paintGrid"] = true; // Configuration::editor_PaintGrid();
	m_properties["paintStitches"] = true; // Configuration::editor_PaintStitches();
	m_properties["paintBackstitches"] = true; // Configuration::editor_PaintBackstitches;
	m_properties["paintFrenchKnots"] = true; // Configuration::editor_PaintFrenchKnots;

	// use qDeleteAll to delete the objects pointed to in the containers
	// then clear the containers.
	qDeleteAll(m_backgroundImages);
	m_backgroundImages.clear();

	qDeleteAll(m_palette);
	m_palette.clear();

	qDeleteAll(m_canvasStitches);
	m_canvasStitches.clear();

	m_usedFlosses.clear();	// no requirement for qDeleteAll as m_usedFlosses does not contain pointers.

	qDeleteAll(m_canvasKnots);
	m_canvasKnots.clear();

	qDeleteAll(m_canvasBackstitches);
	m_canvasBackstitches.clear();

	m_undoStack.clear();

	setURL(i18n("Untitled"));
}


bool Document::loadURL(const KUrl &url)
{
	QString tmpfile;
	bool	validRead = false;

	QString	magic;
	quint16 fileFormatVersion;
	quint32 width;
	quint32 height;
	QString title;
	QString author;
	QString copyright;
	QString fabric;
	QColor fabricColor;
	QString instructions;
	QString flossSchemeName;
	quint32 paletteCount;
	quint32 flossKey;
	FlossScheme *flossScheme;
	QString flossName;
	QChar	flossSymbol;
	quint8	stitchStrands;
	quint8	backstitchStrands;
	qint32	currentFlossIndex;
	quint32 usedFlosses;
	quint32 usedFlossesValue;
	quint32	canvasStitches;
	quint32	canvasStitchKey;
	quint8	stitchQueueCount;
	quint8	stitchType;
	quint32 usedFlossKey;
	quint32 usedFlossValue;
	quint32 canvasBackstitches;
	QPoint	start;
	QPoint	end;
	quint32 canvasKnots;
	quint32	backgroundImages;
	KUrl	imageURL;
	QRect	imageLocation;
	bool	imageVisible;
	QImage	image;
	QIcon	imageIcon;

	quint32 count;

	if (!url.isEmpty())
	{
		if (KIO::NetAccess::download(url, tmpfile, 0))
		{
			QFile file(tmpfile);
			if (file.open(QIODevice::ReadOnly))
			{
				QDataStream stream(&file);

				stream >> magic;
				if (file.error()) return false;
				if (magic == "KXStitch")
				{
					stream >> fileFormatVersion;
					if (file.error())
					{
						file.close();
						return false;
					}
					switch (fileFormatVersion)
					{
						case 9:
							stream	>> width
									>> height
									>> title
									>> author
									>> copyright
									>> fabric
									>> fabricColor
									>> instructions
									>> m_properties
									>> flossSchemeName;
							if (file.error()) break;
							m_properties["width"] = width;
							m_properties["height"] = height;
							m_properties["title"] = title;
							m_properties["author"] = author;
							m_properties["copyright"] = copyright;
							m_properties["fabric"] = fabric;
							m_properties["fabricColor"] = fabricColor;
							m_properties["instructions"] = instructions;
							m_properties["flossSchemeName"] = flossSchemeName;

							stream	>> paletteCount;
							if (file.error()) break;
							flossScheme = m_schemeManager->scheme(m_properties["flossSchemeName"].toString());
							while (paletteCount--)
							{
								stream	>> flossKey
										>> flossName
										>> flossSymbol
										>> stitchStrands
										>> backstitchStrands;
								if (file.error()) break;
								Floss *floss = flossScheme->find(flossName);
								DocumentFloss *newFloss = new DocumentFloss(floss, flossSymbol, stitchStrands, backstitchStrands);
								m_palette[flossKey] = newFloss;
							}

							stream >> currentFlossIndex;
							if (file.error()) break;
							m_properties["currentFlossIndex"] = currentFlossIndex;

							stream >> usedFlosses;
							if (file.error()) break;
							while (usedFlosses--)
							{
								stream	>> flossKey
										>> usedFlossesValue;
								if (file.error()) break;
								m_usedFlosses[flossKey] = usedFlossesValue;
							}

							stream >> canvasStitches;
							if (file.error()) break;
							while (canvasStitches--)
							{
								stream	>> canvasStitchKey
										>> stitchQueueCount;
								if (file.error()) break;
								m_canvasStitches[canvasStitchKey] = new StitchQueue;
								while (stitchQueueCount--)
								{
									stream	>> stitchType
											>> flossKey;
									if (file.error()) break;
									m_canvasStitches[canvasStitchKey]->enqueue(new Stitch((Stitch::Type)stitchType, flossKey));
								}
							}

							stream >> canvasBackstitches;
							if (file.error()) break;
							while (canvasBackstitches--)
							{
								stream	>> start
										>> end
										>> flossKey;
								if (file.error()) break;
								m_canvasBackstitches.append(new Backstitch(start, end, flossKey));
							}

							stream >> canvasKnots;
							if (file.error()) return false;
							while (canvasKnots--)
							{
								stream	>> start
										>> flossKey;
								if (file.error()) break;
								m_canvasKnots.append(new Knot(start, flossKey));
							}

							stream >> backgroundImages;
							if (file.error()) break;
							while (backgroundImages--)
							{
								stream	>> imageURL
										>> imageLocation
										>> imageVisible
										>> image
										>> imageIcon;
								if (file.error()) break;
								BackgroundImage *backgroundImage = new BackgroundImage(imageURL, imageLocation, imageVisible, image, imageIcon);
								m_backgroundImages.append(backgroundImage);
							}
							validRead = true;
							break;

						case 10:
							stream >> m_properties;
							if (file.error()) break;

							flossScheme = m_schemeManager->scheme(m_properties["flossSchemeName"].toString());

							stream >> count;
							while (count--)
							{
								stream	>> flossKey
										>> flossName
										>> flossSymbol
										>> stitchStrands
										>> backstitchStrands;
								Floss *floss = flossScheme->find(flossName);
								m_palette[flossKey] = new DocumentFloss(floss, flossSymbol, stitchStrands, backstitchStrands);
							}
							if (file.error()) break;

							// TODO remove saving the floss usage in the next revision of the document, recalculated before use anyway so not necessary to save it.
							stream >> count;
							while (count--)
							{
								stream	>> usedFlossKey
										>> usedFlossValue;
								m_usedFlosses[usedFlossKey] = usedFlossValue;
							}
							if (file.error()) break;

							stream >> count;
							while (count--)
							{
								stream	>> canvasStitchKey
										>> stitchQueueCount;
								m_canvasStitches[canvasStitchKey] = new StitchQueue;
								while (stitchQueueCount--)
								{
									stream	>> stitchType
											>> flossKey;
									m_canvasStitches[canvasStitchKey]->enqueue(new Stitch((Stitch::Type)stitchType, flossKey));
								}
							}
							if (file.error()) break;

							stream >> count;
							while (count--)
							{
								stream	>> start
										>> end
										>> flossKey;
								m_canvasBackstitches.append(new Backstitch(start, end, flossKey));
							}
							if (file.error()) break;

							stream >> count;
							while (count--)
							{
								stream	>> start
										>> flossKey;
								m_canvasKnots.append(new Knot(start, flossKey));
							}
							if (file.error()) break;

							stream >> count;
							while (count--)
							{
								stream	>> imageURL
										>> imageLocation
										>> imageVisible
										>> image
										>> imageIcon;
								m_backgroundImages.append(new BackgroundImage(imageURL, imageLocation, imageVisible, image, imageIcon));
							}
							if (file.error()) break;

							validRead = true;
							break;

						default:
							kDebug() << "Invalid file format encountered.";
							break;
					}

					if (validRead)
					{
						m_undoStack.clear();
						setURL(url);
					}
				}
				else
				{
					KMessageBox::detailedError(0, i18n("This file does not appear to be a KXStitch file."), i18n("Not a KXStitch file."));
				}
				file.close();
			}
		}
	}
	return validRead;
}


unsigned int Document::width() const
{
	return m_properties["width"].toUInt();
}


unsigned int Document::height() const
{
	return m_properties["height"].toUInt();
}


void Document::setURL(const KUrl &url)
{
	if (url.fileName().right(4).toLower() == ".pat")	// this looks like a PCStitch file name
		m_documentURL.setFileName(i18n("Untitled"));	// so that the user doesn't overwrite a PCStitch
														// file with a KXStitch file.
	else
		m_documentURL = url;
}


KUrl Document::URL() const
{
	return m_documentURL;
}


bool Document::isModified() const
{
	return !m_undoStack.isClean();
}


bool Document::saveDocument()
{
	// open the file identified by m_documentURL

	const quint16 FILE_FORMAT_VERSION = 10;

	QFile file(m_documentURL.path());
	if (file.open(QIODevice::WriteOnly))
	{
		QDataStream stream(&file);
		stream << QString("KXStitch");
		stream << FILE_FORMAT_VERSION;
		stream << m_properties;

		stream << (quint32)m_palette.count();
		QMap<int, DocumentFloss *>::const_iterator flossIterator;
 		for (flossIterator = m_palette.constBegin() ; flossIterator != m_palette.constEnd() ; ++flossIterator)
			stream	<< (quint32)flossIterator.key()
					<< flossIterator.value()->floss()->name()
					<< flossIterator.value()->symbol()
					<< (quint8)(flossIterator.value()->stitchStrands())
					<< (quint8)(flossIterator.value()->backstitchStrands());

		// TODO remove saving the floss usage in the next revision of the document, recalculated before use anyway so not necessary to save it.
		stream << (quint32)m_usedFlosses.count();
		QMap<int, int>::const_iterator usedFlossesIterator;
		for (usedFlossesIterator = m_usedFlosses.constBegin() ; usedFlossesIterator != m_usedFlosses.constEnd() ; ++usedFlossesIterator)
			stream	<< (quint32)usedFlossesIterator.key()
					<< (quint32)usedFlossesIterator.value();

		stream << (quint32)m_canvasStitches.count();
		QMap<unsigned int, StitchQueue *>::const_iterator stitchIterator;
		for (stitchIterator = m_canvasStitches.constBegin() ; stitchIterator != m_canvasStitches.constEnd() ; ++stitchIterator)
		{
			stream << (quint32)stitchIterator.key();
			StitchQueue *stitchQueue = stitchIterator.value();
			stream << (quint8)stitchQueue->count();
			StitchQueue::const_iterator si;
			for (si = stitchQueue->constBegin() ; si != stitchQueue->constEnd() ; ++si)
				stream	<< (quint8)((*si)->type())
						<< (quint32)((*si)->floss());
		}

		stream << (quint32)m_canvasBackstitches.count();
		QList<Backstitch *>::const_iterator backstitchIterator;
		for (backstitchIterator = m_canvasBackstitches.constBegin() ; backstitchIterator != m_canvasBackstitches.constEnd() ; ++backstitchIterator)
			stream	<< (*backstitchIterator)->start()
					<< (*backstitchIterator)->end()
					<< (quint32)((*backstitchIterator)->floss());

		stream << (quint32)m_canvasKnots.count();
		QList<Knot *>::const_iterator knotIterator;
		for (knotIterator = m_canvasKnots.constBegin() ; knotIterator != m_canvasKnots.constEnd() ; ++knotIterator)
			stream	<< (*knotIterator)->position()
					<< (quint32)((*knotIterator)->floss());

		stream << (quint32)m_backgroundImages.count();
		QList<BackgroundImage *>::const_iterator backgroundImageIterator;
		for (backgroundImageIterator = m_backgroundImages.constBegin() ; backgroundImageIterator != m_backgroundImages.constEnd() ; ++backgroundImageIterator)
			stream	<< (*backgroundImageIterator)->URL()
					<< (*backgroundImageIterator)->location()
					<< (*backgroundImageIterator)->isVisible()
					<< (*backgroundImageIterator)->image()
					<< (*backgroundImageIterator)->icon();

		file.flush();
		file.close();

		m_undoStack.setClean();

		return true;
	}
	return false;
}


QVariant Document::property(const QString &propertyName) const
{
	if (! m_properties.contains(propertyName))
		kDebug() << "Asked for non existant property " << propertyName;
	return m_properties.value(propertyName, QVariant(""));
}


void Document::setProperty(const QString &propertyName, const QVariant &propertyValue)
{
	m_properties[propertyName] = propertyValue;
}


StitchQueue *Document::stitchAt(const QPoint &cell) const
{
	if (validateCell(cell))
		return m_canvasStitches.value(canvasIndex(cell)); // this will be 0 if no queue exists at x,y
	return 0;
}


void Document::replaceStitchAt(const QPoint &cell, StitchQueue *queue)
{
	m_canvasStitches[canvasIndex(cell)] = queue;
}


const Floss *Document::floss(int index) const
{
	return m_palette[index]->floss();
}


int Document::currentFlossIndex() const
{
	return m_properties["currentFlossIndex"].toInt();;
}


void Document::setCurrentFlossIndex(int floss)
{
	m_properties["currentFlossIndex"] = floss;
}


bool Document::addStitch(const QPoint &cell, Stitch::Type type, int floss)
{
	if (!validateCell(cell))
		return false;

	unsigned int index = canvasIndex(cell);

	StitchQueue *stitchQueue = m_canvasStitches.value(index);
	if (stitchQueue == 0)
	{
		/** no stitch queue currently exists for this cell */
		stitchQueue = new StitchQueue();
		m_canvasStitches[index] = stitchQueue;
	}

	stitchQueue->add(type, floss);

	return true;
}


bool Document::deleteStitch(const QPoint &cell, Stitch::Type maskStitch, int maskColor)
{
	if (!validateCell(cell))
		return false;			// Cell isn't valid so it can't be deleted

	unsigned int index = canvasIndex(cell);

	StitchQueue *stitchQueue = m_canvasStitches.value(index);
	if (stitchQueue == 0)
		return false;			// No stitch queue exists at the required location so nothing to delete

	if (stitchQueue->remove(maskStitch, maskColor) == 0)
	{
		delete stitchQueue;
		m_canvasStitches.remove(index);
	}

	return true;
}


bool Document::addBackstitch(const QPoint &start, const QPoint &end, int floss)
{
	if (validateSnap(start) && validateSnap(end))
	{
		m_canvasBackstitches.append(new Backstitch(start, end, floss));
		return true;
	}

	return false;
}



bool Document::deleteBackstitch(const QPoint &start, const QPoint &end, int maskColor)
{
	bool deleted = false;

	for (int i = 0 ; i < m_canvasBackstitches.count() ; ++i)
	{
		if ((m_canvasBackstitches.at(i)->start() == start) && (m_canvasBackstitches.at(i)->end() == end))
		{
			if ((maskColor == -1) || (m_canvasBackstitches.at(i)->floss() == maskColor))
			{
				m_canvasBackstitches.removeAt(i);
				deleted = true;
				break;
			}
		}
	}

	return deleted;
}


bool Document::addFrenchKnot(const QPoint &snap, int floss)
{
	if (validateSnap(snap))
	{
		m_canvasKnots.append(new Knot(snap, floss));
		return true;
	}

	return false;
}


bool Document::deleteFrenchKnot(const QPoint &snap, int maskColor)
{
	bool deleted = false;

	for (int i = 0 ; i < m_canvasKnots.count() ; )
	{
		if (m_canvasKnots.at(i)->position() == snap)
		{
			if ((maskColor == -1) || (m_canvasKnots.at(i)->floss() == maskColor))
			{
				m_canvasKnots.removeAt(i);
				deleted = true;
			}
			else
				i++;
		}
	}

	return deleted;
}


QUndoStack &Document::undoStack()
{
	return m_undoStack;
}


void Document::clearUnusedColors()
{
	QList<QUndoCommand *> changes;

	updateUsedFlosses();
	QMapIterator<int, DocumentFloss *> flosses(m_palette);
	while (flosses.hasNext())
	{
		flosses.next();
		if (m_usedFlosses[flosses.key()] == 0)
			changes.append(new RemoveFlossCommand(this, flosses.key(), flosses.value()));
	}

	if (!changes.isEmpty())
	{
		m_undoStack.beginMacro(i18n("Clear Unused Flosses"));
		QListIterator<QUndoCommand *> it(changes);
		while (it.hasNext())
		{
			m_undoStack.push(it.next());
		}
		m_undoStack.endMacro();
	}
}


QMap<int, DocumentFloss *> &Document::palette()
{
	return m_palette;
}


void Document::addFloss(int key, DocumentFloss *documentFloss)
{
	m_palette.insert(key, documentFloss);

}


DocumentFloss *Document::removeFloss(int key)
{
	DocumentFloss *documentFloss = m_palette.take(key);
	if (m_palette.isEmpty())
	{
		setCurrentFlossIndex(-1);
	}
	return documentFloss;
}


DocumentFloss *Document::changeFloss(int key, DocumentFloss *documentFloss)
{
	DocumentFloss *original = m_palette.value(key);
	m_palette.insert(key, documentFloss);
	return original;
}


QListIterator<BackgroundImage *> Document::backgroundImages() const
{
	return QListIterator<BackgroundImage*>(m_backgroundImages);
}


QListIterator<Backstitch *> Document::backstitches() const
{
	return QListIterator<Backstitch *>(m_canvasBackstitches);
}


QListIterator<Knot *> Document::knots() const
{
	return QListIterator<Knot *>(m_canvasKnots);
}


bool Document::paletteManager()
{
	bool added = false;
	updateUsedFlosses();
	PaletteManagerDlg *paletteManagerDlg = new PaletteManagerDlg(this, m_schemeManager, m_properties["flossSchemeName"].toString(), m_palette, m_usedFlosses);
	if (paletteManagerDlg->exec())
	{
		added = true;
	}

	return added;
}


void Document::addBackgroundImage(BackgroundImage *backgroundImage)
{
	m_backgroundImages.append(backgroundImage);
}


BackgroundImage *Document::removeBackgroundImage(const QString &url)
{
	BackgroundImage *backgroundImage = 0;
	for (int i = 0 ; i < m_backgroundImages.count() ; i++)
	{
		if (m_backgroundImages.at(i)->URL() == url)
		{
			backgroundImage = m_backgroundImages.takeAt(i);
			break;
		}
	}
	return backgroundImage;
}


BackgroundImage *Document::removeBackgroundImage(BackgroundImage *backgroundImage)
{
	for (int i = 0 ; i < m_backgroundImages.count() ; i++)
	{
		if (m_backgroundImages.at(i) == backgroundImage)
		{
			m_backgroundImages.takeAt(i);
			break;
		}
	}
	return backgroundImage;
}


QRect Document::fitBackgroundImage(const QString &url, const QRect &selectionArea)
{
	QRect original;
	for (int i = 0 ; i < m_backgroundImages.count() ; i++)
	{
		if (m_backgroundImages.at(i)->URL() == url)
		{
			original = m_backgroundImages[i]->location();
			m_backgroundImages[i]->setLocation(selectionArea);
			break;
		}
	}
	return original;
}


bool Document::showBackgroundImage(const QString &url, bool visible)
{
	bool original;
	for (int i = 0 ; i < m_backgroundImages.count() ; i++)
	{
		if (m_backgroundImages.at(i)->URL() == url)
		{
			original = m_backgroundImages[i]->isVisible();
			m_backgroundImages[i]->setVisible(visible);
			break;
		}
	}
	return original;
}


unsigned int Document::canvasIndex(const QPoint &cell) const
{
	return cell.y()*m_properties["width"].toInt() + cell.x();
}


bool Document::validateCell(const QPoint &cell) const
{
	if (cell.x() < 0)
		return false;
	if (cell.y() < 0)
		return false;
	if (m_properties["width"].toInt() <= cell.x())
		return false;
	if (m_properties["height"].toInt() <= cell.y())
		return false;
	return true;
}


bool Document::validateSnap(const QPoint &snap) const
{
	if (snap.x() < 0)
		return false;
	if (snap.y() < 0)
		return false;
	if (m_properties["width"].toInt()*2+1 <= snap.x())
		return false;
	if (m_properties["height"].toInt()*2+1 <= snap.y())
		return false;
	return true;
}


void Document::updateUsedFlosses()
{
	m_usedFlosses.clear();
	QMapIterator<int, int> usedStitchFlosses(StitchQueue::usedFlosses());
	while (usedStitchFlosses.hasNext())
	{
		usedStitchFlosses.next();
		m_usedFlosses.insert(usedStitchFlosses.key(), usedStitchFlosses.value());
	}

	QListIterator<Backstitch *> backstitchIterator(m_canvasBackstitches);
	while (backstitchIterator.hasNext())
	{
		Backstitch *backstitch = backstitchIterator.next();
		m_usedFlosses[backstitch->floss()]+=QPoint(backstitch->start() - backstitch->end()).manhattanLength();
	}

	QListIterator<Knot *> knotIterator(m_canvasKnots);
	while (knotIterator.hasNext())
	{
		Knot *knot = knotIterator.next();
		m_usedFlosses[knot->floss()]+=4; // arbitrary value for a relative amount of floss to do a knot.
	}
}