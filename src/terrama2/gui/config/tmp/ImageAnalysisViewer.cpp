/*
  Copyright (C) 2007 National Institute For Space Research (INPE) - Brazil.

  This file is part of TerraMA2 - a free and open source computational
  platform for analysis, monitoring, and alert of geo-environmental extremes.

  TerraMA2 is free software: you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License as published by
  the Free Software Foundation, either version 3 of the License,
  or (at your option) any later version.

  TerraMA2 is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public License
  along with TerraMA2. See LICENSE. If not, write to
  TerraMA2 Team at <terrama2-team@dpi.inpe.br>.
*/

/*!
  \file terrama2/gui/config/ImageAnalysisViewer.cpp

  \brief Definition of Class ImageAnalysisViewer.hpp

  \author Evandro Delatin
  \author Raphael Willian da Costa  
*/

// TerraMA2
#include "ImageAnalysisViewer.hpp"

// QT  
#include <QPainter>

ImageAnalysisViewer::ImageAnalysisViewer(QWidget* parent)
: QWidget(parent)
{
	_imageAnalysis = QImage();
}

ImageAnalysisViewer::~ImageAnalysisViewer()
{
}

void ImageAnalysisViewer::setImageAnalysis(QString pathImageAnalysis)
{
	if (pathImageAnalysis == QString(""))
		_imageAnalysis = QImage();
	else
		_imageAnalysis.load(pathImageAnalysis);

	update();
}

void ImageAnalysisViewer::paintEvent(QPaintEvent * )
{
	QPainter painter;
	painter.begin(this);
	painter.fillRect(rect(),Qt::white);
	
	if (!_imageAnalysis.isNull())
		painter.drawImage(0,0,_imageAnalysis.scaled(this->size()));

	painter.end();
}
