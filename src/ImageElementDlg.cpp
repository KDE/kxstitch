/*
 * Copyright (C) 2013 by Stephen Allewell
 * stephen@mirramar.adsl24.co.uk
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#include "ImageElementDlg.h"

#include <QList>

#include <KDebug>

#include "Document.h"
#include "Element.h"
#include "SelectArea.h"


ImageElementDlg::ImageElementDlg(QWidget *parent, ImageElement *imageElement, Document *document)
    :   KDialog(parent),
        m_imageElement(imageElement),
        m_document(document)
{
    setCaption(i18n("Image Element Properties"));
    setButtons(KDialog::Ok | KDialog::Cancel | KDialog::Help);
    setHelp("ImageElement");

    QWidget *widget = new QWidget(this);
    ui.setupUi(widget);

    m_selectArea = new SelectArea(ui.PreviewFrame, imageElement, document, QList<QRect>());
    ui.PreviewFrame->setWidget(m_selectArea);

    m_selectArea->setPatternRect(imageElement->patternRect());

    QMetaObject::connectSlotsByName(this);

    setMainWidget(widget);
}


ImageElementDlg::~ImageElementDlg()
{
}


void ImageElementDlg::slotButtonClicked(int button)
{
    if (button == KDialog::Ok) {
        m_imageElement->setPatternRect(m_selectArea->patternRect());
        accept();
    } else {
        KDialog::slotButtonClicked(button);
    }
}
