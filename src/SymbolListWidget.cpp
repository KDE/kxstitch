/*
 * Copyright (C) 2012-2014 by Stephen Allewell
 * steve.allewell@gmail.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


/**
 * @file
 * Implement the SymbolListWidget class
 */


/**
 * @page library_list_widget SymbolListWidget
 * This class implements an extension to the QListWidget class that provides population of the widget
 * with the contents of a SymbolLibrary. For each Symbol in the library a QListWidgetItem is created
 * with a data item representing the Symbol identifier in the library and an icon at a given size that
 * is generated from the Symbol path.
 *
 * The widget is intended to be used in a dialog or main window and allows selection of a symbol to be
 * used for some purpose in the application.
 *
 * @image html ui-main-library.png "The user interface showing the library tab"
 */


#include "SymbolListWidget.h"

#include <QBitmap>
#include <QPainter>
#include <QPen>

#include <KLocalizedString>

#include "Stitch.h"
#include "Symbol.h"
#include "SymbolLibrary.h"


/**
 * Constructor.
 */
SymbolListWidget::SymbolListWidget(QWidget *parent)
    :   QListWidget(parent),
        m_library(0),
        m_lastIndex(0)
{
    setResizeMode(QListView::Adjust);
    setViewMode(QListView::IconMode);
    setIconSize(24);
}


/**
 * Destructor.
 */
SymbolListWidget::~SymbolListWidget()
{
}


/**
 * Set the size of the icons to be used.
 * The base QListWidget has the icon size and grid size set to this value.
 *
 * @param size the size in pixels
 */
void SymbolListWidget::setIconSize(int size)
{
    m_size = size;
    QListWidget::setIconSize(QSize(m_size, m_size));
    setGridSize(QSize(m_size, m_size));
}


/**
 * Populate the QListWidget with the QListWidgetItems for each Symbol in the SymbolLibrary.
 * An icon is created for each Symbol.
 *
 * @param library a pointer to the SymbolLibrary containing the Symbols
 */
void SymbolListWidget::loadFromLibrary(SymbolLibrary *library)
{
    if (!library) {
        return;
    }

    m_library = library;
    QList<qint16> keys = library->indexes();

    foreach (qint16 index, keys) {
        addSymbol(index, library->symbol(index));
    }
}


/**
 * Add an individual Symbol to the view.
 *
 * @param index the index of the Symbol
 * @param symbol a const reference to the Symbol to add
 * @return a pointer to a QListWidgetItem
 */
QListWidgetItem *SymbolListWidget::addSymbol(qint16 index, const Symbol &symbol)
{
    QListWidgetItem *item = createItem(index);
    item->setIcon(createIcon(symbol, m_size));

    return item;
}


/**
 * Enable the item so that it can be selected clearing the tooltip.
 *
 * @param item a pointer to the QListWidgetItem that is to be enabled
 */
void SymbolListWidget::enableItem(QListWidgetItem *item)
{
    item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
    item->setToolTip(QString());
}


/**
 * Disable the item so that it can not be selected adding a tooltip to
 * show the user why.
 *
 * @param item a pointer to the QListWidgetItem that is to be disabled
 */
void SymbolListWidget::disableItem(QListWidgetItem *item)
{
    item->setFlags(Qt::NoItemFlags);
    item->setToolTip(QString(i18nc("A symbol that has been used", "Used")));
}


/**
 * Remove a symbol item from the view.
 *
 * @param index the index of the item to remove
 */
void SymbolListWidget::removeSymbol(qint16 index)
{
    if (m_items.contains(index)) {
        delete m_items.take(index);
    }
}


/**
 * Set an item as the currently selected one.
 *
 * @param index the index of the item to be current
 */
void SymbolListWidget::setCurrent(qint16 index)
{
    if (m_items.contains(index)) {
        m_items.value(index)->setSelected(true);
    }
}


/**
 * If an item for the index currently exists return it otherwise create
 * an item to be inserted into the QListWidget.
 * The item created has a data entry added representing the index.
 * The items are inserted so that the Symbols are sorted by their index.
 *
 * @param index an index in the SymbolLibrary
 *
 * @return a pointer to the QListWidgetItem created
 */
QListWidgetItem *SymbolListWidget::createItem(qint16 index)
{
    if (m_items.contains(index)) {
        return m_items.value(index);
    }

    QListWidgetItem *item = new QListWidgetItem;
    item->setData(Qt::UserRole, index);
    m_items.insert(index, item);
    int i = index;

    while (++i < m_lastIndex) {
        if (m_items.contains(i)) {
            break;
        }
    }

    if (i >= m_lastIndex) {
        addItem(item);
        m_lastIndex = index;
    } else {
        insertItem(row(m_items[i]), item);
    }

    return item;
}


/**
 * Create a QIcon for the supplied Symbol.
 *
 * @param symbol a const reference to a Symbol
 * @param size a size for the icon
 *
 * @return a QIcon
 */
QIcon SymbolListWidget::createIcon(const Symbol &symbol, int size)
{
    QPixmap icon(size, size);
    icon.fill(Qt::white);

    QPainter painter(&icon);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setWindow(0, 0, 1, 1);

    QBrush brush(symbol.filled() ? Qt::SolidPattern : Qt::NoBrush);
    QPen pen(Qt::black);

    if (!symbol.filled()) {
        pen.setWidthF(symbol.lineWidth());
        pen.setCapStyle(symbol.capStyle());
        pen.setJoinStyle(symbol.joinStyle());
    }

    painter.setBrush(brush);
    painter.setPen(pen);
    painter.drawPath(symbol.path());
    painter.end();

    icon.setMask(icon.createMaskFromColor(Qt::white));

    return QIcon(icon);
}
