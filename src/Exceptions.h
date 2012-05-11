/********************************************************************************
 * Copyright (C) 2011 by Stephen Allewell                                       *
 * stephen@mirramar.adsl24.co.uk                                                *
 *                                                                              *
 * This program is free software; you can redistribute it and/or modify         *
 * it under the terms of the GNU General Public License as published by         *
 * the Free Software Foundation; either version 2 of the License, or            *
 * (at your option) any later version.                                          *
 ********************************************************************************/


/**
 * @file
 * Header file for the exception classes.
 */


#ifndef Exceptions_H
#define Exceptions_H


#include <QDataStream>
#include <QtGlobal>


/**
 * @brief Invalid file type exception class.
 *
 * This is thrown when the file being opened is not a supported cross stitch file.
 */
class InvalidFileType
{
public:
    InvalidFileType() {};
};


/**
 * @brief Invalid file version exception class.
 *
 * This is thrown when the file opened is a version that is not known.
 */
class InvalidFileVersion
{
public:
    InvalidFileVersion(const QString &v) { version = v; }

    QString version;           /**< the version of the file or element read */
};


/**
 * @brief Failed to read the file exception class.
 *
 * This is thrown when there was an error reading the QDataStream.
 */
class FailedReadFile
{
public:
    FailedReadFile(const QString &m) { message = m; }

    QString message;           /**< message indicating the source of the error */
};


/**
 * @brief Failed to write the file exception class.
 *
 * This is thrown when there was an error writing to the file.
 * The error message should be recovered from the QFile::errorString() function.
 */
class FailedWriteFile
{
public:
    FailedWriteFile() {};
};


#endif
