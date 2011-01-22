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
#include "configuration.h"
#include "document.h"
#include "flossscheme.h"
#include "palettemanagerdlg.h"
#include "schememanager.h"


const int FILE_FORMAT_VERSION = 9;


Document::Document()
{
	m_schemeManager = new SchemeManager();

	m_width = Configuration::document_Width();
	m_height = Configuration::document_Height();
	// m_title				defaults to a blank string
	// m_author				defaults to a blank string
	// m_copyright			defaults to a blank string
	// m_fabric				defaults to a blank string
	// m_fabricColor		defaults to white
	// m_instructions		defaults to a blank string
	m_documentNew = true;
	m_documentModified = false;
	setURL(i18n("Untitled"));
	// m_backgroundImages	defaults to an empty list
	// m_undoStack			defaults to an empty stack
	// m_redoStack			defaults to an empty stack
	m_flossSchemeName = Configuration::palette_DefaultScheme();
	// m_palette			defaults to an empty map
	m_currentFlossIndex = -1;
	// m_canvasStitches		defaults to an empty map
	// m_usedFlosses		defaults to an empty map
	// m_canvasKnots		defaults to an empty list
	// m_canvasBackstitches	defaults to an empty list

	// set all the default properties to display a new document
	// these will be updated by configuring or loading a document

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
}


bool Document::loadURL(const KUrl &url)
{
	QString tmpfile;
	bool	validRead = false;

	QString	magic;
	quint16 fileFormatVersion;
	quint32 width;
	quint32 height;
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
									>> m_title
									>> m_author
									>> m_copyright
									>> m_fabric
									>> m_fabricColor
									>> m_instructions
									>> m_properties
									>> m_flossSchemeName;
							if (file.error()) break;
							m_width = width;
							m_height = height;

							stream	>> paletteCount;
							if (file.error()) break;
							flossScheme = m_schemeManager->scheme(m_flossSchemeName);
							while (paletteCount--)
							{
								stream	>> flossKey
										>> flossName
										>> flossSymbol
										>> stitchStrands
										>> backstitchStrands;
								if (file.error()) break;
								Floss *floss = flossScheme->find(flossName);
								struct FLOSS newFloss = {
									floss,
									flossSymbol,
									stitchStrands,
									backstitchStrands
									};
								m_palette[flossKey] = newFloss;
							}

							stream >> currentFlossIndex;
							if (file.error()) break;
							m_currentFlossIndex = currentFlossIndex;

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

						default:
							kDebug() << "Invalid file format encountered.";
							break;
					}

					if (validRead)
					{
						m_documentNew = false;
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


Document::~Document()
{
}


unsigned int Document::width() const
{
	return m_width;
}


unsigned int Document::height() const
{
	return m_height;
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


bool Document::isNew() const
{
	return m_documentNew;
}


bool Document::isModified() const
{
	return m_documentModified;
}


bool Document::saveDocument()
{
	// open the file identified by m_documentURL
	QFile file(m_documentURL.path());
	if (file.open(QIODevice::WriteOnly))
	{
		QDataStream stream(&file);
		stream << QString("KXStitch");
		stream << (quint16)FILE_FORMAT_VERSION;
		stream << (quint32)m_width;
		stream << (quint32)m_height;
		stream << m_title;
		stream << m_author;
		stream << m_copyright;
		stream << m_fabric;
		stream << m_fabricColor;
		stream << m_instructions;
		stream << m_properties;
		stream << m_flossSchemeName;

		stream << (qint32)m_palette.count();
		QMap<int, FLOSS>::const_iterator flossIterator = m_palette.constBegin();
		while (flossIterator != m_palette.constEnd())
		{
			stream	<< (quint32)flossIterator.key()
					<< flossIterator.value().floss->name
					<< flossIterator.value().symbol
					<< (quint8)(flossIterator.value().stitchStrands)
					<< (quint8)(flossIterator.value().backstitchStrands);
			++flossIterator;
		}

		stream << (qint32)m_currentFlossIndex;

		stream << (quint32)m_usedFlosses.count();
		QMap<int, int>::const_iterator usedFlossesIterator = m_usedFlosses.constBegin();
		while (usedFlossesIterator != m_usedFlosses.constEnd())
		{
			stream	<< (quint32)usedFlossesIterator.key()
					<< (quint32)usedFlossesIterator.value();
			++usedFlossesIterator;
		}

		stream << (quint32)m_canvasStitches.count();
		QMap<unsigned int, StitchQueue *>::const_iterator stitchIterator = m_canvasStitches.constBegin();
		while (stitchIterator != m_canvasStitches.constEnd())
		{
			stream << (quint32)stitchIterator.key();
			StitchQueue *stitchQueue = stitchIterator.value();
			stream << (quint8)stitchQueue->count();
			StitchQueue::const_iterator si = stitchQueue->constBegin();
			while (si != stitchQueue->constEnd())
			{
				stream	<< (quint8)((*si)->type())
						<< (quint32)((*si)->floss());
				++si;
			}
			++stitchIterator;
		}

		stream << (quint32)m_canvasBackstitches.count();
		QList<Backstitch *>::const_iterator backstitchIterator = m_canvasBackstitches.constBegin();
		while (backstitchIterator != m_canvasBackstitches.constEnd())
		{
			stream	<< (*backstitchIterator)->start()
					<< (*backstitchIterator)->end()
					<< (quint32)((*backstitchIterator)->floss());
			++backstitchIterator;
		}

		stream << (quint32)m_canvasKnots.count();
		QList<Knot *>::const_iterator knotIterator = m_canvasKnots.constBegin();
		while (knotIterator != m_canvasKnots.constEnd())
		{
			stream	<< (*knotIterator)->position()
					<< (quint32)((*knotIterator)->floss());
			++knotIterator;
		}

		stream << (quint32)m_backgroundImages.count();
		QList<BackgroundImage *>::const_iterator backgroundImageIterator = m_backgroundImages.constBegin();
		while (backgroundImageIterator != m_backgroundImages.constEnd())
		{
			stream	<< (*backgroundImageIterator)->URL()
					<< (*backgroundImageIterator)->location()
					<< (*backgroundImageIterator)->isVisible()
					<< (*backgroundImageIterator)->image()
					<< (*backgroundImageIterator)->icon();
			++backgroundImageIterator;
		}

		file.flush();
		file.close();
		m_documentModified = false;
		return true;
	}
	return false;
}


QVariant Document::property(QString propertyName) const
{
	if (! m_properties.contains(propertyName))
		kDebug() << "Asked for non existant property " << propertyName;
	return m_properties.value(propertyName, QVariant(""));
}


void Document::setProperty(QString propertyName, QVariant propertyValue)
{
	m_properties[propertyName] = propertyValue;
	m_documentModified = true;
	m_documentNew = false;
}


StitchQueue *Document::stitchAt(QPoint cell) const
{
	if (validateCell(cell))
		return m_canvasStitches.value(canvasIndex(cell)); // this will be 0 if no queue exists at x,y
	return 0;
}


Floss *Document::floss(int index) const
{
	return m_palette[index].floss;
}


int Document::currentFlossIndex() const
{
	return m_currentFlossIndex;
}


void Document::setCurrentFlossIndex(int floss)
{
	m_currentFlossIndex = floss;
	m_documentModified = true;
	m_documentNew = false;
}


bool Document::addStitch(Stitch::Type type, QPoint &cell)
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

	bool miniStitch = (type & 192);

	unsigned int stitchCount = stitchQueue->count();

	unsigned int stitches = stitchCount;

	if (!miniStitch)
	{
		// try and merge it with any existing stitches in the queue to update the stitch being added
		while (stitches--)
		{
			Stitch *stitch = stitchQueue->dequeue();
			if (!(stitch->type() & 192)) // so we don't try and merge existing mini stitches
			{
				if (stitch->floss() == m_currentFlossIndex)
				{
					type = (Stitch::Type)(type | stitch->type());
				}
			}
			stitchQueue->enqueue(stitch);
		}
	}

	switch (type) // add the new stitch checking for illegal types
	{
		case 3: // TLQtr and TRQtr
			stitchQueue->enqueue(new Stitch(Stitch::TLQtr, m_currentFlossIndex));
			stitchQueue->enqueue(new Stitch(Stitch::TRQtr, m_currentFlossIndex));
			m_usedFlosses[m_currentFlossIndex] += 2;
			m_documentModified = true;
			m_documentNew = false;
			break;
		case 5: // TLQtr and BLQtr
			stitchQueue->enqueue(new Stitch(Stitch::TLQtr, m_currentFlossIndex));
			stitchQueue->enqueue(new Stitch(Stitch::BLQtr, m_currentFlossIndex));
			m_usedFlosses[m_currentFlossIndex] += 2;
			m_documentModified = true;
			m_documentNew = false;
			break;
		case 10: // TRQtr and BRQtr
			stitchQueue->enqueue(new Stitch(Stitch::TRQtr, m_currentFlossIndex));
			stitchQueue->enqueue(new Stitch(Stitch::BRQtr, m_currentFlossIndex));
			m_usedFlosses[m_currentFlossIndex] += 2;
			m_documentModified = true;
			m_documentNew = false;
			break;
		case 12: // BLQtr and BRQtr
			stitchQueue->enqueue(new Stitch(Stitch::BLQtr, m_currentFlossIndex));
			stitchQueue->enqueue(new Stitch(Stitch::BRQtr, m_currentFlossIndex));
			m_usedFlosses[m_currentFlossIndex] += 2;
			m_documentModified = true;
			m_documentNew = false;
			break;
		default: // other values are acceptable as is including mini stitches
			stitchQueue->enqueue(new Stitch(type, m_currentFlossIndex));
			m_usedFlosses[m_currentFlossIndex]++;
			m_documentModified = true;
			m_documentNew = false;
			break;
	}

	/** iterate the queue of existing stitches for any that have been overwritten by the new stitch */
	while (stitchCount--)                                              // while there are existing stitches
	{
		Stitch *stitch = stitchQueue->dequeue();                         // get the stitch at the head of the queue
		m_usedFlosses[stitch->floss()]--;
		Stitch::Type currentStitchType = (Stitch::Type)(stitch->type());   // and find its type
		int currentFlossIndex = stitch->floss();                           // and color
		Stitch::Type usageMask = (Stitch::Type)(currentStitchType & 15); // and find which parts of a stitch cell are used
		Stitch::Type interferenceMask = (Stitch::Type)(usageMask & type);
		// interferenceMask now contains a mask of which bits are affected by new stitch
		if (interferenceMask)
		{
			// Some parts of the current stitch are being overwritten
			// but a check needs to be made for illegal values
			Stitch::Type changeMask = (Stitch::Type)(usageMask ^ interferenceMask);
			switch (changeMask)
			{
				// changeMask contains what is left of the original stitch after being overwritten
				// it may contain illegal values, so these are checked for
				case 3:
					stitchQueue->enqueue(new Stitch(Stitch::TLQtr, currentFlossIndex));
					stitchQueue->enqueue(new Stitch(Stitch::TRQtr, currentFlossIndex));
					m_usedFlosses[currentFlossIndex] += 2;
					m_documentModified = true;
					m_documentNew = false;
					changeMask = Stitch::Delete;
					break;
				case 5:
					stitchQueue->enqueue(new Stitch(Stitch::TLQtr, currentFlossIndex));
					stitchQueue->enqueue(new Stitch(Stitch::BLQtr, currentFlossIndex));
					m_usedFlosses[currentFlossIndex] += 2;
					m_documentModified = true;
					m_documentNew = false;
					changeMask = Stitch::Delete;
					break;
				case 10:
					stitchQueue->enqueue(new Stitch(Stitch::TRQtr, currentFlossIndex));
					stitchQueue->enqueue(new Stitch(Stitch::BRQtr, currentFlossIndex));
					m_usedFlosses[currentFlossIndex] += 2;
					m_documentModified = true;
					m_documentNew = false;
					changeMask = Stitch::Delete;
					break;
				case 12:
					stitchQueue->enqueue(new Stitch(Stitch::BLQtr, currentFlossIndex));
					stitchQueue->enqueue(new Stitch(Stitch::BRQtr, currentFlossIndex));
					m_usedFlosses[currentFlossIndex] += 2;
					m_documentModified = true;
					m_documentNew = false;
					changeMask = Stitch::Delete;
					break;
				default:
					// other values are acceptable as is
					break;
			}

			if (changeMask) // Check if there is anything left of the original stitch, Stitch::Delete is 0
			{
				stitch->setType(changeMask);           // and change stitch type to the changeMask value
				stitchQueue->enqueue(stitch);        // and then add it back to the queue
				m_usedFlosses[stitch->floss()]++;
				m_documentModified = true;
				m_documentNew = false;
			}
			else // if changeMask is 0, it does not get requeued, effectively deleting it from the pattern
			{
				delete stitch;                     // delete the Stitch as it is no longer required
				m_documentModified = true;
				m_documentNew = false;
			}
		}
		else
		{
			stitchQueue->enqueue(stitch);
			m_usedFlosses[stitch->floss()] += 1;
			m_documentModified = true;
			m_documentNew = false;
		}
	}

	return true;
}


bool Document::deleteStitch(QPoint &cell, Stitch::Type maskStitch, int maskColor)
{
	if (!validateCell(cell))
		return false;			// Cell isn't valid so it can't be deleted

	unsigned int index = canvasIndex(cell);

	StitchQueue *stitchQueue = m_canvasStitches.value(index);
	if (stitchQueue == 0)
		return false;			// No stitch queue exists at the required location so nothing to delete

	if (maskStitch == Stitch::Delete)
	{
		int stitchCount = stitchQueue->count();
		while (stitchCount--)
		{
			Stitch *stitch = stitchQueue->dequeue();
			m_usedFlosses[stitch->floss()]--;
			if (!maskColor || (stitch->floss() == m_currentFlossIndex))
			{
				delete stitch;
				m_documentModified = true;
				m_documentNew = false;
			}
			else
			{
				stitchQueue->enqueue(stitch);
				m_usedFlosses[stitch->floss()]++;
			}
		}
		if (stitchQueue->count() == 0)
		{
			delete stitchQueue;
			m_canvasStitches.remove(index);
			return true;
		}
	}
	else
	{
		int stitchCount = stitchQueue->count();
		while (stitchCount--)
		{
			Stitch *stitch = stitchQueue->dequeue();
			m_usedFlosses[stitch->floss()]--;
			if ((stitch->type() == maskStitch) && (!maskColor || (stitch->floss() == m_currentFlossIndex)))
			{
				// delete any stitches of the required stitch if it is the correct color or if the color doesn't matter
				delete stitch;
				m_documentModified = true;
				m_documentNew = false;
			}
			else
			{
				if ((stitch->type() & maskStitch) && (!maskColor || (stitch->floss() == m_currentFlossIndex)) && ((stitch->type() & 192) == 0))
				{
					// the mask covers a part of the current stitch and is the correct color or if the color doesn't matter
					Stitch::Type changeMask = (Stitch::Type)(stitch->type() ^ maskStitch);
					int flossIndex = stitch->floss();
					delete stitch;
					m_documentModified = true;
					m_documentNew = false;
					switch (changeMask)
					{
						// changeMask contains what is left of the original stitch after deleting the maskStitch
						// it may contain illegal values, so these are checked for
						case 3:
							stitchQueue->enqueue(new Stitch(Stitch::TLQtr, flossIndex));
							stitchQueue->enqueue(new Stitch(Stitch::TRQtr, flossIndex));
							m_usedFlosses[flossIndex] += 2;
							break;
						case 5:
							stitchQueue->enqueue(new Stitch(Stitch::TLQtr, flossIndex));
							stitchQueue->enqueue(new Stitch(Stitch::BLQtr, flossIndex));
							m_usedFlosses[flossIndex] += 2;
							break;
						case 10:
							stitchQueue->enqueue(new Stitch(Stitch::TRQtr, flossIndex));
							stitchQueue->enqueue(new Stitch(Stitch::BRQtr, flossIndex));
							m_usedFlosses[flossIndex] += 2;
							break;
						case 12:
							stitchQueue->enqueue(new Stitch(Stitch::BLQtr, flossIndex));
							stitchQueue->enqueue(new Stitch(Stitch::BRQtr, flossIndex));
							m_usedFlosses[flossIndex] += 2;
							break;
						default:
							stitchQueue->enqueue(new Stitch((Stitch::Type)changeMask, flossIndex));
							m_usedFlosses[flossIndex]++;
							break;
					}
				}
				else
				{
					stitchQueue->enqueue(stitch);
					m_usedFlosses[stitch->floss()]++;
				}
			}
		}
		return true;
	}
	return false;
}


bool Document::addBackstitch(QPoint &start, QPoint &end)
{
	if (validateSnap(start) && validateSnap(end))
	{
		m_canvasBackstitches.append(new Backstitch(start, end, m_currentFlossIndex));
		m_usedFlosses[m_currentFlossIndex]++;
		m_documentModified = true;
		m_documentNew = false;
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
			kDebug() << !maskColor << (m_canvasBackstitches.at(i)->floss() == m_currentFlossIndex);
			if (!maskColor || (m_canvasBackstitches.at(i)->floss() == m_currentFlossIndex))
			{
				m_usedFlosses[m_canvasBackstitches.at(i)->floss()]--;
				m_canvasBackstitches.removeAt(i);
				m_documentModified = true;
				m_documentNew = false;
				deleted = true;
				break;
			}
		}
	}

	return deleted;
}


bool Document::addFrenchKnot(QPoint &snap)
{
	if (validateSnap(snap))
	{
		m_canvasKnots.append(new Knot(snap, m_currentFlossIndex));
		m_usedFlosses[m_currentFlossIndex]++;
		m_documentModified = true;
		m_documentNew = false;
		return true;
	}

	return false;
}


bool Document::deleteFrenchKnot(QPoint &snap, int maskColor)
{
	bool deleted = false;

	for (int i = 0 ; i < m_canvasKnots.count() ; )
	{
		if (m_canvasKnots.at(i)->position() == snap)
		{
			if (!maskColor || (m_canvasKnots.at(i)->floss() == m_currentFlossIndex))
			{
				m_usedFlosses[m_canvasKnots.at(i)->floss()]--;
				m_canvasKnots.removeAt(i);
				m_documentModified = true;
				m_documentNew = false;
				deleted = true;
			}
			else
				i++;
		}
	}

	return deleted;
}


void Document::selectFloss(int flossIndex)
{
	m_currentFlossIndex = flossIndex;
	m_documentModified = true;
	m_documentNew = false;
}


void Document::clearUnusedColors()
{
	QMutableMapIterator<int, FLOSS> flosses(m_palette);
	while (flosses.hasNext())
	{
		flosses.next();
		if (m_usedFlosses[flosses.key()] == 0)
			flosses.remove();
	}
}


QMap<int, Document::FLOSS> &Document::palette()
{
	return m_palette;
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
	PaletteManagerDlg *paletteManagerDlg = new PaletteManagerDlg(m_schemeManager, m_flossSchemeName, m_palette, m_usedFlosses);
	if (paletteManagerDlg->exec())
	{
		added = true;
		m_documentModified = true;
		m_documentNew = false;
	}

	return added;
}


void Document::addBackgroundImage(KUrl url, QRect &rect)
{
	BackgroundImage *backgroundImage = new BackgroundImage(url, rect);
	m_documentModified = true;
	m_documentNew = false;


	m_backgroundImages.append(backgroundImage);
}


void Document::removeBackgroundImage(QString url)
{
	for (int i = 0 ; i < m_backgroundImages.count() ; i++)
	{
		if (m_backgroundImages.at(i)->URL() == url)
		{
			m_backgroundImages.removeAt(i);
			m_documentModified = true;
			m_documentNew = false;
			break;
		}
	}
}


void Document::fitBackgroundImage(QString url, QRect selectionArea)
{
	for (int i = 0 ; i < m_backgroundImages.count() ; i++)
	{
		if (m_backgroundImages.at(i)->URL() == url)
		{
			m_backgroundImages[i]->setLocation(selectionArea);
			m_documentModified = true;
			m_documentNew = false;
			break;
		}
	}
}


void Document::showBackgroundImage(QString url, bool visible)
{
	for (int i = 0 ; i < m_backgroundImages.count() ; i++)
	{
		if (m_backgroundImages.at(i)->URL() == url)
		{
			m_backgroundImages[i]->setVisible(visible);
			m_documentModified = true;
			m_documentNew = false;
			break;
		}
	}
}


unsigned int Document::canvasIndex(QPoint &cell) const
{
	return cell.y()*m_width + cell.x();
}


bool Document::validateCell(QPoint &cell) const
{
	if (cell.x() < 0)
		return false;
	if (cell.y() < 0)
		return false;
	if (m_width <= cell.x())
		return false;
	if (m_height <= cell.y())
		return false;
	return true;
}


bool Document::validateSnap(QPoint &snap) const
{
	if (snap.x() < 0)
		return false;
	if (snap.y() < 0)
		return false;
	if (m_width*2+1 <= snap.x())
		return false;
	if (m_height*2+1 <= snap.y())
		return false;
	return true;
}
