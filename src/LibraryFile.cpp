/***************************************************************************
 *   Copyright (C) 2003 by Stephen Allewell                                *
 *   stephen@mirramar.fsnet.co.uk                                          *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/


#include <QDataStream>
#include <QFile>
#include <QFileInfo>

#include <KDebug>
#include <KLocale>
#include <KMessageBox>

#include <stdlib.h>

#include "LibraryFile.h"
#include "LibraryPattern.h"


LibraryFile::LibraryFile(const QString &path)
	:	m_read(false),
		m_path(path)
{
}


LibraryFile::~LibraryFile()
{
	qDeleteAll(m_libraryPatterns);
}


QString LibraryFile::path() const
{
	return m_path;
}


bool LibraryFile::isWritable() const
{
	return QFileInfo(m_path).isWritable();
}


void LibraryFile::addPattern(LibraryPattern *libraryPattern)
{
	m_libraryPatterns.append(libraryPattern);
	writeFile();
}


void LibraryFile::deletePattern(LibraryPattern *libraryPattern)
{
	if (m_libraryPatterns.removeOne(libraryPattern))
	{
		writeFile();
	}
}


LibraryPattern *LibraryFile::first()
{
	if (!m_read)
	{
		readFile();
	}

	m_current = 0;
	if (m_current < m_libraryPatterns.count())
	{
		return m_libraryPatterns.at(m_current++);
	}

	return 0;
}


LibraryPattern *LibraryFile::next()
{
	if (m_current < m_libraryPatterns.count())
	{
		return m_libraryPatterns.at(m_current++);
	}

	return 0;
}


void LibraryFile::readFile()
{
	bool ok = true;
	QFile file(localFile());
	if (file.open(QIODevice::ReadOnly))
	{
		QDataStream stream(&file);
		char header[11];
		stream.readRawData(header, 11);
		if (strncmp(header, "KXStitchLib", 11))
		{
			// probably an old library, try and read the old format
#if 0
			file.reset();
			qint32 patterns;
			stream >> patterns;
			while (patterns-- && ok)
			{
				qint16 checksum;
				QByteArray data;
				QByteArray compressed;
				stream >> checksum;
				stream >> compressed;
				data = qUncompress(compressed);
				if (checksum == qChecksum(data.data(), data.size()))
				{
					m_patterns.append(new LibraryPattern(0, Qt::NoButton , 0, checksum, data));
				}
				else
				{
					KMessageBox::sorry(0, i18n("There was a checksum error in %1.").arg(localFile()), i18n("Checksum Error"));
					ok = false;
				}
			}
#endif
		}
		else
		{
			qint32 version;
			qint32 count;
			LibraryPattern *libraryPattern;
			stream >> version;
			switch (version)
			{
				case 100:
					stream >> count;
					while (count--)
					{
						libraryPattern = new LibraryPattern;
						stream >> *libraryPattern;
					}
					break;

				default:
					// not supported
					// throw exception
					break;
			}
		}
		file.close();
		m_read = true;
	}
	else
	{
		KMessageBox::error(0, QString(i18n("The file %1\ncould not be opened.\n%2")).arg(localFile()).arg(file.errorString()), i18n("Error opening file"));
	}
}


void LibraryFile::writeFile()
{
	QFile file(localFile());
	if (file.open(QIODevice::WriteOnly)) // truncates the file
	{
		QDataStream stream(&file);
		stream.writeRawData("KXStitchLib", 11);
		stream << qint32(version);
		stream << qint32(m_libraryPatterns.count());
		QListIterator<LibraryPattern *> iterator(m_libraryPatterns);
		while (iterator.hasNext())
		{
			stream << *iterator.next();
		}
		file.close();
		m_read = true;
	}
}


QString LibraryFile::localFile() const
{
	QFileInfo path(m_path);
	if (path.isDir())
		return m_path+"kxstitch.library";
	else
		return m_path;
}


bool LibraryFile::hasChanged()
{
	bool changed = false;

	QListIterator<LibraryPattern *> libraryPatternIterator(m_libraryPatterns);
	while (libraryPatternIterator.hasNext())
	{
		changed = changed | libraryPatternIterator.next()->hasChanged();
	}

	return changed;
}
