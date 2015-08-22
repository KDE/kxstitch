/*
 * Copyright (C) 2013-2014 by Stephen Allewell
 * steve.allewell@gmail.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#include "ImageElementDlg.h"

#include <QList>

#include <KHelpClient>
#include <KLocalizedString>

#include "Document.h"
#include "Element.h"
#include "SelectArea.h"


ImageElementDlg::ImageElementDlg(QWidget *parent, ImageElement *imageElement, Document *document)
    :   QDialog(parent),
        m_imageElement(imageElement),
        m_document(document)
{
    setWindowTitle(i18n("Image Element Properties"));
    QWidget *widget = new QWidget(this);
    ui.setupUi(widget);

    m_selectArea = new SelectArea(ui.PreviewFrame, imageElement, document, QMap<int, QList<QRect> >());
    m_selectArea->setPatternRect(imageElement->patternRect());

    ui.PreviewFrame->setWidget(m_selectArea);
    ui.ShowBorder->setChecked(imageElement->showBorder());
    ui.BorderColor->setColor(imageElement->borderColor());
    ui.BorderThickness->setValue(double(imageElement->borderThickness()) / 10);

    QMetaObject::connectSlotsByName(this);
}


ImageElementDlg::~ImageElementDlg()
{
}


void ImageElementDlg::on_DialogButtonBox_accepted()
{
    m_imageElement->setPatternRect(m_selectArea->patternRect());
    m_imageElement->setShowBorder(ui.ShowBorder->isChecked());
    m_imageElement->setBorderColor(ui.BorderColor->color());
    m_imageElement->setBorderThickness(int(ui.BorderThickness->value() * 10));

    accept();
}


void ImageElementDlg::on_DialogButtonBox_rejected()
{
    reject();
}


void ImageElementDlg::on_DialogButtonBox_helpRequested()
{
    KHelpClient::invokeHelp("ImageElement", "kxstitch");
}
