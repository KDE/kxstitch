/****************************************************************************
 *	Copyright (C) 2010 by Stephen Allewell									*
 *	stephen@mirramar.adsl24.co.uk											*
 *																			*
 *	This program is free software; you can redistribute it and/or modify	*
 *	it under the terms of the GNU General Public License as published by	*
 *	the Free Software Foundation; either version 2 of the License, or		*
 *	(at your option) any later version.										*
 ****************************************************************************/


#ifndef __FILE_PROPERTIES_DLG
#define __FILE_PROPERTIES_DLG


#include <KDialog>

#include "configuration.h"
#include "document.h"
#include "ui_fileproperties.h"


class FilePropertiesDlg : public KDialog
{
	Q_OBJECT

	public:
		FilePropertiesDlg(Document *);
		~FilePropertiesDlg();

	protected slots:
		virtual void slotButtonClicked(int button);

	private slots:
		void on_UnitsFormat_activated(int);
		void on_PatternWidth_valueChanged(double);
		void on_PatternHeight_valueChanged(double);
		void on_HorizontalClothCount_valueChanged(double);
		void on_VerticalClothCount_valueChanged(double);
		void on_ClothCountLink_clicked(bool);
		void updatePatternSizes();

	private:
		Ui::FileProperties ui;

		Document	*m_document;
		int			m_width;
		int			m_height;
		int			m_minWidth;
		int			m_minHeight;
		double		m_horizontalClothCount;
		double		m_verticalClothCount;
		bool		m_clothCountLink;
		Configuration::EnumDocument_UnitsFormat::type	m_unitsFormat;
		Configuration::EnumEditor_ClothCountUnits::type	m_clothCountUnits;
};


#endif
