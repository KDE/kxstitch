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
 * Implement the Symbol class.
 */

/**
 * @page symbol Symbol
 * The Symbol encapsulates the QPainterPath with an attribute @ref Symbol::filled() defining if the path is drawn filled
 * or not. If the path is drawn as an outline, the @ref Symbol::lineWidth() attribute is used to initialize the pen for
 * drawing the path. The end cap style and join style can be set which are added to the pen when drawing the path.
 */

#include "Symbol.h"

#include <QBrush>
#include <QDataStream>
#include <QPen>
#include <QTransform>

#include <KLocalizedString>

#include "Exceptions.h"

/**
 * Constructor
 *
 * The Symbol is initialized with filling enabled and a default pen width of 0.01 with a Qt::SquareCap end and a
 * Qt::MiterJoin join type. The pen width is scaled with the painter.
 */
Symbol::Symbol()
    : m_filled(true)
    , m_lineWidth(0.01)
    , m_capStyle(Qt::SquareCap)
    , m_joinStyle(Qt::MiterJoin)
{
}

/**
 * Get the QPainterPath for the symbol. The path also incorporates the path fill mode.
 *
 * @return a QPainterPath
 */
QPainterPath Symbol::path() const
{
    return m_paths.value(Stitch::Full);
}

/**
 * Get a version of the symbol based on the stitch type to be rendered.
 * The scaled versions are generated and cached within the symbol as required.
 *
 * @param type a Stitch::Type identifying the type of stitch.
 *
 * @return a scaled QPainterPath
 */
QPainterPath Symbol::path(Stitch::Type type)
{
    if (!m_paths.contains(type)) {
        double twoThirds = 2.0 / 3.0;
        double oneThird = 1.0 / 3.0;
        QTransform transform;

        switch (type) {
        case Stitch::Full:
            // nothing else to do
            break;

        case Stitch::TLQtr:
        case Stitch::TLSmallHalf:
        case Stitch::TLSmallFull:
            transform = QTransform::fromScale(0.5, 0.5);
            break;

        case Stitch::TRQtr:
        case Stitch::TRSmallHalf:
        case Stitch::TRSmallFull:
            transform = QTransform::fromScale(0.5, 0.5) * QTransform::fromTranslate(0.5, 0.0);
            break;

        case Stitch::BLQtr:
        case Stitch::BLSmallHalf:
        case Stitch::BLSmallFull:
            transform = QTransform::fromScale(0.5, 0.5) * QTransform::fromTranslate(0.0, 0.5);
            break;

        case Stitch::BRQtr:
        case Stitch::BRSmallHalf:
        case Stitch::BRSmallFull:
            transform = QTransform::fromScale(0.5, 0.5) * QTransform::fromTranslate(0.5, 0.5);
            break;

        case Stitch::TBHalf:
        case Stitch::BTHalf:
        case Stitch::FrenchKnot:
            transform = QTransform::fromScale(twoThirds, twoThirds) * QTransform::fromTranslate(oneThird / 2.0, oneThird / 2.0);
            break;

        case Stitch::TL3Qtr:
            transform = QTransform::fromScale(twoThirds, twoThirds);
            break;

        case Stitch::TR3Qtr:
            transform = QTransform::fromScale(twoThirds, twoThirds) * QTransform::fromTranslate(oneThird, 0.0);
            break;

        case Stitch::BL3Qtr:
            transform = QTransform::fromScale(twoThirds, twoThirds) * QTransform::fromTranslate(0.0, oneThird);
            break;

        case Stitch::BR3Qtr:
            transform = QTransform::fromScale(twoThirds, twoThirds) * QTransform::fromTranslate(oneThird, oneThird);
            break;

        case Stitch::Delete:
            break;
        }

        m_paths.insert(type, transform.map(m_paths.value(Stitch::Full)));
    }

    return m_paths.value(type);
}

/**
 * Get the path filled status.
 *
 * @return @c true if the path is to be drawn filled, @c false if unfilled
 */
bool Symbol::filled() const
{
    return m_filled;
}

/**
 * Get the outline path line width.
 *
 * @return a qreal representing the line width, this is scaled with the painter
 */
qreal Symbol::lineWidth() const
{
    return m_lineWidth;
}

/**
 * Get the pen cap style, see the QPen documentation for details on the styles.
 *
 * @return a Qt::PenCapStyle value
 */
Qt::PenCapStyle Symbol::capStyle() const
{
    return m_capStyle;
}

/**
 * Get the pen join style, see the QPen documentation for details on the styles.
 *
 * @return a Qt::PenJoinStyle value
 */
Qt::PenJoinStyle Symbol::joinStyle() const
{
    return m_joinStyle;
}

/**
 * Set the QPainterPath for the symbol.
 *
 * @param path a const reference to a QPainterPath
 */
void Symbol::setPath(const QPainterPath &path)
{
    m_paths.clear();
    m_paths.insert(Stitch::Full, path);
}

/**
 * Set the filled status of the path.
 *
 * @param filled @c true if the path is to be drawn filled, @c false if unfilled
 */
void Symbol::setFilled(bool filled)
{
    m_filled = filled;
}

/**
 * Set the line width for the outline paths.
 *
 * @param width the width of the pen
 */
void Symbol::setLineWidth(qreal width)
{
    m_lineWidth = width;
}

/**
 * Set the pen cap style, see the QPen documentation for details on the styles.
 *
 * @param capStyle a Qt::PenCapStyle value
 */
void Symbol::setCapStyle(Qt::PenCapStyle capStyle)
{
    m_capStyle = capStyle;
}

/**
 * Set the pen join style, see the QPen documentation for details on the styles.
 *
 * @param joinStyle a Qt::PenJoinStyle value
 */
void Symbol::setJoinStyle(Qt::PenJoinStyle joinStyle)
{
    m_joinStyle = joinStyle;
}

/**
 * Get a pen based on the parameters of the symbol.
 *
 * @return a black QPen with the symbols width and end styles
 */
QPen Symbol::pen() const
{
    QPen p;

    if (!m_filled) {
        p.setWidthF(m_lineWidth);
        p.setCapStyle(m_capStyle);
        p.setJoinStyle(m_joinStyle);
    } else {
        p = QPen(Qt::NoPen);
    }

    return p;
}

/**
 * Get a brush based on the parameters of the symbol.
 *
 * @return a black brush based on the filled state of the symbol
 */
QBrush Symbol::brush() const
{
    return QBrush(m_filled ? Qt::SolidPattern : Qt::NoBrush);
}

/**
 * Stream out a Symbol.
 *
 * @param stream a reference to the QDataStream to write to
 * @param symbol a const reference to the Symbol to stream
 *
 * @return a reference to the stream
 */
QDataStream &operator<<(QDataStream &stream, const Symbol &symbol)
{
    stream << symbol.version << symbol.m_paths.value(Stitch::Full) << symbol.m_filled << symbol.m_lineWidth << static_cast<qint32>(symbol.m_capStyle)
           << static_cast<qint32>(symbol.m_joinStyle);

    if (stream.status() != QDataStream::Ok) {
        throw FailedWriteFile(stream.status());
    }

    return stream;
}

/**
 * Stream in a Symbol.
 *
 * @param stream a reference to the QDataStream to read from
 * @param symbol a reference to the Symbol to stream
 *
 * @return a reference to the stream
 */
QDataStream &operator>>(QDataStream &stream, Symbol &symbol)
{
    qint32 version;
    qint32 capStyle;
    qint32 joinStyle;
    QPainterPath path;
    stream >> version;

    switch (version) {
    case 100:
        stream >> path >> symbol.m_filled >> symbol.m_lineWidth >> capStyle >> joinStyle;
        symbol.m_capStyle = static_cast<Qt::PenCapStyle>(capStyle);
        symbol.m_joinStyle = static_cast<Qt::PenJoinStyle>(joinStyle);
        symbol.m_paths.insert(Stitch::Full, path);
        break;

    default:
        throw InvalidFileVersion(QString(i18n("Symbol version %1", version)));
        break;
    }

    return stream;
}
