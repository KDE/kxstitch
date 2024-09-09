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
 * Header file for the exception classes.
 */

#ifndef Exceptions_H
#define Exceptions_H

#include <QDataStream>
#include <QtGlobal>
#include <QString>

/**
 * @brief Invalid file exception class.
 *
 * This is thrown when the file being opened is not a supported cross
 * stitch file type or symbol library.
 */
class InvalidFile
{
public:
    InvalidFile();

private:
};

/**
 * @brief Invalid file version exception class.
 *
 * This is thrown when the file being opened is not a supported version
 * of a cross stitch or library file or one of the elements is an
 * unsupported version.
 */
class InvalidFileVersion
{
public:
    explicit InvalidFileVersion(const QString &v);

    QString version; /**< the version of the file read */
};

/**
 * @brief Failed to read the file exception class.
 *
 * This is thrown when there was an error reading the QDataStream.
 */
class FailedReadFile
{
public:
    explicit FailedReadFile(QDataStream::Status s);
    explicit FailedReadFile(const QString &s);

    QString status; /**< the status of the error */
};

/**
 * @brief Failed to write the file exception class.
 *
 * This is thrown when there was an error writing to the QDataStream.
 */
class FailedWriteFile
{
public:
    explicit FailedWriteFile(QDataStream::Status status);

    QString statusMessage() const;

private:
    QDataStream::Status m_status; /**< the status of the error */
};

/**
 * @brief Found an invalid symbol version.
 *
 * This is thrown when the symbol being read was not a known version.
 */
class InvalidSymbolVersion
{
public:
    explicit InvalidSymbolVersion(qint32 v);

    qint32 version; /** the version of the symbol read */
};

#endif
