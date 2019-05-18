/*
 * Copyright (C) 2010-2015 by Stephen Allewell
 * steve.allewell@gmail.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#include "LibraryManagerDlg.h"

#include <QClipboard>
#include <QDir>
#include <QFileInfoList>
#include <QHelpEvent>
#include <QInputDialog>
#include <QMimeData>
#include <QStandardPaths>
#include <QToolTip>

#include <KHelpClient>
#include <KLocalizedString>
#include <KMessageBox>

#include "LibraryFilePathsDlg.h"
#include "LibraryListWidgetItem.h"
#include "LibraryPatternPropertiesDlg.h"
#include "LibraryTreeWidgetItem.h"
#include "Pattern.h"

#include "configuration.h"


LibraryManagerDlg::LibraryManagerDlg(QWidget *parent)
    :   QDialog(parent)
{
    setWindowTitle(i18n("Library Manager"));

    ui.setupUi(this);

    refreshLibraries();

    ui.LibraryTree->setContextMenuPolicy(Qt::CustomContextMenu);
    ui.LibraryIcons->setContextMenuPolicy(Qt::CustomContextMenu);
    ui.LibraryIcons->changeIconSize(Configuration::icon_DefaultSize());
    ui.IconSizeSlider->setValue(Configuration::icon_DefaultSize());
}


LibraryTreeWidgetItem *LibraryManagerDlg::currentLibrary()
{
    return static_cast<LibraryTreeWidgetItem *>(ui.LibraryTree->currentItem());
}


bool LibraryManagerDlg::event(QEvent *event)
{
    if (event->type() == QEvent::ToolTip) {
        QHelpEvent *helpEvent = static_cast<QHelpEvent *>(event);
        QString tip;

        if (ui.LibraryTree->topLevelItemCount() == 0) {
            tip = i18n("The Library Manager can be used to store\nreusable patterns for insertion into\nnew patterns.\n\nThere are no library categories defined.\nClick the Help button for information on creating\nand populating libraries.");
        } else {
            if (ui.LibraryTree->currentItem() == nullptr) {
                tip = i18n("Select a library to show the associated patterns.");
            } else if (ui.LibraryIcons->count() == 0) {
                tip = i18n("There are no patterns defined\nfor this library.\n\nClick the Help button for information\non creating and populating libraries.");
            }
        }

        if (!tip.isEmpty()) {
            QToolTip::showText(helpEvent->globalPos(), tip);
        } else {
            QToolTip::hideText();
        }

        return true;
    }

    return QWidget::event(event);
}


void LibraryManagerDlg::hideEvent(QHideEvent *event)
{
    KConfigGroup(KSharedConfig::openConfig(), QStringLiteral("DialogSizes")).writeEntry(QStringLiteral("LibraryManagerDlg"), size());

    QDialog::hideEvent(event);
}


void LibraryManagerDlg::showEvent(QShowEvent *event)
{
    QDialog::showEvent(event);

    if (KConfigGroup(KSharedConfig::openConfig(), QStringLiteral("DialogSizes")).hasKey(QStringLiteral("LibraryManagerDlg"))) {
        resize(KConfigGroup(KSharedConfig::openConfig(), QStringLiteral("DialogSizes")).readEntry(QStringLiteral("LibraryManagerDlg"), QSize()));
    }
}


void LibraryManagerDlg::setCellSize(double cellWidth, double cellHeight)
{
    ui.LibraryIcons->setCellSize(cellWidth, cellHeight);
}


void LibraryManagerDlg::on_LibraryTree_customContextMenuRequested(const QPoint &position)
{
    m_contextMenu.clear();
    m_contextMenu.addAction(i18n("New Category"), this, &LibraryManagerDlg::newCategory);

    if ((m_contextTreeItem = static_cast<LibraryTreeWidgetItem *>(ui.LibraryTree->itemAt(position)))) {
        m_contextMenu.addAction(i18n("Add to Export List"), this, &LibraryManagerDlg::addLibraryToExportList);
        m_contextMenu.addAction(i18n("Properties..."), this, &LibraryManagerDlg::libraryProperties);

        if (QApplication::clipboard()->mimeData()->hasFormat(QStringLiteral("application/kxstitch"))) {
            m_contextMenu.addAction(i18n("Paste"), this, &LibraryManagerDlg::pasteFromClipboard);
        }
    }

    m_contextMenu.popup(QCursor::pos());
}


void LibraryManagerDlg::on_LibraryIcons_customContextMenuRequested(const QPoint &position)
{
    m_contextMenu.clear();

    if ((m_contextListItem = static_cast<LibraryListWidgetItem *>(ui.LibraryIcons->itemAt(position)))) {
        m_contextMenu.addAction(i18n("Properties..."), this, &LibraryManagerDlg::patternProperties);
//        m_contextMenu.addAction(i18n("Add to Export List"), this, SLOT(addPatternToExportList()));
//        m_contextMenu.addAction(i18n("Copy"), this, SLOT(copyToClipboard()));
        m_contextMenu.addAction(i18n("Delete"), this, &LibraryManagerDlg::deletePattern);
        m_contextMenu.popup(QCursor::pos());
    } else {
        if (QApplication::clipboard()->mimeData()->hasFormat(QStringLiteral("application/kxstitch")) && ui.LibraryTree->selectedItems().count() == 1) {
            m_contextMenu.addAction(i18n("Paste"), this, &LibraryManagerDlg::pasteFromClipboard);
            m_contextMenu.popup(QCursor::pos());
        }
    }
}


void LibraryManagerDlg::on_LibraryTree_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *)
{
    ui.LibraryIcons->clear();
    LibraryTreeWidgetItem *libraryTreeWidgetItem = dynamic_cast<LibraryTreeWidgetItem *>(current);

    for (LibraryPattern *libraryPattern = libraryTreeWidgetItem->first() ; libraryPattern ; libraryPattern = libraryTreeWidgetItem->next()) {
        LibraryListWidgetItem *libraryListWidgetItem = new LibraryListWidgetItem(ui.LibraryIcons, libraryPattern);
        libraryPattern->setLibraryListWidgetItem(libraryListWidgetItem);
    }
}


void LibraryManagerDlg::on_IconSizeSlider_valueChanged(int size)
{
    ui.LibraryIcons->changeIconSize(size);
}


void LibraryManagerDlg::on_DialogButtonBox_rejected()
{
    reject();
}


void LibraryManagerDlg::on_DialogButtonBox_helpRequested()
{
    KHelpClient::invokeHelp(QStringLiteral("PatternLibraryDialog"), QStringLiteral("kxstitch"));
}


void LibraryManagerDlg::newCategory()
{
    QString category;

    bool ok = false;
    bool exists = false;

    category = QInputDialog::getText(this, i18n("Create Category"), i18n("Category Name"), QLineEdit::Normal, QString(), &ok);

    if (!ok) return; // user cancelled

    if (m_contextTreeItem) {
        for (int i = 0 ; i < m_contextTreeItem->childCount() ; ++i) {
            if (m_contextTreeItem->child(i)->text(0) == category) {
                exists = true;
            }
        }
    } else {
        QList<QTreeWidgetItem *> rootItems = ui.LibraryTree->findItems(category, Qt::MatchExactly);

        if (!rootItems.isEmpty()) {
            exists = true;
        }
    }

    if (exists) {
        KMessageBox::sorry(this, i18n("This category already exists."), i18n("Category Exists"));
        return;
    }

    LibraryTreeWidgetItem *newItem;

    if (m_contextTreeItem) {
        newItem = new LibraryTreeWidgetItem(m_contextTreeItem, category);
    } else {
        newItem = new LibraryTreeWidgetItem(ui.LibraryTree, category);
    }

    QString path;
    QFileInfo fileInfo;

    if (m_contextTreeItem) {
        fileInfo.setFile(m_contextTreeItem->path());
        path = m_contextTreeItem->path();

        if (!fileInfo.isWritable()) {
            path.remove(0, path.indexOf(QLatin1String("/library")));
            path.prepend(QStandardPaths::writableLocation(QStandardPaths::DataLocation));
        }
    } else {
        path = QString::fromLatin1("%1/%2").arg(QStandardPaths::writableLocation(QStandardPaths::DataLocation)).arg(QLatin1String("library"));
    }

    path = QString::fromLatin1("%1/%2").arg(path, category);

    if (QDir().mkpath(path)) {
        newItem->addPath(path);
    }

    ui.LibraryTree->setCurrentItem(newItem);
}


void LibraryManagerDlg::addLibraryToExportList()
{
}


void LibraryManagerDlg::libraryProperties()
{
    QPointer<LibraryFilePathsDlg> dialog = new LibraryFilePathsDlg(this, m_contextTreeItem->text(0), m_contextTreeItem->paths());
    dialog->exec();
    delete dialog;
}


void LibraryManagerDlg::pasteFromClipboard()
{
    LibraryTreeWidgetItem *item = static_cast<LibraryTreeWidgetItem *>(ui.LibraryTree->currentItem());
    Pattern *pattern = new Pattern();
    QByteArray data = QApplication::clipboard()->mimeData()->data(QStringLiteral("application/kxstitch"));
    QDataStream stream(&data, QIODevice::ReadOnly);
    stream >> *pattern;
    item->addPattern(new LibraryPattern(pattern));
    on_LibraryTree_currentItemChanged(static_cast<QTreeWidgetItem *>(item), nullptr);
}


void LibraryManagerDlg::patternProperties()
{
    LibraryPattern *libraryPattern = m_contextListItem->libraryPattern();
    QPointer<LibraryPatternPropertiesDlg> dialog = new LibraryPatternPropertiesDlg(this,
            libraryPattern->key(),
            libraryPattern->modifiers(),
            libraryPattern->baseline(),
            libraryPattern->pattern()->palette().schemeName(),
            libraryPattern->pattern()->stitches().width(),
            libraryPattern->pattern()->stitches().height(),
            m_contextListItem->icon());

    if (dialog->exec()) {
        libraryPattern->setKeyModifiers(dialog->key(), dialog->modifiers());
        libraryPattern->setBaseline(dialog->baseline());
    }

    delete dialog;
}


void LibraryManagerDlg::addPatternToExportList()
{
}


void LibraryManagerDlg::copyToClipboard()
{
}


void LibraryManagerDlg::deletePattern()
{
    if (KMessageBox::warningYesNo(nullptr, i18n("Delete this pattern.")) == KMessageBox::Yes) {
        static_cast<LibraryTreeWidgetItem *>(ui.LibraryTree->currentItem())->deletePattern(m_contextListItem->libraryPattern());
        delete m_contextListItem;
    }
}


void LibraryManagerDlg::refreshLibraries()
{
    ui.LibraryTree->clear();
    QStringList libraryDirectories = QStandardPaths::locateAll(QStandardPaths::DataLocation, QStringLiteral("library"), QStandardPaths::LocateDirectory);
    QStringListIterator libraryDirectoriesIterator(libraryDirectories);

    while (libraryDirectoriesIterator.hasNext()) {
        recurseLibraryDirectory(nullptr, libraryDirectoriesIterator.next());
    }
}


void LibraryManagerDlg::recurseLibraryDirectory(LibraryTreeWidgetItem *parent, const QString &path)
{
    QDir directory(path);
    const QFileInfoList directoryEntries = directory.entryInfoList();
    QListIterator<QFileInfo> fileInfoListIterator(directoryEntries);

    while (fileInfoListIterator.hasNext()) {
        QFileInfo fileInfo = fileInfoListIterator.next();

        if (fileInfo.isDir()) {
            if (fileInfo.fileName() != QLatin1String(".") && fileInfo.fileName() != QLatin1String("..")) {
                LibraryTreeWidgetItem *libraryTreeWidgetItem = nullptr;
                QString subPath = QString::fromLatin1("%1/%2").arg(path, fileInfo.fileName());

                if (parent) {
                    int children = parent->childCount();
                    int childIndex = 0;

                    while (childIndex < children) {
                        libraryTreeWidgetItem = dynamic_cast<LibraryTreeWidgetItem *>(parent->child(childIndex));

                        if (libraryTreeWidgetItem->text(0) == fileInfo.fileName()) {
                            break;
                        } else {
                            libraryTreeWidgetItem = nullptr;
                        }

                        childIndex++;
                    }
                } else {
                    QList<QTreeWidgetItem *> rootNodes = ui.LibraryTree->findItems(fileInfo.fileName(), Qt::MatchExactly, 0);

                    if (!rootNodes.isEmpty()) {
                        libraryTreeWidgetItem = dynamic_cast<LibraryTreeWidgetItem *>(rootNodes[0]);
                    }
                }

                if (libraryTreeWidgetItem == nullptr) {
                    if (parent) {
                        libraryTreeWidgetItem = new LibraryTreeWidgetItem(parent, fileInfo.fileName());
                    } else {
                        libraryTreeWidgetItem = new LibraryTreeWidgetItem(ui.LibraryTree, fileInfo.fileName());
                        ui.LibraryTree->sortItems(0, Qt::AscendingOrder);
                    }
                }

                libraryTreeWidgetItem->addPath(subPath);
                recurseLibraryDirectory(libraryTreeWidgetItem, subPath);
            }
        }
    }
}
