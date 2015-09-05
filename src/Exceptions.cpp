/*
 * Copyright (C) 2011-2015 by Stephen Allewell
 * steve.allewell@gmail.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


/**
 * @file
 * Implement the exception classes.
 */


/**
 * @page exceptions Exceptions
 *
 * @section errors Errors
 * When handling data from various sources there is always the risk of encountering errors during
 * reading or writing that data. To avoid the problem of incomplete data being read or written
 * it is necessary to handle these errors to avoid later corruption of the data. Whilst this can
 * be done in different ways, SymbolEditor uses exceptions.
 *
 * @section exception_use Exception Use
 * Exceptions are used by enclosing operations that might cause errors in a try..catch block which
 * tries the operation and catches any exceptions that are thrown by those operations. This gives
 * the opportunity to do any cleaning up and displaying suitable error messages.
 */


#include "Exceptions.h"

#include <KLocalizedString>


/**
 * Constructor
 */
InvalidFile::InvalidFile()
{
}


/**
 * Destructor
 */
InvalidFile::~InvalidFile()
{
}


/**
 * Constructor
 *
 * @param v the version of the file that was being read
 */
InvalidFileVersion::InvalidFileVersion(const QString &v)
    :   version(v)
{
}


/**
 * Destructor
 */
InvalidFileVersion::~InvalidFileVersion()
{
}


/**
 * Constructor
 *
 * @param s the status of the QDataStream that caused the exception
 */
FailedReadFile::FailedReadFile(QDataStream::Status s)
{
    if (s == QDataStream::ReadPastEnd) {
        status = QString(i18n("Tried to read past the end of the data"));
    } else if (s == QDataStream::ReadCorruptData) {
        status = QString(i18n("Tried to read corrupted data"));
    } else {
        status = QString(i18n("Undefined status message status %1", s));
    }
}


/**
 * Constructor
 *
 * @param s the status message for the event causing the error
 */
FailedReadFile::FailedReadFile(const QString &s)
    :   status(s)
{
}


/**
 * Destructor
 */
FailedReadFile::~FailedReadFile()
{
}


/**
 * Constructor
 *
 * @param status the status of the QDataStream that caused the exception
 */
FailedWriteFile::FailedWriteFile(QDataStream::Status status)
    :   m_status(status)
{
}


/**
 * Destructor
 */
FailedWriteFile::~FailedWriteFile()
{
}


/**
 * Get the status message of the QDataStream::Status
 *
 * @return a QString containing the stream status
 */
QString FailedWriteFile::statusMessage() const
{
#if QT_VERSION >= 0x040800

    if (m_status == QDataStream::WriteFailed) {
        return QString(i18n("Failed to write to the device"));
    }

#endif

    return QString(i18n("Undefined status message status %1", m_status));
}


/**
 * Constructor
 *
 * @param v the version of the Symbol being read
 */
InvalidSymbolVersion::InvalidSymbolVersion(qint32 v)
    :   version(v)
{
}


/**
 * Destructor
 */
InvalidSymbolVersion::~InvalidSymbolVersion()
{
}
