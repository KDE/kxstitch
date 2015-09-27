/*
 * Copyright (C) 2009-2015 by Stephen Allewell
 * steve.allewell@gmail.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#ifndef LibraryTreeWidget_H
#define LibraryTreeWidget_H


#include <QTreeWidget>


class QDragEnterEvent;
class QDragLeaveEvent;
class QDragMoveEvent;
class QDropEvent;
class QTimer;
class QTreeWidgetItem;
class QWidget;


class LibraryTreeWidget : public QTreeWidget
{
    Q_OBJECT

public:
    explicit LibraryTreeWidget(QWidget *parent);
    virtual ~LibraryTreeWidget();

protected:
    virtual void dragEnterEvent(QDragEnterEvent *event);
    virtual void dragMoveEvent(QDragMoveEvent *event);
    virtual void dragLeaveEvent(QDragLeaveEvent *event);
    virtual void dropEvent(QDropEvent *event);
    virtual bool acceptDrag(QDropEvent *event) const;

private slots:
    void openBranch();

private:
    QTimer          *m_openBranchTimer;
    QTreeWidgetItem *m_dropItem;
    QTreeWidgetItem *m_currentItem;
};


#endif // LibraryTreeWidget_H
