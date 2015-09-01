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
  \file terrama2/gui/config/ImageAnalysisDialog.cpp

  \brief Definition of Class ImageAnalysisDialog.hpp

  \author Evandro Delatin
  \author Raphael Willian da Costa  
*/

// TerraMA2
#include "ImageAnalysisDialog.hpp"

// QT
#include <QDateTime>
#include <QDir>

ImageAnalysisDialog::ImageAnalysisDialog(QString path, QWidget* parent, Qt::WFlags f)
: QDialog(parent, f)
{
	setupUi(this);
	_path = path;
	connect(_analysisListWidget, SIGNAL(currentRowChanged(int)), SLOT(analysisSelected(int)));
}

ImageAnalysisDialog::~ImageAnalysisDialog()
{
}

void ImageAnalysisDialog::setTimestamp(int year, int month, int day, int hour, int minute, int second)
{
	QDateTime timestamp(QDate(year, month, day), QTime(hour, minute, second));
	
	//Atualizar a string que informa a hora em que as analises foram executadas
	_timestampStr = timestamp.toString("dd/MM/yyyy hh:mm:ss");

	//Atualizar _fileTimestamp. Esta informaçao sera utilizada para recuperar os arquivos de imagens gerados nesta hora e data
	_fileTimestamp = timestamp.toString("_yyyyMMddHHmm.jpg");
}

void ImageAnalysisDialog::reloadAnalysisListAndImage()
{
	_analysisListWidget->clear();
	_imageFiles.clear();
	_timestampLabel->setText(_timestampStr);

	//Obter o diretorio em que estao os arquivos de imagem
	QDir dir(_path);

	//Obter todos os arquivos jpg para um determinado timestamp
	QStringList filter;
	filter << "*" + _fileTimestamp;
	QStringList imageFileNameList = dir.entryList(filter, QDir::Files, QDir::Name);

	//Adicionar o nome da analise à lista e guardar o nome do arquivo tambem
	for(int i=0; i<imageFileNameList.size(); ++i)
	{
		QString fileName = imageFileNameList.at(i);
		_imageFiles.push_back(fileName);
		
		QString analysisName = fileName.remove(_fileTimestamp);
		_analysisListWidget->addItem(analysisName);
	}

    //Se no ultimo evento de analise processado existir alguma analise com objeto monitorado,
	//selecionar o primeiro arquivo de imagem. Senao, limpar o desenho (pintar um quadrado branco)
	if(imageFileNameList.size() > 0)
		_analysisListWidget->setCurrentRow(0);
	else
		_imageAnalysisWidget->setImageAnalysis("");
}

void ImageAnalysisDialog::analysisSelected(int currentRow)
{
	if(currentRow >= 0 && currentRow < _imageFiles.size())
	{
		QString imagePath = _path + _imageFiles.at(currentRow);
		_imageAnalysisWidget->setImageAnalysis(imagePath);
	}
}
