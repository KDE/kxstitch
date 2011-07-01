/********************************************************************************
 *	Copyright (C) 2010 by Stephen Allewell					*
 *	stephen@mirramar.adsl24.co.uk						*
 *										*
 *	This program is free software; you can redistribute it and/or modify	*
 *	it under the terms of the GNU General Public License as published by	*
 *	the Free Software Foundation; either version 2 of the License, or	*
 *	(at your option) any later version.					*
 ********************************************************************************/


#include <QApplication>
#include <QPainter>
#include <QProgressDialog>

#include "configuration.h"
#include "FlossScheme.h"
#include "ImportImageDlg.h"
#include "SchemeManager.h"


ImportImageDlg::ImportImageDlg(QWidget *parent, const Magick::Image &originalImage)
	:	KDialog(parent),
		m_alphaSelect(0),
		m_originalImage(originalImage)
{
	setButtons(KDialog::Ok | KDialog::Cancel | KDialog::Help | KDialog::Reset);
	QWidget *widget = new QWidget(this);
	ui.setupUi(widget);

	m_useFractionals = ui.UseFractionals->isChecked();
	m_ignoreColor = ui.IgnoreColor->isChecked();

	m_originalSize = QSize(m_originalImage.columns(), m_originalImage.rows());
	QString caption = QString("Import Image - Image Size %1 x %2 pixels").arg(m_originalSize.width()).arg(m_originalSize.height());
	setCaption(caption);

	double horizontalClothCount = Configuration::editor_HorizontalClothCount();
	double verticalClothCount = Configuration::editor_VerticalClothCount();
	Configuration::EnumEditor_ClothCountUnits::type clothCountUnits = Configuration::editor_ClothCountUnits();
	ui.HorizontalClothCount->setSuffix((clothCountUnits == Configuration::EnumEditor_ClothCountUnits::CM)?"/cm":"/in");
	ui.VerticalClothCount->setSuffix((clothCountUnits == Configuration::EnumEditor_ClothCountUnits::CM)?"/cm":"/in");
	ui.HorizontalClothCount->setSingleStep((clothCountUnits == Configuration::EnumEditor_ClothCountUnits::CM)?0.01:1.0);
	ui.VerticalClothCount->setSingleStep((clothCountUnits == Configuration::EnumEditor_ClothCountUnits::CM)?0.01:1.0);
	ui.HorizontalClothCount->setValue(horizontalClothCount);
	ui.VerticalClothCount->setValue(verticalClothCount);
	ui.VerticalClothCount->setEnabled(false);
	ui.ClothCountLink->setChecked(Configuration::editor_ClothCountLink());
	ui.ClothCountLink->setIcon(KIcon("link"));

	m_preferredSize = QSize(Configuration::document_Width(), Configuration::document_Height());
	int scaledWidth = m_preferredSize.width()*100/m_originalSize.width();
	int scaledHeight = m_preferredSize.height()*100/m_originalSize.height();
	int scale = std::min(scaledWidth, scaledHeight);

	ui.FlossScheme->addItems(SchemeManager::schemes());
	ui.FlossScheme->setCurrentItem(Configuration::palette_DefaultScheme());
	ui.PatternScale->setValue(scale);
	ui.UseMaximumColors->setChecked(Configuration::import_UseMaximumColors());
	ui.MaximumColors->setValue(Configuration::import_MaximumColors());

	QMetaObject::connectSlotsByName(this);

	createImageMap();
	renderPixmap();

	setMainWidget(widget);
}


ImportImageDlg::~ImportImageDlg()
{
}


Magick::Image ImportImageDlg::convertedImage() const
{
	return m_convertedImage;
}


bool ImportImageDlg::ignoreColor() const
{
	return m_ignoreColor;
}


Magick::Color ImportImageDlg::ignoreColorValue() const
{
	return m_ignoreColorValue;
}


QString ImportImageDlg::flossScheme() const
{
	return ui.FlossScheme->currentText();
}


double ImportImageDlg::horizontalClothCount() const
{
	return ui.HorizontalClothCount->value();
}


double ImportImageDlg::verticalClothCount() const
{
	return ui.VerticalClothCount->value();
}


bool ImportImageDlg::useFractionals() const
{
	return m_useFractionals;
}


void ImportImageDlg::slotButtonClicked(int button)
{
	if (button == KDialog::Ok)
	{
		accept();
	}
	else if (button == KDialog::Reset)
	{
		// reset the form data to the defalts
		m_convertedImage = m_originalImage;
		renderPixmap();
	}
	else
		KDialog::slotButtonClicked(button);
}


void ImportImageDlg::on_FlossScheme_currentIndexChanged(const QString &selectedScheme)
{
	createImageMap();
	renderPixmap();
}


void ImportImageDlg::on_UseMaximumColors_toggled(bool checked)
{
	m_useMaximumColors = checked;
}


void ImportImageDlg::on_MaximumColors_valueChanged(int)
{
	killTimer(m_timer);
	m_timer = startTimer(500);
}


void ImportImageDlg::on_IgnoreColor_toggled(bool checked)
{
	m_ignoreColor = checked;
	delete m_alphaSelect;
	m_alphaSelect = 0;
}


void ImportImageDlg::on_ColorButton_clicked(bool)
{
	m_alphaSelect = new AlphaSelect(ui.ImagePreview);
	connect(m_alphaSelect, SIGNAL(clicked(const QPoint &)), this, SLOT(selectColor(const QPoint &)));
	m_alphaSelect->show();
}


void ImportImageDlg::on_HorizontalClothCount_valueChanged(double horizontalClothCount)
{
	double preferredSizeWidth = m_preferredSize.width();
	double preferredSizeHeight = m_preferredSize.height();
	QString suffix;
	int units = Configuration::document_UnitsFormat();
	if (units != Configuration::EnumDocument_UnitsFormat::Stitches)
	{
		preferredSizeWidth /= horizontalClothCount;
		preferredSizeHeight /= ui.VerticalClothCount->value();
		suffix = ui.HorizontalClothCount->suffix().right(2);
	}
	if (ui.ClothCountLink->isChecked())
		ui.VerticalClothCount->setValue(ui.HorizontalClothCount->value());
	ui.FinalSize->setText(QString(i18n("%1%2 x %3%4")).arg(preferredSizeWidth, 0, 'g', 3).arg(suffix).arg(preferredSizeHeight, 0, 'g', 3).arg(suffix));
}


void ImportImageDlg::on_VerticalClothCount_valueChanged(double verticalClothCount)
{
	double preferredSizeWidth = m_preferredSize.width();
	double preferredSizeHeight = m_preferredSize.height();
	QString suffix;
	int units = Configuration::document_UnitsFormat();
	if (units != Configuration::EnumDocument_UnitsFormat::Stitches)
	{
		preferredSizeWidth /= ui.HorizontalClothCount->value();
		preferredSizeHeight /= verticalClothCount;
		suffix = ui.HorizontalClothCount->suffix().right(2);
	}
	ui.FinalSize->setText(QString(i18n("%1%2 x %3%4")).arg(preferredSizeWidth, 0, 'g', 3).arg(suffix).arg(preferredSizeHeight, 0, 'g', 3).arg(suffix));
}


void ImportImageDlg::on_ClothCountLink_clicked(bool checked)
{
	ui.ClothCountLink->setIcon((checked)?KIcon("link"):KIcon("link_break"));
	if (checked)
	{
		ui.VerticalClothCount->setValue(ui.HorizontalClothCount->value());
		ui.VerticalClothCount->setEnabled(false);
	}
	else
		ui.VerticalClothCount->setEnabled(true);
}


void ImportImageDlg::on_PatternScale_valueChanged(int patternScale)
{
	killTimer(m_timer);
	m_timer = startTimer(500);
}


void ImportImageDlg::on_UseFractionals_toggled(bool checked)
{
	m_useFractionals = checked;
	killTimer(m_timer);
	m_timer = startTimer(500);
}


void ImportImageDlg::calculateSizes()
{
	m_convertedImage = m_originalImage;
	m_preferredSize = m_originalSize*ui.PatternScale->value()/100;
	QSize imageSize = m_preferredSize;
	if (m_useFractionals)
	{
		imageSize *= 2;
	}
	Magick::Geometry geometry(imageSize.width(), imageSize.height());
	geometry.percent(false);
	m_convertedImage.sample(geometry);
	on_HorizontalClothCount_valueChanged(ui.HorizontalClothCount->value());
}


void ImportImageDlg::createImageMap()
{
	FlossScheme *scheme = SchemeManager::scheme(ui.FlossScheme->currentText());
	m_colorMap = *(scheme->createImageMap());
}


void ImportImageDlg::renderPixmap()
{
	QPixmap alpha = KIcon("alpha").pixmap(32, 32);
	ui.ImagePreview->setCursor(Qt::WaitCursor);
	calculateSizes();
	m_convertedImage.modifyImage();
	m_pixmap = QPixmap(m_convertedImage.columns(), m_convertedImage.rows());
	m_pixmap.fill();
	if (ui.UseMaximumColors->isChecked())
	{
		m_convertedImage.quantizeColorSpace(Magick::RGBColorspace);
		m_convertedImage.quantizeColors(ui.MaximumColors->value());
		m_convertedImage.quantize();
	}
	m_convertedImage.map(m_colorMap);
	m_convertedImage.modifyImage();

	QPainter painter;
	painter.begin(&m_pixmap);
	painter.drawTiledPixmap(m_pixmap.rect(), alpha);
	int width = m_convertedImage.columns();
	int height = m_convertedImage.rows();
	int pixelCount = width*height;

	QProgressDialog progress(i18n("Rendering preview"), i18n("Cancel"), 0, pixelCount, this);
	progress.setWindowModality(Qt::WindowModal);

	Magick::Pixels cache(m_convertedImage);
	const Magick::PixelPacket *pixels = cache.getConst(0, 0, width, height);
	for (int dy = 0 ; dy < height ; dy++)
	{
		QApplication::processEvents();
		progress.setValue(dy*width);
		if (progress.wasCanceled()) break;
		for (int dx = 0 ; dx < width ; dx++)
		{
			Magick::PixelPacket packet = *pixels++;
			if (!(packet.opacity))
			{
				if (!(m_ignoreColor && packet == m_ignoreColorValue))
				{
					QColor color(packet.red/257, packet.green/257, packet.blue/257);
					painter.setPen(QPen(color));
					painter.drawPoint(dx, dy);
				}
			}
		}
	}
	painter.end();
	ui.ImagePreview->setPixmap(m_pixmap);
	ui.ImagePreview->setCursor(Qt::ArrowCursor);
}


void ImportImageDlg::timerEvent(QTimerEvent *event)
{
	killTimer(m_timer);;
	renderPixmap();
}


void ImportImageDlg::pickColor()
{
	if (m_ignoreColor)
	{
		m_alphaSelect = new AlphaSelect(ui.ImagePreview);
		connect(m_alphaSelect, SIGNAL(clicked(QPoint)), this, SLOT(selectColor(const QPoint &)));
		m_alphaSelect->show();
	}
	else
	{
		delete m_alphaSelect;
		m_alphaSelect = 0;
	}
}


void ImportImageDlg::selectColor(const QPoint &p)
{
	delete m_alphaSelect;
	m_alphaSelect = 0;
	QPixmap swatch(ui.ColorButton->size());
	int x = p.x() - ((ui.ImagePreview->width()-m_pixmap.width())/2);
	int y = p.y() - ((ui.ImagePreview->height()-m_pixmap.height())/2);
	m_ignoreColorValue = m_convertedImage.pixelColor(x, y);
	swatch.fill(QColor(m_ignoreColorValue.redQuantum()/257, m_ignoreColorValue.greenQuantum()/257, m_ignoreColorValue.blueQuantum()/257));
	ui.ColorButton->setIcon(swatch);
	renderPixmap();
}