/*
 * Copyright (C) 2009-2015 by Stephen Allewell
 * steve.allewell@gmail.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#include "LibraryTreeWidget.h"

#include <QDragEnterEvent>
#include <QDragLeaveEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QToolTip>
#include <QTimer>
#include <QTreeWidgetItem>

#include <KLocale>

#include "LibraryPattern.h"
#include "LibraryTreeWidgetItem.h"
#include "Pattern.h"


/* Auto expanding of QTreeWidgetItems doesn't appear to be working.
 * Implement some code to do it via a timer when hovered over.
 */
const int AUTO_EXPAND_DELAY = 500;


LibraryTreeWidget::LibraryTreeWidget(QWidget *parent)
    :   QTreeWidget(parent)
{
    setAcceptDrops(true);
    m_openBranchTimer = new QTimer(this);
    connect(m_openBranchTimer, SIGNAL(timeout()), this, SLOT(openBranch()));
}


LibraryTreeWidget::~LibraryTreeWidget()
{
    delete m_openBranchTimer;
}


void LibraryTreeWidget::dragEnterEvent(QDragEnterEvent *event)
{
    if (acceptDrag(event)) {
        event->accept();
        m_currentItem = currentItem();
        m_dropItem = itemAt(event->pos());

        if (m_dropItem) {
            m_openBranchTimer->start(AUTO_EXPAND_DELAY);
        }
    } else {
        event->ignore();
    }
}


void LibraryTreeWidget::dragMoveEvent(QDragMoveEvent *event)
{
    m_openBranchTimer->stop();

    if (acceptDrag(event)) {
        event->accept();
        m_dropItem = itemAt(event->pos());

        if (m_dropItem && m_dropItem == m_currentItem) { // tring to drop on same Library
            QToolTip::showText(mapToGlobal(event->pos() + QPoint(10, 0)), i18n("existing library"));
        } else {
            QToolTip::hideText();
        }

        if (m_dropItem) {
            setCurrentItem(m_dropItem);
            m_openBranchTimer->start(AUTO_EXPAND_DELAY);
        }
    } else {
        event->ignore();
    }
}


void LibraryTreeWidget::dragLeaveEvent(QDragLeaveEvent*)
{
    m_openBranchTimer->stop();

    if (m_currentItem) {
        setCurrentItem(m_currentItem);
        scrollToItem(m_currentItem);
    }

    m_currentItem = 0;
    m_dropItem = 0;
}


void LibraryTreeWidget::dropEvent(QDropEvent *event)
{
    m_openBranchTimer->stop();

    if (m_dropItem && m_dropItem != m_currentItem) {
        QByteArray data = event->mimeData()->data("application/kxstitch");
        Pattern *pattern = new Pattern;
        QDataStream stream(&data, QIODevice::ReadOnly);
        stream >> *pattern;
        static_cast<LibraryTreeWidgetItem *>(m_dropItem)->addPattern(new LibraryPattern(pattern));
    }

    if (m_currentItem) {
        setCurrentItem(m_currentItem);
        scrollToItem(m_currentItem);
    }
}


bool LibraryTreeWidget::acceptDrag(QDropEvent *event) const
{
    return event->mimeData()->hasFormat("application/kxstitch");
}


void LibraryTreeWidget::openBranch()
{
    m_openBranchTimer->stop();

    if (m_dropItem && !m_dropItem->isExpanded())
    {
        m_dropItem->setExpanded(true);
    }
}
